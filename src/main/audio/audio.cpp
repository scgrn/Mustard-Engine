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

#include "../pch.h"

#define MINIAUDIO_IMPLEMENTATION

#include "audio.h"
#include "../core/log.h"

namespace AB {

extern Audio audio;

// TODO: hashmap of looping sounds, killAllLoopingSFX()

//  https://youtu.be/Vjm--AqG04Y
f32 dBToVolume(f32 dB) {
    return powf(10.0f, 0.05f * dB);
}

f32 volumeTodB(f32 volume) {
    return 20.0f * log10f(volume);
}

//----------------------------------------------------------------------------------------------------------------------------------

void Sound::load(std::string const& filename) {
    data = new DataObject(filename.c_str());

    for (u32 i = 0; i < INSTANCES; i++) {
         //  initialize the decoder with the memory data
        ma_result result = ma_decoder_init_memory(data->getData(), data->getSize(), NULL, &decoders[i]);
        if (result != MA_SUCCESS) {
            ERR("Failed to initialize decoder from memory: %d", result);
        }

        //  initialize multiple sound objects using the same data source
        result = ma_sound_init_from_data_source(&audio.engine, &decoders[i],
            MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_DECODE, NULL, &sounds[i]);

        if (result != MA_SUCCESS) {
            ERR("Failed to initialize sound from data source: %d", result);
        }
    }

    currentInstance = 0;
}

void Sound::release() {
    for (u32 i = 0; i < INSTANCES; i++) {
        ma_sound_uninit(&sounds[i]);
        ma_decoder_uninit(&decoders[i]);
    }
    
    delete data;
}

i32 Sound::play(f32 volume, f32 pan, b8 loop) {
    ma_sound_set_volume(&sound[currentInstance], volume * audio.soundVolume);
    ma_sound_set_pan(&sound[currentInstance], pan);
    ma_sound_set_looping(&sound[currentInstance], loop);
    
    ma_sound_start(&sounds[currentInstance]);
    currentInstance = (currentInstance + 1) % INSTANCES;

    return 0;
}

void Sound::stop() {
    //wav->stop();
}

b8 Sound::isPlaying() {
    for (u32 i = 0; i < INSTANCES; i++) {
        if (ma_sound_is_playing(&sounds[i])) {
            return true;
        }
    }
    
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------

void Music::load(std::string const& filename) {
    data = new DataObject(filename.c_str());
/*
    wavStream = new SoLoud::WavStream();
    wavStream->loadMem(data->getData(), data->getSize(), false, false);
    
    wavStream->setLooping(true);
    wavStream->setSingleInstance(true);
*/
}

void Music::release() {
//    delete wavStream;
}

void Music::setLoopPoint(f32 loopPoint) {
//    wavStream->setLoopPoint(loopPoint);
}

void Music::play(b8 loop) {
/*
    wavStream->setLooping(loop);
    musicHandle = audio.soloud->play(*wavStream);
    audio.soloud->seek(musicHandle, 0.0f);
    audio.soloud->setVolume(musicHandle, audio.musicVolume);
*/
}

void Music::pause() {
//    audio.soloud->setPause(musicHandle, true);
}

void Music::resume() {
//    audio.soloud->setPause(musicHandle, false);
}

void Music::setVolume(f32 volume) {
//    audio.soloud->setVolume(musicHandle, audio.musicVolume);
}

void Music::fadeIn(f32 duration) {
/*
    musicHandle = audio.soloud->play(*wavStream);
    audio.soloud->seek(musicHandle, 0.0f);
    audio.soloud->setVolume(musicHandle, 0.0f);
    audio.soloud->fadeVolume(musicHandle, audio.musicVolume, duration);
*/
}

void Music::fadeOut(f32 duration) {
//    audio.soloud->fadeVolume(musicHandle, 0, duration);
//    audio.soloud->scheduleStop(musicHandle, duration);
}

void Music::stop() {
//    audio.soloud->stop(musicHandle);
}

b8 Music::isPlaying() {
//    return audio.soloud->countAudioSource(*wavStream) > 0;
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------

b8 Audio::startup() {
    LOG("Audio subsystem startup", 0);

    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return false;
    }
    
    //    TODO: print device info

    initialized = true;
    
    return true;
}

void Audio::shutdown() {
    LOG("Audio subsystem shutdown", 0);

    extern AssetManager<Sound> sounds;
    extern AssetManager<Music> music;

    sounds.clear();
    music.clear();

    ma_engine_uninit(&engine);
}

void Audio::update() {
    for (auto& queuedSound : soundQueue) {
        queuedSound.sound->play(queuedSound.volume, queuedSound.pan, queuedSound.loop);
    }
    soundQueue.clear();
}

void Audio::play(Sound *sound, f32 volume = 1.0f, f32 pan = 0.0f, b8 loop = false) {
    b8 found = false;
    for (auto& queuedSound : soundQueue) {
        if (queuedSound.sound == sound) {
            found = true;
            
            if (volume > queuedSound.volume) {
                queuedSound.volume = volume;
                queuedSound.pan = pan;
            }
            
            break;
        }
    }
    if (!found) {
        QueuedSound queuedSound;
        queuedSound.sound = sound;
        queuedSound.volume = volume;
        queuedSound.pan = pan;
        queuedSound.loop = loop;
        
        soundQueue.push_back(queuedSound);
    }
}

}   //  namespace

