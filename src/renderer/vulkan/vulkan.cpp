#include <vector>

#include <SDL3/SDL_vulkan.h>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    const vector<char const*> VulkanBackend::validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VulkanBackend::VulkanBackend(Main &main) : main(main), severityMap({
        {vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose, main.log.debug},
        {vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo, main.log.info},
        {vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning, main.log.warning},
        {vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, main.log.error},
    }), swapChainImageFormat(vk::Format::eUndefined), currentFrame(0), framebufferResized(false) {}

    bool VulkanBackend::init() {
        if (enableValidationLayers) {
            tryInit(checkValidationLayerSupport());
            listExtensions();
        }

        tryInit(createInstance());
        tryInit(setupDebugMessenger());
        tryInit(createSurface());
        tryInit(pickPhysicalDevice());
        tryInit(createLogicalDevice());
        getQueues();
        tryInit(createSwapChain());
        tryInit(createImageViews());
        tryInit(createRenderPass());
        tryInit(createGraphicsPipeline());
        tryInit(createFramebuffers());
        tryInit(createCommandPool());
        tryInit(createCommandBuffers());
        tryInit(createSyncObjects());

        return true;
    }

    void VulkanBackend::cleanup() {
        logicalDevice.waitIdle();

        cleanupSwapChain();

        for (uint32_t i = 0; i < maxFramesInFlight; i++) {
            logicalDevice.destroySemaphore(imageAvailableSemaphores.at(i));
            logicalDevice.destroySemaphore(renderFinishedSemaphores.at(i));
            logicalDevice.destroyFence(inFlightFences.at(i));
        }
        logicalDevice.destroyCommandPool(commandPool);
        logicalDevice.destroyPipeline(graphicsPipeline);
        logicalDevice.destroyPipelineLayout(pipelineLayout);
        logicalDevice.destroyRenderPass(renderPass);
        logicalDevice.destroy();
        SDL_Vulkan_DestroySurface(instance, surface, nullptr);
        if (enableValidationLayers) destroyDebugMessenger();
        instance.destroy();
    }

    bool VulkanBackend::createSurface() {
        if (!SDL_Vulkan_CreateSurface(main.window, instance, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface))) {
            main.log.fatal << "Couldn't create Vulkan surface: " << SDL_GetError() << endl;
            return false;
        }
        return true;
    }
}
