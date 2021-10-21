
#include "nmfEstimationTab06.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfEstimation_Tab6::nmfEstimation_Tab6(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Covariate Data"));


} // end constructor


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{

}

//void
//nmfEstimation_Tab6::readSettings()
//{
//    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::readSettings");

//    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

//    settings->beginGroup("Settings");
//    m_ModelName   = settings->value("Name","").toString().toStdString();
//    settings->endGroup();

//    settings->beginGroup("SetupTab");
//    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
//    settings->endGroup();

//    settings->beginGroup("Preferences");
//    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
//    settings->endGroup();

//    delete settings;
//}

