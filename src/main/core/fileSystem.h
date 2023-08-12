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

    Uhhhrm... handles file operations.

*/

#ifndef AB_FILE_SYSTEM_H
#define AB_FILE_SYSTEM_H

#include <iostream>
#include <vector>

#include "../core/subsystem.h"

namespace AB {

class FileSystem : public SubSystem {
    public:
        bool startup() override;
        void shutdown() override;
        
        //    this is the only function that should be called before engine startup
        void addArchive(std::string const& path, std::string const& key = "");

        //  caller is responsible to delete() returned pointer
        unsigned char* readFile(std::string const& path, size_t *size);
    
        std::string loadData(std::string key);
        void saveData(std::string key, std::string value);

    private:
        void loadArchive(std::string const& path, std::string const& key = "");
        
        struct ArchiveFile {
            std::string path;
            std::string key;
        };
        std::vector<ArchiveFile> archiveFiles;
};

class DataObject {
    public:
        DataObject(const char* path, bool forceLocal = false);
        ~DataObject();

        unsigned char* getData() { return data; } 
        size_t getSize() { return size; }

    protected:
        DataObject() {}

        unsigned char *data;
        size_t size;

};

}   //  namespace

#endif

