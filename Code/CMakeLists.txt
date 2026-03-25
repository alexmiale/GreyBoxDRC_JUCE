cmake_minimum_required(VERSION 3.15)
project(GreyBoxDRC VERSION 0.0.1) 

add_subdirectory(../../../JUCE ./JUCE)
add_subdirectory(../../../RTNeural/RTNeural ./RTNeural)

juce_add_plugin(GreyBoxDRC
	COMPANY_NAME MialeAlex
	# set to false if you want audio input
	IS_SYNTH FALSE
	NEEDS_MIDI_INPUT FALSE
	IS_MIDI_EFFECT FALSE
	NEEDS_MIDI_OUTPUT FALSE
	COPY_PLUGIN_AFTER_BUILD FALSE
	PLUGIN_MANUFACTURER_CODE TRUE 
	PLUGIN_CODE Abc8
	# should change for each plugin
	FORMATS AU VST3
	# should change for each plugin
	PRODUCT_NAME "GreyBoxDRC")

juce_generate_juce_header(GreyBoxDRC)

target_sources(GreyBoxDRC
	PRIVATE 
	Source/PluginEditor.cpp 
	Source/PluginProcessor.cpp)

target_compile_definitions(GreyBoxDRC
	PUBLIC 
		JUCE_ALSA=1
		JUCE_DIRECTSOUND=1
		JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
		JUCE_USE_OGGVORBIS=1
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_VST3_CAN_REPLACE_VST2=0)

target_include_directories(GreyBoxDRC PRIVATE ../RTNeural/RTNeural)

target_link_libraries(GreyBoxDRC
	PRIVATE
		juce::juce_audio_utils
	PUBLIC
		juce::juce_recommended_config_flags
		juce::juce_recommended_lto_flags
		juce::juce_recommended_warning_flags
		RTNeural)