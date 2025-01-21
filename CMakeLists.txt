cmake_minimum_required(VERSION 3.16)
project(cpp_espanso_helper VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug-Ausgaben
message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Current binary dir: ${CMAKE_BINARY_DIR}")

# Expliziter Pfad zu Qt und yaml-cpp
set(CMAKE_PREFIX_PATH 
    "C:/Qt/6.5.3/mingw_64/lib/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern"
)

# Debug-Ausgabe
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(yaml-cpp REQUIRED)

# Debug-Ausgabe
message(STATUS "yaml-cpp found: ${yaml-cpp_FOUND}")
message(STATUS "yaml-cpp version: ${yaml-cpp_VERSION}")
message(STATUS "yaml-cpp libraries: ${YAML_CPP_LIBRARIES}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)  # Verhindert das Öffnen des Konsolenfensters
endif()

add_executable(cpp_espanso_helper
    main.cpp
    mainwindow.cpp
    mainwindow.h
    resources.qrc
)

target_link_libraries(cpp_espanso_helper PRIVATE 
    Qt6::Widgets
    yaml-cpp::yaml-cpp
)

# Installationsregeln
install(TARGETS cpp_espanso_helper DESTINATION bin)
install(FILES 
    $<TARGET_FILE:Qt6::Widgets>
    $<TARGET_FILE:Qt6::Core>
    $<TARGET_FILE:Qt6::Gui>
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/lib/libyaml-cpp.dll"
    DESTINATION bin
) 