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

//	memory.cpp

// https://stackoverflow.com/questions/8186018/how-to-properly-replace-global-new-delete-operators
// https://en.cppreference.com/w/cpp/memory/new/operator_new

#include "pch.h"
#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <new>

#ifndef MEMPOOL_SIZE_MB
#define MEMPOOL_SIZE_MB 4
#endif // MEMPOOL_SIZE_MB

#define MEMPOOL_SIZE 1024 * 1024 * MEMPOOL_MB
#define PAGE_SIZE 1024
#define TRACKING_UNIT Uint64
#define NUM_TRACKING_UNITS MEMPOOL_SIZE / sizeof(TRACKING_UNIT)

struct MemoryAnalytics {
    std::size_t used = 0;
    std::size_t max = 0;
};

static MemoryAnalytics analytics;

void* operator new(std::size_t sz, const char *file, int line) {
	analytics.used += sz;

#ifdef DEBUG
	//LOG("Allocating %zu bytes from %s, line %d. Current memory allocated: %zu", sz, file, line, analytics.used);
#endif // DEBUG

	void *ptr = std::malloc(sz);
	if (ptr) {
		return ptr;
	} else {
		throw std::bad_alloc{};
	}
}

void* operator new[](std::size_t sz, const char *file, int line) {
    return operator new(sz, file, line);
}


void operator delete(void* ptr, std::size_t sz) noexcept {
	analytics.used -= sz;

	std::free(ptr);

#ifdef DEBUG
	//LOG("Freeing %zu bytes. Current memory allocated: %zu", sz, analytics.used);
#endif // DEBUG
}

void operator delete[](void* ptr, std::size_t sz) noexcept {
    operator delete(ptr, sz);
}

#define new new(__FILE__, __LINE__)

