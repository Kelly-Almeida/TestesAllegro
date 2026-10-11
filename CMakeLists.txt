cmake_minimum_required(VERSION 3.20)
project(TestC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(Test teste05.c
        socket01.h)
        # socket01.h)


# -- Cross Platform Library Finding --
# 1. Try finding Allegro the native vcpkg way
if(WIN32)
    # Windows / Visual Studio path
    find_package(Allegro CONFIG REQUIRED)
    target_link_libraries(Test PRIVATE 
        Allegro::allegro 
        Allegro::allegro_main
        Allegro::allegro_primitives
        Allegro::allegro_image
        Allegro::allegro_font
        Allegro::allegro_ttf
        ws2_32
    )
else()
    # 2. Fall back to Linux pkg-config path
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALLEGRO REQUIRED IMPORTED_TARGET 
        allegro-5 
        allegro_main-5 
        allegro_primitives-5
        allegro_image-5
        allegro_ttf-5
    )
    target_link_libraries(Test PRIVATE PkgConfig::ALLEGRO)
endif()