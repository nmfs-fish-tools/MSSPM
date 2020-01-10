
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
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    settings->setValue("Name", m_SettingsLW->currentItem()->text());
    settings->endGroup();

    delete settings;

    updateWindowTitle();
}

void
LoadDlg::updateWindowTitle()
{
    std::string ProjectName;
    std::string ProjectSettingsConfig;
    std::string winTitle;

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("SetupTab");
    ProjectName     = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Settings");
    ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    delete settings;

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
