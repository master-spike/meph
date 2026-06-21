#include "meph/core/ecs/Registry.hpp"

#include <catch2/catch_test_macros.hpp>

using meph::core::Entity;
using meph::core::ecs::Registry;

namespace
{
struct Position
{
  int x;
  int y;
};

struct Health
{
  int value;
};
} // namespace

TEST_CASE("Registry creates entities and stores components", "[registry]")
{
  Registry<Position, Health> registry;

  const Entity entity = registry.create_entity(Position{3, 4}, Health{10});

  REQUIRE(registry.has_components<Position, Health>(entity));
  REQUIRE(registry.get_component<Position>(entity).x == 3);
  REQUIRE(registry.get_component<Position>(entity).y == 4);
  REQUIRE(registry.get_component<Health>(entity).value == 10);
}

TEST_CASE("Registry updates an existing component through set_component", "[registry]")
{
  Registry<Position, Health> registry;

  const Entity entity = registry.create_entity(Position{1, 2}, Health{5});

  registry.set_component(entity, Position{9, 9});

  REQUIRE(registry.get_component<Position>(entity).x == 9);
  REQUIRE(registry.get_component<Position>(entity).y == 9);
  REQUIRE(registry.get_component<Health>(entity).value == 5);
}

TEST_CASE("Registry removes a component and preserves the rest", "[registry]")
{
  Registry<Position, Health> registry;

  const Entity entity = registry.create_entity(Position{1, 2}, Health{5});

  registry.remove_component<Position>(entity);

  REQUIRE_FALSE(registry.has_components<Position>(entity));
  REQUIRE(registry.has_components<Health>(entity));
  REQUIRE(registry.get_component<Health>(entity).value == 5);
}

TEST_CASE("Registry destroy_entity clears component state and allows reuse", "[registry]")
{
  Registry<Position, Health> registry;

  const Entity first = registry.create_entity(Position{1, 1}, Health{100});
  registry.destroy_entity(first);

  const Entity second = registry.create_entity(Position{2, 2}, Health{200});

  REQUIRE_FALSE(registry.is_alive(first));
  REQUIRE(registry.is_alive(second));
  REQUIRE(registry.get_component<Position>(second).x == 2);
  REQUIRE(registry.get_component<Health>(second).value == 200);
}
