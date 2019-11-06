#-------------------------------------------------
#
# Project created by QtCreator 2018-10-24T20:52:04
#
#-------------------------------------------------

QT       += core gui charts sql datavisualization uitools

TARGET = MSSPM_GuiDiagnostic
TEMPLATE = lib

PRECOMPILED_HEADER = /home/rklasky/workspaceQtCreator/MSSPM/MSSPM_GuiSetup/precompiled_header.h
CONFIG += precompile_header

DEFINES += MSSPM_GUIDIAGNOSTIC_LIBRARY
CONFIG += c++14

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
    nmfDiagnosticTab01.cpp \
    nmfDiagnosticTab02.cpp

HEADERS +=\
    nmfDiagnosticTab01.h \
    nmfDiagnosticTab02.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/release/ -lnmfDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/debug/ -lnmfDatabase
else:unix: LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/ -lnmfDatabase

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/release/ -lBeesAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/debug/ -lBeesAlgorithm
else:unix: LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/ -lBeesAlgorithm

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm
DEPENDPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_ParameterEstimationNLoptAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_ParameterEstimationNLoptAlgorithm
else:unix: LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/ -lMSSPM_ParameterEstimationNLoptAlgorithm

INCLUDEPATH += $$PWD/../MSSPM_ParameterEstimationNLoptAlgorithm
DEPENDPATH += $$PWD/../MSSPM_ParameterEstimationNLoptAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/release/ -lnmfModels
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/debug/ -lnmfModels
else:unix: LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/ -lnmfModels

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfModels
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfModels
