set(TEST_PGRM checkpoint_test)
add_executable(${TEST_PGRM} header_test.cpp checkpoint_test.cpp)
target_link_libraries(${TEST_PGRM} GTest::gtest_main ${PROJECT_NAME})
gtest_discover_tests(${TEST_PGRM})