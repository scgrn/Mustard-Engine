#include "mustard.h"

class App : public AB::Application {
    public:
        void startup() {
        };

        void shutdown() {
        };
        
        void update() {
            AB::script.execute("AB.update()");
        }

        void render() {
            AB::RenderLayer::textureCache.invalidate();
            AB::script.execute("AB.render()");
            AB::renderer.render(AB::camera2d);
        }
};

AB::Application* AB::createApplication() {

#ifdef DIST
#include "addArchive.cpp"
#endif

    return new App();
}

