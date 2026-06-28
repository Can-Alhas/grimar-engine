// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/engine/Entity.hpp"

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


    };
}
