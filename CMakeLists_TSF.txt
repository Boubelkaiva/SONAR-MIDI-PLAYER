# ==============================================================================
#
#   FILE: CMakeLists.txt
#   PROJECT: SONAR MIDI PLAYER
#   DESCRIPTION: Central build configuration.
#                UPDATED: Added FxModal + BankDNAExtractor.cpp (v1.2.0).
#                FIXED: Correct CMake comment syntax.
#   AUTHOR: Iva
#
# ==============================================================================

cmake_minimum_required(VERSION 3.25)

project(SonarMidiPlayer VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- UTF-8 source encoding ---
if(MSVC)
    add_compile_options(/utf-8 /wd4828)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# --- JUCE ---
add_subdirectory(juce)

# --- App Definition ---
juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "SonarMidiPlayer"
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Iva"
    BUNDLE_ID "com.iva.sonarmidiplayer"
    IS_CONTENT_PREVIEW_HELPER NO
)

# --- Source files ---
target_sources(${PROJECT_NAME} PRIVATE
    Main.cpp

    # --- Backend ---
    backend/MidiPlayer/MidiPlayer.cpp
    backend/MidiPlayer/MidiPlayer.h
    backend/BankManager/BankManager.cpp
    backend/BankManager/BankManager.h
    backend/MidiMapper/MidiMapper.cpp
    backend/MidiMapper/MidiMapper.h
    backend/MidiPlayer/tsf_ext.cpp
    backend/BankDNAExtractor/BankDNAExtractor.cpp
    backend/BankDNAExtractor/BankDNAExtractor.h
    backend/MidiAnalyzer/MidiAnalyzer.cpp
    backend/MidiAnalyzer/MidiAnalyzer.h

    # --- Frontend ---
    frontend/MainComponent/MainComponent.cpp
    frontend/MainComponent/MainComponent.h
    frontend/MasterPanel/MasterPanel.cpp
    frontend/MasterPanel/MasterPanel.h
    frontend/SF2ListComponent/SF2ListComponent.cpp
    frontend/SF2ListComponent/SF2ListComponent.h
    frontend/TrackPanelComponent/TrackPanelComponent.cpp
    frontend/TrackPanelComponent/TrackPanelComponent.h
    frontend/TransportComponent/TransportComponent.cpp
    frontend/TransportComponent/TransportComponent.h
    frontend/TrackComponent/TrackComponent.cpp
    frontend/TrackComponent/TrackComponent.h

    # ✅ FX MODAL (DŮLEŽITÉ – FIX LNK2019)
    frontend/FxModal/FxModal.cpp
    frontend/FxModal/FxModal.h
)

juce_generate_juce_header(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_audio_basics
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
)

target_include_directories(${PROJECT_NAME} PRIVATE
    .
    frontend
    backend
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)