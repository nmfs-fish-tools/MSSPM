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
    QString   NLoptMsg;
    QString   BeesMsg;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_FontSize     = 9;
    m_IsMonospaced = false;
    m_ProjectDir   = projectDir;
    m_ProjectSettingsConfig.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Run Estimation"));

    Estimation_Tab6_Bees_ParametersGB       = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_Bees_ParametersGB");
    Estimation_Tab6_NL_ParametersGB         = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_NL_ParametersGB");
    Estimation_Tab6_EstimationAlgorithmCMB  = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EstimationAlgorithmCMB");
    Estimation_Tab6_ObjectiveCriterionCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ObjectiveCriterionCMB");
    Estimation_Tab6_MinimizerAlgorithmCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinimizerAlgorithmCMB");
    Estimation_Tab6_MinimizerAlgorithmLBL   = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_MinimizerAlgorithmLBL");
    Estimation_Tab6_RunTE                   = Estimation_Tabs->findChild<QTextEdit   *>("Estimation_Tab6_RunTE");
    Estimation_Tab6_RunPB                   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_RunPB");
    Estimation_Tab6_ReloadPB                = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ReloadPB");
    Estimation_Tab6_SavePB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_SavePB");
    Estimation_Tab6_PrevPB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_PrevPB");
    Estimation_Tab6_FontSizeCMB             = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_FontSizeCMB");
    Estimation_Tab6_MonoCB                  = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_MonoCB");
    Estimation_Tab6_NumberOfRunsSB          = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NumberOfRunsSB");
    Estimation_Tab6_Bees_NumBeesSB          = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBeesSB");
    Estimation_Tab6_Bees_NumEliteSitesSB    = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteSitesSB");
    Estimation_Tab6_Bees_NumBestSitesSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBestSitesSB");
    Estimation_Tab6_Bees_NumEliteBeesSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteBeesSB");
    Estimation_Tab6_Bees_NumBestBeesSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumOtherBeesSB");
    Estimation_Tab6_Bees_MaxGenerationsSB   = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_MaxGenerationsSB");
    Estimation_Tab6_Bees_NeighborhoodSizeSB = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NeighborhoodSizeSB");
    Estimation_Tab6_ScalingCMB              = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ScalingCMB");
    Estimation_Tab6_NL_StopValCB            = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopValCB");
    Estimation_Tab6_NL_StopAfterTimeCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterTimeCB");
    Estimation_Tab6_NL_StopAfterIterCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterIterCB");
    Estimation_Tab6_NL_StopValLE            = Estimation_Tabs->findChild<QLineEdit   *>("Estimation_Tab6_NL_StopValLE");
    Estimation_Tab6_NL_StopAfterTimeSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterTimeSB");
    Estimation_Tab6_NL_StopAfterIterSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterIterSB");

    // Update tool tip
    BeesMsg  = "Stochastic search algorithm based on the behavior of honey bees.";
    NLoptMsg = "Open-Source NonLinear Optimization package";

    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 0, BeesMsg,        Qt::ToolTipRole);
    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 1, NLoptMsg,       Qt::ToolTipRole);

    QFont font = Estimation_Tab6_RunTE->font();
    font.setBold(false);
    Estimation_Tab6_RunTE->setFont(font);
    Estimation_Tab6_PrevPB->setText("\u25C1--");

    connect(Estimation_Tab6_PrevPB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_PrevPB()));
    connect(Estimation_Tab6_RunPB,                  SIGNAL(clicked()),
            this,                                   SLOT(callback_RunPB()));
    connect(Estimation_Tab6_SavePB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_SavePB()));
    connect(Estimation_Tab6_ReloadPB,               SIGNAL(clicked()),
            this,                                   SLOT(callback_LoadPB()));
    connect(Estimation_Tab6_FontSizeCMB,            SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_Estimation_Tab6_FontSizeCMB(QString)));
    connect(Estimation_Tab6_MonoCB,                 SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_Estimation_Tab6_MonoCB(int)));
    connect(Estimation_Tab6_EstimationAlgorithmCMB, SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_Estimation_Tab6_EstimationAlgorithmCMB(QString)));
    connect(Estimation_Tab6_NL_StopValCB,           SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopValCB(int)));
    connect(Estimation_Tab6_NL_StopAfterTimeCB,     SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopAfterTimeCB(int)));
    connect(Estimation_Tab6_NL_StopAfterIterCB,     SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopAfterIterCB(int)));

    readSettings();

    // Initialize font
    Estimation_Tab6_MonoCB->blockSignals(true);
    Estimation_Tab6_MonoCB->setChecked(m_IsMonospaced);
    Estimation_Tab6_MonoCB->blockSignals(false);
    QString fontName = (m_IsMonospaced) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    QFont defaultFont(fontName,11,QFont::Medium,false);
    setFont(defaultFont);

    callback_Estimation_Tab6_EstimationAlgorithmCMB("Bees Algorithm");

    m_Logger->logMsg(nmfConstants::Normal,"nmfMSPRODTab6::nmfMSPRODTab6 Complete");
}


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{
}

void
nmfEstimation_Tab6::setFont(QFont font)
{
    Estimation_Tab6_RunTE->setFont(font);
}

void
nmfEstimation_Tab6::setOutputTE(QString msg)
{
    Estimation_Tab6_RunTE->setText(msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}
void
nmfEstimation_Tab6::appendOutputTE(QString msg)
{
    QString currentMsg = Estimation_Tab6_RunTE->document()->toHtml();
    Estimation_Tab6_RunTE->setText(currentMsg+msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}

void
nmfEstimation_Tab6::clearOutputTE()
{
    Estimation_Tab6_RunTE->clear();
}

void
nmfEstimation_Tab6::callback_Estimation_Tab6_MonoCB(int isChecked)
{
    QString fontName = (isChecked == Qt::Checked) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    emit ShowRunMessage(fontName);
}

void
nmfEstimation_Tab6::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab6::callback_RunPB()
{
    m_Logger->logMsg(nmfConstants::Normal,"");
    m_Logger->logMsg(nmfConstants::Normal,"Start Estimation");
    emit RunEstimation();
}

void
nmfEstimation_Tab6::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab6::callback_SavePB()
{
    saveSystem(true);
}

void
nmfEstimation_Tab6::callback_SaveSettings()
{
    saveSystem(false);
}

void
nmfEstimation_Tab6::saveSystem(bool RunChecks)
{
    bool okToSave = true;
    std::string msg;
    std::string SystemName;

    readSettings();
    SystemName = m_ProjectSettingsConfig;

    if (RunChecks) {
        msg = "\nOK to save current settings as: " + SystemName + " ?";
        QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Save"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes,
                                                                  QMessageBox::Yes);
        okToSave = (reply == QMessageBox::Yes);
    }
    if (okToSave) {
        m_ProjectSettingsConfig = SystemName;
        saveSettingsConfiguration(RunChecks,SystemName);
        readSettings();
    }

    emit UpdateForecastYears();
}

bool
nmfEstimation_Tab6::saveSettingsConfiguration(bool verbose,
                                              std::string CurrentSettingsName)
{
    std::string cmd;
    std::string errorMsg;

    cmd  = "UPDATE Systems SET";
    cmd += "   NumberOfRuns = "          + std::to_string(Estimation_Tab6_NumberOfRunsSB->value()) +
           ",  TimeStep = 1"             + // std::to_string(Estimation_Tab6_TimeStepSB->value()) +
           ",  Algorithm = '"            + Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString() +
           "', Minimizer = '"            + Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString() +
           "', ObjectiveCriterion = '"   + Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString() +
           "', Scaling = '"              + Estimation_Tab6_ScalingCMB->currentText().toStdString() +
           "', BeesNumTotal = "          + std::to_string(Estimation_Tab6_Bees_NumBeesSB->value()) +
           ",  BeesNumElite = "          + std::to_string(Estimation_Tab6_Bees_NumEliteSitesSB->value()) +
           ",  BeesNumOther = "          + std::to_string(Estimation_Tab6_Bees_NumBestSitesSB->value()) +
           ",  BeesNumEliteSites = "     + std::to_string(Estimation_Tab6_Bees_NumEliteBeesSB->value())  +
           ",  BeesNumBestSites = "      + std::to_string(Estimation_Tab6_Bees_NumBestBeesSB->value()) +
           ",  BeesNumRepetitions = "    + std::to_string(Estimation_Tab6_NumberOfRunsSB->value()) +
           ",  BeesMaxGenerations = "    + std::to_string(Estimation_Tab6_Bees_MaxGenerationsSB->value()) +
           ",  BeesNeighborhoodSize = "  + std::to_string(Estimation_Tab6_Bees_NeighborhoodSizeSB->value()) +
           ",  NLoptUseStopVal = "       + std::to_string(Estimation_Tab6_NL_StopValCB->isChecked() ? 1 : 0) +
           ",  NLoptUseStopAfterTime = " + std::to_string(Estimation_Tab6_NL_StopAfterTimeCB->isChecked() ? 1 : 0) +
           ",  NLoptUseStopAfterIter = " + std::to_string(Estimation_Tab6_NL_StopAfterIterCB->isChecked() ? 1 : 0) +
           ",  NLoptStopVal = "          + Estimation_Tab6_NL_StopValLE->text().toStdString() +
           ",  NLoptStopAfterTime = "    + std::to_string(Estimation_Tab6_NL_StopAfterTimeSB->value()) +
           ",  NLoptStopAfterIter = "    + std::to_string(Estimation_Tab6_NL_StopAfterIterSB->value()) +
           "   WHERE SystemName = '"     + CurrentSettingsName + "'";

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::SaveSettingsConfiguration: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, "Settings Updated",
                                 "\nSettings in Systems table have been successfully updated.\n");
    }
    saveSettings();

    return true;
}

void
nmfEstimation_Tab6::callback_Estimation_Tab6_EstimationAlgorithmCMB(QString algorithm)
{
    algorithm = Estimation_Tab6_EstimationAlgorithmCMB->currentText();

    bool isBeesAlgorithm  = (algorithm == "Bees Algorithm");
    bool isNLoptAlgorithm = (algorithm == "NLopt Algorithm");
    bool enableMinimizer  = isNLoptAlgorithm;

    if (algorithm.isEmpty()) {
        return;
    }

    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();
    Estimation_Tab6_MinimizerAlgorithmCMB->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerAlgorithmLBL->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerAlgorithmCMB->clear();
    if (isNLoptAlgorithm) {
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_ORIG_DIRECT_L");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_DIRECT_L");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_DIRECT_L_RAND");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_CRS2_LM");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GD_StoGO");
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(0,
            "Global Non-derivative Dividing Rectangles Algorithm with Hard-Coded Limitations", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(1,
            "Global Non-derivative Dividing Rectangles Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(2,
            "Global Non-derivative Dividing Rectangles Algorithm with Randomization", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(3,
            "Controlled Random Search with Local Mutation Algorithm with Evolution similar to Nelder-Mead algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(4,
           "Global Stochastic Search followed by a Local Gradient-based Algorithm", Qt::ToolTipRole);

    }

    // Enable all ObjectiveCriterion
    for (int i=0; i<Estimation_Tab6_ObjectiveCriterionCMB->count(); ++i)
        Estimation_Tab6_ObjectiveCriterionCMB->setItemData(i, 33, Qt::UserRole - 1 ); // Hack to enable all qcombobox items temporarily

    if (isBeesAlgorithm) {
        Estimation_Tab6_Bees_ParametersGB->show();
        Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText("Least Squares"); // override what was in the table
    }  else if (isNLoptAlgorithm) {
        Estimation_Tab6_NL_ParametersGB->show();
        Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText("Least Squares"); // override what was in the table
    }

    emit SetAlgorithm(algorithm);
}

std::string
nmfEstimation_Tab6::getCurrentAlgorithm()
{
    return Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentMinimizer()
{
    return Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentObjectiveCriterion()
{
    return Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString();
}

void
nmfEstimation_Tab6::callback_StopValCB(int isChecked)
{
    Estimation_Tab6_NL_StopValLE->setEnabled(isChecked == Qt::Checked);
}

void
nmfEstimation_Tab6::callback_StopAfterTimeCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterTimeSB->setEnabled(isChecked == Qt::Checked);
}

void
nmfEstimation_Tab6::callback_StopAfterIterCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterIterSB->setEnabled(isChecked == Qt::Checked);
}

void
nmfEstimation_Tab6::refreshMsg(QFont font, QString msg)
{
    readSettings();

    clearOutputTE();
    Estimation_Tab6_RunTE->setFont(font);
    setOutputTE(msg);
}

void
nmfEstimation_Tab6::callback_Estimation_Tab6_FontSizeCMB(QString theFontSize)
{
    m_FontSize = theFontSize.toInt();

    QTextCursor cursor = Estimation_Tab6_RunTE->textCursor();
    Estimation_Tab6_RunTE->selectAll();
    Estimation_Tab6_RunTE->setFontPointSize(m_FontSize);
    Estimation_Tab6_RunTE->setTextCursor( cursor );
}

bool
nmfEstimation_Tab6::loadWidgets()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string algorithm = getCurrentAlgorithm();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::loadWidgets()");

    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();

    if (algorithm == "Bees Algorithm") {
        Estimation_Tab6_Bees_ParametersGB->show();
        Estimation_Tab6_Bees_ParametersGB->setVisible(true);
    } else if (algorithm == "NLopt Algorithm") {
        Estimation_Tab6_NL_ParametersGB->show();
        Estimation_Tab6_NL_ParametersGB->setVisible(true);
    }

    readSettings();

    if (m_ProjectSettingsConfig.empty())
        return false;

    fields     = {"SystemName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","Algorithm","Minimizer",
                  "ObjectiveCriterion","Scaling","GAGenerations","GAPopulationSize",
                  "GAMutationRate","GAConvergence","BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr   = "SELECT SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter ";
    queryStr  += "FROM Systems where SystemName = '";
    queryStr  += m_ProjectSettingsConfig + "'";

    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found in Systems" << std::endl;
        return false;
    }

    Estimation_Tab6_NumberOfRunsSB->setValue(std::stoi(dataMap["NumberOfRuns"][0]));
    Estimation_Tab6_EstimationAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Algorithm"][0]));
    Estimation_Tab6_Bees_NumBeesSB->setValue(std::stoi(dataMap["BeesNumTotal"][0]));
    Estimation_Tab6_Bees_NumEliteSitesSB->setValue(std::stoi(dataMap["BeesNumEliteSites"][0]));
    Estimation_Tab6_Bees_NumBestSitesSB->setValue(std::stoi(dataMap["BeesNumBestSites"][0]));
    Estimation_Tab6_Bees_NumEliteBeesSB->setValue(std::stoi(dataMap["BeesNumElite"][0]));
    Estimation_Tab6_Bees_NumBestBeesSB->setValue(std::stoi(dataMap["BeesNumOther"][0]));
    Estimation_Tab6_NumberOfRunsSB->setValue(std::stoi(dataMap["BeesNumRepetitions"][0]));
    Estimation_Tab6_Bees_MaxGenerationsSB->setValue(std::stoi(dataMap["BeesMaxGenerations"][0]));
    Estimation_Tab6_Bees_NeighborhoodSizeSB->setValue(std::stof(dataMap["BeesNeighborhoodSize"][0]));
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(dataMap["ObjectiveCriterion"][0]));
    Estimation_Tab6_ScalingCMB->setCurrentText(QString::fromStdString(dataMap["Scaling"][0]));
    Estimation_Tab6_NL_StopValCB->setChecked(dataMap["NLoptUseStopVal"][0] == "1");
    Estimation_Tab6_NL_StopAfterTimeCB->setChecked(dataMap["NLoptUseStopAfterTime"][0] == "1");
    Estimation_Tab6_NL_StopAfterIterCB->setChecked(dataMap["NLoptUseStopAfterIter"][0] == "1");
    Estimation_Tab6_NL_StopValLE->setText(QString::fromStdString(dataMap["NLoptStopVal"][0]));
    Estimation_Tab6_NL_StopAfterTimeSB->setValue(std::stoi(dataMap["NLoptStopAfterTime"][0]));
    Estimation_Tab6_NL_StopAfterIterSB->setValue(std::stoi(dataMap["NLoptStopAfterIter"][0]));

    callback_Estimation_Tab6_EstimationAlgorithmCMB(QString::fromStdString(dataMap["Algorithm"][0]));
    Estimation_Tab6_MinimizerAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Minimizer"][0]));

    // RSK Hack to set the CMB correctly. Remove after implementing all of the disabled items in the 3 algorithm CMB's.
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(dataMap["ObjectiveCriterion"][0]));

    return true;
}

void
nmfEstimation_Tab6::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");
    int index;

    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Estimation");
    m_EstimationOutputFile = settings->value("OutputFile","").toString().toStdString();
    m_EstimationDataFile   = settings->value("DataFile","").toString().toStdString();
    m_EstimationID         = settings->value("ID","").toString().toStdString();
    m_FontSize           = settings->value("FontSize",9).toString().toInt();
    m_IsMonospaced       = settings->value("Monospace",0).toString().toInt();
    settings->endGroup();

    delete settings;

    index = Estimation_Tab6_FontSizeCMB->findText(QString::number(m_FontSize));
    Estimation_Tab6_FontSizeCMB->setCurrentIndex(index);
}

void
nmfEstimation_Tab6::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("Monospace",  (int)Estimation_Tab6_MonoCB->isChecked());
    settings->endGroup();

    delete settings;
}
