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

#ifndef AB_RENDERER_H
#define AB_RENDERER_H

#include <vector>

#ifdef ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#else

#ifndef __EMSCRIPTEN__
#include "glad/glad.h"
#else
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#endif
#include <SDL2/SDL.h>
#endif

#include "../core/subsystem.h"

#include "camera.h"
#include "shader.h"
#include "textureCache.h"
#include "colorTransform.h"
#include "renderTarget.h"
#include "renderLayer.h"

namespace AB {

class Renderer : public SubSystem {
	public:
		std::map<int, RenderLayer*> layers;
		std::map<int, RenderTarget*> canvases;

		bool startup();
		void shutdown();
		
		void render(const Camera& camera);
		
		void clear(float r, float g, float b, float a);
		void renderFullscreenQuad();
		
		static TextureCache textureCache;
        static GLuint whiteTexture;

	private:
		// no samplers in uniform blocks :(
		struct Uniforms {
            Mat4 projectionMatrix;
			Mat4 viewMatrix;
			Mat4 colorTransform;
            int timer;
            float randomSeed;
		} uniforms;
		
		//std::vector<RenderCommand> commandQueue;

		GLuint ubo;		//	uniform buffer
		GLuint fullscreenQuadVAO;	
		
};

}

#endif
