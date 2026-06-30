// ~ Grimar Engine ~

#pragma once

#include "grimar/core/Math2D.hpp"

namespace grimar::engine {

    enum class BodyType {
        Static,
        Dynamic,
        Kinematic
    };

    struct RigidBody2D {
        grimar::core::Vec2f velocity{0.f, 0.f};

        // 1.0 = default gravity, 0.0 = no gravity
        float gravityScale{1.f};

        // Static: Can not move
        // Dynamic: affected by Gravity/Collision
        // Kinematic: It moves via code; gravity is not applied.
        BodyType bodyType{BodyType::Dynamic};

        bool grounded{false};
    };
}
