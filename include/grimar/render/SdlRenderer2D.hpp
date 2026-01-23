// ~ Grimar Engine ~
#pragma once

#include <vector>
#include <grimar/render/Renderer2D.hpp>

#include "grimar/render/Camera2D.hpp"

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

        void SetCamera(const Camera2D* camera) noexcept override;

    private:
        void Shutdown() noexcept;

        struct DrawCmd {
            RectF rect;
            Color color;
            Layer layer;
        };
    private:
        SDL_Renderer* m_renderer{nullptr};
        const Camera2D* m_camera{nullptr};
        std::vector<DrawCmd> m_queue;

    };

}