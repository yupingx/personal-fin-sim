enable_testing()

add_executable(tests
    test_asset.cpp
    test_dataloading.cpp
)

target_include_directories(tests PRIVATE
    {PROJECT_SOURCE_DIR}/include
)

target_link_libraries(tests
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)