#-------------------------------------------------
#
# Project created by QtCreator 2019-02-21T18:13:31
#
#-------------------------------------------------

QT      -= gui
QT      += widgets charts

TARGET = MSSPM_ParameterEstimationNLoptAlgorithm
TEMPLATE = lib
CONFIG += c++14

DEFINES += MSSPM_PARAMETERESTIMATIONNLOPTALGORITHM_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    NLopt_Estimator.cpp

HEADERS += \
    NLopt_Estimator.h \
    mainpage.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfModels-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfModels
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfModels-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfModels
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfModels-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfModels

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfModels
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfModels



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../nlopt-2.5.0/build/release/ -lnlopt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../nlopt-2.5.0/build/debug/ -lnlopt
else:unix: LIBS += -L$$PWD/../../../nlopt-2.5.0/build/ -lnlopt

INCLUDEPATH += $$PWD/../../../nlopt-2.5.0/build/src/api
DEPENDPATH += $$PWD/../../../nlopt-2.5.0/build/src/api

unix|win32: LIBS += -L/usr/local/lib -lnlopt # -lnlopt_cxx
INCLUDEPATH += /usr/local/lib



