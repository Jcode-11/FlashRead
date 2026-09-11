cmake_minimum_required(VERSION 3.24)

project(FlashRead
    VERSION 0.1.0
    DESCRIPTION "A fast native Markdown viewer"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick QuickDialogs2 Svg)

include(FetchContent)
set(BUILD_MD2HTML_EXECUTABLE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(md4c
    GIT_REPOSITORY https://github.com/mity/md4c.git
    GIT_TAG 472c417005c2c71b8617de4f7b8d6b30411d78f4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(md4c)

qt_standard_project_setup(REQUIRES 6.6)
qt_policy(SET QTP0004 NEW)

set(DIAGRAM_SOURCES
    src/diagram/DiagramTypes.h
    src/diagram/FlowchartParser.h
    src/diagram/FlowchartParser.cpp
    src/diagram/FlowchartLayout.h
    src/diagram/FlowchartLayout.cpp
    src/diagram/SequenceDiagramParser.h
    src/diagram/SequenceDiagramParser.cpp
    src/diagram/DiagramRenderer.h
    src/diagram/DiagramRenderer.cpp
    src/diagram/DiagramImageProvider.h
    src/diagram/DiagramImageProvider.cpp
)

set(PLATFORM_SOURCES "")
if(WIN32)
    set(PLATFORM_SOURCES resources/FlashRead.rc)
endif()

qt_add_executable(FlashRead
    src/DocumentController.cpp
    src/DocumentController.h
    src/MarkdownRenderer.cpp
    src/MarkdownRenderer.h
    src/WorkspaceController.cpp
    src/WorkspaceController.h
    src/main.cpp
    ${DIAGRAM_SOURCES}
    ${PLATFORM_SOURCES}
)

qt_add_qml_module(FlashRead
    URI FlashRead
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    RESOURCES
        resources/FlashRead.png
)

target_link_libraries(FlashRead PRIVATE Qt6::Quick Qt6::QuickDialogs2 Qt6::Svg md4c-html)

set_target_properties(FlashRead PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.flashread.FlashRead"
    MACOSX_BUNDLE_BUNDLE_NAME "FlashRead"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
)

if(APPLE)
    set_target_properties(FlashRead PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/packaging/macos/Info.plist.in"
    )
endif()

# Test runner
add_executable(test_diagram
    tests/test_diagram.cpp
    src/MarkdownRenderer.cpp
    src/MarkdownRenderer.h
    ${DIAGRAM_SOURCES}
)
target_include_directories(test_diagram PRIVATE src)
target_link_libraries(test_diagram PRIVATE Qt6::Gui Qt6::Svg Qt6::Quick md4c-html)

# Icon generator tool
add_executable(generate_icon
    tools/generate_icon.cpp
)
target_link_libraries(generate_icon PRIVATE Qt6::Gui Qt6::Svg)

include(GNUInstallDirs)
install(TARGETS FlashRead
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET FlashRead
    OUTPUT_SCRIPT deploy_script
)
install(SCRIPT ${deploy_script})
