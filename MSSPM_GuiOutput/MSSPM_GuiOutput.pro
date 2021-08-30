#-------------------------------------------------
#
# Project created by QtCreator 2019-02-04T14:18:12
#
#-------------------------------------------------

QT       += core gui charts sql datavisualization uitools

TARGET = MSSPM_GuiOutput
TEMPLATE = lib

PRECOMPILED_HEADER = /home/rklasky/workspaceQtCreator/MSSPM/MSSPM_GuiSetup/precompiled_header.h
CONFIG += precompile_header

DEFINES += MSSPM_GUIOUTPUT_LIBRARY
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
    nmfOutputControls.cpp \
    nmfOutputTableWidgets.cpp

HEADERS += \
    mainpage.h \
    nmfOutputControls.h \
    nmfOutputTableWidgets.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfDatabase-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfDatabase-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfDatabase
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfDatabase-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfDatabase

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase
