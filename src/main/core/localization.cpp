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
#include <vector>
#include <sstream>

#include "log.h"
#include "fileSystem.h"
#include "../script/script.h"

namespace AB {

extern FileSystem fileSystem;
extern Script script;
std::vector<Language> languages;

static std::vector<std::string> split(std::string const& s, char delim) {
    std::vector<std::string> ret;
    std::string current;
    bool inQuotes = false;
    
    for (u32 i = 0; i < s.size(); i++) {
        char c = s[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < s.size() && s[i + 1] == '"') {
                    current += '"';      // escaped quote
                    i++;
                } else {
                    inQuotes = false;    //  closing quote
                }
            } else {
                current += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == delim) {
                ret.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
    }

    if (!current.empty()) {
        ret.push_back(current);
    }

    return ret;
}

void initLocalization(std::string const& filename) {
    //  read string table
    LOG("Loading string table from <%s>", filename.c_str());

    AB::DataObject stringDataObject = fileSystem.loadAsset(filename);
    std::string stringData = std::string((const char*)stringDataObject.getData(), stringDataObject.getSize());

    languages.clear();
    std::istringstream ss(stringData);
    std::string line;

    //  read header
    if (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();   
        }
        auto header = split(line, ',');
        for (u32 i = 1; i < header.size(); i++) {
            languages.push_back(Language{header[i], {}});
        }
    }

    //  read strings
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();   
        }
        if (line.empty()) {
            continue;
        }
        auto fields = split(line, ',');
        for (u32 i = 0; i < languages.size() && i + 1 < fields.size(); i++) {
            languages[i].strings[fields[0]] = fields[i + 1];
        }    
    }

    //  set lua languages table
    std::string cmd = "AB.l10n.languages = {";
    for (auto& language : languages) {
        cmd += "\"" + language.name + "\",";
    }
    cmd += "}";
    script.execute(cmd);
}

}   //  namespace
