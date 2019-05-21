QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYScreenRecorder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        gif.cpp

HEADERS  += mainwindow.h\
        gif.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc