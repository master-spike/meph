#include "meph/core/ecs/ComponentPool.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <utility>

using meph::core::Entity;
using meph::core::ecs::ComponentPool;

namespace
{
struct NonTrivialType
{
  std::string label;
  int value;

  NonTrivialType(std::string label_, int value_) : label(std::move(label_)), value(value_) {}

  NonTrivialType(const NonTrivialType&) = default;
  NonTrivialType& operator=(const NonTrivialType&) = default;
  NonTrivialType(NonTrivialType&&) = default;
  NonTrivialType& operator=(NonTrivialType&&) = default;
};
} // namespace

TEST_CASE("ComponentPool updates an existing entity in place", "[component-pool]")
{
  ComponentPool<int> pool;
  const Entity entity = 7;

  pool.insert(entity, 10);
  pool.insert(entity, 20);

  REQUIRE(pool.contains(entity));
  REQUIRE(pool.get(entity) == 20);
}

TEST_CASE("ComponentPool preserves the moved entity after removing a middle element",
          "[component-pool]")
{
  ComponentPool<int> pool;
  const Entity first = 1;
  const Entity second = 2;
  const Entity third = 3;

  pool.insert(first, 100);
  pool.insert(second, 200);
  pool.insert(third, 300);

  pool.remove(second);

  REQUIRE_FALSE(pool.contains(second));
  REQUIRE(pool.contains(first));
  REQUIRE(pool.contains(third));
  REQUIRE(pool.get(first) == 100);
  REQUIRE(pool.get(third) == 300);
}

TEST_CASE("ComponentPool removal of an unknown entity is a no-op", "[component-pool]")
{
  ComponentPool<int> pool;
  const Entity entity = 42;

  pool.insert(entity, 99);
  pool.remove(1234);

  REQUIRE(pool.contains(entity));
  REQUIRE(pool.get(entity) == 99);
}

TEST_CASE("ComponentPool handles non-trivial component types", "[component-pool]")
{
  ComponentPool<NonTrivialType> pool;
  const Entity first = 1;
  const Entity second = 2;
  const Entity third = 3;

  pool.insert(first, NonTrivialType{"alpha", 1});
  pool.insert(second, NonTrivialType{"beta", 2});
  pool.insert(third, NonTrivialType{"gamma", 3});

  pool.remove(second);

  REQUIRE(pool.contains(first));
  REQUIRE_FALSE(pool.contains(second));
  REQUIRE(pool.contains(third));
  REQUIRE(pool.get(first).label == "alpha");
  REQUIRE(pool.get(third).value == 3);
  REQUIRE(pool.get(third).label == "gamma");
}
