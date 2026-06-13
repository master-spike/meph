#include "types.hpp"

namespace meph
{

class Scheduler
{
public:
  void enqueue_entity(Entity entity, Tick delay);
  Tick current_tick() const;

private:
  Tick m_current_tick;
};

} // namespace meph
