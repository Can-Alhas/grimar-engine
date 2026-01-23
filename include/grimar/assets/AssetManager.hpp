// ~ Grimar Engine ~


#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace grimar::render { class Renderer2D; }
namespace grimar::assets { class Texture2D;  }

namespace grimar::assets {

    class AssetManager {
    public:
        AssetManager()  = default;
        ~AssetManager() = default;

        AssetManager(const AssetManager& ) = delete;
        AssetManager& operator=(const AssetManager& ) = delete;

        // Returns cached texture if already loaded, otherwise loads it.
        std::shared_ptr<Texture2D> LoadTexture(grimar::render::Renderer2D& renderer,
                                               const std::string& path ) noexcept;

        void ClearAll() noexcept;

        bool UnloadTexture(const std::string& path) noexcept ;


        [[nodiscard]] std::size_t TextureCount() const noexcept { return m_textures.size(); }



    private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;


    };
}