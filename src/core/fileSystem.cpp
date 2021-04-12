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

#include "tinyxml/tinyxml.h"

#include "zlib.h"

namespace AB {

extern FileSystem fileSystem;

//	TODO: i'd like to move these as members of filesystem but then dataobject can't see them
struct FileResource {
	std::string path;
	uint32_t offset, sizeCompressed, sizeDecompressed;
};

struct Archive {
	std::string path, key;
	std::vector<FileResource> resources;
	uint32_t headerSizeCompressed, headerSizeDecompressed;
};

std::vector<Archive> archives;
		

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


bool FileSystem::startup() {
	LOG("FileSystem subsystem startup", 0);

	//	load all queued archives
	for (std::vector<ArchiveFile>::iterator archiveFile = archiveFiles.begin(); archiveFile != archiveFiles.end(); archiveFile++) {
		loadArchive(archiveFile->path, archiveFile->key);
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

// TODO: read entire file and keep it in memory?
void FileSystem::loadArchive(std::string const& path, std::string const& key) {
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG("WARNING: Couldn't load archive %s", path.c_str());
        return;
    }

    Archive archive;
    archive.path = path;
    archive.key = key;

    //  fourth byte of header is reserved ... for ...
    Uint8 tag[4];
    fread(&tag, 1, 4, file);
    if (tag[0] != 'A' || tag[1] != 'B') {
        ERR("INVALID ARCHIVE: %s", path.c_str());
    }

    //  check version code. only handle version 1 now. branch for backwards compatibility if extended.
    Uint8 versionCode = tag[2];
    if (versionCode != 1) {
        //ERR("INVALID ARCHIVE: %s", path.c_str());
    }

    //  read and parse header
    fread(&archive.headerSizeCompressed, 1, 4, file);
    fread(&archive.headerSizeDecompressed, 1, 4, file);
/*
    uint8_t *dataCompressed = new uint8_t[archive.headerSizeCompressed];
    fread(dataCompressed, 1, archive.headerSizeCompressed, file);
    crypt(dataCompressed, archive.headerSizeCompressed, key);
    fclose(file);

    //  decompress header data
    uint8_t *data = new uint8_t[archive.headerSizeDecompressed];
    int result = uncompress(data, &archive.headerSizeDecompressed, dataCompressed, archive.headerSizeCompressed);
    //  TODO: check result
*/
    uint8_t *data = new uint8_t[archive.headerSizeDecompressed];
    fread(data, 1, archive.headerSizeDecompressed, file);
    fclose(file);

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
}

unsigned char* FileSystem::readFile(std::string const& path, unsigned int *size) {
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG("ERROR LOADING FILE! %s", path.c_str());
        return 0;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = new unsigned char[*size];
    fread(data, 1, *size, file);
    fclose(file);

    return data;
}

DataObject::DataObject(const char* path, unsigned int *size, bool forceLocal) {
    if (forceLocal) {
        LOG("Loading %s from local filesystem...", path);
        data = fileSystem.readFile(path, size);
        return;
    }

    for (std::vector<Archive>::iterator archive = archives.begin(); archive != archives.end(); archive++) {
        for (std::vector<FileResource>::iterator resource = archive->resources.begin(); resource != archive->resources.end(); resource++) {
            if (resource->path == path) {
                LOG("Loading resource %s from archive %s", resource->path.c_str(), archive->path.c_str());

                FILE *file = fopen(archive->path.c_str(), "rb");
                //fseek(file, resource->offset + archive->headerSizeCompressed, SEEK_SET);
                fseek(file, resource->offset + archive->headerSizeDecompressed + 12, SEEK_SET);
                /*
                *size = resource->sizeCompressed;
                data = new unsigned char[*size];
                fread(data, 1, *size, file);
                crypt(data, *size, archive->key);
                */

                // uint8_t *dataCompressed = new uint8_t[resource->sizeCompressed];
                // fread(dataCompressed, 1, resource->sizeCompressed, file);
                // crypt(dataCompressed, resource->sizeCompressed, archive->key);

                *size = resource->sizeDecompressed;
                data = new unsigned char[*size];
                fread(data, 1, *size, file);

                LOG_EXP(*size);

                //int result = uncompress(data, size, dataCompressed, resource->sizeCompressed);
                //  TODO: check result

                //delete [] dataCompressed;

                fclose(file);

                return;
            }
        }
    }
    LOG("File <%s> not found in archive, loading from local filesystem", path);
#ifdef DEBUG
    data = fileSystem.readFile(("assets/" + std::string(path)).c_str(), size);
    return;
#endif
    std::string filename = path;
    ERR("File not found: %s", filename.c_str());
}

DataObject::~DataObject() {
    delete [] data;
}

unsigned char* DataObject::getData() {
    return data;
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
