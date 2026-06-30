// ~ Grimar Engine ~

#pragma once

#include "grimar/engine/EngineConfig.hpp"

namespace grimar::assets {
    class AssetManager;
}

namespace grimar::platform {
    class Input;
}

namespace grimar::render {
    class Camera2D;
    class Renderer2D;
}

namespace grimar::engine {

    class World;

    struct SceneContext {
        grimar::render::Renderer2D& renderer;
        grimar::platform::Input& input;
        grimar::assets::AssetManager& assets;
        const EngineConfig& config;
        bool debugDrawEnabled{true};
    };

    class Scene {
    public:
        Scene() = default;
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        virtual bool OnLoad(SceneContext& context) noexcept = 0;
        virtual void OnUnload(SceneContext& context) noexcept = 0;
        virtual void OnFixedUpdate(SceneContext& context, double fixedDt) noexcept = 0;
        virtual void OnUpdate(SceneContext& context, double dt) noexcept = 0;
        virtual void OnRender(SceneContext& context, double alpha) noexcept = 0;

        [[nodiscard]] virtual World& GetWorld() noexcept = 0;
        [[nodiscard]] virtual const World& GetWorld() const noexcept = 0;
        [[nodiscard]] virtual grimar::render::Camera2D& GetCamera() noexcept = 0;
        [[nodiscard]] virtual const grimar::render::Camera2D& GetCamera() const noexcept = 0;
    };
}
