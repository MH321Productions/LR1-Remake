#ifndef LR1_REMAKE_APP_HPP
#define LR1_REMAKE_APP_HPP

#include <filesystem>

#include <LR1-remake/util/log.hpp>
#include <LR1-remake/util/res.hpp>
#include <LR1-remake/renderer/vulkan.hpp>

struct SDL_Window;
struct MIX_Mixer;
union SDL_Event;

namespace LR1_Remake {
    class Main {
        friend class VulkanBackend;
        public:
            Main(const std::filesystem::path& rootDir) : res(*this), rootDir(rootDir), window(nullptr), mixer(nullptr), running(false), paused(false), vulkan(*this) {}

            Logger log;
            ResourceHandler res;
            const std::filesystem::path rootDir;

            int onExecute();
            bool triggerQuit();
            bool triggerPause();
            bool triggerResume();
            [[nodiscard]] bool isRunning() const { return running; }
            [[nodiscard]] bool isPaused() const { return paused; }

        private:
            SDL_Window* window;
            MIX_Mixer* mixer;
            bool running, paused;
            VulkanBackend vulkan;

            bool onInit();
            void onEvent(const SDL_Event& event);
            void onLoop();
            void onRender();
            void onCleanup();
    };

    namespace Version {
        static constexpr uint32_t major = 0;
        static constexpr uint32_t minor = 0;
        static constexpr uint32_t patch = 1;
    }
}

#endif //LR1_REMAKE_APP_HPP
