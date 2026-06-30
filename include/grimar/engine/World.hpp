// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <vector>


#include "grimar/engine/ComponentStorage.hpp"
#include "grimar/engine/Entity.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/CharacterController2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"


namespace grimar::engine {

    struct WorldDebugStats {
        std::uint32_t aliveEntities{0};
        std::uint32_t transforms{0};
        std::uint32_t spriteRenderers{0};
        std::uint32_t animators{0};
        std::uint32_t rigidBodies{0};
        std::uint32_t boxColliders{0};
        std::uint32_t characterControllers{0};
    };

    class World {
    public:
        World()  = default;
        ~World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        [[nodiscard]] Entity CreateEntity() noexcept;
        void DestroyEntity(Entity entity) noexcept;

        [[nodiscard]] bool IsAlive(Entity entity) const noexcept;

        [[nodiscard]] std::uint32_t AliveCount() const noexcept {
            return m_aliveCount;
        }

        [[nodiscard]] WorldDebugStats DebugStats() const noexcept;

        void Clear() noexcept;

        template <typename Fn>
        void ForEachEntity(Fn&& fn) const {
            for (Entity::Id id = 0; id < m_generations.size(); ++id) {
                if (id >= m_alive.size() || !m_alive[id]) {
                    continue;
                }

                fn(Entity{id, m_generations[id]});
            }
        }

#pragma region Transform Functions
        // Transform2D component ekler veya varsa mevcut Transform2D'yi degistirir.
        // Sadece alive entity'lere component ekliyoruz.
        // Dead/stale entity gelirse false doner.
        bool AddTransform(Entity entity, Transform2D transform) noexcept;

        // Entity'de Transform2D var mi diye bakar.
        // Entity alive degilse direkt false doner.
        [[nodiscard]] bool HasTransform(Entity entity) const noexcept;

        // Entity'nin Transform2D component'ini pointer olarak dondurur.
        // Yoksa nullptr doner.
        // Pointer donmemizin sebebi: component yok durumunu temiz temsil etmek.
        [[nodiscard]] Transform2D* GetTransform(Entity entity) noexcept;

        // Const World uzerinden component okumak icin const overload.
        [[nodiscard]] const Transform2D* GetTransform(Entity entity) const noexcept;

        // Entity'nin Transform2D component'ini siler.
        // Gercekten silindiyse true, yoksa false doner.
        bool RemoveTransform(Entity entity) noexcept;

        // Transform2D olan entity'leri gezer.
        // Callback entity ve transform referansi alir.
        // Transform referans oldugu icin callback icinde component degistirilebilir.
        template <typename Fn>
        void ForEachTransform(Fn&& fn) {
            m_transforms.ForEach([&](Entity entity, Transform2D& transform) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, transform);
            });
        }
#pragma endregion


#pragma region SpriteRenderer Functions

        // SpriteRenderer component ekler veya varsa mevcut SpriteRenderer'i degistirir.
        // Sadece alive entity'lere component ekliyoruz.
        // Dead/stale entity gelirse false doner.
        bool AddSpriteRenderer(Entity entity, SpriteRenderer spriteRenderer) noexcept;

        // Entity'de SpriteRenderer var mi diye bakar.
        // Entity alive degilse false doner.
        [[nodiscard]] bool HasSpriteRenderer(Entity entity) const noexcept;

        // Entity'nin SpriteRenderer component'ini pointer olarak dondurur.
        // Yoksa nullptr doner.
        [[nodiscard]] SpriteRenderer* GetSpriteRenderer(Entity entity) noexcept;

        // Const World uzerinden SpriteRenderer okumak icin const overload.
        [[nodiscard]] const SpriteRenderer* GetSpriteRenderer(Entity entity) const noexcept;

        // Entity'nin SpriteRenderer component'ini siler.
        // Gercekten silindiyse true, yoksa false doner.
        bool RemoveSpriteRenderer(Entity entity) noexcept;

        // SpriteRenderer olan entity'leri gezer.
        // Callback entity ve SpriteRenderer referansi alir.
        template <typename Fn>
        void ForEachSpriteRenderer(Fn&& fn) {
            m_spriteRenderers.ForEach([&](Entity entity, SpriteRenderer& spriteRenderer) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, spriteRenderer);
            });
        }

#pragma endregion

#pragma region Animation Component Functions

        bool AddAnimator2D(Entity entity, Animator2D animator) noexcept;
        [[nodiscard]] bool HasAnimator2D(Entity entity) const noexcept;
        [[nodiscard]] Animator2D* GetAnimator2D(Entity entity) noexcept;
        [[nodiscard]] const Animator2D* GetAnimator2D(Entity entity) const noexcept;
        bool RemoveAnimator2D(Entity entity) noexcept;

        template <typename Fn>
        void ForEachAnimator2D(Fn&& fn) {
            m_animators.ForEach([&](Entity entity, Animator2D& animator) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, animator);
            });
        }

#pragma endregion

#pragma region Character Controller Component Functions

        bool AddCharacterController2D(Entity entity, CharacterController2D controller) noexcept;
        [[nodiscard]] bool HasCharacterController2D(Entity entity) const noexcept;
        [[nodiscard]] CharacterController2D* GetCharacterController2D(Entity entity) noexcept;
        [[nodiscard]] const CharacterController2D* GetCharacterController2D(Entity entity) const noexcept;
        bool RemoveCharacterController2D(Entity entity) noexcept;

        template <typename Fn>
        void ForEachCharacterController2D(Fn&& fn) {
            m_characterControllers.ForEach([&](Entity entity, CharacterController2D& controller) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, controller);
            });
        }

#pragma endregion

#pragma region Physics Component Functions

        bool AddRigidBody(Entity entity, RigidBody2D rigidBody) noexcept;
        [[nodiscard]] bool HasRigidBody(Entity entity) const noexcept;
        [[nodiscard]] RigidBody2D* GetRigidBody(Entity entity) noexcept;
        [[nodiscard]] const RigidBody2D* GetRigidBody(Entity entity) const noexcept;
        bool RemoveRigidBody(Entity entity) noexcept;


        template<typename Fn>
        void ForEachRigidBody(Fn&& fn) {
            m_rigidBodies.ForEach([&](Entity entity, RigidBody2D& rigidBody) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, rigidBody);
            });
        }


        bool AddBoxCollider(Entity entity, BoxCollider2D collider) noexcept;
        [[nodiscard]] bool HasBoxCollider(Entity entity) const noexcept;
        [[nodiscard]] BoxCollider2D* GetBoxCollider(Entity entity) noexcept;
        [[nodiscard]] const BoxCollider2D* GetBoxCollider(Entity entity) const noexcept;
        bool RemoveBoxCollider(Entity entity) noexcept;

        template <typename Fn>
        void ForEachBoxCollider(Fn&& fn) {
            m_boxColliders.ForEach([&](Entity entity, BoxCollider2D& collider) {
                if (!IsAlive(entity)) {
                    return;
                }

                fn(entity, collider);
            });
        }
#pragma endregion
    private:
        // Her entity id icin aktif generation degerini tutar.
        // IsAlive(entity) buradan generation karsilastirir.
        std::vector<Entity::Generation> m_generations{};

        // Destroy edilmis entity id'lerini tekrar kullanmak icin free-list.
        // Bu performans icin yeni id uretmek yerine eski slotlari kullanmamizi saglar.
        std::vector<Entity::Id> m_freeIds{};
        std::vector<std::uint8_t> m_alive{};

        // Su an yasayan entity sayisi.
        std::uint32_t m_aliveCount{0};

        // Component storage'lar generic sparse-set uzerinden tutulur.
        // Public World API sabit kalir; sistemler storage detayini bilmez.
        ComponentStorage<Transform2D> m_transforms{};
        ComponentStorage<SpriteRenderer> m_spriteRenderers{};

        ComponentStorage<Animator2D> m_animators{};
        ComponentStorage<CharacterController2D> m_characterControllers{};


        ComponentStorage<RigidBody2D>   m_rigidBodies{};
        ComponentStorage<BoxCollider2D> m_boxColliders{};


    };
}
