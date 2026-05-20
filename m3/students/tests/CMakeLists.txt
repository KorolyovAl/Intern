find_package(Catch2 3 QUIET)

if (NOT Catch2_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
    )

    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(tests
                test_students.cpp
)

target_link_libraries(tests PRIVATE
                        students_lib
                        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(tests)