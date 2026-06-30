// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace grimar::assets {

    using TileId = std::int32_t;

    struct TileDefinition {
        std::string spriteName{};
        bool solid{false};
    };

    class TileMap {
    public:
        TileMap() = default;
        ~TileMap() = default;

        TileMap(const TileMap&) = delete;
        TileMap& operator=(const TileMap&) = delete;

        bool Create(int width,
                    int height,
                    int tileWidth,
                    int tileHeight,
                    std::string spriteSheetPath) noexcept;
        bool Load(const std::string& jsonPath) noexcept;
        bool Save(const std::string& jsonPath) const noexcept;
        void Clear() noexcept;

        [[nodiscard]] int Width() const noexcept { return m_width; }
        [[nodiscard]] int Height() const noexcept { return m_height; }
        [[nodiscard]] int TileWidth() const noexcept { return m_tileWidth; }
        [[nodiscard]] int TileHeight() const noexcept { return m_tileHeight; }
        [[nodiscard]] const std::string& SpriteSheetPath() const noexcept { return m_spriteSheetPath; }

        void SetSpriteSheetPath(std::string spriteSheetPath) noexcept;
        bool SetDefinition(TileId id, TileDefinition definition) noexcept;
        bool SetTile(int x, int y, TileId id) noexcept;
        [[nodiscard]] TileId TileAt(int x, int y) const noexcept;
        [[nodiscard]] const TileDefinition* GetDefinition(TileId id) const noexcept;
        [[nodiscard]] bool IsSolid(TileId id) const noexcept;
        [[nodiscard]] bool IsSolidAt(int x, int y) const noexcept;

        template <typename Fn>
        void ForEachDefinition(Fn&& fn) const {
            for (const auto& [id, definition] : m_definitions) {
                fn(id, definition);
            }
        }

    private:
        int m_width{0};
        int m_height{0};
        int m_tileWidth{0};
        int m_tileHeight{0};
        std::string m_spriteSheetPath{};
        std::vector<TileId> m_data{};
        std::unordered_map<TileId, TileDefinition> m_definitions{};
    };
}
