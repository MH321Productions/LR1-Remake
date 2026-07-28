#include <set>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

struct DeviceInfo {
    vk::PhysicalDevice device;
    uint32_t rating;
};

namespace LR1_Remake {
    const vector<const char*> VulkanBackend::deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool VulkanBackend::pickPhysicalDevice() {
        try {
            const vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
            if (devices.empty()) {
                main.log.fatal << "No suitable GPU found" << endl;
                return false;
            }

            vector<DeviceInfo> infos;
            for (const vk::PhysicalDevice& device : devices) {
                const uint32_t rating = ratePhysicalDevice(device);
                infos.push_back({.device = device, .rating = rating});
                main.log.debug << "Found \"" << device.getProperties().deviceName << "\" (score " << rating << ")" << endl;
            }

            ranges::sort(infos, [&](const DeviceInfo& a, const DeviceInfo& b) {return a.rating > b.rating;});

            if (infos.front().rating > 0) {
                physicalDevice = infos.front().device;
                main.log.info << "Using \"" << physicalDevice.getProperties().deviceName << "\" as GPU" << endl;
                return true;
            }

            return false;
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't pick GPU: " << e.what() << endl;
            return false;
        }
    }

    uint32_t VulkanBackend::ratePhysicalDevice(const vk::PhysicalDevice& device) const {
        uint32_t score = 0;
        const vk::PhysicalDeviceProperties properties = device.getProperties();
        const vk::PhysicalDeviceFeatures features = device.getFeatures();

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000;   //Prefer discrete GPUs
        score += properties.limits.maxImageDimension2D;                                     //Prefer GPUs with higher possible texture resolution

        if (!features.geometryShader) return 0;                                             //We need Geometry shaders
        if (!findQueueFamilies(device).isComplete()) return 0;                              //We need the required queue families
        if (!areDeviceExtensionsSupported(device)) return 0;                                //We need the required device extensions to be supported

        return score;
    }

    QueueFamilyIndices VulkanBackend::findQueueFamilies(const vk::PhysicalDevice& device) const {
        QueueFamilyIndices indices;
        const vector<vk::QueueFamilyProperties> families = device.getQueueFamilyProperties();

        for (uint32_t i = 0; i < families.size(); i++) {
            if (families.at(i).queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;
            if (device.getSurfaceSupportKHR(i, surface)) indices.presentFamily = i;

            if (indices.isComplete()) break;
        }

        return indices;
    }

    bool VulkanBackend::areDeviceExtensionsSupported(const vk::PhysicalDevice& device) {
        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    bool QueueFamilyIndices::isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
}
