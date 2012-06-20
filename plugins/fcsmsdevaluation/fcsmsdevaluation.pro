TEMPLATE = lib
CONFIG += plugin

TARGET = fcsmsdevaluation
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)
include(../../extlibs/lmfit.pri)

# Input
HEADERS += qffcsmsdevaluation.h \
           qffcsmsdevaluation_item.h \
           qffcsmsdevaluation_editor.h \
    ../base_classes/qfusesresultsevaluation.h \
    ../base_classes/qfusesresultsbyindexevaluation.h \
    ../interfaces/qfrdrfcsdatainterface.h \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.h \
    ../base_classes/qfusesresultsbyindexevaluationeditor.h \
    ../base_classes/qffcsbyindexandmodelevaluationeditor.h 

SOURCES += qffcsmsdevaluation.cpp \
           qffcsmsdevaluation_item.cpp \
           qffcsmsdevaluation_editor.cpp \
    ../base_classes/qfusesresultsevaluation.cpp \
    ../base_classes/qfusesresultsbyindexevaluation.cpp \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.cpp \
    ../base_classes/qfusesresultsbyindexevaluationeditor.cpp \
    ../base_classes/qffcsbyindexandmodelevaluationeditor.cpp

FORMS =   

RESOURCES += fcsmsdevaluation.qrc

TRANSLATIONS= ./translations/de.fcsmsdevaluation.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl


ASSETSTESTDATA_FILES.files = ./assets/testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/testdata/


INSTALLS += ASSETSTESTDATA_FILES

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS