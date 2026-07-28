#ifndef LR1_REMAKE_VULKAN_HPP
#define LR1_REMAKE_VULKAN_HPP

#include <vector>
#include <map>
#include <ostream>

#include <vulkan/vulkan.hpp>

#ifdef LR1_DEBUG
#define validate true
#else
#define validate false
#endif

#define checkResult(var, func) \
    try {\
        (var) = (func);\
    } catch (runtime_error& e) { \
        main.log.fatal << "Couldn't create " << #var << ": " << e.what() << endl; \
        return false; \
    }\
    return true

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
            static constexpr bool enableValidationLayers = validate;

            Main& main;

            vk::Instance instance;

            vk::DebugUtilsMessengerEXT debugMessenger;
            const std::map<vk::DebugUtilsMessageSeverityFlagBitsEXT, std::ostream&> severityMap;

            vk::PhysicalDevice physicalDevice;

            //Instance creation
            bool createInstance();
            static std::vector<char const*> getRequiredExtensions();
            void listExtensions() const;
            [[nodiscard]] bool checkValidationLayerSupport() const;

            //Debug message creation
            bool setupDebugMessenger();
            void destroyDebugMessenger() const;
            static VKAPI_ATTR uint32_t VKAPI_CALL debugMessageCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, vk::DebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData);

            //Physical Device and queue family
            bool pickPhysicalDevice();
            int ratePhysicalDevice(const vk::PhysicalDevice& device);
    };
}

#endif //LR1_REMAKE_VULKAN_HPP
