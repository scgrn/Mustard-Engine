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

#include <unordered_map>
#include <cmath>

#include "profiler.h"
#include "../../main/core/log.h"

extern "C" {

//  these will bookend all function calls when compiled with -finstrument-functions
void __cyg_profile_func_enter(void *thisFunc, void *callSite) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *thisFunc, void *callSite) __attribute__((no_instrument_function));

void __cyg_profile_func_enter(void *thisFunc, void *callSite) {
    //    why does this crash?
  //printf("ENTER: %p, from %p\n", thisFunc, callSite);
}

void __cyg_profile_func_exit(void *thisFunc, void *callSite) {
  //printf("EXIT:  %p, from %p\n", thisFunc, callSite);
}
}

static std::unordered_map<std::string, std::vector<AB::f32>> samples;

namespace AB {

ScopedTimer::~ScopedTimer() {
    f32 time = timer.elapsedMillis();
    samples[name].emplace_back(time);
}

struct Stats {
    u32 count;
    f32 mean;
    f32 median;
    f32 standardDeviation;
    f32 min;
    f32 max;
};

Stats computeStats(const std::vector<float>& v) {
    Stats stats{};
    if (v.empty()) {
        return stats;
    }

    stats.count = v.size();

    //  mean
    f32 sum = 0.0;
    for (f32 x : v) {
        sum += x;
    }
    stats.mean = sum / v.size();

    //  sort to find median
    std::vector<float> sorted = v;
    std::sort(sorted.begin(), sorted.end());
    stats.median = sorted[(int)stats.count / 2];

    //  min/max
    stats.min = sorted.front();
    stats.max = sorted.back();

    //  standard deviation
    f32 variance = 0.0f;
    for (f32 x : v) {
        variance += (x - stats.mean) * (x - stats.mean);
    }
    stats.standardDeviation = std::sqrt(variance / v.size());

    return stats;
}

void reportProfiling() {
    std::string report;
    for (const auto& [name, vec] : samples) {
        Stats stats = computeStats(vec);

        if (stats.count > 1) {
            report += name + " (" + std::to_string(stats.count) + " samples)\n";
            report += "\tMean: " + std::to_string(stats.mean) + " ms\n";
            report += "\tMedian: " + std::to_string(stats.median) + " ms\n";
            report += "\tStandard deviation: " + std::to_string(stats.standardDeviation) + " ms\n";
            report += "\tMin: " + std::to_string(stats.min) + " ms\n";
            report += "\tMax: " + std::to_string(stats.max) + " ms\n\n";
        } else {
            report += name + ": " + std::to_string(stats.max) + " ms\n\n";
        }
    }
    LOG("\n\n===== Profiling Report =====\n\n%s", report.c_str());
}

}

