#ifndef LR1_REMAKE_RES_HPP
#define LR1_REMAKE_RES_HPP

#include <cinttypes>
#include <filesystem>
#include <array>
#include <string>
#include <vector>

namespace LR1_Remake {
    class Main;

    enum class ResourceType: uint8_t {
        VertexShader,
        FragmentShader,

        Undefined
    };

    struct ResourceInfo {
        std::string folder;
        std::string extension;
    };

    class ResourceHandler {
        public:
            explicit ResourceHandler(Main& main) : main(main) {}

            [[nodiscard]] std::filesystem::path getResourcePath(const std::string& name, ResourceType type = ResourceType::Undefined) const;

            [[nodiscard]] static std::vector<uint8_t> loadResource(const std::filesystem::path& filepath);
            [[nodiscard]] std::vector<uint8_t> loadResource(const std::string& name, const ResourceType type = ResourceType::Undefined) const { return loadResource(getResourcePath(name, type)); }

        private:
            static const std::array<ResourceInfo, static_cast<size_t>(ResourceType::Undefined)> infos;
            static constexpr std::string resDir = "res";

            Main& main;
    };
}

#endif //LR1_REMAKE_RES_HPP
