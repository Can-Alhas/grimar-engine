// ~ Grimar Engine ~

#pragma once

#include "grimar/assets/Animation2D.hpp"
#include "grimar/assets/SpriteSheet.hpp"
#include "grimar/assets/TileMap.hpp"
#include "grimar/core/Math2D.hpp"
#include "grimar/engine/Scene.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/systems/AnimationSystem.hpp"
#include "grimar/engine/systems/CharacterControllerSystem.hpp"
#include "grimar/engine/systems/DebugDrawSystem.hpp"
#include "grimar/engine/systems/PhysicsSystem.hpp"
#include "grimar/engine/systems/RenderSystem.hpp"
#include "grimar/engine/systems/TileMapSystem.hpp"
#include "grimar/render/Camera2D.hpp"

namespace grimar::sandbox {

    class SandboxScene final : public grimar::engine::Scene {
    public:
        bool OnLoad(grimar::engine::SceneContext& context) noexcept override;
        void OnUnload(grimar::engine::SceneContext& context) noexcept override;
        void OnFixedUpdate(grimar::engine::SceneContext& context, double fixedDt) noexcept override;
        void OnUpdate(grimar::engine::SceneContext& context, double dt) noexcept override;
        void OnRender(grimar::engine::SceneContext& context, double alpha) noexcept override;

        [[nodiscard]] grimar::engine::World& GetWorld() noexcept override { return m_world; }
        [[nodiscard]] const grimar::engine::World& GetWorld() const noexcept override { return m_world; }
        [[nodiscard]] grimar::render::Camera2D& GetCamera() noexcept override { return m_camera; }
        [[nodiscard]] const grimar::render::Camera2D& GetCamera() const noexcept override { return m_camera; }

    private:
        grimar::engine::World m_world{};
        grimar::render::Camera2D m_camera{};

        grimar::assets::SpriteSheet m_testSheet{};
        grimar::assets::TileMap m_testTileMap{};
        grimar::assets::AnimationClip m_idleClip{};
        grimar::core::Vec2f m_tileMapOrigin{-384.f, -160.f};

        grimar::engine::AnimationSystem m_animationSystem{};
        grimar::engine::CharacterControllerSystem m_characterControllerSystem{};
        grimar::engine::PhysicsSystem m_physicsSystem{};
        grimar::engine::RenderSystem m_renderSystem{};
        grimar::engine::DebugDrawSystem m_debugDrawSystem{};
        grimar::engine::TileMapSystem m_tileMapSystem{};
    };
}
