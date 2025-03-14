set(TEST_PGRM svector_test)

add_executable(${TEST_PGRM} 
    element_test.cpp 
    svector_test.cpp 
)
target_link_libraries(${TEST_PGRM} GTest::gtest_main GTest::gtest ${PROJECT_NAME})
gtest_discover_tests(${TEST_PGRM})
