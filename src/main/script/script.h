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

#ifndef AB_SCRIPT_H
#define AB_SCRIPT_H

#include <iostream>

extern "C" {
#include "lua-5.3.5/src/lua.h"
#include "lua-5.3.5/src/lualib.h"
#include "lua-5.3.5/src/lauxlib.h"
}

#include "../core/subsystem.h"

namespace AB {

class Script : public SubSystem {
    public:
        b8 startup() override;
        void shutdown() override;
        
        void registerFuncs(std::string const& parent, std::string const& name, const luaL_Reg *funcs);
        void execute(std::string command);
        void reset();
        
        lua_State* getVM() { return luaVM; }
        
        b8 luaError = false;
        
    protected:
        lua_State* luaVM;
};

}   //  namespace

#endif

