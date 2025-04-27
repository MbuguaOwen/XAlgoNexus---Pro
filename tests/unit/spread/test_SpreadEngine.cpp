// tests/core/spread/test_SpreadEngine.cpp

#include <catch2/catch_test_macros.hpp>
#include "core/spread/SpreadEngine.hpp"
#include "core/messaging/MessageBus.hpp"

using namespace XAlgo;

TEST_CASE("SpreadEngine basic spread calculation", "[SpreadEngine]") {
    EventQueue dummy_queue;
    SpreadEngine engine(dummy_queue, 0.0001); // default threshold

    ForexTick eurusd { "EUR/USD", 1.1000, 1.1002, std::chrono::system_clock::now() };
    ForexTick gbpusd { "GBP/USD", 1.3000, 1.3002, std::chrono::system_clock::now() };
    ForexTick eurgbp { "EUR/GBP", 0.8500, 0.8502, std::chrono::system_clock::now() };

    SECTION("No signal generated if spread small") {
        engine.update(eurusd);
        engine.update(gbpusd);
        engine.update(eurgbp);

        REQUIRE(dummy_queue.size_approx() == 0);
    }

    SECTION("Signal generated when spread exceeds threshold") {
        ForexTick tweaked_gbpusd { "GBP/USD", 1.2000, 1.2002, std::chrono::system_clock::now() };

        engine.update(eurusd);
        engine.update(tweaked_gbpusd); // cause big spread
        engine.update(eurgbp);

        REQUIRE(dummy_queue.size_approx() > 0);
    }
}
