/**

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#include "pch.h"

#include <cstdio>
#include <fstream>
#include <cstring>

#include "zlib.h"

#include "fileSystem.h"
#include "log.h"

#define CHUNK_SIZE 16384

namespace AB {

extern FileSystem fileSystem;

bool FileSystem::loadCompiledScripts;

struct ArrayDeleter {
    void operator()(u8* ptr) const {
        delete[] ptr;
    }
};

static void crypt(u8* data, u64 size, std::string const& key) {
    if (key.empty()) {
         return;
    }

    u32 keyIndex = 0;
    for (u64 i = 0; i < size; i++) {
        data[i] ^= (key.c_str()[keyIndex] ^ 0xAA);
        keyIndex++;
        if (keyIndex >= key.length()) {
            keyIndex = 0;
        }
    }
}

i32 decompress(const u8* inputData, u64 inputSize, u8** outputData, u64 &outputSize) {
    i32 ret;
    u32 have;
    z_stream strm;
    u8 in[CHUNK_SIZE];
    u8 out[CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        return ret;
    }

    std::vector<u8> outputBuffer;
    u64 offset = 0;

    do {
        strm.avail_in = (offset + CHUNK_SIZE > inputSize) ? inputSize - offset : CHUNK_SIZE;
        std::memcpy(in, inputData + offset, strm.avail_in);
        offset += strm.avail_in;

        strm.next_in = in;

        do {
            strm.avail_out = CHUNK_SIZE;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    return ret;
            }
            have = CHUNK_SIZE - strm.avail_out;
            outputBuffer.insert(outputBuffer.end(), out, out + have);
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);

    *outputData = new u8[outputBuffer.size()];
    std::copy(reinterpret_cast<const u8*>(outputBuffer.data()), 
        reinterpret_cast<const u8*>(outputBuffer.data() + outputBuffer.size()),
        *outputData);

    outputSize = outputBuffer.size();
    LOG("OUPUT SIZE: %d", outputSize);

    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void zerr(i32 ret) {
    switch (ret) {
        case Z_ERRNO: ERR("I/O error", 0); break;
        case Z_STREAM_ERROR: ERR("Invalid compression level", 0); break;
        case Z_DATA_ERROR: ERR("Invalid or incomplete deflate data", 0); break;
        case Z_MEM_ERROR: ERR("Out of memory", 0); break;
        case Z_VERSION_ERROR: ERR("zlib version mismatch!", 0); break;
        
        default: break;
    }
}

b8 FileSystem::startup() {
    LOG("FileSystem subsystem startup", 0);

    loadCompiledScripts = false;
    
    //    load all queued archives
    for (auto& archive : archiveFiles) {
        archive.load();
    }

    initialized = true;

    return true;
}

void FileSystem::shutdown() {
    LOG("FileSystem subsystem shutdown", 0);
}

void FileSystem::addArchive(std::string const& path, std::string const& key) {
    ArchiveFile archive;
    archive.path = path;
    archive.key = key;
    
    if (initialized) {
        archive.load();
    }

    archiveFiles.push_back(std::move(archive));
}

const void dump(std::string filename, u64 size, u8* data) {
    std::cout << std::endl << "["<< filename << "] (" << size << " bytes):" << std::endl << std::endl;
    for (u64 i = 0; i < size; i++) {
        std::cout << data[i];
    }
    std::cout << std::endl << std::endl;
}

void FileSystem::ArchiveFile::load() {
    LOG("Loading archive: %s", path.c_str());
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG("WARNING: Couldn't load archive %s", path.c_str());
        return;
    }
    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);
    LOG("File size: %d", size);

    u8 tag[3];
    fread(&tag, 1, 3, file);
    if (tag[0] != 'A' || tag[1] != 'B') {
        ERR("INVALID ARCHIVE: %s", path.c_str());
    }

    //  check version code
    u8 versionCode = tag[2];
    if (versionCode != '2') {
        ERR("INVALID ARCHIVE: %s", path.c_str());
    }
    
    u32 bufferSize = size - 3;
    data = std::make_unique<u8[]>(bufferSize);
    u64 bytesRead = fread(data.get(), 1, bufferSize, file);
    fclose(file);
    if (bytesRead != bufferSize) {
        data.reset();
        ERR("Failed to read file: %s", path.c_str());
    }

    // crypt(buffer, bufferSize, key);

    u8* decompressedData = data.get();
    u64 decompressedSize = bufferSize;
/*
    i32 ret = decompress(buffer, bufferSize, &decompressedData, decompressedSize);
    if (ret != Z_OK) {
        zerr(ret);
    }
    LOG("Decompressed size: %d", decompressedSize);
*/

    //  read manifest
    u32 manifestSize;
    std::memcpy(&manifestSize, decompressedData, sizeof(u32));
    
    std::string manifest(reinterpret_cast<char*>(decompressedData + sizeof(u32)), manifestSize);
    std::stringstream ss(manifest);
    LOG("Manifest size: %d", manifestSize);

    u32 numAssets;
    ss >> numAssets;
    LOG("Num assets: %d", numAssets);
    for (u32 i = 0; i < numAssets; i++) {
        std::string path;
        u64 size, offset;
        ss >> path;
        ss >> size;
        ss >> offset;

        LOG("%s %d %d", path.c_str(), size, offset);

        assets[path] = {size, offset + manifestSize + sizeof(u32)};
    }

#ifdef DEBUG
    FileSystem::loadCompiledScripts = false;
#else
    FileSystem::loadCompiledScripts = true;
#endif

/*
#ifdef DEBUG
    for (const auto& [path, assetFile] : assets) {
        // LOG("%s: size: %db, offset: %d", path.c_str(), assetFile.first, assetFile.second);
        if (path.compare(path.size()-5, 5, ".vert") == 0 ||
            path.compare(path.size()-5, 5, ".frag") == 0 || 
            path.compare(path.size()-4, 4, ".fnt") == 0) {

            dump(path, assetFile.first, data.get() + assetFile.second);
        }
    }
#endif
*/
}

DataObject FileSystem::loadAsset(const std::string& filename, b8 forceLocal) {
    if (forceLocal) {
        return loadAssetFromDisk(filename);
    }

    for (const auto& archive : archiveFiles) {
        auto asset = archive.assets.find(filename);
        if (asset != archive.assets.end()) {
            return loadAssetFromArchive(archive, filename);
        }
    }

#ifdef DEBUG
    //  if not found in any archives, load from disk
    return loadAssetFromDisk("assets/" + filename);
#else
    return DataObject(0);
#endif
}

DataObject FileSystem::loadAssetFromDisk(const std::string& filename) {
    LOG("Loading <%s> from disk...", filename.c_str());

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return DataObject();

    u64 fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    DataObject dataObject(fileSize);
    file.read(reinterpret_cast<char*>(dataObject.getData()), fileSize);
    file.close();

    return dataObject;
}

DataObject FileSystem::loadAssetFromArchive(const ArchiveFile& archive, const std::string& filename) {
    LOG("Loading <%s> from archive...", filename.c_str());

    const auto& [size, offset] = archive.assets.at(filename);
    DataObject dataObject(size);
    std::memcpy(dataObject.getData(), archive.data.get() + offset, size);
    return dataObject;
}

std::string FileSystem::loadData(std::string key) {
    std::string value;

    std::ifstream in(key);
    std::stringstream buffer;
    buffer << in.rdbuf();
    value = buffer.str();

    return value;
}

void FileSystem::saveData(std::string key, std::string value) {
    std::ofstream out(key);
    out << value;
    out.close();
}

}   //  namespace
