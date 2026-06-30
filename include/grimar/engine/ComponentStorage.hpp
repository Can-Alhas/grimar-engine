// ~ Grimar Engine ~

#pragma once

#include <cstdint>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "grimar/engine/Entity.hpp"

namespace grimar::engine {

    template <typename T>
    class ComponentStorage {
    public:
        ComponentStorage() = default;
        ~ComponentStorage() = default;

        ComponentStorage(const ComponentStorage&) = delete;
        ComponentStorage& operator=(const ComponentStorage&) = delete;

        bool Add(Entity entity, T component) {
            if (!entity.IsValid()) {
                return false;
            }

            EnsureSparse(entity.id);

            if (Contains(entity)) {
                m_components[m_sparse[entity.id]] = std::move(component);
                return true;
            }

            const auto denseIndex = static_cast<Index>(m_entities.size());
            m_sparse[entity.id] = denseIndex;
            m_entities.push_back(entity);
            m_components.push_back(std::move(component));
            return true;
        }

        [[nodiscard]] bool Contains(Entity entity) const noexcept {
            const Index index = Lookup(entity);
            return index != InvalidIndex;
        }

        [[nodiscard]] T* Get(Entity entity) noexcept {
            const Index index = Lookup(entity);
            if (index == InvalidIndex) {
                return nullptr;
            }

            return &m_components[index];
        }

        [[nodiscard]] const T* Get(Entity entity) const noexcept {
            const Index index = Lookup(entity);
            if (index == InvalidIndex) {
                return nullptr;
            }

            return &m_components[index];
        }

        bool Remove(Entity entity) noexcept {
            const Index index = Lookup(entity);
            if (index == InvalidIndex) {
                return false;
            }

            const Index lastIndex = static_cast<Index>(m_entities.size() - 1U);
            if (index != lastIndex) {
                m_entities[index] = m_entities[lastIndex];
                m_components[index] = std::move(m_components[lastIndex]);
                m_sparse[m_entities[index].id] = index;
            }

            m_sparse[entity.id] = InvalidIndex;
            m_entities.pop_back();
            m_components.pop_back();
            return true;
        }

        void Clear() noexcept {
            m_entities.clear();
            m_components.clear();
            m_sparse.clear();
        }

        [[nodiscard]] std::uint32_t Count() const noexcept {
            return static_cast<std::uint32_t>(m_components.size());
        }

        template <typename Fn>
        void ForEach(Fn&& fn) {
            for (std::size_t i = 0; i < m_components.size(); ++i) {
                fn(m_entities[i], m_components[i]);
            }
        }

        template <typename Fn>
        void ForEach(Fn&& fn) const {
            for (std::size_t i = 0; i < m_components.size(); ++i) {
                fn(m_entities[i], m_components[i]);
            }
        }

    private:
        using Index = std::uint32_t;
        static constexpr Index InvalidIndex = std::numeric_limits<Index>::max();

        void EnsureSparse(Entity::Id id) {
            if (id >= m_sparse.size()) {
                m_sparse.resize(static_cast<std::size_t>(id) + 1U, InvalidIndex);
            }
        }

        [[nodiscard]] Index Lookup(Entity entity) const noexcept {
            if (!entity.IsValid()) {
                return InvalidIndex;
            }

            if (entity.id >= m_sparse.size()) {
                return InvalidIndex;
            }

            const Index index = m_sparse[entity.id];
            if (index == InvalidIndex || index >= m_entities.size()) {
                return InvalidIndex;
            }

            if (m_entities[index] != entity) {
                return InvalidIndex;
            }

            return index;
        }

        std::vector<Entity> m_entities{};
        std::vector<T> m_components{};
        std::vector<Index> m_sparse{};
    };
}
