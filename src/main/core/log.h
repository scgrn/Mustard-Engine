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

    @file log.h
    @author Andrew Krause - contact@alienbug.net
    @date 08.09.11

    Standard logging class. Output can be redirected to any stream.

    @todo Implement log levels.

*/

#ifndef AB_LOG_H
#define AB_LOG_H

#include <iostream>
#include <sstream>

enum {
    LOG_ERROR,
    LOG_INFO,
    LOG_DEBUG,
};

namespace AB {
#ifdef ANDROID
#include <android/log.h>
#define LOG(msg, ...) __android_log_print(ANDROID_LOG_DEBUG, "JNI", msg, __VA_ARGS__)
#define LOG_EXP(name) __android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s = %s", #name, name)
#define ERR(msg, ...) __android_log_print(ANDROID_LOG_ERROR, "JNI", msg, __VA_ARGS__)
#else
#ifdef DEBUG
#define LOG(msg, ...) AB::log(LOG_DEBUG, __LINE__, __FILE__, msg, __VA_ARGS__)
#define LOG_EXP(name) AB::logExp(__LINE__, __FILE__, #name, name)
#define ERR(msg, ...) AB::error(__LINE__, __FILE__, msg, __VA_ARGS__)
#else
#define LOG(msg, ...) 
#define LOG_EXP(name) 
#define ERR(msg, ...) 
#endif

void log(int level, int line, const char* file, const char* msg, ...);
void error(int line, const char* file, const char* msg, ...);
extern void redirectLog(std::streambuf* dest);

extern int logLevel;

template <class T>
void logExp(int line, const char* file, const char* name, T value) {
    std::stringstream out;
    out << name << " == " << value;
    if (out) {
        log(LOG_DEBUG, line, file, out.str().c_str());
    } else {
        LOG("Couldn't convert value to string", 0);
    }
}

}   //  namespace

#endif

#endif
