#include <catch2/catch_test_macros.hpp>
#include "meph/core/scheduler.hpp"

TEST_CASE("Scheduler initialization", "[scheduler]") {
    meph::Scheduler scheduler;
    REQUIRE(scheduler.current_tick() == 0);
}

TEST_CASE("Entity enqueueing", "[scheduler]") {
    meph::Scheduler scheduler;
    meph::Entity entity = 42;
    scheduler.enqueue_entity(entity, 10);
    REQUIRE(scheduler.current_tick() == 0);
}
