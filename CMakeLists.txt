project (dockbartest)
cmake_minimum_required(VERSION 2.8)

#C++11
list( APPEND CMAKE_CXX_FLAGS "-std=c++11" )

#KDE4 libraries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES})

#Sources
set(dockbar_testytest src/MainApplet.cpp src/TaskGroup.cpp)
kde4_add_plugin(plasma_applet_dockbartest ${dockbar_testytest})

#Linking
target_link_libraries(plasma_applet_dockbartest ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} taskmanager)

#Installing
install(TARGETS plasma_applet_dockbartest DESTINATION /usr/lib/kde4/)
#/usr/lib/kde4/plasma_engine_dict.so
#${PLUGIN_INSTALL_DIR}
install(FILES plasma_applet_dockbartest.desktop DESTINATION ${SERVICES_INSTALL_DIR})