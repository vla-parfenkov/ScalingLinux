cmake_minimum_required(VERSION 3.6)
project(scalingUnixSystem)

set(CMAKE_CXX_STANDARD 11)


include_directories(inc)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES} ${X11_Xrandr_LIB})
include_directories(${X11_INCLUDE_DIR})



set(SOURCE_FILES src/main.cpp src/desktop_resizer.cpp src/argument_parser.cpp)
add_executable(scalingUnixSystem ${SOURCE_FILES})