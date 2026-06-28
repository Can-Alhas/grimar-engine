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
        }

        ++m_aliveCount;

        return Entity{id, m_generations[id]};
    }

    void World::DestroyEntity(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return;
        }

        // Bu entity'ye bagli component'leri de siliyoruz.
        // Boylece destroy edilmis entity render/physics sistemlerinde kalmaz.
        m_transforms.erase(entity.id);
        //
        m_spriteRenderers.erase(entity.id);


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

        return m_generations[entity.id] == entity.generation;
    }

#pragma region Transform functions impl
    bool World::AddTransform(Entity entity, Transform2D transform) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        // insert_or_assign:
        // - component yoksa ekler
        // - varsa mevcut component'i yeni degerle degistirir

        m_transforms.insert_or_assign(entity.id, transform);
        return true;
    }

    bool World::HasTransform(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_transforms.find(entity.id) != m_transforms.end();
    }

    Transform2D *World::GetTransform(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        auto it = m_transforms.find(entity.id);
        if (it == m_transforms.end()) {
            return nullptr;
        }

        return &it->second;
    }

    const Transform2D* World::GetTransform(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        auto it = m_transforms.find(entity.id);
        if (it == m_transforms.end()) {
            return nullptr;
        }

        return &it->second;
    }



    bool World::RemoveTransform(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_transforms.erase(entity.id) > 0;
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
        m_spriteRenderers.insert_or_assign(entity.id, spriteRenderer);
        return true;
    }

    bool World::HasSpriteRenderer(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_spriteRenderers.find(entity.id) != m_spriteRenderers.end();
    }

    SpriteRenderer* World::GetSpriteRenderer(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        auto it = m_spriteRenderers.find(entity.id);
        if ( it == m_spriteRenderers.end()) {
            return nullptr;
        }

        return &it->second;
    }

    const SpriteRenderer *World::GetSpriteRenderer(Entity entity) const noexcept {
        if (!IsAlive(entity)) {
            return nullptr;
        }

        auto it = m_spriteRenderers.find(entity.id);
        if ( it == m_spriteRenderers.end()) {
            return nullptr;
        }

        return &it->second;
    }

    bool World::RemoveSpriteRenderer(Entity entity) noexcept {
        if (!IsAlive(entity)) {
            return false;
        }

        return m_spriteRenderers.erase(entity.id) > 0;
    }

#pragma endregion
}
