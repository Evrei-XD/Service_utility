#-------------------------------------------------
#
# Project created by QtCreator 2020-04-28T11:29:47
#
#-------------------------------------------------

QT       += core gui\
        serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = COM_test1
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    logcategories.cpp \
        main.cpp\
        mainwindow.cpp \
    connect.cpp

HEADERS  += \
    LogCategories.h \
    logcategories.h \
         mainwindow.h \
    connect.h

FORMS    += \
         mainwindow.ui

RESOURCES += \
    resources.qrc
