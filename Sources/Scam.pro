#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T09:09:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

TARGET = Scam
TEMPLATE = app

SOURCES += main.cpp\
    WindowMain.cpp \
    MsgHandler.cpp \
    Cameras_FLIR.cpp \
    WindowImage.cpp \
    qcustomplot.cpp

HEADERS  += \
    WidgetOutput.h \
    WindowMain.h \
    MsgHandler.h \
    Cameras_FLIR.h \
    WindowImage.h \
    qcustomplot.h

FORMS    += mainwindow.ui

# === Platform-specific libraries ==========================================

# --- LINUX
unix:!macx: LIBS += -L/usr/local/Spinnaker/lib -lSpinnaker
unix:!macx: LIBS += -Wl,-rpath-link=/usr/local/Spinnaker/lib
unix:!macx: INCLUDEPATH += /usr/local/Spinnaker/include
unix:!macx: DEPENDPATH += /usr/local/Spinnaker/lib

# --- WINDOWS

win32: LIBS += -L'C:/Program Files/Point Grey Research/Spinnaker/lib/vs2015/' -lSpinnaker_v140
win32: LIBS += -Wl,-rpath-link='C:/Program Files/Point Grey Research/Spinnaker/lib/vs2015/'
win32: INCLUDEPATH += 'C:/Program Files/Point Grey Research/Spinnaker/lib/include'
win32: DEPENDPATH += 'C:/Program Files/Point Grey Research/Spinnaker/lib/vs2015'
