cmake_minimum_required(VERSION 3.16)

project(SistemaMatricula VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(SistemaMatricula
    src/main.cpp
    src/views/TeacherWindow.cpp
    src/views/teacherwindow.ui
)

target_link_libraries(SistemaMatricula PRIVATE Qt6::Widgets)