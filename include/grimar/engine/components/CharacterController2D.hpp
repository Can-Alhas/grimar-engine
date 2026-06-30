// ~ Grimar Engine ~

#pragma once

namespace grimar::engine {

    struct CharacterController2D {
        float moveSpeed{260.f};
        float jumpVelocity{520.f};
        float maxFallSpeed{900.f};

        float coyoteTime{0.10f};
        float jumpBufferTime{0.10f};

        float coyoteTimer{0.f};
        float jumpBufferTimer{0.f};
    };
}
