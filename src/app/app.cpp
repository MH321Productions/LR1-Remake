#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    int Main::onExecute() {
        if (!onInit()) return 1;

        log.info << "Successfully initialized" << endl;

        running = true;
        SDL_Event event;
        while (running) {
            while (SDL_PollEvent(&event)) onEvent(event);

            if (paused) continue;

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

    bool Main::triggerPause() {
        if (!paused) {
            paused = true;
            log.debug << "Paused" << endl;
            return true;
        }
        return false;
    }

    bool Main::triggerResume() {
        if (paused) {
            paused = false;
            log.debug << "Resumed" << endl;
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

        window = SDL_CreateWindow("LR1-Remake", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
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

        if (!vulkan.init()) return false;

        return true;
    }

    void Main::onCleanup() {
        vulkan.cleanup();
        if (mixer) MIX_DestroyMixer(mixer);
        if (window) SDL_DestroyWindow(window);
    }
}
