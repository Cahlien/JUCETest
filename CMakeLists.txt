cmake_minimum_required(VERSION 3.16)

project(JUCETest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.7 REQUIRED COMPONENTS Quick)

add_subdirectory(JUCE/)

qt_standard_project_setup(REQUIRES 6.7)

qt_add_executable(appJUCETest
    main.cpp
)

qt_add_qml_module(appJUCETest
    URI JUCETest
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES controller.h controller.cpp
        SOURCES audio_player.h audio_player.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

target_include_directories(appJUCETest PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(appJUCETest PRIVATE ${GTK3_LIBRARIES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)

target_include_directories(appJUCETest PRIVATE ${WEBKIT2_INCLUDE_DIRS})
target_link_libraries(appJUCETest PRIVATE ${WEBKIT2_LIBRARIES})

find_package(CURL REQUIRED)
target_link_libraries(appJUCETest PRIVATE CURL::libcurl)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appJUCETest PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appJUCETest
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appJUCETest
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Core
    PRIVATE juce::juce_audio_basics
    PRIVATE juce::juce_audio_devices
    PRIVATE juce::juce_audio_formats
    PRIVATE juce::juce_audio_processors
    PRIVATE juce::juce_audio_utils
    PRIVATE juce::juce_core
)

include(GNUInstallDirs)
install(TARGETS appJUCETest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
