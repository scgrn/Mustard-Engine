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

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include "audio.h"

#include "../core/log.h"
#include "../core/resourceManager.h"

namespace AB {

extern Audio audio;

// TODO: hashmap of looping sounds, killAllLoopingSFX()

//  https://youtu.be/Vjm--AqG04Y
float dBToVolume(float dB) {
    return powf(10.0f, 0.05f * dB);
}

float volumeTodB(float volume) {
    return 20.0f * log10f(volume);
}

//----------------------------------------------------------------------------------------------------------------------------------

void Sound::load(std::string const& filename) {
    data = new DataObject(filename.c_str());

    wav = new SoLoud::Wav();
    wav->loadMem(data->getData(), data->getSize(), false, false);
}

void Sound::release() {
    delete data;
    delete wav;
}

i32 Sound::play(float volume, float pan, bool loop) {
    wav->setLooping(loop);
    if (audio.soundVolume > 0.01f) {
        i32 handle = audio.soloud->playClocked(1.0f / 60.0f, *wav, volume * audio.soundVolume, pan);
        
        return handle;
    } else {
        return 0;
    }
}

void Sound::stop() {
    wav->stop();
}

bool Sound::isPlaying() {
    return audio.soloud->countAudioSource(*wav) > 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

void Music::load(std::string const& filename) {
    data = new DataObject(filename.c_str());
    wavStream = new SoLoud::WavStream();
    wavStream->loadMem(data->getData(), data->getSize(), false, false);
    
    wavStream->setLooping(true);
    wavStream->setSingleInstance(true);
}

void Music::release() {
    delete wavStream;
}

void Music::setLoopPoint(float loopPoint) {
    wavStream->setLoopPoint(loopPoint);
}

void Music::play(bool loop) {
    wavStream->setLooping(loop);
    musicHandle = audio.soloud->play(*wavStream);
    audio.soloud->seek(musicHandle, 0.0f);
    audio.soloud->setVolume(musicHandle, audio.musicVolume);
}

void Music::pause() {
    audio.soloud->setPause(musicHandle, true);
}

void Music::resume() {
    audio.soloud->setPause(musicHandle, false);
}

void Music::setVolume(float volume) {
    audio.soloud->setVolume(musicHandle, audio.musicVolume);
}

void Music::fadeIn(float duration) {
    musicHandle = audio.soloud->play(*wavStream);
    audio.soloud->seek(musicHandle, 0.0f);
    audio.soloud->setVolume(musicHandle, 0.0f);
    audio.soloud->fadeVolume(musicHandle, audio.musicVolume, duration);
}

void Music::fadeOut(float duration) {
    audio.soloud->fadeVolume(musicHandle, 0, duration);
    audio.soloud->scheduleStop(musicHandle, duration);
}

void Music::stop() {
    audio.soloud->stop(musicHandle);
}

bool Music::isPlaying() {
    return audio.soloud->countAudioSource(*wavStream) > 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

bool Audio::startup() {
    LOG("Audio subsystem startup", 0);

    soloud = new SoLoud::Soloud();
    soloud->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::MINIAUDIO, SoLoud::Soloud::AUTO, SoLoud::Soloud::AUTO);
    
    //    check for error
    
    //    print device info
    
    
    initialized = true;
    
    return true;
}

void Audio::shutdown() {
    LOG("Audio subsystem shutdown", 0);

    extern ResourceManager<Sound> sounds;
    extern ResourceManager<Music> music;

    sounds.clear();
    music.clear();
    
    soloud->deinit(); // Clean up!
    delete soloud;
}

void Audio::update() {
    for (auto& queuedSound : soundQueue) {
        queuedSound.sound->play(queuedSound.volume, queuedSound.pan, queuedSound.loop);
    }
    soundQueue.clear();
}

void Audio::play(Sound *sound, float volume = 1.0f, float pan = 0.0f, bool loop = false) {
    bool found = false;
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
