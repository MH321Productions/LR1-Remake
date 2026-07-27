#include <iostream>

#include <SDL3/SDL.h>

#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    int Main::onExecute() {
        if (!onInit()) return 1;

        running = true;
        SDL_Event event;
        while (running) {
            while (SDL_PollEvent(&event)) onEvent(event);

            onLoop();
            onRender();
        }

        onCleanup();

        return 0;
    }

    bool Main::triggerQuit() {
        if (running) {
            running = false;
            return true;
        }
        return false;
    }

    bool Main::onInit() {
        return true;
    }

    void Main::onCleanup() {
        if (window) SDL_DestroyWindow(window);
    }
}
