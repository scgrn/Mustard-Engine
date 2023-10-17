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

#ifndef AB_PCH_H
#define AB_PCH_H

#include "types.h"

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cstdarg>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cassert>

#ifdef ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#include <emscripten.h>
#else
#include "glad/glad.h"
#include <SDL2/SDL.h>
#endif
#endif

extern "C" {
#include "lua-5.3.5/src/lua.h"
#include "lua-5.3.5/src//lualib.h"
#include "lua-5.3.5/src//lauxlib.h"
}

#ifdef DEBUG
extern void checkOpenGLError(const char* stmt, const char* fname, int line);
#define CALL_GL(stmt) do { \
        stmt; \
        checkOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (false)
#else
#define CALL_GL(stmt) stmt
#endif // DEBUG

#endif // AB_PCH_H

