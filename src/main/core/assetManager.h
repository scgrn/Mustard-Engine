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

    @file AssetManager.h
    @author Andrew Krause - contact@alienbug.net
    @date 08.11.11

    Provides a template and management for Assets.
*/

#ifndef AB_ASSET_MANAGER_H
#define AB_ASSET_MANAGER_H

#include <iostream>
#include <map>

#include "fileSystem.h"
#include "log.h"

namespace AB {

class Asset {
    public:
        //  represents Asset's eligibility for garbage collection.
        enum {PERMANENT, TEMPORARY, MOMENTARY} lifeSpan;

        virtual ~Asset() {};
        virtual void load(std::string const& id) = 0;
        virtual void release() = 0;
};

template<class T>
class AssetManager {
    public:
        typedef int Handle;

        AssetManager();
        ~AssetManager();

        /**
            returns a pointer to a Asset based on it's handle.
            the Asset's load function will be called if it's not loaded.
        */
        T* get(Handle handle);

        //  returns whether a Asset exists without loading it if it doesn't
        bool find(Handle handle);

        //  clears it.
        void clear(bool clearInfoMap = false);

        //  not yet implemented. will iterate through all Assets
        //  and release those whose ref-count is 0
        void collect();

        //  sets an entry in a Asset handle/id table
        //  (id will typically represent a filename)
        void mapAsset(Handle handle, std::string const& id, bool preCache = false);

        //  returns whether an id string is mapped to a handle
        //  its Handle will be returned in handle if it is
        bool isMapped(std::string id, Handle *handle = 0);


        void precacheAll();

//    private:
        //  this is a list of the pointers to the actual asset data
        std::map<Handle, T*> assetData;

        //  this maps asset handles to ids (typically filenames)
        std::map<Handle, std::string> assetInfo;
};

template<class T>
AssetManager<T>::AssetManager() {};

template<class T>
AssetManager<T>::~AssetManager() {
    clear(true);
};

template<class T>
bool AssetManager<T>::find(Handle handle) {
    return assetData.find(handle) != assetData.end();
};

template<class T>
T* AssetManager<T>::get(Handle handle) {
    //  load the asset if it doesn't exist
    if (!find(handle)) {
        if (assetInfo[handle].empty()) {
            ERR("Asset %d not mapped!", handle);
        }

        assetData[handle] = new T();
        assetData[handle]->load(assetInfo[handle]);
    }

    return assetData[handle];
};

template<class T>
void AssetManager<T>::precacheAll() {
    typename std::map<int, std::string>::iterator i;

    for (i = assetInfo.begin(); i != assetInfo.end(); i++) {
        get(i->first);
    }
}

template<class T>
void AssetManager<T>::collect() {
}

//  TODO: check if already cleared to avoid SEGFAULT
template<class T>
void AssetManager<T>::clear(bool clearInfoMap) {
    //  call release() on all Assets.
    for (typename std::map<Handle, T*>::iterator i = assetData.begin();
        i != assetData.end(); i++) {

        i->second->release();
        delete i->second;
    }
    assetData.clear();

    if (clearInfoMap) {
        assetInfo.clear();
    }
}

template<class T>
void AssetManager<T>::mapAsset(Handle handle, std::string const& id, bool preCache) {
    assetInfo[handle] = id;

    if (preCache && !find(handle)) {
        assetData[handle] = new T();
        assetData[handle]->load(id);
    }
}

template<class T>
bool AssetManager<T>::isMapped(std::string id, Handle *handle) {
    typename std::map<int, std::string>::iterator i;

    for (i = assetInfo.begin(); i != assetInfo.end(); i++) {
        if (i->second == id) {
            if (handle)
                *handle = i->first;
            return true;
        }
    }
    return false;
}

}   //  namespace

#endif  //  ASSET_MANAGER_H

