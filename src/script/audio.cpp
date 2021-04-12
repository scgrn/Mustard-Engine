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

//----------------------------------------------------------------- Sfx / music --------------------------------

/// Loads a sound effect for later playback
// @param index handle to sound effect
// @param filename wav filename
// @function AB.audio.loadSound
static int luaLoadSound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

    audio.loadSound(index, filename);
    // bool result = loadSound(index, filename);
    // lua_pushboolean(luaVM, result);

    //  return handle
    lua_pushnumber(luaVM, index);

    return 1;
}

/// Plays a sound effect
// @param index sound effect handle
// @param loop (default false) whether to loop
// @param volume (default 1.0) volume [0..1]
// @function AB.audio.playSound
static int luaPlaySound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

    bool loop = false;
    float volume = 1.0f;
    if (lua_gettop(luaVM) >= 2) {
        loop = (bool)lua_toboolean(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
        volume = (float)lua_tonumber(luaVM, 3);
    }
    audio.playSound(index, loop, volume);

    return 0;
}

/// Stops a sound effect
// @param index sound effect handle
// @function AB.audio.stopSound
static int luaStopSound(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    audio.stopSound(index);

    return 0;
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

    audio.loadMusic(index, filename, loopPoint);

    return 0;
}

static int luaPlayMusic(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    audio.playMusic(index);

    return 0;
}

static int luaPauseMusic(lua_State* luaVM) {
    audio.pauseMusic();

    return 0;
}

static int luaResumeMusic(lua_State* luaVM) {
    audio.resumeMusic();

    return 0;
}

static int luaFadeMusicIn(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    float duration = (float)lua_tonumber(luaVM, 2);
    audio.fadeMusicIn(index, duration);

    return 0;
}

static int luaFadeMusicOut(lua_State* luaVM) {
    float duration = (float)lua_tonumber(luaVM, 1);
    audio.fadeMusicOut(duration);

    return 0;
}

static int luaStopMusic(lua_State* luaVM) {
    audio.stopMusic();

    return 0;
}

static int luaSetSoundVolume(lua_State* luaVM) {
    float volume = (float)lua_tonumber(luaVM, 1);
    audio.setSoundVolume(volume);

    return 0;
}

static int luaSetMusicVolume(lua_State* luaVM) {
    float volume = (float)lua_tonumber(luaVM, 1);
    audio.setMusicVolume(volume);

    return 0;
}

void registerAudioFunctions() {
    static const luaL_Reg audioFuncs[] = {
        { "loadSound", luaLoadSound},
        { "playSound", luaPlaySound},
        { "stopSound", luaStopSound},

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
