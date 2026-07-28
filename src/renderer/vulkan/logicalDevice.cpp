#include <set>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createLogicalDevice() {
        const auto [graphicsFamily, presentFamily] = findQueueFamilies(physicalDevice);
        set uniqueQueueFamilies = {graphicsFamily.value(), presentFamily.value()};
        vector queuePriorities = {1.0f};
        vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        for (const uint32_t& queueFamily : uniqueQueueFamilies) queueCreateInfos.push_back({{}, queueFamily, queuePriorities});

        constexpr vk::PhysicalDeviceFeatures physicalDeviceFeatures{};
        const vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, {}, deviceExtensions, &physicalDeviceFeatures);

        checkResult(logicalDevice, physicalDevice.createDevice(deviceCreateInfo));
    }

    void VulkanBackend::getQueues() {
        const auto [graphicsFamily, presentFamily] = findQueueFamilies(physicalDevice);
        graphicsQueue = logicalDevice.getQueue(graphicsFamily.value(), 0);
        presentQueue = logicalDevice.getQueue(presentFamily.value(), 0);
    }
}
