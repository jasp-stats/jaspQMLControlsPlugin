cmake_minimum_required(VERSION 3.16)
project(jaspQMLComponents LANGUAGES CXX)

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

set(CMAKE_MESSAGE_CONTEXT_SHOW ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_FRAMEWORK ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(FetchContent)
include(CMakePrintHelpers)

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


#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

list(APPEND HEADER_FILES
    analysisbase.h
    analysisform.h
    jasptheme.h
    knownissues.h
    preferencesmodelbase.h
    sortable.h
    variableinfo.h
    simpleDataSetModel.h
)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

file(GLOB_RECURSE QML_CONTROLS "${CMAKE_CURRENT_LIST_DIR}/components/JASP/Controls//*.qml")
file(GLOB_RECURSE QML_THEME "${CMAKE_CURRENT_LIST_DIR}/components/JASP/Theme/*.qml")

#set_target_properties(QMLComponents PROPERTIES AUTOMOC ON)

#if(LINUX)

#  target_link_libraries(QMLComponents PUBLIC ${_LIB_RT})
#  target_link_options(QMLComponents PUBLIC -lrt)

#endif()


configure_file(${CMAKE_CURRENT_LIST_DIR}/utilities/appdirs.h.in
               ${CMAKE_CURRENT_LIST_DIR}/utilities//appdirs.h)
message(STATUS "appdirs.h is successfully generated...")

#target_include_directories(
#  QMLComponents
#  PUBLIC # JASP
#  ${PROJECT_SOURCE_DIR}/Common
#  ${PROJECT_SOURCE_DIR}/controls
#  ${PROJECT_SOURCE_DIR}/rsyntax
#         )
#target_link_libraries(
#  QMLComponents
#  PUBLIC
#         Common
#         Qt::Core
#         Qt::Gui
#         Qt::Widgets
#         Qt::Qml
#         Qt::Quick
#         Qt::QuickLayouts
#         Qt::QuickControls2
#         Qt::QuickControls2Impl
#         Qt::QuickWidgets
#         Qt::QuickTemplates2
#         $<$<NOT:$<BOOL:${FLATPAK_USED}>>:Qt::Core5Compat>
#         $<$<BOOL:${FLATPAK_USED}>:/app/lib/$ENV{FLATPAK_ARCH}-linux-gnu/libQt6Core5Compat.so>
#     )


list(APPEND MODULE_QML_FILES
    components/JASP/Controls/AddColumnField.qml
    components/JASP/Controls/AssignButton.qml
    components/JASP/Controls/AssignedPairsVariablesList.qml
    components/JASP/Controls/AssignedRepeatedMeasuresCells.qml
    components/JASP/Controls/AssignedVariablesList.qml
    components/JASP/Controls/AvailableVariablesList.qml
    components/JASP/Controls/BasicThreeButtonTableView.qml
    components/JASP/Controls/BayesFactorType.qml
    components/JASP/Controls/Button.qml
    components/JASP/Controls/CheckBox.qml
    components/JASP/Controls/Chi2TestTableView.qml
    components/JASP/Controls/CIField.qml
    components/JASP/Controls/ColumnLayout.qml
    components/JASP/Controls/DropDown.qml
    components/JASP/Controls/ComponentsList.qml
    components/JASP/Controls/ComputedColumnField.qml
    components/JASP/Controls/ContrastsList.qml
    components/JASP/Controls/ControlErrorMessage.qml
    components/JASP/Controls/CrossButton.qml
    components/JASP/Controls/CustomContrastsTableView.qml
    components/JASP/Controls/Divider.qml
    components/JASP/Controls/DoubleField.qml
    components/JASP/Controls/Section.qml
    components/JASP/Controls/FactorLevelList.qml
    components/JASP/Controls/FactorsForm.qml
    components/JASP/Controls/FactorsList.qml
    components/JASP/Controls/FileSelector.qml
    components/JASP/Controls/Form.qml
    components/JASP/Controls/FormulaField.qml
    components/JASP/Controls/GridLayout.qml
    components/JASP/Controls/Group.qml
    components/JASP/Controls/HelpButton.qml
    components/JASP/Controls/InputListView.qml
    components/JASP/Controls/IntegerField.qml
    components/JASP/Controls/JagsTableView.qml
    components/JASP/Controls/JAGSTextArea.qml
    components/JASP/Controls/JASPScrollBar.qml
    components/JASP/Controls/Label.qml
    components/JASP/Controls/MenuButton.qml
    components/JASP/Controls/ModelTermsList.qml
    components/JASP/Controls/PercentField.qml
    components/JASP/Controls/RadioButton.qml
    components/JASP/Controls/RadioButtonGroup.qml
    components/JASP/Controls/RectangularButton.qml
    components/JASP/Controls/RoundedButton.qml
    components/JASP/Controls/RowLayout.qml
    components/JASP/Controls/SetSeed.qml
    components/JASP/Controls/SimpleTableView.qml
    components/JASP/Controls/Slider.qml
    components/JASP/Controls/SortMenuButton.qml
    components/JASP/Controls/SubjectivePriors.qml
    components/JASP/Controls/Switch.qml
    components/JASP/Controls/TableView.qml
    components/JASP/Controls/TabView.qml
    components/JASP/Controls/Text.qml
    components/JASP/Controls/TextArea.qml
    components/JASP/Controls/TextField.qml
    components/JASP/Controls/VariablesForm.qml
    components/JASP/Controls/VariablesList.qml
)


qt_add_qml_module(JASPQMLComponents
    URI JASP.Controls
    OUTPUT_DIRECTORY components/JASP/Controls/
    VERSION 1.0
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    #NO_GENERATE_QMLTYPES
    PLUGIN_TARGET JASPQMLComponents
    CLASS_NAME JASPQmlPlugin
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} plugin.cpp
    QML_FILES ${MODULE_QML_FILES}
)


target_include_directories(
  JASPQMLComponents
  PUBLIC # JASP
  ${PROJECT_SOURCE_DIR}/Common
  ${PROJECT_SOURCE_DIR}/controls
  ${PROJECT_SOURCE_DIR}/rsyntax
)


target_link_libraries(JASPQMLComponents PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::QuickControls2Impl
    Qt::Widgets
)


