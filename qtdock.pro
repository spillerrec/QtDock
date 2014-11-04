TEMPLATE = app
TARGET = QtDock
INCLUDEPATH += .
QT += KWindowSystem
QT += widgets

# C++11 support
QMAKE_CXXFLAGS += -std=c++11

# Input
HEADERS += src/extraWidgets.hpp src/TaskManager.hpp src/TaskBar.hpp
SOURCES += src/extraWidgets.cpp src/TaskManager.cpp src/TaskBar.cpp src/main.cpp
