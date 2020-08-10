#-------------------------------------------------
#
# Project created by QtCreator 2018-05-06T14:23:37
#
#-------------------------------------------------

INCLUDEPATH += "C:/Libs/boost_1_71_0-64bit/"
LIBS += -LC:/Libs/boost_1_71_0-64bit/lib \
-lboost_system-mgw81-mt-x64-1_71 \
-lboost_filesystem-mgw81-mt-x64-1_71

#INCLUDEPATH += "C:/Libs/boost_1_71_0-32bit/"
#LIBS += -LC:/Libs/boost_1_71_0-32bit/boost/lib \
#-lboost_system-mgw53-mt-x32-1_71 \
#-lboost_filesystem-mgw53-mt-x32-1_71

QT       += core gui charts sql datavisualization uitools

TARGET = MSSPM_GuiSetup
TEMPLATE = lib

PRECOMPILED_HEADER = "C:\Users\ellio\Documents\Programming Projects\NOAA Projects\Qt Builds\MSSPM\MSSPM_GuiSetup\precompiled_header.h"
CONFIG += precompile_header

DEFINES += MSSPM_GUISETUP_LIBRARY
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
    LoadDlg.cpp \
    nmfSetupTab01.cpp \
    nmfSetupTab02.cpp \
    nmfSetupTab03.cpp \
    nmfSetupTab04.cpp

HEADERS += \
    LoadDlg.h \
    mainpage.h \
    nmfSetupTab01.h \
    nmfSetupTab02.h \
    nmfSetupTab03.h \
    nmfSetupTab04.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_32_bit-Release/release/ -lnmfDatabase
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_32_bit-Release/debug/ -lnmfDatabase
#else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_32_bit-Release/ -lnmfDatabase

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_64_bit-Release/release/ -lnmfDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_64_bit-Release/debug/ -lnmfDatabase
else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfDatabase-Desktop_Qt_5_12_3_MinGW_64_bit-Release/ -lnmfDatabase

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/release/ -lnmfUtilities
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/debug/ -lnmfUtilities
#else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/ -lnmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
