QT += core network widgets
CONFIG += c++11
INCLUDEPATH += ../lib/

include( ../lib/lib.pri )

# Add more folders to ship with the application, here
folder_01.source = qml/androidGUI
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    mainapplication.cpp \
    androidclientlogic.cpp



# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    mainapplication.h \
    androidclientlogic.h

unix:!macx: LIBS += -L$$PWD/../../../androidBuilds/oscpack/libs/armeabi-v7a/ -loscpack

INCLUDEPATH += $$PWD/../../../androidBuilds/oscpack/src
DEPENDPATH += $$PWD/../../../androidBuilds/oscpack/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../androidBuilds/oscpack/libs/armeabi-v7a/liboscpack.a

unix:!macx: LIBS += -L$$PWD/../../../androidBuilds/mdnsresponder/libs/armeabi/ -lmdnssd

INCLUDEPATH += $$PWD/../../../androidBuilds/mdnsresponder/mDNSShared
DEPENDPATH += $$PWD/../../../androidBuilds/mdnsresponder/mDNSShared
