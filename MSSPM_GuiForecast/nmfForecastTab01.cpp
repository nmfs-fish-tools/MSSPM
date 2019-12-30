#include "nmfForecastTab01.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfForecast_Tab1::nmfForecast_Tab1(QTabWidget*  tabs,
                                   nmfLogger*   logger,
                                   nmfDatabase* databasePtr,
                                   std::string& projectDir)
{
    QUiLoader loader;

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_SModel      = nullptr;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab1::nmfForecast_Tab1");

    Forecast_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Forecast/Forecast_Tab01.ui");
    file.open(QFile::ReadOnly);
    Forecast_Tab1_Widget = loader.load(&file,Forecast_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Forecast_Tabs->addTab(Forecast_Tab1_Widget, tr("1. Setup"));

    Forecast_Tab1_SetNamePB             = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab1_SetNamePB");
    Forecast_Tab1_LoadPB                = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab1_ReloadPB");
    Forecast_Tab1_SavePB                = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab1_SavePB");
    Forecast_Tab1_NextPB                = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab1_NextPB");
    Forecast_Tab1_PreviousRunGB         = Forecast_Tabs->findChild<QGroupBox *>("Forecast_Tab1_PreviousRunGB");
    Forecast_Tab1_AlgorithmLBL          = Forecast_Tabs->findChild<QLabel    *>("Forecast_Tab1_AlgorithmLBL");
    Forecast_Tab1_MinimizerLBL          = Forecast_Tabs->findChild<QLabel    *>("Forecast_Tab1_MinimizerLBL");
    Forecast_Tab1_ObjectiveCriterionLBL = Forecast_Tabs->findChild<QLabel    *>("Forecast_Tab1_ObjectiveCriterionLBL");
    Forecast_Tab1_AlgorithmCMB          = Forecast_Tabs->findChild<QComboBox *>("Forecast_Tab1_AlgorithmCMB");
    Forecast_Tab1_MinimizerCMB          = Forecast_Tabs->findChild<QComboBox *>("Forecast_Tab1_MinimizerCMB");
    Forecast_Tab1_ObjectiveCriterionCMB = Forecast_Tabs->findChild<QComboBox *>("Forecast_Tab1_ObjectiveCriterionCMB");
    Forecast_Tab1_StartYearLE           = Forecast_Tabs->findChild<QLineEdit *>("Forecast_Tab1_StartYearLE");
    Forecast_Tab1_EndYearLE             = Forecast_Tabs->findChild<QLineEdit *>("Forecast_Tab1_EndYearLE");
    Forecast_Tab1_RunLengthSB           = Forecast_Tabs->findChild<QSpinBox  *>("Forecast_Tab1_RunLengthSB");
    Forecast_Tab1_NameLE                = Forecast_Tabs->findChild<QLineEdit *>("Forecast_Tab1_NameLE");
    Forecast_Tab1_NumRunsSB             = Forecast_Tabs->findChild<QSpinBox  *>("Forecast_Tab1_NumRunsSB");
    Forecast_Tab1_DeterministicCB       = Forecast_Tabs->findChild<QCheckBox *>("Forecast_Tab1_DeterministicCB");
    Forecast_Tab1_DeterministicSB       = Forecast_Tabs->findChild<QSpinBox  *>("Forecast_Tab1_DeterministicSB");

    connect(Forecast_Tab1_SetNamePB,       SIGNAL(clicked()),
            this,                          SLOT(callback_SetNamePB()));
    connect(Forecast_Tab1_NextPB,          SIGNAL(clicked()),
            this,                          SLOT(callback_NextPB()));
    connect(Forecast_Tab1_LoadPB,          SIGNAL(clicked()),
            this,                          SLOT(callback_LoadPB()));
    connect(Forecast_Tab1_SavePB,          SIGNAL(clicked()),
            this,                          SLOT(callback_SavePB()));
    connect(Forecast_Tab1_AlgorithmCMB,    SIGNAL(activated(QString)),
            this,                          SLOT(callback_AlgorithmCMB(QString)));
    connect(Forecast_Tab1_MinimizerCMB,    SIGNAL(activated(QString)),
            this,                          SLOT(callback_MinimizerCMB(QString)));
    connect(Forecast_Tab1_RunLengthSB,     SIGNAL(valueChanged(int)),
            this,                          SLOT(callback_RunLengthSB(int)));
    connect(Forecast_Tab1_DeterministicCB, SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_DeterministicCB(int)));

    Forecast_Tab1_NextPB->setText("--\u25B7");
    Forecast_Tab1_NameLE->setClearButtonEnabled(true);

    readSettings();

    // Temporarily turn this off
    Forecast_Tab1_PreviousRunGB->setVisible(false);

} // end constructor


nmfForecast_Tab1::~nmfForecast_Tab1()
{

}

std::string
nmfForecast_Tab1::getForecastName()
{
    return Forecast_Tab1_NameLE->text().toStdString();
}

int
nmfForecast_Tab1::getStartForecastYear()
{
    return Forecast_Tab1_StartYearLE->text().toInt();
}

void
nmfForecast_Tab1::callback_SetNamePB()
{
    Forecast_Tab1_NameLE->setText(QString::fromStdString(m_ProjectSettingsConfig));
}

void
nmfForecast_Tab1::callback_NextPB()
{
    int nextPage = Forecast_Tabs->currentIndex()+1;
    Forecast_Tabs->setCurrentIndex(nextPage);
}

int
nmfForecast_Tab1::getSeed()
{
    return (Forecast_Tab1_DeterministicCB->isChecked()) ? Forecast_Tab1_DeterministicSB->value() : -1;
}

void
nmfForecast_Tab1::callback_LoadPB()
{
    std::string forecastToLoad;
    LoadForecastDlg *loadDialog = new LoadForecastDlg(tr("Load Forecast"),
                                                      Forecast_Tabs,
                                                      m_Logger,
                                                      m_DatabasePtr,
                                                      Forecast_Tab1_NameLE,
                                                      Forecast_Tab1_RunLengthSB,
                                                      Forecast_Tab1_NumRunsSB);
    if (loadDialog->exec() == QDialog::Accepted)
    {
       forecastToLoad = loadDialog->getSelectedItem();
       loadForecast(forecastToLoad);
       emit ResetOutputWidgetsForAggProd();
    }
}


void
nmfForecast_Tab1::callback_SavePB()
{
    bool systemFound   = false;
    int  isPreviousRun = false;
    int  NumRecords;
    std::string cmd;
    std::string errorMsg;
    std::string ForecastName    = Forecast_Tab1_NameLE->text().toStdString();
    std::string RunLength       = std::to_string(Forecast_Tab1_RunLengthSB->value());
    std::string StartYear       = Forecast_Tab1_StartYearLE->text().toStdString();
    std::string EndYear         = Forecast_Tab1_EndYearLE->text().toStdString();
    std::string NumRuns         = std::to_string(Forecast_Tab1_NumRunsSB->value());
    std::string IsDeterministic = std::to_string(Forecast_Tab1_DeterministicCB->isChecked());
    std::string Seed            = std::to_string(Forecast_Tab1_DeterministicSB->value());
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;

    readSettings();

    if (ForecastName == "") {
        QMessageBox::information(Forecast_Tabs, "Forecast Name Error",
                                 "\nPlease enter a Forecast name.\n",
                                 QMessageBox::Ok);
        return;
    }

    // Get forms
    fields    = {"GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm","RunLength"};
    queryStr  = "SELECT GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm,RunLength FROM Systems WHERE ";
    queryStr += "SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["RunLength"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfForecast_Tab1::callback_SavePB: No records found in table Systems for Name = "+m_ProjectSettingsConfig);
        return;
    }
    GrowthForm      = dataMap["GrowthForm"][0];
    HarvestForm     = dataMap["HarvestForm"][0];
    CompetitionForm = dataMap["WithinGuildCompetitionForm"][0];
    PredationForm   = dataMap["PredationForm"][0];

    if (isPreviousRun) {
        Algorithm          = Forecast_Tab1_AlgorithmCMB->currentText().toStdString();
        Minimizer          = Forecast_Tab1_MinimizerCMB->currentText().toStdString();
        ObjectiveCriterion = Forecast_Tab1_ObjectiveCriterionCMB->currentText().toStdString();
    } else {
        systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                    Forecast_Tabs,m_Logger,m_ProjectSettingsConfig,
                    Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                    CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
        if (! systemFound) {
            m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab1::callback_SavePB: No systems found");
            return;
        }
    }

    cmd = "DELETE FROM Forecasts WHERE ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab1::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in callback_SavePB command. Couldn't delete record from " +
                             QString::fromStdString(ForecastName) + " table.\n",
                             QMessageBox::Ok);
        return;
    }

    cmd  = "INSERT INTO Forecasts (ForecastName,PreviousRun,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    cmd += "GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm,RunLength,";
    cmd += "StartYear,EndYear,NumRuns,IsDeterministic,Seed) VALUES ";
    cmd += "('" + ForecastName + "'," + std::to_string(isPreviousRun) + ",'" +
            Algorithm + "','" + Minimizer + "','" +
            ObjectiveCriterion + "','" +
            Scaling + "','" +
            GrowthForm + "','" + HarvestForm+ "','" +
            CompetitionForm + "','" + PredationForm + "'," +
            RunLength + "," +StartYear + "," + EndYear + "," +
            NumRuns + "," + IsDeterministic + "," + Seed +")";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab1::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return;
    }

    // Re-load the forecast so that the other Forecast tabs will load properly
    loadForecast(ForecastName);

    QMessageBox::information(Forecast_Tabs, "Forecast Saved",
                             "\nForecast has been successfully saved in database.\n",
                             QMessageBox::Ok);
}


void
nmfForecast_Tab1::loadForecast(std::string forecastToLoad)
{
    int IsDeterministic;
    int Seed;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string RunLength;
    std::string NumRuns;
    std::string StartYear;
    std::string EndYear;
    std::string fAlgorithm,sAlgorithm;
    std::string fMinimizer,sMinimizer;
    std::string fObjectiveCriterion,sObjectiveCriterion;
    std::string fScaling,sScaling;
    std::string fGrowthForm,sGrowthForm;
    std::string fHarvestForm,sHarvestForm;
    std::string fCompetitionForm,sCompetitionForm;
    std::string fPredationForm,sPredationForm;
    std::string msg;

    if (forecastToLoad.empty()) {
        return;
    }

    // Load Output Growth Rate Algorithm, Minimizer, ObjectiveCriterion, and Scaling from OutputGrowthRate
    // so that user can see what runs have already been done.
    fields    = {"ForecastName","PreviousRun","Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                 "GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm",
                 "RunLength","StartYear","EndYear","NumRuns","IsDeterministic","Seed"};
    queryStr  = "SELECT ForecastName,PreviousRun,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr += "GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm,RunLength,";
    queryStr += "StartYear,EndYear,NumRuns,IsDeterministic,Seed from Forecasts WHERE ";
    queryStr += " ForecastName = '" + forecastToLoad + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() == 0) {
        m_Logger->logMsg(nmfConstants::Error,"No records found in table Forecasts for forecast: "+forecastToLoad);
        return;
    }
    RunLength           = dataMap["RunLength"][0];
    StartYear           = dataMap["StartYear"][0];
    EndYear             = dataMap["EndYear"][0];
    NumRuns             = dataMap["NumRuns"][0];
    fAlgorithm          = dataMap["Algorithm"][0];
    fMinimizer          = dataMap["Minimizer"][0];
    fObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    fScaling            = dataMap["Scaling"][0];
    fGrowthForm         = dataMap["GrowthForm"][0];
    fHarvestForm        = dataMap["HarvestForm"][0];
    fCompetitionForm    = dataMap["WithinGuildCompetitionForm"][0];
    fPredationForm      = dataMap["PredationForm"][0];
    IsDeterministic     = std::stoi(dataMap["IsDeterministic"][0]);
    Seed                = std::stoi(dataMap["Seed"][0]);

    // Check that Forecast forms match System forms (if not, the Forecast may fail.)
    fields    = {"SystemName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr  = "SELECT SystemName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm from Systems WHERE ";
    queryStr += " SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["SystemName"].size() == 0) {
        m_Logger->logMsg(nmfConstants::Error,"No records found in table Systems for SystemName: "+m_ProjectSettingsConfig);
        return;
    }
    sAlgorithm          = dataMap["Algorithm"][0];
    sMinimizer          = dataMap["Minimizer"][0];
    sObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    sScaling            = dataMap["Scaling"][0];
    sGrowthForm         = dataMap["GrowthForm"][0];
    sHarvestForm        = dataMap["HarvestForm"][0];
    sCompetitionForm    = dataMap["WithinGuildCompetitionForm"][0];
    sPredationForm      = dataMap["PredationForm"][0];

    if ((fAlgorithm != sAlgorithm) ||
            (fMinimizer != sMinimizer) ||
            (fObjectiveCriterion != sObjectiveCriterion) ||
            (fScaling != sScaling) ||
            (fGrowthForm != sGrowthForm) || (fHarvestForm != sHarvestForm) ||
            (fCompetitionForm != sCompetitionForm) || (fPredationForm != sPredationForm))
    {
        msg  = "\nForecast ("+forecastToLoad+") incompatible with System Run ("+m_ProjectSettingsConfig+").\n\n";
        msg += "(Forecast parameters on left and System Run parameters on right)\n\n";
        msg += fAlgorithm +          " --- " + sAlgorithm +          "\n";
        msg += fMinimizer +          " --- " + sMinimizer +          "\n";
        msg += fObjectiveCriterion + " --- " + sObjectiveCriterion + "\n";
        msg += fScaling +            " --- " + sScaling +            "\n";
        msg += fGrowthForm +         " --- " + sGrowthForm +         "\n";
        msg += fHarvestForm +        " --- " + sHarvestForm +        "\n";
        msg += fCompetitionForm +    " --- " + sCompetitionForm +    "\n";
        msg += fPredationForm +      " --- " + sPredationForm +      "\n";
        msg += "\n\nTry creating a new Forecast.\n";
        m_Logger->logMsg(nmfConstants::Error,msg);
        QMessageBox::warning(Forecast_Tabs, tr("Forecast Mismatch"), QString::fromStdString(msg),
                                      QMessageBox::Ok);
        return;
    }

    Forecast_Tab1_NameLE->setText(QString::fromStdString(forecastToLoad));
    Forecast_Tab1_RunLengthSB->setValue(std::stoi(RunLength));
    Forecast_Tab1_StartYearLE->setText(QString::fromStdString(StartYear));
    Forecast_Tab1_EndYearLE->setText(QString::fromStdString(EndYear));
    Forecast_Tab1_NumRunsSB->setValue(std::stoi(NumRuns));
    Forecast_Tab1_AlgorithmCMB->setCurrentText(QString::fromStdString(fAlgorithm));
    Forecast_Tab1_MinimizerCMB->setCurrentText(QString::fromStdString(fMinimizer));
    Forecast_Tab1_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(fObjectiveCriterion));
    Forecast_Tab1_DeterministicCB->setChecked(IsDeterministic);
    Forecast_Tab1_DeterministicSB->setValue(Seed);

    // Send signal so that other Forecast tabs will load as well
    emit ForecastLoaded(forecastToLoad);
}

/*
void
nmfForecast_Tab1::callback_PreviousRunCB(bool state)
{
    Forecast_Tab1_AlgorithmLBL->setEnabled(state);
    Forecast_Tab1_MinimizerLBL->setEnabled(state);
    Forecast_Tab1_ObjectiveCriterionLBL->setEnabled(state);
    Forecast_Tab1_AlgorithmCMB->setEnabled(state);
    Forecast_Tab1_MinimizerCMB->setEnabled(state);
    Forecast_Tab1_ObjectiveCriterionCMB->setEnabled(state);
}

void
nmfForecast_Tab1::callback_AlgorithmCMB(QString algorithm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString minimizer;

    // Read OutputGrowthRate to find all the Minimizer values for this algorithm value
    fields    = {"Algorithm","Minimizer"};
    queryStr  = "SELECT Algorithm,Minimizer from OutputGrowthRate WHERE ";
    queryStr += "Algorithm = '" + algorithm.toStdString() + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    Forecast_Tab1_MinimizerCMB->clear();
    Forecast_Tab1_ObjectiveCriterionCMB->clear();
    for (unsigned i=0; i<dataMap["Minimizer"].size(); ++i) {
        minimizer = QString::fromStdString(dataMap["Minimizer"][i]);
        if (Forecast_Tab1_MinimizerCMB->findText(minimizer) == -1) {
            Forecast_Tab1_MinimizerCMB->addItem(minimizer);
        }
    }
    minimizer = Forecast_Tab1_MinimizerCMB->currentText();
    callback_MinimizerCMB(minimizer);
}

void
nmfForecast_Tab1::callback_MinimizerCMB(QString minimizer)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString algorithm = Forecast_Tab1_AlgorithmCMB->currentText();
    QString objectiveCriterion;

    // Read OutputGrowthRate to find all the Minimizer values for this algorithm value
    fields    = {"Algorithm","Minimizer","ObjectiveCriterion"};
    queryStr  = "SELECT Algorithm,Minimizer,ObjectiveCriterion from OutputGrowthRate WHERE ";
    queryStr += "Algorithm = '" + algorithm.toStdString() + "' AND ";
    queryStr += "Minimizer = '" + minimizer.toStdString() + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    Forecast_Tab1_ObjectiveCriterionCMB->clear();
    for (unsigned i=0; i<dataMap["ObjectiveCriterion"].size(); ++i) {
        objectiveCriterion = QString::fromStdString(dataMap["ObjectiveCriterion"][i]);
        if (Forecast_Tab1_ObjectiveCriterionCMB->findText(objectiveCriterion) == -1) {
            Forecast_Tab1_ObjectiveCriterionCMB->addItem(objectiveCriterion);
        }
    }
}
*/

void
nmfForecast_Tab1::callback_RunLengthSB(int duration)
{
    int startYear = Forecast_Tab1_StartYearLE->text().toInt();
    int endYear   = startYear + duration;

    Forecast_Tab1_EndYearLE->setText(QString::number(endYear));
}


void
nmfForecast_Tab1::callback_DeterministicCB(int checked)
{
    Forecast_Tab1_DeterministicSB->setEnabled(checked == Qt::Checked);
}

void
nmfForecast_Tab1::clearWidgets()
{
    readSettings();

    Forecast_Tab1_NameLE->clear();
    Forecast_Tab1_RunLengthSB->setValue(1);
    Forecast_Tab1_NumRunsSB->setValue(1);
}

bool
nmfForecast_Tab1::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab1::loadWidgets()");

    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    int StartForecastYear;
    int EndForecastYear;

    // Load years
    fields    = {"StartYear","RunLength"};
    queryStr  = "SELECT StartYear,RunLength from Systems WHERE ";
    queryStr += "SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["StartYear"].size() == 0) {
        std::cout << "Error 2: nmfForecast_Tab1::loadWidgets" << std::endl;
        std::cout << queryStr << std::endl;
        return false;
    }
    StartForecastYear = std::stoi(dataMap["StartYear"][0]) +
                        std::stoi(dataMap["RunLength"][0]);
    EndForecastYear = StartForecastYear + Forecast_Tab1_RunLengthSB->value();
    Forecast_Tab1_StartYearLE->setText(QString::number(StartForecastYear));
    Forecast_Tab1_EndYearLE->setText(QString::number(EndForecastYear));

    return true;
}

void
nmfForecast_Tab1::callback_UpdateForecastYears()
{
    loadWidgets();
}

void
nmfForecast_Tab1::readSettings()
{
    std::string ForecastName;
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Forecast");
    ForecastName = settings->value("Name","").toString().toStdString();
    loadForecast(ForecastName);
    settings->endGroup();

    delete settings;
}

void
nmfForecast_Tab1::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Forecast");
    settings->setValue("Name", Forecast_Tab1_NameLE->text());
    settings->endGroup();

    delete settings;
}
