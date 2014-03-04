TEMPLATE = app
QT += widgets network svg
CONFIG += c++11 thread
DESTDIR = ../../output/

INCLUDEPATH += ../lib/

SOURCES += \
    main.cpp \
    clientview.cpp \
    mainwindow.cpp \
    zeroconfconnectdialog.cpp \
    clientlogic.cpp

HEADERS += \
    clientview.h \
    mainwindow.h \
    zeroconfconnectdialog.h \
    oscconnectionmanager.h \
    clientlogic.h

DEFINES += USE_PNAPI
LIBS += -lpnapi -loscpack -ldns_sd -lgvc -lcgraph

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a

include( ../lib/lib.pri )

FORMS += \
    clientview.ui \
    mainwindow.ui
