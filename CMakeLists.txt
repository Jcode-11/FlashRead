cmake_minimum_required(VERSION 3.24)

project(FlashRead
    VERSION 0.1.0
    DESCRIPTION "A fast native Markdown viewer"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick QuickDialogs2)

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

qt_add_executable(FlashRead
    src/DocumentController.cpp
    src/DocumentController.h
    src/MarkdownRenderer.cpp
    src/MarkdownRenderer.h
    src/WorkspaceController.cpp
    src/WorkspaceController.h
    src/main.cpp
)

qt_add_qml_module(FlashRead
    URI FlashRead
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(FlashRead PRIVATE Qt6::Quick Qt6::QuickDialogs2 md4c-html)

set_target_properties(FlashRead PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FlashRead
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
