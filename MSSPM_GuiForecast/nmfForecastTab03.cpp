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
    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_smodel      = nullptr;
    m_isAggProd   = false;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    m_logger->logMsg(nmfConstants::Normal,"nmfForecast_Tab3::nmfForecast_Tab3");

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
    m_alpha = QString("Competition (") + QChar(0x03B1) + QString(")");
    m_betaS = QString("Competition (") + QChar(0x03B2) + QString("(i))");
    m_betaG = QString("Competition (") + QChar(0x03B2) + QString("(G))");
    m_rho   = QString("Predation (")   + QChar(0x03C1) + QString(")");
    m_FormMap["Linear"]   = {"Growth Rate (r)"};
    m_FormMap["Logistic"] = {"Growth Rate (r)","Carrying Capacity (K)"};
    m_FormMap["F"]        = {"Exploitation"};
    m_FormMap["QE"]       = {"Catchability (q)","Effort"};
    m_FormMap["Catch"]    = {"Catch"};
    m_FormMap["NO_K"]     = {m_alpha.toStdString()};
    m_FormMap["MS-PROD"]  = {m_betaS.toStdString(),m_betaG.toStdString()};
    m_FormMap["AGG-PROD"] = {m_alpha.toStdString(),m_betaG.toStdString()};
    m_FormMap["Type I"]   = {m_rho.toStdString()};
    m_FormMap["Type II"]  = {m_rho.toStdString(),"Handling (h)"};
    m_FormMap["Type III"] = {m_rho.toStdString(),"Handling (h)","Exponent (b)"};

    m_ParameterNames.clear();
    m_ParameterNames << "Growth Rate (r)";
    m_ParameterNames << "Carrying Capacity (K)";
    m_ParameterNames << m_rho;
    m_ParameterNames << m_alpha;
    m_ParameterNames << m_betaS;
    m_ParameterNames << m_betaG;
    m_ParameterNames << "Handling (h)";
    m_ParameterNames << "Exponent (b)";
    m_ParameterNames << "Catchability (q)";
    m_ParameterNames << harvestType();
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
        m_smodel->setItem(selectedIndexesList[i].row(),selectedIndexesList[i].column(), item);
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

    if (m_smodel == NULL) {
        std::cout << "Error: smodel is NULL" << std::endl;
        return;
    }

    for (int j=0; j<m_smodel->rowCount(); ++ j) {
        SpeNames.push_back(m_smodel->verticalHeaderItem(j)->text().toStdString());
    }

    // Get Alg, Min, ObjCrit
    fields     = {"ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr   = "SELECT ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling FROM Forecasts where ";
    queryStr  += "ForecastName = '" + ForecastName + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
    cmd = "DELETE FROM ForecastUncertainty WHERE ForecastName = '" + ForecastName + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Forecast_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from ForecastUncertainty table.\n",
                             QMessageBox::Ok);
        Forecast_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    // Update ForecastUncertainty table
    cmd  = "INSERT INTO ForecastUncertainty (" ;
    cmd += "SpeName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    cmd += "GrowthRate,CarryingCapacity,Predation,Competition,BetaSpecies,";
    cmd += "BetaGuilds,Handling,Exponent,Catchability,Harvest) VALUES ";
    for (int i=0; i<m_smodel->rowCount(); ++i) { // Species
            cmd += "('" + SpeNames[i] + "','" + ForecastName + "','" + Algorithm +
                    "','" + Minimizer + "','" + ObjectiveCriterion + "','" + Scaling + "'";
            for (int j=0; j<m_smodel->columnCount(); ++j) { // Parameters
                index = m_smodel->index(i,j);
                cmd  += "," + index.data().toString().toStdString();
            }
            cmd += "),";
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
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
nmfForecast_Tab3::loadWidgets()
{
std::cout << "nmfForecast_Tab3::loadWidgets()" << std::endl;

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

    // Get Forms to be used later
    fields    = {"ForecastName","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr  = "SELECT ForecastName,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm FROM Forecasts where ";
    queryStr += "ForecastName = '" + ForecastName + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        m_GrowthForm      = dataMap["GrowthForm"][0];
        m_HarvestForm     = dataMap["HarvestForm"][0];
        m_CompetitionForm = dataMap["WithinGuildCompetitionForm"][0];
        m_PredationForm   = dataMap["PredationForm"][0];
    }
    m_isAggProd = (m_CompetitionForm == "AGG-PROD");

    // Get Guild info
    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from Guilds ORDER by GuildName";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    for (int i=0; i<NumGuilds; ++i) {
        GuildNames << QString::fromStdString(dataMap["GuildName"][i]);
    }

    // Get species info
    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    if (m_isAggProd) {
        NumSpeciesOrGuilds  = NumGuilds;
        SpeciesOrGuildNames = GuildNames;
    } else {
        NumSpeciesOrGuilds  = NumSpecies;
        SpeciesOrGuildNames = SpeciesNames;
    }

    m_smodel = new QStandardItemModel( NumSpeciesOrGuilds, NumParameters );

    bool ForecastNameExists = ! ForecastName.empty();
    if (ForecastNameExists) {
        // Find Forecast info
        fields     = {"SpeName","ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                      "GrowthRate","CarryingCapacity","Predation","Competition","BetaSpecies",
                      "BetaGuilds","Handling","Exponent","Catchability","Harvest"};
        queryStr   = "SELECT SpeName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
        queryStr  += "GrowthRate,CarryingCapacity,Predation,Competition,BetaSpecies,";
        queryStr  += "BetaGuilds,Handling,Exponent,Catchability,Harvest FROM ForecastUncertainty where ";
        queryStr  += "ForecastName = '" + ForecastName + "'";
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["ForecastName"].size();
        uncertaintyDataAvailable = (NumRecords == NumSpeciesOrGuilds);
    }

    if (uncertaintyDataAvailable) {
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            param.clear();
            param.emplace_back(QString::fromStdString(dataMap["GrowthRate"][m]));
            param.emplace_back(QString::fromStdString(dataMap["CarryingCapacity"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Predation"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Competition"][m]));
            param.emplace_back(QString::fromStdString(dataMap["BetaSpecies"][m]));
            param.emplace_back(QString::fromStdString(dataMap["BetaGuilds"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Handling"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Exponent"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Catchability"][m]));
            param.emplace_back(QString::fromStdString(dataMap["Harvest"][m]));
            for (int j=0; j<NumParameters; ++j) {
                item = new QStandardItem(param[j]);
                item->setTextAlignment(Qt::AlignCenter);
                m_smodel->setItem(i, j, item);
            }
            ++m;
        }
    } else {
        clearUncertaintyTable(NumSpeciesOrGuilds,NumParameters);
    }

    m_smodel->setVerticalHeaderLabels(SpeciesOrGuildNames);
    m_smodel->setHorizontalHeaderLabels(m_ParameterNames);
    Forecast_Tab3_UncertaintyTV->setModel(m_smodel);
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
            m_smodel->setItem(i, j, item);
        }
    }
}


