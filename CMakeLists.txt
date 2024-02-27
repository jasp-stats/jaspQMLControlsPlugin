cmake_minimum_required(VERSION 3.16)
project(jaspQMLComponents
    VERSION 0.19.0.0
    LANGUAGES CXX)


if(CMAKE_OSX_ARCHITECTURES STREQUAL "")
  set(CMAKE_OSX_ARCHITECTURES
      ${CMAKE_HOST_SYSTEM_PROCESSOR}
      CACHE STRING "" FORCE)
  set(CROSS_COMPILING 0)
  message(STATUS "Building for ${CMAKE_OSX_ARCHITECTURES} architecture")
else()
  if(NOT (CMAKE_OSX_ARCHITECTURES STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR))
    set(CROSS_COMPILING 1)
    message(
      STATUS "Cross compiling for ${CMAKE_OSX_ARCHITECTURES} architecture")
  endif()
endif()

if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      11.0
      CACHE STRING "Minimum macOS version the build will be able to run on"
            FORCE)
else()
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      10.15
      CACHE STRING "Minimum macOS version the build will be able to run on"
            FORCE)
endif()

option(BUILDING_JASP "Indicates whether we are building JASP or not.
                                              This helps jaspResults to find its lib_json." ON)
if(BUILDING_JASP)
   add_definitions(-DBUILDING_JASP)
endif()

add_definitions(-DNO_JASPDESKTOP)

set(CMAKE_MESSAGE_CONTEXT_SHOW ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_FIND_FRAMEWORK ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(FetchContent)
include(CMakePrintHelpers)

if(NOT DEFINED GIT_CURRENT_BRANCH)
    find_package(Git)

    if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")

      message(CHECK_START "Retrieving the git-branch information")

      execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

      execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

      message(CHECK_PASS "done.")

      set(GIT_CURRENT_BRANCH ${GIT_BRANCH})
      set(GIT_CURRENT_COMMIT ${GIT_COMMIT})

      cmake_print_variables(GIT_CURRENT_BRANCH)
      cmake_print_variables(GIT_CURRENT_COMMIT)
    endif()
endif()

find_package(
   Qt6 REQUIRED
   COMPONENTS
               Core
               Gui
               OpenGL
               Widgets
               Qml
               Quick
               QuickLayouts
               QuickControls2
               QuickControls2Impl
               QuickWidgets
               Core5Compat
)

qt6_policy(SET QTP0001 NEW)

add_subdirectory(jaspCommonLib)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/sources/*.h")

file(GLOB QML_CONTROLS RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "components/JASP/Controls/*.qml")
file(GLOB QML_THEME RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "components/JASP/Theme/*.qml")

qt6_add_qml_module(jaspcontrolsplugin
    URI JASP.Controls
    OUTPUT_DIRECTORY "JASP/Controls"
    PLUGIN_TARGET jaspcontrolsplugin
    DEPENDENCIES QtQuick
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    CLASS_NAME JASPQmlPlugin
    SOURCES ${SOURCE_FILES} ${HEADER_FILES}
    QML_FILES ${QML_CONTROLS}
)

target_compile_definitions(jaspcontrolsplugin PUBLIC JASP_USES_QT_HERE)

target_include_directories(
  jaspcontrolsplugin
  PUBLIC # JASP
  ${PROJECT_SOURCE_DIR}/jaspCommonLib
  ${PROJECT_SOURCE_DIR}/jaspCommonLib/QtUtils
  ${PROJECT_SOURCE_DIR}/sources
  ${PROJECT_SOURCE_DIR}/sources/controls
  ${PROJECT_SOURCE_DIR}/sources/rsyntax
)


target_link_libraries(jaspcontrolsplugin PRIVATE
    CommonQt
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::QuickControls2Impl
    Qt::Widgets
)


