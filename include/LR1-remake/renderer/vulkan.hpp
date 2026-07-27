#ifndef LR1_REMAKE_VULKAN_HPP
#define LR1_REMAKE_VULKAN_HPP

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

namespace LR1_Remake {
    class Main;

    class VulkanBackend {
        public:
            explicit VulkanBackend(Main& main);
            ~VulkanBackend() = default;

            bool init();
            void cleanup() const;

        private:
            static const std::vector<char const*> validationLayers;

            Main& main;

            vk::Instance instance;

            bool createInstance();
            void listExtensions() const;
            bool checkValidationLayerSupport() const;
    };
}

#endif //LR1_REMAKE_VULKAN_HPP
