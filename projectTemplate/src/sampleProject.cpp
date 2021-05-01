#include "mustard.h"

class App : public AB::Application {
    public:
		void startup() {
		};

		void shutdown() {};
		
        void update() {
			if (AB::input.wasPressed(41)) {
				AB::quit();
			}
			
            AB::script.execute("AB.update()");
        }

        void render() {
        }
};

AB::Application* AB::createApplication() {
    return new App();
}
