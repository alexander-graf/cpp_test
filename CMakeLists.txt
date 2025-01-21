cmake_minimum_required(VERSION 3.16)
project(mein_gui_projekt VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Expliziter Pfad zu Qt
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(mein_gui_projekt WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(mein_gui_projekt PRIVATE Qt6::Widgets) 