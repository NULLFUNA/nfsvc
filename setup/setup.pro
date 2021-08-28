TEMPLATE = app
#CONFIG += console c++11
#CONFIG -= app_bundle
#CONFIG -= qt

QT += core gui widgets

CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = Setup
INSTALLS += target

SOURCES += \
	SetupMain.cpp

DISTFILES += info.rc
RC_FILE = info.rc
