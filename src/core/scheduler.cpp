#include "meph/core/scheduler.hpp"

namespace meph
{

Scheduler::Scheduler() : m_current_tick(0), m_sequence_number(0) {}

void Scheduler::enqueue_entity(Entity entity, Tick delay)
{
  const Tick scheduled_tick = current_tick() + delay;
  const uint64_t entity_sequence_number = m_sequence_number;
  m_valid_map.insert_or_assign(entity, std::make_pair(scheduled_tick, entity_sequence_number));
  m_queue.emplace(scheduled_tick, entity_sequence_number, entity);
  ++m_sequence_number;
  clean_top();
}

void Scheduler::dequeue_entity(Entity entity)
{
  m_valid_map.erase(entity);
  clean_top();
}

std::optional<Entity> Scheduler::pop_entity()
{
  const auto entity_opt = peek_entity();
  if (!entity_opt.has_value())
  {
    return std::nullopt;
  }
  const auto& [tick, entity] = entity_opt.value();
  m_valid_map.erase(entity);
  m_current_tick = tick;
  clean_top();
  return entity;
}

std::optional<std::pair<Tick, Entity>> Scheduler::peek_entity() const
{
  if (m_queue.empty())
  {
    return std::nullopt;
  }

  const auto& [tick, _, entity] = m_queue.top();
  return std::make_pair(tick, entity);
}

std::optional<Tick> Scheduler::get_scheduled_tick(Entity entity) const
{
  const auto it = m_valid_map.find(entity);
  if (it == m_valid_map.cend())
    return std::nullopt;
  return it->second.first;
}

Tick Scheduler::current_tick() const
{
  return m_current_tick;
}

void Scheduler::clean_top()
{
  while (!m_queue.empty())
  {
    const auto& [_, sequence_number, entity] = m_queue.top();
    const auto it = m_valid_map.find(entity);
    if (it != m_valid_map.cend() && sequence_number == it->second.second)
    {
      return;
    }
    m_queue.pop();
  }
}

} // namespace meph
