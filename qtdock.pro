TEMPLATE = app
TARGET = QtDock
INCLUDEPATH += .
QT += KWindowSystem
QT += widgets

# C++11 support
QMAKE_CXXFLAGS += -std=c++11

# Input
HEADERS += src/extraWidgets.hpp
SOURCES += src/extraWidgets.cpp src/main.cpp
