cmake_minimum_required(VERSION 3.16)

project(AdvancedTextEditor
    VERSION 1.0.0
    DESCRIPTION "Advanced Desktop Text Editor (Notepad++-style) built with Qt 6 and C++17"
    LANGUAGES CXX)

# ----------------------------------------------------------------------------
# Global settings
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt meta-object / UI / resource compilers are run automatically.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Gui Core)

qt_standard_project_setup()

# ----------------------------------------------------------------------------
# Core library (everything except main() so that tests can link against it)
# ----------------------------------------------------------------------------
set(ATE_SOURCES
    src/MainWindow.cpp
    src/core/DocumentManager.cpp
    src/core/FileManager.cpp
    src/core/SettingsManager.cpp
    src/core/ThemeManager.cpp
    src/core/PdfExportManager.cpp
    src/core/TableManager.cpp
    src/core/ImageManager.cpp
    src/core/DocumentStats.cpp
    src/editor/EditorWidget.cpp
    src/dialogs/FindReplaceDialog.cpp
)

set(ATE_HEADERS
    src/MainWindow.h
    src/core/DocumentManager.h
    src/core/FileManager.h
    src/core/SettingsManager.h
    src/core/ThemeManager.h
    src/core/PdfExportManager.h
    src/core/TableManager.h
    src/core/ImageManager.h
    src/core/DocumentStats.h
    src/editor/EditorWidget.h
    src/editor/LineNumberArea.h
    src/dialogs/FindReplaceDialog.h
)

add_library(ate_core STATIC ${ATE_SOURCES} ${ATE_HEADERS})

target_include_directories(ate_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(ate_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport)

# ----------------------------------------------------------------------------
# Application executable
# ----------------------------------------------------------------------------
qt_add_executable(AdvancedTextEditor
    src/main.cpp)

target_link_libraries(AdvancedTextEditor PRIVATE ate_core)

set_target_properties(AdvancedTextEditor PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON)

install(TARGETS AdvancedTextEditor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin)

# ----------------------------------------------------------------------------
# Unit tests (enable with -DBUILD_TESTING=ON, which is the CMake default)
# ----------------------------------------------------------------------------
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()


