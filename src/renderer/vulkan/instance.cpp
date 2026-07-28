#include <vector>

#include <SDL3/SDL_vulkan.h>

#include <LR1-remake/app.hpp>
#include <LR1-remake/renderer/vulkan.hpp>

using namespace std;

#ifdef LR1_DEBUG
#define instanceValidationLayers VulkanBackend::validationLayers
#else
#define instanceValidationLayers {}
#endif

namespace LR1_Remake {
    bool VulkanBackend::createInstance() {
        constexpr uint32_t appVersion = VK_MAKE_VERSION(Version::major, Version::minor, Version::patch);
        constexpr uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
        constexpr vk::ApplicationInfo appInfo("LR1-Remake", appVersion, "No Engine", engineVersion, VK_API_VERSION_1_0);

        const vector<char const *> requiredExtensions = getRequiredExtensions();
        const vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, instanceValidationLayers, requiredExtensions);
        checkResult(instance, vk::createInstance(instanceCreateInfo));
    }

    vector<char const *> VulkanBackend::getRequiredExtensions() {
        vector<char const*> requiredExtensions;
        uint32_t extensionCount = 0;
        char const* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        for (uint32_t i = 0; i < extensionCount; i++) {
            requiredExtensions.emplace_back(extensions[i]);
        }

        if (enableValidationLayers) requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return requiredExtensions;
    }

    void VulkanBackend::listExtensions() const {
        try {
            const vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();
            main.log.debug << "Found " << extensions.size() << " extensions:" << endl;
            for (const vk::ExtensionProperties& extension: extensions) {
                main.log.debug << "\t" << extension.extensionName << endl;
            }
        } catch (std::runtime_error& e) {
            main.log.error << "Couldn't enumerate extensions: " << e.what() << endl;
        }
    }

    bool VulkanBackend::checkValidationLayerSupport() const {
        try {
            const vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
            for (const string& requiredLayer: validationLayers) {
                bool layerFound = false;
                for (const vk::LayerProperties& layer: availableLayers) {
                    if (string(requiredLayer) == layer.layerName) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    main.log.fatal << "Couldn't find required layer: " << requiredLayer << endl;
                    return false;
                }
            }

            return true;
        } catch (std::runtime_error& e) {
            main.log.fatal << "Couldn't enumerate validation layers: " << e.what() << endl;
            return false;
        }
    }
}