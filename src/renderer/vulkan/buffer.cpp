#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    const std::vector<Simple2DColorVertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    bool VulkanBackend::createBuffer(const vk::DeviceSize& size, const vk::BufferUsageFlags usage, const vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) const {
        const vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);

        checkFunc(buffer = logicalDevice.createBuffer(bufferInfo), "Couldn't create buffer");

        const vk::MemoryRequirements memRequirements = logicalDevice.getBufferMemoryRequirements(buffer);
        const vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

        checkFunc(bufferMemory = logicalDevice.allocateMemory(allocInfo), "Couldn't allocate the buffer memory");

        logicalDevice.bindBufferMemory(buffer, bufferMemory, 0);

        return true;
    }

    bool VulkanBackend::createVertexBuffer() {
        const vk::DeviceSize bufferSize = sizeof(Simple2DColorVertex) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        void* data = logicalDevice.mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, vertices.data(), bufferSize);
        logicalDevice.unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        logicalDevice.destroyBuffer(stagingBuffer);
        logicalDevice.freeMemory(stagingBufferMemory);

        return true;
    }

    uint32_t VulkanBackend::findMemoryType(const uint32_t& typeFilter, const vk::MemoryPropertyFlags properties) const {
        const vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
        }

        throw runtime_error("Couldn't find suitable memory type");
    }

    void VulkanBackend::copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const vk::DeviceSize& size) const {
        const vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
        const vk::CommandBuffer commandBuffer = logicalDevice.allocateCommandBuffers(allocInfo).front();
        constexpr vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        const vk::BufferCopy copyRegion(0, 0, size);

        commandBuffer.begin(beginInfo);
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
        commandBuffer.end();

        const vk::SubmitInfo submitInfo({}, {}, commandBuffer, {});
        graphicsQueue.submit(submitInfo);
        graphicsQueue.waitIdle();

        logicalDevice.freeCommandBuffers(commandPool, commandBuffer);
    }
}
