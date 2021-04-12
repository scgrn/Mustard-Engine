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

    @file resourceManager.h
    @author Andrew Krause - contact@alienbug.net
    @date 08.11.11

    Provides a template and management for resources.

    @todo So much... so much.
*/

// http://www.shmup-dev.com/forum/index.php?topic=1718.0
// http://www.gamedev.net/reference/articles/article2503.asp
// http://www.gamedev.net/community/forums/topic.asp?topic_id=577509

#ifndef AB_RESOURCE_MANAGER_H
#define AB_RESOURCE_MANAGER_H

#include <iostream>
#include <map>

#include "fileSystem.h"
#include "log.h"

namespace AB {

class Resource {
	public:
        //  represents resource's eligibility for garbage collection.
        enum {PERMANENT, TEMPORARY, MOMENTARY} lifeSpan;

        virtual void load(std::string const& id) = 0;
		virtual void release() = 0;
};

template<class T>
class ResourceManager {
	public:
		typedef int Handle;

		ResourceManager();
        ~ResourceManager();

        /**
            returns a pointer to a resource based on it's handle.
            the resource's load function will be called if it's not loaded.
        */
		T* get(Handle handle);

        //  returns whether a resource exists without loading it if it doesn't
		bool find(Handle handle);

        //  clears it.
        void clear(bool clearInfoMap = false);

        //  not yet implemented. will iterate through all resources
        //  and release those whose ref-count is 0
        void collect();

        //  sets an entry in a resource handle/id table
        //  (id will typically represent a filename)
        void mapResource(Handle handle, std::string const& id, bool preCache = false);

        //  returns whether an id string is mapped to a handle
        //  its Handle will be returned in handle if it is
        bool isMapped(std::string id, Handle *handle = 0);


        void precacheAll();

//	private:
        //  this is a list of the pointers to the actual resource data
		std::map<Handle, T*> resourceData;

        //  this maps resource handles to ids (typically filenames)
        std::map<Handle, std::string> resourceInfo;
};

template<class T>
ResourceManager<T>::ResourceManager() {};

template<class T>
ResourceManager<T>::~ResourceManager() {
    clear(true);
};

template<class T>
bool ResourceManager<T>::find(Handle handle) {
	return resourceData.find(handle) != resourceData.end();
};

template<class T>
T* ResourceManager<T>::get(Handle handle) {
    //  load the resource if it doesn't exist
    if (!find(handle)) {
        if (resourceInfo[handle].empty()) {
            ERR("Resource %d not mapped!", handle);
        }

        resourceData[handle] = new T();
        resourceData[handle]->load(resourceInfo[handle]);
    }

    return resourceData[handle];
};

template<class T>
void ResourceManager<T>::precacheAll() {
    typename std::map<int, std::string>::iterator i;

	for (i = resourceInfo.begin(); i != resourceInfo.end(); i++) {
        get(i->first);
	}
}

template<class T>
void ResourceManager<T>::collect() {
}

//  TODO: check if already cleared to avoid SEGFAULT
template<class T>
void ResourceManager<T>::clear(bool clearInfoMap) {
    //  call release() on all resources.
	for (typename std::map<Handle, T*>::iterator i = resourceData.begin();
        i != resourceData.end(); i++) {

		i->second->release();
		delete i->second;
	}
    resourceData.clear();

    if (clearInfoMap) {
        resourceInfo.clear();
    }
}

template<class T>
void ResourceManager<T>::mapResource(Handle handle, std::string const& id, bool preCache) {
    resourceInfo[handle] = id;

    if (preCache && !find(handle)) {
        resourceData[handle] = new T();
		resourceData[handle]->load(id);
    }
}

template<class T>
bool ResourceManager<T>::isMapped(std::string id, Handle *handle) {
    typename std::map<int, std::string>::iterator i;

	for (i = resourceInfo.begin(); i != resourceInfo.end(); i++) {
        if (i->second == id) {
            if (handle)
                *handle = i->first;
            return true;
        }
    }
    return false;
}

}   //  namespace

#endif  //  RESOURCE_MANAGER_H
