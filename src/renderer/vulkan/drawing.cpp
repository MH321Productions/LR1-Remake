#include <limits>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createFramebuffers() {
        swapChainFrameBuffers.reserve(swapChainImageViews.size());
        try {
            for (const vk::ImageView& view: swapChainImageViews) {
                vector attachments = {view};
                vk::FramebufferCreateInfo framebufferInfo({}, renderPass, attachments, swapChainExtent.width, swapChainExtent.height, 1);
                swapChainFrameBuffers.push_back(logicalDevice.createFramebuffer(framebufferInfo));
            }
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't create framebuffers: " << e.what() << endl;
            return false;
        }

        return true;
    }

    bool VulkanBackend::createCommandPool() {
        const auto [graphicsFamily, presentFamily] = findQueueFamilies(physicalDevice);
        const vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamily.value());
        checkResult(commandPool, logicalDevice.createCommandPool(poolInfo));
    }

    bool VulkanBackend::createCommandBuffers() {
        const vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, maxFramesInFlight);
        checkResult(commandBuffers, logicalDevice.allocateCommandBuffers(allocInfo));
    }

    bool VulkanBackend::recordCommandBuffer(const vk::CommandBuffer& cmd, const uint32_t& imageIndex) {
        constexpr vk::CommandBufferBeginInfo beginInfo({}, nullptr);
        checkFunc(cmd.begin(beginInfo), "Couldn't begin command buffer recording");

        vector clearColor = {vk::ClearValue({0.0f, 0.0f, 0.0f, 1.0f})};
        const vk::RenderPassBeginInfo renderPassInfo(renderPass, swapChainFrameBuffers.at(imageIndex), {{0, 0}, swapChainExtent}, clearColor);
        cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        const vector viewport{vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f)};
        const vector scissor{vk::Rect2D({0, 0}, swapChainExtent)};
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        cmd.draw(3, 1, 0, 0); //This is it

        cmd.endRenderPass();
        checkFunc(cmd.end(), "Couldn't end command buffer recording");

        return true;
    }

    bool VulkanBackend::createSyncObjects() {
        constexpr vk::SemaphoreCreateInfo semaphoreInfo{};
        constexpr vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

        imageAvailableSemaphores.reserve(maxFramesInFlight);
        renderFinishedSemaphores.reserve(maxSwapChainImages);
        inFlightFences.reserve(maxFramesInFlight);

        try {
            for (uint32_t i = 0; i < maxFramesInFlight; i++) {
                imageAvailableSemaphores.push_back(logicalDevice.createSemaphore(semaphoreInfo));
                inFlightFences.push_back(logicalDevice.createFence(fenceInfo));
            }
            for (uint32_t i = 0; i < maxSwapChainImages; i++) {
                renderFinishedSemaphores.push_back(logicalDevice.createSemaphore(semaphoreInfo));
            }
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't create sync objects: " << e.what() << endl;
            return false;
        }

        return true;
    }

    bool VulkanBackend::drawFrame() {
        const vector fences = {inFlightFences.at(currentFrame)};
        vk::Result res = logicalDevice.waitForFences(fences, true, numeric_limits<uint64_t>::max());

        const vk::ResultValue<uint32_t> imageRes = logicalDevice.acquireNextImageKHR(swapChain, numeric_limits<uint64_t>::max(), imageAvailableSemaphores.at(currentFrame));
        if (imageRes.result == vk::Result::eErrorOutOfDateKHR) {
            recreateSwapChain();
            return true;
        }
        if (imageRes.result != vk::Result::eSuccess && imageRes.result != vk::Result::eSuboptimalKHR) {
            main.log.fatal << "Couldn't acquire swap chain image: " << vk::to_string(res) << endl;
            return false;
        }

        const uint32_t imageIndex = imageRes.value;
        logicalDevice.resetFences(fences);

        commandBuffers.at(currentFrame).reset();
        recordCommandBuffer(commandBuffers.at(currentFrame), imageIndex);

        vector waitSemaphores = {imageAvailableSemaphores.at(currentFrame)};
        vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vector cmdBuffers = {commandBuffers.at(currentFrame)};
        vector signalSemaphores = {renderFinishedSemaphores.at(imageIndex)};
        const vk::SubmitInfo submitInfo(waitSemaphores, waitStages, cmdBuffers, signalSemaphores);
        checkFunc(graphicsQueue.submit(submitInfo, inFlightFences.at(currentFrame)), "Couldn't submit the draw command buffer");

        vector swapChains = {swapChain};
        vector imageIndices = {imageIndex};
        const vk::PresentInfoKHR presentInfo(signalSemaphores, swapChains, imageIndices);
        res = presentQueue.presentKHR(presentInfo);
        if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (res != vk::Result::eSuccess) {
            main.log.fatal << "Couldn't present swap chain image: " << vk::to_string(res) << endl;
            return false;
        }

        currentFrame = (currentFrame + 1) % maxFramesInFlight;

        return true;
    }

    void VulkanBackend::triggerResize() {
        framebufferResized = true;
    }
}
