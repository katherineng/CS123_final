#
# CS123 Final Project Starter Code
# Adapted from starter code graciously provided by CS195-U: 3D Game Engines
#

QT += core gui opengl

TARGET = final
TEMPLATE = app

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g.
# INCLUDEPATH += folder1 folder2
# DEPENDPATH += folder1 folder2

SOURCES += main.cpp \
    mainwindow.cpp \
    view.cpp \
    camera.cpp \
    asteroid.cpp \
    CS123Matrix.inl \
    CS123Vector.inl

HEADERS += mainwindow.h \
    view.h \
    camera.h \
    asteroid.h \
    CS123Algebra.h \
    CS123Common.h

FORMS += mainwindow.ui

OTHER_FILES += \
    textures/posz.jpg \
    textures/posx.jpg \
    textures/negz.jpg \
    textures/negx.jpg \
    textures/asteroid.jpg \
    textures/permtexture.png \
    shaders/displacement.vert \
    shaders/displacement.frag
