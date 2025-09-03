cmake_minimum_required(VERSION 3.10)

project(Alter)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=native")

# Add executable
add_executable(alter
  src/main.cpp
  src/terminal.cpp
  src/tui_editor.cpp
  src/file.cpp
  src/frame.cpp
)
