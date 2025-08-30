QT += core widgets
CONFIG += c++11

TARGET = TSAScreen
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/diagramwidget.cpp

HEADERS += \
    src/diagramwidget.h

# Ensure we're using Qt 5.14.0
QT_VERSION = 5.14.0

# Qt 5.14 compatibility
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050E00

# Keep output tidy
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic 