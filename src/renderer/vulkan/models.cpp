#include <unordered_map>

#include <LR1-remake/renderer/vulkan.hpp>
#include <LR1-remake/app.hpp>

#include <tiny_obj_loader.h>

using namespace std;

namespace LR1_Remake {
    bool VulkanBackend::loadModel() {
        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;
        string warn, err;

        unordered_map<Simple3DColorTextureVertex, uint32_t> uniqueVertices{};
        size_t numVertices = 0;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, main.res.getResourcePath("viking_room", ResourceType::Model).string().c_str())) {
            main.log.fatal << "Couldn't load model file: " << warn << ", " << err << endl;
            return false;
        }

        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index: shape.mesh.indices) {
                Simple3DColorTextureVertex vertex {
                    .pos = {
                        attrib.vertices.at(3 * index.vertex_index + 0),
                        attrib.vertices.at(3 * index.vertex_index + 1),
                        attrib.vertices.at(3 * index.vertex_index + 2),
                    },
                    .color = {1.0f, 1.0f, 1.0f},
                    .texCoord = {
                        attrib.texcoords.at(2 * index.texcoord_index + 0),
                        1.0f - attrib.texcoords.at(2 * index.texcoord_index + 1)
                    }
                };

                if (!uniqueVertices.contains(vertex)) {
                    uniqueVertices.insert({vertex, static_cast<uint32_t>(vertices.size())});
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices.at(vertex));
                numVertices++;
            }
        }

        main.log.debug << "Loaded model with " << numVertices << " vertices, deduplicated to " << vertices.size() << endl;

        return true;
    }
}
