// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <string>

struct SDL_Texture;

namespace grimar::render { class Renderer2D; }

namespace grimar::assets {

    class Texture2D {
    public:
        Texture2D() noexcept = default;
        ~Texture2D() noexcept;

        Texture2D(const Texture2D& ) = delete;
        Texture2D& operator=(const Texture2D& ) = delete;

        Texture2D(Texture2D&& other) noexcept;
        Texture2D& operator=(Texture2D&& other) noexcept;

        // Loads a PNG (or any SDL_image-supported format) and creates a GPU texture
        bool LoadFromFile(grimar::render::Renderer2D& renderer,
                          const std::string& path ) noexcept;

        void Destroy() noexcept;

        [[nodiscard]] bool IsValid() const noexcept { return m_tex != nullptr; }
        [[nodiscard]] int  Width()   const noexcept { return m_w; }
        [[nodiscard]] int Height()   const noexcept { return m_h; }
        [[nodiscard]] SDL_Texture* NativeTexture() const noexcept { return m_tex; }
        [[nodiscard]] const std::string& Path() const noexcept { return m_path; }


    private:
        SDL_Texture* m_tex{nullptr};
        int m_w{0};
        int m_h{0};
        std::string m_path{};
    };
}