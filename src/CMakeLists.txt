# ---- Domain (header-only / Qt非依存) ----
add_library(domain INTERFACE)
target_include_directories(domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Application (Qt非依存) ----
add_library(app
    app/CalculatorController.cpp
)
target_link_libraries(app PUBLIC domain PRIVATE calc_warnings calc_hardening calc_coverage)
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
        ui/QtHistoryStore.cpp
        ui/QtHistoryStore.h
    )
    target_link_libraries(ui PUBLIC Qt6::Widgets domain PRIVATE calc_warnings calc_hardening calc_coverage)
    target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    # WIN32: Windows でコンソール窓を出さない / MACOSX_BUNDLE: macOS で .app バンドルを作る
    add_executable(QtCalculator WIN32 MACOSX_BUNDLE
        main.cpp
    )
    target_link_libraries(QtCalculator PRIVATE Qt6::Widgets app ui calc_warnings calc_hardening calc_coverage)
    set_target_properties(QtCalculator PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "QtCalculator"
        MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.koichikurita.qtcalculator"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
endif()
