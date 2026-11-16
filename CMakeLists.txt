cmake_minimum_required(VERSION 3.22)
project(Phobos67 VERSION 0.2.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(PHOBOS67_JUCE_PATH "" CACHE PATH "JUCE source directory")
if(NOT TARGET juce::juce_dsp)
 add_subdirectory("${PHOBOS67_JUCE_PATH}" JUCE)
endif()
juce_add_plugin(Phobos67 COMPANY_NAME "Papielguapo" BUNDLE_ID "com.papielguapo.phobos67"
 PLUGIN_MANUFACTURER_CODE Papi PLUGIN_CODE Ph67 FORMATS VST3 Standalone
 PRODUCT_NAME "PAPIELGUAPO - D2F" IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
 COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(Phobos67 PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(Phobos67 PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Phobos67 PRIVATE juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags)
option(PHOBOS67_BUILD_VALIDATOR "Build audio validation and preview tool" ON)
if(PHOBOS67_BUILD_VALIDATOR)
 juce_add_console_app(Phobos67Check PRODUCT_NAME Phobos67Check)
 target_sources(Phobos67Check PRIVATE Source/Check.cpp Source/AllocationMonitor.cpp)
 target_include_directories(Phobos67Check PRIVATE "${PHOBOS67_JUCE_PATH}/modules")
 target_compile_definitions(Phobos67Check PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 JUCE_MODAL_LOOPS_PERMITTED=1 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
 target_link_libraries(Phobos67Check PRIVATE Phobos67 juce::juce_recommended_config_flags)
endif()


