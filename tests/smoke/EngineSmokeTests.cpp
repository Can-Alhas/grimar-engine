// ~ Grimar Engine ~
#include "EngineSmokeTests.hpp"

#include <vector>

#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/engine/Entity.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"




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

    }
}
