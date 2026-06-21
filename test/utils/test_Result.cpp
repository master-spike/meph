#include "meph/utils/result.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

using meph::util::make_error;
using meph::util::make_success;

TEST_CASE("flat_map binds successes and preserves errors", "[result]")
{
  auto success = make_success<int, std::string>(41);
  auto bound = success.flat_map([](int value) { return make_success<int, std::string>(value + 1); });

  REQUIRE(bound.is_success());
  REQUIRE(bound.get() == 42);

  auto error = make_error<std::string, int>("boom");
  auto forwarded = error.flat_map([](int) { return make_success<int, std::string>(7); });

  REQUIRE(forwarded.is_error());
  REQUIRE(forwarded.get_err() == "boom");
}

TEST_CASE("recover_with handles errors with a recovery callable", "[result]")
{
  auto error = make_error<std::string, int>("failed");
  auto recovered = error.recover_with([](const std::string& message) {
    return make_success<int>(static_cast<int>(message.size()));
  });

  REQUIRE(recovered.is_success());
  REQUIRE(recovered.get() == 6);
}

TEST_CASE("consume forwards mutable success values", "[result]")
{
  auto success = make_success<int, std::string>(41);
  int observed = 0;

  auto& consumed = success.consume([&](int& value) {
    value += 1;
    observed = value;
  });

  REQUIRE(&consumed == &success);
  REQUIRE(success.get() == 42);
  REQUIRE(observed == 42);
}

TEST_CASE("consume_err forwards mutable error values", "[result]")
{
  auto error = make_error<std::string, int>("boom");
  std::string observed;

  auto& consumed = error.consume_err([&](std::string& message) {
    message += "!";
    observed = message;
  });

  REQUIRE(&consumed == &error);
  REQUIRE(error.get_err() == "boom!");
  REQUIRE(observed == "boom!");
}
