// ~ Grimar Engine ~

#include "sandbox/SandboxScene.hpp"

#include "grimar/assets/AssetManager.hpp"
#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/CharacterController2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/platform/Input.hpp"

namespace grimar::sandbox {

    bool SandboxScene::OnLoad(grimar::engine::SceneContext& context) noexcept {
        m_camera.SetViewport(context.config.windowWidth, context.config.windowHeight);
        m_camera.SetPosition({0.f, 0.f});
        m_camera.SetZoom(1.0f);

        auto t1 = context.assets.LoadTexture(context.renderer, "assets/test.png");
        auto t2 = context.assets.LoadTexture(context.renderer, "assets/test.png");

        GRIMAR_ASSERT(context.assets.TextureCount() == 1);
        if (t1 && t2) {
            GRIMAR_LOG_INFO("AssetManager load OK (twice)");
        } else {
            GRIMAR_LOG_WARN("AssetManager load failed");
        }

        if (!m_testSheet.Load(context.assets, context.renderer, "assets/test.sprites.json")) {
            GRIMAR_LOG_WARN("test sprite sheet failed to load");
            return true;
        }

        m_idleClip.loop = true;
        m_idleClip.frames.clear();
        m_idleClip.frames.push_back({"player_idle_0", 0.20});
        m_idleClip.frames.push_back({"player_idle_1", 0.20});
        m_idleClip.frames.push_back({"player_idle_2", 0.20});

        const auto entity = m_world.CreateEntity();

        grimar::engine::Transform2D transform{};
        transform.SetPosition(-300.f, 100.f);

        grimar::engine::SpriteRenderer sprite{};
        sprite.SetSprite("player_idle_0");
        sprite.SetSize(128.f, 128.f);
        sprite.layer = 5;
        sprite.visible = true;

        GRIMAR_ASSERT(m_world.AddTransform(entity, transform));
        GRIMAR_ASSERT(m_world.AddSpriteRenderer(entity, sprite));

        grimar::engine::Animator2D animator{};
        animator.SetClip(&m_idleClip);
        GRIMAR_ASSERT(m_world.AddAnimator2D(entity, animator));

        grimar::engine::RigidBody2D body{};
        body.bodyType = grimar::engine::BodyType::Dynamic;
        body.velocity = {60.f, 0.f};

        grimar::engine::BoxCollider2D collider{};
        collider.size = {128.f, 128.f};

        GRIMAR_ASSERT(m_world.AddRigidBody(entity, body));
        GRIMAR_ASSERT(m_world.AddBoxCollider(entity, collider));

        grimar::engine::CharacterController2D controller{};
        GRIMAR_ASSERT(m_world.AddCharacterController2D(entity, controller));

        const auto entity2 = m_world.CreateEntity();

        grimar::engine::Transform2D transform2{};
        transform2.SetPosition(-120.f, 120.f);

        grimar::engine::SpriteRenderer sprite2{};
        sprite2.SetSprite("player_idle_1");
        sprite2.SetSize(128.f, 128.f);
        sprite2.layer = 4;
        sprite2.visible = true;

        GRIMAR_ASSERT(m_world.AddTransform(entity2, transform2));
        GRIMAR_ASSERT(m_world.AddSpriteRenderer(entity2, sprite2));

        if (!m_testTileMap.Load("assets/test.tilemap.json")) {
            GRIMAR_LOG_WARN("test tilemap failed to load");
        } else {
            const auto colliderCount = m_tileMapSystem.CreateSolidColliders(
                m_testTileMap,
                m_world,
                m_tileMapOrigin
            );

            GRIMAR_LOG_INFO("test tilemap loaded; solid colliders created");
            GRIMAR_ASSERT(colliderCount > 0);
        }

        GRIMAR_LOG_INFO("SandboxScene loaded");
        return true;
    }

    void SandboxScene::OnUnload(grimar::engine::SceneContext&) noexcept {
        m_testSheet.Clear();
        m_testTileMap.Clear();
        m_idleClip.frames.clear();
        m_world.Clear();
    }

    void SandboxScene::OnFixedUpdate(grimar::engine::SceneContext& context, double fixedDt) noexcept {
        using grimar::platform::Key;

        float moveAxis = 0.f;
        if (context.input.IsKeyDown(Key::A)) {
            moveAxis -= 1.f;
        }
        if (context.input.IsKeyDown(Key::D)) {
            moveAxis += 1.f;
        }

        const grimar::engine::CharacterInput2D input{
            moveAxis,
            context.input.WasKeyPressed(Key::Space),
            context.input.IsKeyDown(Key::Space)
        };

        m_characterControllerSystem.FixedUpdate(m_world, input, fixedDt);
        m_physicsSystem.FixedUpdate(m_world, fixedDt);
    }

    void SandboxScene::OnUpdate(grimar::engine::SceneContext& context, double dt) noexcept {
        auto pos = m_camera.Position();
        const float speed = 300.f * static_cast<float>(dt);

        using grimar::platform::Key;
        if (context.input.IsKeyDown(Key::A)) pos.x -= speed;
        if (context.input.IsKeyDown(Key::D)) pos.x += speed;
        if (context.input.IsKeyDown(Key::W)) pos.y += speed;
        if (context.input.IsKeyDown(Key::S)) pos.y -= speed;

        m_camera.SetPosition(pos);

        float zoom = m_camera.Zoom();
        if (context.input.WasKeyPressed(Key::Q)) {
            zoom *= 0.9f;
        }
        if (context.input.WasKeyPressed(Key::E)) {
            zoom *= 1.1f;
        }

        if (zoom < 0.25f) zoom = 0.25f;
        if (zoom > 6.0f)  zoom = 6.0f;
        m_camera.SetZoom(zoom);

        m_animationSystem.Update(m_world, dt);
    }

    void SandboxScene::OnRender(grimar::engine::SceneContext& context, double) noexcept {
        m_tileMapSystem.Render(m_testTileMap, m_testSheet, context.renderer, m_tileMapOrigin, -10);
        m_renderSystem.Render(m_world, m_testSheet, context.renderer);

        if (context.debugDrawEnabled) {
            m_debugDrawSystem.Render(m_world, context.renderer);
        }
    }
}
