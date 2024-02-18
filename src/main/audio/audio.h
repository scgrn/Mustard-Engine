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

#include "../core/subsystem.h"
#include "../core/fileSystem.h"
#include "../core/resourceManager.h"

namespace SoLoud {
    class Soloud;
    class Wav;
    class WavStream;
};

namespace AB {

class Sound : public Resource {
    public:
        Sound() {};
        virtual ~Sound() {};
        
        void load(std::string const& filename);
        void release();
        
        i32 play(float volume = 1.0f, float pan = 0.0f, bool loop = false);
        void stop();
        bool isPlaying();
        
    protected:
        SoLoud::Wav *wav;
        DataObject *data;

};

class Music : public Resource {
    public:
        void load(std::string const& filename);
        void release();
        
        void setLoopPoint(float loopPoint);
        void play(bool loop = true);
        void pause();
        void resume();
        void setVolume(float volume);
        void fadeIn(float duration);
        void fadeOut(float duration);
        void stop();
        bool isPlaying();
        
    protected:
        SoLoud::WavStream *wavStream;
        DataObject *data;
        int musicHandle;
        
};

class Audio : public SubSystem {
    public:
        bool startup() override;
        void shutdown() override;
        
        //    plays all queued sound effects
        void update();
        
        //    queues a sound effect if it hasn't already been played this frame
        void play(Sound *sound, float volume, float pan, bool loop);
        
        float soundVolume = 1.0f;
        float musicVolume = 1.0f;

        SoLoud::Soloud *soloud;
        
    private:
        struct QueuedSound {
            Sound* sound;
            float volume;
            float pan;
            bool loop;
        };
        
        std::vector<QueuedSound> soundQueue;

};

}

#endif
