// ~ Grimar Engine ~

#pragma once

namespace grimar::render {
    class Renderer2D;
}

namespace grimar::engine {

    class World;

    class DebugDrawSystem {
    public:
        DebugDrawSystem() = default;
        ~DebugDrawSystem() = default;

        DebugDrawSystem(const DebugDrawSystem&) = delete;
        DebugDrawSystem& operator=(const DebugDrawSystem&) = delete;

        void Render(World& world, grimar::render::Renderer2D& renderer) noexcept;
    };
}
