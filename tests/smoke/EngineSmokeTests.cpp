// ~ Grimar Engine ~
#include "EngineSmokeTests.hpp"

#include <vector>

#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/assets/Animation2D.hpp"
#include "grimar/assets/TileMap.hpp"
#include "grimar/engine/Entity.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/engine/systems/AnimationSystem.hpp"
#include "grimar/engine/systems/PhysicsSystem.hpp"
#include "grimar/engine/systems/TileMapSystem.hpp"




namespace grimar::engine {
    void RunEngineSmokeTests() noexcept {
          //Entity create test
        {

            grimar::engine::World world{};

            auto a = world.CreateEntity();
            GRIMAR_ASSERT(a.IsValid());
            GRIMAR_ASSERT(world.IsAlive(a));
            GRIMAR_ASSERT(world.AliveCount() == 1);

            world.DestroyEntity(a);
            GRIMAR_ASSERT(!world.IsAlive(a));
            GRIMAR_ASSERT(world.AliveCount() == 0);

            auto b = world.CreateEntity();
            GRIMAR_ASSERT(b.IsValid());
            GRIMAR_ASSERT(world.IsAlive(b));
            GRIMAR_ASSERT(b.id == a.id);
            GRIMAR_ASSERT(b.generation != a.generation);

            GRIMAR_LOG_INFO("World entity lifecycle tests OK");
        }


        // 100+ entity create test
        {
            grimar::engine::World world{};
            std::vector<grimar::engine::Entity> entities{};

            for (int i = 0; i < 128; ++i) {
                auto e = world.CreateEntity();
                GRIMAR_ASSERT(e.IsValid());
                GRIMAR_ASSERT(world.IsAlive(e));
                entities.push_back(e);
            }

            GRIMAR_ASSERT(world.AliveCount() == 128);

            for (auto e: entities) {
                world.DestroyEntity(e);
                GRIMAR_ASSERT(!world.IsAlive(e));
            }

            GRIMAR_ASSERT(world.AliveCount() == 0);
            GRIMAR_LOG_INFO("World 100+ entity create // destroy test OK");
            //


            // transform test
            grimar::engine::Transform2D t{};
            GRIMAR_ASSERT(t.position.x == 0.f);
            GRIMAR_ASSERT(t.position.y == 0.f);
            GRIMAR_ASSERT(t.scale.x == 1.f);
            GRIMAR_ASSERT(t.scale.y == 1.f);

            t.SetPosition(10.f, 20.f);
            GRIMAR_ASSERT(t.position.x == 10.f);
            GRIMAR_ASSERT(t.position.y == 20.f);

            t.Translate({5.f, -2.f});
            GRIMAR_ASSERT(t.position.x == 15.f);
            GRIMAR_ASSERT(t.position.y == 18.f);

            t.Reset();
            GRIMAR_ASSERT(t.position.x == 0.f);
            GRIMAR_ASSERT(t.position.y == 0.f);
            GRIMAR_ASSERT(t.rotation == 0.f);
            GRIMAR_ASSERT(t.scale.x == 1.f);
            GRIMAR_ASSERT(t.scale.y == 1.f);

            GRIMAR_LOG_INFO("Transform2D tests OK");
        }


        // World Transform storage tests
        {
            grimar::engine::World world{};

            auto entity = world.CreateEntity();

            grimar::engine::Transform2D transform{};
            transform.SetPosition(10.f, 20.f);

            GRIMAR_ASSERT(world.AddTransform(entity, transform));
            GRIMAR_ASSERT(world.HasTransform(entity));

            auto* stored = world.GetTransform(entity);
            GRIMAR_ASSERT(stored != nullptr);
            GRIMAR_ASSERT(stored->position.x == 10.f);
            GRIMAR_ASSERT(stored->position.y == 20.f);

            stored->Translate({5.f, -2.f});
            GRIMAR_ASSERT(stored->position.x == 15.f);
            GRIMAR_ASSERT(stored->position.y == 18.f);

            int visited = 0;
            world.ForEachTransform([&](grimar::engine::Entity e,
                                       grimar::engine::Transform2D& t) {
                GRIMAR_ASSERT(e == entity);
                t.Translate({1.f, 1.f});
                ++visited;
            });

            GRIMAR_ASSERT(visited == 1);
            GRIMAR_ASSERT(stored->position.x == 16.f);
            GRIMAR_ASSERT(stored->position.y == 19.f);

            GRIMAR_ASSERT(world.RemoveTransform(entity));
            GRIMAR_ASSERT(!world.HasTransform(entity));
            GRIMAR_ASSERT(world.GetTransform(entity) == nullptr);

            world.DestroyEntity(entity);
            GRIMAR_ASSERT(!world.AddTransform(entity, transform));

            GRIMAR_LOG_INFO("World Transform storage tests OK");
        }

        //id reuse
        {
            grimar::engine::World world{};

            auto oldEntity = world.CreateEntity();
            const auto oldId = oldEntity.id;
            const auto oldGeneration = oldEntity.generation;

            GRIMAR_ASSERT(world.IsAlive(oldEntity));

            world.DestroyEntity(oldEntity);

            // Destroy sonrasi eski handle artik alive olmamali.
            GRIMAR_ASSERT(!world.IsAlive(oldEntity));

            auto newEntity = world.CreateEntity();

            // Free-list calisiyorsa ayni id tekrar kullanilir.
            GRIMAR_ASSERT(newEntity.id == oldId);

            // Ama generation artmis olmali.
            // Bu sayede oldEntity stale handle olarak yakalanir.
            GRIMAR_ASSERT(newEntity.generation != oldGeneration);

            // Yeni handle alive, eski handle dead olmali.
            GRIMAR_ASSERT(world.IsAlive(newEntity));
            GRIMAR_ASSERT(!world.IsAlive(oldEntity));

            GRIMAR_LOG_INFO("World stale generation reuse test OK");
        }

        //SpriteRenderer component test
        {
            grimar::engine::SpriteRenderer sprite{};
            GRIMAR_ASSERT(sprite.spriteName.empty());
            GRIMAR_ASSERT(sprite.size.x == 1.f);
            GRIMAR_ASSERT(sprite.size.y == 1.f);
            GRIMAR_ASSERT(sprite.layer == 0);
            GRIMAR_ASSERT(sprite.visible);

            sprite.SetSprite("player_idle_0");
            sprite.SetSize(256.f, 128.f);
            sprite.layer = 5;
            sprite.visible = false;

            GRIMAR_ASSERT(sprite.spriteName == "player_idle_0");
            GRIMAR_ASSERT(sprite.size.x == 256.f);
            GRIMAR_ASSERT(sprite.size.y == 128.f);
            GRIMAR_ASSERT(sprite.layer == 5);
            GRIMAR_ASSERT(!sprite.visible);

            GRIMAR_LOG_INFO("SpriteRenderer component tests OK");
        }

        // World Sprite renderer storage tests
        {
            grimar::engine::World world{};

            auto entity = world.CreateEntity();

            grimar::engine::SpriteRenderer sprite{};
            sprite.SetSprite("player_idle_0");
            sprite.SetSize(256.f, 256.f);
            sprite.layer = 5;
            sprite.visible = true;

            GRIMAR_ASSERT(world.AddSpriteRenderer(entity, sprite));
            GRIMAR_ASSERT(world.HasSpriteRenderer(entity));

            auto* stored = world.GetSpriteRenderer(entity);
            GRIMAR_ASSERT(stored != nullptr);
            GRIMAR_ASSERT(stored->spriteName == "player_idle_0");
            GRIMAR_ASSERT(stored->size.x == 256.f);
            GRIMAR_ASSERT(stored->size.y == 256.f);
            GRIMAR_ASSERT(stored->layer == 5);
            GRIMAR_ASSERT(stored->visible);

            int visited = 0;
            world.ForEachSpriteRenderer([&](grimar::engine::Entity e,
                                            grimar::engine::SpriteRenderer& sr) {
                GRIMAR_ASSERT(e == entity);
                GRIMAR_ASSERT(sr.spriteName == "player_idle_0");

                sr.layer = 10;
                ++visited;
            });

            GRIMAR_ASSERT(visited == 1);
            GRIMAR_ASSERT(stored->layer == 10);

            GRIMAR_ASSERT(world.RemoveSpriteRenderer(entity));
            GRIMAR_ASSERT(!world.HasSpriteRenderer(entity));
            GRIMAR_ASSERT(world.GetSpriteRenderer(entity) == nullptr);

            world.DestroyEntity(entity);
            GRIMAR_ASSERT(!world.AddSpriteRenderer(entity, sprite));

            GRIMAR_LOG_INFO("World SpriteRenderer storage tests OK");
        }

        // Animator2D component and World storage tests
        {
            grimar::assets::AnimationClip clip{};
            clip.loop = true;
            clip.frames.push_back({"player_idle_0", 0.1});
            clip.frames.push_back({"player_idle_1", 0.1});

            grimar::engine::Animator2D animator{};
            GRIMAR_ASSERT(animator.clip == nullptr);
            GRIMAR_ASSERT(animator.frameIndex == 0);
            GRIMAR_ASSERT(animator.timer == 0.0);
            GRIMAR_ASSERT(animator.playing);
            GRIMAR_ASSERT(animator.CurrentFrame() == nullptr);

            animator.SetClip(&clip);
            GRIMAR_ASSERT(animator.clip == &clip);
            GRIMAR_ASSERT(animator.frameIndex == 0);
            GRIMAR_ASSERT(animator.timer == 0.0);
            GRIMAR_ASSERT(animator.playing);
            GRIMAR_ASSERT(animator.CurrentFrame() != nullptr);
            GRIMAR_ASSERT(animator.CurrentFrame()->spriteName == "player_idle_0");

            grimar::engine::World world{};
            const auto entity = world.CreateEntity();

            GRIMAR_ASSERT(world.AddAnimator2D(entity, animator));
            GRIMAR_ASSERT(world.HasAnimator2D(entity));

            auto* stored = world.GetAnimator2D(entity);
            GRIMAR_ASSERT(stored != nullptr);
            GRIMAR_ASSERT(stored->clip == &clip);

            int visited = 0;
            world.ForEachAnimator2D([&](grimar::engine::Entity e,
                                        grimar::engine::Animator2D& a) {
                GRIMAR_ASSERT(e == entity);
                a.timer = 0.05;
                ++visited;
            });

            GRIMAR_ASSERT(visited == 1);
            GRIMAR_ASSERT(stored->timer == 0.05);

            GRIMAR_ASSERT(world.RemoveAnimator2D(entity));
            GRIMAR_ASSERT(!world.HasAnimator2D(entity));
            GRIMAR_ASSERT(world.GetAnimator2D(entity) == nullptr);

            GRIMAR_LOG_INFO("Animator2D World storage tests OK");
        }

        // AnimationSystem sprite frame update tests
        {
            grimar::assets::AnimationClip clip{};
            clip.loop = true;
            clip.frames.push_back({"player_idle_0", 0.1});
            clip.frames.push_back({"player_idle_1", 0.1});
            clip.frames.push_back({"player_idle_2", 0.1});

            grimar::engine::World world{};
            grimar::engine::AnimationSystem animationSystem{};

            const auto entity = world.CreateEntity();

            grimar::engine::SpriteRenderer sprite{};
            sprite.SetSprite("unset");

            grimar::engine::Animator2D animator{};
            animator.SetClip(&clip);

            GRIMAR_ASSERT(world.AddSpriteRenderer(entity, sprite));
            GRIMAR_ASSERT(world.AddAnimator2D(entity, animator));

            animationSystem.Update(world, 0.0);
            auto* storedSprite = world.GetSpriteRenderer(entity);
            auto* storedAnimator = world.GetAnimator2D(entity);

            GRIMAR_ASSERT(storedSprite != nullptr);
            GRIMAR_ASSERT(storedAnimator != nullptr);
            GRIMAR_ASSERT(storedSprite->spriteName == "player_idle_0");
            GRIMAR_ASSERT(storedAnimator->frameIndex == 0);

            animationSystem.Update(world, 0.1);
            GRIMAR_ASSERT(storedSprite->spriteName == "player_idle_1");
            GRIMAR_ASSERT(storedAnimator->frameIndex == 1);

            animationSystem.Update(world, 0.2);
            GRIMAR_ASSERT(storedSprite->spriteName == "player_idle_0");
            GRIMAR_ASSERT(storedAnimator->frameIndex == 0);

            GRIMAR_LOG_INFO("AnimationSystem sprite frame update tests OK");
        }

        // AnimationSystem non-loop clip stop tests
        {
            grimar::assets::AnimationClip clip{};
            clip.loop = false;
            clip.frames.push_back({"start", 0.1});
            clip.frames.push_back({"end", 0.1});

            grimar::engine::World world{};
            grimar::engine::AnimationSystem animationSystem{};

            const auto entity = world.CreateEntity();

            grimar::engine::SpriteRenderer sprite{};
            grimar::engine::Animator2D animator{};
            animator.SetClip(&clip);

            GRIMAR_ASSERT(world.AddSpriteRenderer(entity, sprite));
            GRIMAR_ASSERT(world.AddAnimator2D(entity, animator));

            animationSystem.Update(world, 1.0);

            const auto* storedSprite = world.GetSpriteRenderer(entity);
            const auto* storedAnimator = world.GetAnimator2D(entity);

            GRIMAR_ASSERT(storedSprite != nullptr);
            GRIMAR_ASSERT(storedAnimator != nullptr);
            GRIMAR_ASSERT(storedSprite->spriteName == "end");
            GRIMAR_ASSERT(storedAnimator->frameIndex == 1);
            GRIMAR_ASSERT(!storedAnimator->playing);

            GRIMAR_LOG_INFO("AnimationSystem non-loop clip tests OK");
        }

        // Physics component data tests
        {
            grimar::engine::RigidBody2D body{};
            GRIMAR_ASSERT(body.velocity.x == 0.f);
            GRIMAR_ASSERT(body.velocity.y == 0.f);
            GRIMAR_ASSERT(body.gravityScale == 1.f);
            GRIMAR_ASSERT(body.bodyType == grimar::engine::BodyType::Dynamic);
            GRIMAR_ASSERT(!body.grounded);

            body.velocity = {10.f, -5.f};
            body.gravityScale = 0.5f;
            body.bodyType = grimar::engine::BodyType::Kinematic;
            body.grounded = true;

            GRIMAR_ASSERT(body.velocity.x == 10.f);
            GRIMAR_ASSERT(body.velocity.y == -5.f);
            GRIMAR_ASSERT(body.gravityScale == 0.5f);
            GRIMAR_ASSERT(body.bodyType == grimar::engine::BodyType::Kinematic);
            GRIMAR_ASSERT(body.grounded);

            grimar::engine::BoxCollider2D collider{};
            GRIMAR_ASSERT(collider.offset.x == 0.f);
            GRIMAR_ASSERT(collider.offset.y == 0.f);
            GRIMAR_ASSERT(collider.size.x == 1.f);
            GRIMAR_ASSERT(collider.size.y == 1.f);
            GRIMAR_ASSERT(!collider.isTrigger);

            collider.offset = {2.f, 3.f};
            collider.size = {16.f, 32.f};

            grimar::engine::Transform2D transform{};
            transform.SetPosition(10.f, 20.f);

            const auto aabb = grimar::engine::ComputeAABB(transform, collider);
            GRIMAR_ASSERT(aabb.min.x == 12.f);
            GRIMAR_ASSERT(aabb.min.y == 23.f);
            GRIMAR_ASSERT(aabb.max.x == 28.f);
            GRIMAR_ASSERT(aabb.max.y == 55.f);

            GRIMAR_LOG_INFO("Physics component data tests OK");
        }

        // World physics component storage tests
        {
            grimar::engine::World world{};

            auto entity = world.CreateEntity();

            grimar::engine::RigidBody2D body{};
            body.velocity = {4.f, -2.f};
            body.gravityScale = 2.f;
            body.bodyType = grimar::engine::BodyType::Dynamic;

            grimar::engine::BoxCollider2D collider{};
            collider.offset = {1.f, 2.f};
            collider.size = {20.f, 30.f};
            collider.isTrigger = true;

            const bool bodyAdded = world.AddRigidBody(entity, body);
            const bool colliderAdded = world.AddBoxCollider(entity, collider);

            GRIMAR_ASSERT(bodyAdded);
            GRIMAR_ASSERT(colliderAdded);
            GRIMAR_ASSERT(world.HasRigidBody(entity));
            GRIMAR_ASSERT(world.HasBoxCollider(entity));

            auto* storedBody = world.GetRigidBody(entity);
            auto* storedCollider = world.GetBoxCollider(entity);

            GRIMAR_ASSERT(storedBody != nullptr);
            GRIMAR_ASSERT(storedCollider != nullptr);
            GRIMAR_ASSERT(storedBody->velocity.x == 4.f);
            GRIMAR_ASSERT(storedBody->velocity.y == -2.f);
            GRIMAR_ASSERT(storedBody->gravityScale == 2.f);
            GRIMAR_ASSERT(storedCollider->offset.x == 1.f);
            GRIMAR_ASSERT(storedCollider->offset.y == 2.f);
            GRIMAR_ASSERT(storedCollider->size.x == 20.f);
            GRIMAR_ASSERT(storedCollider->size.y == 30.f);
            GRIMAR_ASSERT(storedCollider->isTrigger);

            int bodyVisited = 0;
            world.ForEachRigidBody([&](grimar::engine::Entity e,
                                       grimar::engine::RigidBody2D& rb) {
                GRIMAR_ASSERT(e == entity);
                rb.velocity.x += 1.f;
                ++bodyVisited;
            });

            int colliderVisited = 0;
            world.ForEachBoxCollider([&](grimar::engine::Entity e,
                                         grimar::engine::BoxCollider2D& box) {
                GRIMAR_ASSERT(e == entity);
                box.isTrigger = false;
                ++colliderVisited;
            });

            GRIMAR_ASSERT(bodyVisited == 1);
            GRIMAR_ASSERT(colliderVisited == 1);
            GRIMAR_ASSERT(storedBody->velocity.x == 5.f);
            GRIMAR_ASSERT(!storedCollider->isTrigger);

            world.DestroyEntity(entity);

            GRIMAR_ASSERT(!world.HasRigidBody(entity));
            GRIMAR_ASSERT(!world.HasBoxCollider(entity));
            GRIMAR_ASSERT(world.GetRigidBody(entity) == nullptr);
            GRIMAR_ASSERT(world.GetBoxCollider(entity) == nullptr);

            GRIMAR_LOG_INFO("World physics component storage tests OK");
        }

        // AABB overlap tests
        {
            const grimar::core::AABB2D a{{0.f, 0.f}, {10.f, 10.f}};
            const grimar::core::AABB2D b{{5.f, 5.f}, {15.f, 15.f}};
            const grimar::core::AABB2D separatedX{{11.f, 0.f}, {20.f, 10.f}};
            const grimar::core::AABB2D separatedY{{0.f, 11.f}, {10.f, 20.f}};
            const grimar::core::AABB2D touchingEdge{{10.f, 0.f}, {20.f, 10.f}};

            GRIMAR_ASSERT(grimar::core::AABBOverlaps(a, b));
            GRIMAR_ASSERT(!grimar::core::AABBOverlaps(a, separatedX));
            GRIMAR_ASSERT(!grimar::core::AABBOverlaps(a, separatedY));
            GRIMAR_ASSERT(!grimar::core::AABBOverlaps(a, touchingEdge));

            GRIMAR_LOG_INFO("AABB overlap tests OK");
        }

        // AABB contact normal tests
        {
            const grimar::core::AABB2D solid{{0.f, 0.f}, {10.f, 10.f}};

            const auto leftHit = grimar::core::ComputeAABBContact(
                grimar::core::AABB2D{{8.f, 2.f}, {18.f, 8.f}},
                solid);
            GRIMAR_ASSERT(leftHit.hit);
            GRIMAR_ASSERT(leftHit.normal.x == 1.f);
            GRIMAR_ASSERT(leftHit.normal.y == 0.f);
            GRIMAR_ASSERT(leftHit.penetration == 2.f);

            const auto rightHit = grimar::core::ComputeAABBContact(
                grimar::core::AABB2D{{-8.f, 2.f}, {2.f, 8.f}},
                solid);
            GRIMAR_ASSERT(rightHit.hit);
            GRIMAR_ASSERT(rightHit.normal.x == -1.f);
            GRIMAR_ASSERT(rightHit.normal.y == 0.f);
            GRIMAR_ASSERT(rightHit.penetration == 2.f);

            const auto upperHit = grimar::core::ComputeAABBContact(
                grimar::core::AABB2D{{2.f, 8.f}, {8.f, 18.f}},
                solid);
            GRIMAR_ASSERT(upperHit.hit);
            GRIMAR_ASSERT(upperHit.normal.x == 0.f);
            GRIMAR_ASSERT(upperHit.normal.y == 1.f);
            GRIMAR_ASSERT(upperHit.penetration == 2.f);

            const auto lowerHit = grimar::core::ComputeAABBContact(
                grimar::core::AABB2D{{2.f, -8.f}, {8.f, 2.f}},
                solid);
            GRIMAR_ASSERT(lowerHit.hit);
            GRIMAR_ASSERT(lowerHit.normal.x == 0.f);
            GRIMAR_ASSERT(lowerHit.normal.y == -1.f);
            GRIMAR_ASSERT(lowerHit.penetration == 2.f);

            const auto noHit = grimar::core::ComputeAABBContact(
                grimar::core::AABB2D{{20.f, 20.f}, {30.f, 30.f}},
                solid);
            GRIMAR_ASSERT(!noHit.hit);
            GRIMAR_ASSERT(noHit.normal.x == 0.f);
            GRIMAR_ASSERT(noHit.normal.y == 0.f);
            GRIMAR_ASSERT(noHit.penetration == 0.f);

            GRIMAR_LOG_INFO("AABB contact normal tests OK");
        }

        // PhysicsSystem ground resolve tests
        {
            grimar::engine::World world{};
            grimar::engine::PhysicsSystem physics{};

            const auto player = world.CreateEntity();
            grimar::engine::Transform2D playerTransform{};
            playerTransform.SetPosition(0.f, 30.f);

            grimar::engine::RigidBody2D playerBody{};
            playerBody.bodyType = grimar::engine::BodyType::Dynamic;

            grimar::engine::BoxCollider2D playerCollider{};
            playerCollider.size = {10.f, 10.f};

            GRIMAR_ASSERT(world.AddTransform(player, playerTransform));
            GRIMAR_ASSERT(world.AddRigidBody(player, playerBody));
            GRIMAR_ASSERT(world.AddBoxCollider(player, playerCollider));

            const auto ground = world.CreateEntity();
            grimar::engine::Transform2D groundTransform{};
            groundTransform.SetPosition(-50.f, 0.f);

            grimar::engine::RigidBody2D groundBody{};
            groundBody.bodyType = grimar::engine::BodyType::Static;

            grimar::engine::BoxCollider2D groundCollider{};
            groundCollider.size = {100.f, 10.f};

            GRIMAR_ASSERT(world.AddTransform(ground, groundTransform));
            GRIMAR_ASSERT(world.AddRigidBody(ground, groundBody));
            GRIMAR_ASSERT(world.AddBoxCollider(ground, groundCollider));

            for (int i = 0; i < 120; ++i) {
                physics.FixedUpdate(world, 1.0 / 60.0);
            }

            const auto* storedTransform = world.GetTransform(player);
            const auto* storedBody = world.GetRigidBody(player);

            GRIMAR_ASSERT(storedTransform != nullptr);
            GRIMAR_ASSERT(storedBody != nullptr);
            GRIMAR_ASSERT(storedTransform->position.y == 10.f);
            GRIMAR_ASSERT(storedBody->velocity.y == 0.f);
            GRIMAR_ASSERT(storedBody->grounded);

            GRIMAR_LOG_INFO("PhysicsSystem ground resolve tests OK");
        }

        // PhysicsSystem wall resolve tests
        {
            grimar::engine::World world{};
            grimar::engine::PhysicsSystem physics{};

            const auto player = world.CreateEntity();
            grimar::engine::Transform2D playerTransform{};
            playerTransform.SetPosition(0.f, 10.f);

            grimar::engine::RigidBody2D playerBody{};
            playerBody.bodyType = grimar::engine::BodyType::Dynamic;
            playerBody.gravityScale = 0.f;
            playerBody.velocity = {100.f, 0.f};

            grimar::engine::BoxCollider2D playerCollider{};
            playerCollider.size = {10.f, 10.f};

            GRIMAR_ASSERT(world.AddTransform(player, playerTransform));
            GRIMAR_ASSERT(world.AddRigidBody(player, playerBody));
            GRIMAR_ASSERT(world.AddBoxCollider(player, playerCollider));

            const auto wall = world.CreateEntity();
            grimar::engine::Transform2D wallTransform{};
            wallTransform.SetPosition(20.f, 0.f);

            grimar::engine::RigidBody2D wallBody{};
            wallBody.bodyType = grimar::engine::BodyType::Static;

            grimar::engine::BoxCollider2D wallCollider{};
            wallCollider.size = {10.f, 40.f};

            GRIMAR_ASSERT(world.AddTransform(wall, wallTransform));
            GRIMAR_ASSERT(world.AddRigidBody(wall, wallBody));
            GRIMAR_ASSERT(world.AddBoxCollider(wall, wallCollider));

            for (int i = 0; i < 60; ++i) {
                physics.FixedUpdate(world, 1.0 / 60.0);
            }

            const auto* storedTransform = world.GetTransform(player);
            const auto* storedBody = world.GetRigidBody(player);

            GRIMAR_ASSERT(storedTransform != nullptr);
            GRIMAR_ASSERT(storedBody != nullptr);
            GRIMAR_ASSERT(storedTransform->position.x == 10.f);
            GRIMAR_ASSERT(storedBody->velocity.x == 0.f);

            GRIMAR_LOG_INFO("PhysicsSystem wall resolve tests OK");
        }

        // PhysicsSystem rigid body integration without collider tests
        {
            grimar::engine::World world{};
            grimar::engine::PhysicsSystem physics{};

            const auto entity = world.CreateEntity();
            grimar::engine::Transform2D transform{};

            grimar::engine::RigidBody2D body{};
            body.gravityScale = 0.f;
            body.velocity = {5.f, 7.f};

            GRIMAR_ASSERT(world.AddTransform(entity, transform));
            GRIMAR_ASSERT(world.AddRigidBody(entity, body));

            physics.FixedUpdate(world, 1.0);

            const auto* storedTransform = world.GetTransform(entity);
            GRIMAR_ASSERT(storedTransform != nullptr);
            GRIMAR_ASSERT(storedTransform->position.x == 5.f);
            GRIMAR_ASSERT(storedTransform->position.y == 7.f);

            GRIMAR_LOG_INFO("PhysicsSystem rigid body integration tests OK");
        }

        // TileMap JSON data tests
        {
            grimar::assets::TileMap tileMap{};

            GRIMAR_ASSERT(tileMap.Load("assets/test.tilemap.json"));
            GRIMAR_ASSERT(tileMap.Width() == 12);
            GRIMAR_ASSERT(tileMap.Height() == 4);
            GRIMAR_ASSERT(tileMap.TileWidth() == 64);
            GRIMAR_ASSERT(tileMap.TileHeight() == 64);
            GRIMAR_ASSERT(tileMap.SpriteSheetPath() == "assets/test.sprites.json");

            GRIMAR_ASSERT(tileMap.TileAt(0, 0) == 1);
            GRIMAR_ASSERT(tileMap.TileAt(11, 1) == 2);
            GRIMAR_ASSERT(tileMap.TileAt(0, 1) == 0);
            GRIMAR_ASSERT(tileMap.TileAt(-1, 0) == 0);
            GRIMAR_ASSERT(tileMap.TileAt(12, 0) == 0);

            const auto* solidTile = tileMap.GetDefinition(1);
            GRIMAR_ASSERT(solidTile != nullptr);
            GRIMAR_ASSERT(solidTile->spriteName == "player_idle_2");
            GRIMAR_ASSERT(solidTile->solid);
            GRIMAR_ASSERT(tileMap.IsSolidAt(0, 0));
            GRIMAR_ASSERT(!tileMap.IsSolidAt(0, 1));

            GRIMAR_LOG_INFO("TileMap JSON data tests OK");
        }

        // TileMap solid collision with PhysicsSystem tests
        {
            grimar::assets::TileMap tileMap{};
            GRIMAR_ASSERT(tileMap.Load("assets/test.tilemap.json"));

            grimar::engine::World world{};
            grimar::engine::TileMapSystem tileMapSystem{};
            grimar::engine::PhysicsSystem physics{};

            const auto colliderCount = tileMapSystem.CreateSolidColliders(
                tileMap,
                world,
                {0.f, 0.f}
            );

            GRIMAR_ASSERT(colliderCount == 15);

            const auto player = world.CreateEntity();
            grimar::engine::Transform2D playerTransform{};
            playerTransform.SetPosition(64.f, 160.f);

            grimar::engine::RigidBody2D playerBody{};
            playerBody.bodyType = grimar::engine::BodyType::Dynamic;

            grimar::engine::BoxCollider2D playerCollider{};
            playerCollider.size = {32.f, 32.f};

            GRIMAR_ASSERT(world.AddTransform(player, playerTransform));
            GRIMAR_ASSERT(world.AddRigidBody(player, playerBody));
            GRIMAR_ASSERT(world.AddBoxCollider(player, playerCollider));

            for (int i = 0; i < 120; ++i) {
                physics.FixedUpdate(world, 1.0 / 60.0);
            }

            const auto* storedTransform = world.GetTransform(player);
            const auto* storedBody = world.GetRigidBody(player);

            GRIMAR_ASSERT(storedTransform != nullptr);
            GRIMAR_ASSERT(storedBody != nullptr);
            GRIMAR_ASSERT(storedTransform->position.y == 64.f);
            GRIMAR_ASSERT(storedBody->velocity.y == 0.f);
            GRIMAR_ASSERT(storedBody->grounded);

            GRIMAR_LOG_INFO("TileMap solid collision tests OK");
        }

    }
}
