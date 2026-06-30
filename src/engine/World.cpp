// ~ Grimar Engine ~

#include "grimar/engine/World.hpp"

namespace grimar::engine {

    Entity World::CreateEntity() noexcept {
        Entity::Id id{Entity::InvalidId};

        // Once daha once silinmis bir id var mi diye bakiyoruz.
        // Varsa onu tekrar kullanmak yeni id uretmekten daha verimli.
        if (!m_freeIds.empty()) {
            id = m_freeIds.back();
            m_freeIds.pop_back();
        } else {
            id = static_cast<Entity::Id>(m_generations.size());

            // Yeni entity generation 1 ile baslar.
            // Generation 0 invalid kabul ediliyor.
            m_generations.push_back(1);
            m_alive.push_back(0);
        }

        m_alive[id] = 1;
        ++m_aliveCount;

        return Entity{id, m_generations[id]};
    }

    void World::DestroyEntity(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return;
        }

        // Bu entity'ye bagli component'leri de siliyoruz.
        // Boylece destroy edilmis entity render/physics sistemlerinde kalmaz.
        m_transforms.Remove(entity);
        //
        m_spriteRenderers.Remove(entity);
        m_animators.Remove(entity);

        //
        m_rigidBodies.Remove(entity);
        m_boxColliders.Remove(entity);
        m_characterControllers.Remove(entity);
        //

        m_alive[entity.id] = 0;

        // Generation artiriyoruz.
        // Bu sayede eski handle stale hale gelir.
        ++m_generations[entity.id];

        // Teorik olarak overflow generation'i 0 yaparsa tekrar 1'e cekiyoruz,
        // cunku 0 invalid generation olarak ayrildi.
        if (m_generations[entity.id] == Entity::InvalidGeneration) {
            ++m_generations[entity.id];
        }

        // Id tekrar kullanilabilir hale gelir.
        m_freeIds.push_back(entity.id);

        if (m_aliveCount > 0) {
            --m_aliveCount;
        }
    }

    bool World::IsAlive(Entity entity) const noexcept {
        if (!entity.IsValid()) {
            return false;
        }

        if (entity.id >= m_generations.size()) {
            return false;
        }

        if (entity.id >= m_alive.size() || !m_alive[entity.id]) {
            return false;
        }

        return m_generations[entity.id] == entity.generation;
    }

    WorldDebugStats World::DebugStats() const noexcept {
        return WorldDebugStats{
            m_aliveCount,
            m_transforms.Count(),
            m_spriteRenderers.Count(),
            m_animators.Count(),
            m_rigidBodies.Count(),
            m_boxColliders.Count(),
            m_characterControllers.Count()
        };
    }

    void World::Clear() noexcept {
        m_generations.clear();
        m_freeIds.clear();
        m_alive.clear();
        m_aliveCount = 0;

        m_transforms.Clear();
        m_spriteRenderers.Clear();
        m_animators.Clear();
        m_characterControllers.Clear();
        m_rigidBodies.Clear();
        m_boxColliders.Clear();
    }

#pragma region Transform functions impl
    bool World::AddTransform(Entity entity, Transform2D transform) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        // insert_or_assign:
        // - component yoksa ekler
        // - varsa mevcut component'i yeni degerle degistirir

        m_transforms.Add(entity, transform);
        return true;
    }

    bool World::HasTransform(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_transforms.Contains(entity);
    }

    Transform2D *World::GetTransform(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_transforms.Get(entity);
    }

    const Transform2D* World::GetTransform(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_transforms.Get(entity);
    }



    bool World::RemoveTransform(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_transforms.Remove(entity);
    }

#pragma endregion

#pragma region SpriteRenderer functions impl
    bool World::AddSpriteRenderer(Entity entity, SpriteRenderer spriteRenderer) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        // insert_or_assign:
        //component yoksa ekler
        // varsa mevcut componenti  yeni degerle degisir
        m_spriteRenderers.Add(entity, spriteRenderer);
        return true;
    }

    bool World::HasSpriteRenderer(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_spriteRenderers.Contains(entity);
    }

    SpriteRenderer* World::GetSpriteRenderer(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_spriteRenderers.Get(entity);
    }

    const SpriteRenderer *World::GetSpriteRenderer(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_spriteRenderers.Get(entity);
    }

    bool World::RemoveSpriteRenderer(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_spriteRenderers.Remove(entity);
    }

#pragma endregion

#pragma region Animation component functions impl

    bool World::AddAnimator2D(Entity entity, Animator2D animator) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        m_animators.Add(entity, animator);
        return true;
    }

    bool World::HasAnimator2D(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_animators.Contains(entity);
    }

    Animator2D* World::GetAnimator2D(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_animators.Get(entity);
    }

    const Animator2D* World::GetAnimator2D(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_animators.Get(entity);
    }

    bool World::RemoveAnimator2D(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_animators.Remove(entity);
    }

#pragma endregion

#pragma region Character controller component functions impl

    bool World::AddCharacterController2D(Entity entity, CharacterController2D controller) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        m_characterControllers.Add(entity, controller);
        return true;
    }

    bool World::HasCharacterController2D(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_characterControllers.Contains(entity);
    }

    CharacterController2D* World::GetCharacterController2D(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_characterControllers.Get(entity);
    }

    const CharacterController2D* World::GetCharacterController2D(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_characterControllers.Get(entity);
    }

    bool World::RemoveCharacterController2D(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_characterControllers.Remove(entity);
    }

#pragma endregion


#pragma region Physics component function impl

    bool World::AddRigidBody(Entity entity, RigidBody2D rigidBody) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        m_rigidBodies.Add(entity, rigidBody);
        return true;
    }

    bool World::HasRigidBody(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_rigidBodies.Contains(entity);
    }

    RigidBody2D* World::GetRigidBody(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_rigidBodies.Get(entity);
    }

    const RigidBody2D *World::GetRigidBody(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_rigidBodies.Get(entity);
    }

    bool World::RemoveRigidBody(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_rigidBodies.Remove(entity);
    }

    // colliders functions
    bool World::AddBoxCollider(Entity entity, BoxCollider2D collider) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        m_boxColliders.Add(entity, collider);
        return true;
    }

    bool World::HasBoxCollider(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_boxColliders.Contains(entity);
    }

    BoxCollider2D* World::GetBoxCollider(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_boxColliders.Get(entity);
    }

    const BoxCollider2D *World::GetBoxCollider(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        return m_boxColliders.Get(entity);
    }

    bool World::RemoveBoxCollider(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_boxColliders.Remove(entity);
    }

#pragma endregion
}
