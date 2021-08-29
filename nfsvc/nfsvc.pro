TEMPLATE = app
#CONFIG += console c++11
#CONFIG -= app_bundle
#CONFIG -= qt

LIBS += -luser32 -lpsapi
DEFINES += PSAPI_VERSION=1
SOURCES += \
        controlling.cpp \
        service.cpp

DESTDIR = ../setup/data
