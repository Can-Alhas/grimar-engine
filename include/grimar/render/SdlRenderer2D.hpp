// ~ Grimar Engine ~
#pragma once

#include <grimar/render/Renderer2D.hpp>

struct SDL_Renderer;

namespace grimar::render {

    class SdlRenderer2D final : public Renderer2D {
    public:
        SdlRenderer2D() = default;
        ~SdlRenderer2D() override;

        bool Init(platform::Window& window, const Renderer2DDesc& desc) noexcept override;

        void BeginFrame() noexcept override;
        void Clear(Color color) noexcept override;
        void DrawRect(RectF rect, Color color, Layer layer = 0) noexcept override;
        void EndFrame() noexcept override;

    private:
        void Shutdown() noexcept;

    private:
        SDL_Renderer* m_renderer{nullptr};
    };

}