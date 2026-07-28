#include <LR1-remake/renderer/vulkan.hpp>

#include "LR1-remake/app.hpp"

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::pickPhysicalDevice() {
        try {
            vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
            if (devices.empty()) {
                main.log.fatal << "No suitable GPU found" << endl;
                return false;
            }

            ranges::sort(devices, [&](const vk::PhysicalDevice& a, const vk::PhysicalDevice& b) {return ratePhysicalDevice(a) > ratePhysicalDevice(b);});

        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't pick GPU: " << e.what() << endl;
            return false;
        }
    }
}
