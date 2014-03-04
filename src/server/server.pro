QT += core widgets network svg
TEMPLATE = app
CONFIG +=  c++11 thread
DESTDIR = ../../output/

INCLUDEPATH += ../lib/

SOURCES += \
    main.cpp \
    server.cpp \
    masterview.cpp \
    mainwindow.cpp \
    serverlogic.cpp

HEADERS += server.h \
    masterview.h \
    mainwindow.h \
    serverlogic.h

DEFINES += USE_PNAPI
LIBS += -lpnapi -loscpack -ldns_sd -lgvc -lcgraph

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a

include( ../lib/lib.pri )

FORMS += \
    masterview.ui \
    mainwindow.ui
