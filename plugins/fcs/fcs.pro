TEMPLATE = lib
CONFIG += plugin

TARGET = qfrdrfcs
DEPENDPATH += ./

include(../plugins.pri)

DEFINES += TARGETNAME=$$TARGET

include(../../libquickfitwidgets.pri)

# Input
HEADERS += dlgcsvparameters.h \
           alv5000tools.h \
           alv6000tools.h \
           qfrdrfcscorrelationeditor.h \
           qfrdrfcsdata.h \
           qfrdrfcsrateeditor.h \
             qfprdrfcs.h \
             ../interfaces/qfrdrfcsdatainterface.h \
             ../interfaces/qfrdrcountratesinterface.h \
    ../../../../../LIB/trunk/qt/completertextedit.h \
    qfrdrfcsfitfunctionsimulator.h \
    ../interfaces/qfrdrrunselection.h \
    ../../../../../LIB/trunk/jkmathparser.h \
    ../../../../../LIB/trunk/jkiniparser2.h


SOURCES += dlgcsvparameters.cpp \
           alv5000tools.cpp \
           alv6000tools.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcsrateeditor.cpp \
             qfprdrfcs.cpp \
    ../../../../../LIB/trunk/qt/completertextedit.cpp \
    qfrdrfcsfitfunctionsimulator.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp \
    ../../../../../LIB/trunk/jkiniparser2.cpp


FORMS = dlg_csvparameters.ui \
    qfrdrfcsfitfunctionsimulator.ui

RESOURCES += qfrdrfcs.qrc

TRANSLATIONS= ./translations/de.fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl

ASSETSTESTDATA_FILES.files = ./examples/fcs_testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fcs_testdata/
ASSETSTESTFCCSDATA_FILES.files = ./examples/fccs_data/*.*
ASSETSTESTFCCSDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fccs_data/

INSTALLS += ASSETSTESTDATA_FILES ASSETSTESTFCCSDATA_FILES




