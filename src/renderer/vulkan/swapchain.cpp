#include <limits>
#include <algorithm>

#include <SDL3/SDL_video.h>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    SwapChainSupportDetails VulkanBackend::querySwapChainSupport(const vk::PhysicalDevice& device) const {
        return {
            .capabilities = device.getSurfaceCapabilitiesKHR(surface),
            .formats = device.getSurfaceFormatsKHR(surface),
            .presentModes = device.getSurfacePresentModesKHR(surface)
        };
    }

    vk::SurfaceFormatKHR VulkanBackend::chooseSwapSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats) {
        for (const vk::SurfaceFormatKHR& format: formats)
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return format;

        return formats.front();
    }

    vk::PresentModeKHR VulkanBackend::chooseSwapPresentMode(const vector<vk::PresentModeKHR>& presentModes) {
        for (const vk::PresentModeKHR& presentMode: presentModes)
            if (presentMode == vk::PresentModeKHR::eMailbox) return presentMode;

        /*
         * We currently prefer Mailbox (Triple Buffering) over Fifo (VSync) as fallback
         * TODO: Let the user choose in the settings
         */

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D VulkanBackend::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
        if (capabilities.currentExtent.width != numeric_limits<uint32_t>::max()) return capabilities.currentExtent;

        int w, h;
        if (!SDL_GetWindowSizeInPixels(main.window, &w, &h)) {
            main.log.error << "Couldn't query window size: " << SDL_GetError() << endl;
            return {1280, 720};
        }

        uint32_t width = clamp(static_cast<uint32_t>(w), capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        uint32_t height = clamp(static_cast<uint32_t>(h), capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return {width, height};
    }

    bool VulkanBackend::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        const vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        const vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        const vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);


    }

    bool SwapChainSupportDetails::isAdequate() const {
        return !formats.empty() && !presentModes.empty();
    }
}
