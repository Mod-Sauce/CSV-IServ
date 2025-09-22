QT += widgets core

CONFIG += c++17

TARGET = csv_converter
TEMPLATE = app

# Version Information
VERSION = 2.1.0
QMAKE_TARGET_COMPANY = "Moritz Breier"
QMAKE_TARGET_PRODUCT = "CSV-IServ-Converter"
QMAKE_TARGET_DESCRIPTION = "CSV converter for IServ import"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2025 Moritz Breier"

# Platform specific settings
win32 {
    CONFIG += embed_manifest_exe
    CONFIG += windows
    RC_ICONS = csv.ico
    
    # Version resource
    RC_FILE = version.rc
    
    # Windows 10+ compatibility
    DEFINES += WINVER=0x0A00 _WIN32_WINNT=0x0A00
}

unix:!macx {
    # Linux specific settings
    TARGET = csv-iserv-converter
    
    # Desktop integration
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    
    target.path = $$PREFIX/bin
    
    # Desktop file
    desktop.files = csv-iserv-converter.desktop
    desktop.path = $$PREFIX/share/applications
    
    # Icon
    icon.files = csv.png
    icon.path = $$PREFIX/share/pixmaps
    
    INSTALLS += target desktop icon
    
    # Linux-specific compiler flags
    QMAKE_CXXFLAGS += -std=c++17
    
    # Add rpath for Qt libraries if not system-wide installed
    QMAKE_RPATHDIR += $$[QT_INSTALL_LIBS]
}

macx {
    # macOS specific settings
    TARGET = "CSV IServ Converter"
    ICON = csv.icns
    
    # macOS app bundle settings
    QMAKE_INFO_PLIST = Info.plist
    
    # macOS specific compiler flags
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
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

# Compiler flags for GCC/Clang on Unix systems
unix {
    QMAKE_CXXFLAGS += -Wall -Wextra
    QMAKE_CXXFLAGS_RELEASE += -O2
    QMAKE_CXXFLAGS_DEBUG += -g -O0
}

# Release optimizations
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# Debug settings
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    unix:!macx: TARGET = $$join(TARGET,,,d)
    win32: TARGET = $$join(TARGET,,,d)
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

# Clean target
QMAKE_CLEAN += $$DESTDIR/$(TARGET)

# Additional files for distribution
DISTFILES += \
    README.md \
    LICENSE \
    csv-iserv-converter.desktop \
    csv.png \
    csv.ico
