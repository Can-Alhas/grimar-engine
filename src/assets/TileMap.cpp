// ~ Grimar Engine ~

#include "grimar/assets/TileMap.hpp"

#include <exception>
#include <fstream>

#include <nlohmann/json.hpp>

#include "grimar/core/Log.hpp"

namespace grimar::assets {
    namespace {
        [[nodiscard]] bool ReadPositiveInt(const nlohmann::json& data,
                                           const char* field,
                                           int& out) noexcept {
            if (!data.contains(field) || !data[field].is_number_integer()) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: missing integer field '{}'", field);
                return false;
            }

            out = data[field].get<int>();
            if (out <= 0) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: integer field '{}' must be positive", field);
                return false;
            }

            return true;
        }
    }

    bool TileMap::Load(const std::string& jsonPath) noexcept {
        Clear();

        try {
            std::ifstream file(jsonPath);
            if (!file.is_open()) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: could not open json file");
                return false;
            }

            nlohmann::json data{};
            file >> data;

            if (!ReadPositiveInt(data, "width", m_width) ||
                !ReadPositiveInt(data, "height", m_height) ||
                !ReadPositiveInt(data, "tileWidth", m_tileWidth) ||
                !ReadPositiveInt(data, "tileHeight", m_tileHeight)) {
                Clear();
                return false;
            }

            if (data.contains("spriteSheet") && data["spriteSheet"].is_string()) {
                m_spriteSheetPath = data["spriteSheet"].get<std::string>();
            }

            if (!data.contains("tiles") || !data["tiles"].is_object()) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: missing tiles object");
                Clear();
                return false;
            }

            const auto& tiles = data["tiles"];
            for (auto it = tiles.begin(); it != tiles.end(); ++it) {
                TileId id{};

                try {
                    id = static_cast<TileId>(std::stoi(it.key()));
                } catch (const std::exception&) {
                    GRIMAR_LOG_WARN("TileMap::Load skipped tile definition: invalid tile id");
                    continue;
                }

                const auto& value = it.value();
                if (!value.is_object()) {
                    GRIMAR_LOG_WARN("TileMap::Load skipped tile definition: value is not object");
                    continue;
                }

                TileDefinition definition{};
                if (value.contains("sprite") && value["sprite"].is_string()) {
                    definition.spriteName = value["sprite"].get<std::string>();
                }
                if (value.contains("solid") && value["solid"].is_boolean()) {
                    definition.solid = value["solid"].get<bool>();
                }

                m_definitions.insert_or_assign(id, definition);
            }

            if (!data.contains("data") || !data["data"].is_array()) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: missing data array");
                Clear();
                return false;
            }

            const auto& tileData = data["data"];
            const auto expectedCount = static_cast<std::size_t>(m_width * m_height);
            if (tileData.size() != expectedCount) {
                GRIMAR_LOG_ERROR("TileMap::Load failed: data size does not match width * height");
                Clear();
                return false;
            }

            m_data.reserve(expectedCount);
            for (const auto& value : tileData) {
                if (!value.is_number_integer()) {
                    GRIMAR_LOG_ERROR("TileMap::Load failed: data contains non-integer tile id");
                    Clear();
                    return false;
                }

                const auto id = value.get<TileId>();
                if (id != 0 && m_definitions.find(id) == m_definitions.end()) {
                    GRIMAR_LOG_ERROR("TileMap::Load failed: data references unknown tile id");
                    Clear();
                    return false;
                }

                m_data.push_back(id);
            }

            GRIMAR_LOG_INFO("TileMap loaded");
            return true;
        } catch (const std::exception&) {
            GRIMAR_LOG_ERROR("TileMap::Load failed: json parse error");
            Clear();
            return false;
        }
    }

    void TileMap::Clear() noexcept {
        m_width = 0;
        m_height = 0;
        m_tileWidth = 0;
        m_tileHeight = 0;
        m_spriteSheetPath.clear();
        m_data.clear();
        m_definitions.clear();
    }

    TileId TileMap::TileAt(int x, int y) const noexcept {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
            return 0;
        }

        const auto index = static_cast<std::size_t>((y * m_width) + x);
        if (index >= m_data.size()) {
            return 0;
        }

        return m_data[index];
    }

    const TileDefinition* TileMap::GetDefinition(TileId id) const noexcept {
        const auto it = m_definitions.find(id);
        if (it == m_definitions.end()) {
            return nullptr;
        }

        return &it->second;
    }

    bool TileMap::IsSolid(TileId id) const noexcept {
        const auto* definition = GetDefinition(id);
        return definition && definition->solid;
    }

    bool TileMap::IsSolidAt(int x, int y) const noexcept {
        return IsSolid(TileAt(x, y));
    }
}
