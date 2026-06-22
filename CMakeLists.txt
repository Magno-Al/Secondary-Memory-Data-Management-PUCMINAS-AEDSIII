cmake_minimum_required(VERSION 3.16)

project(SistemaMatricula VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# A interface grafica e header-only sob src/views/gui/ e e incluida por main.cpp
# (descomente o bloco GUI em main.cpp para compilar a GUI em vez do terminal).
add_executable(SistemaMatricula
    src/main.cpp
    src/views/gui/GuiApp.h
    src/views/gui/LoginWindow.h
    src/views/gui/ManagerWindow.h
    src/views/gui/StudentWindow.h
    src/views/gui/ProfessorWindow.h
    src/views/gui/Daos.h
    src/views/gui/resources.qrc
)

target_link_libraries(SistemaMatricula PRIVATE Qt6::Widgets)
