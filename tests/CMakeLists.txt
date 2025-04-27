# Modern Catch2 setup
find_package(Catch2 CONFIG REQUIRED)

add_executable(SpreadEngineTests
    unit/spread/test_SpreadEngine.cpp
    test_main.cpp
)

target_link_libraries(SpreadEngineTests PRIVATE
    core
    fmt::fmt
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch2)
catch_discover_tests(SpreadEngineTests)
