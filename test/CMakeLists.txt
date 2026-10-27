include(FetchContent)

# コミットハッシュで固定し、意図しないコードが取り込まれるのを防ぐ（サプライチェーン対策）
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        b514bdc898e2951020cbdca1304b75f5950d1f59 # v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(CalculatorLogicTest
    CalculatorLogicTest.cpp
)
target_link_libraries(CalculatorLogicTest PRIVATE domain GTest::gtest_main calc_warnings)

include(GoogleTest)
gtest_discover_tests(CalculatorLogicTest)
