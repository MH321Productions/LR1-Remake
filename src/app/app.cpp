#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

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
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)) {
            log.fatal << "Couldn't init SDL: " << SDL_GetError() << endl;
            return false;
        }

        if (!MIX_Init()) {
            log.fatal << "Couldn't init SDL_mixer: " << SDL_GetError() << endl;
            return false;
        }

        window = SDL_CreateWindow("LR1-Remake", 1280, 720, SDL_WINDOW_VULKAN);
        if (!window) {
            log.fatal << "Couldn't create window: " << SDL_GetError() << endl;
            return false;
        }
        log.window = window;

        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!mixer) {
            log.fatal << "Couldn't create mixer: " << SDL_GetError() << endl;
            return false;
        }

        return true;
    }

    void Main::onCleanup() {
        if (mixer) MIX_DestroyMixer(mixer);
        if (window) SDL_DestroyWindow(window);
    }
}
