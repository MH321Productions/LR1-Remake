#include <fstream>

#include <LR1-remake/util/res.hpp>
#include <LR1-remake/app.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1_Remake {
    const array<ResourceInfo, static_cast<size_t>(ResourceType::Undefined)> ResourceHandler::infos = {{
        {.folder = "shaders", .extension = ".vert.spv"},
        {.folder = "shaders", .extension = ".frag.spv"}
    }};

    path ResourceHandler::getResourcePath(const string& name, ResourceType type) const {
        if (type == ResourceType::Undefined) return main.rootDir / resDir / name;

        const auto&[folder, extension] = infos.at(static_cast<size_t>(type));

        return main.rootDir / resDir / folder / (name + extension);
    }

    vector<uint8_t> ResourceHandler::loadResource(const path& filepath) {
        vector<uint8_t> data(file_size(filepath));
        ifstream file(filepath, ios::binary);

        file.read(reinterpret_cast<char*>(data.data()), data.size());
        file.close();

        return data;
    }
}
