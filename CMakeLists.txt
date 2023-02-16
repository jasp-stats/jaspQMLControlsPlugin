# This will build the QMLComponents library
cmake_minimum_required(VERSION 3.21)

if(NOT CMAKE_PROJECT_NAME STREQUAL "JASP")

    if(APPLE)

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

    endif()

    project(
      JASPQMLComponents
      VERSION 0.17.1.0 # <major>[.<minor>[.<patch>[.<tweak>]]]
      LANGUAGES CXX C
      HOMEPAGE_URL "http://jasp-stats.org/"
      DESCRIPTION "A fresh way to do statistics")

    set(CMAKE_MESSAGE_CONTEXT_SHOW ON)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    set(CMAKE_FIND_FRAMEWORK ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE
          "Debug"
          CACHE STRING
                "OBS build type [Release, RelWithDebInfo, Debug, MinSizeRel]" FORCE)
      message(STATUS "Building for ${CMAKE_BUILD_TYPE}")
    endif()

    # ------ CMake Modules

    include(FetchContent)
    include(CMakePrintHelpers)

    option(BUILDING_JASP "Indicates whether we are building JASP or not.
                                              This helps jaspResults to find its lib_json." ON)
    if(BUILDING_JASP)
      add_definitions(-DBUILDING_JASP)
    endif()

    find_package(
      Qt6 REQUIRED
      COMPONENTS Core
                 Gui
                 Qml
                 Quick
                 QuickLayouts
                 QuickControls2
                 QuickControls2Impl
                 QuickWidgets
                 Core5Compat)
endif() # Not inside project JASP

list(APPEND CMAKE_MESSAGE_CONTEXT QMLComponents)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(QMLComponents ${SOURCE_FILES} ${HEADER_FILES})

if(LINUX)

  target_link_libraries(QMLComponents PUBLIC ${_LIB_RT})
  target_link_options(QMLComponents PUBLIC -lrt)

endif()


configure_file(${CMAKE_CURRENT_LIST_DIR}/utilities/appdirs.h.in
               ${CMAKE_CURRENT_LIST_DIR}/utilities//appdirs.h)
message(STATUS "appdirs.h is successfully generated...")

target_include_directories(
  QMLComponents
  PUBLIC # JASP
  ${PROJECT_SOURCE_DIR}/Common
         )
target_link_libraries(
  QMLComponents
  PUBLIC
         Common
         Qt::Core
         Qt::Gui
         Qt::Widgets
         Qt::Qml
         Qt::Quick
         Qt::QuickLayouts
         Qt::QuickControls2
         Qt::QuickControls2Impl
         Qt::QuickWidgets
         Qt::QuickTemplates2
         $<$<NOT:$<BOOL:${FLATPAK_USED}>>:Qt::Core5Compat>
         $<$<BOOL:${FLATPAK_USED}>:/app/lib/$ENV{FLATPAK_ARCH}-linux-gnu/libQt6Core5Compat.so>
     )

 file(GLOB_RECURSE QML_RESOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/components/*")
 qt_add_resources(
   QMLComponents
   "qmlComponents"
   PREFIX
   "/"
   BASE
   ${CMAKE_CURRENT_LIST_DIR}
   FILES
   ${QML_RESOURCE_FILES})

list(POP_BACK CMAKE_MESSAGE_CONTEXT)
