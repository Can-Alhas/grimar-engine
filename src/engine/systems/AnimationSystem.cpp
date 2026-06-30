// ~ Grimar Engine ~

#include "grimar/engine/systems/AnimationSystem.hpp"

#include "grimar/engine/World.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"

namespace grimar::engine {

    void AnimationSystem::Update(World& world, double dt) noexcept {
        world.ForEachAnimator2D([&](Entity entity, Animator2D& animator) {
            Advance(animator, dt);

            const auto* frame = animator.CurrentFrame();
            if (!frame) {
                return;
            }

            auto* sprite = world.GetSpriteRenderer(entity);
            if (!sprite) {
                return;
            }

            sprite->SetSprite(frame->spriteName);
        });
    }

    void AnimationSystem::Advance(Animator2D& animator, double dt) noexcept {
        if (!animator.playing || !animator.clip || animator.clip->frames.empty() || dt <= 0.0) {
            return;
        }

        if (animator.frameIndex >= animator.clip->frames.size()) {
            animator.frameIndex = 0;
            animator.timer = 0.0;
        }

        animator.timer += dt;

        while (animator.playing && animator.clip && !animator.clip->frames.empty()) {
            double frameDuration = animator.clip->frames[animator.frameIndex].duration;
            if (frameDuration <= 0.0) {
                frameDuration = 0.1;
            }

            if (animator.timer < frameDuration) {
                break;
            }

            animator.timer -= frameDuration;

            if (animator.frameIndex + 1 < animator.clip->frames.size()) {
                ++animator.frameIndex;
            } else if (animator.clip->loop) {
                animator.frameIndex = 0;
            } else {
                animator.frameIndex = animator.clip->frames.size() - 1;
                animator.timer = 0.0;
                animator.playing = false;
            }
        }
    }
}
