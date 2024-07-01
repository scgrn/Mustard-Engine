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

#include "fileSystem.h"
#include "log.h"

#include "zlib.h"

#define COMPRESS
#define DECRYPT

namespace AB {

extern FileSystem fileSystem;

static void crypt(u8* data, u64 size, std::string const& key) {
#ifndef DECRYPT
    return;
#endif

    if (key.empty()) return;

    u32 keyIndex = 0;
    for (u64 i = 0; i < size; i++) {
        data[i] ^= (key.c_str()[keyIndex] ^ 0xAA);
        keyIndex++;
        if (keyIndex > key.length()) {
            keyIndex = 0;
        }
    }
}

bool FileSystem::startup() {
    LOG("FileSystem subsystem startup", 0);

    //    load all queued archives
    for (ArchiveFile archiveFile : archiveFiles) {
        loadArchive(archiveFile.path, archiveFile.key);
    }
    
    initialized = true;
    
    return true;
}


void FileSystem::shutdown() {
    LOG("FileSystem subsystem shutdown", 0);
}

void FileSystem::addArchive(std::string const& path, std::string const& key) {
    ArchiveFile archiveFile;
    archiveFile.path = path;
    archiveFile.key = key;
    
    archiveFiles.push_back(archiveFile);

    if (initialized) {
        loadArchive(path, key);
    }
}

void FileSystem::loadArchive(std::string const& path, std::string const& key) {
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG("WARNING: Couldn't load archive %s", path.c_str());
        return;
    }

    ArchiveFile archive;
    archive.path = path;
    archive.key = key;

    //  fourth byte of header is reserved ... for ...
    u8 tag[4];
    fread(&tag, 1, 4, file);
    if (tag[0] != 'A' || tag[1] != 'B') {
        ERR("INVALID ARCHIVE: %s", path.c_str());
    }

    //  check version code. only handle version 1 now. branch for backwards compatibility if extended.
    u8 versionCode = tag[2];
    if (versionCode != 1) {
        //ERR("INVALID ARCHIVE: %s", path.c_str());
    }
/*
    //  read and parse header
    fread(&archive.headerSizeCompressed, 1, 4, file);
    fread(&archive.headerSizeDecompressed, 1, 4, file);
#ifdef COMPRESS
    u8* dataCompressed = new u8[archive.headerSizeCompressed];
    fread(dataCompressed, 1, archive.headerSizeCompressed, file);
    crypt(dataCompressed, archive.headerSizeCompressed, key);
    fclose(file);

    //  decompress header data
    u8* data = new u8[archive.headerSizeDecompressed];
    i32 result = uncompress(data, &archive.headerSizeDecompressed, dataCompressed, archive.headerSizeCompressed);
    //  TODO: check result
#else
    u8* data = new u8[archive.headerSizeDecompressed];
    fread(data, 1, archive.headerSizeDecompressed, file);
    fclose(file);
#endif
    TiXmlDocument *DOM = new TiXmlDocument();
    DOM->Parse((char*)data);
    if (DOM->Error()) {
        delete [] data;
        delete DOM;
        ERR("WARNING: Error processing header: %s", path.c_str());
        return;
    }
    delete [] data;

    TiXmlElement *rootElement = DOM->FirstChildElement("root");
    TiXmlElement *element = rootElement->FirstChildElement("resource");
    while (element) {
        FileResource resource;
        int offset, sizeCompressed, sizeDecompressed;

        resource.path = element->Attribute("path");
        element->Attribute("offset", &offset);
        element->Attribute("sizeCompressed", &sizeCompressed);
        element->Attribute("sizeDecompressed", &sizeDecompressed);

        resource.offset = offset;
        resource.sizeCompressed = sizeCompressed;
        resource.sizeDecompressed = sizeDecompressed;

        // std::cout << "Path: " << resource.path << "\tSize: " << toString(size) << " bytes \tOffset: " << toString(offset) << "\n";

        archive.resources.push_back(resource);
        element = element->NextSiblingElement("resource");
    }
    delete DOM;

    archive.headerSizeCompressed += 12;
    archives.push_back(archive);
*/
}

u8* FileSystem::readFile(std::string const& path, u64 *size) {
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG("ERROR LOADING FILE! %s", path.c_str());
        *size = 0;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* data = new u8[*size];
    fread(data, 1, *size, file);
    fclose(file);

    return data;
}

u8* FileSystem::readFromArchive(std::string const& path, u64 *size) {
    for (ArchiveFile archiveFile : archiveFiles) {
        for (AssetFile assetFile : archiveFile.assets) {
            if (assetFile.path == path) {
                LOG("Loading resource %s from archive %s", assetFile.path.c_str(), archiveFile.path.c_str());

                //  return pointer to archive's DataObject + file offset
                u8* data = nullptr;
                return data;
            }
        }
    }
    size = 0;
    return 0;
}

DataObject::DataObject(const char* path, b8 forceLocal) {
    if (forceLocal) {
        LOG("Loading %s from local filesystem...", path);
        u8* rawData = fileSystem.readFile(path, &size);
        data = std::shared_ptr<u8>(rawData);
        return;
    }

    u8* archiveData = fileSystem.readFromArchive(path, &size);
    if (archiveData == nullptr) {
// #ifdef DEBUG
        LOG("File <%s> not found in archive, loading from local filesystem", path);
        u8* fileData = fileSystem.readFile(("assets/" + std::string(path)).c_str(), &size);
        data = std::shared_ptr<u8>(fileData, std::default_delete<u8[]>());
        return;
// #endif
        std::string filename = path;
        ERR("File not found: %s", filename.c_str());
    } else {
        data = std::shared_ptr<u8>(archiveData, noOpDeleter);
    }
}

DataObject::~DataObject() {
    // delete [] data;
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
