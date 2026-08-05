#include <SDL3/SDL.h>

#include <LR1-remake/app.hpp>

namespace LR1_Remake {
    void Main::onEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                triggerQuit();
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                if (event.window.data1 && event.window.data2) {
                    triggerResume();
                    vulkan.triggerResize();
                } else {
                    triggerPause();
                }
                break;

            case SDL_EVENT_WINDOW_MINIMIZED:
                triggerPause();
                break;

            case SDL_EVENT_WINDOW_RESTORED:
                triggerResume();
                break;

            default:
                break;
        }
    }
}
