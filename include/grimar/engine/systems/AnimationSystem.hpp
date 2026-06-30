// ~ Grimar Engine ~

#pragma once

namespace grimar::engine {

    class World;
    struct Animator2D;

    class AnimationSystem {
    public:
        AnimationSystem() = default;
        ~AnimationSystem() = default;

        AnimationSystem(const AnimationSystem&) = delete;
        AnimationSystem& operator=(const AnimationSystem&) = delete;

        void Update(World& world, double dt) noexcept;

    private:
        static void Advance(Animator2D& animator, double dt) noexcept;
    };
}
