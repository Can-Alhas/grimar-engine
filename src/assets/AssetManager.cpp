// ~ Grimar Engine ~

#include "grimar/assets/AssetManager.hpp"


#include "grimar/assets/Texture2D.hpp"
#include "grimar/core/Log.hpp"

#include "grimar/render/Renderer2D.hpp"

namespace grimar::assets {
    std::shared_ptr<Texture2D> AssetManager::LoadTexture(grimar::render::Renderer2D &renderer, const std::string &path) noexcept {

        // 1) Cache hit
        if (auto it = m_textures.find(path); it != m_textures.end()) {
            GRIMAR_LOG_TRACE("AssetManager: texture cache hit");
            return it->second;
        }

        // 2) Cache miss -> load
        auto tex = std::make_shared<Texture2D>();
        if (!tex->LoadFromFile(renderer, path)) {
            GRIMAR_LOG_ERROR("AssetManager: texture load failed");
            return {};
        }

        m_textures.emplace(path, tex);
        GRIMAR_LOG_INFO("AssetManager: texture loaded and cached");
        return tex;
    }

    void AssetManager::ClearAll() noexcept {
        m_textures.clear();
    }

    bool AssetManager::UnloadTexture(const std::string &path) noexcept {
        return m_textures.erase(path) > 0;
    }
}
