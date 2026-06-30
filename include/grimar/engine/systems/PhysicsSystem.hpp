// ~ Grimar Engine ~

#pragma once

#include "grimar/core/Math2D.hpp"
#include "grimar/engine/Entity.hpp"

namespace grimar::engine {

    class World;
    struct BoxCollider2D;
    struct RigidBody2D;
    struct Transform2D;

    struct PhysicsSystemDesc {
        grimar::core::Vec2f gravity{0.f, -980.f};
    };

    class PhysicsSystem {
    public:
        explicit PhysicsSystem(PhysicsSystemDesc desc = {}) noexcept;

        void FixedUpdate(World& world, double fixedDt) noexcept;

    private:
        void ResolveAxis(World& world,
                         Entity entity,
                         Transform2D& transform,
                         RigidBody2D& body,
                         const BoxCollider2D& collider,
                         bool resolveX) const noexcept;

        grimar::core::Vec2f m_gravity{0.f, -980.f};
    };
}
