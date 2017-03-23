#-------------------------------------------------
#
# Project created by QtCreator 2016-04-28T19:27:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qavrdude
TEMPLATE = app

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

win32 : {
RESOURCES += \
    binaries_win32.qrc
}
