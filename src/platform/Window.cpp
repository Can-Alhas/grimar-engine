// ~ Grimar Engine ~

#include "grimar/platform/Window.hpp"
#include <SDL.h>


namespace grimar::platform {

    Window::~Window() noexcept {
        Destroy();
    }

    bool Window::Create(const WindowDesc& desc) noexcept {
        if (m_handle) return true;

        Uint32 flags = SDL_WINDOW_SHOWN;
        if (desc.resizable) flags |= SDL_WINDOW_RESIZABLE;

        m_handle = SDL_CreateWindow(
            desc.title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            desc.width,
            desc.height,
            flags
        );

        if (!m_handle) {
            return false;
        }

        m_width  = desc.width;
        m_height = desc.height;

        return true;

    }

    void Window::Destroy() noexcept {
        if (m_handle) {
            SDL_DestroyWindow(m_handle);
            m_handle = nullptr;
        }
        m_width  = 0;
        m_height = 0;
    }
}
