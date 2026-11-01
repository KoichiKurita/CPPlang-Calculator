cmake_minimum_required(VERSION 3.24)

project(QtCalculator
    VERSION 1.5.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CALC_BUILD_GUI   "Build the Qt GUI application" ON)
option(CALC_BUILD_TESTS "Build unit tests (GoogleTest)" ON)
option(CALC_ENABLE_COVERAGE "Instrument code for coverage measurement (GCC)" OFF)
option(CALC_BUILD_FUZZERS "Build libFuzzer targets (Clang only)" OFF)

# 自分のコードにだけ適用する警告設定（外部ライブラリには適用しない）
add_library(calc_warnings INTERFACE)
if(MSVC)
    target_compile_options(calc_warnings INTERFACE /W4 /permissive-)
else()
    target_compile_options(calc_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

# 実行ファイルの防御機能（ハードニング）。自分のターゲットにだけ適用する
add_library(calc_hardening INTERFACE)
if(NOT MSVC)
    target_compile_options(calc_hardening INTERFACE
        -fstack-protector-strong                       # スタック破壊の検出
        $<$<NOT:$<CONFIG:Debug>>:-D_FORTIFY_SOURCE=2>   # 危険な標準関数の境界チェック（最適化ビルドで有効）
        -D_GLIBCXX_ASSERTIONS                          # 標準コンテナの範囲外アクセス検出
    )
    target_link_options(calc_hardening INTERFACE -fstack-protector-strong)
    if(WIN32)
        # ASLR（アドレス空間配置のランダム化）と DEP（データ領域の実行禁止）を明示
        target_link_options(calc_hardening INTERFACE
            -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # 位置独立実行形式（PIE）と、再配置後の GOT を読み取り専用にする（Full RELRO）
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
        target_link_options(calc_hardening INTERFACE -pie -Wl,-z,relro -Wl,-z,now)
    endif()
    # macOS は PIE が既定で、Apple のリンカは -z relro / -z now に対応しないため追加しない
endif()

# カバレッジ計測（gcov）。自分のターゲットにだけ適用する
add_library(calc_coverage INTERFACE)
if(CALC_ENABLE_COVERAGE)
    if(MSVC)
        message(FATAL_ERROR "CALC_ENABLE_COVERAGE is supported only with GCC/Clang")
    endif()
    target_compile_options(calc_coverage INTERFACE --coverage -O0 -g)
    target_link_options(calc_coverage INTERFACE --coverage)
endif()

add_subdirectory(src)

if(CALC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(CALC_BUILD_FUZZERS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "CALC_BUILD_FUZZERS requires Clang (libFuzzer)")
    endif()
    add_subdirectory(fuzz)
endif()
