// ~ Grimar Engine ~


#pragma once

#include <cstddef>
#include <cstdint>


#include "grimar/core/Math2D.hpp"
#include "grimar/render/Color.hpp"
#include "grimar/render/Rect.hpp"

namespace grimar::assets {
    class Texture2D;
}

namespace grimar::platform {
    class Window;
}

namespace grimar::render {

    class Camera2D;

    // Sorting layer (simple integer for now)
    using Layer = std::int32_t;

    struct Renderer2DDesc {
        bool vsync{true};
        size_t maxDrawCommands{2048}; // default value maybe change
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
        virtual void Flush() noexcept = 0;
        virtual void Present() noexcept = 0;

        // -------------------------------------------------------------
#pragma region RECTANGLE DRAWING INTERFACE
        // Core pure virtual draw call. No default arguments here to satisfy Clang-Tidy.
        virtual void DrawRect(RectF rect, Color color, Layer layer) noexcept = 0;

        // Non-virtual helper overload allowing default layer arguments safely.
        void DrawRect(RectF rect, Color color) noexcept {
            DrawRect(rect, color, 0);
        }
        // Convenience overload for explicit float coordinates.
        void DrawRect(float x, float y, float w, float h, Color color, Layer layer = 0) noexcept {
            DrawRect(RectF{x, y, w, h}, color, layer);
        }
#pragma endregion

#pragma region LINE DRAWING INTERFACE
        virtual void DrawLine(grimar::core::Vec2f start,
                              grimar::core::Vec2f end,
                              Color color,
                              Layer layer) noexcept = 0;

        void DrawLine(grimar::core::Vec2f start,
                      grimar::core::Vec2f end,
                      Color color) noexcept {
            DrawLine(start, end, color, 0);
        }
#pragma endregion

        virtual void EndFrame() noexcept = 0;
        virtual void SetCamera(const Camera2D* camera) noexcept = 0;


        // Backend-specific native handle (e.g. SDL_Renderer*, OpenGL context ptr, etc.)
        virtual void* NativeHandle() noexcept = 0;

#pragma region SPRITE DRAWING INTERFACE
        // Core pure virtual sprite call. No default arguments to keep Clang-Tidy happy.
        virtual void DrawSprite(const grimar::assets::Texture2D& texture,
                                RectI src,
                                RectF dst,
                                Layer layer) noexcept = 0;
        // Helper overload: Accepts 3 parameters and internally passes 0 as the default layer.
        // With 3 parameters, it safely avoids any signature conflicts with the 4-parameter virtual function.
        void DrawSprite(const grimar::assets::Texture2D& texture,
                        RectI src,
                        RectF dst) noexcept {

            DrawSprite(texture, src, dst, 0);
        }
#pragma endregion

    protected:
        Renderer2D() = default;
    };
}
