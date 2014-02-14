TEMPLATE = app
CONFIG += console c++11 thread
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -lpnapi -loscpack

HEADERS += \
    clock.h \
    clockclient.h \
    clockserver.h \
    oscreceiver.h

unix:!macx: LIBS += -L$$PWD/../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../git/oscpack/build/liboscpack.a
