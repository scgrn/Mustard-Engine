/**

zlib License

(C) 2022 Andrew Krause

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

#ifndef AB_GEOMETRY_RENDERER_H
#define AB_GEOMETRY_RENDERER_H

#include "math.h"
#include "renderer.h"
#include "renderLayer.h"

namespace AB {

class GeometryRenderer : public RenderLayer {
	public:
		GeometryRenderer(Shader *shader = &defaultShader, Mat4 colorTransform = Mat4());
		~GeometryRenderer();
	
		virtual void render(const Camera& camera);
        
        void setLineWidth(float width);
		void setColor(Vec4 color) { currentColor = color; }

		void begin(GLenum mode, GLuint texture = Renderer::whiteTexture);
        void addVertex(float x, float y, float z = -1.0f);
		void end();

        //  TODO: haven't implemented these yet. should probably also support normals.
        void addVertex(float x, float y, float u, float v);
        void addVertex(float x, float y, float r, float g, float b, float a);
        void addVertex(float x, float y, float u, float v, float r, float g, float b, float a);

        void renderTri(float x1, float y1, float x2, float y2, float x3, float y3, bool full = true);
        void renderRectangle(float x1, float y1, float x2, float y2, bool full = true);
        void renderCircle(float x, float y, float radius, bool full = true, int segments = 32);
        void renderArc(float x, float y, float radius, float angle1, float angle2, int segments = 10);
        void renderRoundedRectangle(float x, float y, float w, float h, float radius, bool full = true, int segments = 8);
		
		Shader *shader;
		Mat4 colorTransform;

		static Shader defaultShader;
		
	private:
		struct RenderItem {
			std::vector<GLfloat> vertices;   // x, y, z, u, v, r, g, b, a
			GLenum mode;
			GLuint texture;
		};
		std::vector<RenderItem> renderItems;
		RenderItem currentRenderItem;

		Vec4 currentColor;
        float halfWidth;

		GLuint vao;		//	vertex array object
		GLuint vbo;		//	vertex buffer

};

}

#endif
