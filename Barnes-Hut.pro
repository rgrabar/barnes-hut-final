QT += qml quick
QT += gui

CONFIG += qmltypes
CONFIG += c++20

QML_IMPORT_NAME = BarnesHut
QML_IMPORT_MAJOR_VERSION = 1

HEADERS += \
    barnesHut.h \
    point.h \
    quadTree.h
SOURCES += main.cpp \
    barnesHut.cpp
RESOURCES += openglunderqml.qrc

win32-g++ {
    LIBS += -lopengl32
}
win32-msvc*{
    LIBS += opengl32.lib
}

target.path = $$[QT_INSTALL_EXAMPLES]/quick/scenegraph/openglunderqml
INSTALLS += target
