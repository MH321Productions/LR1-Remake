#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::createGraphicsPipeline() {
        const vector<uint8_t> vertexCode = main.res.loadResource("first", ResourceType::VertexShader);
        const vector<uint8_t> fragmentCode = main.res.loadResource("first", ResourceType::FragmentShader);

        try {
            const vk::ShaderModule vertexModule = createShaderModule(vertexCode);
            const vk::ShaderModule fragmentModule = createShaderModule(fragmentCode);

            vector shaderStages = {
                vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexModule, "main"),
                vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentModule, "main")
            };

            logicalDevice.destroyShaderModule(vertexModule);
            logicalDevice.destroyShaderModule(fragmentModule);
        } catch (runtime_error& e) {
            main.log.fatal << "Couldn't create graphics pipeline: " << e.what() << endl;
            return false;
        }

        return true;
    }

    vk::ShaderModule VulkanBackend::createShaderModule(const vector<uint8_t>& code) {
        vector<uint32_t> intCode(code.size() / 4 + 1);
        memcpy(intCode.data(), code.data(), code.size());

        const vk::ShaderModuleCreateInfo createInfo({}, intCode);
        return logicalDevice.createShaderModule(createInfo);
    }
}
