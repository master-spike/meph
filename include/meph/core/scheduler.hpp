#include "types.hpp"

#include <functional>
#include <optional>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace meph
{

class Scheduler
{
public:
  Scheduler();
  void enqueue_entity(Entity entity, Tick delay);
  void dequeue_entity(Entity entity);
  std::optional<Entity> pop_entity();
  std::optional<std::pair<Tick, Entity>> peek_entity() const;
  std::optional<Tick> get_scheduled_tick(Entity entity) const;
  Tick current_tick() const;

private:
  Tick m_current_tick;
  uint64_t m_sequence_number;
  std::priority_queue<std::tuple<Tick, uint64_t, Entity>,
                      std::vector<std::tuple<Tick, uint64_t, Entity>>,
                      std::greater<std::tuple<Tick, uint64_t, Entity>>>
    m_queue;
  std::unordered_map<Entity, std::pair<Tick, uint64_t>> m_valid_map;

  void clean_top();
};

} // namespace meph
