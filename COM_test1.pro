#-------------------------------------------------
#
# Project created by QtCreator 2020-04-28T11:29:47
#
#-------------------------------------------------

QT       += core gui\
    serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = COM_test1
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    logcategories.cpp \
    main.cpp\
    mainwindow.cpp \
    connect.cpp \
    qcustomplot.cpp

HEADERS  += \
    LogCategories.h \
    constant.h \
    logcategories.h \
    mainwindow.h \
    connect.h \
    qcustomplot.h

FORMS    += \
    mainwindow.ui

RESOURCES += \
    resources.qrc
