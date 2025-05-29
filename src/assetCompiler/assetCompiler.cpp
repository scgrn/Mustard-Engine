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

//  can't use c++17 std::filesystem with mingw because of
//  https://sourceforge.net/p/mingw-w64/bugs/737/
//  which is hogwash of the highest order.

#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cstring>
#include <cassert>

extern "C" {
#include "../vendor/lua-5.3.5/src/lua.h"
#include "../vendor/lua-5.3.5/src/lualib.h"
#include "../vendor/lua-5.3.5/src/lauxlib.h"
}

#include "../vendor/zlib-1.3.1/zlib.h"

#define CHUNK_SIZE 16384

//  likely don't want this because it breaks cross-platform compatibility
//  TODO: benchmark archive size and load speed
#undef COMPILE_SCRIPTS

lua_State* luaVM;
std::string key;

template<class T>
inline std::string toString(T val, bool groupDigits = true) {
    std::ostringstream o;
    o << val;
    std::string s(o.str());

    if (groupDigits) {
        uint32_t offset1 = 3;
        uint32_t offset2 = 3;
        if (val < 0) {
            offset2++;   // account for '-'
        }
        for (uint32_t i = 0; i < 5; i++) {
            if (s.length() > offset2) {
                s.insert(s.length() - offset1, ",");
                offset1 += 4;
                offset2 += 4;
            }
        }
    }
    return s;
}

void crypt(uint8_t *data, uint32_t size, std::string const& key) {
    if (key.empty()) {
        return;
    }

    uint32_t keyIndex = 0;
    for (uint32_t i = 0; i < size; i++) {
        data[i] ^= (key.c_str()[keyIndex] ^ 0xAA);
        keyIndex++;
        if (keyIndex >= key.length()) {
            keyIndex = 0;
        }
    }
}

struct Asset {
    Asset(std::string filename) {
        this->filename = filename;

        FILE *file = fopen(filename.c_str(), "rb");
        if (!file) {
            std::cout << "Unable to open file: " << filename << std::endl;
        }

        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        data = new uint8_t[size];
        uint32_t bytesRead = fread(data, 1, size, file);
        if (bytesRead != size) {
            std::cout << "Size mismatch!" << std::endl;
        }

        fclose(file);
    }

    ~Asset() {
        delete [] data;
    }

    std::string filename;

    uint8_t* data;
    uint64_t size;

    protected:
        Asset();
};

std::vector<Asset*> assets;

void zerr(int32_t ret) {
    switch (ret) {
        case Z_ERRNO: std::cerr << "I/O error" << std::endl; exit(ret); break;
        case Z_STREAM_ERROR: std::cerr << "Invalid compression level" << std::endl; exit(ret); break;
        case Z_DATA_ERROR: std::cerr << "Invalid or incomplete deflate data" << std::endl; exit(ret); break;
        case Z_MEM_ERROR: std::cerr << "Out of memory" << std::endl; exit(ret); break;
        case Z_VERSION_ERROR: std::cerr << "zlib version mismatch!" << std::endl; exit(ret); break;
        
        default: break;
    }
}

int compress(const uint8_t* inputData, uint64_t inputSize, uint8_t** outputData, uint32_t &outputSize, uint32_t level) {
    int32_t ret;
    uint32_t flush;
    uint32_t have;
    z_stream strm;
    uint8_t in[CHUNK_SIZE];
    uint8_t out[CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK) {
        return ret;
    }

    std::vector<uint8_t> outputBuffer;
    uint64_t offset = 0;

    do {
        strm.avail_in = (offset + CHUNK_SIZE > inputSize) ? inputSize - offset : CHUNK_SIZE;
        std::memcpy(in, inputData + offset, strm.avail_in);
        offset += strm.avail_in;

        flush = (offset >= inputSize) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        do {
            strm.avail_out = CHUNK_SIZE;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            assert(ret != Z_STREAM_ERROR);
            have = CHUNK_SIZE - strm.avail_out;
            outputBuffer.insert(outputBuffer.end(), out, out + have);
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);

    deflateEnd(&strm);

    *outputData = new uint8_t[outputBuffer.size()];
    std::copy(reinterpret_cast<const uint8_t*>(outputBuffer.data()), 
        reinterpret_cast<const uint8_t*>(outputBuffer.data() + outputBuffer.size()),
        *outputData);

    outputSize = outputBuffer.size();

    return Z_OK;
}

//  callback function for lua_dump
int dumpBytecode(lua_State* luaVM, const void* p, size_t sz, void* ud) {
    std::ofstream* outFile = static_cast<std::ofstream*>(ud);
    outFile->write(static_cast<const char*>(p), sz);

    return 0;
}

bool compileScript(const std::string& inputFilename, const std::string& outputFilename) {
    //  load the Lua script
    if (luaL_loadfile(luaVM, inputFilename.c_str()) != LUA_OK) {
        std::cerr << "Failed to load script: " << lua_tostring(luaVM, -1) << std::endl;
        return false;
    }

    //  extract the directory path from the filename
    std::filesystem::path filePath(outputFilename);
    std::filesystem::path directory = filePath.parent_path();

    //  create all directories if they don't exist
    if (!std::filesystem::create_directories(directory)) {
        if (!std::filesystem::exists(directory)) {
            std::cerr << "Failed to create directories: " << directory << std::endl;
            return false;
        }
    }

    //  dump the compiled bytecode to a file
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Failed to open output file." << std::endl;
        return false;
    }
    lua_dump(luaVM, dumpBytecode, &outputFile, true);

    outputFile.close();

    return true;
}

void report(uint64_t sizeDataOriginal, uint32_t outputSize) {
    std::cout << std::endl;
    std::cout << "Total asset files: " << assets.size() << std::endl;
    std::cout << std::endl;
    std::cout << "Original size: " << toString(sizeDataOriginal) << " bytes" << std::endl;
    std::cout << "Compressed size: " << toString(outputSize) << " bytes" << std::endl;
    std::cout << std::endl;
    std::cout << "Compression ratio: ";
    std::cout << (uint32_t)((float)outputSize / (float)sizeDataOriginal  * 100.0f) << "%" << std::endl;
    std::cout << std::endl;
}

void buildArchive(std::string archivePath) {
    //  read all asset files
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (!std::filesystem::is_directory(entry.status())) {
            //  exclude empty files (typically .gitkeep files from the project template)
            if (std::filesystem::file_size(entry) > 0) {
                std::string filename = entry.path().relative_path().string();
                filename.erase(0, 2);

                //  exclude any previously compiled lua scripts
                if (filename.compare(filename.length() - 5, 5, ".luac") != 0) {
#ifdef COMPILE_SCRIPTS
                    // compile lua scripts
                    if (filename.compare(filename.length() - 4, 4, ".lua") == 0) {
                        const std::string prefix = "scripts/";

                        std::string output = filename;
                        if (output.substr(0, prefix.length()) == prefix) {
                            output.insert(prefix.length(), "compiled/");
                        }
                        output += 'c';

                        std::cout << "Compiling " << filename << std::endl;
                        if (!compileScript(filename, output)) {
                            lua_close(luaVM);
                            exit(1);
                        }

                        filename = output;
                    }
#endif
                    assets.push_back(new Asset(filename));
                }
            }
        }
    }
    std::cout << std::endl;

    //  build manifest
    uint64_t offset = 0;

    std::stringstream ss;
    ss << assets.size() << "\n";
    for (auto asset :assets) {
        std::string filename = asset->filename;
        std::replace(filename.begin(), filename.end(), ' ', '*'); 
        ss << filename << " ";

        // ss << asset->filename << " ";
        ss << asset->size << " ";
        ss << offset << "\n";

        offset += asset->size;
    }

    std::string manifest = ss.str();
    uint64_t sizeDataOriginal = offset + manifest.size();

    uint8_t* buffer = new uint8_t[sizeDataOriginal];

    //  copy manifest size into the buffer
    uint32_t manifestSize = manifest.size();
    std::memcpy(buffer, &manifestSize, sizeof(uint32_t));

    //  copy manifest and each asset's data into the buffer
    std::memcpy(buffer + sizeof(uint32_t), manifest.c_str(), manifest.size());
    offset = sizeof(uint32_t) + manifest.size();
    for (const auto asset : assets) {
        std::cout << "Adding " << asset->filename << std::endl;
        std::memcpy(buffer + offset, asset->data, asset->size);
        offset += asset->size;
        delete asset;
    }

    uint8_t* outputData;
    uint32_t outputSize;

    compress(buffer, offset, &outputData, outputSize, Z_DEFAULT_COMPRESSION);
    crypt(outputData, outputSize, key);

    //  write archive file
    std::ofstream file(archivePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    file.write("AB2", 3);
    file.write(reinterpret_cast<const char*>(outputData), outputSize);
    file.close();

    std::cout << std::endl;
    std::cout << "Manifest:" << std::endl << std::endl;
    std::cout << manifest << std::endl << std::endl;

    delete [] buffer;

    report(sizeDataOriginal, outputSize);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Mustard Engine Asset Compiler\n\n";
        std::cout << "Usage: Mustard-AssetCompiler outputFile.dat [key]\n";
    } else {
        std::cout << "Building archive [" << argv[1] << "]...\n\n";

        key = argc > 2 ? argv[2] : NULL;
        std::cout << "KEY: " << key << "\n\n";

        luaVM = luaL_newstate();
        if (!luaVM) {
           std::cerr << "Error initializing Lua VM";
        }
        luaL_openlibs(luaVM);
        buildArchive(argv[1]);
        lua_close(luaVM);
    }

    return 0;
}

