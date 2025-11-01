cmake_minimum_required(VERSION 3.16)
project(hbonsai LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# --- notcurses via pkg-config ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(NOTCURSES QUIET IMPORTED_TARGET notcurses)
if (NOT NOTCURSES_FOUND)
  pkg_check_modules(NOTCURSES REQUIRED IMPORTED_TARGET notcurses-core)
endif()


# --- Sources ---
set(BONSAI_SOURCES
  src/main.cpp
  src/bonsai_scene.cpp
  src/bonsai/Bonsai.cpp
  src/config/Config.cpp
  src/renderer/Renderer.cpp
  src/scenemanager.cpp
  src/title/Title.cpp
  src/title_scene.cpp
)

# --- Executable ---
add_executable(hbonsai ${BONSAI_SOURCES})

# --- Include Directories ---
target_include_directories(hbonsai PRIVATE
  include
)

# --- Link Libraries ---
target_link_libraries(hbonsai PRIVATE PkgConfig::NOTCURSES)

# --- Testing ---
# enable_testing()
# add_subdirectory(tests)
