cmake_minimum_required(VERSION 3.10)
project(OUT)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_COMPILER "/usr/bin/g++")
#set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(UFO-Engine) #CMakeLists.txt for engine is in folder called "engine"
set(
    SRC
    main.cpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} UFO)
