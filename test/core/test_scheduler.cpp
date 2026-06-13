#include "meph/core/scheduler.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Scheduler initialization", "[scheduler]")
{
  meph::Scheduler scheduler;
  REQUIRE(scheduler.current_tick() == 0);
  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 0);
}

TEST_CASE("Entity enqueueing", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 42;
  scheduler.enqueue_entity(entity1, 10);
  REQUIRE(scheduler.current_tick() == 0);
  REQUIRE(scheduler.peek_entity() == std::make_pair(10, entity1));
  REQUIRE(scheduler.get_scheduled_tick(entity1) == 10);

  REQUIRE(scheduler.pop_entity().value() == entity1);
  REQUIRE(scheduler.current_tick() == 10);
}

TEST_CASE("Stable insertion", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 1;
  meph::Entity entity2 = 2;
  meph::Entity entity3 = 3;

  scheduler.enqueue_entity(entity1, 10);
  scheduler.enqueue_entity(entity2, 10);
  scheduler.enqueue_entity(entity3, 10);

  REQUIRE(scheduler.current_tick() == 0);
  REQUIRE(scheduler.pop_entity().value() == entity1);
  REQUIRE(scheduler.current_tick() == 10);
  REQUIRE(scheduler.pop_entity().value() == entity2);
  REQUIRE(scheduler.current_tick() == 10);
  REQUIRE(scheduler.pop_entity().value() == entity3);
  REQUIRE(scheduler.current_tick() == 10);
  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 10);
}

TEST_CASE("Enqueueing delay relative to current tick", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity first = 1;
  meph::Entity second = 2;

  scheduler.enqueue_entity(first, 10);
  REQUIRE(scheduler.pop_entity().value() == first);
  REQUIRE(scheduler.current_tick() == 10);

  scheduler.enqueue_entity(second, 5);
  REQUIRE(scheduler.peek_entity() == std::make_pair(15, second));
  REQUIRE(scheduler.get_scheduled_tick(second) == 15);

  REQUIRE(scheduler.pop_entity().value() == second);
  REQUIRE(scheduler.current_tick() == 15);
}

TEST_CASE("Entities should be popped in scheduled order", "[scheduler]")
{
  meph::Scheduler scheduler;
  const meph::Entity entity1 = 1;
  const meph::Entity entity2 = 2;
  const meph::Entity entity3 = 3;
  const meph::Entity entity4 = 4;

  scheduler.enqueue_entity(entity1, 20);
  scheduler.enqueue_entity(entity2, 5);
  scheduler.enqueue_entity(entity3, 15);
  scheduler.enqueue_entity(entity4, 5);

  REQUIRE(scheduler.peek_entity() == std::make_pair(5, entity2));

  REQUIRE(scheduler.pop_entity() == entity2);
  REQUIRE(scheduler.current_tick() == 5);

  REQUIRE(scheduler.pop_entity() == entity4);
  REQUIRE(scheduler.current_tick() == 5);

  REQUIRE(scheduler.pop_entity() == entity3);
  REQUIRE(scheduler.current_tick() == 15);

  REQUIRE(scheduler.pop_entity() == entity1);
  REQUIRE(scheduler.current_tick() == 20);

  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 20);
}

TEST_CASE("Rescheduling should overwrite existing entity's schedule", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 1;
  meph::Entity entity2 = 2;

  scheduler.enqueue_entity(entity1, 10);
  scheduler.enqueue_entity(entity2, 5);

  scheduler.enqueue_entity(entity1, 1);

  REQUIRE(scheduler.get_scheduled_tick(entity1) == 1);
  REQUIRE(scheduler.peek_entity() == std::make_pair(1, entity1));

  REQUIRE(scheduler.pop_entity() == entity1);
  REQUIRE(scheduler.current_tick() == 1);

  REQUIRE(scheduler.pop_entity() == entity2);
  REQUIRE(scheduler.current_tick() == 5);

  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 5);
}

TEST_CASE("Rescheduling an entity should leave a tombstone", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 1;
  meph::Entity entity2 = 2;

  scheduler.enqueue_entity(entity1, 1);
  scheduler.enqueue_entity(entity2, 5);

  scheduler.enqueue_entity(entity1, 10);

  REQUIRE(scheduler.peek_entity() == std::make_pair(5, entity2));
  REQUIRE(scheduler.pop_entity() == 2);

  REQUIRE(scheduler.peek_entity() == std::make_pair(10, entity1));
}

TEST_CASE("Descheduling should remove entity from the queue", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 1;
  meph::Entity entity2 = 2;
  meph::Entity entity3 = 3;

  scheduler.enqueue_entity(entity1, 10);
  scheduler.enqueue_entity(entity2, 5);
  scheduler.enqueue_entity(entity3, 15);

  scheduler.dequeue_entity(entity2);

  REQUIRE(scheduler.get_scheduled_tick(entity2) == std::nullopt);
  REQUIRE(scheduler.peek_entity() == std::make_pair(10, entity1));

  REQUIRE(scheduler.pop_entity() == entity1);
  REQUIRE(scheduler.current_tick() == 10);

  REQUIRE(scheduler.pop_entity() == entity3);
  REQUIRE(scheduler.current_tick() == 15);

  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 15);
}

TEST_CASE("Descheduling should be a no-op if the entity doesn't exist", "[scheduler]")
{
  meph::Scheduler scheduler;
  meph::Entity entity1 = 1;
  meph::Entity entity2 = 2;
  meph::Entity non_existent = 3;

  scheduler.enqueue_entity(entity1, 10);
  scheduler.enqueue_entity(entity2, 5);

  scheduler.dequeue_entity(non_existent);

  REQUIRE(scheduler.get_scheduled_tick(entity1) == 10);
  REQUIRE(scheduler.get_scheduled_tick(entity2) == 5);
  REQUIRE(scheduler.get_scheduled_tick(non_existent) == std::nullopt);
  REQUIRE(scheduler.peek_entity() == std::make_pair(5, entity2));

  REQUIRE(scheduler.pop_entity() == entity2);
  REQUIRE(scheduler.current_tick() == 5);

  REQUIRE(scheduler.pop_entity() == entity1);
  REQUIRE(scheduler.current_tick() == 10);

  REQUIRE(scheduler.pop_entity() == std::nullopt);
  REQUIRE(scheduler.current_tick() == 10);
}
