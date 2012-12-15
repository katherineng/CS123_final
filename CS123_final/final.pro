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
    CS123Vector.inl \
    selectionrecorder.cpp \
    particle/particleemitter.cpp

HEADERS += mainwindow.h \
    view.h \
    camera.h \
    asteroid.h \
    CS123Algebra.h \
    CS123Common.h \
    selectionrecorder.h \
    particle/particleemitter.h

FORMS += mainwindow.ui
INCLUDEPATH += particle
DEPENDPATH += particle
OTHER_FILES += \
    textures/posz.jpg \
    textures/posx.jpg \
    textures/negz.jpg \
    textures/negx.jpg \
    textures/asteroid.jpg \
    textures/asteroid1.jpg \
    textures/smoke.png \
    shaders/displacement.vert \
    shaders/displacement.frag \
    shaders/particles.vert \
    shaders/displacement.frag
