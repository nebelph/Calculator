add_executable(ExpressionManagerTests
    test_ExpressionManager.cpp
    ${CMAKE_SOURCE_DIR}/source/ExpressionManager.cpp
)

target_include_directories(ExpressionManagerTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ExpressionManagerTests
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(ExpressionManagerTests)