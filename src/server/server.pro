QT += core widgets network svg
TEMPLATE = app
CONFIG +=  c++11 thread warn_on
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

QMAKE_CXXFLAGS += -Wall -pedantic -Wextra -Wcast-align  -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion  -Wdisabled-optimization    -Wformat  -Wformat=1  -Wformat-nonliteral -Wformat-security   -Wformat-y2k  -Wimport  -Winit-self  -Winline  -Winvalid-pch    -Wunsafe-loop-optimizations  -Wmissing-braces  -Wmissing-field-initializers -Wmissing-format-attribute    -Wmissing-include-dirs -Wmissing-noreturn  -Wpacked  -Wparentheses  -Wpointer-arith  -Wredundant-decls -Wreturn-type  -Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector -Wstrict-aliasing=3 -Wswitch  -Wswitch-default  -Wswitch-enum -Wtrigraphs  -Wuninitialized  -Wunknown-pragmas  -Wunreachable-code -Wunused  -Wunused-function  -Wunused-label  -Wunused-parameter  -Wunused-value  -Wunused-variable  -Wvariadic-macros  -Wvolatile-register-var  -Wwrite-strings



DEFINES += USE_PNAPI
LIBS += -lpnapi -loscpack -ldns_sd -lgvc -lcgraph

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a
include(../lib/lib.pri)

FORMS += \
    masterview.ui \
    mainwindow.ui
