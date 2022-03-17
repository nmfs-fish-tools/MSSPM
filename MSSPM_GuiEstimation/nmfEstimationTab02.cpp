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
    m_ProjectDir  = projectDir;
    m_ModelName.clear();
    m_ProjectName.clear();
    m_NumSignificantDigits = -1;
    m_PreviousEffortUnits.clear();
    m_PreviousCatchUnits.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::nmfEstimation_Tab2");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab02.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab2_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab2_Widget, tr("2. Harvest Data"));

    Estimation_Tab2_CatchTV         = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_CatchTV");
    Estimation_Tab2_EffortTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_EffortTV");
    Estimation_Tab2_PrevPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_PrevPB");
    Estimation_Tab2_NextPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_NextPB");
    Estimation_Tab2_LoadPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_LoadPB");
    Estimation_Tab2_SavePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SavePB");
    Estimation_Tab2_ImportPB        = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ImportPB");
    Estimation_Tab2_ExportPB        = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ExportPB");
    Estimation_Tab2_UnitsEffortCMB  = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_UnitsEffortCMB");
    Estimation_Tab2_ConvertEffortCB = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_ConvertEffortCB");
    Estimation_Tab2_UnitsCatchCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_UnitsCatchCMB");
    Estimation_Tab2_ConvertCatchCB  = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_ConvertCatchCB");
    Estimation_Tab2_EffortW         = Estimation_Tabs->findChild<QWidget     *>("Estimation_Tab2_EffortW");
    Estimation_Tab2_CatchW          = Estimation_Tabs->findChild<QWidget     *>("Estimation_Tab2_CatchW");

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
    connect(Estimation_Tab2_UnitsEffortCMB, SIGNAL(currentTextChanged(QString)),
            this,                           SLOT(callback_UnitsEffortCMB(QString)));
    connect(Estimation_Tab2_UnitsCatchCMB,  SIGNAL(currentTextChanged(QString)),
            this,                           SLOT(callback_UnitsCatchCMB(QString)));

    m_PreviousEffortUnits = Estimation_Tab2_UnitsEffortCMB->currentText();
    m_PreviousCatchUnits  = Estimation_Tab2_UnitsCatchCMB->currentText();

    Estimation_Tab2_PrevPB->setText("\u25C1--");
    Estimation_Tab2_NextPB->setText("--\u25B7");
}

nmfEstimation_Tab2::~nmfEstimation_Tab2()
{

}

bool
nmfEstimation_Tab2::areTablesOK()
{
    return nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab2_CatchTV,
                nmfConstantsMSSPM::DontShowError);
}

void
nmfEstimation_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab2_CatchTV,Estimation_Tab2_EffortTV});
}

QString
nmfEstimation_Tab2::getCurrentUnits()
{
    return Estimation_Tab2_UnitsEffortCMB->currentText();
}

bool
nmfEstimation_Tab2::isConvertCatchChecked()
{
    return Estimation_Tab2_ConvertCatchCB->isChecked();
}

bool
nmfEstimation_Tab2::isConvertEffortChecked()
{
    return Estimation_Tab2_ConvertEffortCB->isChecked();
}

void
nmfEstimation_Tab2::loadCSVFile(const QString& filePath,
                                const QString& tableName,
                                QTableView* tableView)
{
    bool loadOK;
    QString errorMsg;
    QString tableNameStr = tableName.contains(".csv") ? tableName : tableName+".csv";
    std::pair<int,int> nonZeroCell;

    tableNameStr = QDir(filePath).filePath(tableNameStr);

    loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, tableView,
                filePath, tableNameStr,
                nmfConstantsMSSPM::FirstLineNotReadOnly,
                nmfConstantsMSSPM::FixedNotation,
                nmfConstantsMSSPM::DontAllowBlanks,
                nonZeroCell,errorMsg);

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        QMessageBox::warning(Estimation_Tabs,"Time Series Load Error","\n"+errorMsg+"\n", QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "Import Successful",
                                 "\nSuccessful import of Harvest CSV file:\n\n" + tableNameStr + "\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab2::loadUnits(const std::vector<std::string>& tableNames)
{
    int NumRecords;
    std::string units;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"ProjectName","ModelName","TableName","Units"};
    for (int i=0; i<(int)tableNames.size(); ++i) {
        queryStr   = "SELECT ProjectName,ModelName,TableName,Units FROM " +
                nmfConstantsMSSPM::TableUnits +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName +
                "' AND TableName = '"    + tableNames[i] + "'";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Units"].size();
        if (NumRecords == 1) {
            setCurrentUnits(QString::fromStdString(dataMap["Units"][0]));
        } else if (NumRecords > 1) {
            QString msg = "Error reading units table. Found " + QString::number(NumRecords) + " record(s)";
            m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
            QMessageBox::warning(Estimation_Tabs,"Error","\n"+msg+"\n", QMessageBox::Ok);
        }
    }
}

bool
nmfEstimation_Tab2::loadWidgets()
{
    int m;
    int NumSpecies;
    int RunLength;
    int StartYear;
    int NumRecords;
    int NumVisibleTables = (int)m_HarvestTables.size();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList VerticalList;
    QLocale locale(QLocale::English);
    QString valueWithComma;
    QStandardItemModel* smodel;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::loadWidgets()");

    readSettings();

    clearWidgets();

    if (m_ModelName.empty())
        return false;
    if (m_HarvestTables.empty() || (m_HarvestTables[0] == "Null")) {
        m_Logger->logMsg(nmfConstants::Warning,"Warning: Harvest Type set to Null.");
        Estimation_Tab2_EffortW->setVisible(false);
        Estimation_Tab2_CatchW->setVisible(false);
        return true;
    }

    if (NumVisibleTables == 2) {
        Estimation_Tab2_EffortW->setVisible(true);
        Estimation_Tab2_CatchW->setVisible(true);
    } else {
        if (m_HarvestTables[0] == nmfConstantsMSSPM::TableHarvestCatch) {
            Estimation_Tab2_EffortW->setVisible(false);
            Estimation_Tab2_CatchW->setVisible(true);
        } else {
            Estimation_Tab2_EffortW->setVisible(true);
            Estimation_Tab2_CatchW->setVisible(false);
        }
    }

    fields   = {"RunLength","StartYear"};
    queryStr = "SELECT RunLength,StartYear FROM " +
                nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["RunLength"].size() == 0)  {
        m_Logger->logMsg(nmfConstants::Warning,"No records found in Models table.");
        return false;
    }
    RunLength = std::stoi(dataMap["RunLength"][0]);
    StartYear = std::stoi(dataMap["StartYear"][0]);

    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    fields   = {"ProjectName","ModelName","SpeName","Year","Value"};
    for (int i=0; i<NumVisibleTables; ++i) {
        queryStr = "SELECT ProjectName,ModelName,SpeName,Year,Value FROM " +
                m_HarvestTables[i] +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName   +
                "' ORDER BY SpeName,Year ";
        dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        m = 0;
        smodel = new QStandardItemModel( RunLength, NumSpecies );
        VerticalList.clear();
        for (int i=0; i<=RunLength; ++i) {
            VerticalList << " " + QString::number(StartYear+i) + " ";
        }
        for (int j=0; j<NumSpecies; ++j) {
            for (int i=0; i<=RunLength; ++i) {
                if ((m < NumRecords) && (SpeciesNames[j].toStdString() == dataMap["SpeName"][m])) {
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                std::stod(dataMap["Value"][m++]),m_NumSignificantDigits,2);
                    item = new QStandardItem(valueWithComma);
                } else {
                    item = new QStandardItem(QString(""));
                }
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
        }

        smodel->setVerticalHeaderLabels(VerticalList);
        smodel->setHorizontalHeaderLabels(SpeciesNames);
        m_TableViews[i]->setModel(smodel);
        m_TableViews[i]->resizeColumnsToContents();
    }

    loadUnits(m_HarvestTables);

    return true;
}

void
nmfEstimation_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab2::saveCSVFile(std::string& tableName, QTableView* tableView)
{
    // Save time series data to a .csv file
    QString inputDataPath     = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());

    bool okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
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
nmfEstimation_Tab2::setCurrentUnits(QString currentUnits)
{
    Estimation_Tab2_UnitsEffortCMB->setCurrentText(currentUnits);
}

void
nmfEstimation_Tab2::setHarvestType(std::string harvestType)
{
    m_HarvestTables.clear();
    m_TableViews.clear();
    if (harvestType == "Catch") {
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_TableViews = {Estimation_Tab2_CatchTV};
    } else if (harvestType == "Effort (qE)") {
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_TableViews = {Estimation_Tab2_EffortTV};
    } else if (harvestType == "Effort Fit to Catch") {
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_TableViews = {Estimation_Tab2_EffortTV,Estimation_Tab2_CatchTV};
    }
//    else if (harvestType == "Exploitation") {
//        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestExploitation);
//    }
}

void
nmfEstimation_Tab2::callback_ExportPB()
{
    // Save time series data to a .csv file
    int numTables = (int)m_HarvestTables.size();
    std::string tableName;
    std::vector<std::string> tableNames = m_HarvestTables;
    QString msg = "\nOK to use default file name(s) for Harvest .csv file(s) and overwrite any previous file(s)?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Harvest CSV File(s)"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        for (int i=0; i<numTables; ++i) {
            saveCSVFile(m_HarvestTables[i],m_TableViews[i]);
        }
    } else {
        bool ok;
        for (int i=0; i<numTables; ++i) {
            QString effortOrCatch = (QString::fromStdString(m_HarvestTables[i]).contains("effort")) ? "Effort" : "Catch";
            QString msg = "Enter Harvest " + effortOrCatch + " filename version tag: ";
            QString tag = QInputDialog::getText(Estimation_Tabs, tr("Harvest Files"),
                                                tr(msg.toLatin1()), QLineEdit::Normal,
                                                "", &ok);
            if (ok && !tag.isEmpty()) {
                tableName = m_HarvestTables[i] + "_" + tag.toStdString();
                saveCSVFile(tableName,m_TableViews[i]);
            } else if (tag.isEmpty()) {
                QMessageBox::warning(Estimation_Tabs, "Tag Error",
                                     "\nError: Please enter a valid (i.e., non-blank) tag.\n",
                                     QMessageBox::Ok);
            }
        }
    }
}

void
nmfEstimation_Tab2::callback_HarvestFormChanged(QString harvestForm)
{
    setHarvestType(harvestForm.toStdString());
    loadWidgets();
}

void
nmfEstimation_Tab2::callback_ImportPB()
{
    int numTables = (int)m_HarvestTables.size();
    QString tag="";
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // Load default CSV files
    std::string msg = "\nLoad default Harvest .csv file(s)?";
    QString tableName; // = QString::fromStdString(m_HarvestTables);
    QMessageBox::StandardButton reply = QMessageBox::question(
                Estimation_Tabs, tr("Default Harvest CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        for (int i=0; i<numTables; ++i) {
            loadCSVFile(inputDataPath,QString::fromStdString(m_HarvestTables[i]),m_TableViews[i]);
        }
    } else {
        // if no, raise browser and have user select the Harvest files.
        for (int i=0; i<numTables; ++i) {
            QString effortOrCatch = (QString::fromStdString(m_HarvestTables[i]).contains("effort")) ? "Effort" : "Catch";
            QString fileTypes = (effortOrCatch == "Effort") ? "harvesteffort*.csv HarvestEffort*.csv" :
                                                              "harvestcatch*.csv  HarvestCatch*.csv";
            QString msg = "Select Harvest " + effortOrCatch + " file";
            QString filenameWithPath = QFileDialog::getOpenFileName(
                        Estimation_Tabs,
                        QObject::tr(msg.toLatin1()), inputDataPath,
                        QObject::tr("Data Files (" + fileTypes.toLatin1() + ")"));
            if (! filenameWithPath.isEmpty()) {
                QFileInfo fi(filenameWithPath);
                QString fileName     = fi.fileName();
                QString absolutePath = fi.absolutePath();
                if (absolutePath == inputDataPath) {
                    if (filenameWithPath.contains("_")) {
                        if (nmfUtilsQt::extractTag(fi.baseName(),tag)) {
                            tableName = QString::fromStdString(m_HarvestTables[i]) + "_" + tag;
                            loadCSVFile(absolutePath,tableName,m_TableViews[i]);
                        } else {
                            QMessageBox::information(Estimation_Tabs, "Harvest CSV Import",
                                                     "\nPlease make sure to select the Harvest file containing the desired tag\n",
                                                     QMessageBox::Ok);
                        }
                    } else if (! filenameWithPath.isEmpty()){
                        QMessageBox::critical(Estimation_Tabs, "Harvest CSV Import",
                                              "\nError: No tag found in filename\n",
                                              QMessageBox::Ok);
                    }
                } else {
                    loadCSVFile(absolutePath,fileName,m_TableViews[i]);
                }
            } else {
                return;
            }
        }
    }
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
nmfEstimation_Tab2::callback_LoadWidgets()
{
    loadWidgets();
}

void
nmfEstimation_Tab2::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab2::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab2::callback_SavePB()
{
    int numTables = (int)m_HarvestTables.size();
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::string harvestTable;
    std::vector<std::string> SpeNames;
    std::vector<std::string> modelsInProject = {};
    QString msg;
    QString value;
    QString valueWithoutComma;
    QStandardItemModel* smodel;

    if (! m_DatabasePtr->updateAllModelsInProject(
                Estimation_Tabs,"Harvest",m_ProjectName,m_ModelName,modelsInProject)) {
        return;
    }
    if (numTables == 0) {
        return;
    }
    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Get the species names from one of the tables
    smodel = qobject_cast<QStandardItemModel*>(m_TableViews[0]->model());
    for (int j=0; j<smodel->columnCount(); ++ j) {
        SpeNames.push_back(smodel->horizontalHeaderItem(j)->text().toStdString());
    }

    // Check data integrity
//    for (int j=0; j<m_SModel->columnCount(); ++j) { // Species
//        for (int i=0; i<m_SModel->rowCount(); ++i) { // Time
//            index = m_SModel->index(i,j);
//            value = index.data().toString();
//            if (value.contains(',')) {
//                msg = "Invalid value found. No commas or special characters allowed in a number.";
//                m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
//                QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
//                Estimation_Tabs->setCursor(Qt::ArrowCursor);
//                return;
//            }
//        }
//    }

    // Check harvest table for blanks
    for (QTableView* harvestTable : {Estimation_Tab2_EffortTV,Estimation_Tab2_CatchTV}) {
        if (harvestTable->isVisible()) {
            if (! nmfUtilsQt::checkTableForBlanks(harvestTable)) {
                msg = "No blanks allowed in harvest table: " + harvestTable->objectName();
                m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
                QMessageBox::warning(Estimation_Tabs, "Save Error", "\n"+msg, QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    for (int i=0; i<numTables; ++i)
    {
        for (std::string projectModel : modelsInProject)
        {
            smodel = qobject_cast<QStandardItemModel*>(m_TableViews[i]->model());
            harvestTable = m_HarvestTables[i];
            cmd = "DELETE FROM " +
                    m_HarvestTables[i] +
                    " WHERE ProjectName = '" + m_ProjectName +
                    "' AND ModelName = '"    + projectModel  + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command. Couldn't delete all records from" +
                                     QString::fromStdString(harvestTable) + " table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
            cmd = "INSERT INTO " + harvestTable + " (ProjectName,ModelName,SpeName,Year,Value) VALUES ";
            for (int j=0; j<smodel->columnCount(); ++j) {  // Species
                for (int i=0; i<smodel->rowCount(); ++i) { // Time
                    index = smodel->index(i,j);
                    valueWithoutComma = index.data().toString().remove(",");
                    cmd += "('"  + m_ProjectName + "','" + projectModel +
                            "','" + SpeNames[j] + "'," + std::to_string(i) +
                            ", " + valueWithoutComma.toStdString() + "),";
                    //      ", " + std::to_string(valueWithoutComma.toDouble()) + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::callback_SavePB: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command. Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            m_DatabasePtr->updateUnitsTable(
                        Estimation_Tabs,m_Logger,m_ProjectName,projectModel,
                        harvestTable,getCurrentUnits().toStdString());
        }

        m_TableViews[i]->resizeColumnsToContents();

        QMessageBox::information(Estimation_Tabs, QString::fromStdString(harvestTable) + " Updated",
                                 "\n" + QString::fromStdString(harvestTable) +
                                 " table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }
    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfEstimation_Tab2::callback_UnitsEffortCMB(QString currentEffortUnits)
{
    if (isConvertEffortChecked()) {
        nmfUtilsQt::convertTableView(Estimation_Tab2_EffortTV,
                                     m_NumSignificantDigits,
                                     m_PreviousEffortUnits,currentEffortUnits);
    }
    m_PreviousEffortUnits = currentEffortUnits;
}

void
nmfEstimation_Tab2::callback_UnitsCatchCMB(QString currentCatchUnits)
{
    if (isConvertCatchChecked()) {
        nmfUtilsQt::convertTableView(Estimation_Tab2_CatchTV,
                                     m_NumSignificantDigits,
                                     m_PreviousCatchUnits,currentCatchUnits);
    }
    m_PreviousCatchUnits = currentCatchUnits;
}
