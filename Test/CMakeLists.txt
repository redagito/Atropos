add_executable(atropos_tests
    source/ConfigFileTest.cpp
)

target_link_libraries(atropos_tests PRIVATE
    atropos
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(atropos_tests)
