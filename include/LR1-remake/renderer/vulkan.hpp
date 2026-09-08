#ifndef LR1_REMAKE_VULKAN_HPP
#define LR1_REMAKE_VULKAN_HPP

#include <vector>
#include <map>
#include <ostream>
#include <optional>
#include <chrono>

#include <vulkan/vulkan.hpp>

#include <LR1-remake/renderer/data.hpp>

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
        main.log.fatal << (err) << ": " << e.what() << endl; \
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
            static constexpr uint32_t maxFramesInFlight = 2;
            static constexpr uint32_t maxSwapChainImages = 4;

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

            vk::DescriptorSetLayout descriptorSetLayout;
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

            vk::Buffer vertexBuffer;
            vk::DeviceMemory vertexBufferMemory;
            vk::Buffer indexBuffer;
            vk::DeviceMemory indexBufferMemory;
            std::vector<vk::Buffer> uniformBuffers;
            std::vector<vk::DeviceMemory> uniformBufferMemories;
            std::vector<void*> mappedUniformBuffers;
            vk::DescriptorPool descriptorPool;
            std::vector<vk::DescriptorSet> descriptorSets;

            vk::Image textureImage;
            vk::DeviceMemory textureImageMemory;
            vk::ImageView textureImageView;
            vk::Sampler textureSampler;

            vk::Image depthImage;
            vk::DeviceMemory depthImageMemory;
            vk::ImageView depthImageView;

            std::vector<Simple3DColorNormalVertex> vertices;
            std::vector<uint32_t> indices;

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
            bool createDescriptorSetLayout();
            bool createGraphicsPipeline(const std::vector<vk::VertexInputBindingDescription>& vertexBindingDescriptions, const std::vector<vk::VertexInputAttributeDescription>& vertexAttributeDescriptions);
            template <IVertex TData> bool createGraphicsPipeline() { return createGraphicsPipeline(TData::bindingDescriptions, TData::attributeDescriptions); }
            vk::ShaderModule createShaderModule(const std::vector<uint8_t>& code) const;
            bool createRenderPass();

            //Drawing
            bool createFramebuffers();
            bool createCommandPool();
            bool createCommandBuffers();
            bool recordCommandBuffer(const vk::CommandBuffer& cmd, const uint32_t& imageIndex);
            bool createSyncObjects();
            void updateUniformBuffer(const uint32_t& currentImage);

            //Buffers
            bool createBuffer(const vk::DeviceSize& size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) const;
            bool createVertexBuffer(const size_t& vertexSize);
            template <IVertex TData> bool createVertexBuffer() { return createVertexBuffer(sizeof(TData)); }
            bool createIndexBuffer();
            bool createUniformBuffers();
            bool createDescriptorPool();
            bool createDescriptorSets();
            [[nodiscard]] uint32_t findMemoryType(const uint32_t& typeFilter, vk::MemoryPropertyFlags properties) const;
            void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const vk::DeviceSize& size) const;
            vk::CommandBuffer beginSingleTimeCommands() const;
            void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const;

            //Textures
            bool createImage(const uint32_t& width, const uint32_t& height, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties, vk::Image& image, vk::DeviceMemory& imageMemory) const;
            bool createTextureImage();
            void transitionLayout(const vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
            void copyBufferToImage(const vk::Buffer& buffer, const vk::Image& image, const uint32_t& width, const uint32_t& height) const;
            bool createTextureImageView();
            [[nodiscard]] vk::ImageView createImageView(const vk::Image& image, vk::Format format, const vk::ImageAspectFlags& aspectFlags) const;
            bool createTextureSampler();

            //Depth buffering
            bool createDepthResources();
            [[nodiscard]] vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& features) const;
            [[nodiscard]] vk::Format findDepthFormat() const;
            [[nodiscard]] static bool hasStencilComponent(vk::Format format);

            //Models
            bool loadModel();
    };
}

#endif //LR1_REMAKE_VULKAN_HPP
