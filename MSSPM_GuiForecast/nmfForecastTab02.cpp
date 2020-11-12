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
    m_HarvestType = "ForecastCatch";
    m_SModel      = nullptr;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab2::nmfForecast_Tab2");

    Forecast_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Forecast/Forecast_Tab02.ui");
    file.open(QFile::ReadOnly);
    Forecast_Tab2_Widget = loader.load(&file,Forecast_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Forecast_Tabs->addTab(Forecast_Tab2_Widget, tr("2. Harvest Parameters"));

    Forecast_Tab2_HarvestTV     = Forecast_Tabs->findChild<QTableView     *>("Forecast_Tab2_HarvestTV");
    Forecast_Tab2_HarvestGB     = Forecast_Tabs->findChild<QGroupBox      *>("Forecast_Tab2_HarvestGB");
    Forecast_Tab2_PrevPB        = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_PrevPB");
    Forecast_Tab2_NextPB        = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_NextPB");
    Forecast_Tab2_LoadPB        = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_LoadPB");
    Forecast_Tab2_SavePB        = Forecast_Tabs->findChild<QPushButton    *>("Forecast_Tab2_SavePB");
    Forecast_Tab1_NameLE        = Forecast_Tabs->findChild<QLineEdit      *>("Forecast_Tab1_NameLE");
    Forecast_Tab2_MultiplierCB  = Forecast_Tabs->findChild<QCheckBox      *>("Forecast_Tab2_MultiplierCB");
    Forecast_Tab2_MultiplierCMB = Forecast_Tabs->findChild<QComboBox      *>("Forecast_Tab2_MultiplierCMB");
    Forecast_Tab2_MultiplierDSB = Forecast_Tabs->findChild<QDoubleSpinBox *>("Forecast_Tab2_MultiplierDSB");
    Forecast_Tab2_AutoSaveCB    = Forecast_Tabs->findChild<QCheckBox      *>("Forecast_Tab2_AutoSaveCB");

    connect(Forecast_Tab2_PrevPB,        SIGNAL(clicked(bool)),
            this,                        SLOT(callback_PrevPB()));
    connect(Forecast_Tab2_NextPB,        SIGNAL(clicked(bool)),
            this,                        SLOT(callback_NextPB()));
    connect(Forecast_Tab2_LoadPB,        SIGNAL(clicked(bool)),
            this,                        SLOT(callback_LoadPB()));
    connect(Forecast_Tab2_SavePB,        SIGNAL(clicked(bool)),
            this,                        SLOT(callback_SavePB()));
    connect(Forecast_Tab2_MultiplierCB,  SIGNAL(clicked(bool)),
            this,                        SLOT(callback_MultiplierCB(bool)));
    connect(Forecast_Tab2_MultiplierDSB, SIGNAL(valueChanged(double)),
            this,                        SLOT(callback_MultiplierChangedDSB(double)));
    connect(Forecast_Tab2_MultiplierCMB, SIGNAL(activated(QString)),
            this,                        SLOT(callback_MultiplierChangedCMB(QString)));
    connect(Forecast_Tab2_AutoSaveCB,    SIGNAL(clicked(bool)),
            this,                        SLOT(callback_AutoSaveCB(bool)));

    Forecast_Tab2_PrevPB->setText("\u25C1--");
    Forecast_Tab2_NextPB->setText("--\u25B7");

    Forecast_Tab2_MultiplierDSB->setDecimals(2);
    Forecast_Tab2_MultiplierDSB->setSingleStep(0.01);

} // end constructor


nmfForecast_Tab2::~nmfForecast_Tab2()
{

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
nmfForecast_Tab2::callback_AutoSaveCB(bool checked)
{
    if (checked) {
        callback_MultiplierChangedDSB(Forecast_Tab2_MultiplierDSB->value());
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
    // Get selected cells.
    QModelIndexList indexList = Forecast_Tab2_HarvestTV->selectionModel()->selectedIndexes();
    if (indexList.size() == 0) {
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
                colStartValues.push_back(indexList[index].data().toDouble());
            } else {
                newValue = colStartValues[index%cellsPerRow] * multiplier;
                m_SModel->setData(indexList[index],newValue);
            }
        }
    } else if (Forecast_Tab2_MultiplierCMB->currentText() == "Variable") {
        for (int index=0; index<listSize; ++index) {
            if (index < cellsPerRow) { // means at first row in col
                colStartValues.push_back(indexList[index].data().toDouble());
            } else {
                newValue = colStartValues[index%cellsPerRow] * multiplier;
                m_SModel->setData(indexList[index],newValue);
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

    Forecast_Tab2_MultiplierCMB->setEnabled(checked);
    Forecast_Tab2_MultiplierDSB->setEnabled(checked);

    if (checked) {
        value = Forecast_Tab2_MultiplierDSB->value();
        callback_MultiplierChangedDSB(value);
    } else {
        QModelIndexList indexList = Forecast_Tab2_HarvestTV->selectionModel()->selectedIndexes();
        loadWidgets();

        for (QModelIndex index : indexList) {
            Forecast_Tab2_HarvestTV->selectionModel()->select(index,QItemSelectionModel::Select);
        }
    }
}

void
nmfForecast_Tab2::callback_PrevPB()
{
    int prevPage = Forecast_Tabs->currentIndex()-1;
    Forecast_Tabs->setCurrentIndex(prevPage);
}

void
nmfForecast_Tab2::callback_NextPB()
{
    int nextPage = Forecast_Tabs->currentIndex()+1;
    Forecast_Tabs->setCurrentIndex(nextPage);
}

void
nmfForecast_Tab2::callback_LoadPB()
{
    loadWidgets();
}

void
nmfForecast_Tab2::callback_SavePB()
{
    saveHarvestData(true);
}

void
nmfForecast_Tab2::saveHarvestData(bool verbose)
{
    bool systemFound = false;
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

    if (m_SModel == nullptr) {
        return;
    }

    systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                Forecast_Tabs,m_Logger,m_ProjectSettingsConfig,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab1::saveHarvestData: No systems found");
        return;
    }

    Forecast_Tabs->setCursor(Qt::WaitCursor);

    for (int j=0; j<m_SModel->columnCount(); ++ j) {
        SpeNames.push_back(m_SModel->horizontalHeaderItem(j)->text().toStdString());
    }

    cmd = "DELETE FROM " + m_HarvestType + " WHERE ForecastName = '" +
           ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab2::saveHarvestData: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from" +
                             QString::fromStdString(m_HarvestType) + " table.\n",
                             QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    cmd = "INSERT INTO " + m_HarvestType + " (ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year,Value) VALUES ";
    for (int j=0; j<m_SModel->columnCount(); ++j) { // Species
        for (int i=0; i<m_SModel->rowCount(); ++i) { // Time
            index = m_SModel->index(i,j);
            value = index.data().toString().toStdString();
            cmd += "('" + ForecastName +
                    "','" + Algorithm + "','" + Minimizer +
                    "','" + ObjectiveCriterion + "','" + Scaling +
                    "','" + SpeNames[j] + "'," + std::to_string(i) +
                    ", " + value + "),";
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
        QMessageBox::information(Forecast_Tabs, QString::fromStdString(m_HarvestType) + " Updated",
                                 "\n" + QString::fromStdString(m_HarvestType) +
                                 " table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    if (Forecast_Tab2_AutoSaveCB->isChecked())
    {
        emit RunForecast(ForecastName,true);
    }

    Forecast_Tabs->setCursor(Qt::ArrowCursor);
}


//void
//nmfForecast_Tab2::callback_HarvestFormChanged(QString harvestForm)
//{
//    setHarvestType(harvestForm.toStdString());
//    loadWidgets();
//}


void
nmfForecast_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    delete settings;
}


bool
nmfForecast_Tab2::restoreData(int minRow, int minCol, int maxRow, int maxCol)
{
    int m;
    int NumSpecies;
    int RunLength=0;
//  int StartYear=nmfConstantsMSSPM::Start_Year;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();

    // Find species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    // Find Forecast info
    fields     = {"ForecastName","RunLength","StartYear","EndYear"};
    queryStr   = "SELECT ForecastName,RunLength,StartYear,EndYear FROM Forecasts where ";
    queryStr  += "ForecastName = '" + ForecastName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        RunLength  = std::stoi(dataMap["RunLength"][0]);
//      StartYear  = std::stoi(dataMap["StartYear"][0]);
    }

    fields     = {"ForecastName","SpeName","Year","Value"};
    queryStr   = "SELECT ForecastName,SpeName,Year,Value FROM " + m_HarvestType + " where ";
    queryStr  += "ForecastName = '" + ForecastName + "'";
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
                m_SModel->setItem(i, j, item);
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
nmfForecast_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Forecast_Tab2_HarvestTV});
}

/*
 * harvestType: Catch, Effort, Exploitation
 */
bool
nmfForecast_Tab2::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab2::loadWidgets()");

    int m;
    int NumSpecies;
    int RunLength=0;
    int StartYear=nmfConstantsMSSPM::Start_Year;
    int NumRecords;
    std::string harvestForm;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList VerticalList;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();

    readSettings();
    if (m_ProjectSettingsConfig.empty())
        return false;

    clearWidgets();

    Forecast_Tab2_HarvestGB->setTitle(QString::fromStdString(m_HarvestType).replace("Forecast","Forecast "));

    // Get latest harvest form. RSK Need to change this once user can select a run which wasn't the last run
    fields     = {"GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr   = "SELECT GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm";
    queryStr  += " FROM Systems WHERE SystemName='" + m_ProjectSettingsConfig + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["GrowthForm"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfForecast_Tab2::nmfForecast_Tab2: No records found in table Systems for Name = "+m_ProjectSettingsConfig);
        return false;
    }
    harvestForm = dataMap["HarvestForm"][0];
    if (harvestForm == "Catch") {
        m_HarvestType = "ForecastCatch";
    } else if (harvestForm == "Exploitation (F)") {
        m_HarvestType = "ForecastExploitation";
    } else if (harvestForm == "Effort (qE)") {
        m_HarvestType = "ForecastEffort";
    }

    // Find species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    // Find Forecast info
    fields     = {"ForecastName","RunLength","StartYear","EndYear"};
    queryStr   = "SELECT ForecastName,RunLength,StartYear,EndYear FROM Forecasts where ";
    queryStr  += "ForecastName = '" + ForecastName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        RunLength  = std::stoi(dataMap["RunLength"][0]);
        StartYear  = std::stoi(dataMap["StartYear"][0]);
    }

    fields     = {"ForecastName","SpeName","Year","Value"};
    queryStr   = "SELECT ForecastName,SpeName,Year,Value FROM " + m_HarvestType + " where ";
    queryStr  += "ForecastName = '" + ForecastName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    bool RunLengthHasChanged = (NumRecords != NumSpecies*(RunLength+1));

    // Create list of years
    VerticalList.clear();
    for (int i=0; i<=RunLength; ++i) {
        VerticalList << QString::number(StartYear+i);
    }

    // Load catch data or blanks if no catch data has yet been entered
    m = 0;
    m_SModel = new QStandardItemModel( RunLength, NumSpecies );
    for (int j=0; j<NumSpecies; ++j) {
        for (int i=0; i<=RunLength; ++i) {
            if ((NumRecords == 0) || RunLengthHasChanged)
                item = new QStandardItem(QString(""));
            else
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
            item->setTextAlignment(Qt::AlignCenter);
            m_SModel->setItem(i, j, item);
        }
    }

    Forecast_Tab2_HarvestTV->disconnect();

    m_SModel->setVerticalHeaderLabels(VerticalList);
    m_SModel->setHorizontalHeaderLabels(SpeciesNames);
    Forecast_Tab2_HarvestTV->setModel(m_SModel);
    Forecast_Tab2_HarvestTV->resizeColumnsToContents();

    connect(Forecast_Tab2_HarvestTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,     SLOT(callback_SelectionChanged(const QItemSelection &, const QItemSelection &)));

    return true;
}

void
nmfForecast_Tab2::setHarvestType(std::string harvestType)
{
    m_HarvestType = harvestType;
}

