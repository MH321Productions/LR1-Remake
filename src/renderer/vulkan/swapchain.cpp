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
        auto [capabilities, formats, presentModes] = querySwapChainSupport(physicalDevice);

        const vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
        const vk::PresentModeKHR presentMode = chooseSwapPresentMode(presentModes);
        const vk::Extent2D extent = chooseSwapExtent(capabilities);

        const uint32_t neededImages = capabilities.minImageCount + 1;
        const uint32_t maxImages = capabilities.maxImageCount;
        const uint32_t imageCount = maxImages > 0 ? min(neededImages, maxImages) : neededImages;

        vector indicesVector = {queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value()};

        vk::SharingMode sharingMode;
        vk::ArrayProxyNoTemporaries<uint32_t const> indices;
        if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily) {
            sharingMode = vk::SharingMode::eConcurrent;
            indices = indicesVector;
        } else {
            sharingMode = vk::SharingMode::eExclusive;
            indices = {};
        }

        const vk::SwapchainCreateInfoKHR createInfo(
            {},
            surface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            sharingMode,
            indices,
            capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            presentMode,
            true,
            nullptr
        );

        try {
            swapChain = logicalDevice.createSwapchainKHR(createInfo);
            swapChainImages = logicalDevice.getSwapchainImagesKHR(swapChain);
            swapChainImageFormat = surfaceFormat.format;
            swapChainExtent = extent;
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't create swap chain: " << e.what();
            return false;
        }

        return true;
    }

    bool VulkanBackend::createImageViews() {
        for (const vk::Image& image: swapChainImages) {
            vk::ImageViewCreateInfo createInfo(
                {},
                image,
                vk::ImageViewType::e2D,
                swapChainImageFormat,
                vk::ComponentMapping(
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity
                ),
                vk::ImageSubresourceRange(
                    vk::ImageAspectFlagBits::eColor,
                    0,
                    1,
                    0,
                    1
                )
            );

            try {
                swapChainImageViews.push_back(logicalDevice.createImageView(createInfo));
            } catch (runtime_error& e) {
                main.log.fatal << "Couldn't create image view: " << e.what() << endl;
                return false;
            }
        }

        return true;
    }

    bool SwapChainSupportDetails::isAdequate() const {
        return !formats.empty() && !presentModes.empty();
    }
}
