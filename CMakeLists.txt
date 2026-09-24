cmake_minimum_required(VERSION 3.20)
project(TestC C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)

pkg_check_modules(ALLEGRO REQUIRED allegro-5 allegro_main-5)
pkg_check_modules(ALLEGRO REQUIRED allegro-5 allegro_main-5 allegro_primitives-5)

include_directories(${ALLEGRO_INCLUDE_DIRS})

add_executable(Test teste04.c)

target_link_libraries(Test PRIVATE ${ALLEGRO_LIBRARIES})