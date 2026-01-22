// ~ Grimar Engine ~

#pragma once

#include <cstdint>

struct SDL_Window;

namespace grimar::platform {

    struct WindowDesc {
        int width   = 1200;
        int height  = 720;
        const char* title = "Grimar Engine";
        bool resizable = false;
    };


    class Window {
    public:
        Window()  noexcept = default;
        ~Window() noexcept;

        Window(const Window& ) = delete;
        Window& operator=(const Window&) = delete;

        bool Create(const WindowDesc& desc) noexcept;
        void Destroy() noexcept;

        [[nodiscard]] bool IsValid() const noexcept { return m_handle != nullptr; }
        [[nodiscard]] SDL_Window* NativeHandle() const noexcept { return m_handle; }

        int Width()  const noexcept { return m_width; }
        int Height() const noexcept { return m_height; }

    private:
        SDL_Window* m_handle{nullptr};
        int m_width{0};
        int m_height{0};
    };
}
