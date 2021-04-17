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
	LOG_FATAL,
	LOG_ERROR,
	LOG_WARN,
	LOG_INDO,

	// (V debug builds V)
	LOG_DEBUG,
	LOG_TRACE
};

// switches for warn...trace

namespace AB {

#ifdef ANDROID
#include <android/log.h>
#define LOG(msg, arg...) __android_log_print(ANDROID_LOG_DEBUG, "JNI", msg, arg);
#define LOG_EXP(name)__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s = $s", #name, name);
#define ERR(msg, arg...)__android_log_print(ANDROID_LOG_ERROR, "JNI", msg, arg);
#else
#define LOG(msg, arg...) AB::log(__LINE__, __FILE__, msg, arg);
#define LOG_EXP(name) AB::logExp(__LINE__, __FILE__, #name, name);
#define ERR(msg, arg...) AB::error(__LINE__, __FILE__, msg, arg);

void log(int line, const char* file, const char* msg, ...);
void error(int line, const char* file, const char* msg, ...);
extern void redirectLog(std::streambuf* dest);

template <class T>
void logExp(int line, const char* file, const char* name, T value) {
    std::stringstream out;
    out << name << " == " << value;
    log(line, file, out.str().c_str());
}

}   //  namespace

#endif

#endif
