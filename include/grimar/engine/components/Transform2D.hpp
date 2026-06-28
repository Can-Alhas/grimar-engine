// ~ Grimar Engine ~

#pragma once

#include "grimar/core/Math2D.hpp"
#include "grimar/render/Camera2D.hpp"

namespace grimar::engine {

    using Vec2f = grimar::core::Vec2f;

    struct Transform2D {

        // World-Space position.
        // Platformer char, tile object, sprite entitities use this
        Vec2f position{0.f, 0.f};

        // Radyan veya derece kararini ileride netlestirebiliriz.
        // Simdilik renderer rotation kullanmadigi icin data olarak duruyor.
        float rotation{0.f};

        // 1,1 normal boyut demek.
        // Sprite scale veya physics scale icin ileride kullanilabilir
        Vec2f scale{1.f, 1.f};

        // Transform u default hale getirir.
        void Reset() noexcept {
            position = {0.f, 0.f};
            rotation = 0.f;
            scale = {1.f, 1.f};
        }

        // Sadece position set etmek icin kisa helper
        void SetPosition(float x, float y) noexcept {
            position = {x, y};
        }


        // Position i delta kadar hareket ettirir
        // Platformer movement veya camera takibi gibi yerlerde kullanisli
        void Translate(Vec2f delta) noexcept {
            position.x += delta.x;
            position.y += delta.y;
        }
    };
}

