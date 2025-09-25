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

#ifndef AB_PROFILER_H
#define AB_PROFILER_H

#include <chrono>

#ifdef DEBUG
#define PROFILE(name) AB::ScopedTimer scopedTimer(#name);
#else
#define PROFILE(name) 
#endif

namespace AB {

class Timer {
    public:
        Timer() { reset(); }
        void reset() { start = std::chrono::high_resolution_clock::now(); }
        float elapsed() const { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f * 0.001f; }
        float elapsedMillis() const { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f; }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

class ScopedTimer {
    public:
        ScopedTimer(std::string name) : name(name) {}
        ~ScopedTimer() {
            float time = timer.elapsedMillis();
            std::cout << name << " - " << time << "ms\n";
        }

    private:
        Timer timer;
        std::string name;
};

}

#endif
