// ~ Grimar Engine ~

#include "grimar/engine/systems/RenderSystem.hpp"

#include "grimar/assets/SpriteSheet.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/render/Renderer2D.hpp"


namespace grimar::engine {
    void RenderSystem::Render(World &world,
                              const grimar::assets::SpriteSheet &spriteSheet,
                              grimar::render::Renderer2D &renderer) noexcept {


        world.ForEachSpriteRenderer([&](const grimar::engine::Entity entity,
                                            const grimar::engine::SpriteRenderer& sprite) {

           if (!sprite.visible) {
               return;
           }

           const auto* transform = world.GetTransform(entity);
           if (!transform) {
               return;
           }

           const auto* frame = spriteSheet.GetFrame(sprite.spriteName);
           if (!frame || !frame->texture) {
               GRIMAR_LOG_WARN("World sprite frame not found");
               return;
           }

           renderer.DrawSprite(
               *frame->texture,
               frame->srcRect,
               grimar::render::RectF{
                   transform->position.x,
                   transform->position.y,
                   sprite.size.x,
                   sprite.size.y
               },

               sprite.layer

           );

       });

    }
}
