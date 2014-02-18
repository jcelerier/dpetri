QT += widgets network
TEMPLATE = app
CONFIG +=  c++11 thread
DESTDIR = ../../output/

INCLUDEPATH += ../lib/

SOURCES += \
    main.cpp \
    server.cpp \
    masterview.cpp \
    mainwindow.cpp

HEADERS += server.h \
    masterview.h \
    mainwindow.h


LIBS += -lpnapi -loscpack -ldns_sd

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a

include( ../lib/lib.pri )

FORMS += \
    masterview.ui \
    mainwindow.ui
