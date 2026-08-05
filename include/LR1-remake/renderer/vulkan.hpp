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

#define tryInit(func) if (!(func)) return false

#define checkResult(var, func) \
    try {\
        (var) = (func);\
    } catch (runtime_error& e) { \
        main.log.fatal << "Couldn't create " << #var << ": " << e.what() << endl; \
        return false; \
    }\
    return true

#define checkFunc(func, err) \
    try {\
        (func);\
    } catch (runtime_error& e) { \
        main.log.fatal << err << ": " << e.what() << endl; \
        return false; \
    }

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
            void cleanup();
            bool drawFrame();
            void triggerResize();

        private:
            static const std::vector<char const*> validationLayers;
            static constexpr bool enableValidationLayers = validate;
            static const std::vector<const char*> deviceExtensions;
            static constexpr uint32_t maxFramesInFlight = 3;

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
            std::vector<vk::Image> swapChainImages;
            vk::Format swapChainImageFormat;
            vk::Extent2D swapChainExtent;

            std::vector<vk::ImageView> swapChainImageViews;

            vk::PipelineLayout pipelineLayout;
            vk::Pipeline graphicsPipeline;
            vk::RenderPass renderPass;

            std::vector<vk::Framebuffer> swapChainFrameBuffers;
            vk::CommandPool commandPool;
            std::vector<vk::CommandBuffer> commandBuffers;
            std::vector<vk::Semaphore> imageAvailableSemaphores, renderFinishedSemaphores;
            std::vector<vk::Fence> inFlightFences;
            uint32_t currentFrame;
            bool framebufferResized;

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
            bool createImageViews();
            bool recreateSwapChain();
            void cleanupSwapChain();

            //Graphics Pipeline
            bool createGraphicsPipeline();
            vk::ShaderModule createShaderModule(const std::vector<uint8_t>& code);
            bool createRenderPass();

            //Drawing
            bool createFramebuffers();
            bool createCommandPool();
            bool createCommandBuffers();
            bool recordCommandBuffer(const vk::CommandBuffer& cmd, const uint32_t& imageIndex);
            bool createSyncObjects();
    };
}

#endif //LR1_REMAKE_VULKAN_HPP
