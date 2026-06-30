// ~ Grimar Engine ~

#pragma once

#include "grimar/core/Math2D.hpp"
#include "grimar/engine/components/Transform2D.hpp"

namespace grimar::engine {

    struct BoxCollider2D {
        //
        grimar::core::Vec2f offset{0.f, 0.f};

        // Collider size
        grimar::core::Vec2f size{1.f, 1.f};

        // Trigger ileride collision event uretir ama resolve yapmaz
        bool isTrigger{false};
    };

    [[nodiscard]] inline grimar::core::AABB2D ComputeAABB(
        const Transform2D& transform,
        const BoxCollider2D& collider) noexcept {

        const grimar::core::Vec2f min{
            transform.position.x + collider.offset.x,
            transform.position.y  + collider.offset.y
        };

        const grimar::core::Vec2f max {
            min.x + collider.size.x,
            min.y + collider.size.y
        };

        return {min, max};
    }
}