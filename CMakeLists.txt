cmake_minimum_required(VERSION 3.6)
project(lscaling)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


include_directories(inc)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES} ${X11_Xrandr_LIB})
include_directories(${X11_INCLUDE_DIR})



set(SOURCE_FILES src/main.cpp src/desktop_resizer.cpp src/argument_parser.cpp src/screen_resource.cpp )
add_executable(lscaling ${SOURCE_FILES})