 /**

zlib License

(C) 2021 Andrew Krause

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

#ifndef AB_BATCH_RENDERER_H
#define AB_BATCH_RENDERER_H

#include "math.h"
#include "renderer.h"

namespace AB {

class BatchRenderer : public Renderer {
	public:
		BatchRenderer(Shader *shader = &defaultShader, Mat4 colorTransform = Mat4(), bool depthSorting = false);
		~BatchRenderer();

		virtual void beginScene(const Camera& camera);
		virtual void endScene();
		
		void renderQuad(const Quad& quad);

		std::vector<Quad> renderBatch;
		Shader *shader;
		bool depthSorting;
		Mat4 colorTransform;

		static Shader defaultShader;

	private:
		//	for std::sort
		static inline bool cmp(const Quad& a, const Quad& b) {
			return a.textureID < b.textureID;
		}

		static inline bool cmpDepth(const Quad& a, const Quad& b) {
			if (a.pos.z == b.pos.z) {
				return a.textureID < b.textureID;
			} else {
				return a.pos.z < b.pos.z;
			}
		}
		
		void flush(int begin, int end);

		GLuint vao;		//	vertex array object
		GLuint vbo;		//	vertex buffer
		GLuint ebo;		//	element buffer
		GLuint ibo;		//	per-instance vertex buffer

};

}

#endif
