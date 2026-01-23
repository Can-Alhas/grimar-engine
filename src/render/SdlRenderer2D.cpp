// ~ Grimar Engine ~

#include "grimar/render/SdlRenderer2D.hpp"

#include <algorithm>

#include "grimar/platform/Window.hpp"
#include "grimar/core/Log.hpp"

#include <SDL.h>

namespace grimar::render {

    SdlRenderer2D::~SdlRenderer2D(){
        Shutdown();
    }

    bool SdlRenderer2D::Init(platform::Window& window,
                             const Renderer2DDesc &desc) noexcept {
        if (!window.IsValid()) {
            GRIMAR_LOG_ERROR("SdlRenderer2D::Init failed: window invalid");
            return false;
        }

        if (m_renderer) {
            return true; // already inir
        }

        const Uint32 flags =
            SDL_RENDERER_ACCELERATED |
                (desc.vsync ? SDL_RENDERER_PRESENTVSYNC : 0);

        m_renderer = SDL_CreateRenderer(window.NativeHandle(), -1, flags);
        if (!m_renderer) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            return false;
        }

        return true;

    }

    void SdlRenderer2D::Shutdown() noexcept {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
    }

    void SdlRenderer2D::SetCamera(const Camera2D* camera) noexcept {
        m_camera = camera;
    }

    void SdlRenderer2D::BeginFrame() noexcept {
        // now empty (batching vs.)
        m_queue.clear();
    }

    void SdlRenderer2D::Clear(Color c ) noexcept {
        if (!m_renderer) return;
        SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
        SDL_RenderClear(m_renderer);
    }


    void SdlRenderer2D::DrawRect(RectF rect, Color c, Layer layer ) noexcept {
        // simdilik camera yok: world == screen

        #pragma region SDL Rect Draw old
        /* if (!m_renderer) return;
        //
        // RectF screen = rect;
        //
        // if (m_camera) {
        //     const auto p = m_camera->WorldToScreen({rect.x, rect.y});
        //     screen.x = p.x;
        //     screen.y = p.y;
        //     screen.w = rect.w * m_camera->Zoom();
        //     screen.h = rect.h * m_camera->Zoom();
        // }
        //
        // SDL_FRect r{screen.x, screen.y, screen.w, screen.h};
        //
        // //SDL_FRect r{rect.x, rect.y, rect.w, rect.h};
        // SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
        // SDL_RenderFillRectF(m_renderer, &r); */
#pragma endregion

        m_queue.push_back(DrawCmd{rect, c, layer});

    }

    void SdlRenderer2D::EndFrame() noexcept {
        if (!m_renderer) return;

        std::sort(m_queue.begin(), m_queue.end(),
        [](const DrawCmd& a, const DrawCmd& b) {
                        return a.layer < b.layer;
            }
            );

        for (const auto& cmd: m_queue) {
            RectF screen = cmd.rect;

            if (m_camera) {
                const auto p = m_camera->WorldToScreen({cmd.rect.x, cmd.rect.y});
                screen.x = p.x;
                screen.y = p.y;
                screen.w = cmd.rect.w * m_camera->Zoom();
                screen.h = cmd.rect.h * m_camera->Zoom();
            }

            SDL_FRect r{screen.x, screen.y, screen.w, screen.h};
            SDL_SetRenderDrawColor(
                m_renderer,
                cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a
            );
            SDL_RenderFillRectF(m_renderer, &r);
        }


        SDL_RenderPresent(m_renderer);
    }


}
