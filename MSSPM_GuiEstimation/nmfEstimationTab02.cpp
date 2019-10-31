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

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_smodel      = nullptr;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    m_logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::nmfEstimation_Tab2");

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
    Estimation_Tab2_CatchTV->reset();
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

    if (m_smodel == nullptr) {
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    for (int j=0; j<m_smodel->columnCount(); ++ j) {
        SpeNames.push_back(m_smodel->horizontalHeaderItem(j)->text().toStdString());
    }

    // Check data integrity
    for (int j=0; j<m_smodel->columnCount(); ++j) { // Species
        for (int i=0; i<m_smodel->rowCount(); ++i) { // Time
            index = m_smodel->index(i,j);
            value = index.data().toString();
            if (value.contains(',')) {
                msg = "Invalid value found. No commas or special characters allowed in a number.";
                m_logger->logMsg(nmfConstants::Error,msg.toStdString());
                QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    cmd = "DELETE FROM " + m_harvestType + " WHERE SystemName = '" +
           m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from" +
                             QString::fromStdString(m_harvestType) + " table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }
    cmd = "INSERT INTO " + m_harvestType + " (MohnsRhoLabel,SystemName,SpeName,Year,Value) VALUES ";
    for (int j=0; j<m_smodel->columnCount(); ++j) { // Species
        for (int i=0; i<m_smodel->rowCount(); ++i) { // Time
            index = m_smodel->index(i,j);
            value = index.data().toString();
            cmd += "('" + MohnsRhoLabel +
                   "','" + m_ProjectSettingsConfig +
                   "','" + SpeNames[j] + "'," + std::to_string(i) +
                   ", " + value.toStdString() + "),";
        }
    }

    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Estimation_Tab2_CatchTV->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, QString::fromStdString(m_harvestType) + " Updated",
                             "\n" + QString::fromStdString(m_harvestType) +
                             " table has been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfEstimation_Tab2::readSettings()
{
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Settings");
    m_ProjectSettingsConfig = settings.value("Name","").toString().toStdString();
    settings.endGroup();

    settings.beginGroup("SetupTab");
    m_ProjectDir = settings.value("ProjectDir","").toString().toStdString();
    settings.endGroup();
}

bool
nmfEstimation_Tab2::loadWidgets()
{
std::cout << "nmfEstimation_Tab2::loadWidgets()" << std::endl;

    loadWidgets("");

    return true;
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
    if (m_harvestType.empty() || (m_harvestType == "Null")) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: Harvest Type set to Null.");
        return true;
    }

    Estimation_Tab2_CatchGB->setTitle(QString::fromStdString(m_GroupBoxTitle[m_harvestType]));

    fields   = {"RunLength","StartYear"};
    queryStr = "SELECT RunLength,StartYear FROM Systems where SystemName = '" + SystemName.toStdString() + "'";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["RunLength"].size() == 0)  {
        std::cout << "Error: No records found in Systems table." << std::endl;
        return false;
    }
    RunLength = std::stoi(dataMap["RunLength"][0]);
    StartYear = std::stoi(dataMap["StartYear"][0]);

    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    fields   = {"MohnsRhoLabel","SystemName","SpeName","Year","Value"};
    queryStr = "SELECT MohnsRhoLabel,SystemName,SpeName,Year,Value FROM " + m_harvestType +
               " WHERE SystemName = '" + SystemName.toStdString() +
               "' AND MohnsRhoLabel = '" + MohnsRhoLabel.toStdString() +
               "' ORDER BY SpeName,Year ";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    m = 0;
    m_smodel = new QStandardItemModel( RunLength, NumSpecies );
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
            m_smodel->setItem(i, j, item);
        }
    }

    m_smodel->setVerticalHeaderLabels(VerticalList);
    m_smodel->setHorizontalHeaderLabels(SpeciesNames);
    Estimation_Tab2_CatchTV->setModel(m_smodel);
    Estimation_Tab2_CatchTV->resizeColumnsToContents();

    return true;
}

void
nmfEstimation_Tab2::setHarvestType(std::string harvestType)
{
    m_harvestType = harvestType;
    if (m_harvestType == "F") {
        m_harvestType = "Exploitation";
    } else if (m_harvestType == "QE") {
        m_harvestType = "Effort";
    }
}

void
nmfEstimation_Tab2::callback_HarvestFormChanged(QString harvestForm)
{
    setHarvestType(harvestForm.toStdString());
    loadWidgets();
}

