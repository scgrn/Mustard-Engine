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

//  can't use c++17 std::filesystem because of this bullshit:
//      https://sourceforge.net/p/mingw-w64/bugs/737/

#include <filesystem>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>

#include "zlib.h"
#include "tinyxml.h"

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
    return; //  TODO: remove after testing

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

struct Resource {
    Resource(std::string filename) {
        this->filename = filename;

        FILE *file = fopen(filename.c_str(), "rb");
        if (!file) {
            std::cout << "Unable to open file: " << filename << std::endl;
        }

        std::cout << filename;

        fseek(file, 0, SEEK_END);
        sizeDataOriginal = ftell(file);
        fseek(file, 0, SEEK_SET);

        dataOriginal = new unsigned char[sizeDataOriginal];
        fread(dataOriginal, 1, sizeDataOriginal, file);
        fclose(file);


        sizeDataCompressed  = (sizeDataOriginal * 1.1) + 12;
        dataCompressed = new unsigned char[sizeDataCompressed];
        // std::cout << "Allocated " << sizeDataCompressed << " bytes for compressed data" << std::endl;

        int result = compress(dataCompressed, &sizeDataCompressed, dataOriginal, sizeDataOriginal);
        // TODO: check result

        // encrypt compressed data
        crypt(dataCompressed, sizeDataCompressed, key);

        std::cout << std::string(std::max(0, 50 - (int)filename.length()), ' ');
        std::cout << "[" << (int)((float)sizeDataCompressed / (float)sizeDataOriginal  * 100.0f) << "%]\n";
        //std::cout << "[" << sizeDataOriginal << "]\n";
    }

    ~Resource() {
        // std::cout << "Cleaning up " << filename << std::endl;
        delete [] dataOriginal;
        delete [] dataCompressed;
    }

    std::string filename;

    unsigned char* dataOriginal;
    unsigned long sizeDataOriginal;

    unsigned char* dataCompressed;
    unsigned long sizeDataCompressed;

    uint32_t offset;

protected:
        Resource();
};

std::vector<Resource*> resourceFiles;

void buildArchive(std::string archivePath) {
    //  read all resource files
    int fileCount = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (!std::filesystem::is_directory(entry.status())) {
                std::string filename = entry.path().relative_path().string();
                filename.erase(0, 2);

                resourceFiles.push_back(new Resource(filename));

                fileCount++;
        }
    }

    uint32_t offset = 0;

    //  build XML header
    TiXmlDocument doc;
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "iso-8859-1", "");
    doc.LinkEndChild(decl);

    TiXmlElement *rootElement = new TiXmlElement("root");
    doc.LinkEndChild(rootElement);

    //  for every loaded resource, add its size and offset into the header
    /*
    for (std::vector<Resource*>::iterator i = resources.begin(); i != resources.end(); i++) {
        (*i)->setOffset(offset);
        offset += (*i)->getSize();
    */
    for (auto resource :resourceFiles) {
        resource->offset = offset;

        //  TODO: set to compressed
        offset += resource->sizeDataOriginal;

        TiXmlElement *resourceElement = new TiXmlElement("resource");

        std::replace(resource->filename.begin(), resource->filename.end(), '\\', '/');

        resourceElement->SetAttribute("path", resource->filename.c_str());
        resourceElement->SetAttribute("offset", toString(resource->offset, false).c_str());
        resourceElement->SetAttribute("sizeCompressed", toString(resource->sizeDataCompressed, false).c_str());
        resourceElement->SetAttribute("sizeDecompressed", toString(resource->sizeDataOriginal, false).c_str());
        rootElement->LinkEndChild(resourceElement);
    }
    std::cout << "Total resource data size " << toString(offset) << " bytes.\n\n";

    TiXmlPrinter printer;
    printer.SetIndent("    ");

    doc.Accept(&printer);
    std::string xmlText = printer.CStr();
    uint32_t size = xmlText.size();
    uint8_t *data = new uint8_t[size + 1];
    std::copy(xmlText.begin(), xmlText.end(), data);
    data[size] = '\0';

    //  compress and encrypt header
    unsigned long sizeDataCompressed  = (size * 1.1) + 12;
    unsigned char* dataCompressed = new unsigned char[sizeDataCompressed];
    int result = compress(dataCompressed, &sizeDataCompressed, data, size);
    // TODO: check result

    // encrypt compressed data
    crypt(dataCompressed, sizeDataCompressed, key);


    FILE *file = fopen(archivePath.c_str(), "wb");

    fwrite("AB1 ",1, 4, file);
    fwrite(&sizeDataCompressed, 1, 4, file);      // header size compressed
    fwrite(&size, 1, 4, file);      // header size decompressed
    //fwrite(dataCompressed, 1, sizeDataCompressed, file);
    fwrite(data, 1, size, file);

    //  write all loaded resources
    for (auto resource :resourceFiles) {
        //fwrite(resource->dataCompressed, 1, resource->sizeDataCompressed, file);
        fwrite(resource->dataOriginal, 1, resource->sizeDataOriginal, file);
    }
    /*
    for (std::vector<Resource*>::iterator i = resources.begin(); i != resources.end(); i++) {
        (*i)->writeData(file, key);
    }
    */
    fclose(file);

    delete [] data;
    delete [] dataCompressed;

    std::cout << "\nProcessed " << fileCount << " files\n";

    unsigned long total = 0L;
    unsigned long totalCompressed = 0L;

    //  clean up files
    for (auto resource :resourceFiles) {
        total += resource->sizeDataOriginal;
        totalCompressed += resource->sizeDataCompressed;

        delete resource;
    }

    std::cout << "Total size original: " << toString(total) << std::endl;
    std::cout << "Total size compresed: " << toString(totalCompressed) << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
		std::cout << "Alien Bug Asset Compiler\n\n";
        std::cout << "Usage: ABAC outputFile.dat [key]\n";
    } else {
        std::cout << "Building archive [" << argv[1] << "]...\n\n";

        key = argc > 2 ? argv[2] : NULL;
        std::cout << "KEY: " << key << "\n\n";

        buildArchive(argv[1]);
    }

    return 0;
}
