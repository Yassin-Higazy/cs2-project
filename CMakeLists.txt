cmake_minimum_required(VERSION 3.16)
project(LostExpedition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(LostExpedition MANUAL_FINALIZATION
    main.cpp
    resources.qrc
)
target_link_libraries(LostExpedition PRIVATE Qt6::Widgets)
set_target_properties(LostExpedition PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
qt_finalize_executable(LostExpedition)
