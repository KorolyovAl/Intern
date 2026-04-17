find_package(Catch2 REQUIRED)

add_executable(tests 
    test_log_filter.cpp
)

target_link_libraries(tests PRIVATE
                    log_filter_core
                    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(tests)