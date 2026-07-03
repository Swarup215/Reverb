cmake_minimum_required(VERSION 3.22)

project(ReverseFlow VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
if(NOT EXISTS "${JUCE_ROOT}/CMakeLists.txt")
    set(JUCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
endif()

if(NOT EXISTS "${JUCE_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found. Clone JUCE into ReverseFlow/JUCE or the workspace root JUCE/ directory: git clone https://github.com/juce-framework/JUCE.git")
endif()

add_subdirectory("${JUCE_ROOT}" JUCE)

juce_add_plugin(ReverseFlow
    COMPANY_NAME "ReverseFlowAudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_FORMATS VST3 AU Standalone
    PRODUCT_NAME "ReverseFlow"
    MICROPHONE_PERMISSION_ENABLED TRUE
)

target_sources(ReverseFlow
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/GUI/MainComponent.cpp
)

target_include_directories(ReverseFlow
    PRIVATE
        Source
        Source/DSP
        Source/GUI
        Source/Utils
)

target_link_libraries(ReverseFlow
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
