// ~ Grimar Engine ~

#pragma once

#include <cstdint>

namespace grimar::render {

    struct Vec2f {
        float x{0.f};
        float y{0.f};
    };


    class Camera2D {
    public:
        // viewport: screen pixel size (1270x720)
        void SetViewport(int width, int height) noexcept {
            m_viewW = width;
            m_viewH = height;
        }

        void SetPosition(Vec2f p) noexcept { m_pos = p; }
        Vec2f Position() const noexcept { return m_pos; }

        // zoom 1.0 = normal, 2.0 = 2x zoomin, 0.5 = 2x zoomout
        void SetZoom(float z) noexcept { m_zoom = (z > 0.f) ? z: 0.0001f; }
        [[nodiscard]] float Zoom() const noexcept { return m_zoom; }

        // World -> Screen (pixel space)
        // camera position = screen center
        Vec2f WorldToScreen(Vec2f world) const noexcept {
            // World relative to camera
            const float rx = (world.x - m_pos.x) * m_zoom;
            const float ry = (world.y - m_pos.y) * m_zoom;

            // to screen center
            return Vec2f{
                rx + (m_viewW * 0.5f),
                (m_viewH * 0.5f) - ry // Y inverted
            };
        }


        // Screen -> World (opposite, to mouse picking )
        Vec2f ScreenToWorld(Vec2f screen) const noexcept {

            const float rx = (screen.x - (m_viewW * 0.5f)) / m_zoom;

            //narrowing!!!
            const float ry = ((m_viewH * 0.5f) - screen.y) / m_zoom;


            return Vec2f {
                rx + m_pos.x,
                ry + m_pos.y
            };
        }



    private:
        Vec2f m_pos{0.f, 0.f};
        float m_zoom{1.f};
        int m_viewW{1280};
        int m_viewH{720};
    };
}