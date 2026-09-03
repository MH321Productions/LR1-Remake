#ifndef LR1_REMAKE_MODELDATA_HPP
#define LR1_REMAKE_MODELDATA_HPP

#include <vector>
#include <concepts>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

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

    struct Simple3DColorTextureVertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static const std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        static const std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

        bool operator==(const Simple3DColorTextureVertex& other) const;
    };
}

namespace std {
    template<> struct hash<LR1_Remake::Simple3DColorTextureVertex> {
        size_t operator()(const LR1_Remake::Simple3DColorTextureVertex& vertex) noexcept {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

#endif //LR1_REMAKE_MODELDATA_HPP
