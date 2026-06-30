// ~ Grimar Engine ~

#pragma once

namespace grimar::engine {

    class World;
    struct CharacterController2D;
    struct RigidBody2D;

    struct CharacterInput2D {
        float moveAxis{0.f};
        bool jumpPressed{false};
        bool jumpHeld{false};
    };

    class CharacterControllerSystem {
    public:
        CharacterControllerSystem() = default;
        ~CharacterControllerSystem() = default;

        CharacterControllerSystem(const CharacterControllerSystem&) = delete;
        CharacterControllerSystem& operator=(const CharacterControllerSystem&) = delete;

        void FixedUpdate(World& world, CharacterInput2D input, double fixedDt) noexcept;

    private:
        static void Step(CharacterController2D& controller,
                         RigidBody2D& body,
                         CharacterInput2D input,
                         float dt) noexcept;
    };
}
