#ifndef LR1_REMAKE_MODELDATA_HPP
#define LR1_REMAKE_MODELDATA_HPP

#include <vector>
#include <concepts>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace LR1_Remake {
    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    template<typename TData>
    concept IVertex = requires
    {
        {TData::bindingDescriptions} -> std::convertible_to<std::vector<vk::VertexInputBindingDescription>>;
        {TData::attributeDescriptions} -> std::convertible_to<std::vector<vk::VertexInputAttributeDescription>>;
    };

    struct Simple2DColorVertex {
        glm::vec2 pos;
        glm::vec3 color;

        static const std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        static const std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    };

    struct Simple2DColorTextureVertex {
        glm::vec2 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static const std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        static const std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    };
}

#endif //LR1_REMAKE_MODELDATA_HPP
