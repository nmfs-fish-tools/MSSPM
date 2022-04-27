#include "nmfForecastTab02.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfForecast_Tab2::nmfForecast_Tab2(QTabWidget*  tabs,
                                   nmfLogger*   logger,
                                   nmfDatabase* databasePtr,
                                   std::string& projectDir)
{
    QUiLoader loader;

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatch;
    m_ProjectDir  = projectDir;
    m_ModelName.clear();
    m_ProjectName.clear();
    m_NumSignificantDigits = -1;
    m_PreviousUnits.clear();
    m_EstimationHarvestModel = nullptr;
    m_EstimationHarvestModelCatchFTC  = nullptr;
    m_EstimationHarvestModelEffortFTC = nullptr;

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab2::nmfForecast_Tab2");

    Forecast_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Forecast/Forecast_Tab02.ui");
    file.open(QFile::ReadOnly);
    Forecast_Tab2_Widget = loader.load(&file,Forecast_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Forecast_Tabs->addTab(Forecast_Tab2_Widget, tr("2. Harvest Data"));

    Forecast_Tab2_HarvestTV        = Forecast_Tabs->findChild<QTableView     *>("Forecast_Tab2_HarvestTV");
    Forecast_Tab2_HarvestGB        = Forecast_Tabs->findChild<QGroupBox      *>("Forecast_Tab2_HarvestGB");
    Forecast_Tab2_PrevPB           = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_PrevPB");
    Forecast_Tab2_NextPB           = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_NextPB");
    Forecast_Tab2_LoadPB           = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_LoadPB");
    Forecast_Tab2_SavePB           = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_SavePB");
    Forecast_Tab2_SetDefaultHarvestPB = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_SetDefaultHarvestPB");
    Forecast_Tab2_SetHarvestTypePB = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_SetHarvestTypePB");
    Forecast_Tab1_NameLE           = Forecast_Tabs->findChild<QLineEdit      *>("Forecast_Tab1_NameLE");
    Forecast_Tab2_MultiplierCB     = Forecast_Tabs->findChild<QCheckBox      *>("Forecast_Tab2_MultiplierCB");
    Forecast_Tab2_MultiplierCMB    = Forecast_Tabs->findChild<QComboBox      *>("Forecast_Tab2_MultiplierCMB");
    Forecast_Tab2_MultiplierDSB    = Forecast_Tabs->findChild<QDoubleSpinBox *>("Forecast_Tab2_MultiplierDSB");
    Forecast_Tab2_AutoSaveCB       = Forecast_Tabs->findChild<QCheckBox      *>("Forecast_Tab2_AutoSaveCB");
    Forecast_Tab2_UnitsCMB         = Forecast_Tabs->findChild<QComboBox      *>("Forecast_Tab2_UnitsCMB");
    Forecast_Tab2_ConvertCB        = Forecast_Tabs->findChild<QCheckBox      *>("Forecast_Tab2_ConvertCB");

    connect(Forecast_Tab2_PrevPB,           SIGNAL(clicked(bool)),
            this,                           SLOT(callback_PrevPB()));
    connect(Forecast_Tab2_NextPB,           SIGNAL(clicked(bool)),
            this,                           SLOT(callback_NextPB()));
    connect(Forecast_Tab2_LoadPB,           SIGNAL(clicked(bool)),
            this,                           SLOT(callback_LoadPB()));
    connect(Forecast_Tab2_SavePB,           SIGNAL(clicked(bool)),
            this,                           SLOT(callback_SavePB()));
    connect(Forecast_Tab2_SetDefaultHarvestPB, SIGNAL(clicked(bool)),
            this,                              SLOT(callback_SetDefaultHarvestPB()));
    connect(Forecast_Tab2_SetHarvestTypePB, SIGNAL(clicked(bool)),
            this,                           SLOT(callback_SetHarvestTypePB()));
    connect(Forecast_Tab2_MultiplierCB,     SIGNAL(clicked(bool)),
            this,                           SLOT(callback_MultiplierCB(bool)));
    connect(Forecast_Tab2_MultiplierDSB,    SIGNAL(valueChanged(double)),
            this,                           SLOT(callback_MultiplierChangedDSB(double)));
    connect(Forecast_Tab2_MultiplierCMB,    SIGNAL(activated(QString)),
            this,                           SLOT(callback_MultiplierChangedCMB(QString)));
    connect(Forecast_Tab2_AutoSaveCB,       SIGNAL(clicked(bool)),
            this,                           SLOT(callback_AutoSaveCB(bool)));
    connect(Forecast_Tab2_UnitsCMB,         SIGNAL(currentTextChanged(QString)),
            this,                           SLOT(callback_UnitsCMB(QString)));

    m_PreviousUnits = Forecast_Tab2_UnitsCMB->currentText();

    Forecast_Tab2_PrevPB->setText("\u25C1--");
    Forecast_Tab2_NextPB->setText("--\u25B7");

    Forecast_Tab2_MultiplierDSB->setDecimals(2);
    Forecast_Tab2_MultiplierDSB->setSingleStep(0.01);
    Forecast_Tab2_HarvestTV->setSelectionMode(QAbstractItemView::ContiguousSelection);

} // end constructor


nmfForecast_Tab2::~nmfForecast_Tab2()
{

}

bool
nmfForecast_Tab2::areDataComplete()
{
    return nmfUtilsQt::allCellsArePopulated(
                Forecast_Tabs,
                Forecast_Tab2_HarvestTV,
                nmfConstantsMSSPM::DontShowError);
}

void
nmfForecast_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Forecast_Tab2_HarvestTV});
}

void
nmfForecast_Tab2::enableHarvestTypeButton(
        const bool& enable,
        const QString& buttonLabelToSet)
{
    Forecast_Tab2_SetHarvestTypePB->setEnabled(enable);

    if (! getHarvestType().contains(buttonLabelToSet)) {
        callback_SetDefaultHarvestPB();
    }
}


QString
nmfForecast_Tab2::getCurrentUnits()
{
    return Forecast_Tab2_UnitsCMB->currentText();
}


QString
nmfForecast_Tab2::getHarvestType()
{
    return Forecast_Tab2_SetHarvestTypePB->text().remove(":");
}

bool
nmfForecast_Tab2::isConvertChecked()
{
    return Forecast_Tab2_ConvertCB->isChecked();
}

void
nmfForecast_Tab2::loadUnits(const std::string& tableName)
{
    int NumRecords;
    std::string units;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"ProjectName","ModelName","TableName","Units"};
    queryStr   = "SELECT ProjectName,ModelName,TableName,Units FROM " +
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
        QMessageBox::warning(Forecast_Tabs,"Error","\n"+msg+"\n", QMessageBox::Ok);
    }
}

bool
nmfForecast_Tab2::loadWidgets()
{
    if (! loadWidgets(nmfConstantsMSSPM::ReadHarvestTypeFromTable)) {
        return false;
    }
    return true;
}

bool
nmfForecast_Tab2::loadWidgets(const bool& readHarvestTypeFromTable)
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab2::loadWidgets()");

    readSettings();

    if (readHarvestTypeFromTable) {
        if (! setHarvestTypeFromModel()) {
            return false;
        }
    }

    if (! loadTable()) {
        return false;
    }

    return true;
}

bool
nmfForecast_Tab2::loadTable()
{
    int NumRecords;
    int NumSpecies;
    int RunLength=0;
    int NumRuns;
    int StartYear=nmfConstantsMSSPM::Start_Year;
    QStringList SpeciesNames;
    QStringList VerticalList;
    QStandardItem *item;
    QString valueWithComma;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string ForecastHarvestType;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    std::string Algorithm,Minimizer,ObjCrit,Scaling;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;

    clearWidgets();

    // Find species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    // Find Forecast info
    m_DatabasePtr->getForecastInfo(m_ProjectName,m_ModelName,ForecastName,RunLength,StartYear,
                                   Algorithm,Minimizer,ObjCrit,Scaling,
                                   GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                   ForecastHarvestType,NumRuns);

    // Get harvest data
    fields     = {"ProjectName","ModelName","ForecastName","SpeName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,SpeName,Year,Value FROM " +
                  m_HarvestTableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName   +
                 "' AND ForecastName = '" + ForecastName  + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    bool RunLengthHasChanged = (NumRecords != NumSpecies*(RunLength+1));

    // Create list of years
    VerticalList.clear();
    for (int i=0; i<=RunLength; ++i) {
        VerticalList << QString::number(StartYear+i);
    }

    // Load harvest data or blanks if no catch data has yet been entered
    int m = 0;
    QStandardItemModel* smodel = new QStandardItemModel( RunLength, NumSpecies );
    for (int j=0; j<NumSpecies; ++j) {
        for (int i=0; i<=RunLength; ++i) {
            if ((NumRecords == 0) || RunLengthHasChanged) {
                item = new QStandardItem(QString(""));
            } else {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            std::stod(dataMap["Value"][m++]),m_NumSignificantDigits,2);
                item = new QStandardItem(valueWithComma);
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
    }

    Forecast_Tab2_HarvestTV->disconnect();

    smodel->setVerticalHeaderLabels(VerticalList);
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    Forecast_Tab2_HarvestTV->setModel(smodel);
    Forecast_Tab2_HarvestTV->resizeColumnsToContents();

    loadUnits(m_HarvestTableName);

    connect(Forecast_Tab2_HarvestTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,     SLOT(callback_SelectionChanged(const QItemSelection &, const QItemSelection &)));

    return true;
}

void
nmfForecast_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}

bool
nmfForecast_Tab2::restoreData(int minRow, int minCol, int maxRow, int maxCol)
{
    int m;
    int NumSpecies;
    int RunLength=0;
    int NumRecords;
    int StartYear;
    int NumRuns;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string ForecastHarvestType;
    std::string Algorithm,Minimizer,ObjCrit,Scaling;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;
    QStandardItem *item;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Forecast_Tab2_HarvestTV->model());

    // Find species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    // Find Forecast info
    m_DatabasePtr->getForecastInfo(m_ProjectName,m_ModelName,ForecastName,RunLength,StartYear,
                                   Algorithm,Minimizer,ObjCrit,Scaling,
                                   GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                   ForecastHarvestType,NumRuns);


    fields     = {"ProjectName","ModelName","ForecastName","SpeName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,SpeName,Year,Value FROM " +
                  m_HarvestTableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' AND ForecastName = '" + ForecastName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();

    // Load catch data or blanks if no catch data has yet been entered
    m = 0;
    for (int j=0; j<NumSpecies; ++j) {
        for (int i=0; i<=RunLength; ++i) {
            if ((j >= minCol) && (j <= maxCol) && (i >= minRow) && (i <= maxRow) && (m < NumRecords))
            {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
            ++m;
        }
    }

    disconnect(Forecast_Tab2_HarvestTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,     SLOT(callback_SelectionChanged(const QItemSelection &, const QItemSelection &)));

    Forecast_Tab2_HarvestTV->resizeColumnsToContents();

    connect(Forecast_Tab2_HarvestTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,     SLOT(callback_SelectionChanged(const QItemSelection &, const QItemSelection &)));

    return true;
}

void
nmfForecast_Tab2::saveHarvestData(bool verbose)
{
    bool modelFound = false;
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::string value;
    std::vector<std::string> SpeNames;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QString valueWithoutComma;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Forecast_Tab2_HarvestTV->model());

    if (smodel == nullptr) {
        return;
    }

    if (! nmfUtilsQt::areAllCellsNonBlank(Forecast_Tab2_HarvestTV)) {
        QString msg = "No blanks allowed in the Forecast Harvest table";
        m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
        QMessageBox::critical(Forecast_Tabs, "Save Error", "\n"+msg, QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    modelFound = m_DatabasePtr->getAlgorithmIdentifiers(
                Forecast_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! modelFound) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab1::saveHarvestData: No models found");
        return;
    }

    Forecast_Tabs->setCursor(Qt::WaitCursor);

    for (int j=0; j<smodel->columnCount(); ++ j) {
        SpeNames.push_back(smodel->horizontalHeaderItem(j)->text().toStdString());
    }

    cmd = "DELETE FROM " + m_HarvestTableName +
          " WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"    + m_ModelName + "'";
          "' AND ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab2::saveHarvestData: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from " +
                             QString::fromStdString(m_HarvestTableName) + " table.\n",
                             QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    // Update Forecast Harvest Type in Forecasts table
    cmd  = "UPDATE " + nmfConstantsMSSPM::TableForecasts +
           " SET ForecastHarvestType = '" + getHarvestType().toStdString() +
           "' WHERE ProjectName = '" + m_ProjectName +
           "' AND ModelName = '" + m_ModelName +
           "' AND ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab2::saveHarvestData: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error","\nError in Save command.\n",QMessageBox::Ok);
        return;
    }

    cmd = "INSERT INTO " + m_HarvestTableName +
          " (ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year,Value) VALUES ";
    for (int j=0; j<smodel->columnCount(); ++j) { // Species
        for (int i=0; i<smodel->rowCount(); ++i) { // Time
            index = smodel->index(i,j);
            valueWithoutComma = index.data().toString().remove(",");
            value = valueWithoutComma.toStdString();
            cmd += "('"   + m_ProjectName +
                    "','" + m_ModelName +
                    "','" + ForecastName +
                    "','" + Algorithm + "','" + Minimizer +
                    "','" + ObjectiveCriterion + "','" + Scaling +
                    "','" + SpeNames[j] + "'," + std::to_string(i) +
                    ", "  + value + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab2::saveHarvestData: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        if (verbose) {
            QMessageBox::warning(Forecast_Tabs, "Error",
                                 "\nError in Save command.  Check that all cells are populated.\n",
                                 QMessageBox::Ok);
        }
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Forecast_Tab2_HarvestTV->resizeColumnsToContents();

    if (verbose && ! Forecast_Tab2_AutoSaveCB->isChecked()) {
        QMessageBox::information(Forecast_Tabs, QString::fromStdString(m_HarvestTableName) + " Updated",
                                 "\n" + QString::fromStdString(m_HarvestTableName) +
                                 " table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    if (Forecast_Tab2_AutoSaveCB->isChecked())
    {
        emit RunForecast(ForecastName,true);
    }

    m_DatabasePtr->updateUnitsTable(
                Forecast_Tabs,m_Logger,m_ProjectName,m_ModelName,
                m_HarvestTableName,getCurrentUnits().toStdString());

    emit UpdateUncertaintyTable(); // because changing harvest type from Catch to Effort must expose the Catchability (q) uncertainty parameter

    Forecast_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfForecast_Tab2::setCurrentUnits(QString currentUnits)
{
    Forecast_Tab2_UnitsCMB->setCurrentText(currentUnits);
}

//void
//nmfForecast_Tab2::setHarvestTableName(std::string harvestType)
//{
//    m_HarvestTableName = harvestType;
//}

void
nmfForecast_Tab2::setForecastHarvestLabel(const QString& harvestType)
{
    Forecast_Tab2_SetHarvestTypePB->setText(harvestType);
}

void
nmfForecast_Tab2::setForecastEstimationHarvestModels(
        QStandardItemModel* Harvest_smodel,
        QStandardItemModel* CatchFTC_smodel,
        QStandardItemModel* EffortFTC_smodel)
{
    m_EstimationHarvestModel          = Harvest_smodel;
    m_EstimationHarvestModelCatchFTC  = CatchFTC_smodel;
    m_EstimationHarvestModelEffortFTC = EffortFTC_smodel;
}

bool
nmfForecast_Tab2::setHarvestTypeFromModel()
{
    int NumRecords;
    std::string harvestForm;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QLocale locale(QLocale::English);

    if (m_ModelName.empty()) {
        return false;
    }

    // Get latest harvest form. RSK Need to change this once user can select a run which wasn't the last run
    fields     = {"GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr   = "SELECT GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm";
    queryStr  += " FROM " + nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName  +
                 "' AND ModelName='" + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["GrowthForm"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfForecast_Tab2::nmfForecast_Tab2: No records found in table Models for Name = "+m_ModelName);
        return false;
    }
    harvestForm = dataMap["HarvestForm"][0];
    if (harvestForm == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatch;
        setForecastHarvestLabel(nmfConstantsMSSPM::ForecastHarvestTypeCatch);
    } else if (harvestForm == "Exploitation (F)") {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestExploitation;
        setForecastHarvestLabel(QString("tbd:"));
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffort;
        setForecastHarvestLabel(nmfConstantsMSSPM::ForecastHarvestTypeEffort);
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffortFTC;
        setForecastHarvestLabel(nmfConstantsMSSPM::ForecastHarvestTypeEffort);
    }

    return true;
}


void
nmfForecast_Tab2::callback_AutoSaveCB(bool checked)
{
    if (checked) {
        callback_MultiplierChangedDSB(Forecast_Tab2_MultiplierDSB->value());
    }
}

void
nmfForecast_Tab2::callback_SetDefaultHarvestPB()
{
    QStandardItemModel* estimation_smodel = nullptr;

    emit GetEstimationHarvestModel();

    // Means user selected effort fit to catch and must allow user to select either Effort or Catch
    if (m_EstimationHarvestModel == nullptr) {
        QString userSelectedHarvestType = getHarvestType();
        if (userSelectedHarvestType == nmfConstantsMSSPM::ForecastHarvestTypeCatch) {
            estimation_smodel = m_EstimationHarvestModelCatchFTC;
        } else if (userSelectedHarvestType == nmfConstantsMSSPM::ForecastHarvestTypeEffort) {
            estimation_smodel = m_EstimationHarvestModelEffortFTC;
        }
    } else {
        estimation_smodel = m_EstimationHarvestModel;
    }

    // Now with the correct model, populate the forecast harvest table
    if (estimation_smodel != nullptr) {
        QStandardItem *item;
        int RunLength  = estimation_smodel->rowCount();
        int NumSpecies = estimation_smodel->columnCount();
        int lastRow = RunLength-1;
        int NumYearsInForecast;
        int StartYear,NumRuns;
        std::string Algorithm,Minimizer,ObjCrit,Scaling;
        std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;
        std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
        std::string ForecastHarvestType;
        QStringList hLabels = {};
        QStringList vLabels = {};

        // Find Forecast info
        m_DatabasePtr->getForecastInfo(m_ProjectName,m_ModelName,
                                       ForecastName,NumYearsInForecast,StartYear,
                                       Algorithm,Minimizer,ObjCrit,Scaling,
                                       GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                       ForecastHarvestType,NumRuns);
        ++NumYearsInForecast; // Add 1 to include the first year
        QStandardItemModel* forecast_smodel = new QStandardItemModel(NumYearsInForecast,NumSpecies);
        for (int row=0; row<NumYearsInForecast; ++row) {
            for (int col=0; col<NumSpecies; ++col) {
                QModelIndex index = estimation_smodel->index(lastRow,col);
                item = new QStandardItem(index.data().toString());
                item->setTextAlignment(Qt::AlignCenter);
                forecast_smodel->setItem(row,col,item);
            }
            vLabels << QString::number(StartYear+row);
        }
        // Set header labels
        for (int col=0; col<NumSpecies; ++col) {
            hLabels << estimation_smodel->headerData(col,Qt::Orientation::Horizontal).toString();
        }
        forecast_smodel->setHorizontalHeaderLabels(hLabels);
        forecast_smodel->setVerticalHeaderLabels(vLabels);

        // Set model
        Forecast_Tab2_HarvestTV->setModel(forecast_smodel);
        Forecast_Tab2_HarvestTV->resizeColumnsToContents();
    }
}

void
nmfForecast_Tab2::callback_LoadPB()
{
    if (getHarvestType() == nmfConstantsMSSPM::ForecastHarvestTypeCatch) {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatch;
        if (Forecast_Tab2_SetHarvestTypePB->isEnabled()) {
            m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatchFTC;
        }
    } else {
        m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffort;
        if (Forecast_Tab2_SetHarvestTypePB->isEnabled()) {
            m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffortFTC;
        }
    }
    if (loadWidgets(nmfConstantsMSSPM::DontReadHarvestTypeFromTable)) {
        QString msg = "\nSuccessfully loaded table: " + QString::fromStdString(m_HarvestTableName) + "\n";
        QMessageBox::information(Forecast_Tabs,"Load",msg,QMessageBox::Ok);
    }
}

void
nmfForecast_Tab2::callback_MultiplierChangedCMB(QString type)
{
    callback_MultiplierChangedDSB(Forecast_Tab2_MultiplierDSB->value());
}

void
nmfForecast_Tab2::callback_MultiplierChangedDSB(double multiplier)
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Forecast_Tab2_HarvestTV->model());

    // Get selected cells
    QModelIndexList indexList = Forecast_Tab2_HarvestTV->selectionModel()->selectedIndexes();
    if (indexList.size() == 0) {
        return;
    }
    // Prevents a crash if user is holding down the Ctrl key
    if (QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        return;
    }

    int listSize    = indexList.size();
    int lastIndex   = listSize-1;
    int colMin      = indexList[0].column();
    int colMax      = indexList[lastIndex].column();
    int cellsPerRow = colMax-colMin+1;
    double newValue;
    std::vector<double> colStartValues;

    if (Forecast_Tab2_MultiplierCMB->currentText() == "Constant")
    {
        for (int index=0; index<listSize; ++index) {
            if (index < cellsPerRow) { // means at first row in col
                colStartValues.push_back(indexList[index].data().toString().replace(",","").toDouble());
            } else {
                newValue = colStartValues[index%cellsPerRow] * multiplier;
                smodel->setData(indexList[index],QString::number(newValue,'f',6));
            }
        }
    } else if (Forecast_Tab2_MultiplierCMB->currentText() == "Variable") {
        for (int index=0; index<listSize; ++index) {
            if (index < cellsPerRow) { // means at first row in col
                colStartValues.push_back(indexList[index].data().toString().replace(",","").toDouble());
            } else {
                newValue = colStartValues[index%cellsPerRow] * multiplier;
                smodel->setData(indexList[index],QString::number(newValue,'f',6));
                colStartValues[index%cellsPerRow] = newValue;
            }
        }
    }
    Forecast_Tab2_HarvestTV->resizeColumnsToContents();
}

void
nmfForecast_Tab2::callback_MultiplierCB(bool checked)
{
    double value;
    QModelIndex tmpIndex;

    Forecast_Tab2_MultiplierCMB->setEnabled(checked);
    Forecast_Tab2_MultiplierDSB->setEnabled(checked);

    if (checked) {
        value = Forecast_Tab2_MultiplierDSB->value();
        callback_MultiplierChangedDSB(value);
    } else {
        QModelIndexList indexList = Forecast_Tab2_HarvestTV->selectionModel()->selectedIndexes();
        loadWidgets(); // loses the previous selection
        for (QModelIndex index : indexList) { // need to reselect the previous selection
            tmpIndex = Forecast_Tab2_HarvestTV->model()->index(index.row(),index.column());
            Forecast_Tab2_HarvestTV->selectionModel()->select(tmpIndex,QItemSelectionModel::Select);
        }
    }
}

void
nmfForecast_Tab2::callback_NextPB()
{
    int nextPage = Forecast_Tabs->currentIndex()+1;
    Forecast_Tabs->setCurrentIndex(nextPage);
}

void
nmfForecast_Tab2::callback_PrevPB()
{
    int prevPage = Forecast_Tabs->currentIndex()-1;
    Forecast_Tabs->setCurrentIndex(prevPage);
}

void
nmfForecast_Tab2::callback_SavePB()
{
    saveHarvestData(true);
}

void
nmfForecast_Tab2::callback_SelectionChanged(const QItemSelection &sel,
                                            const QItemSelection &desel)
{
    int selSize   = sel.indexes().size();
    int deselSize = desel.indexes().size();

    if (Forecast_Tab2_MultiplierCB->isChecked()) {
        callback_MultiplierChangedDSB(Forecast_Tab2_MultiplierDSB->value());
    }

    for (QModelIndex index : sel.indexes()) {
        Forecast_Tab2_HarvestTV->selectionModel()->select(index, QItemSelectionModel::Select);
    }

    if ((deselSize == 0) || ((selSize ==1) && (deselSize == 1))) {
            return;
    }

    if (selSize != 1) {
        if (deselSize > 0) {
            restoreData(desel.indexes()[0].row(),
                    desel.indexes()[0].column(),
                    desel.indexes()[deselSize-1].row(),
                    desel.indexes()[deselSize-1].column());
        }
    }
}

void
nmfForecast_Tab2::callback_SetHarvestTypePB()
{
   if (getHarvestType() == nmfConstantsMSSPM::ForecastHarvestTypeCatch) {
       m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffort;
       setForecastHarvestLabel(nmfConstantsMSSPM::ForecastHarvestTypeEffort);
       if (Forecast_Tab2_SetHarvestTypePB->isEnabled()) {
           m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestEffortFTC;
       }
   } else { // if current harvest type is Effort
       m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatch;
       setForecastHarvestLabel(nmfConstantsMSSPM::ForecastHarvestTypeCatch);
       if (Forecast_Tab2_SetHarvestTypePB->isEnabled()) {
           m_HarvestTableName = nmfConstantsMSSPM::TableForecastHarvestCatchFTC;
       }
   }
   loadTable();
}

void
nmfForecast_Tab2::callback_UnitsCMB(QString currentUnits)
{
    if (isConvertChecked()) {
        nmfUtilsQt::convertTableView(Forecast_Tab2_HarvestTV,
                                m_NumSignificantDigits,
                                m_PreviousUnits,currentUnits);
    }
    m_PreviousUnits = currentUnits;
}
