#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "grimar/render/Rect.hpp"

namespace grimar::render {
    class Renderer2D;
}

namespace grimar::assets {

    class AssetManager;
    class Texture2D;

    struct SpriteFrame {
        std::shared_ptr<Texture2D> texture{};
        grimar::render::RectI srcRect{};
    };

    class SpriteSheet {
    public:
        SpriteSheet() = default;
        ~SpriteSheet() = default;

        SpriteSheet(const SpriteSheet&) = delete;
        SpriteSheet& operator=(const SpriteSheet&) = delete;

        bool Load(AssetManager& assets,
                  grimar::render::Renderer2D& renderer,
                  const std::string& jsonPath) noexcept;

        [[nodiscard]] const SpriteFrame* GetFrame(const std::string& name) const noexcept;

        void Clear() noexcept;

        [[nodiscard]] std::size_t FrameCount() const noexcept {
            return m_frames.size();
        }

    private:
        std::shared_ptr<Texture2D> m_texture{};
        std::unordered_map<std::string, SpriteFrame> m_frames{};
    };

}
