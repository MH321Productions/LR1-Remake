#include <stb/stb_image.h>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createTextureImage() {
        int width, height, channels;
        uint8_t* pixels = stbi_load(main.res.getResourcePath("viking_room", ResourceType::Texture).string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        const vk::DeviceSize imageSize = width * height * 4;

        if (!pixels) {
            main.log.error << "Couldn't load image: " << stbi_failure_reason() << std::endl;
            return false;
        }

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        if (!createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory)) return false;

        void* data = logicalDevice.mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, imageSize);
        logicalDevice.unmapMemory(stagingBufferMemory);
        stbi_image_free(pixels);

        if (!createImage(
            width, height,
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            textureImage, textureImageMemory
        )) return false;

        transitionLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(stagingBuffer, textureImage, width, height);
        transitionLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        logicalDevice.destroyBuffer(stagingBuffer);
        logicalDevice.freeMemory(stagingBufferMemory);

        return true;
    }

    bool VulkanBackend::createImage(
        const uint32_t& width, const uint32_t& height, const vk::Format format, const vk::ImageTiling tiling,
        const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties, vk::Image& image, vk::DeviceMemory& imageMemory
    ) const {
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

    void VulkanBackend::transitionLayout(const vk::Image& image, vk::Format format, const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout) const {
        const vk::CommandBuffer commandBuffer = beginSingleTimeCommands();
        vk::AccessFlags srcAccessMask, dstAccessMask;
        vk::PipelineStageFlags srcStage, dstStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
            srcAccessMask = {};
            dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            dstStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            dstAccessMask = vk::AccessFlagBits::eShaderRead;
            srcStage = vk::PipelineStageFlagBits::eTransfer;
            dstStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else {
            throw runtime_error("Unsupported layout transition");
        }

        const vk::ImageMemoryBarrier barrier(
            srcAccessMask,
            dstAccessMask,
            oldLayout,
            newLayout,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            image,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
        );

        commandBuffer.pipelineBarrier(
            srcStage,
            dstStage,
            {},
            {},
            {},
            barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    void VulkanBackend::copyBufferToImage(const vk::Buffer& buffer, const vk::Image& image, const uint32_t& width, const uint32_t& height) const {
        const vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        const vk::BufferImageCopy region(
            0, 0, 0,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
            {0, 0, 0},
            {width, height, 1}
        );
        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

        endSingleTimeCommands(commandBuffer);
    }

    bool VulkanBackend::createTextureImageView() {
        checkResult(textureImageView, createImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor));
    }

    vk::ImageView VulkanBackend::createImageView(const vk::Image& image, const vk::Format format, const vk::ImageAspectFlags& aspectFlags) const {
        const vk::ImageViewCreateInfo viewInfo(
            {},
            image,
            vk::ImageViewType::e2D,
            format,
            vk::ComponentSwizzle::eIdentity,
            vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1)
        );

        return logicalDevice.createImageView(viewInfo);
    }

    bool VulkanBackend::createTextureSampler() {
        const vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
        const vk::PhysicalDeviceFeatures features = physicalDevice.getFeatures();
        const vk::SamplerCreateInfo samplerInfo(
            {},
            vk::Filter::eLinear, vk::Filter::eLinear,
            vk::SamplerMipmapMode::eLinear,
            vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
            0.0f,
            features.samplerAnisotropy, features.samplerAnisotropy ? properties.limits.maxSamplerAnisotropy : 1.0f,
            false, vk::CompareOp::eAlways,
            0.0f, 0.0f,
            vk::BorderColor::eIntOpaqueBlack,
            false
        );

        checkResult(textureSampler, logicalDevice.createSampler(samplerInfo));
    }
}
