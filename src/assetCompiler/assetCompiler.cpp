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

//  can't use c++17 std::filesystem because of
//  https://sourceforge.net/p/mingw-w64/bugs/737/
//  which is hogwash of the highest order.

#include <filesystem>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cstring>

#include "../vendor/zlib-1.3.1/zlib.h"

#define COMPRESS

std::string key;

template<class T>
inline std::string toString(T val, bool groupDigits = true) {
    std::ostringstream o;
    o << val;
    std::string s(o.str());

    if (groupDigits) {
        unsigned int offset1 = 3;
        unsigned int offset2 = 3;
        if (val < 0) {
            offset2++;   // account for "-"
        }
        for (int i = 0; i < 5; i++) {
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
#ifndef COMPRESS
    return;
#endif

    if (key.empty()) return;

    uint32_t keyIndex = 0;
    for (uint32_t i = 0; i < size; i++) {
        data[i] ^= (key.c_str()[keyIndex] ^ 0xAA);
        keyIndex++;
        if (keyIndex > key.length()) {
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

        data = new unsigned char[size];
        int bytesRead = fread(data, 1, size, file);
        if (bytesRead != size) {
            std::cout << "Size mismatch!" << std::endl;
        }
        fclose(file);
    }

    ~Asset() {
        delete [] data;
    }

    std::string filename;

    unsigned char* data;
    unsigned long size;

    protected:
        Asset();
};

std::vector<Asset*> assets;

void buildArchive(std::string archivePath) {
    //  read all asset files
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (!std::filesystem::is_directory(entry.status())) {
            std::string filename = entry.path().relative_path().string();
            filename.erase(0, 2);

            assets.push_back(new Asset(filename));
        }
    }

    uint64_t offset = 0;

    std::stringstream ss;
    
    ss << assets.size() << "\n";
    for (auto asset :assets) {
        ss << asset->filename << " ";
        ss << asset->size << " ";
        ss << offset << "\n";
        
        offset += asset->size;
    }
    
    std::string manifest = ss.str();
    uint64_t sizeDataOriginal = offset + manifest.size();
    std::cout << sizeDataOriginal << std::endl;
    
    unsigned char* buffer = new unsigned char[sizeDataOriginal];

    // copy manifest and each asset's data into the buffer
    std::memcpy(buffer, manifest.c_str(), manifest.size());
    offset = manifest.size();
    for (const auto asset : assets) {
        std::memcpy(buffer + offset, asset->data, asset->size);
        offset += asset->size;
        delete asset;
    }

    //  ...
    
    delete [] buffer;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Mustard Engine Asset Compiler\n\n";
        std::cout << "Usage: Mustard-AssetCompiler outputFile.dat [key]\n";
    } else {
        std::cout << "Building archive [" << argv[1] << "]...\n\n";

        key = argc > 2 ? argv[2] : NULL;
        std::cout << "KEY: " << key << "\n\n";

        buildArchive(argv[1]);
    }

    return 0;
}

