#-------------------------------------------------
#
# Project created by QtCreator 2015-10-20T23:43:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtcsdr
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qmyspectrumwidget.cpp

HEADERS  += mainwindow.h \
    qmyspectrumwidget.h

FORMS    += mainwindow.ui

CONFIG += c++11
