# ---- Domain (header-only / Qt非依存) ----
add_library(domain INTERFACE)
target_include_directories(domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Application (Qt非依存) ----
add_library(app
    app/CalculatorController.cpp
)
target_link_libraries(app PUBLIC domain PRIVATE calc_warnings)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---- UI / 実行ファイル（Qtが必要な部分だけ）----
if(CALC_BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    add_library(ui
        ui/QtCalculatorUI.cpp
        ui/QtCalculatorUI.h
    )
    target_link_libraries(ui PUBLIC Qt6::Widgets domain PRIVATE calc_warnings)
    target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    add_executable(QtCalculator WIN32
        main.cpp
    )
    target_link_libraries(QtCalculator PRIVATE Qt6::Widgets app ui calc_warnings)
endif()
