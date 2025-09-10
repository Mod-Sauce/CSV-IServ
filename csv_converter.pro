QT += widgets core

CONFIG += c++17
CONFIG += windows

TARGET = csv_converter
TEMPLATE = app

# Version Information
VERSION = 2.1.0
QMAKE_TARGET_COMPANY = "Moritz Breier"
QMAKE_TARGET_PRODUCT = "CSV-IServ-Converter"
QMAKE_TARGET_DESCRIPTION = "CSV converter for IServ import"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2025 Moritz Breier"

# Windows specific settings
win32 {
    CONFIG += embed_manifest_exe
    RC_ICONS = csv.ico
    
    # Version resource
    RC_FILE = version.rc
    
    # Windows 10+ compatibility
    DEFINES += WINVER=0x0A00 _WIN32_WINNT=0x0A00
}

# Compiler flags for MSVC
win32-msvc* {
    QMAKE_CXXFLAGS += /std:c++17
    QMAKE_CXXFLAGS_RELEASE += /O2
}

# Compiler flags for MinGW/GCC
win32-g++* {
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_CXXFLAGS_RELEASE += -O2
}

# Release optimizations
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# Debug settings
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    TARGET = $$join(TARGET,,,d)
}

SOURCES += main.cpp

RESOURCES += resources.qrc

# Output directories
CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
