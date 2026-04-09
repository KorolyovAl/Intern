find_package(Catch2 REQUIRED)

add_executable(tests 
    test_log_filter.cpp
    ../src/log_filter.cpp
    ../src/log_filter.h
)

target_link_libraries(tests PRIVATE
                    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(tests)