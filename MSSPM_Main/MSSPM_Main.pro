#-------------------------------------------------
#
# Project created by QtCreator 2018-05-06T14:44:14
#
#-------------------------------------------------

QT       += core gui charts sql datavisualization uitools concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MSSPM
TEMPLATE = app

PRECOMPILED_HEADER = /home/rklasky/workspaceQtCreator/MSSPM/MSSPM_Main/precompiled_header.h
CONFIG += precompile_header

QMAKE_CXXFLAGS += -DATL_HAS_EIGEN
QMAKE_CFLAGS += -DATL_HAS_EIGEN

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -std=c++0x


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

LIBS += -lboost_system -lboost_filesystem

SOURCES += \
    main.cpp \
    nmfMainWindow.cpp \
    ClearOutputDialog.cpp \
    PreferencesDialog.cpp

HEADERS  += \
    mainpage.h \
    nmfMainWindow.h \
    ClearOutputDialog.h \
    PreferencesDialog.h

FORMS += \
    nmfMainWindow.ui

RESOURCES += \
    resource.qrc \
    qdarkstyle/style.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# For the Bees code
INCLUDEPATH += /home/rklasky

unix|win32: LIBS += -L/usr/local/lib -lnlopt_cxx
INCLUDEPATH += /usr/local/lib

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/release/ -lnmfGuiComponentsMain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/debug/ -lnmfGuiComponentsMain
else:unix: LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/ -lnmfGuiComponentsMain

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfGuiComponentsMain
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfGuiComponentsMain

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/release/ -lnmfCharts
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/debug/ -lnmfCharts
else:unix: LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/ -lnmfCharts

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfCharts
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfCharts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/release/ -lBeesAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/debug/ -lBeesAlgorithm
else:unix: LIBS += -L$$PWD/../../build-BeesAlgorithm-Qt_5_12_3_gcc64-Release/ -lBeesAlgorithm

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm
DEPENDPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/release/ -lnmfModels
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/debug/ -lnmfModels
else:unix: LIBS += -L$$PWD/../../build-nmfModels-Qt_5_12_3_gcc64-Release/ -lnmfModels

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfModels
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfModels

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/release/ -lnmfGuiDialogs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/debug/ -lnmfGuiDialogs
else:unix: LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/ -lnmfGuiDialogs

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfGuiDialogs
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfGuiDialogs

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_ParameterEstimationNLoptAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_ParameterEstimationNLoptAlgorithm
else:unix: LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationNLoptAlgorithm-Qt_5_12_3_gcc64-Release/ -lMSSPM_ParameterEstimationNLoptAlgorithm

INCLUDEPATH += $$PWD/../MSSPM_ParameterEstimationNLoptAlgorithm
DEPENDPATH += $$PWD/../MSSPM_ParameterEstimationNLoptAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationBeesAlgorithm-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_ParameterEstimationBeesAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationBeesAlgorithm-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_ParameterEstimationBeesAlgorithm
else:unix: LIBS += -L$$PWD/../../build-MSSPM_ParameterEstimationBeesAlgorithm-Qt_5_12_3_gcc64-Release/ -lMSSPM_ParameterEstimationBeesAlgorithm

INCLUDEPATH += $$PWD/../MSSPM_ParameterEstimationBeesAlgorithm
DEPENDPATH += $$PWD/../MSSPM_ParameterEstimationBeesAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiSetup-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_GuiSetup
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiSetup-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_GuiSetup
else:unix: LIBS += -L$$PWD/../../build-MSSPM_GuiSetup-Qt_5_12_3_gcc64-Release/ -lMSSPM_GuiSetup

INCLUDEPATH += $$PWD/../MSSPM_GuiSetup
DEPENDPATH += $$PWD/../MSSPM_GuiSetup

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiEstimation-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_GuiEstimation
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiEstimation-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_GuiEstimation
else:unix: LIBS += -L$$PWD/../../build-MSSPM_GuiEstimation-Qt_5_12_3_gcc64-Release/ -lMSSPM_GuiEstimation

INCLUDEPATH += $$PWD/../MSSPM_GuiEstimation
DEPENDPATH += $$PWD/../MSSPM_GuiEstimation

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiDiagnostic-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_GuiDiagnostic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiDiagnostic-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_GuiDiagnostic
else:unix: LIBS += -L$$PWD/../../build-MSSPM_GuiDiagnostic-Qt_5_12_3_gcc64-Release/ -lMSSPM_GuiDiagnostic

INCLUDEPATH += $$PWD/../MSSPM_GuiDiagnostic
DEPENDPATH += $$PWD/../MSSPM_GuiDiagnostic

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiForecast-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_GuiForecast
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiForecast-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_GuiForecast
else:unix: LIBS += -L$$PWD/../../build-MSSPM_GuiForecast-Qt_5_12_3_gcc64-Release/ -lMSSPM_GuiForecast

INCLUDEPATH += $$PWD/../MSSPM_GuiForecast
DEPENDPATH += $$PWD/../MSSPM_GuiForecast

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiOutput-Qt_5_12_3_gcc64-Release/release/ -lMSSPM_GuiOutput
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSSPM_GuiOutput-Qt_5_12_3_gcc64-Release/debug/ -lMSSPM_GuiOutput
else:unix: LIBS += -L$$PWD/../../build-MSSPM_GuiOutput-Qt_5_12_3_gcc64-Release/ -lMSSPM_GuiOutput

INCLUDEPATH += $$PWD/../MSSPM_GuiOutput
DEPENDPATH += $$PWD/../MSSPM_GuiOutput
