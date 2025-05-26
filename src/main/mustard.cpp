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

#include "mustard.h"
#include "core/version.h"

namespace AB {

// each subsystem has a single global instance
FileSystem fileSystem;
Renderer renderer;
Audio audio;
Script script;
Input input;
Window window;
#if defined(DEBUG) && !defined(__EMSCRIPTEN__)
Console console;
#endif

AssetManager<Sprite> sprites;
AssetManager<Shader> shaders;
AssetManager<Font> fonts;
AssetManager<Sound> sounds;
AssetManager<Music> music;

std::map<int, RenderLayer*> batchRenderers;
std::map<int, RenderTarget*> canvases;

OrthographicCamera camera2d;

void startup(Application *app) {
    LOG("Engine Startup - %s - %s", VERSION, BUILD_STAMP);
    LOG(std::string(79, '-').c_str(), 0);

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        ERR("Unable to initialize SDL: %s", SDL_GetError());
    }
    
    LOG("\tPlatform: %s", SDL_GetPlatform());
    LOG("\tCPU count: %d", SDL_GetCPUCount());
    LOG("\tSystem RAM: %dMB", SDL_GetSystemRAM());
    
    fileSystem.startup();
    script.startup();
    input.startup();
    audio.startup();
    window.startup(app);
    renderer.startup();
#if defined(DEBUG) && !defined(__EMSCRIPTEN__)
    console.startup();
#endif
}

void shutdown() {
    shaders.clear(true);
    sprites.clear(true);
    fonts.clear(true);
    sounds.clear(true);
    music.clear(true);

    LOG("Engine Shutdown - Total runtime %dms", SDL_GetTicks());

#if defined(DEBUG) && !defined(__EMSCRIPTEN__)
    console.shutdown();
#endif
    renderer.shutdown();
    window.shutdown();
    audio.shutdown();
    input.shutdown();
    script.shutdown();
    fileSystem.shutdown();

    SDL_Quit();

    LOG("Goodbye.", 0);
}

}
