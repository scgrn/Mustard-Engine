/*

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

*/

/**
Localization functions
*/

#include "../pch.h"

#include "script.h"
#include "../core/localization.h"

namespace AB {

extern Script script;

/// Loads string definitions. Will populate AB.l10n.languages with a table of available languages.
// @param filename csv file with string definitions
// @function AB.l10n.init
static int luaInit(lua_State* luaVM) {
    std::string filename = lua_tostring(luaVM, 1);

	initLocalization(filename);
	
	return 0;
}

/// Sets the current language
// @param language
// @function AB.l10n.setLanguage
static int luaSetLanguage(lua_State* luaVM) {
    int index = (int)lua_tointeger(luaVM, 1) - 1;
    language = static_cast<Language>(index);

    return 0;
}

/// Retrieves a string in the current language
// @param id String identifier
// @function AB.l10n.getString
static int luaGetString(lua_State* luaVM) {
    std::string key = lua_tostring(luaVM, 1);

    std::string s = strings[language][key];
    if (s.empty()) {
        s = "[STRING UNDEFINED]";
    }

    lua_pushstring(luaVM, s.c_str());

    return 1;
}

void registerLocalizationFunctions() {
    static const luaL_Reg l10nFuncs[] = {
		{ "init", luaInit},
        { "setLanguage", luaSetLanguage},
        { "getString", luaGetString},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "l10n", l10nFuncs);
}

}
