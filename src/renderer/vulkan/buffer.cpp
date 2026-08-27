#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    const std::vector<Simple2DColorTextureVertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const vector<uint32_t> VulkanBackend::indices = {0, 1, 2, 2, 3, 0};

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
        const vk::DeviceSize bufferSize = sizeof(Simple2DColorTextureVertex) * vertices.size();

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

    bool VulkanBackend::createIndexBuffer() {
        const vk::DeviceSize bufferSize = sizeof(uint32_t) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        void* data = logicalDevice.mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, indices.data(), bufferSize);
        logicalDevice.unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);
        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        logicalDevice.destroyBuffer(stagingBuffer);
        logicalDevice.free(stagingBufferMemory);

        return true;
    }

    bool VulkanBackend::createUniformBuffers() {
        uniformBuffers.resize(maxFramesInFlight);
        uniformBufferMemories.resize(maxFramesInFlight);
        mappedUniformBuffers.resize(maxFramesInFlight);

        for (int i = 0; i < maxFramesInFlight; i++) {
            constexpr vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
            if (!createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers.at(i), uniformBufferMemories.at(i))) return false;
            mappedUniformBuffers.at(i) = logicalDevice.mapMemory(uniformBufferMemories.at(i), 0, bufferSize);
        }

        return true;
    }

    bool VulkanBackend::createDescriptorPool() {
        constexpr array poolSizes{
            vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, maxFramesInFlight),
            vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, maxFramesInFlight)
        };
        const vk::DescriptorPoolCreateInfo poolInfo({}, maxFramesInFlight, poolSizes);
        checkResult(descriptorPool, logicalDevice.createDescriptorPool(poolInfo));
    }

    bool VulkanBackend::createDescriptorSets() {
        vector layouts(maxFramesInFlight, descriptorSetLayout);
        const vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, layouts);
        checkFunc(descriptorSets = logicalDevice.allocateDescriptorSets(allocInfo), "Couldn't allocate descriptor sets");

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            vk::DescriptorBufferInfo bufferInfo(uniformBuffers.at(i), 0, sizeof(UniformBufferObject));
            vk::DescriptorImageInfo imageInfo(textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal);

            array writeInfos{
                vk::WriteDescriptorSet(descriptorSets.at(i), 0, 0, vk::DescriptorType::eUniformBuffer, {}, bufferInfo, {}),
                vk::WriteDescriptorSet(descriptorSets.at(i), 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, {}, {})
            };
            logicalDevice.updateDescriptorSets(writeInfos, {});
        }

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
        const vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        const vk::BufferCopy copyRegion(0, 0, size);
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    vk::CommandBuffer VulkanBackend::beginSingleTimeCommands() const {
        const vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
        const vk::CommandBuffer commandBuffer = logicalDevice.allocateCommandBuffers(allocInfo).front();
        constexpr vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VulkanBackend::endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const {
        commandBuffer.end();

        const vk::SubmitInfo submitInfo({}, {}, commandBuffer, {});
        graphicsQueue.submit(submitInfo);
        graphicsQueue.waitIdle();

        logicalDevice.freeCommandBuffers(commandPool, commandBuffer);
    }
}
