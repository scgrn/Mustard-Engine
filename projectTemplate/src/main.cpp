#include "mustard.h"

class App : public AB::Application {
    public:
        void startup() {
        };

        void shutdown() {
        };

        void update() {
            if (focused) {
                AB::script.execute("AB.update()");
            }
        }

        void onPause() {
            focused = false;
            AB::script.execute("AB.onFocusLost()");
        }

        void onResume() {
            focused = true;
            AB::script.execute("AB.onFocusGained()");
        }

        void render() {
            AB::script.execute("AB.render()");
            AB::renderer.render(AB::camera2d);
        }

        bool focused = true;
};

AB::Application* AB::createApplication() {

#if defined(DIST) || defined(__EMSCRIPTEN__)
#include "addArchive.cpp"
#endif

    return new App();
}

