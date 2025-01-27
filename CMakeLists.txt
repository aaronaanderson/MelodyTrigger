cmake_minimum_required(VERSION 3.22) 

project(MELODYTRIGGER VERSION 0.0.1)

add_subdirectory(JUCE)

juce_add_plugin(MelodyTrigger
    VERSION "0.0.1"                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "Dawesome"                          # Specify the name of the plugin's author
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE              # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE              # Does the plugin need midi output?
    IS_MIDI_EFFECT TRUE                 # Is this plugin a MIDI effect?
    BUNDLE_ID com.AaronAnderson.MelodyTrigger
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Aaro               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE M3tr                        # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    APP_SANDBOX_ENABLED TRUE        # Needed for GarageBand
    FORMATS Standalone VST3 AU     # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Melody Trigger"         # The name of the final executable, which can differ from the target name    
)    

set_target_properties(MelodyTrigger PROPERTIES 
    CXX_STANDARD 17
    COMPILE_WARNING_AS_ERROR ON
)

target_sources(MelodyTrigger
    PRIVATE
        Source/MainEditor.cpp
        Source/MainProcessor.cpp)

target_compile_definitions(MelodyTrigger
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MelodyTrigger
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)