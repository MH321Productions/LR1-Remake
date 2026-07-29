#ifndef LR1_REMAKE_VULKAN_HPP
#define LR1_REMAKE_VULKAN_HPP

#include <vector>
#include <map>
#include <ostream>
#include <optional>

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

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const;
    };

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        [[nodiscard]] bool isAdequate() const;
    };

    class VulkanBackend {
        public:
            explicit VulkanBackend(Main& main);
            ~VulkanBackend() = default;

            bool init();
            void cleanup() const;

        private:
            static const std::vector<char const*> validationLayers;
            static constexpr bool enableValidationLayers = validate;
            static const std::vector<const char*> deviceExtensions;

            Main& main;

            vk::Instance instance;

            vk::DebugUtilsMessengerEXT debugMessenger;
            const std::map<vk::DebugUtilsMessageSeverityFlagBitsEXT, std::ostream&> severityMap;

            vk::PhysicalDevice physicalDevice;
            QueueFamilyIndices queueFamilyIndices;

            vk::Device logicalDevice;
            vk::Queue graphicsQueue;
            vk::Queue presentQueue;

            vk::SurfaceKHR surface;

            vk::SwapchainKHR swapChain;

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
            [[nodiscard]] uint32_t ratePhysicalDevice(const vk::PhysicalDevice& device) const;
            [[nodiscard]] QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
            static bool areDeviceExtensionsSupported(const vk::PhysicalDevice& device) ;

            //Logical Device
            bool createLogicalDevice();
            void getQueues();

            //Surface
            bool createSurface();

            //Swap Chain
            [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;
            static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
            static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
            [[nodiscard]] vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
            bool createSwapChain();
    };
}

#endif //LR1_REMAKE_VULKAN_HPP
