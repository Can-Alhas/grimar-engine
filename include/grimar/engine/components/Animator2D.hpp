// ~ Grimar Engine ~

#pragma once

#include <cstddef>

#include "grimar/assets/Animation2D.hpp"

namespace grimar::engine {

    struct Animator2D {
        const grimar::assets::AnimationClip* clip{nullptr};
        std::size_t frameIndex{0};
        double timer{0.0};
        bool playing{true};

        void SetClip(const grimar::assets::AnimationClip* newClip) noexcept {
            clip = newClip;
            frameIndex = 0;
            timer = 0.0;
            playing = true;
        }

        void Reset() noexcept {
            frameIndex = 0;
            timer = 0.0;
            playing = true;
        }

        [[nodiscard]] const grimar::assets::AnimationFrame* CurrentFrame() const noexcept {
            if (!clip || clip->frames.empty() || frameIndex >= clip->frames.size()) {
                return nullptr;
            }

            return &clip->frames[frameIndex];
        }
    };
}
