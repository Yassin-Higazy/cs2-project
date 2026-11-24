cmake_minimum_required(VERSION 3.16)
project(LostExpedition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(LostExpedition MANUAL_FINALIZATION
    main.cpp
    wall.h
    player.h
    clue.h
    cluetracker.h cluetracker.cpp
    cluehud.h cluehud.cpp
    pressureplate.h pressureplate.cpp
    cluepickup.h cluepickup.cpp
    puzzle.h puzzle.cpp
    twinsealspuzzle.h twinsealspuzzle.cpp
    glyphfloorpuzzle.h glyphfloorpuzzle.cpp
    hazard.h hazard.cpp
    spiketrap.h spiketrap.cpp
    vaultgate.h vaultgate.cpp
    resources.qrc
)
target_link_libraries(LostExpedition PRIVATE Qt6::Widgets)
set_target_properties(LostExpedition PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
qt_finalize_executable(LostExpedition)
