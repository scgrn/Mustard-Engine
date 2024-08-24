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
#include <memory>

#include "../types.h"

#include "subsystem.h"

namespace AB {

class DataObject {
    public:
        DataObject(const char* path, b8 forceLocal = false);
        DataObject() : data(nullptr), size(0) {}

        void setData(u8* newData, u64 newSize);

        ~DataObject();

        // u8* getData() { return data.get(); }
        u8* getData() { return data; }
        u64 getSize() { return size; }

    protected:
        // std::shared_ptr<u8> data;
        u8* data;
        u64 size;

    private:
        static void noOpDeleter(u8* p) {}

};

class FileSystem : public SubSystem {
    public:
        b8 startup() override;
        void shutdown() override;
        
        //    this is the only function that should be called before engine startup
        void addArchive(std::string const& path, std::string const& key = "");

        //  caller is responsible to delete() returned pointer
        u8* readFile(std::string const& path, u64 *size);

        //  returned pointer should not be deleted
        u8* readFromArchive(std::string const& path, u64 *size);

        std::string loadData(std::string key);
        void saveData(std::string key, std::string value);

        static bool loadCompiledScripts;

    private:
        struct AssetFile {
            void dump(u8* basePtr);

            std::string path;
            u64 offset;
            u64 size;
        };

        struct ArchiveFile {
            std::string path;
            std::string key;
            DataObject *dataObject;

            std::vector<AssetFile> assets;
            u8* basePtr;

            void load();
            ~ArchiveFile() { delete dataObject; }
        };
        
        std::vector<ArchiveFile> archiveFiles;
};

}   //  namespace

#endif

