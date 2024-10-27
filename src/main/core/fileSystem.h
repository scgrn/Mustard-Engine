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

/**

    @file fileSystem.h
    @date 08.11.10

*/

#ifndef AB_FILE_SYSTEM_H
#define AB_FILE_SYSTEM_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../types.h"

#include "subsystem.h"

namespace AB {

class DataObject {
    public:
        DataObject() : size(0), data(nullptr) {}
        DataObject(u64 size) : size(size), data(new u8[size]) {}

        DataObject(u8* externalData, u64 size) : size(size), data(nullptr), externalData(externalData) {}
    
        //  move constructor and assignment
        DataObject(DataObject&& other) noexcept : size(other.size), data(std::move(other.data)), externalData(other.externalData) {
            other.size = 0;
            other.externalData = nullptr;
        }
        
        DataObject& operator=(DataObject&& other) noexcept {
            if (this != &other) {
                size = other.size;
                data = std::move(other.data);
                externalData = other.externalData;
                other.size = 0;
                other.externalData = nullptr;
            }
            return *this;
        }

        //  deleted copy constructor and copy assignment
        DataObject(const DataObject&) = delete;
        DataObject& operator=(const DataObject&) = delete;

        u8* getData() const { return data ? data.get() : externalData; }
        u64 getSize() const { return size; }

    protected:
        u64 size;
        std::unique_ptr<u8[]> data;
        u8* externalData = nullptr;
    
    private:
        static void noOpDeleter(u8* p) {}

};

class FileSystem : public SubSystem {
    public:
        b8 startup() override;
        void shutdown() override;
        
        //    this is the only function that should be called before engine startup
        void addArchive(std::string const& filename, std::string const& key = "");

        DataObject loadAsset(std::string const& filename, b8 forceLocal = false);

        std::string loadData(std::string key);
        void saveData(std::string key, std::string value);

        static bool loadCompiledScripts;

    private:
        struct ArchiveFile {
            std::string path;
            std::string key;

            // filename -> (size, offset)
            std::unordered_map<std::string, std::pair<u64, u64>> assets;

            std::unique_ptr<u8[]> data;
            u64 size;

            void load();
        };

        DataObject loadAssetFromDisk(const std::string& filename);
        DataObject loadAssetFromArchive(const ArchiveFile& archive, const std::string& filename);

        std::vector<ArchiveFile> archiveFiles;
};

}   //  namespace

#endif

