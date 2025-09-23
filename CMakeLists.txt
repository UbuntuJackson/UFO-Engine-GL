cmake_minimum_required(VERSION 3.16)
project(UFO)

#PGE dependencies
option(HAS_TERMINAL "Show a terminal window for STDOUT/STDERR" ON)
option(USE_ALSA       "Force using ALSA as audio backend (Linux-only)")
option(USE_PULSEAUDIO "Force using PulseAudio as audio backend (Linux-only)")
option(USE_SDL2_MIXER "Force using SDL2_mixer as audio backend")

# Set C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_COMPILER "/usr/bin/g++")
#set(CMAKE_C_COMPILER "/usr/bin/gcc")

if(NOT WIN32)

    # Find required libraries
    find_package(CURL REQUIRED)
    find_package(PNG REQUIRED)
    find_package(ZLIB REQUIRED)

endif()
#PGE Dependencies END

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(SDL)
add_subdirectory(freetype-2.14.1)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(glm-1.0.1)

set(
    SRC
    src/engine.cpp
    src/texture_2d.cpp
    #main.cpp
    src/level.cpp
    src/actor.cpp
    src/camera.cpp
    src/shader.cpp
    src/sprite_renderer.cpp
    src/openglv4_5_asset_manager.cpp
    src/Main.cpp
    src/text_rendering.cpp
    src/control_settings.cpp
    glad/src/glad.c
    utils/file.cpp
    utils/stb_image_implementation.cpp

    ufo_maths/ufo_maths.cpp
    shapes/circle.cpp
    shapes/pill.cpp
    shapes/raw_shape_base.cpp
    shapes/ray2.cpp
    shapes/rectangle.cpp
    external/olcPixelGameEngine.cpp
    random/random_number_generator.cpp
)

# Create your game executable target as usual
add_library(UFO ${SRC})

target_include_directories(UFO PUBLIC src)
target_include_directories(UFO PUBLIC ufo_maths)
target_include_directories(UFO PUBLIC utils)

if(NOT WIN32)

    # Link libraries
    target_link_libraries(UFO PUBLIC
        CURL::libcurl
        PNG::PNG
        ZLIB::ZLIB
    )

endif()

# OpenGL
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
target_include_directories(UFO PUBLIC ${OpenGL_INCLUDE_DIRS})
target_link_libraries(UFO PUBLIC OpenGL::GL)

# X11
if(NOT WIN32)
    find_package(X11 REQUIRED)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 not found. Please install X11 development libraries.")
    endif()
    target_include_directories(UFO PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(UFO PUBLIC X11::X11)
endif()

# Threads
find_package(Threads REQUIRED)
target_link_libraries(UFO PUBLIC Threads::Threads)

# stdc++fs
target_compile_features(UFO PUBLIC cxx_std_17)

# Platform-specific audio backends
if(NOT WIN32)  # Only consider ALSA or PulseAudio on Linux
    if(USE_ALSA)
        
        # ALSA
        find_package(ALSA REQUIRED)
        target_link_libraries(UFO PUBLIC ALSA::ALSA)
        include_directories(${ALSA_INCLUDE_DIRS})
        add_compile_definitions(SOUNDWAVE_USING_ALSA=1)

    else()

        # PulseAudio
        find_package(PulseAudio REQUIRED)
        target_link_libraries(UFO PUBLIC ${PULSEAUDIO_LIBRARY} pulse-simple)
        include_directories(${PULSEAUDIO_INCLUDE_DIR})
        add_compile_definitions(SOUNDWAVE_USING_PULSE=1)

    endif()
endif()

if(USE_SDL2_MIXER AND NOT EMSCRIPTEN)

    # SDL2_mixer
    find_package(SDL2_mixer REQUIRED)
    target_link_libraries(UFO PUBLIC SDL2_mixer::SDL2_mixer)

    add_compile_definitions(SOUNDWAVE_USING_SDLMIXER=1)

endif()

# Link to the actual SDL3 library.
target_link_libraries(UFO PUBLIC SDL3::SDL3)
target_link_libraries(UFO PUBLIC freetype)
target_include_directories(UFO PUBLIC glad/include)