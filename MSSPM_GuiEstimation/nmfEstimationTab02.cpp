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
    m_GroupBoxTitle["Exploitation"] = nmfConstantsMSSPM::OutputChartExploitation.toStdString();

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab02.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab2_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab2_Widget, tr("2. Harvest Data"));

    Estimation_Tab2_HarvestTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_HarvestTV");
    Estimation_Tab2_HarvestGB  = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab2_HarvestGB");
    Estimation_Tab2_PrevPB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_PrevPB");
    Estimation_Tab2_NextPB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_NextPB");
    Estimation_Tab2_LoadPB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_LoadPB");
    Estimation_Tab2_SavePB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SavePB");
    Estimation_Tab2_ImportPB   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ImportPB");
    Estimation_Tab2_ExportPB   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ExportPB");
    Estimation_Tab2_HarvestLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab2_HarvestLBL");

    connect(Estimation_Tab2_PrevPB,   SIGNAL(clicked(bool)),
            this,                     SLOT(callback_PrevPB()));
    connect(Estimation_Tab2_NextPB,   SIGNAL(clicked(bool)),
            this,                     SLOT(callback_NextPB()));
    connect(Estimation_Tab2_LoadPB,   SIGNAL(clicked(bool)),
            this,                     SLOT(callback_LoadPB()));
    connect(Estimation_Tab2_SavePB,   SIGNAL(clicked(bool)),
            this,                     SLOT(callback_SavePB()));
    connect(Estimation_Tab2_ImportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ImportPB()));
    connect(Estimation_Tab2_ExportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ExportPB()));

    Estimation_Tab2_PrevPB->setText("\u25C1--");
    Estimation_Tab2_NextPB->setText("--\u25B7");
}


nmfEstimation_Tab2::~nmfEstimation_Tab2()
{

}

void
nmfEstimation_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab2_HarvestTV});
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
nmfEstimation_Tab2::callback_LoadWidgets()
{
    loadWidgets();
}

void
nmfEstimation_Tab2::callback_LoadPB()
{
    if (loadWidgets()) {
        QMessageBox::information(Estimation_Tabs, "Harvest Load",
                                 "\nHarvest table(s) successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab2::callback_ImportPB()
{
    QString tag="";
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // Load default CSV files
    std::string msg = "\nLoad default Harvest .csv file?";
    std::string tableName = m_HarvestType;
    QMessageBox::StandardButton reply = QMessageBox::question(
                Estimation_Tabs, tr("Default Harvest CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        loadCSVFile(tableName);
    } else {
        // if no, raise browser and have user select the Harvest file.
        QString filename = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr("Select Harvest file"), inputDataPath,
                    QObject::tr("Data Files (*.csv)"));
        if (filename.contains("_")) {
            QFileInfo fi(filename);
            if (nmfUtilsQt::extractTag(fi.baseName(),tag)) {
                tableName += "_"+tag.toStdString();
                loadCSVFile(tableName);
            } else {
                QMessageBox::information(Estimation_Tabs, "Harvest CSV Import",
                                         "\nPlease make sure to select the Harvest file containing the desired tag\n",
                                         QMessageBox::Ok);
            }
        } else {
            QMessageBox::critical(Estimation_Tabs, "Harvest CSV Import",
                                  "\nError: No tag found in filename\n",
                                  QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab2::loadCSVFile(std::string& tableName)
{
    bool loadOK;
    QString errorMsg;
    QString tableNameStr;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::pair<int,int> nonZeroCell;

    tableNameStr = QString::fromStdString(tableName);
    tableNameStr = QDir(inputDataPath).filePath(tableNameStr+".csv");
std::cout << "input: " << tableNameStr.toStdString() << std::endl;

    loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, Estimation_Tab2_HarvestTV, inputDataPath, tableNameStr,
                nmfConstantsMSSPM::FirstLineNotReadOnly,
                nmfConstantsMSSPM::FixedNotation,
                nonZeroCell,errorMsg);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }
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
    if (nmfUtilsQt::isAnError(errorMsg)) {
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
//          value = index.data().toString();
            cmd += "('" + MohnsRhoLabel +
                   "','" + m_ProjectSettingsConfig +
                   "','" + SpeNames[j] + "'," + std::to_string(i) +
                    ", " + std::to_string(index.data().toDouble()) + "),";
//                  ", " + value.toStdString() + "),";
        }
    }

    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Estimation_Tab2_HarvestTV->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, QString::fromStdString(m_HarvestType) + " Updated",
                             "\n" + QString::fromStdString(m_HarvestType) +
                             " table has been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}


void
nmfEstimation_Tab2::callback_ExportPB()
{
    // Save time series data to a .csv file
    std::string tableName = m_HarvestType;
    QString msg = "\nOK to use default file name for Harvest .csv file and overwrite any previous file?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Harvest CSV File"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        saveCSVFile(tableName);
    } else {
        bool ok;
        QString tag = QInputDialog::getText(Estimation_Tabs, tr("Harvest Files"),
                                            tr("Enter Harvest CSV filename version tag: "), QLineEdit::Normal,
                                            "", &ok);
        if (ok && !tag.isEmpty()) {
            tableName += "_"+tag.toStdString();
            saveCSVFile(tableName);
        } else if (tag.isEmpty()) {
            QMessageBox::warning(Estimation_Tabs, "Tag Error",
                                 "\nError: Please enter a valid (i.e., non-blank) tag.\n",
                                 QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab2::saveCSVFile(std::string& tableName)
{
    bool okSave;
    QString tableNameWithPath;

    // Save time series data to a .csv file
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,m_SModel,inputDataPath,tableNameWithPath);

    if (okSave) {
        QMessageBox::information(Estimation_Tabs, "Harvest File Saved",
                                 "\nHarvest CSV file has been successfully saved as:\n\n"+tableNameWithPath+"\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "Harvest File Saved",
                                 "\nError: Harvest CSV file has not been saved. Please enter a valid (i.e., non-blank) filename.\n",
                                 QMessageBox::Ok);
    }
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

    readSettings();

    QString SystemName = QString::fromStdString(m_ProjectSettingsConfig);
    // Strip off the MohnsRho suffix
    auto parts = SystemName.split("__");
    SystemName =  parts[0];

    clearWidgets();

    if (SystemName.isEmpty())
        return false;
    if (m_HarvestType.empty() || (m_HarvestType == "Null")) {
        m_Logger->logMsg(nmfConstants::Warning,"Warning: Harvest Type set to Null.");
        Estimation_Tab2_HarvestTV->setEnabled(false);
        return true;
    }

    Estimation_Tab2_HarvestTV->setEnabled(true);

    Estimation_Tab2_HarvestGB->setTitle(QString::fromStdString(m_GroupBoxTitle[m_HarvestType]));

    fields   = {"RunLength","StartYear"};
    queryStr = "SELECT RunLength,StartYear FROM Systems where SystemName = '" + SystemName.toStdString() + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["RunLength"].size() == 0)  {
        m_Logger->logMsg(nmfConstants::Warning,"No records found in Systems table.");
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
    Estimation_Tab2_HarvestTV->setModel(m_SModel);
    Estimation_Tab2_HarvestTV->resizeColumnsToContents();
    Estimation_Tab2_HarvestLBL->setText(QString::fromStdString(m_HarvestType+":").replace("Harvest",""));

    return true;
}

void
nmfEstimation_Tab2::setHarvestType(std::string harvestType)
{
    m_HarvestType = "Harvest"+(QString::fromStdString(harvestType).split(" ")[0]).toStdString();
}

void
nmfEstimation_Tab2::callback_HarvestFormChanged(QString harvestForm)
{
    setHarvestType(harvestForm.toStdString());
    loadWidgets();
}

bool
nmfEstimation_Tab2::areTablesOK()
{
    return nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab2_HarvestTV,
                nmfConstantsMSSPM::DontShowError);
}

