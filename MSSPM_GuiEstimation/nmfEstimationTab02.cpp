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
    m_HarvestType.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::nmfEstimation_Tab2");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab02.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab2_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab2_Widget, tr("2. Harvest Data"));
    Estimation_Tab2_FitToCatchTB              = Estimation_Tabs->findChild<QTabWidget  *>("Estimation_Tab2_FitToCatchTB");
    Estimation_Tab2_WeightsFTCTV              = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_WeightsFTCTV");
    Estimation_Tab2_CatchTV                   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_CatchTV");
    Estimation_Tab2_CatchFTCTV                = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_CatchFTCTV");
    Estimation_Tab2_EffortTV                  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_EffortTV");
    Estimation_Tab2_EffortFTCTV               = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab2_EffortFTCTV");
    Estimation_Tab2_SetAllWeightsPB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_FTCWeightsSetAllPB");
    Estimation_Tab2_PrevPB                    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_PrevPB");
    Estimation_Tab2_NextPB                    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_NextPB");
    Estimation_Tab2_LoadPB                    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_LoadPB");
    Estimation_Tab2_SavePB                    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SavePB");
    Estimation_Tab2_ImportPB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ImportPB");
    Estimation_Tab2_ExportPB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_ExportPB");
    Estimation_Tab2_SetBiomassWeightsToHalfPB = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SetBiomassWeightsToHalfPB");
    Estimation_Tab2_SetBiomassWeightsTo1PB    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SetBiomassWeightsTo1PB");
    Estimation_Tab2_SetBiomassWeightsTo0PB    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab2_SetBiomassWeightsTo0PB");
    Estimation_Tab2_UnitsEffortCMB            = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_UnitsEffortCMB");
    Estimation_Tab2_FTCUnitsEffortCMB         = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_FTCUnitsEffortCMB");
    Estimation_Tab2_ConvertEffortCB           = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_ConvertEffortCB");
    Estimation_Tab2_FTCConvertEffortCB        = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_FTCConvertEffortCB");
    Estimation_Tab2_UnitsCatchCMB             = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_UnitsCatchCMB");
    Estimation_Tab2_FTCUnitsCatchCMB          = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab2_FTCUnitsCatchCMB");
    Estimation_Tab2_ConvertCatchCB            = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_ConvertCatchCB");
    Estimation_Tab2_FTCConvertCatchCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab2_FTCConvertCatchCB");
    Estimation_Tab2_EffortW                   = Estimation_Tabs->findChild<QWidget     *>("Estimation_Tab2_EffortW");
    Estimation_Tab2_CatchW                    = Estimation_Tabs->findChild<QWidget     *>("Estimation_Tab2_CatchW");

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
    connect(Estimation_Tab2_SetBiomassWeightsTo1PB, SIGNAL(clicked()),
            this,                                   SLOT(callback_SetBiomassWeightsTo1PB()));
    connect(Estimation_Tab2_SetBiomassWeightsTo0PB, SIGNAL(clicked()),
            this,                                   SLOT(callback_SetBiomassWeightsTo0PB()));
    connect(Estimation_Tab2_SetBiomassWeightsToHalfPB,SIGNAL(clicked(bool)),
            this,                                     SLOT(callback_SetWeightsEqualPB()));
    connect(Estimation_Tab2_SetAllWeightsPB,    SIGNAL(clicked(bool)),
            this,                               SLOT(callback_SetAllWeightsPB()));
    connect(Estimation_Tab2_FTCUnitsEffortCMB,  SIGNAL(currentTextChanged(QString)),
            this,                               SLOT(callback_UnitsEffortCMB(QString)));
    connect(Estimation_Tab2_FTCUnitsCatchCMB,   SIGNAL(currentTextChanged(QString)),
            this,                               SLOT(callback_UnitsCatchCMB(QString)));
    connect(Estimation_Tab2_UnitsEffortCMB,     SIGNAL(currentTextChanged(QString)),
            this,                               SLOT(callback_UnitsEffortCMB(QString)));
    connect(Estimation_Tab2_UnitsCatchCMB,      SIGNAL(currentTextChanged(QString)),
            this,                               SLOT(callback_UnitsCatchCMB(QString)));

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
    return (nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab2_CatchTV,
                nmfConstantsMSSPM::DontShowError) &&
            nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab2_CatchFTCTV,
                nmfConstantsMSSPM::DontShowError));
}

void
nmfEstimation_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab2_CatchTV,
                                Estimation_Tab2_CatchFTCTV,
                                Estimation_Tab2_EffortTV,
                                Estimation_Tab2_EffortFTCTV});
}

QString
nmfEstimation_Tab2::getCurrentUnits()
{
    QString currentUnits;

    if (isEffort()) {
        currentUnits = Estimation_Tab2_UnitsEffortCMB->currentText();
    } else if (isCatch()) {
        currentUnits = Estimation_Tab2_UnitsCatchCMB->currentText();
    } else {
        switch (Estimation_Tab2_FitToCatchTB->currentIndex()) {
          case 1: // Effort
            currentUnits = Estimation_Tab2_FTCUnitsEffortCMB->currentText();
            break;
          case 2:
            currentUnits = Estimation_Tab2_FTCUnitsCatchCMB->currentText();
            break;
        }
    }

    return currentUnits;
}

bool
nmfEstimation_Tab2::isCatch()
{
    return Estimation_Tab2_CatchW->isVisible();
}

bool
nmfEstimation_Tab2::isEffort()
{
    return Estimation_Tab2_EffortW->isVisible();
}

bool
nmfEstimation_Tab2::isFitToCatch()
{
   return (! Estimation_Tab2_CatchW->isVisible() &&
           ! Estimation_Tab2_EffortW->isVisible());
}

bool
nmfEstimation_Tab2::isConvertCatchChecked()
{
    bool retv = false;

    if (isCatch()) {
        retv = Estimation_Tab2_ConvertCatchCB->isChecked();
    } else if (isFitToCatch()) {
        retv = Estimation_Tab2_FTCConvertCatchCB->isChecked();
    }

    return retv;
}

bool
nmfEstimation_Tab2::isConvertEffortChecked()
{
    bool retv = false;

    if (isEffort()) {
        retv = Estimation_Tab2_ConvertEffortCB->isChecked();
    } else if (isFitToCatch()) {
        retv = Estimation_Tab2_FTCConvertEffortCB->isChecked();
    }

    return retv;
}

bool
nmfEstimation_Tab2::loadCSVFile(const QString& filePath,
                                const QString& tableName,
                                QTableView* tableView,
                                const bool& verbose)
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
        if (verbose) {
            QMessageBox::warning(Estimation_Tabs,"Time Series Load Error","\n"+errorMsg+"\n", QMessageBox::Ok);
        }
    } else {
        if (verbose) {
            QMessageBox::information(Estimation_Tabs, "Import Successful",
                                     "\nSuccessful import of Harvest CSV file:\n\n" + tableNameStr + "\n",
                                     QMessageBox::Ok);
        }
    }

    return loadOK;
}

void
nmfEstimation_Tab2::loadUnits(const std::string& tableName)
{
    int NumRecords;
    std::string units;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"ProjectName","ModelName","TableName","Units"};
    queryStr = "SELECT ProjectName,ModelName,TableName,Units FROM " +
            nmfConstantsMSSPM::TableUnits +
            " WHERE ProjectName = '" + m_ProjectName +
            "' AND ModelName = '"    + m_ModelName +
            "' AND TableName = '"    + tableName + "'";
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

bool
nmfEstimation_Tab2::loadWeights(const bool& verbose)
{
    int NumSpecies;
    bool retv = true;
    QStringList SpeciesList;
    QStringList WeightNameList = {"Biomass","Catch"};
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName = nmfConstantsMSSPM::TableFitWeights;

    // Find species and create model
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }
    QStandardItemModel* smodel = new QStandardItemModel(NumSpecies,WeightNameList.size());

    // Populate model with table data
    fields     = {"ProjectName","ModelName","SpeName","Biomass","Catch"};
    queryStr   = "SELECT ProjectName,ModelName,SpeName,Biomass,Catch FROM " + tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["SpeName"].size();
    if (NumRecords == NumSpecies) {
        for (int row=0; row<NumRecords; ++row) {
            QStandardItem* firstItem  = new QStandardItem(QString::fromStdString(dataMap["Biomass"][row]));
            QStandardItem* secondItem = new QStandardItem(QString::fromStdString(dataMap["Catch"][row]));
            firstItem->setTextAlignment(Qt::AlignCenter);
            secondItem->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,0,firstItem);
            smodel->setItem(row,1,secondItem);
        }
    } else if (NumRecords == 0) { // load and save default data
        for (int row=0; row<NumSpecies; ++row) {
            QStandardItem* firstItem  = new QStandardItem("0.5");
            QStandardItem* secondItem = new QStandardItem("0.5");
            firstItem->setTextAlignment(Qt::AlignCenter);
            secondItem->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,0,firstItem);
            smodel->setItem(row,1,secondItem);
        }
        saveFitWeights(nmfConstantsMSSPM::VerboseOff);
    } else {
        QString msg = "Discrepancy reading " + QString::fromStdString(tableName) +
                      " table. Found " + QString::number(NumRecords) + " record(s) and " +
                      QString::number(NumSpecies) + " species.";
        m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        if (verbose) {
            QMessageBox::warning(Estimation_Tabs,"Error","\n"+msg+"\n", QMessageBox::Ok);
        }
    }
    smodel->setVerticalHeaderLabels(SpeciesList);
    smodel->setHorizontalHeaderLabels(WeightNameList);
    Estimation_Tab2_WeightsFTCTV->setModel(smodel);
    Estimation_Tab2_WeightsFTCTV->resizeColumnsToContents();

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, "Harvest Load",
                             "\nSuccessful load of table: " +
                             QString::fromStdString(tableName) + "\n",
                             QMessageBox::Ok);
    }

    // Set the callback for when the user changes a cell value
    connect(smodel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(callback_SetWeightsPB(const QModelIndex&, const QModelIndex&)));

    return retv;
}

bool
nmfEstimation_Tab2::
loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab2::loadWidgets()");

    readSettings();

    clearWidgets();

    if (m_ModelName.empty()) {
        return false;
    }

    loadTable(Estimation_Tab2_CatchTV,    nmfConstantsMSSPM::TableHarvestCatch, nmfConstantsMSSPM::VerboseOff);
    loadTable(Estimation_Tab2_CatchFTCTV, nmfConstantsMSSPM::TableHarvestCatch, nmfConstantsMSSPM::VerboseOff);
    loadTable(Estimation_Tab2_EffortTV,   nmfConstantsMSSPM::TableHarvestEffort,nmfConstantsMSSPM::VerboseOff);
    loadTable(Estimation_Tab2_EffortFTCTV,nmfConstantsMSSPM::TableHarvestEffort,nmfConstantsMSSPM::VerboseOff);
    loadWeights(nmfConstantsMSSPM::VerboseOff);

    return true;
}

bool
nmfEstimation_Tab2::loadTable(QTableView* tableView,
                              const std::string& tableName,
                              const bool& verbose)
{
    int RunLength;
    int StartYear;
    int NumRecords;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList VerticalList;
    QStringList SpeciesList;
    QLocale locale(QLocale::English);
    QString valueWithComma;
    QStandardItemModel* smodel;

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

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    // Read table and set headers
    fields   = {"ProjectName","ModelName","SpeName","Year","Value"};
    queryStr = "SELECT ProjectName,ModelName,SpeName,Year,Value FROM " + tableName +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName   +
               "' ORDER BY SpeName,Year ";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    smodel = new QStandardItemModel( RunLength, NumSpecies );
    VerticalList.clear();
    for (int i=0; i<=RunLength; ++i) {
        VerticalList << " " + QString::number(StartYear+i) + " ";
    }

    // Populate model with table data
    int m = 0;
    for (int j=0; j<NumSpecies; ++j) {
        for (int i=0; i<=RunLength; ++i) {
            if ((m < NumRecords) && (SpeciesList[j].toStdString() == dataMap["SpeName"][m])) {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            std::stod(dataMap["Value"][m++]),m_NumSignificantDigits,6);
                item = new QStandardItem(valueWithComma);
            } else {
                item = new QStandardItem(QString(""));
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
    }

    smodel->setVerticalHeaderLabels(VerticalList);
    smodel->setHorizontalHeaderLabels(SpeciesList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();

    loadUnits(tableName);

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, "Harvest Load",
                                 "\nSuccessful load of table: " +
                                 QString::fromStdString(tableName) + "\n",
                                 QMessageBox::Ok);
    }

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

bool
nmfEstimation_Tab2::saveCSVFile(std::string& tableName,
                                QTableView* tableView,
                                const bool& verbose)
{
    // Save time series data to a .csv file
    QString inputDataPath     = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
//std::cout << "Saving: " << tableView->objectName().toStdString() << ", tableName: " << tableName <<
//             ", smodel(0,0): " << smodel->index(0,0).data().toDouble() << std::endl;
    bool okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
    if (verbose) {
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
    return okSave;
}

void
nmfEstimation_Tab2::setCurrentUnits(QString currentUnits)
{
    Estimation_Tab2_UnitsEffortCMB->setCurrentText(currentUnits);
}

QStandardItemModel*
nmfEstimation_Tab2::getHarvestModel()
{
    QStandardItemModel* smodel = nullptr;

    if (m_HarvestType == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_CatchTV->model());
    } else if (m_HarvestType == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_EffortTV->model());
    }

    return smodel;
}

QStandardItemModel*
nmfEstimation_Tab2::getHarvestModelFTCCatch()
{
    return qobject_cast<QStandardItemModel*>(Estimation_Tab2_CatchFTCTV->model());
}

QStandardItemModel*
nmfEstimation_Tab2::getHarvestModelFTCEffort()
{
    return qobject_cast<QStandardItemModel*>(Estimation_Tab2_EffortFTCTV->model());
}

void
nmfEstimation_Tab2::setHarvestType(std::string harvestType)
{
    m_HarvestType = harvestType;
    m_HarvestTables.clear();
    m_TableViews.clear();
    Estimation_Tab2_FitToCatchTB->setVisible(false);
    Estimation_Tab2_CatchW->setVisible(false);
    Estimation_Tab2_EffortW->setVisible(false);

    if (harvestType == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        Estimation_Tab2_CatchW->setVisible(true);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_TableViews = {Estimation_Tab2_CatchTV,
                        Estimation_Tab2_CatchFTCTV};
    } else if (harvestType == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        Estimation_Tab2_EffortW->setVisible(true);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_TableViews = {Estimation_Tab2_EffortTV,
                        Estimation_Tab2_EffortFTCTV};
    } else if (harvestType == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        Estimation_Tab2_FitToCatchTB->setVisible(true);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestEffort);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestCatch);
        m_TableViews = {Estimation_Tab2_EffortTV,
                        Estimation_Tab2_EffortFTCTV,
                        Estimation_Tab2_CatchTV,
                        Estimation_Tab2_CatchFTCTV};
    }
//    else if (harvestType == "Exploitation") {
//        m_HarvestTables.push_back(nmfConstantsMSSPM::TableHarvestExploitation);
//    }
}

void
nmfEstimation_Tab2::callback_ExportPB()
{
    // Save time series data to a .csv file
    bool ok;
    bool allSaveOK = true;
    bool saveOK;
    int numTables = (int)m_HarvestTables.size();
//  bool isFitToCatch = (numTables == 2);
    std::string tableName;
    std::vector<std::string> tableNames = m_HarvestTables;
    QString msg = "\nOK to use default file name(s) for Harvest .csv file(s) and overwrite any previous file(s)?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Harvest CSV File(s)"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
//std::cout << "num tables: " << numTables << std::endl;
//std::cout << "isFitToCatch: " << isFitToCatch() << std::endl;
//for (int i=0; i<tableNames.size();++i) {
//std::cout << "-> " << tableNames[i] << std::endl;
//}
//for (int i=0;i<m_HarvestTables.size(); ++i) {
// std::cout << "table name: " << m_HarvestTables[i] << std::endl;
//}
    // Temporarily add the FitWeights table to the table lists if Fit to Catch
    if (isFitToCatch()) {
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableFitWeights);
        m_TableViews.push_back(Estimation_Tab2_WeightsFTCTV);
        ++numTables;
    }

    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        for (int i=0; i<numTables; ++i) {
            saveOK = saveCSVFile(m_HarvestTables[i],m_TableViews[i],nmfConstantsMSSPM::VerboseOff);
            allSaveOK = (! saveOK) ? false : allSaveOK;
        }
        if (allSaveOK) {
            QMessageBox::information(Estimation_Tabs, "Export Successful",
                                     "\nSuccessful export of Harvest data tables.\n",
                                     QMessageBox::Ok);
        } else {
            QMessageBox::warning(Estimation_Tabs,"Error","\nTime Series Save Error\n", QMessageBox::Ok);
        }
    } else {
        for (int i=0; i<numTables; i+=2) {
            QString effortOrCatch = "FitWeights";
            if (QString::fromStdString(m_HarvestTables[i]).contains("effort")) {
                effortOrCatch = "Harvest Effort";
            } else if (QString::fromStdString(m_HarvestTables[i]).contains("catch")) {
                effortOrCatch = "Harvest Catch";
            }

            QString msg = "Enter " + effortOrCatch + " filename version tag: ";
            QString tag = QInputDialog::getText(Estimation_Tabs, tr("Harvest Files"),
                                                tr(msg.toLatin1()), QLineEdit::Normal,"", &ok);
            if (ok) {
                if (! tag.isEmpty()) {
                    tableName = m_HarvestTables[i] + "_" + tag.toStdString();
//std::cout << "1 tableNName: " << tableName << std::endl;
                    saveCSVFile(tableName,m_TableViews[i],nmfConstantsMSSPM::VerboseOff);
                } else if (tag.isEmpty()) {
                    QMessageBox::warning(Estimation_Tabs, "Tag Error",
                                         "\nError: Please enter a valid (i.e., non-blank) tag.\n",
                                         QMessageBox::Ok);
                }
            }
        }
//std::cout << "2 tableNName: " << tableName << std::endl;
        if (isFitToCatch()) {
            saveCSVFile(tableName,Estimation_Tab2_WeightsFTCTV,nmfConstantsMSSPM::VerboseOff);
        }
    }

    // Now remove the FitWeights tables to the table lists
    if (isFitToCatch()) {
        m_HarvestTables.pop_back();
        m_TableViews.pop_back();
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
    bool allLoadOK = true;
    bool loadOK;
    QString tag="";
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // Load default CSV files
    std::string msg = "\nLoad default Harvest .csv file(s)?";
    QString tableName; // = QString::fromStdString(m_HarvestTables);
    QMessageBox::StandardButton reply = QMessageBox::question(
                Estimation_Tabs, tr("Default Harvest CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);

    // Temporarily add the FitWeights table to the table lists
    if (isFitToCatch()) {
        m_HarvestTables.push_back(nmfConstantsMSSPM::TableFitWeights);
        m_TableViews.push_back(Estimation_Tab2_WeightsFTCTV);
        ++numTables;
    }

    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        for (int i=0; i<numTables; ++i) {
            loadOK = loadCSVFile(inputDataPath,QString::fromStdString(m_HarvestTables[i]),m_TableViews[i],
                        nmfConstantsMSSPM::VerboseOff);
            allLoadOK = (! loadOK) ? false : allLoadOK;
        }
        if (allLoadOK) {
            QMessageBox::information(Estimation_Tabs, "Import Successful",
                                     "\nSuccessful import of Harvest file(s)\n",
                                     QMessageBox::Ok);
        } else {
            QMessageBox::warning(Estimation_Tabs,"Error","\nTime Series Load Error\n", QMessageBox::Ok);
        }
    } else {
        // if no, raise browser and have user select the Harvest files.
        for (int i=0; i<numTables; i+=2) {
            QString effortOrCatch = "FitWeights";
            QString fileTypes = "fitweights*.csv FitWeights*.csv";
            if (QString::fromStdString(m_HarvestTables[i]).contains("effort")) {
                effortOrCatch = "Harvest Effort";
                fileTypes = "harvesteffort*.csv HarvestEffort*.csv";
            } else if (QString::fromStdString(m_HarvestTables[i]).contains("catch")) {
                effortOrCatch = "Harvest Catch";
                fileTypes = "harvestcatch*.csv HarvestCatch*.csv";
            }
            QString msg = "Select " + effortOrCatch + " file";
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
                            loadCSVFile(absolutePath,tableName,m_TableViews[i],nmfConstantsMSSPM::VerboseOff);
                            if (i < numTables-1) {
                                loadCSVFile(absolutePath,tableName,m_TableViews[i+1],nmfConstantsMSSPM::VerboseOff);
                            }
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
                    loadCSVFile(absolutePath,fileName,m_TableViews[i],nmfConstantsMSSPM::VerboseOff);
                    if (i < numTables-1) {
                        loadCSVFile(absolutePath,fileName,m_TableViews[i+1],nmfConstantsMSSPM::VerboseOff);
                    }
                }
            } else {
                return;
            }
        }
    }

    // Now remove the FitWeights tables from the table lists
    if (isFitToCatch()) {
        m_HarvestTables.pop_back();
        m_TableViews.pop_back();
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
    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Check harvest table for blanks
    for (QTableView* harvestTable : {Estimation_Tab2_WeightsFTCTV,
                                     Estimation_Tab2_EffortTV,
                                     Estimation_Tab2_EffortFTCTV,
                                     Estimation_Tab2_CatchTV,
                                     Estimation_Tab2_CatchFTCTV})
    {
        if (harvestTable->isVisible()) {
            if (! nmfUtilsQt::areAllCellsNonBlank(harvestTable)) {
                QString msg = "No blanks allowed in harvest table: " + harvestTable->objectName();
                m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
                QMessageBox::warning(Estimation_Tabs, "Save Error", "\n"+msg, QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    if (isCatch()) {
        saveTable(Estimation_Tab2_CatchTV,
                  nmfConstantsMSSPM::TableHarvestCatch);
    } else if (isEffort()) {
        saveTable(Estimation_Tab2_EffortTV,
                  nmfConstantsMSSPM::TableHarvestEffort);
    } else { // FitToCatch
        int currentIndex = Estimation_Tab2_FitToCatchTB->currentIndex();
        switch (currentIndex) {
          case 0: // Weights
            if (! saveFitWeights(nmfConstantsMSSPM::VerboseOn)) {
                return;
            }
            break;
          case 1: // Effort
            saveTable(Estimation_Tab2_EffortFTCTV,
                      nmfConstantsMSSPM::TableHarvestEffort);
            break;
          case 2: // Catch
            saveTable(Estimation_Tab2_CatchFTCTV,
                      nmfConstantsMSSPM::TableHarvestCatch);
            break;
          default:
            break;
        };
    }

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}

bool
nmfEstimation_Tab2::saveTable(QTableView* tableView,
                              const std::string& tableName)
{
    int NumSpecies;
    std::string cmd;
    std::string errorMsg;
    std::string harvestTable;
    QString valueWithoutComma;
    QStringList SpeciesList;
    QStandardItemModel* smodel;
    QModelIndex index;
    std::vector<std::string> modelsInProject = {};

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    if (! m_DatabasePtr->updateAllModelsInProject(
                Estimation_Tabs,"Harvest",m_ProjectName,m_ModelName,modelsInProject)) {
        return false;
    }

    for (std::string projectModel : modelsInProject)
    {
        smodel = qobject_cast<QStandardItemModel*>(tableView->model());
        cmd = "DELETE FROM " + tableName +
              " WHERE ProjectName = '" + m_ProjectName +
              "' AND ModelName = '"    + projectModel  + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::saveTable: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command. Couldn't delete all records from" +
                                 QString::fromStdString(tableName) + " table.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return false;
        }
        cmd = "INSERT INTO " + tableName + " (ProjectName,ModelName,SpeName,Year,Value) VALUES ";
        for (int j=0; j<smodel->columnCount(); ++j) {  // Species
            for (int i=0; i<smodel->rowCount(); ++i) { // Time
                index = smodel->index(i,j);
                valueWithoutComma = index.data().toString().remove(",");
                cmd += "('"  + m_ProjectName + "','" + projectModel +
                        "','" + SpeciesList[j].toStdString() + "'," + std::to_string(i) +
                        ", " + valueWithoutComma.toStdString() + "),";
            }
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::saveTable: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command. Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return false;
        }

        m_DatabasePtr->updateUnitsTable(
                    Estimation_Tabs,m_Logger,m_ProjectName,projectModel,
                    tableName,getCurrentUnits().toStdString());
    }

    tableView->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, QString::fromStdString(tableName) + " Updated",
                             "\n" + QString::fromStdString(tableName) +
                             " table has been successfully updated.\n",
                             QMessageBox::Ok);

    return true;
}

bool
nmfEstimation_Tab2::saveFitWeights(const bool& verbose)
{
    int NumSpecies;
    std::string tableName = nmfConstantsMSSPM::TableFitWeights;
    std::string cmd;
    std::string errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    QModelIndex indexBiomass,indexCatch;
    QStringList SpeciesList;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_WeightsFTCTV->model());

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    if (smodel == nullptr) {
        smodel = new QStandardItemModel(NumSpecies,2);
    }

    cmd = "DELETE FROM " + tableName +
          " WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"    + m_ModelName   + "'";

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::saveFitWeights: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        if (verbose) {
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command. Couldn't delete all records from" +
                                 QString::fromStdString(tableName) + " table.\n",
                                 QMessageBox::Ok);
        }
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return false;
    }

    cmd = "INSERT INTO " + tableName + " (ProjectName,ModelName,SpeName,Biomass,Catch) VALUES ";
    for (int i=0; i<smodel->rowCount(); ++i) {  // Species
            indexBiomass = smodel->index(i,0);
            indexCatch   = smodel->index(i,1);
            cmd += "('"  + m_ProjectName + "','" + m_ModelName +
                    "','" + SpeciesList[i].toStdString() +
                    "', " + indexBiomass.data().toString().toStdString() +
                    ", "  + indexCatch.data().toString().toStdString() + "),";
    }

    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab2::saveFitWeights: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        if (verbose) {
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command. Check that all cells are populated.\n",
                                 QMessageBox::Ok);
        }
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return false;
    }

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, QString::fromStdString(nmfConstantsMSSPM::TableFitWeights) + " Updated",
                                 "\n" + QString::fromStdString(nmfConstantsMSSPM::TableFitWeights) +
                                 " table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    return true;
}

void
nmfEstimation_Tab2::setAllBiomassWeights(const double& value)
{
    QModelIndexList selection = Estimation_Tab2_WeightsFTCTV->selectionModel()->selectedIndexes();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_WeightsFTCTV->model());

    if (selection.size() > 0) {
        for (int i=0; i<selection.size(); ++i) {
            QModelIndex index = selection[i];
            int currRow = index.row();
            QStandardItem* item = new QStandardItem(QString::number(value));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(currRow,0,item);
        }
    } else {
        for (int row=0; row<smodel->rowCount();++row) {
            QStandardItem* item = new QStandardItem(QString::number(value));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,0,item);
        }
    }
    Estimation_Tab2_WeightsFTCTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab2::callback_SetBiomassWeightsTo1PB()
{
    setAllBiomassWeights(1.0);
}

void
nmfEstimation_Tab2::callback_SetBiomassWeightsTo0PB()
{
    setAllBiomassWeights(0.0);
}

void
nmfEstimation_Tab2::callback_UnitsEffortCMB(QString currentEffortUnits)
{
    if (isConvertEffortChecked()) {
        nmfUtilsQt::convertTableView(Estimation_Tab2_EffortTV,
                                     m_NumSignificantDigits,
                                     m_PreviousEffortUnits,currentEffortUnits);
        nmfUtilsQt::convertTableView(Estimation_Tab2_EffortFTCTV,
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
                                     m_PreviousCatchUnits,
                                     currentCatchUnits);
        nmfUtilsQt::convertTableView(Estimation_Tab2_CatchFTCTV,
                                     m_NumSignificantDigits,
                                     m_PreviousCatchUnits,
                                     currentCatchUnits);
    }
    m_PreviousCatchUnits = currentCatchUnits;
}

void
nmfEstimation_Tab2::callback_SetWeightsEqualPB()
{
    setAllBiomassWeights(0.5);
}


void
nmfEstimation_Tab2::callback_SetWeightsPB(const QModelIndex& indexTopLeft,
                                          const QModelIndex& indexBottomRight)
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_WeightsFTCTV->model());

    // Find the selected cell and set its complement cell to be
    // 1.0 minus its value. Clamp in case used enters in a number out of 0-1 range.
    double value = indexTopLeft.data().toDouble();
    nmfUtils::clamp(value,0.0,1.0);
    double compValue = 1.0 - value;
    int currRow = indexTopLeft.row();
    int currCol = indexTopLeft.column();
    QStandardItem* itemOrig = new QStandardItem(QString::number(value));
    QStandardItem* item     = new QStandardItem(QString::number(compValue));
    itemOrig->setTextAlignment(Qt::AlignCenter);
    item->setTextAlignment(Qt::AlignCenter);
    int compCol = (currCol == 0) ? 1 : 0;

    smodel->blockSignals(true);
    smodel->setItem(currRow,compCol,item);
    // Reset original in case it was clamped
    smodel->setItem(currRow,currCol,itemOrig);
    Estimation_Tab2_WeightsFTCTV->resizeColumnsToContents();
    smodel->blockSignals(false);
}

void
nmfEstimation_Tab2::callback_SetAllWeightsPB()
{
    // Get current selection
    QModelIndexList selection = Estimation_Tab2_WeightsFTCTV->selectionModel()->selectedIndexes();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab2_WeightsFTCTV->model());

    // Find the selected cell and set its complement cell to be
    // 1.0 minus its value.
    if (selection.size() > 0) {
        QModelIndex index = selection[0];
        double value = index.data().toDouble();
        nmfUtils::clamp(value,0.0,1.0);
        double compValue = 1.0 - value;
        int currRow = index.row();
        int currCol = index.column();
        int compCol = (currCol == 0) ? 1 : 0;
        for (int row=0; row<smodel->rowCount();++row) {
            QStandardItem* item     = new QStandardItem(QString::number(value));
            QStandardItem* compItem = new QStandardItem(QString::number(compValue));
            item->setTextAlignment(Qt::AlignCenter);
            compItem->setTextAlignment(Qt::AlignCenter);
            if (row != currRow) {
                smodel->setItem(row,currCol,item);
            }
            smodel->setItem(row,compCol,compItem);
        }
        Estimation_Tab2_WeightsFTCTV->resizeColumnsToContents();
    } else {
        QMessageBox::information(Estimation_Tabs, "No Selection Found",
                                 "\nPlease select a cell to be copied to the rest of the column.\n",
                                 QMessageBox::Ok);
    }
}

