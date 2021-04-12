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

static SoLoud::Soloud gSoloud; 
static std::map<int, SoLoud::Wav> sfx;
static std::map<int, SoLoud::WavStream> music;

// TODO: hashmap of looping sounds, killAllLoopingSFX()

#ifdef ANDROID

void initSound() {}
extern void loadSound(int index, std::string filename);
extern void playMusic();
extern void pauseMusic();
extern void playSound(int index, bool loop);
void stopSound(int index) {}
void closeSound() {}

#else
/*
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

static SoLoud::Soloud gSoloud; 
std::map<int, SoLoud::Wav> sfx;
std::map<int, SoLoud::WavStream> music;
*/
int musicHandle;


class Sample : public Resource {
    public:
        void load(std::string const& filename) {
            dataObject = new DataObject(filename.c_str(), &size);
        }

        void release() {
            delete dataObject;
        }

        DataObject *dataObject;
        unsigned int size;
};

ResourceManager<Sample> samples;


//  https://youtu.be/Vjm--AqG04Y
float dBToVolume(float dB) {
    return powf(10.0f, 0.05f * dB);
}

float volumeTodB(float volume) {
    return 20.0f * log10f(volume);
}

bool Audio::startup() {
	LOG("Audio subsystem startup", 0);

    gSoloud.init(); // Initialize SoLoud
	initialized = true;
	
	return true;
}

void Audio::shutdown() {
	LOG("Audio subsystem shutdown", 0);

    // TODO: iterate through both sfx and music hashmaps and call stop()
    //  ...see if that fixes mutex crash at shutdown

    stopMusic();
    gSoloud.deinit(); // Clean up!
}

void Audio::loadSound(int index, std::string filename) {
    filename = "assets/" + filename;
    sfx[index].load(filename.c_str());
    // TODO: error check
}

void Audio::loadMusic(int index, std::string filename, float loopPoint) {
    filename = "assets/" + filename;
    music[index].load(filename.c_str());
    music[index].setLoopPoint(loopPoint);
    music[index].setLooping(true);
    music[index].setSingleInstance(true);

    // TODO: error check
}

void Audio::playMusic(int index) {
    LOG("playMusic() called, index: %d", index);

    musicHandle = gSoloud.play(music[index]);
    gSoloud.seek(musicHandle, 0.0f);
    gSoloud.setVolume(musicHandle, musicVolume);
}

void Audio::pauseMusic() {
    gSoloud.setPause(musicHandle, true);
}

void Audio::resumeMusic() {
    gSoloud.setPause(musicHandle, false);
}

void Audio::fadeMusicIn(int index, float duration) {
    LOG("fadeMusicIn() called, index: %d", index);

    musicHandle = gSoloud.play(music[index]);
    gSoloud.seek(musicHandle, 0.0f);
    gSoloud.setVolume(musicHandle, 0.0f);
    gSoloud.fadeVolume(musicHandle, musicVolume, duration);
}

void Audio::fadeMusicOut(float duration) {
    gSoloud.fadeVolume(musicHandle, 0, duration);
    gSoloud.scheduleStop(musicHandle, duration);
}

void Audio::stopMusic() {
    gSoloud.stop(musicHandle);
}

void Audio::playSound(int index, bool loop, float volume) {
    if (soundVolume > 0.01f) {
        sfx[index].stop();
        int handle = gSoloud.playClocked(1.0f / 60.0f, sfx[index]);
        gSoloud.setVolume(handle, volume * soundVolume);
#ifdef DEBUG
        extern void recordSFX(int index);
        recordSFX(index);
#endif
    }
}

void Audio::stopSound(int index) {
    sfx[index].stop();
}

bool Audio::isPlaying(int index) {
	// TODO: how?
	//return (!sfx[index].hasEnded());

	return false;
}

void Audio::setSoundVolume(float volume) {
    soundVolume = volume;
}

void Audio::setMusicVolume(float volume) {
    musicVolume = volume;
    gSoloud.setVolume(musicHandle, musicVolume);
}

#endif	// desktop

}   //  namespace
