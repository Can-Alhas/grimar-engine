// ~ Grimar Engine ~

#include "grimar/assets/Texture2D.hpp"

#include "grimar/render/Renderer2D.hpp"

#include "grimar/core/Log.hpp"

#include <SDL.h>

#include <SDL_image.h>

namespace grimar::assets {

    Texture2D::~Texture2D() noexcept {
        Destroy();
    }

    Texture2D::Texture2D(Texture2D &&other) noexcept {
        *this = std::move(other);
    }

    Texture2D & Texture2D::operator=(Texture2D &&other) noexcept {
        if (this == &other ) return *this;
        Destroy();

        m_tex  = other.m_tex;
        m_w    = other.m_w;
        m_h    = other.m_h;
        m_path = std::move(other.m_path);

        other.m_tex = nullptr;
        other.m_w   = 0;
        other.m_h   = 0;

        return *this;
    }

    bool Texture2D::LoadFromFile(grimar::render::Renderer2D &renderer, const std::string &path) noexcept {

        Destroy();

        // Renderer native handle must be SDL_Renderer* for SDL backend
        auto* sdlRenderer = static_cast<SDL_Renderer*>(renderer.NativeHandle());
        if (!sdlRenderer) {
            GRIMAR_LOG_ERROR("Texture2D::LoadFromFile failed: renderer native handle is null");
            return false;
        }

        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            GRIMAR_LOG_ERROR(IMG_GetError());
            return false;
        }

        m_w = surf->w;
        m_h = surf->h;

        m_tex = SDL_CreateTextureFromSurface(sdlRenderer, surf);
        SDL_FreeSurface(surf);

        if (!m_tex) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            m_w = 0;
            m_h = 0;
            return false;
        }

        m_path = path;
        return true;
    }

    void Texture2D::Destroy() noexcept {
        if (m_tex) {
            SDL_DestroyTexture(m_tex);
            m_tex = nullptr;
        }
        m_w = 0;
        m_h = 0;
        m_path.clear();
    }
}
