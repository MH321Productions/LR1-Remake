#include <SDL3/SDL.h>

#include <LR1-remake/app.hpp>

namespace LR1_Remake {
    void Main::onEvent(const SDL_Event& event) {
        if (event.type == SDL_EVENT_QUIT) triggerQuit();
    }
}
