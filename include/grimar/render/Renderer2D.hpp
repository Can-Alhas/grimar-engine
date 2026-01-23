// ~ Grimar Engine ~


#pragma once

#include <cstdint>


#include "grimar/render/Color.hpp"
#include "grimar/render/Rect.hpp"



namespace grimar::platform {
    class Window;
}

namespace grimar::render {

    class Camera2D;

    // Sorting layer (simple integer for now)
    using Layer = std::int32_t;

    struct Renderer2DDesc {
        bool vsync = true;
    };

    // Backend-independent 2D renderer interface.
    class Renderer2D {
    public:
        virtual ~Renderer2D() = default;

        Renderer2D(const Renderer2D& ) = delete;
        Renderer2D& operator=(const Renderer2D& ) = delete;

        // Must be called once before rendering.
        // ✅ PURE VIRTUAL (interface)
        virtual bool Init(platform::Window& window, const Renderer2DDesc& desc) noexcept = 0;
        virtual void BeginFrame() noexcept = 0;
        virtual void Clear(Color color) noexcept = 0;
        virtual void DrawRect(RectF rect, Color color, Layer layer = 0) noexcept = 0;
        void DrawRect(float x, float y, float w, float h,
                      Color color, Layer layer = 0 ) noexcept {
            DrawRect(RectF{x, y, w, h}, color, layer);
        }
        virtual void EndFrame() noexcept = 0;

        virtual void SetCamera(const Camera2D* camera) noexcept = 0;

    protected:
        Renderer2D() = default;
    };
}
