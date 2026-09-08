#include <LR1-remake/renderer/data.hpp>

using namespace std;

namespace LR1_Remake {
    const vector<vk::VertexInputBindingDescription> Simple2DColorVertex::bindingDescriptions = {
        vk::VertexInputBindingDescription(0, sizeof(Simple2DColorVertex), vk::VertexInputRate::eVertex)
    };
    const vector<vk::VertexInputAttributeDescription> Simple2DColorVertex::attributeDescriptions = {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Simple2DColorVertex, pos)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple2DColorVertex, color))
    };

    const vector<vk::VertexInputBindingDescription> Simple2DColorTextureVertex::bindingDescriptions = {
        vk::VertexInputBindingDescription(0, sizeof(Simple2DColorTextureVertex), vk::VertexInputRate::eVertex)
    };
    const vector<vk::VertexInputAttributeDescription> Simple2DColorTextureVertex::attributeDescriptions = {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Simple2DColorTextureVertex, pos)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple2DColorTextureVertex, color)),
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Simple2DColorTextureVertex, texCoord))
    };

    const vector<vk::VertexInputBindingDescription> Simple3DColorTextureVertex::bindingDescriptions = {
        vk::VertexInputBindingDescription(0, sizeof(Simple3DColorTextureVertex), vk::VertexInputRate::eVertex)
    };
    const vector<vk::VertexInputAttributeDescription> Simple3DColorTextureVertex::attributeDescriptions = {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple3DColorTextureVertex, pos)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple3DColorTextureVertex, color)),
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Simple3DColorTextureVertex, texCoord))
    };

    bool Simple3DColorTextureVertex::operator==(const Simple3DColorTextureVertex &other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }

    const vector<vk::VertexInputBindingDescription> Simple3DColorNormalVertex::bindingDescriptions = {
        vk::VertexInputBindingDescription(0, sizeof(Simple3DColorNormalVertex), vk::VertexInputRate::eVertex)
    };
    const vector<vk::VertexInputAttributeDescription> Simple3DColorNormalVertex::attributeDescriptions = {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple3DColorNormalVertex, pos)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Simple3DColorNormalVertex, normal)),
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Simple3DColorNormalVertex, texCoord))
    };

    bool Simple3DColorNormalVertex::operator==(const Simple3DColorNormalVertex &other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
}
