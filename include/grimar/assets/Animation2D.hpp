// ~ Grimar Engine ~

#pragma once

#include <string>
#include <vector>

namespace grimar::assets {

    struct AnimationFrame {
        std::string spriteName{};
        double duration{0.1};
    };

    struct AnimationClip {
        std::vector<AnimationFrame> frames{};
        bool loop{true};
    };
}
