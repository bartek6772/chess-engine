include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    engine_tests 
    main.cpp
)

target_link_libraries(
    engine_tests 
    GTest::gtest_main
    engine
)

include(GoogleTest)
gtest_discover_tests(engine_tests)