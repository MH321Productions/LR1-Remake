#include <stb/stb_image.h>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createTextureImage() {
        int width, height, channels;
        uint8_t* pixels = stbi_load(main.res.getResourcePath("flowers", ResourceType::Texture).c_str(), &width, &height, &channels, STBI_rgb_alpha);
        const vk::DeviceSize imageSize = width * height * 4;

        if (!pixels) {
            main.log.error << "Couldn't load image: " << stbi_failure_reason() << std::endl;
            return false;
        }

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        checkFunc(
            createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory),
            "Couldn't create image staging buffer"
        );

        void* data = logicalDevice.mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, imageSize);
        logicalDevice.unmapMemory(stagingBufferMemory);
        stbi_image_free(pixels);

        return createImage(
            width, height,
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            textureImage, textureImageMemory
        );
    }

    bool VulkanBackend::createImage(const uint32_t& width, const uint32_t& height, const vk::Format format, const vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties, vk::Image& image, vk::DeviceMemory& imageMemory) const {
        const vk::ImageCreateInfo imageInfo(
            {},
            vk::ImageType::e2D,
            format,
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height),
                1
            },
            1,
            1,
            vk::SampleCountFlagBits::e1,
            tiling,
            usage,
            vk::SharingMode::eExclusive,
            {},
            vk::ImageLayout::eUndefined
        );

        checkFunc(image = logicalDevice.createImage(imageInfo), "Couldn't create image");

        const vk::MemoryRequirements memRequirements = logicalDevice.getImageMemoryRequirements(image);
        const vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

        checkFunc(imageMemory = logicalDevice.allocateMemory(allocInfo), "Couldn't allocate memory for image");
        checkFunc(logicalDevice.bindImageMemory(image, imageMemory, 0), "Couldn't bind image memory");

        return true;
    }
}
