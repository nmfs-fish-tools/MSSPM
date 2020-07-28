#-------------------------------------------------
#
# Project created by QtCreator 2018-08-14T16:36:27
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

QT       -= gui

TARGET = MSSPM_ParameterEstimationBeesAlgorithm
TEMPLATE = lib
CONFIG += c++14

DEFINES += MSSPM_PARAMETERESTIMATIONBEESALGORITHM_LIBRARY

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
    Bees_Estimator.cpp \
    BeesStats.cpp

HEADERS += \
    Bees_Estimator.h \
    BeesStats.h \
    mainpage.h

unix {
    target.path = /Users/ellio
    INSTALLS += target
}

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_32_bit-Release/release/ -lnmfModels
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_32_bit-Release/debug/ -lnmfModels
#else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_32_bit-Release/ -lnmfModels

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_64_bit-Release/release/ -lnmfModels
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_64_bit-Release/debug/ -lnmfModels
else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfModels-Desktop_Qt_5_12_3_MinGW_64_bit-Release/ -lnmfModels

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfModels
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfModels

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/release/ -lnmfUtilities
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/debug/ -lnmfUtilities
#else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_32_bit-Release/ -lnmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-nmfUtilities-Desktop_Qt_5_12_3_MinGW_64_bit-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_32_bit-Release/release/ -lBeesAlgorithm
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_32_bit-Release/debug/ -lBeesAlgorithm
#else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_32_bit-Release/ -lBeesAlgorithm

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_64_bit-Release/release/ -lBeesAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_64_bit-Release/debug/ -lBeesAlgorithm
else:unix: LIBS += -L$$PWD/../../"nmfSharedUtilities Builds"/build-BeesAlgorithm-Desktop_Qt_5_12_3_MinGW_64_bit-Release/ -lBeesAlgorithm

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm
DEPENDPATH += $$PWD/../../nmfSharedUtilities/BeesAlgorithm
