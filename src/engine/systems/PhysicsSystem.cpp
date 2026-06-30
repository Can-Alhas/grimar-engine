// ~ Grimar Engine ~

#include "grimar/engine/systems/PhysicsSystem.hpp"

#include "grimar/core/Math2D.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/Transform2D.hpp"

namespace grimar::engine {
    namespace {
        [[nodiscard]] float AxisPenetration(grimar::core::AABB2D moving,
                                            grimar::core::AABB2D other,
                                            bool resolveX) noexcept {
            if (resolveX) {
                const float movingCenter = (moving.min.x + moving.max.x) * 0.5f;
                const float otherCenter = (other.min.x + other.max.x) * 0.5f;

                if (movingCenter < otherCenter) {
                    return other.min.x - moving.max.x;
                }

                return other.max.x - moving.min.x;
            }

            const float movingCenter = (moving.min.y + moving.max.y) * 0.5f;
            const float otherCenter = (other.min.y + other.max.y) * 0.5f;

            if (movingCenter < otherCenter) {
                return other.min.y - moving.max.y;
            }

            return other.max.y - moving.min.y;
        }
    }

    PhysicsSystem::PhysicsSystem(PhysicsSystemDesc desc) noexcept
        : m_gravity(desc.gravity) {
    }

    void PhysicsSystem::FixedUpdate(World& world, double fixedDt) noexcept {
        const float dt = static_cast<float>(fixedDt);

        world.ForEachRigidBody([&](Entity entity, RigidBody2D& body) {
            if (body.bodyType == BodyType::Static) {
                body.grounded = false;
                return;
            }

            auto* transform = world.GetTransform(entity);
            if (!transform) {
                body.grounded = false;
                return;
            }

            const auto* collider = world.GetBoxCollider(entity);
            body.grounded = false;

            if (body.bodyType == BodyType::Dynamic) {
                body.velocity.x += m_gravity.x * body.gravityScale * dt;
                body.velocity.y += m_gravity.y * body.gravityScale * dt;
            }

            transform->position.x += body.velocity.x * dt;
            if (collider && !collider->isTrigger) {
                ResolveAxis(world, entity, *transform, body, *collider, true);
            }

            transform->position.y += body.velocity.y * dt;
            if (collider && !collider->isTrigger) {
                ResolveAxis(world, entity, *transform, body, *collider, false);
            }
        });
    }

    void PhysicsSystem::ResolveAxis(World& world,
                                    Entity entity,
                                    Transform2D& transform,
                                    RigidBody2D& body,
                                    const BoxCollider2D& collider,
                                    bool resolveX) const noexcept {
        world.ForEachBoxCollider([&](Entity otherEntity, BoxCollider2D& otherCollider) {
            if (otherEntity == entity || otherCollider.isTrigger) {
                return;
            }

            const auto* otherTransform = world.GetTransform(otherEntity);
            if (!otherTransform) {
                return;
            }

            const auto movingAabb = ComputeAABB(transform, collider);
            const auto otherAabb = ComputeAABB(*otherTransform, otherCollider);

            if (!grimar::core::AABBOverlaps(movingAabb, otherAabb)) {
                return;
            }

            const float penetration = AxisPenetration(movingAabb, otherAabb, resolveX);

            if (resolveX) {
                transform.position.x += penetration;
                body.velocity.x = 0.f;
                return;
            }

            transform.position.y += penetration;
            body.velocity.y = 0.f;

            if (penetration > 0.f) {
                body.grounded = true;
            }
        });
    }
}
