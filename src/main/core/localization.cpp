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

#include "localization.h"

#include <map>
#include <sstream>

#include "log.h"
#include "fileSystem.h"
#include "../script/script.h"

namespace AB {

extern FileSystem fileSystem;

std::map<std::string, std::string> strings[LAST];
Language language;

//  TODO: make this a lua function
void initLocalization(std::string const& filename) {

    //  read string table
    LOG("Loading string table from <%s>", filename.c_str());

    AB::DataObject stringDataObject = fileSystem.loadAsset(filename);
    std::string stringData = std::string((const char*)stringDataObject.getData(), stringDataObject.getSize());

    std::istringstream ss(stringData);

    std::string key, e, f, i, g, s;

    // TODO: handle quotes so we can use commas
    while (std::getline(ss, key, ',')) {
        std::getline(ss, e, ',');
        std::getline(ss, f, ',');
        std::getline(ss, i, ',');
        std::getline(ss, g, ',');
        std::getline(ss, s);

        strings[ENGLISH][key] = e;
        strings[FRENCH][key] = f;
        strings[ITALIAN][key] = i;
        strings[GERMAN][key] = g;
        strings[SPANISH][key] = s;
    }
}

}   //  namespace
