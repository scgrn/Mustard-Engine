#include "mustard.h"

class App : public AB::Application {
    public:
		void startup() {
		};

		void shutdown() {
		};
		
        void update() {
			if (AB::input.wasKeyPressed(41)) {
				AB::quit();
			}
			
            AB::script.execute("AB.update()");
        }

        void render() {
            AB::script.execute("AB.render()");
			AB::renderer.renderBatches(AB::camera2d);
        }
};

AB::Application* AB::createApplication() {
    return new App();
}
