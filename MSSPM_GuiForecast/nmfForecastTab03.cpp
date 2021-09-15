#include "nmfForecastTab03.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfForecast_Tab3::nmfForecast_Tab3(QTabWidget*  tabs,
                                   nmfLogger*   logger,
                                   nmfDatabase* databasePtr,
                                   std::string& projectDir)
{
    QUiLoader loader;

    Forecast_Tabs = tabs;
    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_SModel      = nullptr;
    m_IsAggProd   = false;
    m_ProjectDir  = projectDir;
    m_ModelName.clear();
    m_ProjectName.clear();
    m_NumSignificantDigits = -1;

    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab3::nmfForecast_Tab3");

    // Load ui as a widget from disk
    QFile file(":/forms/Forecast/Forecast_Tab03.ui");
    file.open(QFile::ReadOnly);
    Forecast_Tab3_Widget = loader.load(&file,Forecast_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Forecast_Tabs->addTab(Forecast_Tab3_Widget, tr("3. Uncertainty Parameters"));
    Forecast_Tab3_UncertaintyTV = Forecast_Tabs->findChild<QTableView  *>("Forecast_Tab3_UncertaintyTV");
    Forecast_Tab3_PrevPB        = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab3_PrevPB");
    Forecast_Tab3_NextPB        = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab3_NextPB");
    Forecast_Tab2_HarvestGB     = Forecast_Tabs->findChild<QGroupBox   *>("Forecast_Tab2_HarvestGB");
    Forecast_Tab1_NameLE        = Forecast_Tabs->findChild<QLineEdit   *>("Forecast_Tab1_NameLE");
    Forecast_Tab3_LoadPB        = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab3_LoadPB");
    Forecast_Tab3_SavePB        = Forecast_Tabs->findChild<QPushButton *>("Forecast_Tab3_SavePB");
    Forecast_Tab3_AutoSaveRunCB = Forecast_Tabs->findChild<QCheckBox   *>("Forecast_Tab3_AutoSaveRunCB");

    // Set up connections
    connect(Forecast_Tab3_PrevPB, SIGNAL(clicked(bool)),
            this,                 SLOT(callback_PrevPB()));
    connect(Forecast_Tab3_NextPB, SIGNAL(clicked(bool)),
            this,                 SLOT(callback_NextPB()));
    connect(Forecast_Tab3_LoadPB, SIGNAL(clicked(bool)),
            this,                 SLOT(callback_LoadPB()));
    connect(Forecast_Tab3_SavePB, SIGNAL(clicked(bool)),
            this,                 SLOT(callback_SavePB()));

    Forecast_Tab3_UncertaintyTV->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(Forecast_Tab3_UncertaintyTV, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(callback_ContextMenu(QPoint)));

    // Set up arrow images on previous and next buttons
    Forecast_Tab3_PrevPB->setText("\u25C1--");
    Forecast_Tab3_NextPB->setText("--\u25B7");

    // Set up mapping from form name to parameters in that form
    m_Alpha  = QString("Competition (") + QChar(0x03B1) + QString(")");    // alpha
    m_BetaS  = QString("Competition (") + QChar(0x03B2) + QString("(i))");
    m_BetaSG = QString("Competition (") + QChar(0x03B2) + QString("(SG))");
    m_BetaGG = QString("Competition (") + QChar(0x03B2) + QString("(GG))");
    m_Rho    = QString("Predation (")   + QChar(0x03C1) + QString(")");
    m_FormMap["Linear"]   = {"Growth Rate (r)"};
    m_FormMap["Logistic"] = {"Growth Rate (r)","Carrying Capacity (K)"};
    m_FormMap["Exploitation (F)"] = {"Exploitation"};
    m_FormMap["Effort (qE)"]      = {"Catchability (q)","Effort"};
    m_FormMap["Catch"]    = {"Catch"};
    m_FormMap["NO_K"]     = {m_Alpha.toStdString()};
    m_FormMap["MS-PROD"]  = {m_BetaS.toStdString(),m_BetaSG.toStdString()};
    m_FormMap["AGG-PROD"] = {m_BetaGG.toStdString()};
    m_FormMap["Type I"]   = {m_Rho.toStdString()};
    m_FormMap["Type II"]  = {m_Rho.toStdString(),"Handling (h)"};
    m_FormMap["Type III"] = {m_Rho.toStdString(),"Handling (h)","Exponent (b)"};

    m_ParameterNames.clear();
    m_ParameterNames << "Initial Absolute Biomass (B₀)";
    m_ParameterNames << "Growth Rate (r)";
    m_ParameterNames << "Carrying Capacity (K)";
    m_ParameterNames << m_Rho;
    m_ParameterNames << m_Alpha;
    m_ParameterNames << m_BetaS;
    m_ParameterNames << m_BetaSG;
    m_ParameterNames << m_BetaGG;
    m_ParameterNames << "Handling (h)";
    m_ParameterNames << "Exponent (b)";
    m_ParameterNames << "Catchability (q)";
    m_ParameterNames << "SurveyQ";
    m_ParameterNames << harvestType();

    readSettings();
}


nmfForecast_Tab3::~nmfForecast_Tab3()
{

}

QString
nmfForecast_Tab3::harvestType()
{
    return Forecast_Tab2_HarvestGB->title();
}


void
nmfForecast_Tab3::callback_ContextMenu(QPoint pos)
{
    QMenu contextMenu("", Forecast_Tab3_UncertaintyTV);
    QAction actionClear("Clear Sel", this);
    actionClear.setToolTip("Set selected cells to 0");
    actionClear.setStatusTip("Set selected cells to 0");

    connect(&actionClear, SIGNAL(triggered()),
            this, SLOT(callback_ClearSelection()));

    contextMenu.addAction(&actionClear);
    contextMenu.exec(Forecast_Tab3_UncertaintyTV->viewport()->mapToGlobal(pos));
}

void
nmfForecast_Tab3::callback_ClearSelection()
{
    QStandardItem* item;
    QModelIndexList selectedIndexesList = Forecast_Tab3_UncertaintyTV->selectionModel()->selectedIndexes();

    for (int i=0; i<selectedIndexesList.size(); ++i) {
        item = new QStandardItem(QString("0.0"));
        item->setTextAlignment(Qt::AlignCenter);
        m_SModel->setItem(selectedIndexesList[i].row(),selectedIndexesList[i].column(), item);
    }
}

void
nmfForecast_Tab3::callback_ItemChanged(const QModelIndex& unusedA,
                                       const QModelIndex& unusedB)
{
    if (Forecast_Tab3_AutoSaveRunCB->isChecked()) {
        std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
        emit RunForecast(ForecastName,true);
    }
}

void
nmfForecast_Tab3::callback_PrevPB()
{
    int prevPage = Forecast_Tabs->currentIndex()-1;
    Forecast_Tabs->setCurrentIndex(prevPage);
}

void
nmfForecast_Tab3::callback_NextPB()
{
    int nextPage = Forecast_Tabs->currentIndex()+1;
    Forecast_Tabs->setCurrentIndex(nextPage);
}


void
nmfForecast_Tab3::callback_SavePB()
{
    int NumRecords;
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> SpeNames;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    if (ForecastName.empty()) {
        QMessageBox::information(Forecast_Tabs, "Save Error",
                                 "\nPlease select or enter a new Forecast in the Forecast->Setup tab.",
                                 QMessageBox::Ok);
        return;
    }

    if (m_SModel == NULL) {
        std::cout << "Error: smodel is NULL" << std::endl;
        return;
    }

    for (int j=0; j<m_SModel->rowCount(); ++ j) {
        SpeNames.push_back(m_SModel->verticalHeaderItem(j)->text().toStdString());
    }

    // Get Alg, Min, ObjCrit
    fields     = {"ProjectName","ModelName","ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling FROM " +
                  nmfConstantsMSSPM::TableForecasts +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' AND ForecastName = '" + ForecastName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found." << std::endl;
        std::cout << queryStr << std::endl;
        return;
    }
    Algorithm = dataMap["Algorithm"][0];
    Minimizer = dataMap["Minimizer"][0];
    ObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    Scaling = dataMap["Scaling"][0];

    // Clear previous entry in ForecastUncertainty table
    cmd = "DELETE FROM " + nmfConstantsMSSPM::TableForecastUncertainty +
          " WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"    + m_ModelName +
          "' AND ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from ForecastUncertainty table.\n",
                             QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    // Update ForecastUncertainty table
    cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableForecastUncertainty + " (" +
           "SpeName,ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling," +
           "InitBiomass,GrowthRate,CarryingCapacity,Catchability,CompetitionAlpha,CompetitionBetaSpecies," +
           "CompetitionBetaGuilds,CompetitionBetaGuildsGuilds,PredationRho,PredationHandling,PredationExponent,SurveyQ,Harvest) VALUES ";
    for (int i=0; i<m_SModel->rowCount(); ++i) { // Species
            cmd += "('"  + SpeNames[i] +
                   "','" + m_ProjectName +
                   "','" + m_ModelName +
                   "','" + ForecastName +
                   "','" + Algorithm +
                   "','" + Minimizer +
                   "','" + ObjectiveCriterion +
                   "','" + Scaling + "'";
            for (int j=0; j<m_SModel->columnCount(); ++j) { // Parameters
                index = m_SModel->index(i,j);
                cmd  += "," + index.data().toString().toStdString();
            }
            cmd += "),";
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Forecast_Tab3_UncertaintyTV->resizeColumnsToContents();
    adjustColumnVisibility();

    if (Forecast_Tab3_AutoSaveRunCB->isChecked()) {
        emit RunForecast(ForecastName,true);
    } else {
        QMessageBox::information(Forecast_Tabs, "Save Forecast",
                                 "\nUncertainty Forecast values saved successfully.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfForecast_Tab3::readSettings()
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


void
nmfForecast_Tab3::callback_LoadPB()
{
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    if (ForecastName.empty()) {
        QMessageBox::information(Forecast_Tabs, "Load Error",
                                 "\nPlease select or enter a new Forecast in the Forecast->Setup tab.",
                                 QMessageBox::Ok);
        return;
    }

    loadWidgets();
}

bool
nmfForecast_Tab3::areDataComplete()
{
    return nmfUtilsQt::allCellsArePopulated(
                Forecast_Tabs,
                Forecast_Tab3_UncertaintyTV,
                nmfConstantsMSSPM::DontShowError);
}

bool
nmfForecast_Tab3::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab3::loadWidgets()");

    int m;
    int NumSpeciesOrGuilds;
    int NumGuilds;
    int NumSpecies;
    int NumRecords=0;
    int NumParameters = m_ParameterNames.size();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList GuildNames;
    QStringList SpeciesOrGuildNames;
    std::string ForecastName = Forecast_Tab1_NameLE->text().toStdString();
    std::vector<QString> param;
    bool uncertaintyDataAvailable = false;
    QString valueWithComma;

    readSettings();

    // Get Forms to be used later
    fields    = {"ProjectName","ModelName","ForecastName","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr  = "SELECT ProjectName,ModelName,ForecastName,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm FROM " +
                nmfConstantsMSSPM::TableForecasts +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName +
                "' AND ForecastName = '" + ForecastName + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        m_GrowthForm      = dataMap["GrowthForm"][0];
        m_HarvestForm     = dataMap["HarvestForm"][0];
        m_CompetitionForm = dataMap["WithinGuildCompetitionForm"][0];
        m_PredationForm   = dataMap["PredationForm"][0];
    }
    m_IsAggProd = (m_CompetitionForm == "AGG-PROD");

    // Get Guild info
    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from " + nmfConstantsMSSPM::TableGuilds + " ORDER by GuildName";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    for (int i=0; i<NumGuilds; ++i) {
        GuildNames << QString::fromStdString(dataMap["GuildName"][i]);
    }

    // Get species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    if (m_IsAggProd) {
        NumSpeciesOrGuilds  = NumGuilds;
        SpeciesOrGuildNames = GuildNames;
    } else {
        NumSpeciesOrGuilds  = NumSpecies;
        SpeciesOrGuildNames = SpeciesNames;
    }

    m_SModel = new QStandardItemModel( NumSpeciesOrGuilds, NumParameters );

    bool ForecastNameExists = ! ForecastName.empty();
    if (ForecastNameExists) {
        // Find Forecast info
        fields     = {"ProjectName","ModelName","SpeName","ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                      "InitBiomass","GrowthRate","CarryingCapacity","Catchability",
                      "CompetitionAlpha","CompetitionBetaSpecies","CompetitionBetaGuilds","CompetitionBetaGuildsGuilds",
                      "PredationRho","PredationHandling","PredationExponent","SurveyQ","Harvest"};
        queryStr   = "SELECT ProjectName,ModelName,SpeName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
        queryStr  += "InitBiomass,GrowthRate,CarryingCapacity,Catchability,CompetitionAlpha,CompetitionBetaSpecies,";
        queryStr  += "CompetitionBetaGuilds,CompetitionBetaGuildsGuilds,PredationRho,PredationHandling,PredationExponent,SurveyQ,Harvest FROM " +
                      nmfConstantsMSSPM::TableForecastUncertainty +
                      " WHERE ProjectName = '" + m_ProjectName +
                      "' AND ModelName = '"    + m_ModelName +
                      "' AND ForecastName = '" + ForecastName + "'";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["ForecastName"].size();
        uncertaintyDataAvailable = (NumRecords == NumSpeciesOrGuilds);
    }

    if (uncertaintyDataAvailable) {
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            param.clear();
            param.emplace_back(QString::fromStdString(dataMap["InitBiomass"][m]));
            param.emplace_back(QString::fromStdString(dataMap["GrowthRate"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CarryingCapacity"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Catchability"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CompetitionAlpha"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CompetitionBetaSpecies"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CompetitionBetaGuilds"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CompetitionBetaGuildsGuilds"][m]));
            param.emplace_back(QString::fromStdString(dataMap["PredationRho"][m]));
            param.emplace_back(QString::fromStdString(dataMap["PredationHandling"][m]));
            param.emplace_back(QString::fromStdString(dataMap["PredationExponent"][m]));
            param.emplace_back(QString::fromStdString(dataMap["SurveyQ"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Harvest"][m]));
            for (int j=0; j<NumParameters; ++j) {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            param[j].toDouble(),m_NumSignificantDigits,2);
                item = new QStandardItem(valueWithComma);
                item->setTextAlignment(Qt::AlignCenter);
                m_SModel->setItem(i, j, item);
            }
            ++m;
        }
    } else {
        clearUncertaintyTable(NumSpeciesOrGuilds,NumParameters);
    }

    m_SModel->setVerticalHeaderLabels(SpeciesOrGuildNames);
    m_SModel->setHorizontalHeaderLabels(m_ParameterNames);
    Forecast_Tab3_UncertaintyTV->setModel(m_SModel);
    Forecast_Tab3_UncertaintyTV->resizeColumnsToContents();
    // Change column width to 0 for any parameters not reflected in the current model
    adjustColumnVisibility();

    return true;
}


void
nmfForecast_Tab3::adjustColumnVisibility()
{
    std::vector<std::string> Forms = {m_GrowthForm,m_HarvestForm,m_CompetitionForm,m_PredationForm};
    std::vector<std::string> vec;
    bool showColumn;

    for (int col=0; col<m_ParameterNames.size(); ++col) {
        showColumn = false;
        for (std::string Form : Forms) {
            vec = m_FormMap[Form];
            for (unsigned i=0; i<vec.size(); ++i) {
                if (m_ParameterNames[col] == QString::fromStdString(vec[i])) {
                    showColumn = true;
                    break;
                }
            }
            if (showColumn) {
                break;
            }
        }
        if (! showColumn) {
            Forecast_Tab3_UncertaintyTV->setColumnWidth(col,0);
        }
    }
}

void
nmfForecast_Tab3::clearUncertaintyTable(int& NumSpeciesOrGuilds, int& NumParameters)
{
    QStandardItem *item;

    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        for (int j=0; j<NumParameters; ++j) {
            item = new QStandardItem(QString(""));
            item->setTextAlignment(Qt::AlignCenter);
            m_SModel->setItem(i, j, item);
        }
    }
}


