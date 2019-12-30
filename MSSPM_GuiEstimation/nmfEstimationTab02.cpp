#include "nmfEstimationTab02.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab2::nmfEstimation_Tab2(QTabWidget  *tabs,
                                       nmfLogger   *logger,
                                       nmfDatabase *databasePtr,
                                       std::string &projectDir)
{
    QUiLoader loader;

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_SModel      = nullptr;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::nmfEstimation_Tab2");

    Estimation_Tabs = tabs;

    m_GroupBoxTitle["Catch"]        = "Catch";
    m_GroupBoxTitle["Effort"]       = "Effort";
    m_GroupBoxTitle["Exploitation"] = "Exploitation Rate";

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab02.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab2_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab2_Widget, tr("2. Harvest Parameters"));

    Estimation_Tab2_CatchTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_CatchTV");
    Estimation_Tab2_CatchGB = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab2_CatchGB");
    Estimation_Tab2_PrevPB  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_PrevPB");
    Estimation_Tab2_NextPB  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_NextPB");
    Estimation_Tab2_LoadPB  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_LoadPB");
    Estimation_Tab2_SavePB  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SavePB");

    connect(Estimation_Tab2_PrevPB, SIGNAL(clicked(bool)),
            this,                   SLOT(callback_PrevPB()));
    connect(Estimation_Tab2_NextPB, SIGNAL(clicked(bool)),
            this,                   SLOT(callback_NextPB()));
    connect(Estimation_Tab2_LoadPB, SIGNAL(clicked(bool)),
            this,                   SLOT(callback_LoadPB()));
    connect(Estimation_Tab2_SavePB, SIGNAL(clicked(bool)),
            this,                   SLOT(callback_SavePB()));

    Estimation_Tab2_PrevPB->setText("\u25C1--");
    Estimation_Tab2_NextPB->setText("--\u25B7");
}


nmfEstimation_Tab2::~nmfEstimation_Tab2()
{

}

void
nmfEstimation_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab2_CatchTV});
}

void
nmfEstimation_Tab2::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab2::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab2::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab2::callback_SavePB()
{
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> SpeNames;
    std::string MohnsRhoLabel = "";
    QString value;
    QString msg;

    if (m_SModel == nullptr) {
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    for (int j=0; j<m_SModel->columnCount(); ++ j) {
        SpeNames.push_back(m_SModel->horizontalHeaderItem(j)->text().toStdString());
    }

    // Check data integrity
    for (int j=0; j<m_SModel->columnCount(); ++j) { // Species
        for (int i=0; i<m_SModel->rowCount(); ++i) { // Time
            index = m_SModel->index(i,j);
            value = index.data().toString();
            if (value.contains(',')) {
                msg = "Invalid value found. No commas or special characters allowed in a number.";
                m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
                QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    cmd = "DELETE FROM " + m_HarvestType + " WHERE SystemName = '" +
           m_ProjectSettingsConfig + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from" +
                             QString::fromStdString(m_HarvestType) + " table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }
    cmd = "INSERT INTO " + m_HarvestType + " (MohnsRhoLabel,SystemName,SpeName,Year,Value) VALUES ";
    for (int j=0; j<m_SModel->columnCount(); ++j) { // Species
        for (int i=0; i<m_SModel->rowCount(); ++i) { // Time
            index = m_SModel->index(i,j);
            value = index.data().toString();
            cmd += "('" + MohnsRhoLabel +
                   "','" + m_ProjectSettingsConfig +
                   "','" + SpeNames[j] + "'," + std::to_string(i) +
                   ", " + value.toStdString() + "),";
        }
    }

    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Estimation_Tab2_CatchTV->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, QString::fromStdString(m_HarvestType) + " Updated",
                             "\n" + QString::fromStdString(m_HarvestType) +
                             " table has been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfEstimation_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

bool
nmfEstimation_Tab2::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::loadWidgets()");

    return loadWidgets("");
}

bool
nmfEstimation_Tab2::loadWidgets(QString MohnsRhoLabel)
{
    int m;
    int NumSpecies;
    int RunLength;
    int StartYear;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList VerticalList;
    QString SystemName = QString::fromStdString(m_ProjectSettingsConfig);

    // Strip off the MohnsRho suffix
    auto parts = SystemName.split("__");
    SystemName =  parts[0];

    readSettings();
    if (SystemName.isEmpty())
        return false;
    if (m_HarvestType.empty() || (m_HarvestType == "Null")) {
        m_Logger->logMsg(nmfConstants::Warning,"Warning: Harvest Type set to Null.");
        return true;
    }

    clearWidgets();

    Estimation_Tab2_CatchGB->setTitle(QString::fromStdString(m_GroupBoxTitle[m_HarvestType]));

    fields   = {"RunLength","StartYear"};
    queryStr = "SELECT RunLength,StartYear FROM Systems where SystemName = '" + SystemName.toStdString() + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["RunLength"].size() == 0)  {
        std::cout << "Error: No records found in Systems table." << std::endl;
        return false;
    }
    RunLength = std::stoi(dataMap["RunLength"][0]);
    StartYear = std::stoi(dataMap["StartYear"][0]);

    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    fields   = {"MohnsRhoLabel","SystemName","SpeName","Year","Value"};
    queryStr = "SELECT MohnsRhoLabel,SystemName,SpeName,Year,Value FROM " + m_HarvestType +
               " WHERE SystemName = '" + SystemName.toStdString() +
               "' AND MohnsRhoLabel = '" + MohnsRhoLabel.toStdString() +
               "' ORDER BY SpeName,Year ";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    m = 0;
    m_SModel = new QStandardItemModel( RunLength, NumSpecies );
    VerticalList.clear();
    for (int i=0; i<=RunLength; ++i) {
        VerticalList << " " + QString::number(StartYear+i) + " ";
    }
    for (int j=0; j<NumSpecies; ++j) {
        for (int i=0; i<=RunLength; ++i) {
            if ((m < NumRecords) && (SpeciesNames[j].toStdString() == dataMap["SpeName"][m]))
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
            else
                item = new QStandardItem(QString(""));
            item->setTextAlignment(Qt::AlignCenter);
            m_SModel->setItem(i, j, item);
        }
    }

    m_SModel->setVerticalHeaderLabels(VerticalList);
    m_SModel->setHorizontalHeaderLabels(SpeciesNames);
    Estimation_Tab2_CatchTV->setModel(m_SModel);
    Estimation_Tab2_CatchTV->resizeColumnsToContents();

    return true;
}

void
nmfEstimation_Tab2::setHarvestType(std::string harvestType)
{
    m_HarvestType = harvestType;
    if (m_HarvestType == "F") {
        m_HarvestType = "Exploitation";
    } else if (m_HarvestType == "QE") {
        m_HarvestType = "Effort";
    }
}

void
nmfEstimation_Tab2::callback_HarvestFormChanged(QString harvestForm)
{
    setHarvestType(harvestForm.toStdString());
    loadWidgets();
}

