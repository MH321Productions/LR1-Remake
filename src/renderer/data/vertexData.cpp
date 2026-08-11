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
}
