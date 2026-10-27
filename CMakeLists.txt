cmake_minimum_required(VERSION 3.24)

project(QtCalculator
    VERSION 1.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CALC_BUILD_GUI   "Build the Qt GUI application" ON)
option(CALC_BUILD_TESTS "Build unit tests (GoogleTest)" ON)

# 自分のコードにだけ適用する警告設定（外部ライブラリには適用しない）
add_library(calc_warnings INTERFACE)
if(MSVC)
    target_compile_options(calc_warnings INTERFACE /W4 /permissive-)
else()
    target_compile_options(calc_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

if(CALC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
