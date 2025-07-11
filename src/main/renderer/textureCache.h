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

#ifndef AB_TEXTURE_CACHE_H
#define AB_TEXTURE_CACHE_H

namespace AB {

class TextureCache {
    public:
        TextureCache();
        ~TextureCache()  {}

        //    returns texture unit or -1 if no slot available
        //    will not evict textures bound after the last call to advanceFrame
        //    reserve is not currently used
        i32 bindTexture(u32 textureID, bool reserve = false);
        
        //  resets all slots to fair game
        void advanceFrame();
        
        //    remove a texture from the cache while unbinding it
        //    mostly for unbinding renderTargets
        //    this will not complain if the texture isn't in the cache
        void evictTexture(u32 textureID);
        void invalidate();
        
    private:
        u32 frameID;
        
        //    meh, maybe query for max. for now the spec min is fine.
        static const u32 MAX_TEXTURE_UNITS = 16;
        
        struct CachedTexture {
            CachedTexture() {}
            CachedTexture(u32 textureID, u32 lastFrame) : textureID(textureID), lastFrame(lastFrame) {}

            u32 textureID;
            u32 lastFrame;
        } textureBindings[MAX_TEXTURE_UNITS];

};

}

#endif
