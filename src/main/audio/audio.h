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

#ifndef AUDIO_H
#define AUDIO_H

#include "../../vendor/miniaudio/miniaudio.h"

#include "../core/subsystem.h"
#include "../core/fileSystem.h"
#include "../core/assetManager.h"

namespace AB {

class Sound : public Asset {
    public:
        static const u32 INSTANCES = 8;
        
        Sound() {};
        virtual ~Sound() {};
        
        void load(std::string const& filename);
        void release();
        
        void play(f32 volume = 1.0f, f32 pan = 0.0f, b8 loop = false);
        void stop();
        b8 isPlaying();
        
    protected:
        DataObject *data;
        ma_sound sounds[INSTANCES];
        ma_decoder decoders[INSTANCES];
        u32 currentInstance;
};

class Music : public Asset {
    public:
        void load(std::string const& filename);
        void release();
        
        void setLoopPoint(f32 loopPoint);
        void play(b8 loop = true);
        void pause();
        void resume();
        void setVolume(f32 volume = 1.0f);
        void fadeIn(f32 duration);
        void fadeOut(f32 duration);
        void stop();
        b8 isPlaying();
        
    protected:
        DataObject *data;
        ma_sound sound;
        ma_decoder decoder;
};

class Audio : public SubSystem {
    public:
        b8 startup() override;
        void shutdown() override;
        
        //    plays all queued sound effects
        void update();
        
        //    queues a sound effect if it hasn't already been played this frame
        void play(Sound *sound, f32 volume, f32 pan, b8 loop);
        
        f32 soundVolume = 1.0f;
        f32 musicVolume = 1.0f;

        ma_engine engine;
        
    private:
        struct QueuedSound {
            Sound* sound;
            f32 volume;
            f32 pan;
            b8 loop;
        };
        
        std::vector<QueuedSound> soundQueue;
};

}

#endif
