#-------------------------------------------------
#
# Project created by QtCreator 2017-08-22T11:32:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT       += multimedia multimediawidgets

TARGET = mvc_demo
TEMPLATE = app


SOURCES += main.cpp\
        mvccamera.cpp \
    aboutdlg.cpp \
    trigsettingsdlg.cpp \
    autoexposure.cpp \
    autowhitebalance.cpp \
    workthread.cpp

HEADERS  += mvccamera.h \
    aboutdlg.h \
    trigsettingsdlg.h \
    autoexposure.h \
    autowhitebalance.h \
    workthread.h

FORMS    += mvccamera.ui \
    aboutdlg.ui \
    trigsettingsdlg.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lMVCAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lMVCAPI

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    mvcres.qrc
