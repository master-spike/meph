#include "ComponentPool.hpp"

#include "meph/meta/meta.hpp"

#include <bitset>
#include <memory>
#include <type_traits>
#include <vector>

namespace meph::core::ecs
{

template <typename... Ts>
  requires meta::concepts::all_distinct<Ts...> && (std::same_as<std::remove_cvref_t<Ts>, Ts> && ...)
class Registry;

template <typename... Ts>
  requires meta::concepts::all_distinct<Ts...> && (std::same_as<std::remove_cvref_t<Ts>, Ts> && ...)
class Registry
{
public:
  Registry()
  {
    (m_component_pools.emplace_back(std::make_unique<ComponentPool<Ts>>()), ...);
  }

  template <typename... Us> Entity create_entity(Us&&... components)
  {
    size_t index = 0;
    size_t generation = 0;
    if (m_free_indices.empty())
    {
      index = m_signatures.size();
      generation = 0;
      m_generations.emplace_back(0);
      m_signatures.emplace_back();
    }
    else
    {
      index = m_free_indices.back();
      m_free_indices.pop_back();
      generation = m_generations[index];
    }
    Entity entity = (static_cast<Entity>(generation) << 32) | static_cast<Entity>(index);
    (get_pool<std::remove_cvref_t<Us>>()->insert(entity, std::forward<Us>(components)), ...);
    (m_signatures[index].set(get_component_id<std::remove_cvref_t<Us>>()), ...);
    return entity;
  }

  void destroy_entity(Entity entity)
  {
    const size_t index = get_entity_index(entity);
    const size_t generation = get_entity_generation(entity);
    if (is_alive(entity))
    {
      m_free_indices.push_back(index);
      ++m_generations[index];
      (get_pool<Ts>()->remove(entity), ...);
      m_signatures[index].reset();
    }
  }

  bool is_alive(Entity entity) const
  {
    const size_t index = get_entity_index(entity);
    const size_t generation = get_entity_generation(entity);

    return index < m_generations.size() && generation == m_generations[index];
  }

  // Entity must exist
  template <typename... Cs> bool has_components(Entity entity) const
  {
    static constexpr Signature mask = get_mask<Cs...>();
    return (m_signatures[get_entity_index(entity)] & mask) == mask;
  }

  // Entity must exist
  template <typename C> C const& get_component(Entity entity) const
  {
    return get_pool<C>()->get(entity);
  }

  // Entity must exist
  template <typename C> void set_component(Entity entity, C&& component)
  {
    get_pool<C>()->insert(entity, std::forward(component));
    m_signatures[get_entity_index(entity)].set(get_component_id<C>());
  }

  // Entity must exist
  template <typename C> void remove_component(Entity entity)
  {
    get_pool<C>()->remove(entity);
    m_signatures[get_entity_index(entity)].set(get_component_id<C>(), false);
  }

private:
  using Signature = std::bitset<sizeof...(Ts)>;
  std::vector<Signature> m_signatures;
  std::vector<uint32_t> m_generations;
  std::vector<uint32_t> m_free_indices;

  template <typename... Cs> constexpr Signature get_mask() const
  {
    Signature m;
    (m.set(meta::index<Cs, Ts...>::value), ...);
    return m;
  }

  // component pools
  std::vector<std::unique_ptr<IComponentPool>> m_component_pools;

  template <typename C> constexpr size_t get_component_id() const
  {
    return meta::index<C, Ts...>::value;
  }

  template <typename C> ComponentPool<C>* get_pool()
  {
    return static_cast<ComponentPool<C>*>(m_component_pools[get_component_id<C>()].get());
  }

  template <typename C> const ComponentPool<C>* get_pool() const
  {
    return static_cast<const ComponentPool<C>*>(m_component_pools.at(get_component_id<C>()).get());
  }

  size_t get_entity_index(Entity entity) const
  {
    return entity & static_cast<Entity>(std::numeric_limits<uint32_t>::max());
  }

  size_t get_entity_generation(Entity entity) const
  {
    return (entity >> 32) & (static_cast<Entity>(std::numeric_limits<uint32_t>::max()));
  }
};

} // namespace meph::core::ecs
