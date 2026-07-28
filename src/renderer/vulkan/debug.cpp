#include <map>

#include <LR1-remake/app.hpp>
#include <LR1-remake/renderer/vulkan.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::setupDebugMessenger() {
        if constexpr (!enableValidationLayers) return true;

        vk::DebugUtilsMessengerCreateInfoEXT createInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            &VulkanBackend::debugMessageCallback,
            this
        );

        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
        if (func) {
            VkResult res = func(instance, reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, reinterpret_cast<VkDebugUtilsMessengerEXT*>(&debugMessenger));
            if (res != VK_SUCCESS) {
                main.log.fatal << "Couldn't create debug message callback: " << vk::to_string(static_cast<vk::Result>(res)) << endl;
                return false;
            }

            return true;
        }

        main.log.fatal << "Couldn't create debug message callback: create function not found" << endl;
        return false;
    }

    void VulkanBackend::destroyDebugMessenger() const {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
        if (func) func(instance, debugMessenger, nullptr);
    }

    uint32_t VulkanBackend::debugMessageCallback(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, vk::DebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData) {
        const auto* vulkan = static_cast<VulkanBackend*>(userData);
        vulkan->severityMap.at(severity) << vk::to_string(type) << ": " << callbackData->pMessage << endl;
        return false;
    }
}
