TEMPLATE = app
CONFIG += console c++11 thread
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../lib/

SOURCES += main.cpp


LIBS += -lpnapi -loscpack

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a

include( ../lib/lib.pri )
