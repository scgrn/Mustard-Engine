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

#include <iostream>
#include <cstdarg>
#include <sstream>
#include <stdexcept>
#include <cstdio>

#include "log.h"

#ifndef ANDROID

namespace AB {

void redirectLog(std::streambuf* dest) { std::clog.rdbuf(dest); }

//    oh wow variadic functions

void error(int line, const char* file, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    char szBuf[4096];
    vsprintf(szBuf, msg, args);
    va_end(args);

    std::ostringstream o;
    // o << file << " [" << line << "] \t" << szBuf << std::endl;
    o << "[ERR] \t" << szBuf << std::endl;
    std::string s(o.str());

    throw (std::runtime_error(s));
}

void log(int line, const char* file, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    char szBuf[4096];
    vsprintf(szBuf, msg, args);
    va_end(args);

    // prefix << file << " [" << line << "]";

    std::clog << "[INFO] \t" << szBuf << std::endl;
}

}   //  namespace

#endif
