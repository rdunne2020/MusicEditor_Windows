cmake_minimum_required(VERSION 3.5)

project(WindowsMusicEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64")

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_SOURCE_DIR}/lib)

find_library(FLAC++ NAMES FLAC++ PATHS ${CMAKE_SOURCE_DIR}/lib)
find_library(FLAC NAMES FLAC PATHS ${CMAKE_SOURCE_DIR}/lib)
find_library(OGG NAMES ogg)

message(${FLAC++})
message(${FLAC})
message(${OGG})
message($ENV{PATH})

message(${CMAKE_LIBRARY_PATH})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_SOURCE_DIR}/include/)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        editor.cpp
        editor.h
        editor.ui
        flaccontainer.cpp
        flaccontainer.h
        songlistobject.h
        songlistobject.cpp
        songlistobject.ui
        copystatusdialog.h
        copystatusdialog.cpp
        copystatusdialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(WindowsMusicEditor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET WindowsMusicEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(WindowsMusicEditor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(WindowsMusicEditor
            ${PROJECT_SOURCES}
        )
    endif()
endif()                                         

target_include_directories(WindowsMusicEditor PUBLIC 
    #${CMAKE_SOURCE_DIR}/include/FLAC++ 
    #${CMAKE_SOURCE_DIR}/include/FLAC
    #${CMAKE_SOURCE_DIR}/include/ogg
    ${CMAKE_SOURCE_DIR}/include/
)
target_link_libraries(WindowsMusicEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${FLAC++} ${FLAC} ${OGG})

set_target_properties(WindowsMusicEditor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS WindowsMusicEditor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(WindowsMusicEditor)
endif()
