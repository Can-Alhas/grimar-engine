
// `Grimar Engine

#include "grimar/assets/SpriteSheet.hpp"


#include "grimar/assets/AssetManager.hpp"
#include "grimar/assets/Texture2D.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/render/Renderer2D.hpp"

#include <fstream>

#include <exception>

#include <nlohmann/json.hpp>



namespace grimar::assets {


    bool SpriteSheet::Load(AssetManager& assets,
                           grimar::render::Renderer2D& renderer,
                           const std::string& jsonPath ) noexcept
    {

        Clear();

        try {
            std::ifstream file(jsonPath);
            if(!file.is_open()) {
                GRIMAR_LOG_ERROR("SpriteSheet::Load failed: could not open json file");
                return false;
            }

            nlohmann::json data{};
            file >> data;


            if(!data.contains("texture") || !data["texture"].is_string()) {
                GRIMAR_LOG_ERROR("SpriteSheet::Load failed: missing texture path");
                return false;
            }

            if(!data.contains("sprites") || !data["sprites"].is_object()) {
                GRIMAR_LOG_ERROR("SpriteSheet::Load failed: missing sprites object");
                return false;
            }

            const std::string texturePath = data["texture"].get<std::string>();

            m_texture = assets.LoadTexture(renderer, texturePath);
            if(!m_texture) {
                GRIMAR_LOG_ERROR("SpriteSheet::Load failed: texture load failed");
                Clear();
                return false;
            }

            const auto& sprites = data["sprites"];

            for(auto it = sprites.begin(); it != sprites.end(); ++it) {
                const std::string name = it.key();
                const auto& value = it.value();

                if(!value.is_object()) {
                    GRIMAR_LOG_WARN("SpriteSheet::Load skipped sprite: value is not object");
                    continue;
                }

                ////////////
                if(!value.contains("x") ||
                   !value.contains("y") ||
                   !value.contains("w") ||
                   !value.contains("h") )
                {
                    GRIMAR_LOG_WARN("SpriteSheet::Load skipped sprite: missing rect fields");
                    continue;                                    
                }
                /////////////


                const int x = value["x"].get<int>();
                const int y = value["y"].get<int>();
                const int w = value["w"].get<int>();
                const int h = value["h"].get<int>();


                if(w <= 0 || h <= 0) {
                    GRIMAR_LOG_WARN("SpriteSheet::Load skipped sprite: invalid size");
                    continue;
                }

                SpriteFrame frame{};
                frame.texture = m_texture;
                frame.srcRect = grimar::render::RectI{x, y, w, h};

                m_frames.emplace(name, frame);
                
            }

            if(m_frames.empty()){
                GRIMAR_LOG_ERROR("SpriteSheet::Load failed: no valid sprites loaded");
                Clear();
                return false;
            }

            GRIMAR_LOG_INFO("SpriteSheet loaded");
            return true;

        }

        catch (const std::exception&) {
            GRIMAR_LOG_ERROR("SpriteSheet::Load failed: json parse error");
            Clear();
            return false;
        }
       
    }


    const SpriteFrame* SpriteSheet::GetFrame(const std::string& name) const noexcept {

        const auto it = m_frames.find(name);

        if(it == m_frames.end()) {
            return nullptr;
        }

        return &it->second;
    }

    void SpriteSheet::Clear() noexcept {

        m_frames.clear();
        m_texture.reset();
    }
          
}
