
#include "nmfSetupTab04.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

LoadDlg::LoadDlg(const QString &title,
                 QWidget*     parent,
                 nmfLogger*   theLogger,
                 nmfDatabase* theDatabasePtr,
                 const QString &currentConfig)
    : QDialog(parent)
{
    QLabel *label0 = new QLabel("");

    m_logger      = theLogger;
    m_databasePtr = theDatabasePtr;
    m_SettingNames.clear();
    m_SettingsLW = new QListWidget();

    reloadSystemsList();

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(label0);
    mainLayout->addWidget(m_SettingsLW);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);
    setWindowTitle(title);

    // Add context menu for delete item
    m_SettingsLW->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_SettingsLW,SIGNAL(customContextMenuRequested(QPoint)),
            this,      SLOT(callback_ShowContextMenu(QPoint)));

    connect(m_buttonBox,  SIGNAL(accepted()),    this, SLOT(callback_LoadOk()));
    connect(m_buttonBox,  SIGNAL(rejected()),    this, SLOT(reject()));
    connect(m_SettingsLW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,       SLOT(callback_ItemDoubleClicked(QListWidgetItem*)));

    m_buttonBox->setFocus();

    // Set currentConfig
    QList<QListWidgetItem*> items = m_SettingsLW->findItems(currentConfig,Qt::MatchExactly);
    if (items.size() > 0) {
        m_SettingsLW->setCurrentItem(items[0]);
    }    

}

void
LoadDlg::callback_ShowContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = m_SettingsLW->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Delete", this, SLOT(callback_DeleteItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}


void
LoadDlg::callback_ItemDoubleClicked(QListWidgetItem* item)
{
    callback_LoadOk();
}



void
LoadDlg::reloadSystemsList()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string name;
    QListWidgetItem *item;

    m_SettingsLW->clear();
    m_SettingNames.clear();

    fields     = {"SystemName"};
    queryStr   = "SELECT SystemName FROM Systems";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned int i=0; i<dataMap["SystemName"].size(); ++i) {
        name = dataMap["SystemName"][i];
        m_SettingNames.push_back(name);
        item = new QListWidgetItem();
        item->setText(QString::fromStdString(name));
        m_SettingsLW->addItem(item);
    }
}

void
LoadDlg::callback_LoadOk()
{
    if (m_SettingsLW->selectedItems().size() == 0) {
        QMessageBox::information(this, "NULL Selection",
                                 "\nPlease select a Settings configuration to load or click Cancel.\n");
        return;
    }
    saveSettings();
    accept();
}


void
LoadDlg::saveSettings()
{
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Settings");
    settings.setValue("Name", m_SettingsLW->currentItem()->text());
    settings.endGroup();

    updateWindowTitle();
}

void
LoadDlg::updateWindowTitle()
{
    std::string ProjectName;
    std::string ProjectSettingsConfig;
    std::string winTitle;

    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("SetupTab");
    ProjectName     = settings.value("ProjectName","").toString().toStdString();
    settings.endGroup();

    settings.beginGroup("Settings");
    ProjectSettingsConfig = settings.value("Name","").toString().toStdString();
    settings.endGroup();

    winTitle = "MSSPM (" + ProjectName + " - " + ProjectSettingsConfig + ")";

    foreach (QWidget *w, qApp->topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            mainWin->setWindowTitle(QString::fromStdString(winTitle));
}


void
LoadDlg::callback_DeleteItem()
{
    QMessageBox::StandardButton reply;
    QString msg;
    QString currentItem = m_SettingsLW->currentItem()->text();
    std::string cmd;
    std::string errorMsg;

    msg = "\nOK to delete configuration: " + currentItem + " ?";
    reply = QMessageBox::question(this, tr("Delete Configuration"),
                       tr(msg.toLatin1()),
                       QMessageBox::No|QMessageBox::Yes,
                       QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        cmd  = "DELETE FROM Systems WHERE SystemName = '" + currentItem.toStdString() + "'";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"SaveDlg callback_DeleteItem: Delete error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
        reloadSystemsList();
        emit ClearSystemName();
        QMessageBox::information(this, "Settings Configuration",
                                 "\nSuccessfully deleted Settings configuration.\n");
    }
}

void
LoadDlg::getSettingData(SystemData &data)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string GrowthStr,HarvestStr,CompetitionStr,PredationStr;

    if (m_SettingsLW->selectionModel()->selectedRows().size() == 0) {
        m_logger->logMsg(nmfConstants::Error,"LoadDlg callback_LoadOk: No Settings configuration selected. ");
        QMessageBox::information(this, "NULL Selection",
                                 "\nPlease select a Settings configuration to load.\n");
        return;
    }

    QString currentItem = m_SettingsLW->currentItem()->text();

    fields    = {"SystemName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm",
                 "WithinGuildCompetitionForm","NumberOfRuns","RunLength","TimeStep",
                 "Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                 "GAGenerations","GAPopulationSize","GAMutationRate","GAConvergence",
                 "BeesNumTotal","BeesNumElite","BeesNumOther",
                 "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                 "BeesMaxGenerations","BeesNeighborhoodSize",
                 "GradMaxIterations","GradMaxLineSearches",
                 "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                 "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr  = "SELECT SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,";
    queryStr += "WithinGuildCompetitionForm,NumberOfRuns,RunLength,TimeStep,";
    queryStr += "Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr += "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr += "BeesNumTotal,BeesNumElite,BeesNumOther,";
    queryStr += "BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr += "GradMaxIterations,GradMaxLineSearches,";
    queryStr += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter ";
    queryStr += "FROM Systems WHERE SystemName = '" + currentItem.toStdString() + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != 1) {
        m_logger->logMsg(nmfConstants::Error,"LoadDlg callback_LoadOk: No records found to load");
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

    data.Name                  = dataMap["SystemName"][0];
    data.CarryingCapacity      = dataMap["CarryingCapacity"][0];
    data.GrowthForm            = GrowthStr;
    data.HarvestForm           = HarvestStr;
    data.CompetitionForm       = CompetitionStr;
    data.PredationForm         = PredationStr;
    data.NumberOfRuns          = std::stoi(dataMap["NumberOfRuns"][0]);
    data.RunLength             = std::stoi(dataMap["RunLength"][0]);
    data.TimeStep              = std::stoi(dataMap["TimeStep"][0]);
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
    data.Scaling               = dataMap["Scaling"][0];
    data.GradMaxIterations     = std::stoi(dataMap["GradMaxIterations"][0]);
    data.GradMaxLineSearches   = std::stoi(dataMap["GradMaxLineSearches"][0]);
    data.NLoptUseStopVal       = std::stoi(dataMap["NLoptUseStopVal"][0]);
    data.NLoptUseStopAfterTime = std::stoi(dataMap["NLoptUseStopAfterTime"][0]);
    data.NLoptUseStopAfterIter = std::stoi(dataMap["NLoptUseStopAfterIter"][0]);
    data.NLoptStopVal          = std::stod(dataMap["NLoptStopVal"][0]);
    data.NLoptStopAfterTime    = std::stoi(dataMap["NLoptStopAfterTime"][0]);
    data.NLoptStopAfterIter    = std::stoi(dataMap["NLoptStopAfterIter"][0]);
}

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
    m_ProjectSettingsConfig.clear();

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
    m_ModelPresetNames = {"Build Your Own Model","Linear","Logistic","Schaefer","GB-Pilot","MS-PROD","AGG-PROD"};
    m_ModelPresets["Build Your Own Model"] = {"Null",     "Null",   "Null",  "Null"};
    m_ModelPresets["Linear"]               = {"Linear",   "Null",   "Null",  "Null"};
    m_ModelPresets["Logistic"]             = {"Logistic", "Null",   "Null",  "Null"};
    m_ModelPresets["Schaefer"]             = {"Logistic", "Null",   "Catch", "Null"};
    m_ModelPresets["GB-Pilot"]             = {"Linear",   "Null",   "Catch", "NO_K"};
    m_ModelPresets["MS-PROD"]              = {"Logistic", "Type I", "F",     "MS-PROD"};
    m_ModelPresets["AGG-PROD"]             = {"Logistic", "Type I", "F",     "AGG-PROD"};

    Setup_Tab4_SystemNameLE             = Setup_Tabs->findChild<QLineEdit *>("Setup_Tab4_SystemNameLE");
    Setup_Tab4_SystemCarryingCapacityLE = Setup_Tabs->findChild<QLineEdit *>("Setup_Tab4_SystemCarryingCapacityLE");
    Setup_Tab4_ModelEquationTE          = Setup_Tabs->findChild<QTextEdit *>("Setup_Tab4_ModelEquationTE");
    Setup_Tab4_FontSizeCMB              = Setup_Tabs->findChild<QComboBox *>("Setup_Tab4_FontSizeCMB");
    Setup_Tab4_GrowthHighlightPB        = Setup_Tabs->findChild<QPushButton  *>("GrowthHighlightPB");
    Setup_Tab4_HarvestHighlightPB       = Setup_Tabs->findChild<QPushButton  *>("HarvestHighlightPB");
    Setup_Tab4_PredationHighlightPB     = Setup_Tabs->findChild<QPushButton  *>("PredationHighlightPB");
    Setup_Tab4_CompetitionHighlightPB   = Setup_Tabs->findChild<QPushButton  *>("CompetitionHighlightPB");
    Setup_Tab4_SystemNamePB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab4_SystemNamePB");
    Setup_Tab4_StartYearSB              = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab4_StartYearSB");
    Setup_Tab4_EndYearLE                = Setup_Tabs->findChild<QLineEdit    *>("Setup_Tab4_EndYearLE");
    Setup_Tab4_RunLengthSB              = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab4_RunLengthSB");
    Setup_Tab4_ModelPresetsLBL          = Setup_Tabs->findChild<QLabel       *>("Setup_Tab4_ModelPresetsLBL");
    Setup_Tab4_ModelPresetsCMB          = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_ModelPresetsCMB");
    Setup_Tab4_GrowthFormCMB            = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_GrowthFormCMB");
    Setup_Tab4_PredationFormCMB         = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_PredationFormCMB");
    Setup_Tab4_HarvestFormCMB           = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_HarvestFormCMB");
    Setup_Tab4_CompetitionFormCMB       = Setup_Tabs->findChild<QComboBox    *>("Setup_Tab4_CompetitionFormCMB");
    Setup_Tab4_PrevPB                   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_PrevPB");
    Setup_Tab4_NextPB                   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_NextPB");
    Setup_Tab4_LoadPB                   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_LoadPB");
    Setup_Tab4_DelPB                    = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_DelPB");
    Setup_Tab4_SavePB                   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_SavePB");
    Setup_Tab4_CalcPB                   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab4_CalcPB");

    // Set widget parameters
    Setup_Tab4_LoadPB->setEnabled(true);
    Setup_Tab4_SavePB->setEnabled(true);
    Setup_Tab4_PrevPB->setText("\u25C1--");
    Setup_Tab4_NextPB->setText("--\u25B7");
    Setup_Tab4_SystemNameLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColor);
    Setup_Tab4_EndYearLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColor);

    // Get and set the highlight colors
    setHighlightColors();

    connect(Setup_Tab4_ModelPresetsCMB,        SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_ModelPresetsCMB(QString)));
    connect(Setup_Tab4_GrowthFormCMB,          SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_GrowthFormCMB(QString)));
    connect(Setup_Tab4_PredationFormCMB,       SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_PredationFormCMB(QString)));
    connect(Setup_Tab4_HarvestFormCMB,         SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_HarvestFormCMB(QString)));
    connect(Setup_Tab4_CompetitionFormCMB,     SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_CompetitionFormCMB(QString)));
    connect(Setup_Tab4_FontSizeCMB,            SIGNAL(currentTextChanged(QString)),
            this,                              SLOT(callback_Setup_Tab4_FontSizeCMB(QString)));
    connect(Setup_Tab4_GrowthHighlightPB,      SIGNAL(clicked()),
            this,                              SLOT(callback_GrowthHighlightPB()));
    connect(Setup_Tab4_HarvestHighlightPB,     SIGNAL(clicked()),
            this,                              SLOT(callback_HarvestHighlightPB()));
    connect(Setup_Tab4_PredationHighlightPB,   SIGNAL(clicked()),
            this,                              SLOT(callback_PredationHighlightPB()));
    connect(Setup_Tab4_CompetitionHighlightPB, SIGNAL(clicked()),
            this,                              SLOT(callback_CompetitionHighlightPB()));
    connect(Setup_Tab4_SystemNamePB,           SIGNAL(clicked()),
            this,                              SLOT(callback_SystemNamePB()));
    connect(Setup_Tab4_CalcPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_CalcPB()));
    connect(Setup_Tab4_LoadPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_Setup_Tab4_LoadPB()));
    connect(Setup_Tab4_DelPB,                  SIGNAL(clicked()),
            this,                              SLOT(callback_Setup_Tab4_DelPB()));
    connect(Setup_Tab4_SavePB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_Setup_Tab4_SavePB()));
    connect(Setup_Tab4_PrevPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_Setup_Tab4_PrevPB()));
    connect(Setup_Tab4_NextPB,                 SIGNAL(clicked()),
            this,                              SLOT(callback_Setup_Tab4_NextPB()));
    connect(Setup_Tab4_StartYearSB,            SIGNAL(valueChanged(int)),
            this,                              SLOT(callback_UpdateEndYear(int)));
    connect(Setup_Tab4_RunLengthSB,            SIGNAL(valueChanged(int)),
            this,                              SLOT(callback_UpdateEndYear(int)));
}

nmfSetup_Tab4::~nmfSetup_Tab4()
{
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
nmfSetup_Tab4::setSystemName(QString systemName)
{
    Setup_Tab4_SystemNameLE->setText(systemName);
    saveSettings();
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
nmfSetup_Tab4::modelPresetsCMB()
{
    return Setup_Tab4_ModelPresetsCMB;
}

QComboBox*
nmfSetup_Tab4::growthFormCMB()
{
    return Setup_Tab4_GrowthFormCMB;
}
QComboBox*
nmfSetup_Tab4::predationFormCMB()
{
    return Setup_Tab4_PredationFormCMB;
}
QComboBox*
nmfSetup_Tab4::harvestFormCMB()
{
    return Setup_Tab4_HarvestFormCMB;
}
QComboBox*
nmfSetup_Tab4::competitionFormCMB()
{
    return Setup_Tab4_CompetitionFormCMB;
}

void
nmfSetup_Tab4::drawEquation(QString Label, QString Eqn, QString Key)
{
    Setup_Tab4_ModelEquationTE->setText("<p><br/>"+Label+"<br/><br/>"+Eqn+"<br/><br/>"+Key+"</p>");
    callback_Setup_Tab4_FontSizeCMB(Setup_Tab4_FontSizeCMB->currentText());
}


void
nmfSetup_Tab4::readSettings()
{
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Settings");
    m_ProjectSettingsConfig = settings.value("Name","").toString().toStdString();
    settings.endGroup();
}

void
nmfSetup_Tab4::saveSettings()
{
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Settings");
    settings.setValue("Name", QString::fromStdString(m_ProjectSettingsConfig));
    settings.endGroup();

    settings.beginGroup("SetupTab");
    settings.setValue("FontSize", getFontSize());
    settings.endGroup();

    emit SaveMainSettings();
}

void
nmfSetup_Tab4::callback_Setup_Tab4_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab4::callback_Setup_Tab4_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab4::loadSystem()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    SystemData data;

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"Loading: "+m_ProjectSettingsConfig);

    fields     = {"SystemName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","GAGenerations","GAPopulationSize","GAMutationRate","GAConvergence",
                  "Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                  "BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "GradMaxIterations","GradMaxLineSearches",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr   = "SELECT SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,";
    queryStr  += "BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "GradMaxIterations,GradMaxLineSearches,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter FROM Systems ";
    queryStr  += "WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["SystemName"].size() == 0) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetupTab3::callback_Setup_Tab4_LoadPB: No system config table entry found.");
        return;
    }

    data.Name                  = dataMap["SystemName"][0];
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

    loadDialog->getSettingData(data);

    Setup_Tab4_SystemNameLE->setText(QString::fromStdString(data.Name));
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::fromStdString(data.CarryingCapacity));
    Setup_Tab4_GrowthFormCMB->setCurrentText(QString::fromStdString(data.GrowthForm));
    Setup_Tab4_HarvestFormCMB->setCurrentText(QString::fromStdString(data.HarvestForm));
    Setup_Tab4_CompetitionFormCMB->setCurrentText(QString::fromStdString(data.CompetitionForm));
    Setup_Tab4_PredationFormCMB->setCurrentText(QString::fromStdString(data.PredationForm));
    Setup_Tab4_StartYearSB->setValue(data.StartYear);
    setRunLength(data.RunLength);
    Setup_Tab4_EndYearLE->setText(QString::number(data.StartYear+data.RunLength));

    m_ProjectSettingsConfig = data.Name;

    // Save Algorithm, Minimizer, and ObjectiveFunction back into table so user can run without
    // hitting Save.....
    std::string cmd;
    std::string errorMsg;

    cmd  = "UPDATE Systems SET";
    cmd += "  Algorithm = '"          + data.Algorithm + "'";
    cmd += ", Minimizer = '"          + data.Minimizer + "'";
    cmd += ", ObjectiveCriterion = '" + data.ObjectiveCriterion + "'";
    cmd += ", Scaling = '"            + data.Scaling + "'";
    cmd += " WHERE SystemName = '"    + m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetupTab3 callback_Setup_Tab4_LoadPB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }

    saveSettings();

    emit SystemLoaded();
}

void
nmfSetup_Tab4::callback_Setup_Tab4_LoadPB()
{
    loadDialog = new LoadDlg(tr("Load Settings"), Setup_Tabs,
                             m_logger, m_databasePtr,
                             QString::fromStdString(m_ProjectSettingsConfig));

    connect(loadDialog, SIGNAL(ClearSystemName()),
            this,       SLOT(callback_ClearSystemName()));

    if (loadDialog->exec() == QDialog::Accepted) {
        loadSystem();
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
        queryStr   = "SELECT GuildName,GuildK FROM Guilds";
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["GuildName"].size();
        for (int i=0; i<NumRecords; ++i) {
            SystemK += std::stod(dataMap["GuildK"][i]);
        }
    } else {
        fields     = {"SpeName","SpeciesK"};
        queryStr   = "SELECT SpeName,SpeciesK FROM Species";
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        for (int i=0; i<NumRecords; ++i) {
            SystemK += std::stod(dataMap["SpeciesK"][i]);
        }
    }
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::number(SystemK));

    m_logger->logMsg(nmfConstants::Normal,"Setting SystemK to: "+std::to_string(SystemK));

    return SystemK;
}


void
nmfSetup_Tab4::callback_Setup_Tab4_DelPB()
{
    QMessageBox::StandardButton reply;
    QString msg;
    QString currentSystemName = Setup_Tab4_SystemNameLE->text();

    if (currentSystemName.isEmpty()) {
        QMessageBox::warning(Setup_Tabs,
                             tr("Warning"),
                             tr("Cannot delete a System Name that's blank."),
                             QMessageBox::Ok);
        return;
    }

    msg = "\nOK to delete configuration: " + currentSystemName + " ?";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete Configuration"),
                                  tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        deleteSystem(currentSystemName);
        QMessageBox::information(Setup_Tabs, "Settings Configuration",
                                 "\nSuccessfully deleted Settings configuration.\n");
    }
}

void
nmfSetup_Tab4::deleteSystem(QString systemToDelete)
{
    std::string cmd;
    std::string errorMsg;

    cmd  = "DELETE FROM Systems WHERE SystemName = '" + systemToDelete.toStdString() + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"[Error 1] deleteSystem: Delete error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }
    callback_ClearSystemName();
}

void
nmfSetup_Tab4::callback_ClearSystemName()
{
    Setup_Tab4_SystemNameLE->setText("");
}

void
nmfSetup_Tab4::callback_Setup_Tab4_SavePB()
{
    saveSystem(true);
}

void
nmfSetup_Tab4::callback_SaveSystem()
{
    saveSystem(true);
}

void
nmfSetup_Tab4::reloadSystemName()
{
    QString SystemName = Setup_Tab4_SystemNameLE->text();
    readSettings();

    if (SystemName.isEmpty() || SystemName.contains("__")) {
        Setup_Tab4_SystemNameLE->setText(QString::fromStdString(m_ProjectSettingsConfig));
    }
}

void
nmfSetup_Tab4::saveSystem(bool RunChecks)
{
    bool okToSave = true;
    std::string msg;
    std::string SystemName = Setup_Tab4_SystemNameLE->text().toStdString();

    calculateSystemCarryingCapacity();

    if (RunChecks && SystemName.empty()) {
        msg = "\nError: A System Name must be given prior to saving.\n";
        m_logger->logMsg(nmfConstants::Error,"");
        QMessageBox::critical(Setup_Tabs, "Save Error",QString::fromStdString(msg));
        return;
    }

    if (RunChecks) {
        msg = "\nOK to save current settings as: " + SystemName + " ?";
        QMessageBox::StandardButton reply = QMessageBox::question(Setup_Tabs, tr("Save"),
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

    emit UpdateInitialObservedBiomass();

}


bool
nmfSetup_Tab4::saveSettingsConfiguration(bool verbose,
                                         std::string CurrentSettingsName)
{
    std::string cmd;
    std::string errorMsg;
    int NumberOfParameters      = 0;
    std::string SystemK         = Setup_Tab4_SystemCarryingCapacityLE->text().toStdString();
    std::string GrowthForm      = Setup_Tab4_GrowthFormCMB->currentText().toStdString();
    std::string PredationForm   = Setup_Tab4_PredationFormCMB->currentText().toStdString();
    std::string HarvestForm     = Setup_Tab4_HarvestFormCMB->currentText().toStdString();
    std::string CompetitionForm = Setup_Tab4_CompetitionFormCMB->currentText().toStdString();
    std::string StartYear       = std::to_string(Setup_Tab4_StartYearSB->value());
    std::string RunLength       = std::to_string(getRunLength());
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    if (GrowthForm == "Linear")
        NumberOfParameters += 1;
    else if (GrowthForm == "Logistic")
        NumberOfParameters += 2;
    if (HarvestForm == "F")
        NumberOfParameters += 1;
    if (CompetitionForm != "Null")
        NumberOfParameters += 1;
    if (PredationForm == "Type I")
        NumberOfParameters += 1;
    else if (PredationForm == "Type II")
        NumberOfParameters += 2;
    else if (PredationForm == "Type III")
        NumberOfParameters += 3;

    // Check if system exists, if so Update else REPLACE
    fields   = {"SystemName"};
    queryStr = "SELECT SystemName from Systems where SystemName = '" + CurrentSettingsName + "'";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["SystemName"].size() != 0) { // This means the system name exists so do an update
        cmd  = "UPDATE Systems SET";
        cmd += "   SystemName = '"                 + CurrentSettingsName +
               "', CarryingCapacity = "            + SystemK +
               ",  GrowthForm = '"                 + GrowthForm +
               "', PredationForm = '"              + PredationForm +
               "', HarvestForm = '"                + HarvestForm +
               "', WithinGuildCompetitionForm = '" + CompetitionForm +
               "', NumberOfParameters = "          + std::to_string(NumberOfParameters) +
               ",  StartYear = "                   + StartYear +
               ",  RunLength = "                   + RunLength +
               " WHERE SystemName = '"             + CurrentSettingsName + "'";
    } else { // This means the system name does not exist so do a replace
        cmd  = "REPLACE INTO Systems (SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,";
        cmd += "WithinGuildCompetitionForm,StartYear,RunLength,NumberOfParameters) ";
        cmd += "VALUES ('" +
                CurrentSettingsName                + "', "  +
                SystemK                            + ", '"  +
                GrowthForm                         + "', '" +
                PredationForm                      + "', '" +
                HarvestForm                        + "', '" +
                CompetitionForm                    + "', "  +
                StartYear                          + ", "   +
                RunLength                          + ", "   +
                std::to_string(NumberOfParameters) + " );";
    }

    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::SaveSettingsConfiguration: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }
    if (verbose) {
        QMessageBox::information(Setup_Tabs, "Settings Updated",
                                 "\nSettings in Systems database table have been successfully updated.\n");
    }
    saveSettings();

    emit SaveEstimationRunSettings();
    emit ReloadWidgets();

    return true;
}

void
nmfSetup_Tab4::callback_ReloadWidgets()
{
    emit ReloadWidgets();
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
    if (m_ProjectSettingsConfig.empty())
        return;

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
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found in Systems" << std::endl;
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
    Setup_Tab4_SystemNameLE->setText(QString::fromStdString(dataMap["SystemName"][0]));
    Setup_Tab4_SystemCarryingCapacityLE->setText(QString::fromStdString(dataMap["CarryingCapacity"][0]));
    Setup_Tab4_GrowthFormCMB->setCurrentText(QString::fromStdString(GrowthStr));
    Setup_Tab4_HarvestFormCMB->setCurrentText(QString::fromStdString(HarvestStr));
    Setup_Tab4_CompetitionFormCMB->setCurrentText(QString::fromStdString(CompetitionStr));
    Setup_Tab4_PredationFormCMB->setCurrentText(QString::fromStdString(PredationStr));
    Setup_Tab4_StartYearSB->setValue(std::stoi(dataMap["StartYear"][0]));
    setRunLength(std::stoi(dataMap["RunLength"][0]));
    Setup_Tab4_EndYearLE->setText(QString::number(Setup_Tab4_StartYearSB->value()+getRunLength()));
}


void
nmfSetup_Tab4::clearWidgets()
{

}


void
nmfSetup_Tab4::callback_Setup_Tab4_ModelPresetsCMB(QString name)
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
nmfSetup_Tab4::setModelName(std::string modelName)
{
    Setup_Tab4_ModelPresetsCMB->blockSignals(true);
    Setup_Tab4_ModelPresetsCMB->setCurrentText(QString::fromStdString(modelName));
    Setup_Tab4_ModelPresetsCMB->blockSignals(false);
}


void
nmfSetup_Tab4::callback_Setup_Tab4_GrowthFormCMB(QString growthForm)
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
                setModelName(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelName("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_Setup_Tab4_HarvestFormCMB(QString harvestForm)
{
    int i=0;
    int NumPresets = m_ModelPresets.size();
    bool done = false;
    std::vector<std::string> ModelPreset;

    // Change the Estimation->Harvest Parameters table to correspond with the selected harvestForm
    if (harvestForm == "QE")
        harvestForm = "Effort";
    else if (harvestForm == "F")
        harvestForm = "Exploitation";

    while (!done and (i<NumPresets)) {
        for (std::string ModelName : m_ModelPresetNames) {
            ModelPreset = m_ModelPresets[ModelName];
            if  ((ModelPreset[0] == Setup_Tab4_GrowthFormCMB->currentText().toStdString()) &&
                 (ModelPreset[1] == Setup_Tab4_PredationFormCMB->currentText().toStdString()) &&
                 (ModelPreset[2] == harvestForm.toStdString()) &&
                 (ModelPreset[3] == Setup_Tab4_CompetitionFormCMB->currentText().toStdString())) {
                setModelName(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelName("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_Setup_Tab4_CompetitionFormCMB(QString competitionForm)
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
                setModelName(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelName("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_Setup_Tab4_PredationFormCMB(QString predationForm)
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
                setModelName(ModelName);
                done = true;
            }
            if (done)
                break;
        }
        ++i;
    }
    if (! done) {
        setModelName("Build Your Own Model");
    }
}

void
nmfSetup_Tab4::callback_Setup_Tab4_FontSizeCMB(QString theFontSize)
{
    int fontSize = theFontSize.toInt();

    QTextCursor cursor = Setup_Tab4_ModelEquationTE->textCursor();
    Setup_Tab4_ModelEquationTE->selectAll();
    Setup_Tab4_ModelEquationTE->setFontPointSize(fontSize);
    Setup_Tab4_ModelEquationTE->setTextCursor( cursor );
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
    m_logger->logMsg(nmfConstants::Normal,"System Carrying Capacity set to: "+std::to_string(systemK));
}

void
nmfSetup_Tab4::callback_SystemNamePB()
{
    bool ok;
    QString SystemName = QInputDialog::getText(Setup_Tabs,
                                               tr("New System File"),
                                               tr("Enter New System File Name:"),
                                               QLineEdit::Normal, "", &ok);
    if (ok) {
        if (SystemName.isEmpty()) {
            QMessageBox::warning(Setup_Tabs,
                                 tr("Illegal Name"),
                                 tr("\nSystem file name cannot be blank.\n"),
                                 QMessageBox::Ok);
        } else {
            if (systemFileExists(SystemName)) {
                QMessageBox::warning(Setup_Tabs,
                                     tr("System File Exists"),
                                     tr("\nSystem file already exists. Please select another name.\n"),
                                     QMessageBox::Ok);
            } else {
                Setup_Tab4_SystemNameLE->setText(SystemName);
            }
        }
    }
}

bool
nmfSetup_Tab4::systemFileExists(QString SystemName)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"SystemName"};
    queryStr = "SELECT SystemName FROM Systems WHERE SystemName = '" +
                SystemName.toStdString() + "'";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);

    return (dataMap["SystemName"].size() > 0);
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
