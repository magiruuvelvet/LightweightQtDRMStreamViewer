cmake_minimum_required(VERSION 3.8)

project(LightweightQtDRMStreamViewer)

# Project configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)
# Use project folders in IDE's
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Meta and Helper Modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
    "${CMAKE_SOURCE_DIR}/CMake/Modules")

include(SetCppStandard)

# Folder where generated files go
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
include_directories(${GENERATED_DIR})

# Handle configuration types
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the CMake configuration type to use")
message(STATUS "Using configuration type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Enable (strict) C++14 features
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release mode flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -rpath -fPIC")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -rpath -fPIC")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-command-line-argument")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-unused-command-line-argument")

# Debug mode flags
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Building with debug macros.")
    add_definitions(-DDEBUG_BUILD)
endif()

# Generic flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-switch")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-switch")

# Install Configuration
include(GNUInstallDirs)

#######################################################################################################################
# Target
#######################################################################################################################

# Set target destination for built targets
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Finding Qt...")
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineCore REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

# Qt deprecated warnings
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# Qt automoc
message(STATUS "Enabling Qt automoc...")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Handle Qt qrc files automatically
message(STATUS "Enabling Qt autorcc...")
set(CMAKE_AUTORCC ON)

qt5_add_resources(RCC_SOURCES "${CMAKE_SOURCE_DIR}/Resources/Resources.qrc")
add_custom_target(GenerateEmbeddedAssets DEPENDS ${RCC_SOURCES})

file(GLOB_RECURSE SourceList
    "${CMAKE_SOURCE_DIR}/Core/*.cpp"
    "${CMAKE_SOURCE_DIR}/Core/*.hpp"

    "${CMAKE_SOURCE_DIR}/Gui/*.cpp"
    "${CMAKE_SOURCE_DIR}/Gui/*.hpp"

    "${CMAKE_SOURCE_DIR}/Util/*.cpp"
    "${CMAKE_SOURCE_DIR}/Util/*.hpp"

    "${CMAKE_SOURCE_DIR}/Widgets/*.cpp"
    "${CMAKE_SOURCE_DIR}/Widgets/*.hpp"
)
add_library("AppLib" STATIC ${SourceList})
SetCppStandard("AppLib" 14)
target_link_libraries("AppLib"
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::WebEngine
    Qt5::WebEngineWidgets
)

add_dependencies("AppLib" GenerateEmbeddedAssets)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} AppLib)

#######################################################################################################################
# Install rules
#######################################################################################################################

install(FILES "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    RENAME lightweightqtdrmstreamviewer)

# NOTICE: sample-config is only for demonstration, don't add to installed files!
