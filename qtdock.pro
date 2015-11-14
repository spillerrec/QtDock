TEMPLATE = app
TARGET = QtDock
CONFIG += c++11
QT += KWindowSystem KGlobalAccel
QT += widgets x11extras
LIBS += -lxcb

# Input
HEADERS += src/extraWidgets.hpp src/TaskManager.hpp src/TaskBar.hpp src/WindowList.hpp src/TaskGroup.hpp src/TrayWidget.hpp
SOURCES += src/extraWidgets.cpp src/TaskManager.cpp src/TaskBar.cpp src/WindowList.cpp src/TaskGroup.cpp src/TrayWidget.cpp src/main.cpp

# Move build files
UI_DIR      = .build/ui
OBJECTS_DIR = .build/obj
MOC_DIR     = .build/moc
RCC_DIR     = .build/qrc