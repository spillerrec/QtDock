QtDock
======

A simple Windows7-like taskbar/dock for Linux using Qt5 and KDE Frameworks 5.

## Major shortcomings

- Can only be possitioned at the left side of the desktop, support all sides
- Mising System tray, spec: http://standards.freedesktop.org/systemtray-spec/systemtray-spec-latest.html
- System tray applications, like battery monitor? How do other do it, can wee plug into an existing framework, etc.?
- Launcher?

## Dependencies

- Qt5
- KWindowSystem
- KGlobalAccel

## Compiling

- qmake
- make