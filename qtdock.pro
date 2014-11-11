TEMPLATE = app
TARGET = QtDock
INCLUDEPATH += .
QT += KWindowSystem KGlobalAccel
QT += widgets

LIBS += -lxcb
QT += x11extras

# C++11 support
QMAKE_CXXFLAGS += -std=c++11

# Input
HEADERS += src/extraWidgets.hpp src/TaskManager.hpp src/TaskBar.hpp src/WindowList.hpp
SOURCES += src/extraWidgets.cpp src/TaskManager.cpp src/TaskBar.cpp src/WindowList.cpp src/main.cpp

# Move build files
UI_DIR      = .build/ui
OBJECTS_DIR = .build/obj
MOC_DIR     = .build/moc
RCC_DIR     = .build/qrc