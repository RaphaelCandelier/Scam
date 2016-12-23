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
