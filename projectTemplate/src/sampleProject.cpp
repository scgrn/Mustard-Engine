#include "mustard.h"
#include "renderer/sprite.h"

class App : public AB::Application {
    public:
		void startup() {
			sprite.load("gfx/tv1.tga");
			sprite.uploadToGPU();
		};

		void shutdown() {};
		
        void update() {
			if (AB::input.wasPressed(41)) {
				AB::quit();
			}
			
            AB::script.execute("AB.update()");
        }

        void render() {
			AB::renderer.beginScene(camera);
			
			glClearColor(0.5f, 0.2f, 0.35f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
            AB::script.execute("AB.render()");
			
			AB::renderer.defineRenderGroup(1, &AB::renderer.defaultShader);
			AB::renderer.defineRenderGroup(2, &AB::renderer.defaultShader);
			
			AB::Renderer::RenderQuad quad;
			
			quad.pos = glm::vec3(400.0f, 300.0f, -1.0f);
			quad.size = glm::vec2(23.0f, 18.0f); 
			quad.scale = 4.0f;
			quad.rotation = 0.0f;
			quad.uv = glm::vec4(0.0f, 0.0f, 0.71875f, 0.5625f);
			quad.textureID = sprite.texture->glHandle;
			quad.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			AB::renderer.renderQuad(1, quad);

			quad.pos = glm::vec3(500.0f, 300.0f, -1.0f);
			AB::renderer.renderQuad(2, quad);

			quad.pos = glm::vec3(200.0f, 300.0f, -1.0f);
			AB::renderer.renderQuad(2, quad);

			quad.pos = glm::vec3(400.0f, 100.0f, -1.0f);
			AB::renderer.renderQuad(2, quad);

			AB::renderer.endScene();
        }
		
		AB::OrthographicCamera camera;
		AB::Sprite sprite;
};

AB::Application* AB::createApplication() {
    return new App();
}
