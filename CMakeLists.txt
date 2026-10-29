cmake_minimum_required(VERSION 3.24)
project(TestC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall)
endif()

add_executable(Test
        src/main.c
  #--      src/sock.c
        src/graficos.c
        src/infra.c)

if(NOT WIN32)
  target_sources(Test PRIVATE src/sock.c)
endif()


add_custom_command(TARGET Test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/resources
          $<TARGET_FILE_DIR:Test>/resources)

# -- Cross Platform Library Finding --
if(WIN32)
  # Caminho do pacote NuGet. Vazio = tenta o vcpkg.
  set(ALLEGRO_NUGET_DIR "" CACHE PATH "Pasta do pacote NuGet do Allegro (ex: .../packages/Allegro.5.2.11.3)")
  set(ALLEGRO_MODULOS allegro allegro_main allegro_primitives allegro_image allegro_font allegro_ttf)

  if(ALLEGRO_NUGET_DIR)
    set(_nativo "${ALLEGRO_NUGET_DIR}/build/native")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(_arq x64)
    else()
      set(_arq win32)
    endif()
    # MSVC_TOOLSET_VERSION = 143 (VS 2022) ou 145 (VS 2026)
    set(_dir "${_nativo}/v${MSVC_TOOLSET_VERSION}/${_arq}")

    foreach(m IN LISTS ALLEGRO_MODULOS)
      add_library(Allegro::${m} SHARED IMPORTED)
      set_target_properties(Allegro::${m} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_nativo}/include"
        IMPORTED_CONFIGURATIONS       "DEBUG;RELEASE"
        IMPORTED_IMPLIB_RELEASE       "${_dir}/lib/${m}.lib"
        IMPORTED_LOCATION_RELEASE     "${_dir}/bin/${m}-5.2.dll"
        IMPORTED_IMPLIB_DEBUG         "${_dir}/lib/${m}-debug.lib"
        IMPORTED_LOCATION_DEBUG       "${_dir}/bin/${m}-debug-5.2.dll"
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE
        MAP_IMPORTED_CONFIG_MINSIZEREL     RELEASE)
    endforeach()
  else()
    find_package(Allegro CONFIG REQUIRED)
  endif()

  target_link_libraries(Test PRIVATE
        Allegro::allegro
        Allegro::allegro_main
        Allegro::allegro_primitives
        Allegro::allegro_image
        Allegro::allegro_font
        Allegro::allegro_ttf
        # ws2_32   # volta quando o socket for portado
  )

  # Copia as DLLs para junto do .exe (senão: "allegro-5.2.dll não encontrado")
  add_custom_command(TARGET Test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:Test> $<TARGET_FILE_DIR:Test>
    COMMAND_EXPAND_LISTS)
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


