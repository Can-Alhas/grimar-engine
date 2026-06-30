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
            GRIMAR_LOG_ERROR("{}", SDL_GetError());
            return false;
        }

        // Memory optimization
        // (reallocation) stutter block
        m_queue.reserve(desc.maxDrawCommands);

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

        DrawCmd cmd{};
        cmd.type = DrawCmd::Type::Rect;
        cmd.rect = rect;
        cmd.color = c;
        cmd.layer = layer;
        m_queue.push_back(cmd);

    }

    void SdlRenderer2D::DrawLine(grimar::core::Vec2f start,
                                 grimar::core::Vec2f end,
                                 Color color,
                                 Layer layer) noexcept {
        DrawCmd cmd{};
        cmd.type = DrawCmd::Type::Line;
        cmd.lineStart = start;
        cmd.lineEnd = end;
        cmd.color = color;
        cmd.layer = layer;
        m_queue.push_back(cmd);
    }


    void SdlRenderer2D::Flush() noexcept {
        if (!m_renderer) return;

        if (m_queue.size() > m_currentCapacity) {
            m_currentCapacity = m_queue.capacity();
            GRIMAR_LOG_WARN(
                "Grimar_Render: Render queue limit exceeded! "
                "Capacity automatically increased to %zu."
                ,m_currentCapacity  );

        }

        // layer sort
        std::sort(m_queue.begin(), m_queue.end(),
                  [](const DrawCmd& a, const DrawCmd& b) {
                      return a.layer < b.layer;
                  });

        for (const auto& cmd : m_queue) {
            if (cmd.type == DrawCmd::Type::Rect) {
                RectF screen = cmd.rect;

                if (m_camera) {
                    const auto p = m_camera->WorldToScreen({cmd.rect.x, cmd.rect.y + cmd.rect.h});
                    screen.x = p.x;
                    screen.y = p.y;
                    screen.w = cmd.rect.w * m_camera->Zoom();
                    screen.h = cmd.rect.h * m_camera->Zoom();
                }

                SDL_FRect r{screen.x, screen.y, screen.w, screen.h};

                SDL_SetRenderDrawColor(m_renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
                SDL_RenderFillRectF(m_renderer, &r);
            }
            else if (cmd.type == DrawCmd::Type::Sprite) {
                RectF screen = cmd.rect;

                if (m_camera) {
                    const auto p = m_camera->WorldToScreen({cmd.rect.x, cmd.rect.y + cmd.rect.h});
                    screen.x = p.x;
                    screen.y = p.y;
                    screen.w = cmd.rect.w * m_camera->Zoom();
                    screen.h = cmd.rect.h * m_camera->Zoom();
                }

                if (!cmd.texture) {
                    GRIMAR_LOG_ERROR("Sprite cmd has null Texture2D pointer");
                    continue;
                }
                if (!cmd.texture->NativeTexture()) {
                    GRIMAR_LOG_ERROR("Sprite cmd has null SDL_Texture*");
                    continue;
                }
                if (cmd.src.w <= 0 || cmd.src.h <= 0) {
                    GRIMAR_LOG_ERROR("Sprite cmd has invalid source rect");
                    continue;
                }

                SDL_Rect src{cmd.src.x, cmd.src.y, cmd.src.w, cmd.src.h};
                SDL_FRect dst{screen.x, screen.y, screen.w, screen.h};

                const int rc = SDL_RenderCopyF(
                    m_renderer,
                    cmd.texture->NativeTexture(),
                    &src,
                    &dst
                );

                if (rc != 0) {
                    GRIMAR_LOG_ERROR("{}" ,SDL_GetError());
                }
            }
            else if (cmd.type == DrawCmd::Type::Line) {
                auto start = cmd.lineStart;
                auto end = cmd.lineEnd;

                if (m_camera) {
                    start = m_camera->WorldToScreen(start);
                    end = m_camera->WorldToScreen(end);
                }

                SDL_SetRenderDrawColor(m_renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
                SDL_RenderDrawLineF(m_renderer, start.x, start.y, end.x, end.y);
            }
        }

        m_queue.clear();
    }

    void SdlRenderer2D::Present() noexcept {
        if (!m_renderer) {
            return;
        }

        SDL_RenderPresent(m_renderer);
    }

    void SdlRenderer2D::EndFrame() noexcept {
        Flush();
        Present();
    }


    void * SdlRenderer2D::NativeHandle() noexcept {
        return m_renderer;
    }

    void SdlRenderer2D::DrawSprite(const grimar::assets::Texture2D& texture, RectI src, RectF dst,Layer layer) noexcept {
        DrawCmd cmd{};
        cmd.type  = DrawCmd::Type::Sprite;
        cmd.rect  = dst;
        cmd.layer = layer;
        cmd.texture = &texture;
        cmd.src     = src;
        m_queue.push_back(cmd);

    }
}
