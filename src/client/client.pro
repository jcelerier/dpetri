TEMPLATE = app
QT += widgets network svg
CONFIG += c++11 thread
DESTDIR = ../../output/

INCLUDEPATH += ../lib/

SOURCES += \
    main.cpp \
    clientview.cpp \
    mainwindow.cpp \
    zeroconfconnectdialog.cpp

HEADERS += \
    clientview.h \
    mainwindow.h \
    zeroconfconnectdialog.h \
    oscconnectionmanager.h


LIBS += -lpnapi -loscpack -ldns_sd -lgraph -lgvc -lcdt

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a

include( ../lib/lib.pri )

FORMS += \
    clientview.ui \
    mainwindow.ui
