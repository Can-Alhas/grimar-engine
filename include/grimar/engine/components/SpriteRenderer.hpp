// ~ Grimar Engine ~


#pragma once

#include <string>
#include <utility>

#include "grimar/core/Math2D.hpp"
#include "grimar/render/Renderer2D.hpp"

namespace grimar::engine {

    struct SpriteRenderer {
        // SpriteSheet icindeki frame adi.
        // Ornek: "player_idle_0", "player_run_1".
        //
        // Simdilik string kullaniyoruz cunku SpriteSheet name -> frame mapping ile calisiyor.
        // Ileride performans icin string yerine SpriteId / hashed id / handle kullanabiliriz.

        std::string spriteName{};

        // Sprite'in world-space icinde hangi boyutta cizilecegi.
        // Transform2D position verir, SpriteRenderer size verir.
        //
        // Ornek:
        // position = {200, 150}
        // size     = {256, 256}
        grimar::core::Vec2f size{1.f, 1.f};

        // Render layer.
        // Kucuk layer once, buyuk layer sonra cizilir.
        // Renderer2D zaten layer sorting yapiyor.
        grimar::render::Layer layer{0};

        // Entity gecici olarak gizlenmek istenirse false yapilir.
        // Component durur ama RenderSystem cizmez
        bool visible{true};

        // Sadece sprite name set etmek icin kisa yardimci
        void SetSprite(std::string name) {
            spriteName = std::move(name);
        }

        // Draw size ayarlamak icin yardimci.
        void SetSize(float w, float h) noexcept {
            size = {w, h};
        }
    };
}
