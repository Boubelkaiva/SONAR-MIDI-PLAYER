# ==============================================================================
#    FILE: CMakeLists.txt
#    PROJECT: SONAR MIDI PLAYER
#    VERSION: 2.1.2
# ==============================================================================

cmake_minimum_required(VERSION 3.25)
project(SonarMidiPlayer VERSION 2.1.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8 /wd4828)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

find_package(FluidSynth CONFIG REQUIRED)
add_subdirectory(juce)

juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "SonarMidiPlayer"
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Iva"
    BUNDLE_ID "com.iva.sonarmidiplayer"
    IS_CONTENT_PREVIEW_HELPER NO
)

target_sources(${PROJECT_NAME} PRIVATE
    Main.cpp

    # --- Backend ---
    backend/MidiPlayer/MidiPlayer.cpp
    backend/MidiPlayer/MidiPlayer.h
    backend/BankManager/BankManager.cpp
    backend/BankManager/BankManager.h
    backend/MidiMapper/MidiMapper.cpp
    backend/MidiMapper/MidiMapper.h
    backend/MasterEffects/MasterEffects.cpp
    backend/MasterEffects/MasterEffects.h
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
    
    # TRANSPORT
    frontend/TransportComponent/TransportComponent.cpp
    frontend/TransportComponent/TransportComponent.h
    
    # SETTINGS BAR
    frontend/SettingsBarComponent/SettingsBarComponent.cpp
    frontend/SettingsBarComponent/SettingsBarComponent.h
    
    # MODUL: VU Meter
    frontend/VUMeter/VUMeter.cpp
    frontend/VUMeter/VUMeter.h

    # TRACK COMPONENT
    frontend/TrackComponent/TrackComponent.cpp
    frontend/TrackComponent/TrackComponent.h
    
    # MODUL: Master Channel Strip
    frontend/MasterChannelStrip/MasterChannelStrip.cpp
    frontend/MasterChannelStrip/MasterChannelStrip.h
    
    # MODUL: Version Footer
    frontend/VersionFooter/VersionFooter.h
    
    frontend/FxModal/FxModal.cpp
    frontend/FxModal/FxModal.h
)

juce_generate_juce_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE . frontend backend)

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
    juce::juce_dsp
    FluidSynth::libfluidsynth
)