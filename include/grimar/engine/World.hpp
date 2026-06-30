// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>


#include "grimar/engine/Entity.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"


namespace grimar::engine {

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
            for (auto& [id, transform] : m_transforms) {
                Entity entity{id, m_generations[id]};


                // Destroy edilmis/stale entity'leri callback'e gondermiyoruz.
                if (!IsAlive(entity)) {
                    continue;
                }

                fn(entity, transform);
            }
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
            for (auto& [id, spriteRenderer] : m_spriteRenderers) {
                Entity entity{id, m_generations[id]};

                // Destroy edilmis/stale entity'leri callback'e gondermiyoruz.
                if (!IsAlive(entity)) {
                    continue;
                }

                fn(entity, spriteRenderer);
            }
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
            for (auto& [id, animator] : m_animators) {
                Entity entity{id, m_generations[id]};

                if (!IsAlive(entity)) {
                    continue;
                }

                fn(entity, animator);
            }
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
            for (auto& [id, rigidBody] : m_rigidBodies) {
                Entity entity{id, m_generations[id]};

                if (!IsAlive(entity)) {
                    continue;
                }


                fn(entity, rigidBody);
            }
        }


        bool AddBoxCollider(Entity entity, BoxCollider2D collider) noexcept;
        [[nodiscard]] bool HasBoxCollider(Entity entity) const noexcept;
        [[nodiscard]] BoxCollider2D* GetBoxCollider(Entity entity) noexcept;
        [[nodiscard]] const BoxCollider2D* GetBoxCollider(Entity entity) const noexcept;
        bool RemoveBoxCollider(Entity entity) noexcept;

        template <typename Fn>
        void ForEachBoxCollider(Fn&& fn) {
            for (auto& [id, collider] : m_boxColliders ) {
                Entity entity{id, m_generations[id]};

                if (!IsAlive(entity)) {
                    continue;
                }

                fn(entity, collider);
            }
        }
#pragma endregion
    private:
        // Her entity id icin aktif generation degerini tutar.
        // IsAlive(entity) buradan generation karsilastirir.
        std::vector<Entity::Generation> m_generations{};

        // Destroy edilmis entity id'lerini tekrar kullanmak icin free-list.
        // Bu performans icin yeni id uretmek yerine eski slotlari kullanmamizi saglar.
        std::vector<Entity::Id> m_freeIds{};

        // Su an yasayan entity sayisi.
        std::uint32_t m_aliveCount{0};

        //MVP component storage:
        // Entity id -> Transform2D
        // Generic ECS degil; once tek component mantigi

        std::unordered_map<Entity::Id, Transform2D> m_transforms{};

        // MVP SpriteRenderer component storage:
        // Entity id -> SpriteRenderer.
        //
        // Bu da Transform2D gibi unordered_map ile basliyor.
        // Ileride dense/sparse storage'a gecilebilir.
        std::unordered_map<Entity::Id, SpriteRenderer> m_spriteRenderers{};

        std::unordered_map<Entity::Id, Animator2D> m_animators{};


        std::unordered_map<Entity::Id, RigidBody2D>   m_rigidBodies{};
        std::unordered_map<Entity::Id, BoxCollider2D> m_boxColliders{};


    };
}
