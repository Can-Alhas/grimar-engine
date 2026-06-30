// ~ Grimar Engine ~

#include "grimar/engine/systems/DebugDrawSystem.hpp"

#include "grimar/engine/World.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/render/Color.hpp"
#include "grimar/render/Renderer2D.hpp"

namespace grimar::engine {
    namespace {
        constexpr grimar::render::Layer DebugLayer = 10'000;
        constexpr float VelocityScale = 0.15f;

        [[nodiscard]] grimar::core::Vec2f CenterOf(grimar::core::AABB2D aabb) noexcept {
            return {
                (aabb.min.x + aabb.max.x) * 0.5f,
                (aabb.min.y + aabb.max.y) * 0.5f
            };
        }
    }

    void DebugDrawSystem::Render(World& world, grimar::render::Renderer2D& renderer) noexcept {
        world.ForEachBoxCollider([&](Entity entity, const BoxCollider2D& collider) {
            const auto* transform = world.GetTransform(entity);
            if (!transform) {
                return;
            }

            const auto aabb = ComputeAABB(*transform, collider);
            const auto color = collider.isTrigger
                ? grimar::render::Color{255, 220, 80, 255}
                : grimar::render::Color{80, 255, 140, 255};

            const grimar::core::Vec2f bottomLeft{aabb.min.x, aabb.min.y};
            const grimar::core::Vec2f bottomRight{aabb.max.x, aabb.min.y};
            const grimar::core::Vec2f topRight{aabb.max.x, aabb.max.y};
            const grimar::core::Vec2f topLeft{aabb.min.x, aabb.max.y};

            renderer.DrawLine(bottomLeft, bottomRight, color, DebugLayer);
            renderer.DrawLine(bottomRight, topRight, color, DebugLayer);
            renderer.DrawLine(topRight, topLeft, color, DebugLayer);
            renderer.DrawLine(topLeft, bottomLeft, color, DebugLayer);

            const auto* body = world.GetRigidBody(entity);
            if (!body) {
                return;
            }

            const auto center = CenterOf(aabb);
            const grimar::core::Vec2f velocityEnd{
                center.x + (body->velocity.x * VelocityScale),
                center.y + (body->velocity.y * VelocityScale)
            };

            renderer.DrawLine(center, velocityEnd, {255, 90, 90, 255}, DebugLayer + 1);
        });
    }
}
