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


namespace AB {
	
class Renderer : public SubSystem {
	public:
		// a 64 svelte bytes
		// TODO: constructor to set defaults
		struct RenderQuad {
			glm::vec3 pos;
			glm::vec2 size;
			float scale;
			float rotation;		// radians
			glm::vec4 uv;		// {u1, v1, u2, v2}
			GLint textureID;	// set to 0 for white texture
			glm::vec4 color;		
		};

		bool startup();
		void shutdown();
		
		// typically called once per frame from the main game loop.
		// only needed per perspective changes, ie 3d game scene followed by a 2d overlay
		// basically just for top-level compositing.
		void beginScene(const Camera& camera);
		
		// pass in state, render command (VAO, primitive type)
		// void submit(const RenderCommand& command);
		
		void defineRenderGroup(int index, Shader *shader, glm::mat4 colorTransform = glm::mat4(1.0f), bool depthSorting = false);
		
		void renderQuad(const int renderGroup, const RenderQuad& quad);
		void renderQuad(const RenderQuad& quad);

		//	renders the scene.
		void endScene();
		
		TextureCache textureCache;
		Shader defaultShader;

		
	private:
		//	for std::sort
		static inline bool cmp(const RenderQuad& a, const RenderQuad& b) {
			return a.textureID < b.textureID;
		}

		static inline bool cmpDepth(const RenderQuad& a, const RenderQuad& b) {
			if (a.pos.z == b.pos.z) {
				return a.textureID < b.textureID;
			} else {
				return a.pos.z < b.pos.z;
			}
		}
		
		GLuint vao;		//	vertex array object
		GLuint vbo;		//	vertex buffer
		GLuint ebo;		//	element buffer
		GLuint ibo;		//	per-instance vertex buffer
		GLuint ubo;		//	uniform buffer

		// no samplers in uniform blocks :(
		struct Uniforms {
            glm::mat4 projectionMatrix;
			glm::mat4 viewMatrix;
			glm::mat4 colorTransform;
            int timer;
            float randomSeed;
		} uniforms;

		struct RenderGroup {
			std::vector<RenderQuad> renderBatch;
			Shader *shader;
			bool depthSorting;
			glm::mat4 colorTransform;
		};
		std::unordered_map<int, RenderGroup> renderGroups;
		
		void flush(RenderGroup *renderGroup, int begin, int end);

		//std::vector<RenderCommand> commandQueue;
		bool inScene = false;

        GLuint whiteTexture;

};

}

#endif
