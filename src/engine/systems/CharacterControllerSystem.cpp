// ~ Grimar Engine ~

#include "grimar/engine/systems/CharacterControllerSystem.hpp"

#include "grimar/engine/World.hpp"
#include "grimar/engine/components/CharacterController2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"

namespace grimar::engine {

    namespace {
        [[nodiscard]] float Clamp(float value, float minValue, float maxValue) noexcept {
            if (value < minValue) {
                return minValue;
            }

            if (value > maxValue) {
                return maxValue;
            }

            return value;
        }

        [[nodiscard]] float TickDown(float value, float dt) noexcept {
            value -= dt;
            return (value > 0.f) ? value : 0.f;
        }
    }

    void CharacterControllerSystem::FixedUpdate(World& world,
                                                CharacterInput2D input,
                                                double fixedDt) noexcept {
        const float dt = static_cast<float>(fixedDt);

        world.ForEachCharacterController2D([&](Entity entity,
                                               CharacterController2D& controller) {
            auto* body = world.GetRigidBody(entity);
            if (!body || body->bodyType == BodyType::Static) {
                return;
            }

            Step(controller, *body, input, dt);
        });
    }

    void CharacterControllerSystem::Step(CharacterController2D& controller,
                                         RigidBody2D& body,
                                         CharacterInput2D input,
                                         float dt) noexcept {
        input.moveAxis = Clamp(input.moveAxis, -1.f, 1.f);

        if (body.grounded) {
            controller.coyoteTimer = controller.coyoteTime;
        } else {
            controller.coyoteTimer = TickDown(controller.coyoteTimer, dt);
        }

        if (input.jumpPressed) {
            controller.jumpBufferTimer = controller.jumpBufferTime;
        } else {
            controller.jumpBufferTimer = TickDown(controller.jumpBufferTimer, dt);
        }

        body.velocity.x = input.moveAxis * controller.moveSpeed;

        if (controller.jumpBufferTimer > 0.f && controller.coyoteTimer > 0.f) {
            body.velocity.y = controller.jumpVelocity;
            body.grounded = false;
            controller.jumpBufferTimer = 0.f;
            controller.coyoteTimer = 0.f;
        }

        if (body.velocity.y < -controller.maxFallSpeed) {
            body.velocity.y = -controller.maxFallSpeed;
        }
    }
}
