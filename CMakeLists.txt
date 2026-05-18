cmake_minimum_required(VERSION 3.16)

project(LinkyApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp

    src/ui/widget.cpp
    src/ui/widget.h
    src/ui/widget.ui

    src/ui/MenuManager.cpp
    src/ui/MenuManager.h

    src/ui/InputHandler.cpp
    src/ui/InputHandler.h

    src/controller/ListController.h

    src/core/LinkedList.h
    src/core/SingleLinkedList.h
    src/core/DoubleLinkedList.h

    resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LinkyApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(LinkyApp ${PROJECT_SOURCES})
endif()

target_include_directories(LinkyApp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(LinkyApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    set(app_icon_rc "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico" native_icon_path)
    string(REPLACE "\\" "\\\\" escaped_icon_path "${native_icon_path}")
    file(WRITE "${app_icon_rc}" "IDI_ICON1 ICON DISCARDABLE \"${escaped_icon_path}\"\n")
    target_sources(LinkyApp PRIVATE "${app_icon_rc}")
endif()

set_target_properties(LinkyApp PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

install(TARGETS LinkyApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LinkyApp)
endif()