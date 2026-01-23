// ~ Grimar Engine ~

#pragma once

namespace grimar::render {
    // World-space rectangle (floating point for
    //                        camera/physics friendliness)
    struct RectF {
        float x{0}, y{0}, w{0}, h{0};
    };

    struct RectI {
        int x{0}, y{}, w{0}, h{0};
    };
}