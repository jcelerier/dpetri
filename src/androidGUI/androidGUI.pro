TEMPLATE = app
QT += widgets network svg
CONFIG += c++11 thread

INCLUDEPATH += ../lib/

include( ../lib/lib.pri )

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    androidclientlogic.cpp \
    mainwindow.cpp \
    zeroconfconnectdialog.cpp \
    clientview.cpp \
    ipconnectdialog.cpp

HEADERS += \
    androidclientlogic.h \
    mainwindow.h \
    zeroconfconnectdialog.h \
    clientview.h \
    ipconnectdialog.h

FORMS += \
    mainwindow.ui \
    clientview.ui \
    ipconnectdialog.ui

android-g++ {
unix:!macx: LIBS += -L$$PWD/../../../androidBuilds/oscpack/libs/armeabi-v7a/ -loscpack

INCLUDEPATH += $$PWD/../../../androidBuilds/oscpack/src
DEPENDPATH += $$PWD/../../../androidBuilds/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../androidBuilds/oscpack/libs/armeabi-v7a/liboscpack.a

unix:!macx: LIBS += -L$$PWD/../../../androidBuilds/mdnsresponder/libs/armeabi/ -lmdnssd

INCLUDEPATH += $$PWD/../../../androidBuilds/mdnsresponder/mDNSShared
DEPENDPATH += $$PWD/../../../androidBuilds/mdnsresponder/mDNSShared

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml

ANDROID_EXTRA_LIBS =
}

linux-g++ {
LIBS += -loscpack -ldns_sd

unix:!macx: LIBS += -L$$PWD/../../../../../git/oscpack/build/ -loscpack

INCLUDEPATH += $$PWD/../../../../../git/oscpack/src
DEPENDPATH += $$PWD/../../../../../git/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../git/oscpack/build/liboscpack.a
}

