#include "meph/core/types.hpp"

#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace meph::core::ecs
{

class IComponentPool
{
public:
  virtual ~IComponentPool() = default;
  virtual void remove(Entity entity) = 0;
};

template <typename T> class ComponentPool : public IComponentPool
{
public:
  bool contains(Entity entity)
  {
    return m_entity_indices.contains(entity);
  }

  const T& get(Entity entity) const
  {
    return m_data.at(m_entity_indices.at(entity));
  }

  T& get(Entity entity)
  {
    return m_data.at(m_entity_indices.at(entity));
  }

  void insert(Entity entity, T&& component)
  {
    const auto it = m_entity_indices.find(entity);
    if (it != m_entity_indices.cend())
    {
      m_data[it->second] = std::forward<T>(component);
    }
    else
    {
      m_entity_indices.emplace(entity, m_data.size());
      m_index_entities.emplace(m_data.size(), entity);
      m_data.emplace_back(std::forward<T>(component));
    }
  }

  void remove(Entity entity)
  {
    const auto idx_it = m_entity_indices.find(entity);

    if (idx_it == m_entity_indices.cend())
    {
      return;
    }

    const size_t idx = idx_it->second;
    const size_t last_idx = m_data.size() - 1;

    if (idx != last_idx)
    {
      const Entity last_entity = m_index_entities.at(last_idx);
      m_index_entities.at(idx) = last_entity;
      m_entity_indices.at(last_entity) = idx;
      std::swap(m_data.at(idx), m_data.back());
    }

    m_data.pop_back();
    m_entity_indices.erase(idx_it);
    m_index_entities.erase(last_idx);
  }

private:
  std::vector<T> m_data;
  std::unordered_map<Entity, size_t> m_entity_indices;
  std::unordered_map<size_t, Entity> m_index_entities;
};

} // namespace meph::core::ecs
