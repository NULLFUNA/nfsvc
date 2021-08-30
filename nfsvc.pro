TEMPLATE = app
CONFIG += c++11
CONFIG -= qt

LIBS += -luser32 -lpsapi
DEFINES += PSAPI_VERSION=1
SOURCES += \
        controlling.cpp \
        service.cpp

DISTFILES += info.rc
RC_FILE = info.rc
