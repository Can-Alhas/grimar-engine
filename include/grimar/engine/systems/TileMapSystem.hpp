// ~ Grimar Engine ~

#pragma once

#include <cstdint>

#include "grimar/core/Math2D.hpp"
#include "grimar/render/Renderer2D.hpp"

namespace grimar::assets {
    class SpriteSheet;
    class TileMap;
}

namespace grimar::engine {

    class World;

    class TileMapSystem {
    public:
        TileMapSystem() = default;
        ~TileMapSystem() = default;

        TileMapSystem(const TileMapSystem&) = delete;
        TileMapSystem& operator=(const TileMapSystem&) = delete;

        void Render(const grimar::assets::TileMap& tileMap,
                    const grimar::assets::SpriteSheet& spriteSheet,
                    grimar::render::Renderer2D& renderer,
                    grimar::core::Vec2f origin,
                    grimar::render::Layer layer) noexcept;

        [[nodiscard]] std::uint32_t CreateSolidColliders(const grimar::assets::TileMap& tileMap,
                                                         World& world,
                                                         grimar::core::Vec2f origin) noexcept;
    };
}
