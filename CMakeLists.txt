cmake_minimum_required(VERSION 3.25)
project(SonarMidiPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- RtMidi ---
set(RTMIDI_INCLUDE_DIR "D:/C++/SONAR MIDI PLAYER/vcpkg/installed/x64-windows/include/rtmidi")
set(RTMIDI_LIBRARY "D:/C++/SONAR MIDI PLAYER/vcpkg/installed/x64-windows/lib/rtmidi.lib")

if(NOT EXISTS ${RTMIDI_INCLUDE_DIR})
    message(FATAL_ERROR "RtMidi include directory neexistuje: ${RTMIDI_INCLUDE_DIR}")
endif()
if(NOT EXISTS ${RTMIDI_LIBRARY})
    message(FATAL_ERROR "RtMidi lib neexistuje: ${RTMIDI_LIBRARY}")
endif()

# --- FluidSynth ---
set(FLUIDSYNTH_DIR "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows")
set(FLUIDSYNTH_INCLUDE_DIR "${FLUIDSYNTH_DIR}/include")
set(FLUIDSYNTH_LIB_DIR     "${FLUIDSYNTH_DIR}/lib")

file(GLOB FLUIDSYNTH_LIBS "${FLUIDSYNTH_LIB_DIR}/libfluidsynth*.lib")

set(FLUIDSYNTH_LIB_DEBUG "")
set(FLUIDSYNTH_LIB_RELEASE "")

foreach(lib ${FLUIDSYNTH_LIBS})
    if(lib MATCHES "-d\\.lib$")
        set(FLUIDSYNTH_LIB_DEBUG ${lib})
    else()
        set(FLUIDSYNTH_LIB_RELEASE ${lib})
    endif()
endforeach()

if(FLUIDSYNTH_LIB_DEBUG STREQUAL "")
    set(FLUIDSYNTH_LIB_DEBUG ${FLUIDSYNTH_LIB_RELEASE})
endif()

# --- SDL3 ---
set(SDL3_DIR "D:/C++/Libraries/SDL3-3.4.2")
set(SDL3_INCLUDE_DIR "${SDL3_DIR}/include")
set(SDL3_LIBRARY "${SDL3_DIR}/lib/x64/SDL3.lib")

# --- ImGui ---
set(IMGUI_DIR "D:/C++/Libraries/imgui-master")
set(IMGUI_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- Executable ---
add_executable(${PROJECT_NAME}
    main.cpp
    gui/GuiController.cpp
    gui/GuiWorker.cpp
    ${IMGUI_FILES}
)

# --- Includes ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${RTMIDI_INCLUDE_DIR}
    ${FLUIDSYNTH_INCLUDE_DIR}
    ${SDL3_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# --- OpenGL (FIX!) ---
find_package(OpenGL REQUIRED)

# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RTMIDI_LIBRARY}

    $<$<CONFIG:Debug>:${FLUIDSYNTH_LIB_DEBUG}>
    $<$<CONFIG:Release>:${FLUIDSYNTH_LIB_RELEASE}>

    ${SDL3_LIBRARY}

    OpenGL::GL     # ✅ hlavní fix
    opengl32       # ✅ jistota na Windows
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)