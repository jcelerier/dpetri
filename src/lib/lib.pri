TEMPLATE=subdirs

HEADERS += \
    $$PWD/clock/clock.h \
    $$PWD/clock/clockclient.h \
    $$PWD/clock/clockserver.h \
    $$PWD/osc/oscreceiver.h \
    $$PWD/osc/oscsender.h \
    $$PWD/clientmanager.h \
    $$PWD/petrinetstuff.h \
    $$PWD/zeroconf/bonjourrecord.h \
    $$PWD/zeroconf/bonjourserviceregister.h \
    $$PWD/zeroconf/bonjourservicebrowser.h \
    $$PWD/zeroconf/bonjourserviceresolver.h \
    $$PWD/gui/petrinetview.h \
    $$PWD/gui/petrinetmodel.h \
    $$PWD/osc/oscmessagegenerator.h \
    $$PWD/petrinetserializer.h \
    $$PWD/nodepool.h \
    $$PWD/client/remoteclient.h \
    $$PWD/ownednode.h \
    $$PWD/gui/executionalgorithmbase.h \
    $$PWD/client/client.h \
    $$PWD/client/localclient.h \
    $$PWD/commonlogic/commonlogic.h \
    $$PWD/commonlogic/commonlogic.handlers.h \
    $$PWD/commonlogic/commonlogic.algorithm.h \
    $$PWD/simpleNet/simpleReader.h \
    $$PWD/simpleNet/simplePetriNet.h \
    $$PWD/type_helper.h

SOURCE += \
    $$PWD/simpleNet/simplePetriNet.cpp

SOURCES += \
    ../lib/simpleNet/simplePetriNet.cpp
