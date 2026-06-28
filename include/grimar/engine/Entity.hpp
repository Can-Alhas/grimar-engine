// ~ Grimar Engine
#pragma once

#include <cstdint>

namespace grimar::engine {
    struct Entity {

        using Id = std::uint32_t;

        // Generation
        using Generation = std::uint32_t;

        // that id cannot use by any entity
        // UINT32_MAX NICE SENTINEL VALUE

        static constexpr Id InvalidId = UINT32_MAX;

        // Generation 0 is invalid
        // real entities will start 1
        static constexpr Generation InvalidGeneration = 0;

        Id id{InvalidId};
        Generation generation{InvalidGeneration};

        //
        [[nodiscard]] bool IsValid() const noexcept {
            return id != InvalidId && generation != InvalidGeneration;
        }

    };

    // 2 entity handle same id and same generation = mean ref same entity
    [[nodiscard]] inline bool operator==(Entity a, Entity b) noexcept {
        return a.id == b.id && a.generation == b.generation;
    }

    [[nodiscard]] inline bool operator!=(Entity a, Entity b) noexcept {
        return !(a == b);
    }
}