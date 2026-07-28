#include <vector>

#include <LR1-remake/renderer/vulkan.hpp>

#include "LR1-remake/app.hpp"

#define tryInit(func) if (!(func)) return false

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
    }) {}

    bool VulkanBackend::init() {
        if (enableValidationLayers) {
            tryInit(checkValidationLayerSupport());
            listExtensions();
        }

        tryInit(createInstance());
        tryInit(setupDebugMessenger());

        return true;
    }

    void VulkanBackend::cleanup() const {
        if (enableValidationLayers) destroyDebugMessenger();

        instance.destroy();
    }
}
