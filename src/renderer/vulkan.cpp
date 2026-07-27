#include <vector>

#include <SDL3/SDL_vulkan.h>

#include <LR1-remake/app.hpp>
#include <LR1-remake/renderer/vulkan.hpp>

#define tryInit(func) if (!(func)) return false
#define checkResult(var, func) \
    try {\
        (var) = (func);\
    } catch (runtime_error& e) { \
        main.log.fatal << "Couldn't create " << #var << ": " << e.what() << endl; \
        return false; \
    }\
    return true

#ifdef LR1_DEBUG
#define enabledValidationLayers VulkanBackend::validationLayers
#else
#define enabledValidationLayers {}
#endif

using namespace std;

namespace LR1_Remake {
    const vector<char const*> VulkanBackend::validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VulkanBackend::VulkanBackend(Main &main) : main(main) {}

    bool VulkanBackend::init() {
#ifdef LR1_DEBUG
        tryInit(checkValidationLayerSupport());
        listExtensions();
#endif

        tryInit(createInstance());

        return true;
    }

    void VulkanBackend::cleanup() const {
        instance.destroy();
    }

    bool VulkanBackend::createInstance() {
        constexpr uint32_t appVersion = VK_MAKE_VERSION(Version::major, Version::minor, Version::patch);
        constexpr uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
        constexpr vk::ApplicationInfo appInfo("LR1-Remake", appVersion, "No Engine", engineVersion, VK_API_VERSION_1_0);

        vector<char const *> requiredExtensions;
        uint32_t extensionCount = 0;
        char const* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        for (uint32_t i = 0; i < extensionCount; i++) {
            requiredExtensions.emplace_back(extensions[i]);
        }
        const vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabledValidationLayers, requiredExtensions);
        checkResult(instance, vk::createInstance(instanceCreateInfo));
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
