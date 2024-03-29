#include "nmfSetupTab04.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"


nmfSetup_Tab4::nmfSetup_Tab4(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_databasePtr = databasePtr;
    m_logger      = logger;
    m_ProjectDir  = projectDir;
    m_ModelName.clear();
    m_EstimatedParameters.clear();

    Setup_Tab4_GrowthHighlightPB = nullptr;

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab04.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab4_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab4_Widget->setObjectName("Setup_Tab4_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab4_Widget, tr("4. Model Setup"));

    // Create Model Presets
    m_ModelPresetNames = {"Build Your Own Model","Linear","Logistic","Schaefer-Pella-Tomlinson","GB-Pilot","MS-PROD","AGG-PROD"};
    m_ModelPresets["Build Your Own Model"] = {"Null",     "Null",   "Null",  "Null"};
    m_ModelPresets["Linear"]               = {"Linear",   "Null",   "Null",  "Null"};
    m_ModelPresets["Logistic"]             = {"Logistic", "Null",   "Null",  "Null"};
    m_ModelPresets["Schaefer-Pella-Tomlinson"] = {"Logistic", "Null",   "Catch", "Null"};
    m_ModelPresets["GB-Pilot"]             = {"Linear",   "Null",   "Catch", "NO_K"};
    m_ModelPresets["MS-PROD"]              = {"Logistic", "Type I", "Exploitation (F)", "MS-PROD"};
    m_ModelPresets["AGG-PROD"]             = {"Logistic", "Type I", "Exploitation (F)", "AGG-PROD"};

    Setup_Tab4_ModelNameLE              = Setup_Tabs->findChild<QLineEdit    *>("Setup_Tab4_ModelNameLE");
    Setup_Tab4_SystemCarryingCapacityLE = Setup_Tabs->findChild<QLineEdit    *>("Setup_Tab4_SystemCarryingCapacityLE");
    Setup_Tab4_ModelEquationTE          = Setup_Tabs->findChild<QTextEdit    *>("Setup_Tab4_ModelEquationTE");
    Setup_Tab4_FontSizeCMB              = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_FontSizeCMB");
    Setup_Tab4_GrowthHighlightPB        = Setup_Tabs->findChild<QPushButton  *>("GrowthHighlightPB");
    Setup_Tab4_HarvestHighlightPB       = Setup_Tabs->findChild<QPushButton  *>("HarvestHighlightPB");
    Setup_Tab4_PredationHighlightPB     = Setup_Tabs->findChild<QPushButton  *>("PredationHighlightPB");
    Setup_Tab4_CompetitionHighlightPB   = Setup_Tabs->findChild<QPushButton  *>("CompetitionHighlightPB");
    Setup_Tab4_ModelNewPB               = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_ModelNewPB");
    Setup_Tab4_ModelCopyPB              = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_ModelCopyPB");
    Setup_Tab4_ModelDelPB               = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_ModelDelPB");
    Setup_Tab4_StartYearSB              = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab4_StartYearSB");
    Setup_Tab4_EndYearLE                = Setup_Tabs->findChild<QLineEdit    *>("Setup_Tab4_EndYearLE");
    Setup_Tab4_RunLengthSB              = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab4_RunLengthSB");
    Setup_Tab4_ModelPresetsLBL          = Setup_Tabs->findChild<QLabel       *>("Setup_Tab4_ModelPresetsLBL");
    Setup_Tab4_ModelPresetsCMB          = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_ModelPresetsCMB");
    Setup_Tab4_GrowthFormCMB            = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_GrowthFormCMB");
    Setup_Tab4_PredationFormCMB         = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_PredationFormCMB");
    Setup_Tab4_HarvestFormCMB           = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_HarvestFormCMB");
    Setup_Tab4_CompetitionFormCMB       = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_CompetitionFormCMB");
    Setup_Tab4_ObsBiomassTypeCMB        = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_ObsBiomassTypeCMB");
    Setup_Tab4_PrevPB                   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_PrevPB");
    Setup_Tab4_NextPB                   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_NextPB");
    Setup_Tab4_LoadPB                   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_LoadPB");
    Setup_Tab4_SavePB                   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_SavePB");
    Setup_Tab4_CalcPB                   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_CalcPB");

    // RSK - Just do this temporarily until functionality is complete
//    Setup_Tab4_ModelCopyPB->setVisible(false);

    // Set widget parameters
    Setup_Tab4_LoadPB->show();
    Setup_Tab4_LoadPB->setEnabled(true);
    Setup_Tab4_SavePB->setEnabled(true);
    Setup_Tab4_PrevPB->setText("\u25C1--");
    Setup_Tab4_NextPB->setText("--\u25B7");
//  Setup_Tab4_ModelNameLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColor);
//  Setup_Tab4_EndYearLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColor);

    // Get and set the highlight colors
    setHighlightColors();

    connect(Setup_Tab4_ModelPresetsCMB,        SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_ModelPresetsCMB(QString)));
    connect(Setup_Tab4_GrowthFormCMB,          SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_GrowthFormCMB(QString)));
    connect(Setup_Tab4_PredationFormCMB,       SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_PredationFormCMB(QString)));
    connect(Setup_Tab4_HarvestFormCMB,         SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_HarvestFormCMB(QString)));
    connect(Setup_Tab4_CompetitionFormCMB,     SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_CompetitionFormCMB(QString)));
    connect(Setup_Tab4_ObsBiomassTypeCMB,     SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_ObsBiomassTypeCMB(QString)));
    connect(Setup_Tab4_FontSizeCMB,            SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_FontSizeCMB(QString)));
    connect(Setup_Tab4_GrowthHighlightPB,      SIGNAL(clicked()),
            this,                              SLOT(callback_GrowthHighlightPB()));
    connect(Setup_Tab4_HarvestHighlightPB,     SIGNAL(clicked()),
            this,                              SLOT(callback_HarvestHighlightPB()));
    connect(Setup_Tab4_PredationHighlightPB,   SIGNAL(clicked()),
            this,                              SLOT(callback_PredationHighlightPB()));
    connect(Setup_Tab4_CompetitionHighlightPB, SIGNAL(clicked()),
            this,                              SLOT(callback_CompetitionHighlightPB()));
    connect(Setup_Tab4_ModelNewPB,             SIGNAL(clicked()),
            this,                              SLOT(callback_ModelNewPB()));
    connect(Setup_Tab4_ModelCopyPB,            SIGNAL(clicked()),
            this,                              SLOT(callback_ModelCopyPB()));
    connect(Setup_Tab4_ModelDelPB,             SIGNAL(clicked()),
            this,                              SLOT(callback_ModelDelPB()));
    connect(Setup_Tab4_CalcPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_CalcPB()));
    connect(Setup_Tab4_LoadPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_LoadPB()));
    connect(Setup_Tab4_SavePB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_SavePB()));
    connect(Setup_Tab4_PrevPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_PrevPB()));
    connect(Setup_Tab4_NextPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_NextPB()));
    connect(Setup_Tab4_StartYearSB,            SIGNAL(valueChanged(int)),
            this,                              SLOT(callback_UpdateEndYear(int)));
    connect(Setup_Tab4_RunLengthSB,            SIGNAL(valueChanged(int)),
            this,                              SLOT(callback_UpdateEndYear(int)));

    // Temporarily disable AGG-PROD - Fix this for v2.0
    QStandardItemModel* model;
    QStandardItem*      item;
    model = qobject_cast<QStandardItemModel*>(Setup_Tab4_CompetitionFormCMB->model());
    item  = model->item(3);
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    model = qobject_cast<QStandardItemModel*>(Setup_Tab4_ModelPresetsCMB->model());
    item  = model->item(6);
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);

}

nmfSetup_Tab4::~nmfSetup_Tab4()
{
}

QString
nmfSetup_Tab4::getModelName()
{
    return Setup_Tab4_ModelNameLE->text();
}

void
nmfSetup_Tab4::setStartYear(int StartYear)
{
    Setup_Tab4_StartYearSB->setValue(StartYear);
}

void
nmfSetup_Tab4::setRunLength(int RunLength)
{
    Setup_Tab4_RunLengthSB->setValue(RunLength+1);
}

int
nmfSetup_Tab4::getRunLength()
{
    return Setup_Tab4_RunLengthSB->value()-1;
}

void
nmfSetup_Tab4::uncheckHighlightButtons()
{
    Setup_Tab4_GrowthHighlightPB->setChecked(false);
    Setup_Tab4_PredationHighlightPB->setChecked(false);;
    Setup_Tab4_HarvestHighlightPB->setChecked(false);
    Setup_Tab4_CompetitionHighlightPB->setChecked(false);
    emit RedrawEquation();
}

void
nmfSetup_Tab4::setHighlightColors()
{
    QString growthHighlightColorStyle;
    QString harvestHighlightColorStyle;
    QString competitionHighlightColorStyle;
    QString predationHighlightColorStyle;
    QString growthColor;
    QString harvestColor;
    QString competitionColor;
    QString predationColor;

    getHighlightColors(growthHighlightColorStyle,
                       harvestHighlightColorStyle,
                       competitionHighlightColorStyle,
                       predationHighlightColorStyle,
                       growthColor,
                       harvestColor,
                       competitionColor,
                       predationColor);

    Setup_Tab4_GrowthHighlightPB->setToolTip("Highlight Growth Term ("+growthColor+")");
    Setup_Tab4_GrowthHighlightPB->setStatusTip("Highlight Growth Term ("+growthColor+")");
    Setup_Tab4_HarvestHighlightPB->setToolTip("Highlight Harvest Term ("+harvestColor+")");
    Setup_Tab4_HarvestHighlightPB->setStatusTip("Highlight Harvest Term ("+harvestColor+")");
    Setup_Tab4_CompetitionHighlightPB->setToolTip("Highlight Competition Term ("+competitionColor+")");
    Setup_Tab4_CompetitionHighlightPB->setStatusTip("Highlight Competition Term ("+competitionColor+")");
    Setup_Tab4_PredationHighlightPB->setToolTip("Highlight PredationTerm ("+predationColor+")");
    Setup_Tab4_PredationHighlightPB->setStatusTip("Highlight Predation Term ("+predationColor+")");

    Setup_Tab4_GrowthHighlightPB->setStyleSheet(growthHighlightColorStyle);
    Setup_Tab4_HarvestHighlightPB->setStyleSheet(harvestHighlightColorStyle);
    Setup_Tab4_CompetitionHighlightPB->setStyleSheet(competitionHighlightColorStyle);
    Setup_Tab4_PredationHighlightPB->setStyleSheet(predationHighlightColorStyle);
}

void
nmfSetup_Tab4::setIsDark(QString style)
{
    if (style == "Dark"){
        m_isDark = true;
    } else {
        m_isDark = false;
    }

}

void
nmfSetup_Tab4::setLineEditColors()
{
    if (m_isDark){
        nmfUtilsQt::setBackgroundLineEdit(Setup_Tab4_ModelNameLE,nmfUtilsQt::ReadOnlyLineEditBgColorDark);
        nmfUtilsQt::setBackgroundLineEdit(Setup_Tab4_EndYearLE,  nmfUtilsQt::ReadOnlyLineEditBgColorDark);
    } else {
        nmfUtilsQt::setBackgroundLineEdit(Setup_Tab4_ModelNameLE,nmfUtilsQt::ReadOnlyLineEditBgColorLight);
        nmfUtilsQt::setBackgroundLineEdit(Setup_Tab4_EndYearLE,  nmfUtilsQt::ReadOnlyLineEditBgColorLight);
    }
}

void
nmfSetup_Tab4::getHighlightColors(QString& growthHighlightColor,
                                  QString& harvestHighlightColor,
                                  QString& competitionHighlightColor,
                                  QString& predationHighlightColor,
                                  QString& growthColorName,
                                  QString& harvestColorName,
                                  QString& competitionColorName,
                                  QString& predationColorName)
{
    if (qApp->styleSheet().isEmpty()) { // Means it's Light
        growthHighlightColor      = "QPushButton:checked { background-color: yellow;           }";
        harvestHighlightColor     = "QPushButton:checked { background-color: gold;             }";
        competitionHighlightColor = "QPushButton:checked { background-color: aqua;             }";
        predationHighlightColor   = "QPushButton:checked { background-color: LightGreen;       }";
        growthColorName      = "yellow";
        harvestColorName     = "gold";
        competitionColorName = "aqua";
        predationColorName   = "green";
    } else { // Means it's Dark
        growthHighlightColor      = "QPushButton:checked { background-color: blue;             }";
        harvestHighlightColor     = "QPushButton:checked { background-color: rgb(130,147, 86); }";
        competitionHighlightColor = "QPushButton:checked { background-color: rgb(128,  0,128); }";
        predationHighlightColor   = "QPushButton:checked { background-color: gray;             }";
        growthColorName      = "blue";
        harvestColorName     = "green";
        competitionColorName = "purple";
        predationColorName   = "gray";
    }
}


QComboBox*
nmfSetup_Tab4::getModelPresetsCMB()
{
    return Setup_Tab4_ModelPresetsCMB;
}

QComboBox*
nmfSetup_Tab4::getGrowthFormCMB()
{
    return Setup_Tab4_GrowthFormCMB;
}
QComboBox*
nmfSetup_Tab4::getPredationFormCMB()
{
    return Setup_Tab4_PredationFormCMB;
}
QComboBox*
nmfSetup_Tab4::getHarvestFormCMB()
{
    return Setup_Tab4_HarvestFormCMB;
}
QComboBox*
nmfSetup_Tab4::getCompetitionFormCMB()
{
    return Setup_Tab4_CompetitionFormCMB;
}

QString
nmfSetup_Tab4::getEquation()
{
   return Setup_Tab4_ModelEquationTE->toHtml();
}

void
nmfSetup_Tab4::drawEquation(QString Label, QString Eqn, QString Key)
{
    Setup_Tab4_ModelEquationTE->setText("<p><br/>"+Label+"<br/><br/>"+Eqn+"<br/><br/>"+Key+"</p>");
    callback_FontSizeCMB(Setup_Tab4_FontSizeCMB->currentText());
}


void
nmfSetup_Tab4::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab4::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    settings->setValue("Name", QString::fromStdString(m_ModelName));
    settings->endGroup();

    settings->beginGroup("SetupTab");
    settings->setValue("FontSize", getFontSize());
    settings->endGroup();

    delete settings;

//    emit SaveMainSettings();
}

void
nmfSetup_Tab4::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab4::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab4::loadModel()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    ModelData data;

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"Loading: "+m_ModelName);
    fields     = {"ProjectName","ModelName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","GAGenerations","GAPopulationSize","GAMutationRate","GAConvergence",
                  "Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                  "BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "GradMaxIterations","GradMaxLineSearches",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr   = "SELECT ProjectName,ModelName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,";
    queryStr  += "BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "GradMaxIterations,GradMaxLineSearches,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter FROM " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ModelName"].size() == 0) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetupTab4::callback_Setup_Tab4_LoadPB: No data found in Models table");
        m_logger->logMsg(nmfConstants::Error,queryStr);
        return;
    }

    data.ProjectName           = dataMap["ProjectName"][0];
    data.ModelName             = dataMap["ModelName"][0];
    data.CarryingCapacity      = dataMap["CarryingCapacity"][0];
    data.GrowthForm            = dataMap["GrowthForm"][0];
    data.HarvestForm           = dataMap["HarvestForm"][0];
    data.CompetitionForm       = dataMap["WithinGuildCompetitionForm"][0];
    data.PredationForm         = dataMap["PredationForm"][0];
    data.NumberOfRuns          = std::stoi(dataMap["NumberOfRuns"][0]);
    data.StartYear             = std::stoi(dataMap["StartYear"][0]);
    data.RunLength             = std::stoi(dataMap["RunLength"][0]);
    data.TimeStep              = std::stoi(dataMap["TimeStep"][0]);
    data.NumGenerations        = std::stoi(dataMap["GAGenerations"][0]);
    data.PopulationSize        = std::stoi(dataMap["GAPopulationSize"][0]);
    data.MutationRate          = std::stod(dataMap["GAMutationRate"][0]);
    data.ConvergenceNum        = std::stoi(dataMap["GAConvergence"][0]);
    data.Algorithm             = dataMap["Algorithm"][0];
    data.Minimizer             = dataMap["Minimizer"][0];
    data.ObjectiveCriterion    = dataMap["ObjectiveCriterion"][0];
    data.Scaling               = dataMap["Scaling"][0];
    data.BeesNumTotal          = std::stoi(dataMap["BeesNumTotal"][0]);
    data.BeesNumElite          = std::stoi(dataMap["BeesNumElite"][0]);
    data.BeesNumOther          = std::stoi(dataMap["BeesNumOther"][0]);
    data.BeesNumEliteSites     = std::stoi(dataMap["BeesNumEliteSites"][0]);
    data.BeesNumBestSites      = std::stoi(dataMap["BeesNumBestSites"][0]);
    data.BeesNumRepetitions    = std::stoi(dataMap["BeesNumRepetitions"][0]);
    data.BeesMaxGenerations    = std::stoi(dataMap["BeesMaxGenerations"][0]);
    data.BeesNeighborhoodSize  = std::stof(dataMap["BeesNeighborhoodSize"][0]);
    data.NLoptStopVal          = std::stod(dataMap["NLoptStopVal"][0]);
    data.NLoptStopAfterTime    = std::stoi(dataMap["NLoptStopAfterTime"][0]);
    data.NLoptStopAfterIter    = std::stoi(dataMap["NLoptStopAfterIter"][0]);
    data.NLoptUseStopVal       = std::stoi(dataMap["NLoptUseStopVal"][0]);
    data.NLoptUseStopAfterTime = std::stoi(dataMap["NLoptUseStopAfterTime"][0]);
    data.NLoptUseStopAfterIter = std::stoi(dataMap["NLoptUseStopAfterIter"][0]);

    if (data.StartYear < nmfConstantsMSSPM::Start_Year)
        data.StartYear = nmfConstantsMSSPM::Start_Year;

//    m_LoadDialog->getSettingData(data);

    Setup_Tab4_ModelNameLE->setText(QString::fromStdString(data.ModelName));
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::fromStdString(data.CarryingCapacity));
    Setup_Tab4_GrowthFormCMB->setCurrentText(QString::fromStdString(data.GrowthForm));
    Setup_Tab4_HarvestFormCMB->setCurrentText(QString::fromStdString(data.HarvestForm));
    Setup_Tab4_CompetitionFormCMB->setCurrentText(QString::fromStdString(data.CompetitionForm));
    Setup_Tab4_PredationFormCMB->setCurrentText(QString::fromStdString(data.PredationForm));
    Setup_Tab4_StartYearSB->setValue(data.StartYear);
    setRunLength(data.RunLength);
    Setup_Tab4_EndYearLE->setText(QString::number(data.StartYear+data.RunLength));

    m_ModelName = data.ModelName;

    // Save Algorithm, Minimizer, and ObjectiveFunction back into table so user can run without
    // hitting Save.....
    std::string cmd;
    std::string errorMsg;

    cmd  = "UPDATE " + nmfConstantsMSSPM::TableModels + " SET";
    cmd += "  Algorithm = '"          + data.Algorithm + "'";
    cmd += ", Minimizer = '"          + data.Minimizer + "'";
    cmd += ", ObjectiveCriterion = '" + data.ObjectiveCriterion + "'";
    cmd += ", Scaling = '"            + data.Scaling + "'";
    cmd += " WHERE ProjectName = '" + m_ProjectName +
           "' AND ModelName = '"    + m_ModelName + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetupTab4 callback_Setup_Tab4_LoadPB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }

    saveSettings();

    setEstimatedParameterNames();

    emit ModelLoaded();
    emit UpdateDiagnosticParameterChoices();
}

void
nmfSetup_Tab4::callback_LoadPB()
{
    m_LoadDialog = new LoadDlg(tr("Load Settings"), Setup_Tabs,
                             m_logger, m_databasePtr, m_ProjectName,
                             QString::fromStdString(m_ModelName));

    connect(m_LoadDialog, SIGNAL(ClearModelName()),
            this,         SLOT(callback_ClearModelName()));

    if (m_LoadDialog->exec() == QDialog::Accepted) {
        loadModel();
        QMessageBox::information(Setup_Tabs, "Load", "\nSuccessfully loaded Model.\n");
        emit ModelSaved();
    }
}


int
nmfSetup_Tab4::getFontSize()
{
    return Setup_Tab4_FontSizeCMB->currentText().toInt();
}

void
nmfSetup_Tab4::setFontSize(int fontSize)
{
    if (Setup_Tab4_FontSizeCMB != NULL) {
        int index = Setup_Tab4_FontSizeCMB->findText(QString::number(fontSize));
        Setup_Tab4_FontSizeCMB->setCurrentIndex(index);
    }
}

double
nmfSetup_Tab4::calculateSystemCarryingCapacity()
{
    int NumRecords;
    double SystemK = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    if (isAggProd()) {
        fields     = {"GuildName","GuildK"};
        queryStr   = "SELECT GuildName,GuildK FROM " + nmfConstantsMSSPM::TableGuilds;
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["GuildName"].size();
        for (int i=0; i<NumRecords; ++i) {
            SystemK += std::stod(dataMap["GuildK"][i]);
        }
    } else {
        fields     = {"SpeName","SpeciesK"};
        queryStr   = "SELECT SpeName,SpeciesK FROM " + nmfConstantsMSSPM::TableSpecies;
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        for (int i=0; i<NumRecords; ++i) {
            SystemK += std::stod(dataMap["SpeciesK"][i]);
        }
    }
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::number(SystemK));

    m_logger->logMsg(nmfConstants::Normal,"Setting SystemK to: "+QString::number(SystemK).toStdString());

    return SystemK;
}


void
nmfSetup_Tab4::callback_ModelDelPB()
{
    QMessageBox::StandardButton reply;
    QString msg;
    QString currentModelName = Setup_Tab4_ModelNameLE->text();

    if (currentModelName.isEmpty()) {
        QMessageBox::warning(Setup_Tabs,
                             tr("Warning"),
                             tr("Cannot delete a Model Name that's blank."),
                             QMessageBox::Ok);
        return;
    }

    msg = "\nOK to delete all data associated with Model: " + currentModelName + " ?";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete Model"),
                                  tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        deleteModel(currentModelName);
        QMessageBox::information(Setup_Tabs, "Delete", "\nSuccessfully deleted Model.\n");
        emit ModelDeleted();
    }
}

void
nmfSetup_Tab4::deleteModel(QString modelToDelete)
{
    m_databasePtr->cleanTables(m_logger,m_ProjectName,modelToDelete.toStdString());

    callback_ClearModelName();
}

void
nmfSetup_Tab4::callback_ClearModelName()
{
    Setup_Tab4_ModelNameLE->setText("");
}

void
nmfSetup_Tab4::callback_SavePB()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::callback_SavePB start");

    saveModel(nmfConstantsMSSPM::RunChecks);

    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::callback_SavePB end");
}

QStringList
nmfSetup_Tab4::getEstimatedParameterNames()
{
    return m_EstimatedParameters;
}

void
nmfSetup_Tab4::setEstimatedParameterNames()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::setEstimatedParameterNames start");

    nmfStructsQt::EstimateRunBox runBox;

    std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes;

    QString growthForm      = getGrowthFormCMB()->currentText();
    QString harvestForm     = getHarvestFormCMB()->currentText();
    QString competitionForm = getCompetitionFormCMB()->currentText();
    QString predationForm   = getPredationFormCMB()->currentText();
    QString obsBiomassType  = getObsBiomassType();

    m_EstimatedParameters.clear();

    // Set Initial Absolute Biomass
    m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameInitialBiomass);
    runBox.parameter = "InitBiomass";
    runBox.state     = std::make_pair(true,true);
    EstimateRunBoxes.push_back(runBox);

    // Set SurveyQ
    if (obsBiomassType == "Relative") {
        runBox.parameter = "SurveyQ";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameSurveyQ);
        EstimateRunBoxes.push_back(runBox);
    }

    // Load Growth estimated parameters
    if (growthForm == "Linear") {
        runBox.parameter = "GrowthRate";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameGrowthRate);
        EstimateRunBoxes.push_back(runBox);
    } else if (growthForm == "Logistic") {
        runBox.parameter = "GrowthRate";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameGrowthRate);
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "GrowthRateShape";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameGrowthRateShape);
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "CarryingCapacity";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCarryingCapacity);
        EstimateRunBoxes.push_back(runBox);
    }

    // Load Harvest estimated parameters
    if ((harvestForm == "Effort (qE)") || (harvestForm == "Effort Fit to Catch")) {
        runBox.parameter = "Catchability";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCatchability);
        EstimateRunBoxes.push_back(runBox);
    }

    // Load Competition estimated parameters
    if (competitionForm == "NO_K") {
        runBox.parameter = "CompetitionAlpha";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCompetitionAlpha);
        EstimateRunBoxes.push_back(runBox);
    } else if (competitionForm == "MS-PROD") {
        runBox.parameter = "CompetitionAlpha";
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCompetitionBetaSpecies);
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCompetitionBetaGuilds);
        runBox.parameter = "CompetitionBetaSpeciesSpecies";
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "CompetitionBetaGuildSpecies";
        EstimateRunBoxes.push_back(runBox);
    } else if (competitionForm == "AGG-PROD") {
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNameCompetitionBetaGuildsGuilds);
        runBox.parameter = "CompetitionBetaGuildGuild";
        EstimateRunBoxes.push_back(runBox);
    }

    // Load Predation estimated parameters
    if (predationForm == "Type I") {
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationRho);
        runBox.parameter = "PredationRho";
        EstimateRunBoxes.push_back(runBox);
    } else if (predationForm == "Type II") {
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationRho);
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationHandling);
        runBox.parameter = "PredationRho";
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "PredationHandling";
        EstimateRunBoxes.push_back(runBox);
    } else if (predationForm == "Type III") {
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationRho);
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationHandling);
        m_EstimatedParameters.push_back(nmfConstantsMSSPM::ParameterNamePredationExponent);
        runBox.parameter = "PredationRho";
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "PredationHandling";
        EstimateRunBoxes.push_back(runBox);
        runBox.parameter = "PredationExponent";
        EstimateRunBoxes.push_back(runBox);
    }

    emit SetEstimateRunCheckboxes(EstimateRunBoxes);
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::setEstimatedParameterNames end");

}

void
nmfSetup_Tab4::reloadModelName()
{
    QString ModelName = Setup_Tab4_ModelNameLE->text();
    readSettings();

    if (ModelName.isEmpty() || ModelName.contains("__")) {
        Setup_Tab4_ModelNameLE->setText(QString::fromStdString(m_ModelName));
    }
}

void
nmfSetup_Tab4::saveModel(bool RunChecks)
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::saveModel start");

    bool okToSave = true;
    std::string msg;
    std::string ModelName = Setup_Tab4_ModelNameLE->text().toStdString();

    calculateSystemCarryingCapacity();

    if (RunChecks && ModelName.empty()) {
        msg = "\nError: A Model Name must be given prior to saving.\n";
        m_logger->logMsg(nmfConstants::Error,"");
        QMessageBox::critical(Setup_Tabs, "Save Error",QString::fromStdString(msg));
        return;
    }

    if (RunChecks) {
        msg = "\nOK to save current settings as: " + ModelName + " ?";
        QMessageBox::StandardButton reply = QMessageBox::question(Setup_Tabs, tr("Save"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes,
                                                                  QMessageBox::Yes);
        okToSave = (reply == QMessageBox::Yes);
    }
    if (okToSave) {
        m_ModelName = ModelName;
        saveModelData(RunChecks,ModelName);
        readSettings();
    }

    emit SaveEstimationRunSettings();
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::saveModel emitting UpdateInitialObservedBiomass");
    emit UpdateInitialObservedBiomass(getObsBiomassType());
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::saveModel emitting UpdateInitialForecastYear");
    emit UpdateInitialForecastYear();
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::saveModel emitting ReloadWidgets");
    emit ReloadWidgets(nmfConstantsMSSPM::DontReloadProject);
    emit ReloadTab7Widgets();
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::saveModel end");
    emit UpdateWindowTitle();

}


bool
nmfSetup_Tab4::saveModelData(bool verbose,std::string currentModelName)
{
    std::string cmd;
    std::string errorMsg;
    int NumberOfParameters      = 0;
    std::string SystemK         = Setup_Tab4_SystemCarryingCapacityLE->text().toStdString();
    std::string GrowthForm      = Setup_Tab4_GrowthFormCMB->currentText().toStdString();
    std::string PredationForm   = Setup_Tab4_PredationFormCMB->currentText().toStdString();
    std::string HarvestForm     = Setup_Tab4_HarvestFormCMB->currentText().toStdString();
    std::string CompetitionForm = Setup_Tab4_CompetitionFormCMB->currentText().toStdString();
    std::string ObsBiomassType  = Setup_Tab4_ObsBiomassTypeCMB->currentText().toStdString();
    std::string StartYear       = std::to_string(Setup_Tab4_StartYearSB->value());
    std::string RunLength       = std::to_string(getRunLength());
    std::string CovariateAlgorithmTypeDefault = nmfConstantsMSSPM::Linear;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    if (GrowthForm == "Linear")
        NumberOfParameters += 1;
    else if (GrowthForm == "Logistic")
        NumberOfParameters += 2;
    if (HarvestForm == "Exploitation (F)")
        NumberOfParameters += 1;
    if (CompetitionForm != "Null")
        NumberOfParameters += 1;
    if (PredationForm == "Type I")
        NumberOfParameters += 1;
    else if (PredationForm == "Type II")
        NumberOfParameters += 2;
    else if (PredationForm == "Type III")
        NumberOfParameters += 3;

    // For now, assume initial absolute biomass is always estimated (adjust this assumption in Estimation Tab7)
    NumberOfParameters += 1;

    // Check if Model exists, if so Update else REPLACE
    fields   = {"ProjectName","ModelName"};
    queryStr = "SELECT ProjectName,ModelName from " +
                nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName    +
               "' AND ModelName = '"    + currentModelName + "'";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ModelName"].size() != 0) { // This means the model name exists so do an update
        cmd  = "UPDATE " + nmfConstantsMSSPM::TableModels + " SET";
        cmd += "   ProjectName = '"                + m_ProjectName +
               "', ModelName = '"                  + currentModelName +
               "', CarryingCapacity = "            + SystemK +
               ",  ObsBiomassType = '"             + ObsBiomassType +
               "', GrowthForm = '"                 + GrowthForm +
               "', PredationForm = '"              + PredationForm +
               "', HarvestForm = '"                + HarvestForm +
               "', WithinGuildCompetitionForm = '" + CompetitionForm +
               "', NumberOfParameters = "          + std::to_string(NumberOfParameters) +
               ",  StartYear = "                   + StartYear +
               ",  RunLength = "                   + RunLength +
               ",  CovariateAlgorithmType = '"     + CovariateAlgorithmTypeDefault +
               "' WHERE ModelName = '"             + currentModelName +
               "' AND ProjectName = '"             + m_ProjectName + "'";
    } else { // This means the system name does not exist so do a replace
        cmd  = "REPLACE INTO " + nmfConstantsMSSPM::TableModels +
               " (ProjectName,ModelName,CarryingCapacity,ObsBiomassType,GrowthForm,PredationForm,HarvestForm,";
        cmd += "WithinGuildCompetitionForm,StartYear,RunLength,CovariateAlgorithmType,NumberOfParameters) ";
        cmd += "VALUES ('" +
                m_ProjectName                      + "','"  +
                currentModelName                   + "', "  +
                SystemK                            + ", '"  +
                ObsBiomassType                     + "', '" +
                GrowthForm                         + "', '" +
                PredationForm                      + "', '" +
                HarvestForm                        + "', '" +
                CompetitionForm                    + "', "  +
                StartYear                          + ", "   +
                RunLength                          + ", '"  +
                CovariateAlgorithmTypeDefault      + "', "  +
                std::to_string(NumberOfParameters) + " );";
    }

    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::saveModelData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }
    if (verbose) {
        QString msg = "\nSettings in Models database table have been successfully updated.\n";
        msg += "\nReminder:\n\nPlease reset the Covariate Assignment and Covariate Initial Value & Ranges tables.\n";
        QMessageBox::information(Setup_Tabs, "Settings Updated",msg);
    }
    saveSettings();

    return true;
}

void
nmfSetup_Tab4::updateOutputWidget()
{

}

void
nmfSetup_Tab4::loadWidgets()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string GrowthStr;
    std::string HarvestStr;
    std::string CompetitionStr;
    std::string PredationStr;

    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::loadWidgets()");

    readSettings();
    if (m_ModelName.empty())
        return;

    clearWidgets();

    fields     = {"ProjectName","ModelName","CarryingCapacity","ObsBiomassType","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","Algorithm","Minimizer",
                  "ObjectiveCriterion","Scaling","GAGenerations","GAPopulationSize",
                  "GAMutationRate","GAConvergence","BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr   = "SELECT ProjectName,ModelName,CarryingCapacity,ObsBiomassType,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter ";
    queryStr  += "FROM " + nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName   + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: " + nmfConstantsMSSPM::TableModels << std::endl;
        std::cout << queryStr << std::endl;
        return;
    }

    GrowthStr      = dataMap["GrowthForm"][0];
    HarvestStr     = dataMap["HarvestForm"][0];
    CompetitionStr = dataMap["WithinGuildCompetitionForm"][0];
    PredationStr   = dataMap["PredationForm"][0];
    if (GrowthStr.empty())      GrowthStr      = "Null";
    if (HarvestStr.empty())     HarvestStr     = "Null";
    if (CompetitionStr.empty()) CompetitionStr = "Null";
    if (PredationStr.empty())   PredationStr   = "Null";

    // Load widgets with saved table data
    Setup_Tab4_ModelNameLE->setText(QString::fromStdString(dataMap["ModelName"][0]));
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::fromStdString(dataMap["CarryingCapacity"][0]));
    Setup_Tab4_GrowthFormCMB->setCurrentText(QString::fromStdString(GrowthStr));
    Setup_Tab4_HarvestFormCMB->setCurrentText(QString::fromStdString(HarvestStr));
    Setup_Tab4_CompetitionFormCMB->setCurrentText(QString::fromStdString(CompetitionStr));
    Setup_Tab4_PredationFormCMB->setCurrentText(QString::fromStdString(PredationStr));
    Setup_Tab4_StartYearSB->setValue(std::stoi(dataMap["StartYear"][0]));
    setRunLength(std::stoi(dataMap["RunLength"][0]));
    Setup_Tab4_EndYearLE->setText(QString::number(Setup_Tab4_StartYearSB->value()+getRunLength()));
    Setup_Tab4_ObsBiomassTypeCMB->setCurrentIndex(-1);
    Setup_Tab4_ObsBiomassTypeCMB->setCurrentText(QString::fromStdString(dataMap["ObsBiomassType"][0]));

    setEstimatedParameterNames();

    emit RedrawEquation();
}


void
nmfSetup_Tab4::clearWidgets()
{
    Setup_Tab4_ModelNameLE->clear();
    Setup_Tab4_SystemCarryingCapacityLE->clear();
    Setup_Tab4_StartYearSB->clear();
    Setup_Tab4_EndYearLE->clear();
    Setup_Tab4_RunLengthSB->clear();
    Setup_Tab4_ModelEquationTE->clear();
}


void
nmfSetup_Tab4::callback_ModelPresetsCMB(QString name)
{
    std::vector<std::string> ModelPreset = m_ModelPresets[name.toStdString()];

    Setup_Tab4_GrowthFormCMB->blockSignals(true);
    Setup_Tab4_HarvestFormCMB->blockSignals(true);
    Setup_Tab4_CompetitionFormCMB->blockSignals(true);
    Setup_Tab4_PredationFormCMB->blockSignals(true);

    Setup_Tab4_GrowthFormCMB->setCurrentText(QString::fromStdString(ModelPreset[0]));
    Setup_Tab4_HarvestFormCMB->setCurrentText(QString::fromStdString(ModelPreset[2]));
    Setup_Tab4_CompetitionFormCMB->setCurrentText(QString::fromStdString(ModelPreset[3]));
    Setup_Tab4_PredationFormCMB->setCurrentText(QString::fromStdString(ModelPreset[1]));

    Setup_Tab4_GrowthFormCMB->blockSignals(false);
    Setup_Tab4_HarvestFormCMB->blockSignals(false);
    Setup_Tab4_CompetitionFormCMB->blockSignals(false);
    Setup_Tab4_PredationFormCMB->blockSignals(false);
}

void
nmfSetup_Tab4::setModelName(QString modelName)
{
    m_ModelName = modelName.toStdString();
    Setup_Tab4_ModelNameLE->setText(modelName);
    saveSettings();
}

void
nmfSetup_Tab4::setModelPreset(std::string modelName)
{
    Setup_Tab4_ModelPresetsCMB->blockSignals(true);
    Setup_Tab4_ModelPresetsCMB->setCurrentText(QString::fromStdString(modelName));
    Setup_Tab4_ModelPresetsCMB->blockSignals(false);
}

void
nmfSetup_Tab4::callback_GrowthFormCMB(QString growthForm)
{
    int i=0;
    int NumPresets = m_ModelPresets.size();
    bool done = false;
    std::vector<std::string> ModelPreset;

    while (!done and (i<NumPresets)) {
        for (std::string ModelName : m_ModelPresetNames) {
            ModelPreset = m_ModelPresets[ModelName];
            if  ((ModelPreset[0] == growthForm.toStdString()) &&
                 (ModelPreset[1] == Setup_Tab4_PredationFormCMB->currentText().toStdString()) &&
                 (ModelPreset[2] == Setup_Tab4_HarvestFormCMB->currentText().toStdString()) &&
                 (ModelPreset[3] == Setup_Tab4_CompetitionFormCMB->currentText().toStdString())) {
                setModelPreset(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelPreset("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_HarvestFormCMB(QString harvestForm)
{
    int i=0;
    int NumPresets = m_ModelPresets.size();
    bool done = false;
    std::vector<std::string> ModelPreset;

    // Change the Estimation->Harvest Data table to correspond with the selected harvestForm
    harvestForm = harvestForm.split(" ")[0];

    while (!done and (i<NumPresets)) {
        for (std::string ModelName : m_ModelPresetNames) {
            ModelPreset = m_ModelPresets[ModelName];
            if  ((ModelPreset[0] == Setup_Tab4_GrowthFormCMB->currentText().toStdString()) &&
                 (ModelPreset[1] == Setup_Tab4_PredationFormCMB->currentText().toStdString()) &&
                 (ModelPreset[2] == harvestForm.toStdString()) &&
                 (ModelPreset[3] == Setup_Tab4_CompetitionFormCMB->currentText().toStdString())) {
                setModelPreset(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelPreset("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_CompetitionFormCMB(QString competitionForm)
{
    int i=0;
    int NumPresets = m_ModelPresets.size();
    bool done = false;
    std::vector<std::string> ModelPreset;

    emit CompetitionFormChanged(competitionForm);

    while (!done and (i<NumPresets)) {
        for (std::string ModelName : m_ModelPresetNames) {
            ModelPreset = m_ModelPresets[ModelName];
            if  ((ModelPreset[0] == Setup_Tab4_GrowthFormCMB->currentText().toStdString()) &&
                 (ModelPreset[1] == Setup_Tab4_PredationFormCMB->currentText().toStdString()) &&
                 (ModelPreset[2] == Setup_Tab4_HarvestFormCMB->currentText().toStdString()) &&
                 (ModelPreset[3] == competitionForm.toStdString())) {
                setModelPreset(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelPreset("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_PredationFormCMB(QString predationForm)
{
    int i=0;
    int NumPresets = m_ModelPresets.size();
    bool done = false;
    std::vector<std::string> ModelPreset;

    emit PredationFormChanged(predationForm);

    while (!done and (i<NumPresets)) {
        for (std::string ModelName : m_ModelPresetNames) {
            ModelPreset = m_ModelPresets[ModelName];
            if  ((ModelPreset[0] == Setup_Tab4_GrowthFormCMB->currentText().toStdString()) &&
                 (ModelPreset[1] == predationForm.toStdString()) &&
                 (ModelPreset[2] == Setup_Tab4_HarvestFormCMB->currentText().toStdString()) &&
                 (ModelPreset[3] == Setup_Tab4_CompetitionFormCMB->currentText().toStdString())) {
                setModelPreset(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelPreset("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_FontSizeCMB(QString theFontSize)
{
    int fontSize = theFontSize.toInt();

    QTextCursor cursor = Setup_Tab4_ModelEquationTE->textCursor();
    Setup_Tab4_ModelEquationTE->selectAll();
    Setup_Tab4_ModelEquationTE->setFontPointSize(fontSize);
    Setup_Tab4_ModelEquationTE->setTextCursor( cursor );
}

QString
nmfSetup_Tab4::getObsBiomassType()
{
    return Setup_Tab4_ObsBiomassTypeCMB->currentText();
}

void
nmfSetup_Tab4::callback_ObsBiomassTypeCMB(QString obsBiomassType)
{
    emit ObservedBiomassType(obsBiomassType);
}

bool
nmfSetup_Tab4::isGrowthFormHighlighted()
{
    return Setup_Tab4_GrowthHighlightPB->isChecked();
}

void
nmfSetup_Tab4::callback_GrowthHighlightPB()
{
    emit RedrawEquation();
}

bool
nmfSetup_Tab4::isHarvestFormHighlighted()
{
    return Setup_Tab4_HarvestHighlightPB->isChecked();
}


void
nmfSetup_Tab4::callback_HarvestHighlightPB()
{
    emit RedrawEquation();
}


bool
nmfSetup_Tab4::isPredationFormHighlighted()
{
    return Setup_Tab4_PredationHighlightPB->isChecked();
}


void
nmfSetup_Tab4::callback_PredationHighlightPB()
{
    emit RedrawEquation();
}


bool
nmfSetup_Tab4::isCompetitionFormHighlighted()
{
    return Setup_Tab4_CompetitionHighlightPB->isChecked();
}


void
nmfSetup_Tab4::callback_CompetitionHighlightPB()
{
    emit RedrawEquation();
}

void
nmfSetup_Tab4::callback_UpdateEndYear(int value)
{
    Setup_Tab4_EndYearLE->setText(
                QString::number(Setup_Tab4_StartYearSB->value()+
                                getRunLength()));
}

void
nmfSetup_Tab4::callback_CalcPB()
{
    double systemK = calculateSystemCarryingCapacity();
    m_logger->logMsg(nmfConstants::Normal,"System Carrying Capacity set to: "+QString::number(systemK).toStdString());
}

void
nmfSetup_Tab4::callback_ModelCopyPB()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::callback_CopyModelPB start");
    QString currentModelName = QString::fromStdString(m_ModelName);
    QString newModelName     = getNewModelName();
    if (! newModelName.isEmpty()) {
        // Copy all currentModelName datasets to newModelName
        copyModelDataInAllTables(currentModelName,newModelName);
        setModelName(newModelName);
        callback_SavePB();
    }
}

bool
nmfSetup_Tab4::copyModelDataInAllTables(const QString& currentModelName,
                                        const QString& newModelName)
{
    bool modelCopyAborted = false;
    int NumTables = 0;
    std::string temp_table = "temp_table";
    std::vector<std::string> cmds;
    std::vector<std::string> allTables;
    std::string table;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap,dataMap2;
    std::string queryStr;
    std::string currentDBName = m_databasePtr->nmfGetCurrentDatabase();

    // Get list of table names that have a ModelName column
    fields    = {"TABLE_NAME"};
    queryStr  = "SELECT DISTINCT TABLE_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE COLUMN_NAME IN ('ModelName') and TABLE_SCHEMA='" + currentDBName + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumTables = dataMap["TABLE_NAME"].size();

//  QProgressDialog progressDlg("\nCopying tables...", "Abort Copy", 0, NumTables, Setup_Tabs);
    QProgressDialog progressDlg("\nCopying tables...", QString(), 0, NumTables, Setup_Tabs); // Hide the cancel button
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.show();

    for (int i=0; i<NumTables; ++i) {

        progressDlg.setValue(i);
        if (progressDlg.wasCanceled()) {
            modelCopyAborted = true;
            break;
        }

        table = dataMap["TABLE_NAME"][i];

        // Skip over any tables that begin with: summary*, diagnostic*, output*, forecast*, modelreview*
        QString tableStr = QString::fromStdString(table);
        if (tableStr.startsWith("diagnostic") ||
            tableStr.startsWith("output")     ||
            tableStr.startsWith("forecast")   ||
            tableStr.startsWith("summary")    ||
            tableStr.startsWith("modelreview")) {
              continue; // skip the table
        }

        // Process the table only if it isn't empty
        fields   = {"HasData"};
        queryStr = "SELECT EXISTS (SELECT 1 FROM `" + table + "`) AS HasData";
        dataMap2 = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        if (dataMap2["HasData"][0] == "1") {
            // These commands copy all rows with the currentModelName to new rows with
            // the newModelName replacing the currentModelName.
            cmds.clear();
            cmds.push_back("DROP TABLE IF EXISTS `" + temp_table + "`");
            cmds.push_back("CREATE TABLE " + temp_table + " AS SELECT * FROM " + table +
                           " WHERE ModelName=\"" + currentModelName.toStdString() + "\"");
            cmds.push_back("UPDATE " + temp_table + " SET ModelName=\"" + newModelName.toStdString() + "\"");
            cmds.push_back("INSERT INTO " + table + " SELECT * FROM " + temp_table);
            cmds.push_back("DROP TABLE IF EXISTS `" + temp_table + "`");
            for (std::string cmd : cmds) {
                std::string errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
                if (nmfUtilsQt::isAnError(errorMsg)) {
                    m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::copyModelDataInAllTables: " + errorMsg);
                    m_logger->logMsg(nmfConstants::Error,"==> cmd: " + cmd);
                }
            }
        }
    }
    progressDlg.setValue(NumTables);

    QString msg = (modelCopyAborted) ? "\nModel copy incomplete. Copy was aborted.\n" : "\nModel copy complete.\n";
    QMessageBox::information(Setup_Tabs, tr("Copy"),
                             tr(msg.toLatin1()),
                             QMessageBox::Ok);

    return true;
}

QString
nmfSetup_Tab4::getNewModelName()
{
    bool ok;
    bool ModelNameOK = false;
    QString ModelName = "";

    while (! ModelNameOK) {
        ModelName = QInputDialog::getText(Setup_Tabs,
                                          tr("New Model Name"),
                                          tr("Enter New Model Name:"),
                                          QLineEdit::Normal, "", &ok);
        if (ok && ModelName.isEmpty()) {
            QMessageBox::warning(Setup_Tabs,
                                 tr("Illegal Name"),
                                 tr("\nModel Name cannot be blank.\n"),
                                 QMessageBox::Ok);
        } else {
            if (modelExists(ModelName)) {
                QMessageBox::warning(Setup_Tabs,
                                     tr("Model Name Exists"),
                                     tr("\nModel Name already exists. Please select another name.\n"),
                                     QMessageBox::Ok);
            } else {
                ModelNameOK = true;
            }
        }
    }

    return ModelName;
}

void
nmfSetup_Tab4::callback_ModelNewPB()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::callback_ModelNewPB start");

    QString newModelName = getNewModelName();
    if (! newModelName.isEmpty()) {
        setModelName(newModelName);
        callback_SavePB();
    }

    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::callback_ModelNewPB end");
}

bool
nmfSetup_Tab4::modelExists(QString ModelName)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"ProjectName","ModelName"};
    queryStr = "SELECT ProjectName,ModelName FROM " + nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + ModelName.toStdString() + "'";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);

    return (dataMap["ModelName"].size() > 0);
}

bool
nmfSetup_Tab4::isAggProd()
{
    return (Setup_Tab4_CompetitionFormCMB->currentText() == "AGG-PROD");
}

bool
nmfSetup_Tab4::isTypeIII()
{
    return (Setup_Tab4_PredationFormCMB->currentText() == "Type III");
}
