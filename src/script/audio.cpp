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

//----------------------------------------------------------------- Sfx / music --------------------------------

/// Loads a sound effect for later playback
// @param index handle to sound effect
// @param filename wav filename
// @function AB.audio.loadSound
static int luaLoadSound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

	
    sounds.mapResource(index, filename, true);
    // bool result = loadSound(index, filename);
    // lua_pushboolean(luaVM, result);

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

static int luaIsPlaying(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, sounds.get(index)->isPlaying());
	
	return 1;
}

static int luaLoadMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

    float loopPoint = 0.0f;
    if (lua_gettop(luaVM) >= 4) {
        int beats = (int)lua_tonumber(luaVM, 3);
        int BPM = (int)lua_tonumber(luaVM, 4);

		float BPS = (float)BPM / 60.0f;

        loopPoint = (1.0f / BPS) * (beats * 2);
    }

	music.mapResource(index, filename, true);
	if (loopPoint > 0) {
		music.get(index)->setLoopPoint(loopPoint);
	}

    return 0;
}

static int luaPlayMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	music.get(index)->play();

    return 0;
}

static int luaPauseMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	music.get(index)->pause();

    return 0;
}

static int luaResumeMusic(lua_State* luaVM) {
	audio.currentMusic->resume();
	
    return 0;
}

static int luaFadeMusicIn(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    float duration = (float)lua_tonumber(luaVM, 2);
    
	music.get(index)->fadeIn(duration);

    return 0;
}

static int luaFadeMusicOut(lua_State* luaVM) {
	float duration = (float)lua_tonumber(luaVM, 1);
	audio.currentMusic->fadeOut(duration);
	
    return 0;
}

static int luaStopMusic(lua_State* luaVM) {
    audio.currentMusic->stop();

    return 0;
}

static int luaSetSoundVolume(lua_State* luaVM) {
    float volume = (float)lua_tonumber(luaVM, 1);
    audio.soundVolume = volume;

    return 0;
}

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
		{ "isPlaying", luaIsPlaying},

        { "loadMusic", luaLoadMusic},
        { "playMusic", luaPlayMusic},
        { "pauseMusic", luaPauseMusic},
        { "resumeMusic", luaResumeMusic},
        { "fadeMusicIn", luaFadeMusicIn},
        { "fadeMusicOut", luaFadeMusicOut},
        { "stopMusic", luaStopMusic},

        { "setSoundVolume", luaSetSoundVolume},
        { "setMusicVolume", luaSetMusicVolume},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "audio", audioFuncs);
}

}
