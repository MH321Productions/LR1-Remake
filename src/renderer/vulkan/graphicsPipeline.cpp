#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createGraphicsPipeline(const std::vector<vk::VertexInputBindingDescription>& vertexBindingDescriptions, const std::vector<vk::VertexInputAttributeDescription>& vertexAttributeDescriptions) {
        const vector<uint8_t> vertexCode = main.res.loadResource("first", ResourceType::VertexShader);
        const vector<uint8_t> fragmentCode = main.res.loadResource("first", ResourceType::FragmentShader);

        try {
            const vk::ShaderModule vertexModule = createShaderModule(vertexCode);
            const vk::ShaderModule fragmentModule = createShaderModule(fragmentCode);

            vector shaderStages = {
                vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexModule, "main"),
                vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentModule, "main")
            };

            vector dynamicPipelineStages = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
            vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicPipelineStages);

            vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, vertexBindingDescriptions, vertexAttributeDescriptions);

            vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList, false);
            vector viewport{vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f)};
            vector scissor{vk::Rect2D({0, 0}, swapChainExtent)};
            vk::PipelineViewportStateCreateInfo viewportState({}, viewport, scissor);
            vk::PipelineRasterizationStateCreateInfo rasterizer(
                {},
                false,
                false,
                vk::PolygonMode::eFill,
                vk::CullModeFlagBits::eBack,
                vk::FrontFace::eCounterClockwise,
                false,
                0.0f,
                0.0f,
                0.0f,
                1.0f
            );

            vk::PipelineMultisampleStateCreateInfo multisampling( //TODO: Come back later
                {},
                vk::SampleCountFlagBits::e1,
                false,
                1.0f,
                nullptr,
                false,
                false
            );

            vector colorBlendAttachment{
                vk::PipelineColorBlendAttachmentState(
                    true,
                    vk::BlendFactor::eSrcAlpha,
                    vk::BlendFactor::eOneMinusSrcAlpha,
                    vk::BlendOp::eAdd,
                    vk::BlendFactor::eOne,
                    vk::BlendFactor::eZero,
                    vk::BlendOp::eAdd,
                    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
                )
            };
            vk::PipelineColorBlendStateCreateInfo colorBlending({}, false, vk::LogicOp::eCopy, colorBlendAttachment, {0.0f, 0.0f, 0.0f, 0.0f});

            vk::PipelineDepthStencilStateCreateInfo depthStencil({}, true, true, vk::CompareOp::eLess, false, false, {}, {}, 0.0f, 1.0f);

            vector<vk::PushConstantRange> pushConstantRanges;
            vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, descriptorSetLayout, pushConstantRanges);
            pipelineLayout = logicalDevice.createPipelineLayout(pipelineLayoutInfo);

            vk::GraphicsPipelineCreateInfo pipelineInfo(
                {},
                shaderStages,
                &vertexInputInfo,
                &inputAssembly,
                nullptr,
                &viewportState,
                &rasterizer,
                &multisampling,
                &depthStencil,
                &colorBlending,
                &dynamicState,
                pipelineLayout,
                renderPass,
                0,
                nullptr,
                -1
            );

            const vk::ResultValue<vk::Pipeline> res = logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo);
            if (res.result != vk::Result::eSuccess) throw runtime_error(vk::to_string((res.result)));

            graphicsPipeline = res.value;

            logicalDevice.destroyShaderModule(vertexModule);
            logicalDevice.destroyShaderModule(fragmentModule);
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't create graphics pipeline: " << e.what() << endl;
            return false;
        }

        return true;
    }

    bool VulkanBackend::createDescriptorSetLayout() {
        constexpr array bindings{
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),            //UBO Layout binding
            vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)    //Sampler Layout binding
        };

        const vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
        checkResult(descriptorSetLayout, logicalDevice.createDescriptorSetLayout(layoutInfo));
    }

    vk::ShaderModule VulkanBackend::createShaderModule(const vector<uint8_t>& code) const {
        const vk::ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<uint32_t const *>(code.data()));
        return logicalDevice.createShaderModule(createInfo);
    }

    bool VulkanBackend::createRenderPass() {
        vector colorAttachment{
            vk::AttachmentDescription(
                {},
                swapChainImageFormat,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::ePresentSrcKHR
            )
        };
        vector colorAttachmentRef{vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};

        vk::AttachmentDescription depthAttachment(
            {},
            findDepthFormat(),
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eDontCare,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        );
        constexpr vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

        array attachments {colorAttachment.front(), depthAttachment};

        vector subpass{vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentRef, {}, &depthAttachmentRef, {})};
        vector dependencies{
            vk::SubpassDependency(
                vk::SubpassExternal,
                0,
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
                vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
            )
        };
        const vk::RenderPassCreateInfo renderPassInfo({}, attachments, subpass, dependencies);

        checkResult(renderPass, logicalDevice.createRenderPass(renderPassInfo));
    }
}
