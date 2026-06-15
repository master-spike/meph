#include "meph/core/ecs/Registry.hpp"
#include <string>

using meph::core::ecs::Registry;

template <typename... Ts>
concept can_instantiate_Registry = requires { typename Registry<Ts...>; };

struct A
{
  int a;
};
struct B
{
  int a;
};

static_assert(can_instantiate_Registry<uint32_t, bool, uint64_t, std::string, A, B>);
static_assert(!can_instantiate_Registry<const uint32_t>);
static_assert(!can_instantiate_Registry<volatile uint32_t>);
static_assert(!can_instantiate_Registry<uint32_t&>);
static_assert(!can_instantiate_Registry<const B&>);
static_assert(!can_instantiate_Registry<A, B, const B>);
