#ifndef LR1_REMAKE_APP_HPP
#define LR1_REMAKE_APP_HPP

#include <LR1-remake/util/log.hpp>

struct SDL_Window;
struct MIX_Mixer;
union SDL_Event;

namespace LR1_Remake {
    class Main {
        public:
            Main() : window(nullptr), mixer(nullptr), running(false) {}

            Logger log;

            int onExecute();
            bool triggerQuit();
            [[nodiscard]] bool isRunning() const { return running; }

        private:
            SDL_Window* window;
            MIX_Mixer* mixer;
            bool running;

            bool onInit();
            void onEvent(const SDL_Event& event);
            void onLoop();
            void onRender();
            void onCleanup();
    };
}

#endif //LR1_REMAKE_APP_HPP
