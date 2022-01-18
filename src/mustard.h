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

#ifndef AB_H
#define AB_H

#include "pch.h"

#include "core/application.h"

#include "audio/audio.h"
#include "script/script.h"
#include "input/input.h"
#include "core/fileSystem.h"
#include "core/window.h"
#include "core/log.h"
#include "misc/console.h"

#include "renderer/renderer.h"
#include "renderer/sprite.h"
#include "renderer/palette.h"
#include "renderer/font.h"
#include "renderer/renderTarget.h"
#include "renderer/colorTransform.h"
#include "renderer/batchRenderer.h"
#include "renderer/skybox.h"
#include "renderer/model.h"

#include "math/random.h"
#include "math/perlin.h"
#include "misc/misc.h"

namespace AB {
	extern FileSystem fileSystem;
	extern Renderer renderer;
	extern Audio audio;
	extern Script script;
	extern Input input;
	extern Window window;
#ifdef DEBUG
	extern Console console;
#endif	

	void startup(Application *app);
	void shutdown();
	
	extern ResourceManager<Sprite> sprites;
	extern ResourceManager<Shader> shaders;
	extern ResourceManager<Font> fonts;

	extern ResourceManager<Sound> sounds;
	extern ResourceManager<Music> music;
	
	extern OrthographicCamera camera2d;
}

#endif // AB_H
