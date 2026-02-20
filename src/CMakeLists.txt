find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Domain (header-only)
add_library(domain INTERFACE)
target_include_directories(domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Application
add_library(app
    app/CalculatorController.cpp
)
target_link_libraries(app PRIVATE domain)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# UI
add_library(ui
    ui/QtCalculatorUI.cpp
    ui/QtCalculatorUI.h
)
target_link_libraries(ui PRIVATE Qt6::Widgets domain)
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Executable
add_executable(QtCalculator
    main.cpp
)

target_link_libraries(QtCalculator
    PRIVATE Qt6::Widgets app ui
)
