// ~ Grimar Engine ~

#pragma once

namespace grimar::assets {
    class SpriteSheet;
}

namespace grimar::render {
    class Renderer2D;
}

namespace grimar::engine {
    class World;

    class RenderSystem {

    public:
        RenderSystem() = default;
        ~RenderSystem() = default;

        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;

        // World icindeki Transform2D + SpriteRenderer componentlerine sahip entity'leri cizer.
        //
        // World:
        //   Entity ve component storage burada.
        //
        // SpriteSheet:
        //   SpriteRenderer.spriteName -> SpriteFrame lookup burada yapilir.
        //
        // Renderer2D:
        //   Son cizim komutunu renderer queue'ya gonderir.
        void Render(World& world,
                    const grimar::assets::SpriteSheet& spriteSheet,
                    grimar::render::Renderer2D& renderer) noexcept;
    };
}
