TEMPLATE = app
TARGET = QtDock
INCLUDEPATH += .
QT += KWindowSystem
QT += widgets

# C++11 support
QMAKE_CXXFLAGS += -std=c++14

# Input
HEADERS += src/extraWidgets.hpp src/TaskManager.hpp
SOURCES += src/extraWidgets.cpp src/TaskManager.cpp src/main.cpp
