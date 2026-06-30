// ~ Grimar Engine ~

#pragma once

namespace grimar::core {
    struct Vec2f {
        float x{0.f};
        float y{0.f};
    };

    struct AABB2D {
        Vec2f min{};
        Vec2f max{};
    };

    struct AABBContact2D {
        bool hit{false};
        Vec2f normal{0.f, 0.f};
        float penetration{0.f};
    };

    [[nodiscard]] inline bool AABBOverlaps(AABB2D a, AABB2D b) noexcept {
        return a.min.x < b.max.x &&
               a.max.x > b.min.x &&
               a.min.y < b.max.y &&
               a.max.y > b.min.y;
    }

    [[nodiscard]] inline AABBContact2D ComputeAABBContact(AABB2D a, AABB2D b) noexcept {
        if (!AABBOverlaps(a, b)) {
            return {};
        }

        const float overlapLeft  = a.max.x - b.min.x;
        const float overlapRight = b.max.x - a.min.x;
        const float overlapDown  = a.max.y - b.min.y;
        const float overlapUp    = b.max.y - a.min.y;

        const float overlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        const float overlapY = (overlapDown < overlapUp) ? overlapDown : overlapUp;

        if (overlapX < overlapY) {
            const float centerA = (a.min.x + a.max.x) * 0.5f;
            const float centerB = (b.min.x + b.max.x) * 0.5f;

            return {
                true,
                (centerA < centerB) ? Vec2f{-1.f, 0.f} : Vec2f{1.f, 0.f},
                overlapX
            };
        }

        const float centerA = (a.min.y + a.max.y) * 0.5f;
        const float centerB = (b.min.y + b.max.y) * 0.5f;

        return {
            true,
            (centerA < centerB) ? Vec2f{0.f, -1.f} : Vec2f{0.f, 1.f},
            overlapY
        };
    }
}
