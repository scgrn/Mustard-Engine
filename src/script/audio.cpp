/*

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

*/

/**
Audio functions
*/

#include "../pch.h"

#include "script.h"
#include "../audio/audio.h"

namespace AB {

extern Script script;
extern Audio audio;

extern ResourceManager<Sound> sounds;
extern ResourceManager<Music> music;

static int sfxHandle = 1;
static int musicHandle = 1;

//----------------------------------------------------------------- Sfx / music --------------------------------

/// Loads a sound effect for later playback
// @function AB.audio.loadSound
// @param filename WAV filename
// @param index (optional) sound effect handle
// @return handle to sound effect
static int luaLoadSound(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));

	int index;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    } else {
		index = sfxHandle;
		sfxHandle++;
	}
	
	sounds.mapResource(index, filename, true);

	//  return handle
	lua_pushnumber(luaVM, index);

    return 1;
}

/// Plays a sound effect
// @param index sound effect handle
// @param volume (default 1.0) volume [0..1]
// @param pan (default 0.0) pan [-1..1]
// @param loop (default false) whether to loop
// @function AB.audio.playSound
static int luaPlaySound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    float volume = 1.0f;
    float pan = 0.0f;
    bool loop = false;

    if (lua_gettop(luaVM) >= 2) {
        volume = (float)lua_tonumber(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
        pan = (float)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
        loop = (bool)lua_toboolean(luaVM, 4);
    }
	sounds.get(index)->play(volume, pan, loop);
	
    return 0;
}

/// Stops a sound effect
// @param index sound effect handle
// @function AB.audio.stopSound
static int luaStopSound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	sounds.get(index)->stop();

    return 0;
}

/// Checks if a sound effect is currently playing
// @function AB.audio.isSoundPlaying
// @param index sound effect handle
// @return playing
static int luaIsSoundPlaying(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, sounds.get(index)->isPlaying());
	
	return 1;
}

/// Loads a music loop for later playback
// @function AB.audio.loadMusic
// @param filename OGG filename
// @param loopPointBeats (0) Number of beats to set loop point
// @param BPM (optional, must be included to compute non-zero loop point) Beats per minute of music
// @param index (optional) music handle
// @return music loop handle
static int luaLoadMusic(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));

    float loopPoint = 0.0f;
    if (lua_gettop(luaVM) >= 2) {
        int beats = (int)lua_tonumber(luaVM, 2);
        int BPM = (int)lua_tonumber(luaVM, 3);

		float BPS = (float)BPM / 60.0f;

		// TODO: figure out (shouldn't it be 4? why does 2 work?)
        loopPoint = (1.0f / BPS) * (beats * 2);
    }

	int index;
    if (lua_gettop(luaVM) >= 4) {
        index = (int)lua_tonumber(luaVM, 4);
    } else {
		index = musicHandle;
		musicHandle++;
	}
	
	music.mapResource(index, filename, true);
	if (loopPoint > 0) {
		music.get(index)->setLoopPoint(loopPoint);
	}

    //  return handle
    lua_pushnumber(luaVM, index);

    return 1;
}

/// Plays a music loop
// @function AB.audio.playMusic
// @param index Music loop handle
static int luaPlayMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	
    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->play();

    return 0;
}

/// Pauses a music loop
// @function AB.audio.pauseMusic
// @param index Music loop handle
static int luaPauseMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->pause();

    return 0;
}

/// Resumes a paused music loop
// @function AB.audio.resumeMusic
// @param index Music loop handle
static int luaResumeMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->resume();
	
    return 0;
}

/// Fades a music loop in
// @function AB.audio.fadeMusicIn
// @param index Music loop handle
// @param duration Duration in seconds
static int luaFadeMusicIn(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    float duration = (float)lua_tonumber(luaVM, 2);
    
    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->fadeIn(duration);

    return 0;
}

/// Fades a music loop out
// @function AB.audio.fadeMusicOut
// @param index Music loop handle
// @param duration Duration in seconds
static int luaFadeMusicOut(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	float duration = (float)lua_tonumber(luaVM, 2);

    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->fadeOut(duration);
	
    return 0;
}

/// Stops a music loop
// @function AB.audio.stopMusic
// @param index Music loop handle
static int luaStopMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
	music.get(index)->stop();
	
    return 0;
}

/// Checks if a music loop is currently playing
// @function AB.audio.isMusicPlaying
// @param index Music loop handle
// @return playing
static int luaIsMusicPlaying(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    if (!music.find(index)) {
        if (music.resourceInfo[index].empty()) {
			LOG("Unknown music loop: %d", index);
			return 0;
		}
	}
    lua_pushboolean(luaVM, music.get(index)->isPlaying());
	
	return 1;
}

/// Sets global sound effect volume
// @function AB.audio.setSoundVolume
// @param volume Sound effect volume
static int luaSetSoundVolume(lua_State* luaVM) {
    float volume = (float)lua_tonumber(luaVM, 1);
    audio.soundVolume = volume;

    return 0;
}

/// Sets global music volume
// @function AB.audio.setMusicVolume
// @param volume Music volume
static int luaSetMusicVolume(lua_State* luaVM) {
    float volume = (float)lua_tonumber(luaVM, 1);
    audio.musicVolume = volume;

    return 0;
}

void registerAudioFunctions() {
    static const luaL_Reg audioFuncs[] = {
        { "loadSound", luaLoadSound},
        { "playSound", luaPlaySound},
        { "stopSound", luaStopSound},
		{ "isSoundPlaying", luaIsSoundPlaying},

        { "loadMusic", luaLoadMusic},
        { "playMusic", luaPlayMusic},
        { "pauseMusic", luaPauseMusic},
        { "resumeMusic", luaResumeMusic},
        { "fadeMusicIn", luaFadeMusicIn},
        { "fadeMusicOut", luaFadeMusicOut},
        { "stopMusic", luaStopMusic},
		{ "isMusicPlaying", luaIsMusicPlaying},

        { "setSoundVolume", luaSetSoundVolume},
        { "setMusicVolume", luaSetMusicVolume},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "audio", audioFuncs);
}

}
