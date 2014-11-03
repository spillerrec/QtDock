TEMPLATE = app
TARGET = QtDock
INCLUDEPATH += .
QT += KWindowSystem
QT += widgets

# C++11 support
QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += src/main.cpp
