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

        //m_queue.push_back(DrawCmd{rect, c, layer});

        m_queue.push_back( DrawCmd{
            DrawCmd::Type::Rect, rect, c, layer, nullptr, {}
        });

    }


    void SdlRenderer2D::EndFrame() noexcept {
    if (!m_renderer) return;

    std::sort(m_queue.begin(), m_queue.end(),
        [](const DrawCmd& a, const DrawCmd& b) {
            return a.layer < b.layer;
        });

    for (const auto& cmd : m_queue) {
        RectF screen = cmd.rect;

        if (m_camera) {
            const auto p = m_camera->WorldToScreen({cmd.rect.x, cmd.rect.y});
            screen.x = p.x;
            screen.y = p.y;
            screen.w = cmd.rect.w * m_camera->Zoom();
            screen.h = cmd.rect.h * m_camera->Zoom();
        }

        if (cmd.type == DrawCmd::Type::Rect) {
            SDL_FRect r{screen.x, screen.y, screen.w, screen.h};

            SDL_SetRenderDrawColor(m_renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
            SDL_RenderFillRectF(m_renderer, &r);
        }
        else if (cmd.type == DrawCmd::Type::Sprite) {

            // ---- DEBUG MARKER:----
            SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
            SDL_FRect marker{20.f, 20.f, 20.f, 20.f};
            SDL_RenderFillRectF(m_renderer, &marker);
            // debug

            // ---- Validate texture ----
            if (!cmd.texture) {
                GRIMAR_LOG_ERROR("Sprite cmd has null Texture2D pointer");
                continue;
            }
            if (!cmd.texture->NativeTexture()) {
                GRIMAR_LOG_ERROR("Sprite cmd has null SDL_Texture*");
                continue;
            }

            SDL_FRect dst{screen.x, screen.y, screen.w, screen.h};

            // ---- DEBUG BYPASS: ignore srcRect and draw full texture ----
            // If sprite appears with nullptr src, the bug is cmd.src values.
            const int rc = SDL_RenderCopyF(
                m_renderer,
                cmd.texture->NativeTexture(),
                nullptr,   // ✅ full texture
                &dst
            );

            if (rc != 0) {
                GRIMAR_LOG_ERROR(SDL_GetError());
            }

            // ---- Optional: log srcRect once (commented to avoid spam) ----
            // GRIMAR_LOG_INFO("srcRect w/h check");
            // GRIMAR_ASSERT(cmd.src.w > 0 && cmd.src.h > 0);
        }
    }

    SDL_RenderPresent(m_renderer);
    m_queue.clear();
}


    void * SdlRenderer2D::NativeHandle() noexcept {
        return m_renderer;
    }

    void SdlRenderer2D::DrawSprite(const grimar::assets::Texture2D& texture, RectI src, RectF dst,Layer layer) noexcept {

        GRIMAR_LOG_INFO("DrawSprite queued");

        DrawCmd cmd{};
        cmd.type  = DrawCmd::Type::Sprite;
        cmd.rect  = dst;
        cmd.layer = layer;
        cmd.texture = &texture;
        cmd.src     = src;
        m_queue.push_back(cmd);

    }
}
