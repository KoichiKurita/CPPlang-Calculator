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
    CalculatorControllerTest.cpp
    NumberFormatTest.cpp
    ErrorMessageTest.cpp
    KeyInputTest.cpp
    ExpressionTest.cpp
    HistoryTest.cpp
)
target_link_libraries(CalculatorLogicTest PRIVATE app domain GTest::gtest_main calc_warnings calc_hardening calc_coverage)

include(GoogleTest)
gtest_discover_tests(CalculatorLogicTest)

# ---- GUI テスト（Qt の画面を offscreen で動かし、ボタン・キー入力の結果を確かめる）----
if(CALC_BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(QtCalculatorUITest
        QtCalculatorUITest.cpp
    )
    target_link_libraries(QtCalculatorUITest PRIVATE app ui Qt6::Test GTest::gtest calc_warnings calc_hardening calc_coverage)
    gtest_discover_tests(QtCalculatorUITest)

    # QSettings による履歴の保存（一時ファイルを使う）
    add_executable(QtHistoryStoreTest
        QtHistoryStoreTest.cpp
    )
    target_link_libraries(QtHistoryStoreTest PRIVATE ui GTest::gtest_main calc_warnings calc_hardening calc_coverage)
    gtest_discover_tests(QtHistoryStoreTest)
endif()
