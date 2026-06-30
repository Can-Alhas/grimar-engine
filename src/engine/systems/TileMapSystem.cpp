// ~ Grimar Engine ~

#include "grimar/engine/systems/TileMapSystem.hpp"

#include "grimar/assets/SpriteSheet.hpp"
#include "grimar/assets/TileMap.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/Transform2D.hpp"

namespace grimar::engine {

    void TileMapSystem::Render(const grimar::assets::TileMap& tileMap,
                               const grimar::assets::SpriteSheet& spriteSheet,
                               grimar::render::Renderer2D& renderer,
                               grimar::core::Vec2f origin,
                               grimar::render::Layer layer) noexcept {
        const float tileW = static_cast<float>(tileMap.TileWidth());
        const float tileH = static_cast<float>(tileMap.TileHeight());

        for (int y = 0; y < tileMap.Height(); ++y) {
            for (int x = 0; x < tileMap.Width(); ++x) {
                const auto tileId = tileMap.TileAt(x, y);
                if (tileId == 0) {
                    continue;
                }

                const auto* definition = tileMap.GetDefinition(tileId);
                if (!definition || definition->spriteName.empty()) {
                    continue;
                }

                const auto* frame = spriteSheet.GetFrame(definition->spriteName);
                if (!frame || !frame->texture) {
                    GRIMAR_LOG_WARN("TileMapSystem::Render skipped tile: sprite frame not found");
                    continue;
                }

                renderer.DrawSprite(
                    *frame->texture,
                    frame->srcRect,
                    grimar::render::RectF{
                        origin.x + (static_cast<float>(x) * tileW),
                        origin.y + (static_cast<float>(y) * tileH),
                        tileW,
                        tileH
                    },
                    layer
                );
            }
        }
    }

    std::uint32_t TileMapSystem::CreateSolidColliders(const grimar::assets::TileMap& tileMap,
                                                      World& world,
                                                      grimar::core::Vec2f origin) noexcept {
        const float tileW = static_cast<float>(tileMap.TileWidth());
        const float tileH = static_cast<float>(tileMap.TileHeight());
        std::uint32_t created{0};

        for (int y = 0; y < tileMap.Height(); ++y) {
            for (int x = 0; x < tileMap.Width(); ++x) {
                const auto tileId = tileMap.TileAt(x, y);
                if (!tileMap.IsSolid(tileId)) {
                    continue;
                }

                const auto entity = world.CreateEntity();

                Transform2D transform{};
                transform.SetPosition(
                    origin.x + (static_cast<float>(x) * tileW),
                    origin.y + (static_cast<float>(y) * tileH)
                );

                RigidBody2D body{};
                body.bodyType = BodyType::Static;

                BoxCollider2D collider{};
                collider.size = {tileW, tileH};

                const bool transformAdded = world.AddTransform(entity, transform);
                const bool bodyAdded = world.AddRigidBody(entity, body);
                const bool colliderAdded = world.AddBoxCollider(entity, collider);

                if (transformAdded && bodyAdded && colliderAdded) {
                    ++created;
                }
            }
        }

        return created;
    }
}
