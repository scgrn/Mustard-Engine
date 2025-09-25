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

static std::unordered_map<std::string, std::vector<float>> samples;

namespace AB {

ScopedTimer::~ScopedTimer() {
    f32 time = timer.elapsedMillis();
    samples[name].emplace_back(time);
}

struct Stats {
    u32 count;
    f32 mean;
    f32 median;
    f32 stddev;
    f32 min;
    f32 max;
    f32 p95;
};

Stats computeStats(const std::vector<float>& v) {
    Stats s{};
    if (v.empty()) {
        return s;
    }

    s.count = v.size();

    //  mean
    f32 sum = 0.0;
    for (f32 x : v) {
        sum += x;
    }
    s.mean = sum / v.size();

    //  median + percentiles require sorting
    std::vector<float> sorted = v;
    std::sort(sorted.begin(), sorted.end());

    auto getPercentile = [&](float pct) {
        size_t idx = static_cast<size_t>(pct * (sorted.size() - 1));
        return sorted[idx];
    };

    s.median = getPercentile(0.5f);
    s.p95 = getPercentile(0.95f);

    //  min/max
    s.min = sorted.front();
    s.max = sorted.back();

    //  standard deviation
    f32 variance = 0.0f;
    for (f32 x : v) {
        variance += (x - s.mean) * (x - s.mean);
    }
    s.stddev = std::sqrt(variance / v.size());

    return s;
}

void reportProfiling() {
    LOG("\n===== Profiling Report =====\n", 0);

    for (const auto& [name, vec] : samples) {
        Stats s = computeStats(vec);

        std::cout << name << " (" << s.count << " samples)\n"
            << "\tMean: " << s.mean << " ms\n"
            << "\tMedian: " << s.median << " ms\n"
            << "\tStandard deviation: " << s.stddev << " ms\n"
            << "\tMin: " << s.min << " ms\n"
            << "\tMax: " << s.max  << " ms\n"
            << "\tP95: " << s.p95  << " ms\n\n";
    }
}

}

