#-------------------------------------------------
#
# Project created by QtCreator 2011-09-09T14:10:48
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

# ---------------------------------------------------------------
# Se debe incluir esta linea para usar la libreria qextserialport
#include (qextserialport-1.2rc/src/qextserialport.pri)
# obviamente, debe respetarse el nombre y path de la misma
# ---------------------------------------------------------------

TARGET = QtInfotronic
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    qcustomplot.cpp \
    puertoserie.cpp

HEADERS  += mainwindow.h \	
    qcustomplot.h \
    puertoserie.h

FORMS    += mainwindow.ui

OTHER_FILES += \
	QtInfotronic.pr

DISTFILES += \
    gota.png \
    termometro.png

