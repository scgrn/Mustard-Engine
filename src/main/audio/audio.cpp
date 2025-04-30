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

extern FileSystem fileSystem;
extern Audio audio;

// TODO: hashmap of looping sounds, killAllLoopingSFX()

std::vector<ma_sound*> pausedSounds;

//  https://youtu.be/Vjm--AqG04Y
f32 dBToVolume(f32 dB) {
    return powf(10.0f, 0.05f * dB);
}

f32 volumeTodB(f32 volume) {
    return 20.0f * log10f(volume);
}

//----------------------------------------------------------------------------------------------------------------------------------

void Sound::load(std::string const& filename) {
    data = fileSystem.loadAsset(filename);

    for (u32 i = 0; i < INSTANCES; i++) {
         //  initialize the decoder with the memory data
        ma_result result = ma_decoder_init_memory(data.getData(), data.getSize(), NULL, &decoders[i]);
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
}

void Sound::play(f32 volume, f32 pan, b8 loop) {
    ma_sound_set_volume(&sounds[currentInstance], volume * audio.soundVolume);
    ma_sound_set_pan(&sounds[currentInstance], pan);
    ma_sound_set_looping(&sounds[currentInstance], loop);
    
    ma_sound_start(&sounds[currentInstance]);
    currentInstance = (currentInstance + 1) % INSTANCES;
}

void Sound::stop() {
    for (u32 i = 0; i < INSTANCES; i++) {
        if (ma_sound_is_playing(&sounds[i])) {
            ma_sound_stop(&sounds[i]);
        }
    }
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
    data = fileSystem.loadAsset(filename);

    ma_result result = ma_decoder_init_memory(data.getData(), data.getSize(), NULL, &decoder);
    if (result != MA_SUCCESS) {
        ERR("Failed to initialize decoder from memory: %d", result);
    }

    result = ma_sound_init_from_data_source(&audio.engine, &decoder, MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, &sound);

    if (result != MA_SUCCESS) {
        ERR("Failed to initialize sound from data source: %d", result);
    }
}

void Music::release() {
    ma_decoder_uninit(&decoder);
    ma_sound_uninit(&sound);
}

void Music::setLoopPoint(f32 loopPoint) {
    u32 loopPointInFrames = (u32)(loopPoint * decoder.outputSampleRate);
    ma_data_source_set_loop_point_in_pcm_frames(&decoder, loopPointInFrames, ~(ma_uint64)0);
}

void Music::play(b8 loop) {
    //  https://github.com/mackron/miniaudio/issues/714
    ma_sound_set_stop_time_in_milliseconds(&sound, ~(ma_uint64)0);
    ma_sound_set_fade_in_milliseconds(&sound, 0.0f, 1.0f, 1);

    ma_sound_seek_to_pcm_frame(&sound, 0);
    ma_sound_set_volume(&sound, audio.musicVolume);
    ma_sound_set_looping(&sound, loop);
    ma_sound_start(&sound);
}

void Music::pause() {
    ma_sound_stop(&sound);
}

void Music::resume() {
    ma_sound_start(&sound);
}

void Music::setVolume(f32 volume) {
    ma_sound_set_volume(&sound, volume * audio.musicVolume);
}

void Music::fadeIn(f32 duration) {
    play(true);

    u32 ms = (u32)(duration * 1000.0f);
    ma_sound_set_fade_in_milliseconds(&sound, 0, audio.musicVolume, ms);
}

void Music::fadeOut(f32 duration) {
    u32 ms = (u32)(duration * 1000.0f);
    ma_sound_stop_with_fade_in_milliseconds(&sound, ms);
}

void Music::stop() {
    ma_sound_seek_to_pcm_frame(&sound, 0);
    ma_sound_stop(&sound);
}

b8 Music::isPlaying() {
    return ma_sound_is_playing(&sound);
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

void Audio::pauseAll() {
    pausedSounds.clear();

    extern AssetManager<Music> music;
    for (const auto& pair : music.assetData) {
        if (pair.second->isPlaying()) {
            pair.second->pause();
            pausedSounds.push_back(&pair.second->sound);
        }
    }

    extern AssetManager<Sound> sounds;
    for (const auto& pair : sounds.assetData) {
        for (u32 i = 0; i < Sound::INSTANCES; i++) {
            if (ma_sound_is_playing(&pair.second->sounds[i])) {
                ma_sound_stop(&pair.second->sounds[i]);
                pausedSounds.push_back(&pair.second->sounds[i]);
            }
        }
    }
}

void Audio::resumeAll() {
    for (ma_sound* sound : pausedSounds) {
        ma_sound_start(sound);
    }
    pausedSounds.clear();
}

}   //  namespace

