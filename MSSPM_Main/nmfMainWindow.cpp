
#include "nmfMainWindow.h"
#include "ui_nmfMainWindow.h"
#include "nmfUtilsQt.h"

#include "nmfConstants.h"
#include "nmfConstantsMSSPM.h"

#include <QLineSeries>
#include <QProcess>
#include <QtConcurrent>
#include <QWhatsThis>

// This is needed since a signal is passing a std::string type
Q_DECLARE_METATYPE (std::string)

nmfMainWindow::nmfMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_UI(new Ui::nmfMainWindow)
{
    m_UI->setupUi(this);

//    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    m_Estimator_Bees   = nullptr;
    m_ShowSignificantDigitsDlg = true;
    m_RunOutputMsg.clear();
    m_NumMohnsRhoRanges = 0;
    m_SeedValue = -1;
    m_ScreenshotOn = false;
    m_NumScreenShot = 0;
    m_Pixmaps.clear();
    m_MShotNumRows = 4;
    m_MShotNumCols = 3;
    m_isStartUpOK = true;
    m_isRunning = false;
    m_NumRuns = 0;
    m_AveBiomass.clear();
    m_OutputBiomassEnsemble.clear();
    m_ProgressChartTimer = nullptr;
    m_ProgressWidget     = nullptr;
    m_MModeViewerWidget  = nullptr;
    m_ViewerWidget = nullptr;
    m_NumSignificantDigits = -1;
    m_IsMultiRun = false;
    m_MultiRunType.clear();
    m_Estimator_NLopt = nullptr;
    m_DBTimeout = nmfConstantsMSSPM::DefaultDBTimeoutDays;
    m_UsingHPCFiles = false;
    m_SingeMultiToggle = 1;
    m_HPC_Algorithm = "";
    m_HPC_Minimizer = "";
    m_HPC_ObjectiveCriterion = "";
    m_HPC_Scaling = "";

    m_ProjectDir.clear();
    m_ProjectDatabase.clear();
    m_ProjectName.clear();
    m_ModelName.clear();
//  Hostname.clear();
    m_Username.clear();
    m_Password.clear();
//  m_Session.clear();
    m_RunNumBees        = 1;
    m_RunNumNLopt       = 0;
    Setup_Tab3_ptr      = nullptr;
    InitBiomassTV       = nullptr;
    GrowthRateTV        = nullptr;
    GrowthRateShapeTV   = nullptr;
    CompetitionAlphaTV  = nullptr;
    CompetitionBetaSTV  = nullptr;
    CompetitionBetaGTV  = nullptr;
    PredationRhoTV      = nullptr;
    PredationHandlingTV = nullptr;
    PredationExponentTV = nullptr;
    SurveyQTV           = nullptr;
    CarryingCapacityTV  = nullptr;
    CatchabilityTV      = nullptr;
    CovariateCoefficientsTV = nullptr;
    m_ChartView3d       = nullptr;
    m_Graph3D           = nullptr;
    OutputChartMainLayt = nullptr;
    Output_Controls_ptr = nullptr;
    Output_TableWidgets_ptr = nullptr;
    Remora_ptr          = nullptr;
    m_PreferencesDlg    = new QDialog(this);
    m_PreferencesWidget = nullptr;
    m_TableNamesWidget  = nullptr;
    m_ViewerWidget      = nullptr;
    m_AveragedData = nmfUtilsStatisticsAveraging();
    m_MohnsRhoData = nmfUtilsStatisticsMohnsRho();

    callback_SetRetrospectiveAnalysis(false);

    OutputChartMainLayt     = nullptr;
    m_ChartWidget           = nullptr;
    m_ChartView2d           = nullptr;
    m_EstimatedParametersTW = nullptr;
    m_EstimatedParametersMap.clear();
    m_UI->actionToggleManagerModeViewer->setEnabled(false);

    m_UI->actionShowAllSavedRunsTB->setEnabled(false);

    // This are needed since a custom signal is passing a custom type
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::string>("nmfStructsQt::ModelDataStruct");
    qRegisterMetaType<std::vector<double> >("std::vector<double>");
    qRegisterMetaType<std::vector<std::string> >("std::vector<std::string>");

    setNumLines(1);

    // Check for and make if necessary hidden dirs for program usage: logs, data
    nmfUtilsQt::checkForAndCreateDirectories(nmfConstantsMSSPM::HiddenDir,
                                 nmfConstantsMSSPM::HiddenDataDir,
                                 nmfConstantsMSSPM::HiddenLogDir);

    // Check for log files
    nmfUtilsQt::checkForAndDeleteLogFiles("MSSPM",
             nmfConstantsMSSPM::HiddenLogDir,
             nmfConstantsMSSPM::LogFilter);

    m_Logger = new nmfLogger();
    m_Logger->initLogger("MSSPM");

    // On Windows, the following Sql code must be done in main .exe file or else
    // the program can't find the libmysql.dll driver.  Not sure why, but moving
    // the following logic from nmfDatabase.dll to here fixes the issue.
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    m_DatabasePtr = new nmfDatabase();
    m_DatabasePtr->nmfSetConnectionByName(db.connectionName());

    readSettingsGuiOrientation(nmfConstantsMSSPM::ResetPositionAlso);
    readSettings();

    m_EstimationCompleteDlg = new EstimationCompleteDialog(this,"Estimation Completed");
    m_TableNamesDlg         = new TableNamesDialog(   this, m_DatabasePtr);
    m_TroubleshootingDlg    = new ScrollingTextDialog(this,"Troubleshooting");
    initializePreferencesDlg();

    // Hide Progress Chart and Log dock widgets. Show them once user does their first MSSPM run.
    setDefaultDockWidgetsVisibility();

    // Prompt user for database login and password
    if (nmfDatabaseUtils::menu_connectToDatabase(
                this,nmfConstantsMSSPM::SettingsDirWindows,m_DatabasePtr,
                m_Username,m_Password))
    {
        queryUserPreviousDatabase();
    } else {
        m_isStartUpOK = false;
        return;
    }
    m_AppStartTime = QDateTime::currentDateTime();

    if (m_LoadLastProject) {
        loadDatabase();
    }

    initializeMMode();
    this->setMouseTracking(true);

    // Setup Log Widget
    setupLogWidget();

    // Setup Output
    setupOutputChartWidgets();
    setupOutputEstimateParametersWidgets();
    setupOutputViewerWidget();
    setupProgressChart();

    // Load GUIs and Connections
    initGUIs();
    setupOutputModelFitSummaryWidgets();
    setupOutputDiagnosticSummaryWidgets();

    initConnections();
    m_SaveSettings = false; // RSK - fix this hack.  Inside completeInit the LoadProject() does a SaveSettings which causes issues with floating windows being displayed properly from the settings
    if (checkIfTablesAlreadyCreated()) {
        completeApplicationInitialization();
    }

    m_SaveSettings = true;
    qApp->installEventFilter(this);

    if (! m_LoadLastProject) {
        Setup_Tab2_ptr->clearProject();
        enableApplicationFeatures("SetupGroup",false);
        enableApplicationFeatures("AllOtherGroups",false);
    }

    // Turn these off for now
    m_UI->actionGeneticTB->setVisible(false);
    m_UI->actionGradientTB->setVisible(false);
    readSettings("style");
    readSettings("OutputDockWidgetIsFloating");
    readSettings("OutputDockWidgetSize");
    readSettings("OutputDockWidgetPos");
    readSettings("LogDockWidgetIsVisible");
    readSettings("ProgressDockWidgetIsVisible");
    readSettings("OutputControls");

//    callback_ModelLoaded();

    // Enable rest of Navigator tree only if user has
    // created and loaded a System file
    enableApplicationFeatures("AllOtherGroups",
        !Setup_Tab4_ptr->getModelName().isEmpty());

    setOutputControlsWidth();
    setDatabaseWaitTime();

    // Clear the output chart upon application start-up
//    callback_ShowChart("","");
    clearChart();
    clearOutputTables();
}


nmfMainWindow::~nmfMainWindow()
{
    delete m_UI;
    if (m_MModeViewerWidget != nullptr) {
        delete m_MModeViewerWidget;
    }
    if (m_ViewerWidget != nullptr) {
        delete m_ViewerWidget;
    }
}

void
nmfMainWindow::setDatabaseWaitTime()
{
    int timeoutSeconds   = m_DBTimeout * nmfConstants::SecondsPerDay;
    std::string cmd      = "SET GLOBAL wait_timeout=" + std::to_string(timeoutSeconds);
    std::string errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);

    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] setDatabaseWaitTime: set global error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }
}

bool
nmfMainWindow::isStartUpOK()
{
    return m_isStartUpOK;
}

//void
//nmfMainWindow::removeExistingMultiRuns()
//{
//    QString ensembleFilesToRemove;
//    QString ensembleFilename = QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename);
//    QStringList parts = ensembleFilename.split(".");
//    if (parts.size() == 2) {
//        ensembleFilesToRemove = parts[0]+"_*." + parts[1];
//        QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
//        fullPath = QDir(fullPath).filePath(ensembleFilesToRemove);
//std::cout << "Removing: " << fullPath.toStdString() << std::endl;
//        QFile::remove(fullPath); // doesn't remove filename with *, need to modify logic here
//    }
//}

void
nmfMainWindow::initializePreferencesDlg()
{
    QUiLoader loader;
    QFile file(":/forms/Main/PreferencesDlg.ui");
    file.open(QFile::ReadOnly);
    m_PreferencesWidget = loader.load(&file,this);
    file.close();

    QSpinBox*    numRowsSB    = m_PreferencesWidget->findChild<QSpinBox   *>("PrefNumRowsSB");
    QSpinBox*    numColumnsSB = m_PreferencesWidget->findChild<QSpinBox   *>("PrefNumColumnsSB");
    QComboBox*   styleCMB     = m_PreferencesWidget->findChild<QComboBox  *>("PrefAppStyleCMB");
    QPushButton* cancelPB     = m_PreferencesWidget->findChild<QPushButton*>("PrefCancelPB");
    QPushButton* okPB         = m_PreferencesWidget->findChild<QPushButton*>("PrefOkPB");
    QCheckBox*   sigdigCB     = m_PreferencesWidget->findChild<QCheckBox  *>("PrefSignificantDigitsCB");
//  QSpinBox*    sigdigSB     = m_PreferencesWidget->findChild<QSpinBox   *>("PrefSignificantDigitsSB");

    QVBoxLayout* layt = new QVBoxLayout();
    layt->addWidget(m_PreferencesWidget);
    m_PreferencesDlg->setLayout(layt);
    m_PreferencesDlg->setWindowTitle("Preferences");

    numRowsSB->setValue(m_MShotNumRows);
    numColumnsSB->setValue(m_MShotNumCols);

    connect(styleCMB,         SIGNAL(currentTextChanged(QString)),
            this,             SLOT(callback_PreferencesSetStyleSheet(QString)));
    connect(okPB,             SIGNAL(clicked()),
            this,             SLOT(callback_PreferencesOkPB()));
    connect(sigdigCB,         SIGNAL(stateChanged(int)),
            this,             SLOT(callback_PreferencesSigDigCB(int)));
    connect(cancelPB,         SIGNAL(clicked()),
            m_PreferencesDlg, SLOT(close()));
}


void
nmfMainWindow::enableApplicationFeatures(std::string navigatorGroup,
                                         bool enable)
{
    QTreeWidgetItem* item;

    this->setCursor(Qt::WaitCursor);

    // Adjust some items in the Setup group
    if (navigatorGroup == "SetupGroup") {
        item = NavigatorTree->topLevelItem(0);
        if (enable) {
            for (int j=2; j<item->childCount(); ++j) {
                item->child(j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
                Setup_Tab2_ptr->enableSetupTabs(true);
            }
        } else {
            for (int j=2; j<item->childCount(); ++j) {
                item->child(j)->setFlags(Qt::NoItemFlags);
                Setup_Tab2_ptr->enableSetupTabs(false);
            }
        }
    } else {
        // Adjust other Navigation groups
        for (int i=1; i<NavigatorTree->topLevelItemCount(); ++i) {
            item = NavigatorTree->topLevelItem(i);
            if (enable) {
                item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            } else {
                item->setFlags(Qt::NoItemFlags);
            }
        }
    }

    this->setCursor(Qt::ArrowCursor);
}

void
nmfMainWindow::queryUserPreviousDatabase()
{
    QMessageBox::StandardButton reply;
    std::string msg  = "\nLast Project worked on:  " + m_ProjectName +
                       "\n\nContinue working with this Project?\n";
    reply = QMessageBox::question(this, tr("Open"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);

    m_LoadLastProject = (reply == QMessageBox::Yes);
}


void
nmfMainWindow::initLogo()
{
    QPixmap logoImage(":/icons/NOAA.png");
    QPixmap logoImageScaled = logoImage.scaled(m_UI->centralWidget->width()+100,
                                               m_UI->centralWidget->width()+100,
                                               Qt::KeepAspectRatio);
    QLabel *logoLBL = new QLabel();
    logoLBL->setObjectName("Logo");
    logoLBL->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    logoLBL->setPixmap(logoImageScaled);
    m_UI->centralWidget->layout()->addWidget(logoLBL);

} // end initLogo



void
nmfMainWindow::setupLogWidget()
{
    m_LogWidget = new nmfLogWidget(m_Logger,nmfConstantsMSSPM::LogDir);
    m_UI->LogWidget->setLayout(m_LogWidget->vMainLayt);
}

void
nmfMainWindow::completeApplicationInitialization()
{
    NavigatorTree->setEnabled(true);
    loadGuis();
    initPostGuiConnections();
}

bool
nmfMainWindow::checkIfTablesAlreadyCreated()
{
    // See if there are any tables in the current database
    std::string msg;
    std::string currentDatabase;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    currentDatabase.clear();
    fields   = {"database()"};
    queryStr = "SELECT database()";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["database()"].size() > 0) {
        currentDatabase = dataMap["database()"][0];
    }

    if (! currentDatabase.empty()) {
        std::vector<std::string> tables = m_DatabasePtr->nmfGetTableNames();
        if (tables.size() == 0) {
            NavigatorTree->setEnabled(false);
            msg = "No tables found. Please run: Utilities ⟶ Create Tables";
            m_Logger->logMsg(nmfConstants::Warning,msg);
            QMessageBox::warning(this, "Warning",
                                 QString::fromStdString("\n"+msg+"\n"),
                                 QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

void
nmfMainWindow::menu_layoutOutput()
{
    m_UI->OutputDockWidget->setFloating(true);
    m_UI->OutputDockWidget->setGeometry(this->x()+this->width()*(2.0/3.0),
                                        this->y(),
                                        1000,350);
    m_UI->OutputDockWidget->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    m_UI->OutputDockWidget->show();
    m_UI->OutputDockWidget->raise();
}

void
nmfMainWindow::menu_layoutDefault()
{
    MModeDockWidget->hide();
    MModeViewerDockWidget->hide();
    setVisibilityToolbarButtons(true);

//  this->addDockWidget(Qt::LeftDockWidgetArea,  m_UI->NavigatorDockWidget);
//  this->addDockWidget(Qt::RightDockWidgetArea, m_UI->OutputDockWidget);
    this->restoreDockWidget(m_UI->NavigatorDockWidget);
    this->restoreDockWidget(m_UI->OutputDockWidget);
    m_UI->NavigatorDockWidget->show();
    m_UI->OutputDockWidget->setFloating(false);
    m_UI->OutputDockWidget->show();
    m_UI->ProgressDockWidget->setFloating(false);
    m_UI->ProgressDockWidget->show();
    m_UI->LogDockWidget->setFloating(false);
    m_UI->LogDockWidget->show();
    centralWidget()->show();
    this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);

    readSettingsGuiOrientation(nmfConstantsMSSPM::DontResetPosition);

}

void
nmfMainWindow::setupOutputChartWidgets()
{
    setup2dChart();
    setup3dChart();
    callback_SetChartView2d(true);
}

void
nmfMainWindow::setup2dChart()
{
    if (OutputChartMainLayt != nullptr) {
        delete OutputChartMainLayt;
    }
    OutputChartMainLayt = new QVBoxLayout();
    if (m_ChartWidget != nullptr) {
        delete m_ChartWidget;
    }
    m_ChartWidget = new QChart();
    if (m_ChartView2d != nullptr) {
        delete m_ChartView2d;
    }
    m_ChartView2d = new QChartView(m_ChartWidget);

    OutputChartMainLayt->addWidget(m_ChartView2d);
    m_UI->MSSPMOutputChartTab->setLayout(OutputChartMainLayt);
}

void
nmfMainWindow::setupOutputEstimateParametersWidgets()
{
    m_EstimatedParametersTW  = new QTabWidget();
    QVBoxLayout* mainLayt    = new QVBoxLayout();

    InitBiomassTV           = new QTableView();
    GrowthRateTV            = new QTableView();
    GrowthRateShapeTV       = new QTableView();
    CompetitionAlphaTV      = new QTableView();
    CompetitionBetaSTV      = new QTableView();
    CompetitionBetaGTV      = new QTableView();
    PredationRhoTV          = new QTableView();
    PredationHandlingTV     = new QTableView();
    PredationExponentTV     = new QTableView();
    SurveyQTV               = new QTableView();
    CarryingCapacityTV      = new QTableView();
    CatchabilityTV          = new QTableView();
    BMSYTV                  = new QTableView();
    MSYTV                   = new QTableView();
    FMSYTV                  = new QTableView();
    OutputBiomassTV         = new QTableView();
    CovariateCoefficientsTV = new QTableView();
    SurveyQTV->setObjectName("SurveyQTV");
    PredationRhoTV->setObjectName("PredationRhoTV");
    GrowthRateShapeTV->setObjectName("GrowthRateShapeTV");

    m_EstimatedParametersTW->addTab(InitBiomassTV,"Initial Absolute Biomass");
    m_EstimatedParametersTW->addTab(GrowthRateTV,"Growth Rate (r)");
    m_EstimatedParametersTW->addTab(GrowthRateShapeTV,"Growth Rate Shape (p)");
    m_EstimatedParametersTW->addTab(CarryingCapacityTV,"Carrying Capacity (K)");
    m_EstimatedParametersTW->addTab(CatchabilityTV,"Catchability (q)");
    m_EstimatedParametersTW->addTab(CompetitionAlphaTV,"Competition ("+QString(QChar(0x03B1))+")");
    m_EstimatedParametersTW->addTab(CompetitionBetaSTV,"Competition ("+QString(QChar(0x03B2))+":Species)");
    m_EstimatedParametersTW->addTab(CompetitionBetaGTV,"Competition ("+QString(QChar(0x03B2))+":Guilds)");
    m_EstimatedParametersTW->addTab(PredationRhoTV,"Predation ("+ QString(QChar(0x03C1)) +")");
    m_EstimatedParametersTW->addTab(PredationHandlingTV,"Predation Handling (h)");
    m_EstimatedParametersTW->addTab(PredationExponentTV,"Predation Exponent (b)");
    m_EstimatedParametersTW->addTab(SurveyQTV,"SurveyQ");
    m_EstimatedParametersTW->addTab(CovariateCoefficientsTV,"Covariate Coefficients");
    m_EstimatedParametersTW->addTab(OutputBiomassTV,"Output Biomass");
    m_EstimatedParametersTW->addTab(BMSYTV,"Biomass MSY (K/2)");
    m_EstimatedParametersTW->addTab(MSYTV, "MSY (rK/4)");
    m_EstimatedParametersTW->addTab(FMSYTV,"Fishing Mortality MSY (r/2)");
    m_EstimatedParametersTW->setObjectName("EstimatedParametersTab");

    m_EstimatedParametersMap["Initial Absolute Biomass"] = InitBiomassTV;
    m_EstimatedParametersMap["Growth Rate (r)"]          = GrowthRateTV;
    m_EstimatedParametersMap["Growth Rate Shape (p)"]    = GrowthRateShapeTV;
    m_EstimatedParametersMap["Carrying Capacity (K)"]    = CarryingCapacityTV;
    m_EstimatedParametersMap["Catchability (q)"]         = CatchabilityTV;
    m_EstimatedParametersMap["Competition ("+QString(QChar(0x03B1))+")"] = CompetitionAlphaTV;
    m_EstimatedParametersMap["Competition ("+QString(QChar(0x03B2))+":Species)"] = CompetitionBetaSTV;
    m_EstimatedParametersMap["Competition ("+QString(QChar(0x03B2))+":Guilds)"] = CompetitionBetaGTV;
    m_EstimatedParametersMap["Predation ("  +QString(QChar(0x03C1))+")"] = PredationRhoTV;
    m_EstimatedParametersMap["Predation Handling (h)"] = PredationHandlingTV;
    m_EstimatedParametersMap["Predation Exponent (b)"] = PredationExponentTV;
    m_EstimatedParametersMap["SurveyQ"]                = SurveyQTV;
    m_EstimatedParametersMap["Output Biomass"]         = OutputBiomassTV;
    m_EstimatedParametersMap["Covariate Coefficients"] = CovariateCoefficientsTV;
    m_EstimatedParametersMap["Biomass MSY (K/2)"]      = BMSYTV;
    m_EstimatedParametersMap["MSY (rK/4)"]             = MSYTV;
    m_EstimatedParametersMap["Fishing Mortality MSY (r/2)"] = FMSYTV;

    QTabBar* bar = m_EstimatedParametersTW->tabBar();
    QFont font = bar->font();
    font.setBold(true);
    bar->setFont(font);

    mainLayt->addWidget(m_EstimatedParametersTW);

    m_UI->MSSPMOutputEstimatedParametersTab->setLayout(mainLayt);

}

bool
nmfMainWindow::getFinalEstimatedBiomass(boost::numeric::ublas::vector<double>& FinalBiomass)
{
    int NumRecords;
    int NumSpecies;
    int RunLength;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList SpeciesList;
    boost::numeric::ublas::matrix<double> EstimatedBiomass;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    // Get RunLength
    fields     = {"ProjectName","ModelName","RunLength"};
    queryStr   = "SELECT ProjectName,ModelName,RunLength from " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" +  m_ProjectName +
                 "' AND ModelName = '"    +  m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] getFinalEstimatedBiomass: No records found.");
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }
    RunLength = std::stoi(dataMap["RunLength"][0]);

    // Get NumSpecies
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    // Get final estimated biomass values
    fields     = {"ProjectName","ModelName","Value"};
    queryStr   = "SELECT ProjectName,ModelName,Value from " +
                  nmfConstantsMSSPM::TableOutputBiomass +
                 " WHERE ProjectName = '" +  m_ProjectName +
                 "' AND ModelName = '"    +  m_ModelName +
                 "' AND Algorithm = '"    +  Algorithm +
                 "' AND Minimizer = '"    +  Minimizer +
                 "' AND Scaling   = '"    +  Scaling +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Year = "                + std::to_string(RunLength);

    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == NumSpecies) {
        nmfUtils::initialize(FinalBiomass,NumSpecies);
        for (int species=0; species<NumSpecies; ++species) {
            FinalBiomass[species] = QString::fromStdString(dataMap["Value"][species]).toDouble();
        }
    } else {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] getFinalEstimatedBiomass: NumRecords found does not equal NumSpecies");
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }

    return true;
}

bool
nmfMainWindow::getFinalObservedBiomass(boost::numeric::ublas::vector<double>& FinalBiomass)
{
    int NumRecords;
    int NumSpecies;
    int RunLength;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList SpeciesList;
    boost::numeric::ublas::matrix<double> ObservedBiomass;

    // Get RunLength
    fields     = {"ProjectName","ModelName","RunLength"};
    queryStr   = "SELECT ProjectName,ModelName,RunLength from " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" +  m_ProjectName +
                 "' AND ModelName = '"    +  m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] getFinalObservedBiomass: No records found.");
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }
    RunLength = std::stoi(dataMap["RunLength"][0]);

    // Get NumSpecies
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return false;

    // Get final observed biomass values
    std::string ObsBiomassTableName = getObservedBiomassTableName(!nmfConstantsMSSPM::PreEstimation);

    if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                           "",ObsBiomassTableName,
                                           NumSpecies,RunLength,ObservedBiomass)) {
        return false;
    }
    nmfUtils::initialize(FinalBiomass,NumSpecies);
    for (int species=0; species<NumSpecies; ++species) {
        FinalBiomass[species] = ObservedBiomass(RunLength,species);
    }

    return true;
}

bool
nmfMainWindow::getOutputParameterMatrix(
        const std::string& TableName,
        const bool& useMultiRun,
        const std::string& ForecastName,
        boost::numeric::ublas::matrix<double>& parameter)
{
    int m=0;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;

    parameter.clear();
    nmfUtils::initialize(parameter,0,0);

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (useMultiRun) {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    } else if (ForecastName.empty()) {
        checkDiagnosticAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    } else {
        checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    }

    fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
    queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " +
                 TableName;
    queryStr += " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + std::to_string(isAggProd());
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = std::sqrt(dataMap["SpeciesA"].size());
    if (NumSpecies == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning 1] getOutputParameterVector: No data found in table: "+TableName);
//      m_Logger->logMsg(nmfConstants::Warning,queryStr);
        return false;
    }

    nmfUtils::initialize(parameter,NumSpecies,NumSpecies);
    for (int row=0; row<NumSpecies; ++row) {
        for (int col=0; col<NumSpecies; ++col) {
            parameter(row,col) = std::stod(dataMap["Value"][m++]);
        }
    }
    return true;
}

bool
nmfMainWindow::getOutputParameterVector(
        const std::string& TableName,
        const bool& useMultiRun,
        const std::string& ForecastName,
        QList<double> &parameter)
{
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;

    parameter.clear();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (useMultiRun) {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    } else if (ForecastName.empty()) {
        checkDiagnosticAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    } else {
        checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    }

    fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
    queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                 TableName;
    queryStr += " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + std::to_string(isAggProd());
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] getOutputParameterVector: No data found in table: "+TableName);
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }
    for (int i=0; i<NumSpecies; ++i) {
        parameter.append(std::stod(dataMap["Value"][i]));
    }

    return true;
}

bool
nmfMainWindow::getInitialObservedBiomass(boost::numeric::ublas::vector<double> &InitBiomass)
{

    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"SpeName","InitBiomass"};
    queryStr   = "SELECT SpeName,InitBiomass from " +
                  nmfConstantsMSSPM::TableSpecies +
                 " ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] getInitialObservedBiomass: No species found in table Species");
        return false;
    }
    nmfUtils::initialize(InitBiomass,NumSpecies);
    for (int i=0; i<NumSpecies; ++i) {
        InitBiomass[i] = std::stod(dataMap["InitBiomass"][i]);
    }

    return true;
}

bool
nmfMainWindow::getSpeciesWithGuilds(int&         NumSpecies,
                                    QStringList& SpeciesList,
                                    QStringList& GuildList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    SpeciesList.clear();

    fields   = {"SpeName","GuildName"};
    queryStr = "SELECT SpeName,GuildName from " + nmfConstantsMSSPM::TableSpecies;
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] getSpeciesWithGuilds: No species found in table Species");
        return false;
    }

    for (int species=0; species<NumSpecies; ++species) {
        SpeciesList << QString::fromStdString(dataMap["SpeName"][species]);
        GuildList << QString::fromStdString(dataMap["GuildName"][species]);
    }

    return true;
}


bool
nmfMainWindow::getDiagnosticsData(
        const int   &NumPoints,
        const int   &NumSpeciesOrGuilds,
        std::string &Algorithm,
        std::string &Minimizer,
        std::string &ObjectiveCriterion,
        std::string &Scaling,
        std::string &isAggProd,
        boost::numeric::ublas::matrix<double> &DiagnosticsValue,
        boost::numeric::ublas::matrix<double> &DiagnosticsFitness)
{
    int m=0;
    int NumRecords;
    int TotalNumPoints = 2*NumPoints+1;
    double fitness;
    std::vector<std::string> fields;
    std::string queryStr;
    std::string errorMsg;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string TableName = Diagnostic_Tab1_ptr->getTableName(Output_Controls_ptr->getOutputParameter());

    DiagnosticsValue.clear();
    DiagnosticsFitness.clear();
    nmfUtils::initialize(DiagnosticsValue,  TotalNumPoints,NumSpeciesOrGuilds);
    nmfUtils::initialize(DiagnosticsFitness,TotalNumPoints,NumSpeciesOrGuilds);

    // Load Diagnostics data
    fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value","Fitness"};
    queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value,Fitness FROM " +
                 TableName +
                "  WHERE ProjectName = '"      + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + isAggProd +
                "  ORDER BY SpeName,Value";
    dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        errorMsg  = "[Warning] getDiagnosticsData: No records found in table: " + TableName;
//      errorMsg += "\n" + queryStr;
        m_Logger->logMsg(nmfConstants::Warning,errorMsg);
        for (int j=0; j<NumSpeciesOrGuilds; ++j) {
            for (int i=0; i<TotalNumPoints; ++i) {
                DiagnosticsValue(i,j)   = 0;
                DiagnosticsFitness(i,j) = 0;
            }
        }
        return false;
    }

    int speciesNum;

    int NumSpecies;
    QStringList SpeciesList;
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    for (int j=0; j<NumSpeciesOrGuilds; ++j) {
        for (int i=0; i<TotalNumPoints; ++i) {
            if (m < NumRecords) {
                speciesNum = SpeciesList.indexOf(QString::fromStdString(dataMap["SpeName"][m]));
                DiagnosticsValue(i,speciesNum) = std::stod(dataMap["Value"][m]);
                fitness = std::stod(dataMap["Fitness"][m]);
                DiagnosticsFitness(i,speciesNum) = fitness;
            }
            ++m;
        }
    }

    return true;
}



int
nmfMainWindow::getNumDistinctRecords(const std::string& field,
                                     const std::string& table)
{
    std::map<std::string, std::vector<std::string> > dataMap;
    std::vector<std::string> fields;
    std::string queryStr;

    fields     = {field};
    queryStr   = "SELECT DISTINCT " + field + " FROM " + table +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    return dataMap[field].size();
}

bool
nmfMainWindow::getMultiScenarioBiomass(
        const std::string& ScenarioName,
        int& NumSpecies,
        int& NumYears,
        const QStringList& SortedForecastLabels,
        QStringList& ForecastLabels,
        std::vector<boost::numeric::ublas::matrix<double> >& MultiScenarioBiomass)
{
    int m=0;
    int NumRecords;
    std::vector<std::string> fields;
    std::string queryStr;
    std::string errorMsg;
    QString msg;
    std::map<std::string, std::vector<std::string> > dataMap;
    boost::numeric::ublas::matrix<double> TmpMatrix;

    MultiScenarioBiomass.clear();

    NumSpecies = getNumDistinctRecords("SpeName",nmfConstantsMSSPM::TableForecastBiomassMultiScenario);
    NumYears   = getNumDistinctRecords("Year",   nmfConstantsMSSPM::TableForecastBiomassMultiScenario);

    fields     = {"ForecastLabel"};
    queryStr   = "SELECT DISTINCT ForecastLabel FROM " +
                  nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName   +
                 "' AND ScenarioName = '" + ScenarioName  +
                 "' ORDER BY ForecastLabel";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastLabel"].size();
    for (int i=0; i<NumRecords; ++i) {
        ForecastLabels << QString::fromStdString(dataMap["ForecastLabel"][i]);
    }

    int NumSortedForecastLabels = SortedForecastLabels.size();
    if (NumSortedForecastLabels > 0) {
        ForecastLabels = SortedForecastLabels;
    }

    for (QString ForecastLabel : ForecastLabels) {

        // Load Forecast Biomass data (ie, calculated from estimated parameters r and alpha)
        fields     = {"ProjectName","ModelName","ScenarioName","ForecastLabel","SpeName","Year","Value"};
        queryStr   = "SELECT ProjectName,ModelName,ScenarioName,ForecastLabel,SpeName,Year,Value FROM " +
                      nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
                     " WHERE ProjectName = '"  + m_ProjectName +
                     "' AND ModelName = '"     + m_ModelName +
                     "' AND ScenarioName = '"  + ScenarioName +
                     "' AND ForecastLabel = '" + ForecastLabel.toStdString() +
                     "' ORDER BY SpeName,Year";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords == 0) {
            errorMsg  = "[Error 1] getMultiScenarioBiomass: No records found in table ForecastBiomassMultiScenario for ScenarioName = '" +
                    ScenarioName + "'";
            errorMsg += "\n" + queryStr;
            m_Logger->logMsg(nmfConstants::Error,errorMsg);
        }

        m = 0;
        for (int i=0; i<NumRecords; ++i) {
            nmfUtils::initialize(TmpMatrix,NumYears,NumSpecies);
            for (int species=0; species<NumSpecies; ++species) {
                for (int time=0; time<NumYears; ++time) {
                    TmpMatrix(time,species) = std::stod(dataMap["Value"][m++]);
                }
            }
            MultiScenarioBiomass.push_back(TmpMatrix);
            i += (NumSpecies*NumYears);
        }

    }

    return true;
}


bool
nmfMainWindow::getOutputBiomass(const int &NumLines,
                                const int &NumSpecies,
                                const int &RunLength,
                                std::vector<std::string> &Algorithms,
                                std::vector<std::string> &Minimizers,
                                std::vector<std::string> &ObjectiveCriteria,
                                std::vector<std::string> &Scalings,
                                std::string              &isAggProd,
                                std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass)
{
    bool isAveraged = isAMultiOrMohnsRhoRun();
    int m=0;
    int NumRecords;
    std::vector<std::string> fields;
    std::string queryStr;
    std::string errorMsg;
    std::string DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    std::string Algorithm          = DefaultAveragingAlgorithm;
    std::string Minimizer          = DefaultAveragingAlgorithm;
    std::string ObjectiveCriterion = DefaultAveragingAlgorithm;
    std::string Scaling            = DefaultAveragingAlgorithm;
    std::string CompetitionForm;
    std::string filterStr;
    std::map<std::string, std::vector<std::string> > dataMapCalculatedBiomass;

    OutputBiomass.clear();

    isAggProd = (isAggProd.empty()) ? "0" : isAggProd;

    if (! isAveraged) {
        m_DatabasePtr->getAlgorithmIdentifiers(
                    this,m_Logger,m_ProjectName,m_ModelName,
                    Algorithm,Minimizer,ObjectiveCriterion,
                    Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    }

    // Load Calculated Biomass data (i.e., calculated from estimated parameters r and alpha)
    fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Year","Value"};
    queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value FROM " +
                 nmfConstantsMSSPM::TableOutputBiomass;
    if ((NumLines == 1) && (! isAtLeastOneFilterPressed())) {
        queryStr += " WHERE ProjectName       = '" + m_ProjectName +
                    "' AND ModelName          = '" + m_ModelName +
                    "' AND Algorithm          = '" + Algorithm +
                    "' AND Minimizer          = '" + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling            = '" + Scaling +
                    "' AND isAggProd          = "  + isAggProd;
    } else {
        filterStr = getFilterButtonsResult();
        if (filterStr.empty()) { // This will never be empty. Fix when implement AggProd fixes.
            queryStr += " WHERE isAggProd = " + isAggProd;
        } else {
            queryStr += filterStr;
        }
    }

    queryStr += " ORDER BY Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year";
    dataMapCalculatedBiomass = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMapCalculatedBiomass["SpeName"].size();
// "NumRecords: " << NumRecords << ", Output biomass: " << QString::fromStdString(queryStr);
    if (NumRecords == 0) {
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Year","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value FROM " +
                     nmfConstantsMSSPM::TableOutputBiomass;
        if ((NumLines == 1) && (! isAtLeastOneFilterPressed())) {
            queryStr += " WHERE ProjectName = '" + m_ProjectName +
                        "' AND ModelName = '" + m_ModelName +
                        "' AND Algorithm = '" + Algorithm +
                        "' AND Minimizer = '" + Minimizer +
                        "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                        "' AND Scaling = '" + Scaling +
                        "' AND isAggProd = "  + isAggProd;
        } else {
            filterStr = getFilterButtonsResult();
            if (filterStr.empty()) { // This will never be empty. Fix when implement AggProd fixes.
                queryStr += " WHERE isAggProd = " + isAggProd;
            } else {
                queryStr += filterStr;
            }
        }
        queryStr += " ORDER BY Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year";
        dataMapCalculatedBiomass = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMapCalculatedBiomass["SpeName"].size();

        // m_Logger->logMsg(nmfConstants::Normal,"q2: "+queryStr);
        // m_Logger->logMsg(nmfConstants::Normal,"2NumRecords = "+std::to_string(NumRecords));

    }

    if (! isAMohnsRhoMultiRun()) {
        if (NumRecords == 0) {
            errorMsg  = "[Error 1] getOutputBiomass: No records found in table OutputBiomass";
            m_Logger->logMsg(nmfConstants::Error,errorMsg);
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return false;
        }
        if (NumRecords != NumLines*NumSpecies*(RunLength+1)) {
            errorMsg  = "[Warning 2] getOutputBiomass: Number of records found (" + std::to_string(NumRecords) + ") in ";
            errorMsg += "table OutputBiomass does not equal number of NumLines*NumSpecies*(RunLength+1) (";
            errorMsg += std::to_string(NumLines) + " * " + std::to_string(NumSpecies) + "*" + std::to_string((RunLength+1)) + "=";
            errorMsg += std::to_string(NumLines*NumSpecies*(RunLength+1)) + ") records";
            errorMsg += "\n" + queryStr;
            m_Logger->logMsg(nmfConstants::Error,errorMsg);
            return false;
        }
    } else {
        return false;
    }

    boost::numeric::ublas::matrix<double> TmpMatrix;
    nmfUtils::initialize(TmpMatrix,RunLength+1,NumSpecies);

    bool firstRecord;
    double val;
    Algorithms.clear();
    Minimizers.clear();
    ObjectiveCriteria.clear();
    Scalings.clear();
    for (int chart=0; chart<NumLines; ++chart) {
        firstRecord = true;
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<=RunLength; ++time) {
                if (firstRecord) {
                    Algorithms.push_back(dataMapCalculatedBiomass["Algorithm"][m]);
                    Minimizers.push_back(dataMapCalculatedBiomass["Minimizer"][m]);
                    ObjectiveCriteria.push_back(dataMapCalculatedBiomass["ObjectiveCriterion"][m]);
                    Scalings.push_back(dataMapCalculatedBiomass["Scaling"][m]);
                    firstRecord = false;
                }
//              val = std::stod(dataMapCalculatedBiomass["Value"][m++]);
                val = QString::fromStdString(dataMapCalculatedBiomass["Value"][m++]).toDouble();
                TmpMatrix(time,species) = QString::number(val,'f',nmfConstantsMSSPM::Output_Precision).toDouble();
            }
        }
        OutputBiomass.push_back(TmpMatrix);
    }

    return true;
}

void
nmfMainWindow::getOutputParameterVector(
        const std::string& tableName,
        std::vector<double> &EstParameter)
{
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QStringList SpeciesList;

    EstParameter.clear();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return;
    }

    fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
    queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " + tableName;
    queryStr  += " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["Value"].size();
    for (int i=0; i<NumRecords; ++i) {
        EstParameter.push_back(std::stod(dataMap["Value"][i]));
    }
}


void
nmfMainWindow::getOutputCompetition(std::vector<double> &EstCompetition)
{
    int m;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QStringList SpeciesList;

    EstCompetition.clear();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;

    fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeciesA","SpeciesB","Value"};
    queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeciesA,SpeciesB,Value FROM " +
                  nmfConstantsMSSPM::TableOutputCompetitionAlpha;
    queryStr  += " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (int(dataMap["Algorithm"].size()) != NumSpecies*NumSpecies) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] GetOutputCompetition: Incorrect number of records found in OutputCompetitionAlpha");
        return;
    }
    m = 0;
    for (int row=0; row<NumSpecies; ++row) {
        for (int col=0; col<NumSpecies; ++col) {
            EstCompetition.push_back(std::stod(dataMap["Value"][m++]));
        }
    }

}


void
nmfMainWindow::menu_stopRun()
{
    m_ProgressWidget->stopAllRuns(true);
//  m_ProgressWidget->writeToStopRunFile();
    enableRunWidgets(true);
}

void
nmfMainWindow::menu_whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void
nmfMainWindow::menu_screenMultiShot()
{
    bool createPdf = false;
    int xOffset = 0;
    int yOffset = 0;
    int totPmHeight = 0;
    int maxPmWidth  = 0;
    int tempTotPmWidth = 0;
    int numImages = (int)m_Pixmaps.size();
    int numRows   = numImages/m_MShotNumCols + 1;
    int numImage=0;
    int imageHeight=0;
    int lastImageHeight=0;
    int quality = -1;
    QPixmap pm;
    QPixmap finalPm;
    QString outputFile,outputFilePdf;
    QString imagePath;

    if (m_ScreenshotOn && (m_Pixmaps.size() > 0)) {

        // Calculate height and width of final composited image
        yOffset = 0;
        for (int i=0; i<numRows; ++i) {
            xOffset = 0;
            lastImageHeight = 0;
            tempTotPmWidth = 0;
            // Lay out all images in a row
            for (int col=0; col<m_MShotNumCols; ++col) {
                if (numImage < numImages) {
                    pm = m_Pixmaps[numImage];
                    tempTotPmWidth += pm.width();
                    imageHeight = pm.height();
                    imageHeight = (imageHeight > lastImageHeight) ? imageHeight : lastImageHeight;
                    lastImageHeight = imageHeight;
                    ++numImage;
                }
            }
            totPmHeight += imageHeight;
            maxPmWidth = (tempTotPmWidth > maxPmWidth) ? tempTotPmWidth : maxPmWidth;
            if (numImage >= numImages) {
                break;
            }
        }

        // Set final image parameters
        QImage resultImage(maxPmWidth,totPmHeight,QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&resultImage);
        QPixmap pmBG = QPixmap(maxPmWidth,totPmHeight);
        pmBG.fill();
        painter.drawPixmap(0,0,pmBG);        

        // Lay out the individual images correctly in the composite image
        numImage = 0;
        yOffset = 0;
        for (int i=0; i<numRows; ++i) {
            xOffset = 0;
            lastImageHeight = 0;
            // Lay out all images in a row
            for (int col=0; col<m_MShotNumCols; ++col) {
                if (numImage < numImages) {
                    pm = m_Pixmaps[numImage];
                    painter.drawPixmap(xOffset,yOffset,pm );
                    xOffset += pm.width();
                    imageHeight = pm.height();
                    imageHeight = (imageHeight > lastImageHeight) ? imageHeight : lastImageHeight;
                    lastImageHeight = imageHeight;
                    ++numImage;
                }
            }
            yOffset += imageHeight;
        }

        // Query the user for name of composite image file

        int resolution = 100;
        ChartSaveDialog *dlg = new ChartSaveDialog(this);

        if (dlg->exec()) {
            outputFile    = dlg->getFilename();
            outputFilePdf = dlg->getFilenamePdf();
            resolution    = dlg->getResolution();
            quality       = dlg->getQuality();
            createPdf     = dlg->createPdf();
        }
        delete dlg;
        if (! outputFile.isEmpty()) {
            int dotsPerMeter = resolution * nmfConstants::INCHES_to_METERS;
            resultImage.setDotsPerMeterX(dotsPerMeter);
            resultImage.setDotsPerMeterY(dotsPerMeter);

            // Write out the final composited image file
            finalPm = QPixmap::fromImage(resultImage);
            saveScreenshot(outputFile,finalPm,quality,imagePath);
            if (createPdf) {
                nmfUtilsQt::savePdf(imagePath,outputFile,resolution,resultImage);
            }
        }
    }

    m_Pixmaps.clear();
    m_NumScreenShot = 0;
    // toggle the boolean indicating multi shot mode is on/off
    m_ScreenshotOn = ! m_ScreenshotOn;
}


void
nmfMainWindow::menu_screenShot()
{
    bool createPdf = false;
    int quality = -1;
    int resolution = 300;
    QPixmap pm;
    QString outputFile;
    QString imagePath;

    if (runningREMORA()) {

        Remora_ptr->grabImage(pm);

        ChartSaveDialog *dlg = new ChartSaveDialog(this);
        if (dlg->exec()) {
            outputFile = dlg->getFilename();
            resolution = dlg->getResolution();
            quality    = dlg->getQuality();
            createPdf  = dlg->createPdf();
        }
        delete dlg;
        if (outputFile.isEmpty()) {
            return;
        }
        saveScreenshot(outputFile,pm,quality,imagePath);
        if (createPdf) {
            nmfUtilsQt::savePdf(imagePath,outputFile,resolution,pm.toImage());
        }
    }
    else {
        QString currentTabName = nmfUtilsQt::getCurrentTabName(m_UI->MSSPMOutputTabWidget);
        if ((currentTabName != "Chart") && (currentTabName != "Screen Shot Viewer")) {
            // Save numeric table
            if (currentTabName == "Estimated Parameters")
            {
                QString currentSubTabName = nmfUtilsQt::getCurrentTabName(m_EstimatedParametersTW);
                nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,"Estimated Parameters",
                                               currentSubTabName.toStdString(),
                                               m_EstimatedParametersMap[currentSubTabName],
                                               nmfConstantsMSSPM::Query_User_For_Filename,
                                               nmfConstantsMSSPM::RemoveCommas,"",
                                               nmfConstantsMSSPM::VerboseOn);
            } else if ((currentTabName == "Data") ||
                       (currentTabName == "Model Fit Summary"))
            {
                nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,"Summary Data",
                                               currentTabName.toStdString(),
                                               m_EstimatedParametersMap[currentTabName],
                                               nmfConstantsMSSPM::Query_User_For_Filename,
                                               nmfConstantsMSSPM::RemoveCommas,"",
                                               nmfConstantsMSSPM::VerboseOn);
            }
            return;
        }


        // Need this so as to ensure each screen shot finishes
        // before the next one starts.
        QApplication::sync();

        setCurrentOutputTab("Chart");

        if (m_ChartView2d->isVisible() || m_ChartView3d->isVisible())
        {
            if (! m_ScreenshotOn) {
                ChartSaveDialog *dlg = new ChartSaveDialog(this);
                if (dlg->exec()) {
                    outputFile = dlg->getFilename();
                    resolution = dlg->getResolution();
                    quality    = dlg->getQuality();
                    createPdf  = dlg->createPdf();
                }
                delete dlg;
                if (outputFile.isEmpty()) {
                    return;
                }
            }

            // Grab the image and store in a pixmap
            if (m_ChartView2d->isVisible()) {
                m_ChartView2d->update();
                m_ChartView2d->repaint();
                pm = m_ChartView2d->grab();
            } else if (m_ChartView3d->isVisible()) {
                m_ChartView3d->update();
                m_ChartView3d->repaint();
                pm.convertFromImage(m_Graph3D->renderToImage(nmfConstants::ANTIALIASING_SAMPLES));
            }

            // Either load pixmap into vector of pixmaps for a composite image
            // or save the pixmap to disk
            if (m_ScreenshotOn) {
                m_Pixmaps.push_back(pm);
            } else {
                saveScreenshot(outputFile,pm,quality,imagePath);
                if (createPdf) {
                    nmfUtilsQt::savePdf(imagePath,outputFile,resolution,pm.toImage());
                }
            }

        }
    }
    // RSK add...
    // else check for visible table

}

void
nmfMainWindow::menu_screenShotAll()
{
    QString currentTabName = nmfUtilsQt::getCurrentTabName(m_UI->MSSPMOutputTabWidget);

    if ((currentTabName != "Chart") && (currentTabName != "Screen Shot Viewer")) {
        return;
    }

    int NumSpeciesOrGuilds = Output_Controls_ptr->getNumberSpecies();
    if (NumSpeciesOrGuilds <= 0)
        return;

    // Flip to last species so clearing (as I do in setup2dChart) will work properly
    Output_Controls_ptr->setSpeciesNum(NumSpeciesOrGuilds-1);

    // Flip through all of the species and build a composite image
    // from each of their screen shots
    menu_screenMultiShot();
    bool is3dImage = m_ChartView3d->isVisible();
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        if (is3dImage) {
            setup3dChart();
        } else {
            setup2dChart();
        }
        Output_Controls_ptr->setSpeciesNum(i);
        menu_screenShot();
    }
    menu_screenMultiShot();
}

void
nmfMainWindow::menu_exportHPCFiles()
{
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling;
    std::string CompetitionForm;
    QString inputFile;
    QString outputFile;
    QString hpcDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataHPCDir));
    hpcDir = QDir(hpcDir).filePath(QString::fromStdString(m_ModelName));
    QString useCovariateData = (Estimation_Tab6_ptr->isCovariateData()) ? "1" : "0";

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,
                nmfConstantsMSSPM::DontShowPopupError);

    QDir pathDir(hpcDir);
    if (! pathDir.exists()) {
        pathDir.mkpath(hpcDir);
    }

    QString msg = "\nThis will export all of the HPC-needed data files to the project's outputData/hpc directory and into the appropriate model's subdirectory.";
    msg += "\n\nOK to continue?\n";
    QMessageBox::StandardButton reply =
            QMessageBox::question(this, tr("HPC Export"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }

    // Write out hpc meta data file
    QString growthForm      = getFormGrowth();
    QString harvestForm     = getFormHarvest();
    QString competitionForm = getFormCompetition();
    QString predationForm   = getFormPredation();
    QString biomassType     = getObsBiomassType();
    QString hpcMetaDataFilename = QDir(hpcDir).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameHPCMetaData));
    QFile file(hpcMetaDataFilename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);

        // Line 1
        stream << "\n# 1. Project Name, Model Name\n";
        stream << QString::fromStdString(m_ProjectName) << ", ";
        stream << QString::fromStdString(m_ModelName)  << "\n";

        // Line 2
        stream << "\n# 2. Growth Form, Harvest Form, Competition Form, Predation Form\n";
        stream << growthForm << ", ";
        stream << harvestForm << ", ";
        stream << competitionForm << ", ";
        stream << predationForm << "\n";

        // Line 3
        stream << "\n# 3. Use covariate data? (0 = no, 1 = yes)\n";
        stream << useCovariateData << "\n";

        // Line 3
        stream << "\n# 4. Algorithm Family, Minimizer, Objective Criterion, Scaling\n";
        stream << QString::fromStdString(Algorithm) << ", ";
        stream << QString::fromStdString(Minimizer) << ", ";
        stream << QString::fromStdString(ObjectiveCriterion) << ", ";
        stream << QString::fromStdString(Scaling) << "\n";

        // Line 4
        stream << "\n# 5. Initial Parameter Value type, Range Jitter Value (%), Range Jitter Repeatable boolean\n";
        stream << Estimation_Tab7_ptr->getEnsembleRandInitParamValue() << ", ";
        stream << Estimation_Tab7_ptr->getEnsembleRangeJitterValue() << ", ";
        stream << Estimation_Tab7_ptr->isEnsembleRangeJitterRepeatable() << "\n";

        // Line 5
        stream << "\n# 6. Number of Total Runs, Stop Tolerance, Stop Time (sec)\n";
        stream << QString::number(Estimation_Tab7_ptr->getEnsembleNumberOfTotalRuns()) << ", ";
        stream << QString::number(Estimation_Tab7_ptr->getCurrentToleranceStopValue()) << ", ";
        stream << QString::number(Estimation_Tab7_ptr->getCurrentStopAfterTime()) << "\n";

        // Line 6
        stream << "\n# 7. Parameter Name, Enabled, Checked, ...\n";
        std::vector<nmfStructsQt::EstimateRunBox> estimatedRunBoxes = Estimation_Tab7_ptr->getEstimateRunBoxes();
        for (unsigned i=0; i<estimatedRunBoxes.size(); ++i) {
            stream << QString::fromStdString(estimatedRunBoxes[i].parameter) << ", ";
            stream << QString::number(estimatedRunBoxes[i].state.first) << ", ";
            stream << QString::number(estimatedRunBoxes[i].state.second);
            if (i < estimatedRunBoxes.size()-1) {
                stream << ", ";
            }
        }
        stream << "\n";

        //stream << "\n# Parameter Name, Enabled, Checked, ...\n";

        file.close();
    }

    //
    // Copy the appropriate model files to the hpc output directory (hpcDir)
    QString inputDataDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // 1. Always copy Species and Guilds files
    copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableSpecies));
    copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableGuilds));

    // 2. Always copy both biomass files (relative and absolute)
    // copyHPCFile(inputDataDir,hpcDir,"Biomass"+biomassType.toLower()+".csv");
    copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableBiomassRelative));
    copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute));

    // 3. Copy Harvest file(s)
    if (harvestForm == nmfConstantsMSSPM::HarvestCatch) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch));
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffort) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableHarvestEffort));
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableHarvestEffort));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableFitWeights));
    }

    // 4. Copy Competition file(s)
    if (competitionForm == nmfConstantsMSSPM::CompetitionNOK) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionAlpha));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionAlphaMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionAlphaMax));
    } else if (competitionForm == nmfConstantsMSSPM::CompetitionMSPROD) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaSpecies));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaGuilds));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaGuildsMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCompetitionBetaGuildsMax));
    }

    // 5. Copy Predation file(s)
    if (predationForm == nmfConstantsMSSPM::PredationTypeI) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRho));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMax));
    } else if (predationForm == nmfConstantsMSSPM::PredationTypeII) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRho));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMax));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandling));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandlingMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandlingMax));
    } else if (predationForm == nmfConstantsMSSPM::PredationTypeIII) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRho));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationRhoMax));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandling));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandlingMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationHandlingMax));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationExponent));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationExponentMin));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TablePredationExponentMax));
    }

    // 6. Check for and include covariate data if present
    if (Estimation_Tab6_ptr->isCovariateData()) {
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCovariate));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCovariate+"_scaled"));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCovariateAssignment));
        copyHPCFile(inputDataDir,hpcDir,QString::fromStdString(nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges));
// RSK make the 2 needed skip files here...
    }

    // Print completed message
    msg  = "\nExport of MSSPM files for HPC run complete.\n\n";
    msg += "Please zip the files and secure copy them to your HPC account.\n\nThey're located in:\n\n";
    msg += hpcDir + "\n";
    QMessageBox::information(this, tr("Export HPC"), tr(msg.toLatin1()), QMessageBox::Ok);
}

void
nmfMainWindow::copyHPCFile(const QString& srcDir,
                           const QString& dstDir,
                           const QString& tableName)
{
    QString srcFile = QDir(srcDir).filePath(tableName+".csv");
    QString dstFile = QDir(dstDir).filePath(tableName+".csv");
    QString msg = srcFile + " to " + dstFile;
    QFile::remove(dstFile); // if it exists, it must be removed prior to the copy command
    if (! QFile::copy(srcFile, dstFile)) {
        m_Logger->logMsg(nmfConstants::Error,"menu_exportHPCFiles: Couldn't copy file " + msg.toStdString());
    }
}

void
nmfMainWindow::loadModelFromHPCMetaFile(std::string& hpcProjectName,
                                        std::string& hpcModelName)
{
    QString hpcDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataHPCDir));
    hpcProjectName = "";
    hpcModelName   = "";

    // Clear run summary as well as all estimated output tables
    Estimation_Tab7_ptr->clearOutputTE();
    clearOutputTables();

    // Read hpc meta file and extract project and model name
    QString fullPath = QDir(hpcDir).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameHPCMetaData));
    QFile file(fullPath);
    if (! file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "Error", file.errorString());
        return;
    }
    QString line;
    QTextStream in(&file);
    bool foundModelName = false;
    while (!in.atEnd() && !foundModelName) {
        line = in.readLine();
        if ((! line.isEmpty()) && (line.trimmed()[0] != '#')) { // project name and model name are first data items in meta file
            QStringList parts = line.split(",");
            if (parts.count() == 2) {
                hpcProjectName = parts[0].trimmed().toStdString();
                hpcModelName   = parts[1].trimmed().toStdString();
                m_Logger->logMsg(nmfConstants::Normal,"Setting project,model: " + hpcProjectName + ", " + hpcModelName);
            } else {
                m_Logger->logMsg(nmfConstants::Error,"Problem parsing HPC Meta file. Read project/model name line: " + line.toStdString());
            }
            foundModelName = true;
        }
    }
    file.close();

//    // Load model name from hpc meta file
//    Setup_Tab4_ptr->setModelName(QString::fromStdString(hpcModelName));
//    Setup_Tab4_ptr->loadModel();
//    callback_ModelSaved();
}

bool
nmfMainWindow::checkForMissingHPCFiles(const int& numHPCFiles)
{
    int numFiles;
    int NumHPCParameterGroups = (int)nmfConstantsMSSPM::HPCFilePrefixes.size();
    QString hpcDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataHPCDir));
    QDir source(hpcDir);
    QStringList files;
    std::string errorMsg;
    QString filename;

    for (int i=0; i<NumHPCParameterGroups; ++i) {
        filename = QString::fromStdString(nmfConstantsMSSPM::HPCFilePrefixes[i] + "_*.csv");
        files = source.entryList(QStringList() << filename, QDir::Files);
        numFiles = files.size();
        if ((numFiles != 0) && (numFiles != numHPCFiles)) {
            errorMsg = "Found only " + std::to_string(numFiles) + " file(s) of form: " +
                    QDir(hpcDir).filePath(filename).toStdString();
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] checkForMissingHPCFiles: " + errorMsg);
            QMessageBox::warning(this, "Warning",
                                 QString::fromStdString("\nChecking completeness of HPC files...\n\n"+errorMsg+"\n\nImport cancelled.\n"), QMessageBox::Ok);
            return false;
        }
    }

    return true;
}

void
nmfMainWindow::menu_importHPCFiles()
{
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling;
    std::string CompetitionForm;
    std::string multiRunModelFilename;
    std::string hpcProjectName,hpcModelName;
    std::string Label = ""; // Temporarily keep blank until find a use for this field
    std::vector<double> fitnessVec;
    QString hpcDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataHPCDir));
    int numHPCFiles = nmfUtilsQt::getNumHPCFiles(hpcDir);
    QMessageBox::StandardButton reply;

    nmfUtilsQt::getHPCFitness(numHPCFiles,hpcDir,fitnessVec);
    if (numHPCFiles == 0) {
        QMessageBox::warning(this, "Warning", "\nNo HPC .csv files found in current project. Please assure that the HPC .csv files are in the project's inputData/hpc directory.\n", QMessageBox::Ok);
        return;
    }

    // Load model from HPCMetaData.csv file in the inputData/hpc directory
    loadModelFromHPCMetaFile(hpcProjectName,hpcModelName);

    QString msg = "\nThis will import HPC .csv files from the project's inputData/hpc directory.";
    msg += "\n\nSetting Project Name to: "    + QString::fromStdString(hpcProjectName);
    msg += "\nNumber of HPC runs to import: " + QString::number(numHPCFiles);
    msg += "\n\nOK to continue?\n";
    reply = QMessageBox::question(this, tr("HPC Import"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }
    m_ProjectName = hpcProjectName;


    msg  = "\nOK to set Model Name to: " + QString::fromStdString(hpcModelName) + " ?\n";
    reply = QMessageBox::question(this, tr("New Model Name"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        m_ModelName   = hpcModelName;
        Setup_Tab4_ptr->setModelName(QString::fromStdString(hpcModelName));
        Setup_Tab4_ptr->loadModel();
        callback_ModelSaved();
    }


    // Check to make sure there are the correct number of each parameter's estimated biomass files.
    if (! checkForMissingHPCFiles(numHPCFiles)) {
        return;
    }

    bool loadOK = loadParameters(m_DataStruct,nmfConstantsMSSPM::VerboseOn);
    if (! loadOK) {
        std::cout << "nmfMainWindow::menu_importHPCFiles: Error loading parameters" << loadOK << std::endl;
        return;
    }
    m_ProgressWidget->stopTimer();
    m_ProgressWidget->startTimer(100);
    m_ProgressWidget->startRun();

    m_UsingHPCFiles = true;

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,
                nmfConstantsMSSPM::DontShowPopupError);

    QProgressDialog progress("\nLoading HPC data files...", "Cancel", 0, numHPCFiles, this);
    progress.setAttribute(Qt::WA_DeleteOnClose, true);
    progress.setModal(true);
    progress.setMinimumDuration(0);
    progress.show();
    progress.setValue(0);
    progress.setLabelText("\nRemoving current HPC run...");

    // Delete all similar, existing HPC runs
    std::string cmd = "DELETE FROM " + nmfConstantsMSSPM::TableOutputBiomassEnsemble +
            " WHERE ProjectName = '"       + m_ProjectName +
            "' AND ModelName = '"          + m_ModelName +
//          "' AND Label = '"              + Label +
            "' AND Algorithm = '"          + Algorithm +
            "' AND Minimizer = '"          + Minimizer +
            "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
            "' AND Scaling = '"            + Scaling + "'";
std::cout << "before import: " << cmd << std::endl;
    std::string errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] menu_importHPCFiles: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        msg = "\n[Error 2] menu_importHPCFiles:  Couldn't delete all records from OutputBiomassEnsemble table.\n";
        QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
        return;
    }

    // Load the HPC runs
    progress.setLabelText("\nLoading HPC data files...");
    for (int run=0; run<numHPCFiles; ++run) {
        progress.setValue(run);
        if (progress.wasCanceled()) {
            break;
        }
        callback_SubRunCompleted(run,
                                 numHPCFiles,
                                 Algorithm,
                                 Minimizer,
                                 ObjectiveCriterion,
                                 Scaling,
                                 multiRunModelFilename,
                                 fitnessVec[run]);
    }
    progress.close();

    // Set these globals so that when statistics run they'll reflect the averaging algorithm
    std::string AveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    m_HPC_Algorithm = AveragingAlgorithm;
    m_HPC_Minimizer = AveragingAlgorithm;
    m_HPC_ObjectiveCriterion = AveragingAlgorithm;
    m_HPC_Scaling = AveragingAlgorithm;

    // Statistics are run from here
    std::string elapsedTime = "";
    std::string summaryMsg = "\nEstimated values created by HPC system.";
    QString msgSuffix = "\nHPC data read.\n";
    callback_AllSubRunsCompleted(elapsedTime,summaryMsg,msgSuffix);

    // Stop all run variables and timer
    m_ProgressWidget->stopAllRuns(false);
    m_ProgressWidget->stopTimer();
    m_IsMultiRun = false;
    Estimation_Tab7_ptr->enableEnsembleControls(false);

    m_SingeMultiToggle = 1;
}

void
nmfMainWindow::menu_importDatabase()
{
    importDatabase("",nmfConstantsMSSPM::VerboseOn);
}

void
nmfMainWindow::importDatabase(const QString& DefaultFilename,
                              const bool& VerboseOn)
{
    QMessageBox::StandardButton reply;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    bool ok = false;
    QStringList items = {};

    // Go to project data page
    setPage(nmfConstantsMSSPM::SectionSetup,
            nmfConstantsMSSPM::PageSetupProject);

    // Ask if user wants to clear the Project meta data
    if (VerboseOn) {
        std::string msg  = "\nDo you want to overwrite current Project data with imported database information?";
        msg += "\n\nYes: Overwrites Project data\nNo: Clears Project data, user enters new Project data\n";
        reply = QMessageBox::question(this, tr("Import Database"), tr(msg.c_str()),
                                      QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    // Do the import
    if (! DefaultFilename.isEmpty()) {
        QString msg = "\nImporting database file:\n\n" + DefaultFilename + "\n\n" +
                "Please be patient. This may take several minutes.\n";
        QMessageBox::information(this, tr("Import Database"), tr(msg.toLatin1()), QMessageBox::Ok);
    }
    QString dbName = m_DatabasePtr->importDatabase(this, m_Logger, m_ProjectDir,
                                                   m_Username, m_Password, DefaultFilename);
qDebug() << "dbName: " << dbName;

    m_Logger->logMsg(nmfConstants::Normal,"Imported database: "+ dbName.toStdString());
    QApplication::sync();
    QApplication::processEvents();
    if (! dbName.isEmpty()) {
        loadGuis(nmfConstantsMSSPM::DontLoadProjectData);
        if (reply == QMessageBox::No) {
            Setup_Tab2_ptr->clearProject();
            QMessageBox::information(this, tr("Project"),
                                     tr("\nPlease fill in Project data fields before continuing."),
                                     QMessageBox::Ok);
            Setup_Tab2_ptr->enableProjectData();
        }
        Setup_Tab2_ptr->setProjectDatabase(dbName);
        Setup_Tab2_ptr->saveProject(nmfConstantsMSSPM::VerboseOff);

        // Need to call menu_createTables() in case some tables are missing
        menu_createTables();

        // Query user to select the desired project
        fields   = {"ProjectName"};
        queryStr = "SELECT DISTINCT ProjectName from " +
                    nmfConstantsMSSPM::TableModels +
                   " ORDER BY ProjectName";
        dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        for (int i=0; i<(int)dataMap["ProjectName"].size(); ++i) {
            items << QString::fromStdString(dataMap["ProjectName"][i]);
        }
        if (items.size() == 0) {
            std::string currentDatabase = m_DatabasePtr->nmfGetCurrentDatabase();
            QString qmsg = "\nNo projects found to load in database: "+QString::fromStdString(currentDatabase)+"\n\n";
            qmsg += "This means that there were no table entries in the models table, meaning either: \n";
            qmsg += " 1. the database didn't import properly or\n";
            qmsg += " 2. the database didn't have any models saved in it\n\n";
            qmsg += "To do:\n";
            qmsg += " 1. Check that the mysql.exe application is in the PATH environment variable.\n";
            qmsg += " 2. At the MySQL command prompt check that there's data in the species table with the following MySQL commands:\n";
            qmsg += "    use keyrun_2022;\n";
            qmsg += "    select * from species;\n";
            QMessageBox::warning(this, "Error", qmsg, QMessageBox::Ok);
            return;
        }
        ok = false;
        QString selectedProjectName = "";
        if (VerboseOn) {
            selectedProjectName = QInputDialog::getItem(this, tr("Select Project"),
                                                        tr("Select initial project to load:"),
                                                        items, 0, false, &ok);
        }
        if (ok && ! selectedProjectName.isEmpty()) {
            Setup_Tab2_ptr->setProjectName(selectedProjectName);
            Remora_ptr->setProjectName(selectedProjectName.toStdString());
        } else { // the user clicked Cancel...so select the first project in the list
            Setup_Tab2_ptr->setProjectName(items[0]);
            Remora_ptr->setProjectName(items[0].toStdString());
        }

        // Check that the project name is the same as at least one of the project names found in the models table
        if (! items.contains(QString::fromStdString(m_ProjectName))) {
            QString qmsg = "\nProject Name must be one of the following:\n\n";
            for (QString item : items) {
                qmsg += item + "\n";
            }
            qmsg += "\nPlease check current Project Name.\n";
            QMessageBox::warning(this, "Error", qmsg, QMessageBox::Ok);
        }

        Setup_Tab2_ptr->saveSettings(nmfConstantsMSSPM::ClearModelName);

        Setup_Tab2_ptr->saveProject(nmfConstantsMSSPM::VerboseOff);
        Setup_Tab2_ptr->callback_ReloadProject();
        if (VerboseOn) {
            QMessageBox::information(this, "Load Model", "\nPlease load the desired model.\n", QMessageBox::Ok);
        }
        setPage(nmfConstantsMSSPM::SectionSetup,
                nmfConstantsMSSPM::PageSetupModel);
    }

}

void
nmfMainWindow::setPage(const QString& section,
                       const int& page)
{
    if (section == nmfConstantsMSSPM::SectionSetup) {
        NavigatorTree->setCurrentIndex(NavigatorTree->model()->index(0,0));
        m_UI->SetupInputTabWidget->setCurrentIndex(page);
    } else if (section == nmfConstantsMSSPM::SectionEstimation) {
        NavigatorTree->setCurrentIndex(NavigatorTree->model()->index(1,0));
        m_UI->EstimationDataInputTabWidget->setCurrentIndex(page);
    } else if (section == nmfConstantsMSSPM::SectionDiagnostic) {
        NavigatorTree->setCurrentIndex(NavigatorTree->model()->index(2,0));
        m_UI->DiagnosticsDataInputTabWidget->setCurrentIndex(page);
    } else if (section == nmfConstantsMSSPM::SectionForecast) {
        NavigatorTree->setCurrentIndex(NavigatorTree->model()->index(3,0));
        m_UI->ForecastDataInputTabWidget->setCurrentIndex(page);
    }
}

void
nmfMainWindow::menu_exportDatabase()
{
    m_DatabasePtr->exportDatabase(this,
                                  m_ProjectDir,
                                  m_Username,
                                  m_Password,
                                  m_ProjectDatabase,"");
}

void
nmfMainWindow::menu_exportAllDatabases()
{
    QList<QString> authDBs = {};
    m_DatabasePtr->getListOfAuthenticatedDatabaseNames(authDBs);

    QList<QString>::iterator authDBsIterator;
    std::string projectDatabase;
    for (authDBsIterator = authDBs.begin(); authDBsIterator != authDBs.end(); authDBsIterator++)
    {
        projectDatabase = authDBsIterator->toStdString();

        m_DatabasePtr->exportDatabase(this,
                                      m_ProjectDir,
                                      m_Username,
                                      m_Password,
                                      projectDatabase,"");
    }
}

bool
nmfMainWindow::runningREMORA()
{
    return MModeDockWidget->isVisible();
}



void
nmfMainWindow::menu_toggleManagerModeViewer()
{
    if (runningREMORA()) {
        if (MModeViewerDockWidget->isVisible()) {
            MModeViewerDockWidget->hide();
            m_UI->actionToggleManagerModeViewer->setChecked(false);
            m_UI->actionOpenCSVFile->setEnabled(false);
        } else {
            showMModeViewerDockWidget();
        }
    }
}

void
nmfMainWindow::menu_openCSVFile()
{
    QString fileName;
    QString pathData         = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataDirMMode));
    QString filenameWithPath = QDir(pathData).filePath(m_MModeViewerWidget->getCurrentFilename());
    QStringList argList = {};

    nmfUtilsQt::switchFileExtensions(filenameWithPath,".csv",{".jpg",".png",".tiff"});

    fileName = QFileDialog::getOpenFileName(
                this, tr("Open CSV File"),
                filenameWithPath, tr("*.csv"));
    if (! fileName.isEmpty()) {
        if (nmfUtils::isOSWindows()) {
            std::string cmd = "start " + fileName.toStdString();
            std::system(cmd.c_str());
        } else {
            std::string cmd = "/usr/bin/libreoffice";
            argList << fileName;
            QProcess::execute(cmd.c_str(),argList);
        }
    }
}

QString
nmfMainWindow::getFormGrowth()
{
    return Setup_Tab4_ptr->getGrowthFormCMB()->currentText();
}

QString
nmfMainWindow::getFormHarvest()
{
    return Setup_Tab4_ptr->getHarvestFormCMB()->currentText();
}

QString
nmfMainWindow::getFormPredation()
{
    return Setup_Tab4_ptr->getPredationFormCMB()->currentText();
}

QString
nmfMainWindow::getFormCompetition()
{
    return Setup_Tab4_ptr->getCompetitionFormCMB()->currentText();
}

bool
nmfMainWindow::getForecastInitialData(
        QString& forecastName,
        int&     numYearsPerRun,
        int&     numRunsPerForecast,
        QString& growthForm,
        QString& harvestForm)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    bool ok;
    QStringList items;

    numYearsPerRun     = 0;
    numRunsPerForecast = 0;

    fields     = {"ProjectName","ModelName","ForecastName","RunLength","NumRuns","GrowthForm","HarvestForm"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,RunLength,NumRuns,GrowthForm,HarvestForm FROM " +
                  nmfConstantsMSSPM::TableForecasts +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    if (! forecastName.isEmpty()) {
        queryStr  += " AND ForecastName = '" + forecastName.toStdString() + "'";
    }
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    if (NumRecords == 0) {
        return false;
    } else if (NumRecords == 1) {
        forecastName       = QString::fromStdString(dataMap["ForecastName"][0]);
        numYearsPerRun     = std::stod(dataMap["RunLength"][0]);
        numRunsPerForecast = std::stod(dataMap["NumRuns"][0]);
        growthForm         = QString::fromStdString(dataMap["GrowthForm"][0]);
        harvestForm        = QString::fromStdString(dataMap["HarvestForm"][0]);
    } else {
        for (int i=0; i<NumRecords; ++i) {
            items << QString::fromStdString(dataMap["ForecastName"][i]);
        }
        QString item = QInputDialog::getItem(this, tr("Forecast List"),
                                             tr("Select a Forecast prior to running REMORA:"),
                                             items, 0, false, &ok);
        if (! ok) {
            return false;
        }
        if (ok && !item.isEmpty()) {
            int index =  items.indexOf(item);
            forecastName       = QString::fromStdString(dataMap["ForecastName"][index]);
            numYearsPerRun     = std::stod(dataMap["RunLength"][index]);
            numRunsPerForecast = std::stod(dataMap["NumRuns"][index]);
            growthForm         = QString::fromStdString(dataMap["GrowthForm"][index]);
            harvestForm        = QString::fromStdString(dataMap["HarvestForm"][index]);
        }
    }

    // Make sure there's data for the selected forecast
    for (std::string tableName : {nmfConstantsMSSPM::TableForecastBiomass,
                                  nmfConstantsMSSPM::TableForecastBiomassMonteCarlo}) {
        fields     = {"ProjectName","ModelName","ForecastName"};
        queryStr   = "SELECT ProjectName,ModelName,ForecastName FROM " + tableName +
                     " WHERE ProjectName = '" + m_ProjectName +
                     "' AND ModelName = '"    + m_ModelName +
                     "' AND ForecastName = '" + forecastName.toStdString() + "'";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["ForecastName"].size();
        if (NumRecords == 0) {
            return false;
        }
    }

    return true;
}

QString
nmfMainWindow::getObsBiomassType()
{
    return Setup_Tab4_ptr->getObsBiomassType();
}

void
nmfMainWindow::menu_toggleManagerMode()
{
    bool ok;
    int numSpecies;
    QString growthForm;
    QString harvestForm;
    QStringList speciesList;
    bool showManagerMode = ! runningREMORA();
    int numYearsPerRun;
    int numRunsPerForecast;
    QString forecastName = "";

    // Set state of relevant menu items
    m_UI->actionToggleManagerModeViewer->setEnabled(showManagerMode);
    if (! showManagerMode) {
        m_UI->actionToggleManagerModeViewer->setChecked(false);
    }

    if (showManagerMode) {
        saveSettings();
        loadAllWidgets();

        // Get desired forecast data and initialize Remora with them
        ok = getForecastInitialData(forecastName,numYearsPerRun,numRunsPerForecast,growthForm,harvestForm);
        if (! ok and forecastName.isEmpty()) {
            m_UI->actionToggleManagerMode->setChecked(false);
            return;
        }
        if (! ok and ! forecastName.isEmpty()) {
            QString msg = "\nPlease make sure a Forecast has been run for: "+forecastName+"\n";
            QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
            m_UI->actionToggleManagerMode->setChecked(false);
            menu_layoutDefault();
            return;
        }
        if (forecastName.isEmpty()) {
            QString msg = "\nAt least one forecast must be created prior to using REMORA.\n";
            QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
            m_UI->actionToggleManagerMode->setChecked(false);
            menu_layoutDefault();
            return;
        }
        Remora_ptr->setForPublishing(
                    Output_Controls_ptr->isCheckedOutputGridLines(),
                    Output_Controls_ptr->getOutputLineWidthData(),
                    Output_Controls_ptr->getOutputFontSizeLabel(),
                    Output_Controls_ptr->getOutputFontSizeNumber(),
                    Output_Controls_ptr->getOutputFont(),
                    Output_Controls_ptr->getOutputLineWidthAxis(),
                    Output_Controls_ptr->getOutputLineWidthAxis());
        Remora_ptr->setForecastName(forecastName);
        Remora_ptr->setForecastNumYearsPerRun(numYearsPerRun);
        Remora_ptr->setForecastNumRunsPerForecast(numRunsPerForecast);
        Remora_ptr->setHarvestType(harvestForm);
        Remora_ptr->enableCarryingCapacityWidgets(growthForm=="Logistic");

        // Set visibility on dock widgets
        m_UI->OutputDockWidget->setVisible(false);
        m_UI->NavigatorDockWidget->setVisible(false);
        m_UI->ProgressDockWidget->setVisible(false);
        m_UI->LogDockWidget->setVisible(false);
        m_UI->centralWidget->setVisible(false);
        this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);

        // Load species into manager mode
        m_DatabasePtr->getSpecies(m_Logger,numSpecies, speciesList);
        Remora_ptr->setSpeciesList(speciesList);

        // Show Manager Mode UI
        MModeDockWidget->setVisible(true);

    } else {
        menu_layoutDefault();
    }

    // Set visibility on toolbar buttons
    setVisibilityToolbarButtons(! showManagerMode);

}

void
nmfMainWindow::setVisibilityToolbarButtons(bool isVisible)
{
    m_UI->actionSaveCurrentRunTB->setEnabled(isVisible);
    m_UI->actionShowCurrentRunTB->setEnabled(isVisible);
    m_UI->actionSaveViewTB->setEnabled(isVisible);
    m_UI->actionBeesTB->setEnabled(isVisible);
    m_UI->actionNLoptTB->setEnabled(isVisible);
    m_UI->actionScreenShotAll->setEnabled(isVisible);
    m_UI->actionMultiShot->setEnabled(isVisible);
    m_UI->actionRunTB->setEnabled(isVisible);
    m_UI->actionToggleSignificantDigits->setEnabled(isVisible);
}

bool
nmfMainWindow::saveScreenshot(QString &outputImageFile,
                              QPixmap &pm,
                              int& quality,
                              QString &pathImage)
{
    QString msg;
//  QString pathImage;
    QString pathData;
    QMessageBox::StandardButton reply;
    QString outputImageFileWithPath;
    QString outputDataFileWithPath;
    QString outputDataFile = outputImageFile;

    nmfUtilsQt::switchFileExtensions(outputDataFile,".csv",{".jpg",".png",".tiff"});

    if (runningREMORA()) {
        pathImage = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDirMMode));
        pathData  = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataDirMMode));
    } else {
        pathImage = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDir));
        pathData  = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataDir));
    }

    // If path doesn't exist make it
    for (QString path : {pathImage,pathData}) {
        QDir pathDir(path);
        if (! pathDir.exists()) {
            pathDir.mkpath(path);
        }
    }
    outputDataFileWithPath  = QDir(pathData).filePath(outputDataFile);
    outputImageFileWithPath = QDir(pathImage).filePath(outputImageFile);
    if (QFileInfo(outputImageFileWithPath).exists()) {
        msg   = "\nFile exists. OK to overwrite?\n";
        reply = QMessageBox::question(this, tr("File Exists"), tr(msg.toLatin1()),
                                      QMessageBox::No|QMessageBox::Yes,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No)
            return false;
    }

    // Save the image
    pm.save(outputImageFileWithPath,nullptr,quality);

    // Save the data file
    if (runningREMORA()) {
        saveRemoraDataFile(outputDataFileWithPath);
    }

    // Notify user image has been saved
    msg = "\nCapture image saved to file:\n\n" + outputImageFileWithPath;
    if (runningREMORA()) {
        msg += "\n\nView captured image in \"REMORA Viewer\" window?\n";
    } else {
        msg += "\n\nView captured image in \"Screen Shot Viewer\" tab?\n";
    }
    reply = QMessageBox::question(this,
                                  tr("Image Saved"),
                                  tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes)
    {
        if (runningREMORA()) {
            // Enable the viewer tab
            m_MModeViewerWidget->updateScreenShotViewer(outputImageFile);
            showMModeViewerDockWidget();
        } else {
            // Enable the viewer tab
            m_ViewerWidget->updateScreenShotViewer(outputImageFile);
            setCurrentOutputTab("Screen Shot Viewer");
        }
    } else {
        // Refresh the viewer image list so that if the user just toggles on the viewer,
        // it will show the most recent images in its combobox and won't need to be refreshed.
        m_MModeViewerWidget->refreshList();
    }

    m_Logger->logMsg(nmfConstants::Normal,"menu_screenshot: Image saved: "+ outputImageFile.toStdString());

    return true;
}

void
nmfMainWindow::saveRemoraDataFile(QString filename)
{
    int NumSpecies=0;
    int RunLength;
    int NumRuns;
    int StartYearForecast;
    int NumYearsPerRun;
    int NumRunsPerForecast;
    int Year;
    double val  = 0;
    double val0 = 0;
    double numerator,denominator;
    QString ForecastName = "";
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string ForecastHarvestType;
    QStringList SpeciesList;
    QString Header = "Year";
    QStringList HeaderList;
    std::vector<boost::numeric::ublas::matrix<double> > ForecastHarvest;
    std::vector<boost::numeric::ublas::matrix<double> > ForecastBiomass;
    QStandardItem* item;
    QString growthForm;
    QString harvestForm;
    QString tableForecastHarvestCatch = QString::fromStdString(nmfConstantsMSSPM::TableForecastHarvestCatch);
    std::map<QString,QString> previousUnits;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;

    m_DatabasePtr->createUnitsMap(m_ProjectName,m_ModelName,previousUnits);

    // Get number of years in forecast and number of species
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;
    HeaderList << "Year";
    for (QString Species : SpeciesList) {
        Header += ", " + Species;
        HeaderList << Species;
    }

    ForecastName = QString::fromStdString(Remora_ptr->getForecastName());
    getForecastInitialData(ForecastName,NumYearsPerRun,NumRunsPerForecast,growthForm,harvestForm);

    // Find Forecast info
    if (! m_DatabasePtr->getForecastInfo(
                m_ProjectName,m_ModelName,ForecastName.toStdString(),RunLength,StartYearForecast,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                ForecastHarvestType,NumRuns)) {
        return;
    }

    // Get ForecastBiomass data
    if (! m_DatabasePtr->getForecastBiomass(
                this,m_Logger,m_ProjectName,m_ModelName,ForecastName.toStdString(),NumSpecies,RunLength,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,ForecastBiomass)) {
        return;
    }

    // Get ForecastHarvest data
    if (! m_DatabasePtr->getForecastHarvest(
                this,m_Logger,m_ProjectName,m_ModelName,
                ForecastName.toStdString(),NumSpecies,RunLength,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                harvestForm.toStdString(),
                ForecastHarvest)) {
        return;
    }
    if (previousUnits.find(tableForecastHarvestCatch) != previousUnits.end()) {
        nmfUtilsQt::convertMatrix(ForecastHarvest[0], previousUnits[tableForecastHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        stream << "Number of Species, " << NumSpecies << "\n";
        stream << "Number of Years, "   << RunLength  << "\n";

        QStandardItemModel* smodel1 = new QStandardItemModel(RunLength,1+NumSpecies);
        QStandardItemModel* smodel2 = new QStandardItemModel(RunLength,1+NumSpecies);
        QStandardItemModel* smodel3 = new QStandardItemModel(RunLength,1+NumSpecies);
        QStandardItemModel* smodel4 = new QStandardItemModel(RunLength,1+NumSpecies);

        // Write out Absolute Biomass data
        stream << "\n" << "# Biomass (absolute)" << "\n";
        stream << Header << "\n";
        for (int i=0; i<=RunLength; ++i) {
            stream << StartYearForecast+i;
            item = new QStandardItem(QString::number(StartYearForecast+i,'d',0));
            item->setTextAlignment(Qt::AlignCenter);
            smodel1->setItem(i, 0, item);
            for (int j=0; j<NumSpecies; ++j) {
                val = ForecastBiomass[0](i,j);
                stream << ", " << val;
                item = new QStandardItem(QString::number(val,'f',6));
                item->setTextAlignment(Qt::AlignCenter);
                smodel1->setItem(i, j+1, item);
            }
            stream << "\n";
        }
        smodel1->setHorizontalHeaderLabels(HeaderList);

        // Write out Relative Biomass data
        stream << "\n" << "# Biomass (relative)" << "\n";
        stream << Header << "\n";
        for (int i=0; i<=RunLength; ++i) {
            stream << StartYearForecast+i;
            item = new QStandardItem(QString::number(StartYearForecast+i,'d',0));
            item->setTextAlignment(Qt::AlignCenter);
            smodel2->setItem(i, 0, item);
            for (int j=0; j<NumSpecies; ++j) {
                val0 = (   i == 0) ? ForecastBiomass[0](0,j) : val0;
                val  = (val0 != 0) ? ForecastBiomass[0](i,j)/val0 : 0;
                stream << ", " << val;
                item = new QStandardItem(QString::number(val,'f',6));
                item->setTextAlignment(Qt::AlignCenter);
                smodel2->setItem(i, j+1, item);
            }
            stream << "\n";
        }
        smodel2->setHorizontalHeaderLabels(HeaderList);

        // Write out Fishing Mortality data (C/Bc)
        stream << "\n" << "# Fishing Mortality" << "\n";
        stream << Header << "\n";
        for (int i=0; i<=RunLength; ++i) {
            stream << StartYearForecast+i;
            item = new QStandardItem(QString::number(StartYearForecast+i,'d',0));
            item->setTextAlignment(Qt::AlignCenter);
            smodel3->setItem(i, 0, item);
            for (int j=0; j<NumSpecies; ++j) {
                numerator   = ForecastHarvest[0](i,j);
                denominator = ForecastBiomass[0](i,j);
                val = (denominator == 0) ? 0 : numerator/denominator;
                stream << ", " << val;
                item = new QStandardItem(QString::number(val,'f',6));
                item->setTextAlignment(Qt::AlignCenter);
                smodel3->setItem(i, j+1, item);
            }
            stream << "\n";
        }
        smodel3->setHorizontalHeaderLabels(HeaderList);

        // Write out harvest scale factor data
        stream << "\n" << "# Harvest Scale Factor" << "\n";
        stream << Header << "\n";
        for (int i=0; i<=RunLength; ++i) {
            stream << StartYearForecast+i;
            Year = StartYearForecast+i;
            item = new QStandardItem(QString::number(Year,'d',0));
            item->setTextAlignment(Qt::AlignCenter);
            smodel4->setItem(i, 0, item);
            for (int j=0; j<NumSpecies; ++j) {
                val = Remora_ptr->getScaleValueFromPlot(j,i);
                stream << ", " << val;
                item = new QStandardItem(QString::number(val,'f',6));
                item->setTextAlignment(Qt::AlignCenter);
                smodel4->setItem(i, j+1, item);
            }
            stream << "\n";
        }
        smodel4->setHorizontalHeaderLabels(HeaderList);

        stream << "\n";
        file.close();

        // Attach the models to their table views
        m_BiomassAbsTV->setModel(smodel1);
        m_BiomassRelTV->setModel(smodel2);
        m_FishingMortalityTV->setModel(smodel3);
        m_HarvestScaleFactorTV->setModel(smodel4);
    }
}

void
nmfMainWindow::showMModeViewerDockWidget()
{
    MModeViewerDockWidget->show();
    m_UI->actionToggleManagerModeViewer->setChecked(true);
    m_UI->actionOpenCSVFile->setEnabled(true);

    QList<QDockWidget*> tabifiedWithViewer = this->tabifiedDockWidgets(MModeViewerDockWidget);
    if (tabifiedWithViewer.size() > 0) {
        MModeViewerDockWidget->raise();
    }
}

void
nmfMainWindow::menu_about()
{
    QString name    = "Multi-Species Surplus Production Model";
    QString version = "MSSPM v1.7.6 ";
    QString specialAcknowledgement = "";
    QString cppVersion   = "C++??";
    QString mysqlVersion = "?";
    QString boostVersion = "?";
    QString nloptVersion = "?";
    QString beesLink;
    QString qdarkLink;
    QString linuxDeployLink;
    QString nloptLink;
    QString boostLink;
    QString mysqlLink;
    QString qtLink;
    QString msg; // = "<strong><br>MSSPM v"+ver+"</strong>";
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString os = QString::fromStdString(nmfUtils::getOS());
    QDateTime currentAppTime = QDateTime::currentDateTime();
    int upTimeSeconds = m_AppStartTime.secsTo(currentAppTime);

    // Define Qt link
    qtLink = QString("<a href='https://www.qt.io'>https://www.qt.io</a>");

    // Find C++ version in case you want it later
    if (__cplusplus == 201103L)
        cppVersion = "C++11";
    else if (__cplusplus == 201402L)
        cppVersion = "C++14";
    else if (__cplusplus == 201703L)
        cppVersion = "C++17";

    // MySQL version and link
    fields   = {"version()"};
    queryStr = "SELECT version()";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["version()"].size() > 0) {
        mysqlVersion = QString::fromStdString(dataMap["version()"][0]);
    }
    mysqlLink = QString("<a href='https://www.mysql.com'>https://www.mysql.com</a>");

    // Boost version and link
    boostVersion = QString::number(BOOST_VERSION / 100000) + "." +
                   QString::number(BOOST_VERSION / 100 % 1000) + "." +
                   QString::number(BOOST_VERSION / 100);
    boostLink = QString("<a href='https://www.boost.org'>https://www.boost.org</a>");

    // NLopt version and link
    // RSK - this line crashes the program on Windows
    // I think it's only a problem when I use NLopt version 2.6.1. Version 2.4.2
    // behaves correctly and doesn't crash on Windows.
    int major,minor,bugfix;
    nlopt::version(major,minor,bugfix);
    nloptVersion = QString::number(major)+"."+QString::number(minor)+"."+QString::number(bugfix);
//  nloptVersion = m_Estimator_NLopt->getVersion();
    nloptLink = QString("<a href='https://nlopt.readthedocs.io'>https://nlopt.readthedocs.io</a>");

    // Bees link
    beesLink = QString("<a href='http://beesalgorithmsite.altervista.org'>http://beesalgorithmsite.altervista.org</a>");

    // QDarkStyle link
    qdarkLink = QString("<a href='https://github.com/ColinDuquesnoy/QDarkStyleSheet'>https://github.com/ColinDuquesnoy/QDarkStyleSheet</a>");

    // linuxdeployqt link
    linuxDeployLink = QString("<a href='https://github.com/probonopd/linuxdeployqt'>https://github.com/probonopd/linuxdeployqt</a>");

    // Build About message
    //msg += QString("<li>")+cppVersion+QString("</li>");
    msg += QString("<li>")+QString("Qt ")+QString::fromUtf8(qVersion())+QString("<br>")+qtLink+QString("</li>");
    msg += QString("<li>")+QString("MySQL ")+mysqlVersion+QString("<br>")+mysqlLink+QString("</li>");
    msg += QString("<li>")+QString("Boost ")+boostVersion+QString("<br>")+boostLink+QString("</li>");
    msg += QString("<li>")+QString("NLopt ")+nloptVersion+QString("<br>")+nloptLink+QString("</li>");
    msg += QString("<li>")+QString("Bees Algorithm (August 4, 2011) - M. Castellani<br>")+beesLink+QString("</li>");
    msg += QString("<li>")+QString("QDarkStyleSheet 2.6.5 - Colin Duquesnoy (MIT License)<br>")+qdarkLink+QString("</li>");
    msg += QString("<li>")+QString("linuxdeployqt 6 (January 27, 2019)<br>")+linuxDeployLink+QString("</li>");
    msg += QString("</ul>");

    nmfUtilsQt::showAboutWidget(this,name,os,upTimeSeconds,version,specialAcknowledgement,msg);
}

void
nmfMainWindow::menu_resetCursor()
{
    QApplication::restoreOverrideCursor();
}

void
nmfMainWindow::menu_runModel()
{
    Estimation_Tab7_ptr->callback_RunPB();
}


void
nmfMainWindow::menu_deleteLayout()
{
    bool ok = true;
    QString fileDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::LayoutDir));
    QDir dir(fileDir);
    QStringList filenames = dir.entryList(QStringList() << "*.*",QDir::Files);
    while (ok) {
        QString layoutFileToDelete = QInputDialog::getItem(this, tr("Delete Layout"),
                                                           tr("Select layout file to delete:"),
                                                           filenames, 0, false, &ok);
        if (!ok) {
            return;
        }
        dir.remove(layoutFileToDelete);
        filenames.removeOne(layoutFileToDelete);
    }

}


void
nmfMainWindow::menu_loadLayout()
{
    bool ok;
    QString filename = QString("layout_") + QString::fromStdString(m_ProjectName) + ".bin";
    QString fileDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::LayoutDir));
    QString filePath = QDir(fileDir).filePath(filename);
    QDir directory(fileDir);

    //assume the directory exists and contains some files and you want all jpg and JPG files
    QStringList filenames = directory.entryList(QStringList() << "*.*",QDir::Files);
    if (filenames.isEmpty()) {
        QMessageBox::warning(this, "Warning", "\nNo saved layout files to load.\n", QMessageBox::Ok);
        return;
    }
    QString selectLayoutFile = QInputDialog::getItem(this, tr("Select Layout"),
                                                tr("Select layout file to load:"),
                                                filenames, 0, false, &ok);
    if (!ok) {
        return;
    }

    QVector<QPoint> points;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        int sizeByteArray;
        QByteArray byteArray;
        QPoint pos;
        QSize theSize;
        QDataStream in(&file);
        in >> sizeByteArray;
        byteArray.resize(sizeByteArray);
        in >> byteArray;
        in >> pos;
        in >> theSize;
        this->restoreState(byteArray);
        move(pos);
        resize(theSize);
        file.close();
        QMessageBox::information(this, "Success", "\nLayout loaded.", QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_saveLayout()
{
    bool ok;
    QString defaultFilename = QString("layout_") + QString::fromStdString(m_ProjectName) + ".bin";
    QString filename = QInputDialog::getText(
                this, tr("Save Layout"), tr("Enter name of Layout file:"),
                QLineEdit::Normal, defaultFilename, &ok);
    if (filename.isEmpty() || !ok) {
        return;
    }
    QString filePath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::LayoutDir));
    filePath = QDir(filePath).filePath(filename);

    // Check if file exists
    if (QFileInfo::exists(filePath) && QFileInfo(filePath).isFile()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Warning"),
            tr("\nFile exists. OK to overwrite? "), QMessageBox::No|QMessageBox::Yes, QMessageBox::Yes);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << this->saveState().size();
        out << this->saveState();
        out << pos();
        out << size();
        file.close();
        QMessageBox::information(this, "Success", "\nLayout saved.", QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_optimizeAllTables()
{
    QString msg = "\nOptimization of all the project's " +
                   QString::number(m_DatabasePtr->nmfGetTableNames().size()).trimmed() +
                  " tables may take a minute or so.\n\nOK to proceed?\n";
    optimizeTables(msg,m_DatabasePtr->nmfGetTableNames());
}

void
nmfMainWindow::menu_optimizeOutputTables()
{
    QString msg = "\nOptimization of all the project's Output tables may take a minute or so.\n\nOK to proceed?\n";
    optimizeTables(msg,nmfConstantsMSSPM::OutputTables);
}

void
nmfMainWindow::optimizeTables(
        const QString& msg,
        const std::vector<std::string>& tablesToOptimize)
{
    std::string cmd,errorMsg;
    int tableNum = 0;
    int numTablesToOptimize = tablesToOptimize.size();;

    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Warning"), tr(msg.toLatin1()),
        QMessageBox::No|QMessageBox::Yes,
        QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }

    QProgressDialog progress("\nOptimizing tables...", "Cancel", 0, numTablesToOptimize, this);
    progress.setAttribute(Qt::WA_DeleteOnClose, true);
    progress.setModal(true);
    progress.setMinimumDuration(0);
    progress.show();
    progress.setValue(0);
    progress.update();
    progress.repaint();
    qApp->processEvents();
    for (std::string tableName : tablesToOptimize) {
        progress.setLabelText("\nOptimizing table: "+QString::fromStdString(tableName));
        progress.setValue(tableNum++);
        if (progress.wasCanceled()) {
            break;
        }
        progress.update();
        qApp->processEvents();
        cmd = "OPTIMIZE TABLE " + tableName;
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] menu_optimizeOutputTables: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QString msg = "\n[Error 2] nmfMainWindow::menu_optimizeOutputTables:  Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n";
            QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }
    }
    progress.close();

}

void
nmfMainWindow::menu_preferences()
{
    m_PreferencesDlg->show();
}

bool
nmfMainWindow::isSignificantDigitsEnabled()
{
    return (m_NumSignificantDigits >= 0);
}

void
nmfMainWindow::menu_toggleSignificantDigits()
{
    QSpinBox*  sigdigSB = m_PreferencesWidget->findChild<QSpinBox *>("PrefSignificantDigitsSB");
    QCheckBox* sigdigCB = m_PreferencesWidget->findChild<QCheckBox*>("PrefSignificantDigitsCB");

    m_NumSignificantDigits = (m_UI->actionToggleSignificantDigits->isChecked()) ? sigdigSB->value() : -1;
    sigdigCB->setChecked(m_NumSignificantDigits >= 0);

    saveSettings();
    loadAllWidgets();

}

void
nmfMainWindow::menu_toggleSingleMulti()
{
   if (m_SingeMultiToggle == 1) {
       menu_showLastSingleRun();
   } else {
       menu_showLastMultiOrHPCRun();
   }
   m_SingeMultiToggle *= -1;
}

void
nmfMainWindow::menu_troubleshooting()
{
    QString eol;
    QString msg;
    QStringList groups;
    QStringList importTopics,guiTopics,estimationTopics,helpTopics,mysqlTopics,modelReviewTopics;
    QStringList importSolutions,guiSolutions,estimationSolutions,helpSolutions,mysqlSolutions,modelReviewSolutions;
    QMessageBox mbox;
    std::vector<QStringList> topics;
    std::vector<QStringList> solutions;

    // Step 1. Add topic in appropriate group, creating a group if need be
    estimationTopics << "Estimation doesn't run";
    estimationTopics << "Multi-run never converges, just increments seed";
    estimationTopics << "Poor fit when estimating parameters";
    estimationTopics << "Still getting poor fit after adjusting ranges";
    estimationTopics << "Tail of estimation curve is too high";
    modelReviewTopics << "How to sort the Model Review table";
    modelReviewTopics << "How to move a column?";
    modelReviewTopics << "How to hide or delete a row?";
    modelReviewTopics << "Notes entered disappear";
    guiTopics        << "Accidentally closed a sub-panel";
    guiTopics        << "How to revert back to default GUI layout";
    helpTopics       << "How to get help";
    helpTopics       << "Is there a log file?";
    helpTopics       << "No Log file found";
    importTopics     << "Import doesn't work";
    importTopics     << "No Models available after an Import";
    mysqlTopics      << "Can't connect to MySQL";
    mysqlTopics      << "Table read/write is slow";

    // Step 2. Add the group name to the groups list
    groups << "Estimation" << "GUI" << "Help" << "Import" << "Model Review" << "MySQL";

    // Step 3. Add the topic solutions here
    estimationSolutions  << "If estimation doesn't produce any plots it could be a permission issue. If on Windows, exit the application and re-run as Administrator.";
    estimationSolutions  << "If a multi-run continually selects a new seed and never converges, it may be because there are no ranges around any of the input parameters.";
    estimationSolutions  << QString("Try one or more of the following:<ul>") +
                            QString("<li>Adjust parameter ranges and/or initial values</li>") +
                            QString("<li>Adjust model algorithm and/or algorithm parameters</li>") +
                            QString("<li>Adjust stop tolerance</li>") +
                            QString("<li>Adjust stop time</li>") +
                            QString("</ul>");
    estimationSolutions  << "Check that the estimated parameter values aren't running up against either the max/min range limit. If so, adjust the corresponding range limit.";
    estimationSolutions  << "Try increasing the maximum range value of the initial biomass parameter.";
    modelReviewSolutions << "The Model Review table can be sorted by clicking on a column heading.";
    modelReviewSolutions << "A Model Review table column can be moved by clicking and dragging a column heading.";
    modelReviewSolutions << "One or more Model Review rows can be hidden or deleted by first selecting the row(s) and then right-clicking over the table to raise the context menu.";
    modelReviewSolutions << "After entering text in the Notes column, you must click the Save button in order to write out and save the notes to the appropriate database table.";
    guiSolutions         << "To show a hidden sub-panel, right-click on the main menu bar and select the sub-panel to show.";
    guiSolutions         << "From main menu, select Layouts->Default to revert to default layout.";
    helpSolutions        << QString("MSSPM has 2 forms of help:<ol>") +
                            QString("<li>Hover Help<br><br>- Hold cursor over GUI item for a brief popup<br></li>") +
                            QString("<li>\"What's This?\" Help<br><br>- Click the arrow/question mark button at the end of the "
                                    "main menu toolbar and then click over a GUI item when the cursor becomes a question mark with an arrow at the end</li>") +
                            QString("</ol>");
    helpSolutions        << "Yes, there's a time-stamped log file that's generated on every run. To view it, click on the Log tab of the Log/Progress Chart panel at the bottom of the application and then click the Refresh button. If the panel isn't visible, right-click on the main menu bar and select \"Log\".";
    helpSolutions        << "If no Log files found on disk after clicking the Refresh button in the Log panel it could mean a permission problem. Exit the application and re-run as Administrator.";
    importSolutions      << "The import feature uses the mysql exe found on disk. Please make sure the executable is found when you type it at the command prompt. On Windows, must add mysql.exe path to Path system environment variable";
    importSolutions      << "The project name of an imported database must be the same as the current project name.";
    mysqlSolutions       << "This will happen if the user enters the incorrect MySQL password when starting the application or if the MySQL server needs to be restarted.";
    mysqlSolutions       << QString("This may happen if there are many large read/write operations on a table. If this occurs, the ") +
                            "user can optimize the database by running either optimize command found under the Utilities main menu item:<ul>" +
                            "<li>Optimize Output Tables</li>" +
                            "<li>Optimize All Tables</li></ul>";

    // Display the troubleshooting topics as links
    int i=1;
    msg = "<h3 id=\"top\"><center><b><br>Topics</b></center></h3>";

    topics = {estimationTopics,guiTopics,helpTopics,importTopics,modelReviewTopics,mysqlTopics};
    int groupNum = 0;
    for (QString group : groups) {
        msg += "<br><b>" + group + "</b><br>";
        for (QString aTopic : topics[groupNum++]) {
            msg += "&nbsp;&nbsp;" + QString::number(i) + ". " + "<a href=\"#Topic" + QString::number(i) + "\">" + aTopic + "</a><br>";
            ++i;
        }
    }

    // Display the troubleshooting topics and solutions as anchors with a "top" link back to the list of topics
    i = 1;
    groupNum = 0;
    QStringList aSolution;
    solutions = {estimationSolutions,guiSolutions,helpSolutions,importSolutions,modelReviewSolutions,mysqlSolutions};
    msg += "<h3 id=\"solutions\"><center><b>Solutions</b></center></h3>";
    for (QString group : groups) {
        int j=0;
        for (QString aTopic : topics[groupNum]) {
            aSolution = solutions[groupNum];
            eol  = (aSolution[j].contains("<li>")) ? "" : "<br>";
            msg += "<h4 id=\"Topic"+QString::number(i)+"\"><br><b>"+QString::number(i)+". "+aTopic+"</h4>";
            msg += aSolution[j] + eol;
            msg += "<br><a href=\"#top\">top</a>";
            ++j;
            ++i;
        }
        ++groupNum;
    }

    m_TroubleshootingDlg->loadText(msg);
    m_TroubleshootingDlg->show();
}

void
nmfMainWindow::menu_createSimulatedBiomass()
{
    nmfStructsQt::ModelDataStruct dataStruct;
    bool loadOK = loadParameters(dataStruct,nmfConstantsMSSPM::VerboseOff);
    if (! loadOK) {
        std::cout << "nmfMainWindow::menu_createSimulatedBiomass: Error loading parameters" << loadOK << std::endl;
        return;
    }
    SimulatedBiomassDialog simDlg(this,m_ProjectDir,m_ProjectName,m_ModelName,
                                  m_DatabasePtr,m_Logger,dataStruct);
    simDlg.exec();
}

void
nmfMainWindow::menu_createTables()
{
    Setup_Tab2_ptr->createTables(QString::fromStdString(m_ProjectDatabase));
    completeApplicationInitialization();
}

void
nmfMainWindow::setupOutputModelFitSummaryWidgets()
{
//    Output_TableWidgets_ptr->setupSummaryWidgets(m_UI->MSSPMOutputModelFitSummaryTab);

    QVBoxLayout* mainLayt    = new QVBoxLayout();
    QVBoxLayout* summaryLayt = new QVBoxLayout();
    QHBoxLayout* buttonLayt  = new QHBoxLayout();
    QPushButton* loadPB      = new QPushButton("Load");
    QPushButton* importPB    = new QPushButton("Import...");
    QPushButton* exportPB    = new QPushButton("Export...");
    SummaryGB                = new QGroupBox("Summary Statistics:");
    SummaryTV                = new QTableView();
    buttonLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    buttonLayt->addWidget(loadPB);
    buttonLayt->addWidget(importPB);
    buttonLayt->addWidget(exportPB);
    buttonLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    loadPB->setToolTip("Reloads the above table from the saved database table");
    loadPB->setStatusTip("Reloads the above table from the saved database table");
    importPB->setToolTip("Import a previously exported CSV file");
    importPB->setStatusTip("Import a previously exported CSV file");
    exportPB->setToolTip("Export the current table to a CSV file");
    exportPB->setStatusTip("Export the current table to a CSV file");

    summaryLayt->addWidget(SummaryTV);
    summaryLayt->addLayout(buttonLayt);
    SummaryGB->setLayout(summaryLayt);
    mainLayt->addWidget(SummaryGB);

    m_EstimatedParametersMap["Model Fit Summary"] = SummaryTV;
    m_EstimatedParametersMap["Data"]              = m_UI->OutputDataTV;

    // Make the title bold
    QFont font = SummaryGB->font();
    font.setBold(true);
    SummaryGB->setFont(font);
    font.setBold(false);
    SummaryTV->setFont(font);
    loadPB->setFont(font);
    importPB->setFont(font);
    exportPB->setFont(font);

    connect(loadPB,   SIGNAL(clicked()),
            this,     SLOT(callback_SummaryLoadPB()));
    connect(exportPB, SIGNAL(clicked()),
            this,     SLOT(callback_SummaryExportPB()));
    connect(importPB, SIGNAL(clicked()),
            this,     SLOT(callback_SummaryImportPB()));

    m_UI->MSSPMOutputModelFitSummaryTab->setLayout(mainLayt);
    setSummaryStatisticsHelp();
}


void
nmfMainWindow::setupOutputDiagnosticSummaryWidgets()
{
    QVBoxLayout* mainLayt    = new QVBoxLayout();
    QVBoxLayout* summaryLayt = new QVBoxLayout();
    QHBoxLayout* buttonLayt  = new QHBoxLayout();
    QPushButton* loadPB      = new QPushButton("Load");
    QPushButton* importPB    = new QPushButton("Import...");
    QPushButton* exportPB    = new QPushButton("Export...");
    DiagnosticSummaryGB      = new QGroupBox("Mohn's Rho Summary Statistics:");
    DiagnosticSummaryTV      = new QTableView();

    summaryLayt->addWidget(DiagnosticSummaryTV);
    summaryLayt->addLayout(buttonLayt);
    DiagnosticSummaryGB->setLayout(summaryLayt);
    mainLayt->addWidget(DiagnosticSummaryGB);

    buttonLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    buttonLayt->addWidget(loadPB);
    buttonLayt->addWidget(importPB);
    buttonLayt->addWidget(exportPB);
    buttonLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    loadPB->setToolTip("Reloads the above table from the saved database table");
    loadPB->setStatusTip("Reloads the above table from the saved database table");
    importPB->setToolTip("Import a previously exported CSV file");
    importPB->setStatusTip("Import a previously exported CSV file");
    exportPB->setToolTip("Export the current table to a CSV file");
    exportPB->setStatusTip("Export the current table to a CSV file");

    // Make the title bold
    QFont font = DiagnosticSummaryGB->font();
    font.setBold(true);
    DiagnosticSummaryGB->setFont(font);
    font.setBold(false);
    DiagnosticSummaryTV->setFont(font);
    loadPB->setFont(font);
    importPB->setFont(font);
    exportPB->setFont(font);

    connect(loadPB,   SIGNAL(clicked()),
            this,     SLOT(callback_SummaryLoadDiagnosticPB()));
    connect(exportPB, SIGNAL(clicked()),
            this,     SLOT(callback_SummaryExportDiagnosticPB()));
    connect(importPB, SIGNAL(clicked()),
            this,     SLOT(callback_SummaryImportDiagnosticPB()));

    m_UI->MSSPMOutputDiagnosticSummaryTab->setLayout(mainLayt);
    setDiagnosticSummaryHelp();
}


bool
nmfMainWindow::saveSigDigState()
{
    // If user selected significant digits, temporarily turn them off so the highest
    // precision of the values will be saved.  Then turn them on later.
    bool isSigDigChecked = m_UI->actionToggleSignificantDigits->isChecked();
    m_UI->actionToggleSignificantDigits->setChecked(false);
    menu_toggleSignificantDigits();

    return isSigDigChecked;
}

void
nmfMainWindow::restoreSigDigState(bool state)
{
    // If the significant digits were just turned off a few lines up, turn
    // them back on.  This is so the highest precision of values will be saved which
    // is necessary so as not to lose an numerical precision when toggling between
    // significant digits on and off.
    m_UI->actionToggleSignificantDigits->setChecked(state);
    menu_toggleSignificantDigits();
}

void
nmfMainWindow::callback_SummaryExportDiagnosticPB()
{
    bool isSigDigChecked = saveSigDigState();
    Output_TableWidgets_ptr->exportSummaryTable(DiagnosticSummaryTV,
                                                "Summary Diagnostic",
                                                nmfConstantsMSSPM::FilenameSummaryDiagnostic);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::callback_SummaryExportPB()
{
    bool isSigDigChecked = saveSigDigState();
    Output_TableWidgets_ptr->exportSummaryTable(SummaryTV,
                                                "Summary Model Fit",
                                                nmfConstantsMSSPM::FilenameSummaryModelFit);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::callback_SummaryImportDiagnosticPB()
{
    bool isSigDigChecked = saveSigDigState();
    Output_TableWidgets_ptr->importSummaryTable(DiagnosticSummaryTV,
                                                "Summary Diagnostic",
                                                nmfConstantsMSSPM::FilenameSummaryDiagnostic);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::callback_SummaryImportPB()
{
    bool isSigDigChecked = saveSigDigState();
    Output_TableWidgets_ptr->importSummaryTable(SummaryTV,
                                                "Summary Model Fit",
                                                nmfConstantsMSSPM::FilenameSummaryModelFit);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::callback_SummaryLoadDiagnosticPB()
{
    bool isSigDigChecked = saveSigDigState();
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    QStringList EstParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();
    Output_TableWidgets_ptr->loadSummaryTable(DiagnosticSummaryTV,"Summary Diagnostic",EstParamNames,
                                              Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                              nmfConstantsMSSPM::SummaryDiagnosticStatisticNames);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::getCurrentAlgorithmIdentifiers(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling)
{
    std::string CompetitionForm;  // ignore this for now

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! m_HPC_Algorithm.empty() && ! m_HPC_Minimizer.empty() &&
        ! m_HPC_ObjectiveCriterion.empty() && ! m_HPC_Scaling.empty()) {
        Algorithm          = m_HPC_Algorithm;
        Minimizer          = m_HPC_Minimizer;
        ObjectiveCriterion = m_HPC_ObjectiveCriterion;
        Scaling            = m_HPC_Scaling;
    }
}

void
nmfMainWindow::callback_SummaryLoadPB()
{
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    bool isSigDigChecked = saveSigDigState();
    QStringList EstParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();

    getCurrentAlgorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

    Output_TableWidgets_ptr->loadSummaryTable(SummaryTV,"Summary Model Fit",EstParamNames,
                                              Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                              nmfConstantsMSSPM::SummaryModelFitStatisticNames);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::callback_ReadProgressChartDataFile()
{
    bool validPointsOnly = m_ProgressWidget->readValidPointsOnly();
    callback_ReadProgressChartDataFile(validPointsOnly,false);
}

void
nmfMainWindow::callback_ReadProgressChartDataFile(bool validPointsOnly,
                                                  bool clearChart)
{
    QString outputMsg;

    if (clearChart) {
        m_ProgressWidget->clearChartOnly();
    }
    m_ProgressWidget->readChartDataFile("MSSPM",
                                        nmfConstantsMSSPM::MSSPMProgressChartFile,
                                        nmfConstantsMSSPM::MSSPMProgressChartLabelFile,
                                        validPointsOnly);
    std::string runName = "";
    std::string stopRunFile = nmfConstantsMSSPM::MSSPMStopRunFile;
    std::string state = "";
    std::string msg1,msg2;
    if (m_isRunning) {
        m_ProgressWidget->updateTime();
    }


    if (nmfUtils::isStopped(runName,msg1,msg2,stopRunFile,state))
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_isRunning = false;
        m_ProgressWidget->stopTimer();
        // Read chart once more just in case you've missed the last point.
        m_ProgressWidget->readChartDataFile("MSSPM",
                                            nmfConstantsMSSPM::MSSPMProgressChartFile,
                                            nmfConstantsMSSPM::MSSPMProgressChartLabelFile,
                                            validPointsOnly);
//        outputMsg = "<br>" + QString::fromStdString(msg1);
//        Estimation_Tab7_ptr->appendOutputTE(outputMsg);
std::cout << "### Was it stopped: " << m_ProgressWidget->wasStopped() << std::endl;
        if (m_ProgressWidget->wasStopped()) {
            QString elapsedTime = QString::fromStdString(
                        "Elapsed Time: " + m_ProgressWidget->getElapsedTime());
            Estimation_Tab7_ptr->appendOutputTE(elapsedTime);
        } else {
            outputMsg = "<br>" + QString::fromStdString(msg1);
            Estimation_Tab7_ptr->appendOutputTE(outputMsg);
//            m_ProgressWidget->callback_stopPB(true);
        }
        QApplication::restoreOverrideCursor();
    }
    if (clearChart) {
        m_ProgressWidget->callback_rangeSetPB();
    }
}

void
nmfMainWindow::setupOutputViewerWidget()
{
    // Setup viewer widget into tab
    QString imagePath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputImages");
    m_ViewerWidget = new nmfViewerWidget(this,imagePath,m_Logger);
    m_ViewerWidget->hideFrame();
    m_ViewerWidget->hideDataTab();

    QVBoxLayout* vlayt = new QVBoxLayout();
    vlayt->setContentsMargins(0,0,0,0);
    vlayt->addWidget(m_ViewerWidget->getMainWidget());
    m_UI->MSSPMOutputScreenShotViewerTab->setLayout(vlayt);
    m_UI->MSSPMOutputScreenShotViewerTab->setContentsMargins(0,0,0,0);
}

void
nmfMainWindow::setupProgressChart()
{
    int NumRecords;
    int NumGenerations;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Load RunLength
    fields     = {"GAGenerations"};
    queryStr   = "SELECT GAGenerations FROM " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["GAGenerations"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] nmfMainWindow::setupProgressChart: No records found in table Models for Name = "+m_ModelName);
        NumGenerations = 200;
    } else {
        NumGenerations = std::stoi(dataMap["GAGenerations"][0]);
    }

    if (m_ProgressChartTimer == nullptr) {
        m_ProgressChartTimer = new QTimer(this);
        disconnect(m_ProgressChartTimer,0,0,0);
        connect(m_ProgressChartTimer, SIGNAL(timeout()),
                this,                 SLOT(callback_ReadProgressChartDataFile()));
    }

    if (m_ProgressWidget == nullptr) {
        m_ProgressWidget = new nmfProgressWidget(m_ProgressChartTimer,
                                                 m_Logger,
                                                 "MSSPM",
                                                 "<b>Fitness Convergence Value per Generation</b>",
                                                 "Generations",
                                                 "Fitness Convergence Value",
                                                 0.0,(double)NumGenerations,5.0,
                                                 0.0,40.0,2.0);
        m_ProgressWidget->setupConnections();
        updateProgressChartAnnotation(0,(double)NumGenerations,5.0);
        m_UI->ProgressWidget->setLayout(m_ProgressWidget->hMainLayt);
    }

    // Initialize progress output file
    m_ProgressWidget->clearChartData(nmfConstantsMSSPM::MSSPMProgressChartFile);

    // Initialize progress StopRun file
    std::ofstream outputFile(nmfConstantsMSSPM::MSSPMStopRunFile);
    outputFile << "Ready" << std::endl;
    outputFile.close();

} // end setupProgressChart


void
nmfMainWindow::menu_clear()
{
    QString retv = nmfUtilsQt::clear(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Clear"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_copy()
{
    QString msg;
    QTableView* tableViewWithFocus;

    // This causes a hard crash if user copies cells one of which has a combobox, so need this check.
    try {
        tableViewWithFocus = findTableInFocus();
    } catch (...) {
        std::cout << "\nError: Invalid cells found to copy." << std::endl;
        return;
    }

if (m_ShowSignificantDigitsDlg && isSignificantDigitsEnabled()) {
        msg += "\nSignificant digits are currently enabled. Toggle them\n";
        msg += "off if you want to copy the data at full precision.\n\n";
        msg += "Continue with the Copy?\n";

        nmfMessageWithDontShowDialog testDlg(this,msg);
        testDlg.exec();
        m_ShowSignificantDigitsDlg = testDlg.showFutureDialogs();
        if (! testDlg.wasYesClicked()) {
            saveSettings();
            return;
        }
    }

    // Do the copy
    QString retv = nmfUtilsQt::copy(qApp,tableViewWithFocus);
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Copy"),retv,QMessageBox::Ok);
    }

    saveSettings();
}

void
nmfMainWindow::menu_paste()
{
    QString retv = nmfUtilsQt::paste(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Paste"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_pasteAll()
{
    QString msg;
    QTableView* tableViewWithFocus;

    // This causes a hard crash if user copies cells one of which has a combobox, so need this check.
    try {
        tableViewWithFocus = findTableInFocus();
    } catch (...) {
        std::cout << "\nError: Invalid cells found to copy." << std::endl;
        return;
    }

    if (m_ShowSignificantDigitsDlg && isSignificantDigitsEnabled()) {
        msg  = "\nSignificant digits are currently enabled. Toggle them\n";
        msg += "off if you want to copy/paste the selected cell's data at full precision.\n\n";
        msg += "Continue with the Paste All?\n";

        nmfMessageWithDontShowDialog testDlg(this,msg);
        testDlg.exec();
        m_ShowSignificantDigitsDlg = testDlg.showFutureDialogs();
        if (! testDlg.wasYesClicked()) {
            saveSettings();
            return;
        }
    }

    // Do the paste
    QString retv = nmfUtilsQt::pasteAll(qApp,tableViewWithFocus);
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Paste All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_clearAll()
{
    QString retv = nmfUtilsQt::clearAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Clear All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_selectAll()
{
    QString retv = nmfUtilsQt::selectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Select All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_deselectAll()
{
    QString retv = nmfUtilsQt::deselectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Deselect All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_clearSpecificOutputData()
{
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;

    ClearOutputDialog* ClearOutputDlg = new ClearOutputDialog(this,m_DatabasePtr);
    if (ClearOutputDlg->exec()) {  // OK pressed

        Algorithm          = ClearOutputDlg->getAlgorithm();
        Minimizer          = ClearOutputDlg->getMinimizer();
        ObjectiveCriterion = ClearOutputDlg->getObjectiveCriterion();
        Scaling            = ClearOutputDlg->getScaling();

        clearOutputData(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    }
}


void
nmfMainWindow::menu_clearOutputData()
{
    m_AveBiomass.clear();
    nmfUtils::initialize(m_AveBiomass,0,0);
    m_OutputBiomassEnsemble.clear();
    clearOutputData("All","All","All","All");
}

void
nmfMainWindow::clearOutputData(std::string Algorithm,
                               std::string Minimizer,
                               std::string ObjectiveCriterion,
                               std::string Scaling)
{
    std::string cmd;
    std::string errorMsg;
    QString msg;
    std::vector<std::string> TableNames = {nmfConstantsMSSPM::TableOutputBiomass,
                                           nmfConstantsMSSPM::TableOutputBiomassEnsemble,
                                           nmfConstantsMSSPM::TableOutputInitBiomass,
                                           nmfConstantsMSSPM::TableOutputGrowthRate,
                                           nmfConstantsMSSPM::TableOutputGrowthRateShape,
                                           nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs,
                                           nmfConstantsMSSPM::TableOutputCarryingCapacity,
                                           nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs,
                                           nmfConstantsMSSPM::TableOutputCatchability,
                                           nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs,
                                           nmfConstantsMSSPM::TableOutputCompetitionAlpha,
                                           nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies,
                                           nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds,
                                           nmfConstantsMSSPM::TableOutputPredationRho,
                                           nmfConstantsMSSPM::TableOutputPredationHandling,
                                           nmfConstantsMSSPM::TableOutputPredationExponent,
                                           nmfConstantsMSSPM::TableOutputSurveyQ,
                                           nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs,
                                           nmfConstantsMSSPM::TableOutputMSY,
                                           nmfConstantsMSSPM::TableOutputMSYBiomass,
                                           nmfConstantsMSSPM::TableOutputMSYFishing,
                                           nmfConstantsMSSPM::TableDiagnosticInitBiomass,
                                           nmfConstantsMSSPM::TableDiagnosticGrowthRate,
                                           nmfConstantsMSSPM::TableDiagnosticGrowthRateShape,
                                           nmfConstantsMSSPM::TableDiagnosticCarryingCapacity,
                                           nmfConstantsMSSPM::TableDiagnosticCatchability,
                                           nmfConstantsMSSPM::TableDiagnosticSurveyQ,
                                           nmfConstantsMSSPM::TableDiagnosticSurface,
                                           nmfConstantsMSSPM::TableForecastBiomass,
                                           nmfConstantsMSSPM::TableForecastBiomassMonteCarlo,
                                           nmfConstantsMSSPM::TableForecastHarvestCatch,
                                           nmfConstantsMSSPM::TableForecastHarvestEffort,
                                           nmfConstantsMSSPM::TableForecastHarvestExploitation,
                                           nmfConstantsMSSPM::TableForecastUncertainty,
                                           nmfConstantsMSSPM::TableForecasts};

    QMessageBox::StandardButton reply;
    QString modifier = "selected";

    if ((Algorithm == "All") && (Minimizer == "All") &&
        (ObjectiveCriterion == "All") && (Scaling == "All"))
    {
        modifier = "all";
    }
    msg  = "\nOK to clear " + modifier + " Output tables of Estimated, Diagnostic, and Forecast data?\n";
    reply = QMessageBox::question(this, tr("Clear Output Tables"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No)
        return;

    QProgressDialog progress("\nClearing tables...", "Cancel", 0, TableNames.size(), this);
    progress.setAttribute(Qt::WA_DeleteOnClose, true);
    progress.setModal(true);
    progress.setMinimumDuration(0);
    progress.show();
    progress.update();
    progress.repaint();
//    this->setCursor(Qt::WaitCursor);
    int inc = 0;
    for (std::string tableName : TableNames) {
        progress.setLabelText("\nClearing table: "+QString::fromStdString(tableName));
        progress.setValue(++inc);
        if (progress.wasCanceled()) {
            break;
        }
        cmd = "TRUNCATE TABLE " + tableName;
        if (Algorithm != "All") {
            cmd = "DELETE FROM " + tableName;
            cmd += " WHERE ProjectName = '" + m_ProjectName +
                   "' AND ModelName = '"    + m_ModelName   +
                   "' AND Algorithm = '"    + Algorithm     + "'";
            if (Minimizer != "All") {
                cmd += " AND Minimizer = '" + Minimizer + "'";
                if (ObjectiveCriterion != "All") {
                    cmd += " AND ObjectiveCriterion = '" + ObjectiveCriterion + "'";
                    if (Scaling != "All") {
                        cmd += " AND Scaling = '" + Scaling + "'";
                    }
                }
            }
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfMainWindow: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\nError in clearOutputTables command.  Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
            QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            this->setCursor(Qt::ArrowCursor);
            return;
        }
    }
    progress.close();
//    this->setCursor(Qt::ArrowCursor);
    QMessageBox::information(this, "Success", "\nOutput table(s) cleared successfully.", QMessageBox::Ok);

    // Set to Run Estimation tab
    setPage(nmfConstantsMSSPM::SectionEstimation,
            nmfConstantsMSSPM::PageEstimationRun);

    menu_showAllSavedRuns();

//    showOutputChart(false);

}

void
nmfMainWindow::showOutputChart(bool show)
{
    m_ChartView2d->setVisible(show);
    m_UI->OutputDataTV->setVisible(show);
}

void
nmfMainWindow::clearOutputTables()
{
    for (QTableView *tv : {InitBiomassTV, GrowthRateTV, GrowthRateShapeTV,
                           CarryingCapacityTV, CatchabilityTV,
                           CompetitionAlphaTV, CompetitionBetaSTV,
                           CompetitionBetaGTV, PredationRhoTV,
                           PredationHandlingTV, PredationExponentTV,
                           SurveyQTV, CovariateCoefficientsTV, OutputBiomassTV,
                           BMSYTV, MSYTV, FMSYTV,
                           SummaryTV, DiagnosticSummaryTV, m_UI->OutputDataTV})
    {
        tv->setModel(new QStandardItemModel(0,0));
    }
}


void
nmfMainWindow::loadDatabase()
{
    QString msg = QString::fromStdString("Loading database: "+m_ProjectDatabase);
    m_Logger->logMsg(nmfConstants::Normal,msg.toStdString());
    m_DatabasePtr->nmfSetDatabase(m_ProjectDatabase);
}

bool
nmfMainWindow::loadHarvestCatchTables(
        const bool& isAggProd,
        const bool& isMonteCarlo,
        const std::string& ForecastName,
        const std::string& HarvestFormName,
        nmfHarvestForm* harvestFormPtr,
        std::vector<double>& HarvestRandomValues,
        std::vector<double>& HarvestUncertainty,
        std::map<QString,QString>& previousUnits,
        const int& NumSpeciesOrGuilds,
        const int& RunLength,
        boost::numeric::ublas::matrix<double>& Catch)
{
    boost::numeric::ublas::matrix<double> NullMatrix;
    QString tableForecastHarvestCatch = QString::fromStdString(nmfConstantsMSSPM::TableForecastHarvestCatch);
    QString tableHarvestCatch = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);

    if (isAggProd) {
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      ForecastName,tableHarvestCatch.toStdString(),
                                                      "",NumSpeciesOrGuilds,RunLength,
                                                      "",NullMatrix,Catch)) {
            QMessageBox::warning(this, "Error",
                                 "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                 QMessageBox::Ok);
            return false;
        }
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               ForecastName,tableHarvestCatch.toStdString(),
                                               NumSpeciesOrGuilds,RunLength,Catch)) {
            QMessageBox::warning(this, "Error",
                                 "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                 QMessageBox::Ok);
            return false;
        }
    }
    if (ForecastName == "") {
        if (previousUnits.find(tableHarvestCatch) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(Catch, previousUnits[tableHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
        }
    } else {
        if (previousUnits.find(tableForecastHarvestCatch) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(Catch, previousUnits[tableForecastHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
        }
    }
    scaleTimeSeriesIfMonteCarlo(isMonteCarlo,HarvestUncertainty,Catch,HarvestRandomValues);

    return true;
}

bool
nmfMainWindow::loadHarvestEffortTables(
        const bool& isAggProd,
        const bool& isMonteCarlo,
        const std::string& ForecastName,
        const std::string& HarvestFormName,
        nmfHarvestForm* harvestFormPtr,
        std::vector<double>& HarvestRandomValues,
        std::vector<double>& HarvestUncertainty,
        std::map<QString,QString>& previousUnits,
        const int& NumSpeciesOrGuilds,
        const int& RunLength,
        boost::numeric::ublas::matrix<double>& Effort)
{
    std::string HarvestTable;
    boost::numeric::ublas::matrix<double> NullMatrix;

    if (isAggProd) {
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                                      "",NumSpeciesOrGuilds,RunLength,
                                                      "",NullMatrix,Effort))
            return false;
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                               NumSpeciesOrGuilds,RunLength,Effort))
            return false;
    }
    scaleTimeSeriesIfMonteCarlo(isMonteCarlo,HarvestUncertainty,Effort,HarvestRandomValues);

    return true;
}

bool
nmfMainWindow::loadHarvestEffortFitToCatchTables(
        const bool& isAggProd,
        const bool& isMonteCarlo,
        const std::string& ForecastName,
        const std::string& HarvestFormName,
        nmfHarvestForm* harvestFormPtr,
        std::vector<double>& HarvestRandomValues,
        std::vector<double>& HarvestUncertainty,
        std::map<QString,QString>& previousUnits,
        const int& NumSpeciesOrGuilds,
        const int& RunLength,
        boost::numeric::ublas::matrix<double>& Catch,
        boost::numeric::ublas::matrix<double>& Effort)
{
    std::string HarvestTable;
    boost::numeric::ublas::matrix<double> NullMatrix;
    boost::numeric::ublas::matrix<double> Exploitation;
    QString tableHarvestCatch         = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);
    QString tableHarvestEffort        = QString::fromStdString(nmfConstantsMSSPM::TableHarvestEffort);
//  QString tableForecastHarvestCatch = QString::fromStdString(nmfConstantsMSSPM::TableForecastHarvestCatchFTC);

    // Get Harvest data
    QString ForecastButtonLabel = Forecast_Tab2_ptr->getHarvestType();
    checkModelHarvestTypeMatchesForecastButtonLabel(QString::fromStdString(HarvestFormName),ForecastButtonLabel);

    if (ForecastName.empty()) {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               ForecastName,tableHarvestCatch.toStdString(),
                                               NumSpeciesOrGuilds,RunLength,Catch)) {
            QMessageBox::warning(this, "Error","\nError: No data found in Catch table.",QMessageBox::Ok);
            return false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Loaded Catch matrix from table: "+tableHarvestCatch.toStdString());
        }
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               ForecastName,tableHarvestEffort.toStdString(),
                                               NumSpeciesOrGuilds,RunLength,Effort)) {
            QMessageBox::warning(this, "Error","\nError: No data found in Effort table.",QMessageBox::Ok);
            return false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Loaded Effort matrix from table: "+tableHarvestEffort.toStdString());
        }
    } else { // if it is a Forecast
        if (ForecastButtonLabel == nmfConstantsMSSPM::ForecastHarvestTypeCatch) {
            tableHarvestCatch += "ftc";
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,tableHarvestCatch.toStdString(),
                                                   NumSpeciesOrGuilds,RunLength,Catch)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Catch Fit to Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            } else {
                m_Logger->logMsg(nmfConstants::Normal,"Loaded Catch matrix from table: "+tableHarvestCatch.toStdString());
            }
//          HarvestTable = nmfConstantsMSSPM::TableHarvestCatch;
//        harvestFormPtr->setType(nmfConstantsMSSPM::HarvestCatch.toStdString());
//        if (! loadHarvestCatchTables(isAggProd,isMonteCarlo,ForecastName,HarvestFormName,
//                                     harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
//                                     previousUnits,NumSpeciesOrGuilds,RunLength,Catch)) {
//            return false;
//        }
//        m_Logger->logMsg(nmfConstants::Normal,"Loaded Catch as Forecast Harvest data for Effort Fit to Catch model");
//        if (ForecastName == "") {
//            if (previousUnits.find(tableHarvestCatch) != previousUnits.end()) {
//                nmfUtilsQt::convertMatrix(Catch, previousUnits[tableHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
//            }
//        } else {
//            if (previousUnits.find(tableForecastHarvestCatch) != previousUnits.end()) {
//                nmfUtilsQt::convertMatrix(Catch, previousUnits[tableForecastHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
//            }
//        }
            scaleTimeSeriesIfMonteCarlo(isMonteCarlo,HarvestUncertainty,Catch,HarvestRandomValues);
        } else if (ForecastButtonLabel == nmfConstantsMSSPM::ForecastHarvestTypeEffort) {
            tableHarvestEffort += "ftc";
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,tableHarvestEffort.toStdString(),
                                                   NumSpeciesOrGuilds,RunLength,Effort)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Effort Fit to Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            }
//        HarvestTable = nmfConstantsMSSPM::TableHarvestEffort;
//        harvestFormPtr->setType(nmfConstantsMSSPM::HarvestEffort.toStdString());
//        if (! loadHarvestEffortTables(isAggProd,isMonteCarlo,ForecastName,
//                                      HarvestFormName,harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
//                                      previousUnits,NumSpeciesOrGuilds,RunLength,Effort)) {
//            return false;
//        }
//        m_Logger->logMsg(nmfConstants::Normal,"Loaded Effort as Forecast Harvest data for Effort Fit to Catch model");
//        if (isAggProd) {
//            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
//                                                          ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
//                                                          NumSpeciesOrGuilds,RunLength,
//                                                          "",NullMatrix,Effort))
//                return false;
//        } else {
//            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
//                                                   ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
//                                                   NumSpeciesOrGuilds,RunLength,Effort))
//                return false;
//        }
            scaleTimeSeriesIfMonteCarlo(isMonteCarlo,HarvestUncertainty,Effort,HarvestRandomValues);
        } else {
            m_Logger->logMsg(nmfConstants::Error,"Unknown ForecastHarvest Button Label");
        }
    }

    return true;
}

bool
nmfMainWindow::loadHarvestExploitationTables(
        const bool& isAggProd,
        const bool& isMonteCarlo,
        const std::string& ForecastName,
        const std::string& HarvestFormName,
        nmfHarvestForm* harvestFormPtr,
        std::vector<double>& HarvestRandomValues,
        std::vector<double>& HarvestUncertainty,
        std::map<QString,QString>& previousUnits,
        const int& NumSpeciesOrGuilds,
        const int& RunLength,
        boost::numeric::ublas::matrix<double>& Exploitation)
{
    boost::numeric::ublas::matrix<double> NullMatrix;

    if (isAggProd) {
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      ForecastName,nmfConstantsMSSPM::TableHarvestExploitation,
                                                      "",NumSpeciesOrGuilds,RunLength,
                                                      "",NullMatrix,Exploitation))
            return false;
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               ForecastName,nmfConstantsMSSPM::TableHarvestExploitation,
                                               NumSpeciesOrGuilds,RunLength,Exploitation))
            return false;
    }
    scaleTimeSeriesIfMonteCarlo(isMonteCarlo,HarvestUncertainty,Exploitation,HarvestRandomValues);

    return true;
}

void
nmfMainWindow::callback_ToDoAfterModelDelete()
{
    enableApplicationFeatures("AllOtherGroups",false);
}

void
nmfMainWindow::callback_ReloadWidgets(bool reloadProject)
{
    m_LoadLastProject = reloadProject;
    loadGuis();
}

void
nmfMainWindow::callback_ReloadSetupWidgets()
{
    loadSetupWidgets();
}

void
nmfMainWindow::loadSetupWidgets()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Setup_Tab2_ptr->loadWidgets();
    Setup_Tab3_ptr->loadWidgets();
    Setup_Tab4_ptr->loadWidgets();

    // Necessary to reset the parameter estimation checkboxes
    Estimation_Tab7_ptr->loadWidgets();

    QApplication::restoreOverrideCursor();
}

void
nmfMainWindow::loadAllWidgets(bool loadProjectData)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (loadProjectData) {
        Setup_Tab2_ptr->loadWidgets();
    }
    Setup_Tab3_ptr->loadWidgets();
    Setup_Tab4_ptr->loadWidgets();

    Estimation_Tab1_ptr->loadWidgets();
    Estimation_Tab2_ptr->loadWidgets();
    Estimation_Tab3_ptr->loadWidgets();
    Estimation_Tab4_ptr->loadWidgets();
    Estimation_Tab5_ptr->loadWidgets();
    Estimation_Tab6_ptr->loadWidgets();
    Estimation_Tab7_ptr->loadWidgets();
    Estimation_Tab8_ptr->loadWidgets();

    Diagnostic_Tab1_ptr->loadWidgets();
    Diagnostic_Tab2_ptr->loadWidgets();

    Forecast_Tab1_ptr->loadWidgets();
    Forecast_Tab2_ptr->loadWidgets();
    Forecast_Tab3_ptr->loadWidgets();

    Forecast_Tab4_ptr->loadWidgets();
    Output_Controls_ptr->resetMSYWidgets();
    setDatabaseWaitTime();
    refreshOutputTables();

    QApplication::restoreOverrideCursor();
}

void
nmfMainWindow::refreshOutputTables()
{
    int numRows;
    int numCols;
    int numDecimals;
    int colStart;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling;
    QList<QTableView*> outputTablesNoEE =
      {InitBiomassTV, GrowthRateTV, PredationRhoTV,
       PredationHandlingTV, PredationExponentTV,
       CarryingCapacityTV,  CatchabilityTV, BMSYTV,
       MSYTV, FMSYTV, OutputBiomassTV, m_UI->OutputDataTV};
    QList<QTableView*> outputTablesEE          = {GrowthRateShapeTV,SurveyQTV,CovariateCoefficientsTV,
                                                  CompetitionAlphaTV,CompetitionBetaSTV, CompetitionBetaGTV};
    QList<QTableView*> outputTablesDiagnostic  = {SummaryTV, DiagnosticSummaryTV};
    QList<QList<QTableView*> > allOutputTables = {outputTablesNoEE, outputTablesEE, outputTablesDiagnostic};
    QLocale locale(QLocale::English);
    QString valStr;
    QString valueWithComma;
    QString valueWithoutComma;
    QStandardItemModel* smodel;
    QStandardItem* item;

    if ((SummaryTV != nullptr) && (DiagnosticSummaryTV != nullptr)) {
        QStringList EstParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();
        EstParamNames << "Estimated Biomass";
        getCurrentAlgorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
        Output_TableWidgets_ptr->reloadLast(Algorithm,Minimizer,ObjectiveCriterion,Scaling,EstParamNames);
    }

    for (QList<QTableView*> outputTableList : allOutputTables) {
        for (QTableView* outputTable : outputTableList) {

            // Encoding whether to show EE notation into the numDecimals variable
            numDecimals = (outputTablesEE.indexOf(outputTable) >= 0) ? -6 : 6;
            colStart    = (outputTablesDiagnostic.indexOf(outputTable) >= 0) ? 1 : 0;
            if (outputTable != nullptr) {
                smodel = qobject_cast<QStandardItemModel*>(outputTable->model());
                if (smodel) {
                    numRows = smodel->rowCount();
                    numCols = smodel->columnCount();
                    if ((numRows > 0) && (numCols > 0)) {
                        for (int row=0; row<numRows; ++row) {
                            for (int col=colStart; col<numCols; ++col) {
                                valStr = smodel->index(row,col).data().toString();
                                if ((std::fabs(valStr.toDouble()) < 0.001) && (outputTable == DiagnosticSummaryTV)) {
                                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                                valStr.toDouble(),m_NumSignificantDigits,-3);
                                } else if (valStr.contains('e') || valStr.contains('E')) {
                                    valueWithComma = (valStr.toDouble() != 0) ? valStr : "0";
                                } else {
                                    valueWithoutComma = valStr.remove(",");
                                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                            valueWithoutComma.toDouble(),m_NumSignificantDigits,numDecimals);
                                }

                                item = new QStandardItem(valueWithComma);
                                item->setTextAlignment(Qt::AlignCenter);
                                smodel->setItem(row, col, item);
                            }
                        }
                        outputTable->setModel(smodel);
                        outputTable->resizeColumnsToContents();
//if (outputTable->objectName() == "PredationRhoTV") {
//  qDebug() << "rho: (0,0..2),(1..2,0): "
//    << smodel->item(0,0)->data().toString()
//    << smodel->item(0,1)->data().toString()
//    << smodel->item(0,2)->data().toString()
//    << smodel->item(1,0)->data().toString()
//    << smodel->item(2,0)->data().toString();
//}
                    }
                }

            }
        }
    }

}

void
nmfMainWindow::loadGuis(bool loadProjectData)
{
    if (m_LoadLastProject) {
        QString filename = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(m_ProjectName));

        // RSK - Bug here....this causes loadGuis to get called an extra time!
        if (! Setup_Tab2_ptr->loadProject(m_Logger,filename,nmfConstantsMSSPM::DontClearModelName)) {
            enableApplicationFeatures("SetupGroup",false);
            enableApplicationFeatures("AllOtherGroups",false);
        }
    }

    loadAllWidgets(loadProjectData);

    adjustProgressWidget();
    Output_Controls_ptr->loadSpeciesControlWidget();
    Output_TableWidgets_ptr->loadWidgets();

    // Reset settings variables in GUIs
    Setup_Tab4_ptr->setFontSize(m_SetupFontSize);
    Forecast_Tab4_ptr->setFontSize(m_ForecastFontSize);
    Diagnostic_Tab1_ptr->setVariation(m_DiagnosticsVariation);
    Diagnostic_Tab1_ptr->setNumPoints(m_DiagnosticsNumPoints);
    Diagnostic_Tab1_ptr->callback_UpdateDiagnosticParameterChoices();
    Output_Controls_ptr->callback_UpdateDiagnosticParameterChoices();
}

void
nmfMainWindow::adjustProgressWidget()
{
    int MaxNumGenerations;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"GAGenerations"};
    queryStr = "SELECT GAGenerations FROM " +
                nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    MaxNumGenerations = dataMap["GAGenerations"].size();
    if (MaxNumGenerations == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] nmfMainWindow::adjustProgressWidget: No records found in Models for ModelName = "+m_ModelName);
        return;
    }
    MaxNumGenerations = std::stoi(dataMap["GAGenerations"][0]);

    m_ProgressWidget->setMaxNumGenerations(MaxNumGenerations);
}

void
nmfMainWindow::initConnections()
{
    // Action connections
    connect(m_UI->actionRunTB,              SIGNAL(triggered()),
            this,                           SLOT(menu_runModel()));
    connect(m_UI->actionSaveCurrentRunTB,   SIGNAL(triggered()),
            this,                           SLOT(menu_saveCurrentRun()));
    connect(m_UI->actionShowAllSavedRunsTB, SIGNAL(triggered()),
            this,                           SLOT(menu_showAllSavedRuns()));
    connect(m_UI->actionShowCurrentRunTB,   SIGNAL(triggered()),
            this,                           SLOT(menu_showCurrentRun()));
    connect(m_UI->actionBeesTB,             SIGNAL(triggered(bool)),
            this,                           SLOT(menu_setBees(bool)));
    connect(m_UI->actionNLoptTB,            SIGNAL(triggered(bool)),
            this,                           SLOT(menu_setNLopt(bool)));
    connect(m_UI->actionWhatsThisTB,        SIGNAL(triggered()),
            this,                           SLOT(menu_whatsThis()));
    connect(m_UI->actionSaveSettings,       SIGNAL(triggered()),
            this,                           SLOT(menu_saveSettings()));
    connect(m_UI->actionSaveCurrentRun,     SIGNAL(triggered()),
            this,                           SLOT(menu_saveCurrentRun()));
    connect(m_UI->actionShowAllSavedRuns,   SIGNAL(triggered()),
            this,                           SLOT(menu_showAllSavedRuns()));
    connect(m_UI->actionShowCurrentRun,     SIGNAL(triggered()),
            this,                           SLOT(menu_showCurrentRun()));
    connect(m_UI->actionSaveViewTB,         SIGNAL(triggered()),
            this,                           SLOT(menu_saveAndShowCurrentRun()));
    connect(m_UI->actionShowTableNames,     SIGNAL(triggered()),
            this,                           SLOT(menu_showTableNames()));
    connect(m_UI->actionQuit,               SIGNAL(triggered()),
            this,                           SLOT(menu_quit()));
    connect(m_UI->actionClear,              SIGNAL(triggered()),
            this,                           SLOT(menu_clear()));
    connect(m_UI->actionCopy,               SIGNAL(triggered()),
            this,                           SLOT(menu_copy()));
    connect(m_UI->actionPaste,              SIGNAL(triggered()),
            this,                           SLOT(menu_paste()));
    connect(m_UI->actionSelectAll,          SIGNAL(triggered()),
            this,                           SLOT(menu_selectAll()));
    connect(m_UI->actionDeselectAll,        SIGNAL(triggered()),
            this,                           SLOT(menu_deselectAll()));
    connect(m_UI->actionPasteAll,           SIGNAL(triggered()),
            this,                           SLOT(menu_pasteAll()));
    connect(m_UI->actionClearOutputData,    SIGNAL(triggered()),
            this,                           SLOT(menu_clearOutputData()));
    connect(m_UI->actionClearSpecificOutputData,       SIGNAL(triggered()),
            this,                                      SLOT(menu_clearSpecificOutputData()));

//  connect(ui->actionGenerateLinearObservedBiomass,   SIGNAL(triggered()),
//          this,                                      SLOT(menu_generateLinearObservedBiomass()));
//  connect(ui->actionGenerateLogisticObservedBiomass, SIGNAL(triggered()),
//          this,                                      SLOT(menu_generateLogisticObservedBiomass()));
//  connect(ui->actionGenerateLogisticMultiSpeciesObservedBiomass, SIGNAL(triggered()),
//          this,                                                  SLOT(menu_generateLogisticMultiSpeciesObservedBiomass()));
//  connect(m_UI->actionSetEstimatedGrowthRateToTestValue,       SIGNAL(triggered()),
//          this,                                                SLOT(menu_resetGrowthRate()));
//  connect(m_UI->actionSetEstimatedCarryingCapacityToTestValue, SIGNAL(triggered()),
//          this,                                                SLOT(menu_resetCarryingCapacity()));
//  connect(m_UI->actionSetEstmatedCompetitionToTestValues,      SIGNAL(triggered()),
//          this,                                                SLOT(menu_resetCompetition()));
//  connect(m_UI->actionClearEstimatedCompetition,               SIGNAL(triggered()),
//          this,                                                SLOT(menu_clearCompetition()));
    connect(m_UI->actionStopRun,                                 SIGNAL(triggered()),
            this,                                                SLOT(menu_stopRun()));
    connect(m_UI->actionCreateTables,                            SIGNAL(triggered()),
            this,                                                SLOT(menu_createTables()));
    connect(m_UI->actionLayoutOutput,                            SIGNAL(triggered()),
            this,                                                SLOT(menu_layoutOutput()));
    connect(m_UI->actionLayoutDefault,                           SIGNAL(triggered()),
            this,                                                SLOT(menu_layoutDefault()));
    connect(m_UI->actionWhatsThis,                               SIGNAL(triggered()),
            this,                                                SLOT(menu_whatsThis()));
    connect(m_UI->actionAbout,                                   SIGNAL(triggered()),
            this,                                                SLOT(menu_about()));
    connect(m_UI->actionResetCursor,                             SIGNAL(triggered()),
            this,                                                SLOT(menu_resetCursor()));
    connect(m_UI->actionPreferences,                             SIGNAL(triggered()),
            this,                                                SLOT(menu_preferences()));
    connect(m_UI->actionScreenShot,                              SIGNAL(triggered()),
            this,                                                SLOT(menu_screenShot()));
    connect(m_UI->actionScreenShotAll,                           SIGNAL(triggered()),
            this,                                                SLOT(menu_screenShotAll()));
    connect(m_UI->actionMultiShot,                               SIGNAL(triggered()),
            this,                                                SLOT(menu_screenMultiShot()));
    connect(m_UI->actionImportDatabase,                          SIGNAL(triggered()),
            this,                                                SLOT(menu_importDatabase()));
    connect(m_UI->actionExportHPCFiles,                          SIGNAL(triggered()),
            this,                                                SLOT(menu_exportHPCFiles()));
    connect(m_UI->actionImportHPCFiles,                          SIGNAL(triggered()),
            this,                                                SLOT(menu_importHPCFiles()));
    connect(m_UI->actionExportDatabase,                          SIGNAL(triggered()),
            this,                                                SLOT(menu_exportDatabase()));
    connect(m_UI->actionExportAllDatabases,                      SIGNAL(triggered()),
            this,                                                SLOT(menu_exportAllDatabases()));
    connect(m_UI->actionToggleManagerMode,                       SIGNAL(triggered()),
            this,                                                SLOT(menu_toggleManagerMode()));
    connect(m_UI->actionToggleManagerModeViewer,                 SIGNAL(triggered()),
            this,                                                SLOT(menu_toggleManagerModeViewer()));
    connect(m_UI->actionOpenCSVFile,                             SIGNAL(triggered()),
            this,                                                SLOT(menu_openCSVFile()));
    connect(m_UI->actionCreateSimulatedBiomass,                  SIGNAL(triggered()),
            this,                                                SLOT(menu_createSimulatedBiomass()));
    connect(m_UI->actionToggleSignificantDigits,                 SIGNAL(triggered()),
            this,                                                SLOT(menu_toggleSignificantDigits()));
    connect(m_UI->actionTroubleshooting,                         SIGNAL(triggered()),
            this,                                                SLOT(menu_troubleshooting()));
    connect(m_UI->actionOptimizeOutputTables,                    SIGNAL(triggered()),
            this,                                                SLOT(menu_optimizeOutputTables()));
    connect(m_UI->actionOptimizeAllTables,                       SIGNAL(triggered()),
            this,                                                SLOT(menu_optimizeAllTables()));
    connect(m_UI->actionLoadLayout,                              SIGNAL(triggered()),
            this,                                                SLOT(menu_loadLayout()));
    connect(m_UI->actionSaveLayout,                              SIGNAL(triggered()),
            this,                                                SLOT(menu_saveLayout()));
    connect(m_UI->actionDeleteLayout,                            SIGNAL(triggered()),
            this,                                                SLOT(menu_deleteLayout()));
    connect(m_UI->actionShowLastMultiOrHPCRun,                   SIGNAL(triggered()),
            this,                                                SLOT(menu_showLastMultiOrHPCRun()));
    connect(m_UI->actionShowLastSingleRun,                       SIGNAL(triggered()),
            this,                                                SLOT(menu_showLastSingleRun()));
    connect(m_UI->actionToggleSingleMulti,                       SIGNAL(triggered()),
            this,                                                SLOT(menu_toggleSingleMulti()));

    // Widget connections
    connect(NavigatorTree,   SIGNAL(itemSelectionChanged()),
            this,            SLOT(callback_NavigatorSelectionChanged()));
    connect(Setup_Tab2_ptr,  SIGNAL(SaveMainSettings()),
            this,            SLOT(callback_SaveMainSettings()));
    connect(Setup_Tab2_ptr,  SIGNAL(ClearEstimationTables()),
            this,            SLOT(callback_ClearEstimationTables()));
    connect(Setup_Tab2_ptr,  SIGNAL(ProjectSaved()),
            this,            SLOT(callback_ProjectSaved()));
    connect(Setup_Tab2_ptr,  SIGNAL(AddedNewDatabase()),
            this,            SLOT(callback_AddedNewDatabase()));
    connect(Setup_Tab2_ptr,  SIGNAL(ImportDatabase()),
            this,            SLOT(callback_ImportDatabase()));
    connect(Setup_Tab2_ptr,  SIGNAL(DisableNavigatorUntilModelLoad()),
            this,            SLOT(callback_ToDoAfterModelDelete()));
    connect(Setup_Tab3_ptr,  SIGNAL(ReloadWidgets(bool)),
            this,            SLOT(callback_ReloadWidgets(bool)));
    connect(Estimation_Tab1_ptr, SIGNAL(ReloadSetupWidgets()),
            this,                SLOT(callback_ReloadSetupWidgets()));
    connect(Setup_Tab3_ptr,      SIGNAL(SaveSpeciesSupplemental(QString,QList<QString>,QList<QString>,QList<QString>,QList<QString>,QList<QString>,QList<QString>)),
            Estimation_Tab1_ptr, SLOT(callback_QueryAndSaveSpeciesCSVFile(QString,QList<QString>,QList<QString>,QList<QString>,QList<QString>,QList<QString>,QList<QString>)));
    connect(Setup_Tab3_ptr,      SIGNAL(SaveGuildSupplemental(QList<QString>,QList<QString>,QList<QString>)),
            Estimation_Tab1_ptr, SLOT(callback_SaveGuildsCSVFile(QList<QString>,QList<QString>,QList<QString>)));
    connect(Setup_Tab4_ptr,  SIGNAL(SaveMainSettings()),
            this,            SLOT(callback_SaveMainSettings()));
    connect(Setup_Tab4_ptr,      SIGNAL(SetEstimateRunCheckboxes(std::vector<nmfStructsQt::EstimateRunBox>)),
            Estimation_Tab7_ptr, SLOT(callback_SetEstimateRunCheckboxes(std::vector<nmfStructsQt::EstimateRunBox>)));
    connect(Setup_Tab4_ptr,      SIGNAL(SetEstimateRunCheckboxes(std::vector<nmfStructsQt::EstimateRunBox>)),
            Estimation_Tab6_ptr, SLOT(callback_SetEstimateRunCheckboxes(std::vector<nmfStructsQt::EstimateRunBox>)));
    connect(Setup_Tab4_ptr->getModelPresetsCMB(),    SIGNAL(currentTextChanged(QString)),
            this,                                    SLOT(callback_Setup_Tab4_ModelPresetsCMB(QString)));
    connect(Setup_Tab4_ptr->getGrowthFormCMB(),      SIGNAL(currentTextChanged(QString)),
            this,                                    SLOT(callback_Setup_Tab4_GrowthFormCMB(QString)));
    connect(Setup_Tab4_ptr->getPredationFormCMB(),   SIGNAL(currentTextChanged(QString)),
            this,                                    SLOT(callback_Setup_Tab4_PredationFormCMB(QString)));
    connect(Setup_Tab4_ptr->getHarvestFormCMB(),     SIGNAL(currentTextChanged(QString)),
            this,                                    SLOT(callback_Setup_Tab4_HarvestFormCMB(QString)));
    connect(Setup_Tab4_ptr->getCompetitionFormCMB(), SIGNAL(currentTextChanged(QString)),
            this,                                    SLOT(callback_Setup_Tab4_CompetitionFormCMB(QString)));
    connect(Setup_Tab4_ptr,      SIGNAL(UpdateDiagnosticParameterChoices()),
            Diagnostic_Tab1_ptr, SLOT(callback_UpdateDiagnosticParameterChoices()));
    connect(Setup_Tab4_ptr,      SIGNAL(UpdateDiagnosticParameterChoices()),
            Output_Controls_ptr, SLOT(callback_UpdateDiagnosticParameterChoices()));

    connect(Setup_Tab4_ptr,      SIGNAL(RedrawEquation()),
            this,                SLOT(callback_UpdateModelEquationSummary()));
    connect(Setup_Tab4_ptr,      SIGNAL(ModelLoaded()),
            this,                SLOT(callback_ModelLoaded()));
    connect(Setup_Tab4_ptr,      SIGNAL(CompetitionFormChanged(QString)),
            Estimation_Tab3_ptr, SLOT(callback_CompetitionFormChanged(QString)));
    connect(Setup_Tab4_ptr,      SIGNAL(PredationFormChanged(QString)),
            Estimation_Tab4_ptr, SLOT(callback_PredationFormChanged(QString)));
    connect(Setup_Tab4_ptr,      SIGNAL(ReloadWidgets(bool)),
            this,                SLOT(callback_ReloadWidgets(bool)));
    connect(Setup_Tab4_ptr,      SIGNAL(SaveEstimationRunSettings()),
            Estimation_Tab7_ptr, SLOT(callback_SaveSettings()));
    connect(Setup_Tab4_ptr,      SIGNAL(ModelSaved()),
            this,                SLOT(callback_ModelSaved()));
    connect(Setup_Tab4_ptr,      SIGNAL(ModelDeleted()),
            this,                SLOT(callback_ToDoAfterModelDelete()));
    connect(Setup_Tab4_ptr,      SIGNAL(UpdateInitialObservedBiomass(QString)),
            Estimation_Tab5_ptr, SLOT(callback_UpdateInitialObservedBiomass(QString)));
    connect(Setup_Tab4_ptr,      SIGNAL(UpdateInitialForecastYear()),
            Forecast_Tab1_ptr,   SLOT(callback_UpdateForecastYears()));
    connect(Setup_Tab4_ptr,      SIGNAL(ObservedBiomassType(QString)),
            Estimation_Tab5_ptr, SLOT(callback_ObservedBiomassType(QString)));
    connect(Setup_Tab4_ptr,      SIGNAL(ReloadTab7Widgets()),
            Estimation_Tab7_ptr, SLOT(callback_ReloadWidgets()));
    connect(Setup_Tab4_ptr,      SIGNAL(UpdateWindowTitle()),
            this,                SLOT(callback_UpdateWindowTitle()));

    connect(Estimation_Tab1_ptr, SIGNAL(StoreOutputSpecies()),
            this,                SLOT(callback_StoreOutputSpecies()));
    connect(Estimation_Tab1_ptr, SIGNAL(RestoreOutputSpecies()),
            this,                SLOT(callback_RestoreOutputSpecies()));
    connect(Estimation_Tab1_ptr, SIGNAL(LoadSpeciesGuild()),
            this,                SLOT(callback_LoadSpeciesGuild()));
    connect(Estimation_Tab1_ptr, SIGNAL(UpdateSpeciesSetupData(QList<QString>, QList<QString>, QList<QString>, QList<QString>, QList<QString>, QList<QString>)),
            Setup_Tab3_ptr,      SLOT(callback_UpdateSpeciesTable(QList<QString>, QList<QString>, QList<QString>, QList<QString>, QList<QString>, QList<QString>)));
    connect(Estimation_Tab1_ptr, SIGNAL(UpdateGuildSetupData(QList<QString>, QList<QString>, QList<QString>)),
            Setup_Tab3_ptr,      SLOT(callback_UpdateGuildTable(QList<QString>, QList<QString>, QList<QString>)));
    connect(Setup_Tab3_ptr,      SIGNAL(LoadSpeciesSupplemental(bool,bool,QString)),
            Estimation_Tab1_ptr, SLOT(callback_ImportSpecies(bool,bool,QString)));
    connect(Setup_Tab3_ptr,      SIGNAL(LoadGuildSupplemental(bool,QString)),
            Estimation_Tab1_ptr, SLOT(callback_ImportGuild(bool,QString)));
    connect(Setup_Tab3_ptr,      SIGNAL(LoadEstimation()),
            Estimation_Tab1_ptr, SLOT(callback_LoadPBNoEmit()));
    connect(Estimation_Tab1_ptr, SIGNAL(LoadSetup()),
            Setup_Tab3_ptr,      SLOT(callback_LoadPBNoEmit()));
    connect(Estimation_Tab1_ptr, SIGNAL(RunEstimation(bool)),
            this,                SLOT(callback_RunEstimation(bool)));
    connect(Estimation_Tab1_ptr, SIGNAL(ShowDiagnostics()),
            this,                SLOT(callback_ShowDiagnostics()));
    connect(Estimation_Tab1_ptr, SIGNAL(RunDiagnostics()),
            Diagnostic_Tab1_ptr, SLOT(callback_RunPB()));
    connect(Estimation_Tab5_ptr, SIGNAL(ReloadSpecies(bool)),
            Setup_Tab3_ptr,      SLOT(callback_ReloadSpeciesPB(bool)));
    connect(Estimation_Tab1_ptr, SIGNAL(ReloadSpecies(bool)),
            Setup_Tab3_ptr,      SLOT(callback_ReloadSpeciesPB(bool)));
    connect(Estimation_Tab1_ptr, SIGNAL(ReloadGuilds(bool)),
            Setup_Tab3_ptr,      SLOT(callback_ReloadGuildsPB(bool)));
    connect(Estimation_Tab7_ptr, SIGNAL(ShowRunMessage(QString)),
            this,                SLOT(callback_ShowRunMessage(QString)));
    connect(Estimation_Tab7_ptr, SIGNAL(UpdateForecastYears()),
            Forecast_Tab1_ptr,   SLOT(callback_UpdateForecastYears()));
    connect(Estimation_Tab7_ptr, SIGNAL(UpdateForecastYears()),
            Estimation_Tab2_ptr, SLOT(callback_LoadWidgets()));
    connect(Estimation_Tab7_ptr, SIGNAL(UpdateForecastYears()),
            Estimation_Tab5_ptr, SLOT(callback_LoadWidgets()));
    connect(Estimation_Tab7_ptr, SIGNAL(CheckAllEstimationTablesAndRun()),
            this,                SLOT(callback_CheckAllEstimationTablesAndRun()));
    connect(Estimation_Tab1_ptr, SIGNAL(CheckAllEstimationTablesAndRun()),
            this,                SLOT(callback_CheckAllEstimationTablesAndRun()));
    connect(Estimation_Tab7_ptr, SIGNAL(AddToReview()),
            this,                SLOT(callback_AddToReview()));
    connect(Estimation_Tab7_ptr, SIGNAL(EnableRunButtons(bool)),
            this,                SLOT(callback_EnableRunButtons(bool)));
    connect(Estimation_Tab7_ptr, SIGNAL(SetEstimatedParameterNames()),
            this,                SLOT(callback_SetEstimatedParameterNames()));
    connect(Estimation_Tab7_ptr, SIGNAL(LoadEstimatedParameters()),
            this,                SLOT(callback_LoadEstimatedParameters()));
    connect(Estimation_Tab7_ptr, SIGNAL(DisableMohnsRhoRun()),
            this,                SLOT(callback_DisableMohnsRhoRun()));
    connect(Estimation_Tab7_ptr, SIGNAL(SetRetrospectiveAnalysis(bool)),
            this,                SLOT(callback_SetRetrospectiveAnalysis(bool)));
    connect(Estimation_Tab8_ptr, SIGNAL(LoadFromModelReview(nmfStructsQt::ModelReviewStruct)),
            this,                SLOT(callback_LoadFromModelReview(nmfStructsQt::ModelReviewStruct)));
    connect(Estimation_Tab6_ptr, SIGNAL(ReloadDiagnosticWidgets()),
            Diagnostic_Tab1_ptr, SLOT(callback_UpdateDiagnosticParameterChoices()));
    connect(Estimation_Tab6_ptr, SIGNAL(ReloadDiagnosticWidgets()),
            Output_Controls_ptr, SLOT(callback_UpdateDiagnosticParameterChoices()));

    connect(Forecast_Tab1_ptr,   SIGNAL(ForecastLoaded(std::string)),
            this,                SLOT(callback_ForecastLoaded(std::string)));
    connect(Forecast_Tab1_ptr,   SIGNAL(ResetOutputWidgetsForAggProd()),
            Output_Controls_ptr, SLOT(callback_ResetOutputWidgetsForAggProd()));
    connect(Forecast_Tab2_ptr,   SIGNAL(GetEstimationHarvestModel()),
            this,                SLOT(callback_SetForecastHarvestModel()));
    connect(Forecast_Tab2_ptr,   SIGNAL(UpdateUncertaintyTable()),
            this,                SLOT(callback_UpdateUncertaintyTable()));
    connect(Forecast_Tab2_ptr,   SIGNAL(RunForecast(std::string,bool)),
            this,                SLOT(callback_RunForecast(std::string,bool)));
    connect(Forecast_Tab3_ptr,   SIGNAL(RunForecast(std::string,bool)),
            this,                SLOT(callback_RunForecast(std::string,bool)));
    connect(Forecast_Tab4_ptr,   SIGNAL(RunForecast(std::string,bool)),
            this,                SLOT(callback_RunForecast(std::string,bool)));
    connect(Forecast_Tab4_ptr,   SIGNAL(RefreshOutput()),
            this,                SLOT(callback_RefreshOutput()));
    connect(Forecast_Tab4_ptr,   SIGNAL(SetChartType(QString,std::map<QString,QStringList>)),
            this,                SLOT(callback_OutputTypeCMB(QString,std::map<QString,QStringList>)));
    connect(Forecast_Tab4_ptr,   SIGNAL(UpdateOutputScenarios()),
            Output_Controls_ptr, SLOT(callback_LoadScenariosWidget()));
    connect(Forecast_Tab4_ptr,   SIGNAL(QueryOutputScenario()),
            this,                SLOT(callback_SetOutputScenarioForecast()));
    connect(Forecast_Tab4_ptr,   SIGNAL(SetOutputScenarioText(QString)),
            Output_Controls_ptr, SLOT(callback_SetOutputScenario(QString)));
    connect(Forecast_Tab1_ptr,   SIGNAL(EnableRun(bool)),
            Forecast_Tab4_ptr,   SLOT(callback_EnableRun(bool)));
    connect(Forecast_Tab4_ptr,   SIGNAL(EnableRunButtons(bool)),
            this,                SLOT(callback_EnableRunButtons(bool)));
    connect(Forecast_Tab4_ptr,   SIGNAL(CheckMSYBoxes(bool)),
            Output_Controls_ptr, SLOT(callback_CheckMSYBoxes(bool)));

    connect(Diagnostic_Tab1_ptr, SIGNAL(LoadDataStruct()),
            this,                SLOT(callback_LoadDataStruct()));
    connect(Diagnostic_Tab1_ptr, SIGNAL(ResetOutputWidgetsForAggProd()),
            Output_Controls_ptr, SLOT(callback_ResetOutputWidgetsForAggProd()));
    connect(Diagnostic_Tab1_ptr, SIGNAL(SetChartType(std::string,std::string)),
            this,                SLOT(callback_SetChartType(std::string,std::string)));
    connect(Diagnostic_Tab2_ptr, SIGNAL(RunRetrospectiveAnalysis(std::vector<std::pair<int,int> >)),
            this,                SLOT(callback_RunRetrospectiveAnalysisEstimation(std::vector<std::pair<int,int> >)));
    connect(Diagnostic_Tab1_ptr, SIGNAL(EnableRunButtons(bool)),
            this,                SLOT(callback_EnableRunButtons(bool)));
    connect(Diagnostic_Tab1_ptr, SIGNAL(CheckMSYBoxes(bool)),
            Output_Controls_ptr, SLOT(callback_CheckMSYBoxes(bool)));
    connect(Diagnostic_Tab1_ptr, SIGNAL(SetDiagnosticHPCFlag()),
            this,                SLOT(callback_SetDiagnosticHPCFlag()));
    connect(Diagnostic_Tab2_ptr, SIGNAL(EnableRunButtons(bool)),
            this,                SLOT(callback_EnableRunButtons(bool)));
    connect(Diagnostic_Tab2_ptr, SIGNAL(CheckMSYBoxes(bool)),
            Output_Controls_ptr, SLOT(callback_CheckMSYBoxes(bool)));
    connect(Diagnostic_Tab2_ptr, SIGNAL(AllowConvergedOnly(bool)),
            this,                SLOT(callback_AllowConvergedOnly(bool)));
    connect(Diagnostic_Tab1_ptr, SIGNAL(SetRetrospectiveAnalysis(bool)),
            this,                SLOT(callback_SetRetrospectiveAnalysis(bool)));
    connect(Diagnostic_Tab2_ptr, SIGNAL(SetRetrospectiveAnalysis(bool)),
            this,                SLOT(callback_SetRetrospectiveAnalysis(bool)));

    connect(m_UI->EstimationDataInputTabWidget,  SIGNAL(currentChanged(int)),
            this,                                SLOT(callback_EstimationTabChanged(int)));
    connect(m_UI->SetupInputTabWidget,           SIGNAL(currentChanged(int)),
            this,                                SLOT(callback_SetupTabChanged(int)));
    connect(m_UI->ForecastDataInputTabWidget,    SIGNAL(currentChanged(int)),
            this,                                SLOT(callback_ForecastTabChanged(int)));
    connect(m_UI->DiagnosticsDataInputTabWidget, SIGNAL(currentChanged(int)),
            this,                                SLOT(callback_DiagnosticsTabChanged(int)));

    connect(Output_Controls_ptr, SIGNAL(SetChartView2d(bool)),
            this,                SLOT(callback_SetChartView2d(bool)));
    connect(Output_Controls_ptr, SIGNAL(ResetFilterButtons()),
            this,                SLOT(callback_ResetFilterButtons()));
    connect(Output_Controls_ptr, SIGNAL(EnableFilterButtons(bool)),
            this,                SLOT(callback_EnableFilterButtons(bool)));
    connect(Output_Controls_ptr, SIGNAL(ShowChart(QString,QString)),
            this,                SLOT(callback_ShowChart(QString,QString)));
    connect(Output_Controls_ptr, SIGNAL(ShowChartBy(QString,bool)),
            this,                SLOT(callback_ShowChartBy(QString,bool)));
    connect(Output_Controls_ptr, SIGNAL(ShowChartMultiScenario(QStringList)),
            this,                SLOT(callback_ShowChartMultiScenario(QStringList)));
    connect(Output_Controls_ptr, SIGNAL(ShowDiagnosticsChart3d()),
            this,                SLOT(callback_ShowDiagnosticsChart3d()));
    connect(Output_Controls_ptr, SIGNAL(SelectCenterSurfacePoint()),
            this,                SLOT(callback_SelectCenterSurfacePoint()));
    connect(Output_Controls_ptr, SIGNAL(SelectMinimumSurfacePoint()),
            this,                SLOT(callback_SelectMinimumSurfacePoint()));
    connect(Output_Controls_ptr, SIGNAL(ForecastLineBrightnessChanged(double)),
            this,                SLOT(callback_ForecastLineBrightnessChanged(double)));
    connect(Output_Controls_ptr, SIGNAL(ShowChartMohnsRho()),
            this,                SLOT(callback_ShowChartMohnsRho()));
    connect(this,                SIGNAL(KeyPressed(QKeyEvent*)),
            Remora_ptr,          SLOT(callback_KeyPressed(QKeyEvent*)));
    connect(this,                SIGNAL(MouseMoved(QMouseEvent*)),
            Remora_ptr,          SLOT(callback_MouseMoved(QMouseEvent*)));
    connect(this,                SIGNAL(MouseReleased(QMouseEvent*)),
            Remora_ptr,          SLOT(callback_MouseReleased(QMouseEvent*)));
    connect(Remora_ptr,          SIGNAL(SaveOutputBiomassData(std::string)),
            this,                SLOT(callback_SaveForecastOutputBiomassData(std::string)));
    connect(Remora_ptr,          SIGNAL(UpdateSeedValue(int)),
            this,                SLOT(callback_UpdateSeedValue(int)));




    callback_Setup_Tab4_GrowthFormCMB("Null");
    callback_Setup_Tab4_HarvestFormCMB("Null");
    callback_Setup_Tab4_CompetitionFormCMB("Null");
    callback_Setup_Tab4_PredationFormCMB("Null");

    m_isPressedBeesButton     = m_UI->actionBeesTB->isChecked();
    m_isPressedNLoptButton    = m_UI->actionNLoptTB->isChecked();
    m_isPressedGeneticButton  = m_UI->actionGeneticTB->isChecked();
    m_isPressedGradientButton = m_UI->actionGradientTB->isChecked();
}

void
nmfMainWindow::initPostGuiConnections()
{
    connect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
            this,           SLOT(callback_LoadProject()));
}


bool
nmfMainWindow::eventFilter(QObject *object, QEvent *event)
{
    QList<int>     RowNumList;
    QList<QString> RowNameList;

    RowNumList.clear();
    RowNameList.clear();

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = (QMouseEvent*) event;
        emit MouseMoved(mouseEvent);
    } else if (object == Output_Controls_ptr->getListViewViewport()) {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            QModelIndexList modelList = Output_Controls_ptr->getListViewSelectedIndexes();
            if (modelList.size() > 0) {
                for (int i=0; i<modelList.size(); ++i) {
                    RowNumList.append(modelList[i].row());
                    RowNameList.append(modelList[i].data().toString());
                }
                showChartBcVsTimeSelectedSpecies(RowNumList,RowNameList);
            }
            else {
                showChartBcVsTimeSelectedSpecies(RowNumList,RowNameList);
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = (QMouseEvent*) event;
        emit MouseReleased(mouseEvent);
    }

    return QObject::eventFilter(object, event);
}

void
nmfMainWindow::keyPressEvent(QKeyEvent *event)
{
    emit KeyPressed(event);
}

void
nmfMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    emit MouseMoved(event);
}

QTableView*
nmfMainWindow::findTableInFocus()
{
    QTableView *retv = nullptr;

    if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_SpeciesTW")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_SpeciesTW");
    } else if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_GuildsTW")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_GuildsTW");

    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab1_GuildPopulationTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab1_GuildPopulationTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab1_SpeciesPopulationTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab1_SpeciesPopulationTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_EffortTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_EffortTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_CatchTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_CatchTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_WeightsFTCTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_WeightsFTCTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_EffortFTCTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_EffortFTCTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_CatchFTCTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab2_CatchFTCTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionAlphaMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaSpeciesMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_PredationMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_HandlingMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentMinTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentMinTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentMaxTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab4_ExponentMaxTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab5_AbsoluteBiomassTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab5_AbsoluteBiomassTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab5_RelativeBiomassTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab5_RelativeBiomassTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_CovariateTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_CovariateTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_SpeciesParameterTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_SpeciesParameterTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_InitialValuesTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab6_InitialValuesTV");
    } else if (m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab8_ModelReviewTV")->hasFocus()) {
        return m_UI->EstimationDataInputTabWidget->findChild<QTableView *>("Estimation_Tab8_ModelReviewTV");

    } else if (m_UI->ForecastDataInputTabWidget->findChild<QTableView *>("Forecast_Tab2_HarvestTV")->hasFocus()) {
        return m_UI->ForecastDataInputTabWidget->findChild<QTableView *>("Forecast_Tab2_HarvestTV");
    } else if (m_UI->ForecastDataInputTabWidget->findChild<QTableView *>("Forecast_Tab3_UncertaintyTV")->hasFocus()) {
        return m_UI->ForecastDataInputTabWidget->findChild<QTableView *>("Forecast_Tab3_UncertaintyTV");

    } else if (m_UI->MSSPMOutputTabWidget->findChild<QTableView *>("OutputDataTV")->hasFocus()) {
        return m_UI->MSSPMOutputTabWidget->findChild<QTableView *>("OutputDataTV");
    } else if (InitBiomassTV->hasFocus()) {
        return InitBiomassTV;
    } else if (GrowthRateTV->hasFocus()) {
        return GrowthRateTV;
    } else if (GrowthRateShapeTV->hasFocus()) {
        return GrowthRateShapeTV;
    } else if (CompetitionAlphaTV->hasFocus()) {
        return CompetitionAlphaTV;
    } else if (CompetitionBetaSTV->hasFocus()) {
        return CompetitionBetaSTV;
    } else if (CompetitionBetaGTV->hasFocus()) {
        return CompetitionBetaGTV;
    } else if (PredationRhoTV->hasFocus()) {
        return PredationRhoTV;
    } else if (PredationHandlingTV->hasFocus()) {
        return PredationHandlingTV;
    } else if (PredationExponentTV->hasFocus()) {
        return PredationExponentTV;
    } else if (SurveyQTV->hasFocus()) {
        return SurveyQTV;
    } else if (CarryingCapacityTV->hasFocus()) {
        return CarryingCapacityTV;
    } else if (CatchabilityTV->hasFocus()) {
        return CatchabilityTV;
    } else if (MSYTV->hasFocus()) {
        return MSYTV;
    } else if (BMSYTV->hasFocus()) {
        return BMSYTV;
    }  else if (FMSYTV->hasFocus()) {
        return FMSYTV;
    } else if (SummaryTV->hasFocus()) {
        return SummaryTV;
    } else if (DiagnosticSummaryTV->hasFocus()) {
        return DiagnosticSummaryTV;
    } else if (CovariateCoefficientsTV->hasFocus()) {
        return CovariateCoefficientsTV;
    } else if (OutputBiomassTV->hasFocus()) {
        return OutputBiomassTV;
    }  else if (m_BiomassAbsTV->hasFocus()) {
        return m_BiomassAbsTV;
    }  else if (m_BiomassRelTV->hasFocus()) {
        return m_BiomassRelTV;
    }  else if (m_FishingMortalityTV->hasFocus()) {
        return m_FishingMortalityTV;
    }  else if (m_HarvestScaleFactorTV->hasFocus()) {
        return m_HarvestScaleFactorTV;
    }

    else {
        std::cout << "Error: No table found to cut, copy, or paste." << std::endl;
        return retv;
    }

} // end findTableInFocus


void
nmfMainWindow::menu_quit()
{
    if (QMessageBox::Yes == QMessageBox::question(
                this,
                tr("Quit"),
                tr("\nAre you sure you want to quit?\n")))
    {
        close(); // emits closeEvent
    }
}

void
nmfMainWindow::setNumLines(int numLines)
{
    m_NumLines = numLines;
}

int
nmfMainWindow::getNumLines()
{
    return m_NumLines;
}

void
nmfMainWindow::menu_showAllSavedRuns()
{
    int NumSpecies;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList SpeciesList;

    m_UI->OutputDockWidget->raise();

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;

    // Load Calculated Biomass data (ie, calculated from estimated parameters r and alpha)
    fields     = {"ProjectName","ModelName","isAggProd","SpeName","Value"};
    queryStr   = "SELECT ProjectName,ModelName,isAggProd,SpeName,Value FROM " +
                  nmfConstantsMSSPM::TableOutputGrowthRate +
                  getFilterButtonsResult();
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    setNumLines(NumRecords/NumSpecies);

    if (! callback_ShowChart("","")) { //,nmfConstantsMSSPM::IsNotAveraged)) {
        return;
    }
    clearOutputTables(); // Since the data would be ambiguous since you're looking at more than one plot

}

void
nmfMainWindow::menu_saveSettings()
{
    saveSettings();
    loadAllWidgets();
}


QString
nmfMainWindow::createEstimatedFile(std::string algorithm = "")
{
    int NumSpeciesOrGuilds;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::vector<double> EstInitBiomass;
    std::vector<double> EstGrowthRates;
    std::vector<double> EstGrowthRateShapes;
    std::vector<double> EstCarryingCapacities;
    std::vector<double> EstPredationExponent;
    std::vector<double> EstCatchability;
    std::vector<double> EstSurveyQ;
    std::vector<double> EstGrowthRateCovariateCoeffs;
    std::vector<double> EstCarryingCapacityCovariateCoeffs;
    std::vector<double> EstCatchabilityCovariateCoeffs;
    std::vector<double> EstSurveyQCovariateCoeffs;
    boost::numeric::ublas::matrix<double> EstCompetitionAlpha;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> EstPredationRho;
    boost::numeric::ublas::matrix<double> EstPredationHandling;
    QStringList SpeciesList;
    QString filename;
    QString fullPath;
    bool isAMultiRun = isAMultiOrMohnsRhoRun();

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList)) {
        return "";
    }

    // Derive the new Estimated Parameter CSV file name
    QStringList parts     = QString::fromStdString(nmfConstantsMSSPM::FilenameEstimatedParameters).split(".");
    std::string timestamp = m_Logger->getTimestamp(nmfConstants::TimestampWithUnderscore);
    if (parts.size() == 2) {
        fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
        filename = parts[0]+"_"+QString::fromStdString(timestamp)+"."+parts[1];
        fullPath = QDir(fullPath).filePath(filename);
    } else {
        return "";
    }

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (! algorithm.empty()) {
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputInitBiomass,algorithm,EstInitBiomass);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputGrowthRate,algorithm,EstGrowthRates);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputGrowthRateShape,algorithm,EstGrowthRateShapes);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs,algorithm,EstGrowthRateCovariateCoeffs);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCarryingCapacity,algorithm,EstCarryingCapacities);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs,algorithm,EstCarryingCapacityCovariateCoeffs);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCatchability,algorithm,EstCatchability);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs,algorithm,EstCatchabilityCovariateCoeffs);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputSurveyQ,algorithm,EstSurveyQ);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs,algorithm,EstSurveyQCovariateCoeffs);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputPredationRho,algorithm,EstPredationRho);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputPredationHandling,algorithm,EstPredationHandling);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputPredationExponent,algorithm,EstPredationExponent);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCompetitionAlpha,algorithm,EstCompetitionAlpha);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies,algorithm,EstCompetitionBetaSpecies);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds,algorithm,EstCompetitionBetaGuilds);
        m_DatabasePtr->getOutputData(m_Logger,m_ProjectName,m_ModelName,nmfConstantsMSSPM::TableOutputCompetitionBetaGuildsGuilds,algorithm,EstCompetitionBetaGuildsGuilds);
    } else if ((Algorithm == "NLopt Algorithm") && m_Estimator_NLopt) {
        m_Estimator_NLopt->getEstInitBiomass(EstInitBiomass);
        m_Estimator_NLopt->getEstGrowthRates(EstGrowthRates);
        m_Estimator_NLopt->getEstGrowthRateShape(EstGrowthRateShapes);
        m_Estimator_NLopt->getEstCarryingCapacities(EstCarryingCapacities);
        m_Estimator_NLopt->getEstCatchability(EstCatchability);
        m_Estimator_NLopt->getEstCompetitionAlpha(EstCompetitionAlpha);
        m_Estimator_NLopt->getEstCompetitionBetaSpecies(EstCompetitionBetaSpecies);
        m_Estimator_NLopt->getEstCompetitionBetaGuilds(EstCompetitionBetaGuilds);
        m_Estimator_NLopt->getEstCompetitionBetaGuildsGuilds(EstCompetitionBetaGuildsGuilds);
        m_Estimator_NLopt->getEstPredation(EstPredationRho);
        m_Estimator_NLopt->getEstHandling(EstPredationHandling);
        m_Estimator_NLopt->getEstExponent(EstPredationExponent);
        m_Estimator_NLopt->getEstSurveyQ(EstSurveyQ);
        m_Estimator_NLopt->getEstGrowthRateCovariateCoeffs(EstGrowthRateCovariateCoeffs);
        m_Estimator_NLopt->getEstCarryingCapacityCovariateCoeffs(EstCarryingCapacityCovariateCoeffs);
        m_Estimator_NLopt->getEstCatchabilityCovariateCoeffs(EstCatchabilityCovariateCoeffs);
        m_Estimator_NLopt->getEstSurveyQCovariateCoeffs(EstSurveyQCovariateCoeffs);
    } else if ((Algorithm == "Bees Algorithm") && m_Estimator_Bees) {
        m_Estimator_Bees->getEstInitBiomass(EstInitBiomass);
        m_Estimator_Bees->getEstGrowthRates(EstGrowthRates);
        m_Estimator_Bees->getEstGrowthRateShape(EstGrowthRateShapes);
        m_Estimator_Bees->getEstCarryingCapacities(EstCarryingCapacities);
        m_Estimator_Bees->getEstCatchability(EstCatchability);
        m_Estimator_Bees->getEstCompetitionAlpha(EstCompetitionAlpha);
        m_Estimator_Bees->getEstCompetitionBetaSpecies(EstCompetitionBetaSpecies);
        m_Estimator_Bees->getEstCompetitionBetaGuilds(EstCompetitionBetaGuilds);
        m_Estimator_Bees->getEstCompetitionBetaGuildsGuilds(EstCompetitionBetaGuildsGuilds);
        m_Estimator_Bees->getEstPredation(EstPredationRho);
        m_Estimator_Bees->getEstHandling(EstPredationHandling);
        m_Estimator_Bees->getEstExponent(EstPredationExponent);
        m_Estimator_Bees->getEstSurveyQ(EstSurveyQ);
        m_Estimator_Bees->getEstGrowthRateCovariateCoeffs(EstGrowthRateCovariateCoeffs);
        m_Estimator_Bees->getEstCarryingCapacityCovariateCoeffs(EstCarryingCapacityCovariateCoeffs);
        m_Estimator_Bees->getEstCatchabilityCovariateCoeffs(EstCatchabilityCovariateCoeffs);
        m_Estimator_Bees->getEstSurveyQCovariateCoeffs(EstSurveyQCovariateCoeffs);
    }

    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "Estimated Parameters\n";
        stream << extractVectorData(Estimation_Tab7_ptr->isEstInitialBiomassEnabled(),
                                    Estimation_Tab7_ptr->isEstInitialBiomassChecked(),isAMultiRun,
                                    SpeciesList,"Initial Absolute Biomass:",EstInitBiomass,'f',2);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstGrowthRateEnabled(),
                                    Estimation_Tab7_ptr->isEstGrowthRateChecked(),isAMultiRun,
                                    SpeciesList,"Growth Rate:",EstGrowthRates,'f',3);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstGrowthRateShapeEnabled(),
                                    Estimation_Tab7_ptr->isEstGrowthRateShapeChecked(),isAMultiRun,
                                    SpeciesList,"Growth Rate Shape:",EstGrowthRateShapes,'f',3);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstCarryingCapacityEnabled(),
                                    Estimation_Tab7_ptr->isEstCarryingCapacityChecked(),isAMultiRun,
                                    SpeciesList,"Carrying Capacity:",EstCarryingCapacities,'f',2);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstCatchabilityEnabled(),
                                    Estimation_Tab7_ptr->isEstCatchabilityChecked(),isAMultiRun,
                                    SpeciesList,"Catchability:",EstCatchability,'f',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstCompetitionAlphaEnabled(),
                                    Estimation_Tab7_ptr->isEstCompetitionAlphaChecked(),isAMultiRun,
                                    SpeciesList,"Competition Alpha:",EstCompetitionAlpha,'e',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesEnabled(),
                                    Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesChecked(),isAMultiRun,
                                    SpeciesList,"Competition Beta (Species):",EstCompetitionBetaSpecies,'e',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsEnabled(),
                                    Estimation_Tab7_ptr->isEstCompetitionBetaGuildsChecked(),isAMultiRun,
                                    SpeciesList,"Competition Beta (Guilds):",EstCompetitionBetaGuilds,'e',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsEnabled(),
                                    Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsChecked(),isAMultiRun,
                                    SpeciesList,"Competition Beta (Guilds-Guilds):",EstCompetitionBetaGuildsGuilds,'e',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstPredationRhoEnabled(),
                                    Estimation_Tab7_ptr->isEstPredationRhoChecked(),isAMultiRun,
                                    SpeciesList,"Predation (rho):",EstPredationRho,'e',3);
        stream << extractMatrixData(Estimation_Tab7_ptr->isEstPredationHandlingEnabled(),
                                    Estimation_Tab7_ptr->isEstPredationHandlingChecked(),isAMultiRun,
                                    SpeciesList,"Predation (h):",EstPredationHandling,'f',6);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstPredationExponentEnabled(),
                                    Estimation_Tab7_ptr->isEstPredationExponentChecked(),isAMultiRun,
                                    SpeciesList,"Predation (b):",EstPredationExponent,'f',3);
        stream << extractVectorData(Estimation_Tab7_ptr->isEstSurveyQEnabled(),
                                    Estimation_Tab7_ptr->isEstSurveyQChecked(),isAMultiRun,
                                    SpeciesList,"SurveyQ:",EstSurveyQ,'f',3);
        if (EstGrowthRateCovariateCoeffs.size() > 0) {
            stream << extractVectorData(true,true, // there are no check boxes for env covariates, just assume they're always estimated (they'll be 0 if they weren't)
                                        isAMultiRun,
                                        SpeciesList,"Growth Rate Covariate Coeffs:",EstGrowthRateCovariateCoeffs,'f',3);
        }
        if (EstCarryingCapacityCovariateCoeffs.size() > 0) {
            stream << extractVectorData(true,true,isAMultiRun,
                                        SpeciesList,"Carrying Capacity Covariate Coeffs:",EstCarryingCapacityCovariateCoeffs,'f',3);
        }
        if (EstCatchabilityCovariateCoeffs.size() > 0) {
            stream << extractVectorData(true,true,isAMultiRun,
                                        SpeciesList,"Catchability Covariate Coeffs:",EstCatchabilityCovariateCoeffs,'f',3);
        }
        if (EstSurveyQCovariateCoeffs.size() > 0) {
            stream << extractVectorData(true,true,isAMultiRun,
                                        SpeciesList,"Survey Q Covariate Coeffs:",EstSurveyQCovariateCoeffs,'f',3);
        }

        file.close();
    }

    return filename;
}

QString
nmfMainWindow::extractVectorData(const bool& isEnabled,
                                 const bool& isChecked,
                                 const bool& isAMultiRun,
                                 const QStringList& species,
                                 const QString& label,
                                 const std::vector<double>& vector,
                                 const char& format,
                                 const int& precision)
{
    QString str="";

    if ((!isAMultiRun && isEnabled && isChecked) || (isAMultiRun && isChecked)) {
        str = label + "\n";
        for (int i=0; i<int(vector.size()); ++i) {
            str += species[i] + "," + QString::number(vector[i],format,precision) + "\n";
        }
    }

    return str;
}

QString
nmfMainWindow::extractMatrixData(const bool& isEnabled,
                                 const bool& isChecked,
                                 const bool& isAMultiRun,
                                 const QStringList& species,
                                 const QString& label,
                                 const boost::numeric::ublas::matrix<double>& matrix,
                                 const char& format,
                                 const int& precision)
{
    QString str="";

    if ((!isAMultiRun && isEnabled && isChecked) || (isAMultiRun && isChecked)) {
        str  = label + "\n";
        str += " ";
        for (int i=0; i<int(matrix.size1()); ++i) {
                str += "," + species[i];
        }
        str += "\n";
        for (int i=0; i<int(matrix.size1()); ++i) {
            str += species[i];
            for (int j=0; j<int(matrix.size2()); ++j) {
                str += "," + QString::number(matrix(i,j),format,precision);
            }
            str += "\n";
        }
    }

    return str;
}

void
nmfMainWindow::menu_saveCurrentRun()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::menu_saveCurrentRun start");

    int NumSpecies;
    int StartYear=0;
    int NumGuilds;
    int RunLength;
    int RunNum=0;
    int InitialYear=0;
    bool isMonteCarlo = false;
    bool isBiomassAbsolute;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    QStringList GuildList;
    std::vector<double> EstInitBiomass;
    std::vector<double> EstGrowthRates;
    std::vector<double> EstGrowthRateShape;
    std::vector<double> EstGrowthRateCovariateCoeffs;
    std::vector<double> EstCarryingCapacities;
    std::vector<double> EstCarryingCapacityCovariateCoeffs;
    std::vector<double> EstExponent;
    std::vector<double> EstCatchability;
    std::vector<double> EstCatchabilityCovariateCoeffs;
    std::vector<double> EstSurveyQ;
    std::vector<double> EstSurveyQCovariateCoeffs;
    std::vector<double> EstBMSY;
    std::vector<double> EstMSY;
    std::vector<double> EstFMSY;
    boost::numeric::ublas::matrix<double> EstCompetitionAlpha;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> EstPredation;
    boost::numeric::ublas::matrix<double> EstHandling;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::string OutputInitBiomassTable                     = nmfConstantsMSSPM::TableOutputInitBiomass;
    std::string OutputGrowthRateTable                      = nmfConstantsMSSPM::TableOutputGrowthRate;
    std::string OutputGrowthRateShapeTable                 = nmfConstantsMSSPM::TableOutputGrowthRateShape;
    std::string OutputGrowthRateCovariateCoeffsTable       = nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs;
    std::string OutputCarryingCapacityTable                = nmfConstantsMSSPM::TableOutputCarryingCapacity;
    std::string OutputCarryingCapacityCovariateCoeffsTable = nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs;
    std::string OutputCatchabilityTable                    = nmfConstantsMSSPM::TableOutputCatchability;
    std::string OutputCatchabilityCovariateCoeffsTable     = nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs;
    std::string OutputSurveyQTable                         = nmfConstantsMSSPM::TableOutputSurveyQ;
    std::string OutputSurveyQCovariateCoeffsTable          = nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs;
    std::string OutputBiomassTable                         = nmfConstantsMSSPM::TableOutputBiomass;
    std::string ForecastName = "";
    QStringList SpeciesList;
    std::string isAggProd;

    clearOutputTables();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return;
    }

    //std::cout << "#######: StartYear: " << StartYear << std::endl;
    //std::cout << "#######: RunLength: " << RunLength << std::endl;

    bool isCompetitionAlpha   = (CompetitionForm == "NO_K");
    bool isCompetitionMSPROD  = (CompetitionForm == "MS-PROD");
    bool isCompetitionAGGPROD = (CompetitionForm == "AGG-PROD");
    bool isPredation          = (PredationForm   == "Type I");
    bool isHandling           = (PredationForm   == "Type II") || (PredationForm == "Type III");
    bool isExponent           = (PredationForm   == "Type III");
    isAggProd = (isCompetitionAGGPROD) ? "1" : "0";

    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] menu_saveCurrentRun: No records found in table Guilds, Name = "+m_ModelName);
        return;
    }
    if (isCompetitionAGGPROD) {
       NumSpecies  = NumGuilds;
       SpeciesList = GuildList;
    } else {
        if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] menu_saveCurrentRun: No records found in table Species, Name = "+m_ModelName);
            return;
        }
    }

    // Initialize EstCompetition, EstPredation, EstHandling
    nmfUtils::initialize(EstCompetitionAlpha,            NumSpecies,NumSpecies);
    nmfUtils::initialize(EstCompetitionBetaSpecies,      NumSpecies,NumSpecies);
    nmfUtils::initialize(EstCompetitionBetaGuilds,       NumSpecies,NumGuilds);
    nmfUtils::initialize(EstCompetitionBetaGuildsGuilds, NumGuilds, NumGuilds);
    nmfUtils::initialize(EstPredation,                   NumSpecies,NumSpecies);
    nmfUtils::initialize(EstHandling,                    NumSpecies,NumSpecies);

    if ((Algorithm == "NLopt Algorithm") && m_Estimator_NLopt) {
        m_Estimator_NLopt->getEstInitBiomass(EstInitBiomass);
        m_Estimator_NLopt->getEstGrowthRates(EstGrowthRates);
        m_Estimator_NLopt->getEstGrowthRateShape(EstGrowthRateShape);
        m_Estimator_NLopt->getEstGrowthRateCovariateCoeffs(EstGrowthRateCovariateCoeffs);
        m_Estimator_NLopt->getEstCarryingCapacities(EstCarryingCapacities);
        m_Estimator_NLopt->getEstCarryingCapacityCovariateCoeffs(EstCarryingCapacityCovariateCoeffs);
        m_Estimator_NLopt->getEstCatchability(EstCatchability);
        m_Estimator_NLopt->getEstCatchabilityCovariateCoeffs(EstCatchabilityCovariateCoeffs);
        m_Estimator_NLopt->getEstCompetitionAlpha(EstCompetitionAlpha);
        m_Estimator_NLopt->getEstCompetitionBetaSpecies(EstCompetitionBetaSpecies);
        m_Estimator_NLopt->getEstCompetitionBetaGuilds(EstCompetitionBetaGuilds);
        m_Estimator_NLopt->getEstCompetitionBetaGuildsGuilds(EstCompetitionBetaGuildsGuilds);
        m_Estimator_NLopt->getEstPredation(EstPredation);
        m_Estimator_NLopt->getEstHandling(EstHandling);
        m_Estimator_NLopt->getEstExponent(EstExponent);
        m_Estimator_NLopt->getEstSurveyQ(EstSurveyQ);
        m_Estimator_NLopt->getEstSurveyQCovariateCoeffs(EstSurveyQCovariateCoeffs);
        m_Estimator_NLopt->getEstBMSY(EstBMSY);
        m_Estimator_NLopt->getEstMSY(EstMSY);
        m_Estimator_NLopt->getEstFMSY(EstFMSY);

        updateOutputTables(Algorithm, Minimizer, ObjectiveCriterion,
                           Scaling, isCompetitionAGGPROD,
                           SpeciesList, GuildList,
                           EstInitBiomass,
                           EstGrowthRates,
                           EstGrowthRateShape,
                           EstGrowthRateCovariateCoeffs,
                           EstCarryingCapacities,
                           EstCarryingCapacityCovariateCoeffs,
                           EstCatchability,
                           EstCatchabilityCovariateCoeffs,
                           EstCompetitionAlpha,
                           EstCompetitionBetaSpecies,
                           EstCompetitionBetaGuilds,
                           EstCompetitionBetaGuildsGuilds,
                           EstPredation,
                           EstHandling,
                           EstExponent,
                           EstSurveyQ,
                           EstSurveyQCovariateCoeffs,
                           EstBMSY,
                           EstMSY,
                           EstFMSY);
        clearOutputBiomassTable(ForecastName,Algorithm,Minimizer,
                                ObjectiveCriterion,Scaling,
                                isAggProd,OutputBiomassTable);
        updateOutputBiomassTable(ForecastName,StartYear,RunLength,isMonteCarlo,RunNum,
                                 Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,
                                 GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                 OutputInitBiomassTable,
                                 OutputGrowthRateTable,
                                 OutputGrowthRateShapeTable,
                                 OutputGrowthRateCovariateCoeffsTable,
                                 OutputCarryingCapacityTable,OutputCarryingCapacityCovariateCoeffsTable,
                                 OutputCatchabilityTable,OutputCatchabilityCovariateCoeffsTable,
                                 OutputSurveyQTable,OutputSurveyQCovariateCoeffsTable,
                                 OutputBiomassTable);
        if (m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName)) {
            updateObservedBiomassAndEstSurveyQTable(SpeciesList,RunLength+1,
                                                    EstSurveyQ,EstSurveyQCovariateCoeffs);
        }
    } else if ((Algorithm == "Bees Algorithm") && m_Estimator_Bees) {

        m_Estimator_Bees->getEstInitBiomass(EstInitBiomass);
        m_Estimator_Bees->getEstGrowthRates(EstGrowthRates);
        m_Estimator_Bees->getEstGrowthRateShape(EstGrowthRateShape);
        m_Estimator_Bees->getEstGrowthRateCovariateCoeffs(EstGrowthRateCovariateCoeffs);
        m_Estimator_Bees->getEstCarryingCapacities(EstCarryingCapacities);
        m_Estimator_Bees->getEstCarryingCapacityCovariateCoeffs(EstCarryingCapacityCovariateCoeffs);
        m_Estimator_Bees->getEstCatchability(EstCatchability);
        m_Estimator_Bees->getEstBMSY(EstBMSY);
        m_Estimator_Bees->getEstMSY(EstMSY);
        m_Estimator_Bees->getEstFMSY(EstFMSY);
        m_Estimator_Bees->getEstCatchabilityCovariateCoeffs(EstCatchabilityCovariateCoeffs);
        m_Estimator_Bees->getEstExponent(EstExponent);
        if (isCompetitionAlpha) {
            m_Estimator_Bees->getEstCompetitionAlpha(EstCompetitionAlpha);
        } else if (isCompetitionMSPROD) {
            m_Estimator_Bees->getEstCompetitionBetaSpecies(EstCompetitionBetaSpecies);
            m_Estimator_Bees->getEstCompetitionBetaGuilds(EstCompetitionBetaGuilds);
        } else if (isCompetitionAGGPROD) {
            m_Estimator_Bees->getEstCompetitionBetaGuilds(EstCompetitionBetaGuildsGuilds);
        }
        if (isPredation) {
            m_Estimator_Bees->getEstPredation(EstPredation);
        } else if (isHandling) {
            m_Estimator_Bees->getEstPredation(EstPredation);
            m_Estimator_Bees->getEstHandling(EstHandling);
        }
        if (isExponent) {
            m_Estimator_Bees->getEstExponent(EstExponent);
        }
        m_Estimator_Bees->getEstSurveyQ(EstSurveyQ);
        m_Estimator_Bees->getEstSurveyQCovariateCoeffs(EstSurveyQCovariateCoeffs);

        updateOutputTables(Algorithm,Minimizer,
                           ObjectiveCriterion,Scaling,
                           isCompetitionAGGPROD,
                           SpeciesList, GuildList,
                           EstInitBiomass,
                           EstGrowthRates,
                           EstGrowthRateShape,
                           EstGrowthRateCovariateCoeffs,
                           EstCarryingCapacities,
                           EstCarryingCapacityCovariateCoeffs,
                           EstCatchability,
                           EstCatchabilityCovariateCoeffs,
                           EstCompetitionAlpha,
                           EstCompetitionBetaSpecies,
                           EstCompetitionBetaGuilds,
                           EstCompetitionBetaGuildsGuilds,
                           EstPredation,
                           EstHandling,
                           EstExponent,
                           EstSurveyQ,
                           EstSurveyQCovariateCoeffs,
                           EstBMSY,
                           EstMSY,
                           EstFMSY);
        clearOutputBiomassTable(ForecastName,Algorithm,Minimizer,
                                ObjectiveCriterion,Scaling,
                                isAggProd,OutputBiomassTable);
        updateOutputBiomassTable(ForecastName,StartYear,RunLength,isMonteCarlo,RunNum,
                                 Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,
                                 GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                 OutputInitBiomassTable,
                                 OutputGrowthRateTable,
                                 OutputGrowthRateShapeTable,
                                 OutputGrowthRateCovariateCoeffsTable,
                                 OutputCarryingCapacityTable,OutputCarryingCapacityCovariateCoeffsTable,
                                 OutputCatchabilityTable,OutputCatchabilityCovariateCoeffsTable,
                                 OutputSurveyQTable,OutputSurveyQCovariateCoeffsTable,
                                 OutputBiomassTable);
        if (m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName)) {
            updateObservedBiomassAndEstSurveyQTable(SpeciesList,RunLength+1,
                                                    EstSurveyQ,EstSurveyQCovariateCoeffs);
        }
    }

    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::menu_saveCurrentRun end");
}

void
nmfMainWindow::menu_saveAndShowCurrentRun()
{
    menu_saveAndShowCurrentRun(nmfConstantsMSSPM::DontShowDiagnosticsChart);
}

void
nmfMainWindow::menu_saveAndShowCurrentRun(bool showDiagnosticChart)
{
    menu_saveCurrentRun();
    menu_showCurrentRun();
    if (showDiagnosticChart) {
        Output_Controls_ptr->setOutputType("Diagnostics");
    }
}

bool
nmfMainWindow::isAggProd()
{
    return Setup_Tab4_ptr->isAggProd();
}

void
nmfMainWindow::menu_showCurrentRun()
{
std::cout << "Showing current run..." << std::endl;
    callback_ResetFilterButtons();
    setNumLines(1);

    // This takes into account the user may not be selecting the Species group in the
    // Output controls, but may have selected System or Guild group type
    QString outputGroupType      = Output_Controls_ptr->getOutputGroupType();
    QString outputSpeciesOrGuild = Output_Controls_ptr->getOutputSpecies();
    Output_Controls_ptr->callback_OutputGroupTypeCMB(outputGroupType);
    Output_Controls_ptr->setOutputSpecies(outputSpeciesOrGuild);
    if (outputGroupType == "Species") {
        if (! callback_ShowChart("","")) {
            return;
        }
    } else {
        callback_ShowChartBy(outputGroupType,false);
    }
}

void
nmfMainWindow::menu_showLastSingleRun()
{
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    m_UsingHPCFiles = false;
    m_IsMultiRun    = false;
    callback_ShowChart("","");

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    refreshSummaryModelFit(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
}

void
nmfMainWindow::menu_showLastMultiOrHPCRun()
{
    int NumSpeciesOrGuilds;
    QStringList SpeciesList;

    if (m_HPC_Algorithm.empty()          || m_HPC_Minimizer.empty() ||
        m_HPC_ObjectiveCriterion.empty() || m_HPC_Scaling.empty()) {
        m_Logger->logMsg(nmfConstants::Warning,"Last run wasn't an HPC run");
        return;
    }

    m_UsingHPCFiles = true;
    m_IsMultiRun    = true;
    callback_ShowChart("","");

    refreshOutputTableViews();
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList)) {
        return;
    }
    updateOutputCovariateCoefficientsTable(SpeciesList);
    refreshSummaryModelFit(m_HPC_Algorithm,m_HPC_Minimizer,m_HPC_ObjectiveCriterion,m_HPC_Scaling);
}

void
nmfMainWindow::refreshOutputTableViews()
{
    if (m_HPC_Algorithm.empty() || m_HPC_Minimizer.empty() ||
        m_HPC_ObjectiveCriterion.empty() || m_HPC_Scaling.empty()) {
        return;
    }

    QStandardItemModel* smodel;
    QStandardItem* item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName;
    int NumSpecies;
    int NumRecords;
    double val;
    int eeFactor;
    QString valueWithComma;
    QStringList SpeciesNames = {};
    QStringList Years = {};

    QList<QTableView*> VectorTableViews  =
        {InitBiomassTV,GrowthRateTV,GrowthRateShapeTV,
         CarryingCapacityTV,CatchabilityTV,
         PredationExponentTV,SurveyQTV,BMSYTV,MSYTV,FMSYTV};

    // Populate vector output tables from database
    fields = {"SpeName","Value"};
    for (unsigned i=0; i<nmfConstantsMSSPM::OutputVectorTables.size(); ++i) {
        queryStr = "SELECT SpeName,Value FROM " + nmfConstantsMSSPM::OutputVectorTables[i] +
                   "  WHERE ProjectName = '"      + m_ProjectName +
                   "' AND ModelName = '"          + m_ModelName +
                   "' AND Algorithm = '"          + m_HPC_Algorithm +
                   "' AND Minimizer = '"          + m_HPC_Minimizer +
                   "' AND ObjectiveCriterion = '" + m_HPC_ObjectiveCriterion +
                   "' AND Scaling = '"            + m_HPC_Scaling +
                   "' ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumSpecies = (int)dataMap["SpeName"].size();
        smodel = new QStandardItemModel( NumSpecies, 1 );
        for (int j=0; j<NumSpecies; ++j) {
            if (i == 0) {
                SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
            }
            val = std::stod(dataMap["Value"][j]);
            eeFactor = (nmfConstantsMSSPM::OutputVectorTables[i] == nmfConstantsMSSPM::TableOutputSurveyQ) ? -1 : 1;
            valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                             val,m_NumSignificantDigits,eeFactor*6);
            item = new QStandardItem(valueWithComma);
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(j, 0, item);
        }
        smodel->setHorizontalHeaderLabels({nmfConstantsMSSPM::OutputVectorTableHeaders[i]});
        smodel->setVerticalHeaderLabels(SpeciesNames);
        VectorTableViews[i]->setModel(smodel);
    }

    // Populate matrix tables from database
    QList<QTableView*> MatrixTableViews =
      {OutputBiomassTV, PredationRhoTV, PredationHandlingTV,
       CompetitionAlphaTV, CompetitionBetaSTV, CompetitionBetaGTV};

    int m,NumRows,NumCols;;
    for (unsigned i=0; i<nmfConstantsMSSPM::OutputMatrixTables.size(); ++i) {
        tableName = nmfConstantsMSSPM::OutputMatrixTables[i];
        if (tableName == nmfConstantsMSSPM::TableOutputBiomass) {
            fields = {"SpeName","Value"};
            queryStr = "SELECT SpeName,Value FROM " + tableName +
                       "  WHERE ProjectName = '"      + m_ProjectName +
                       "' AND ModelName = '"          + m_ModelName +
                       "' AND Algorithm = '"          + m_HPC_Algorithm +
                       "' AND Minimizer = '"          + m_HPC_Minimizer +
                       "' AND ObjectiveCriterion = '" + m_HPC_ObjectiveCriterion +
                       "' AND Scaling = '"            + m_HPC_Scaling + "'";
//                       "' ORDER BY SpeName";
            dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = (int)dataMap["Value"].size();
            NumCols = NumSpecies;
            NumRows = NumRecords/NumSpecies;
            std::swap(NumRows,NumCols);
        } else if (tableName == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) {
            fields = {"SpeName","Guild","Value"};
            queryStr = "SELECT SpeName,Guild,Value FROM " + tableName +
                       "  WHERE ProjectName = '"      + m_ProjectName +
                       "' AND ModelName = '"          + m_ModelName +
                       "' AND Algorithm = '"          + m_HPC_Algorithm +
                       "' AND Minimizer = '"          + m_HPC_Minimizer +
                       "' AND ObjectiveCriterion = '" + m_HPC_ObjectiveCriterion +
                       "' AND Scaling = '"            + m_HPC_Scaling +
                       "' ORDER BY SpeName,Guild";
            dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = (int)dataMap["Value"].size();
            NumRows = NumSpecies;
            NumCols = NumRecords/NumSpecies;
        } else {
            fields = {"SpeciesA","SpeciesB","Value"};
            queryStr = "SELECT SpeciesA,SpeciesB,Value FROM " + tableName +
                       "  WHERE ProjectName = '"      + m_ProjectName +
                       "' AND ModelName = '"          + m_ModelName +
                       "' AND Algorithm = '"          + m_HPC_Algorithm +
                       "' AND Minimizer = '"          + m_HPC_Minimizer +
                       "' AND ObjectiveCriterion = '" + m_HPC_ObjectiveCriterion +
                       "' AND Scaling = '"            + m_HPC_Scaling +
                       "' ORDER BY SpeciesA,SpeciesB";
            dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = (int)dataMap["Value"].size();
            NumRows = NumSpecies;
            NumCols = NumSpecies;
        }

        if (NumRecords > 0) {
            smodel = new QStandardItemModel( NumRows, NumCols);
            m = 0;
            Years.clear();
            for (int ii=0; ii<NumRows; ++ii) {
                Years << QString::number(ii);
                for (int jj=0; jj<NumCols; ++jj) {
                    val = std::stod(dataMap["Value"][m++]);
                    eeFactor = (tableName == nmfConstantsMSSPM::TableOutputPredationRho) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                     val,m_NumSignificantDigits,eeFactor*6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    if (tableName == nmfConstantsMSSPM::TableOutputBiomass) {
                        smodel->setItem(jj, ii, item);
                    } else {
                        smodel->setItem(ii, jj, item);
                    }
                }
            }
            smodel->setHorizontalHeaderLabels(SpeciesNames);
            if (tableName == nmfConstantsMSSPM::TableOutputBiomass) {
                smodel->setVerticalHeaderLabels(Years);
            } else {
                smodel->setVerticalHeaderLabels(SpeciesNames);
            }
            MatrixTableViews[i]->setModel(smodel);
        }
    }
}

void
nmfMainWindow::refreshSummaryModelFit(
        const std::string& Algorithm,
        const std::string& Minimizer,
        const std::string& ObjectiveCriterion,
        const std::string& Scaling)
{
    int m = 0;
    int eeFactor;
    int speciesRow = 1;
    double val;
    QString speciesName;
    QString valueWithComma;
    QStringList SpeciesList = {};
    QStringList vLabels = {};
    QStandardItemModel* smodel;
    QStandardItem* item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields = {"SpeciesName","SSResiduals","SSDeviations","SSTotals","rSquared","rCorrelationCoeff","AkaikeInfoCriterion","RootMeanSquareError","ReliabilityIndex","AverageError","AverageAbsError","ModelingEfficiency"};
    queryStr = "SELECT SpeciesName,SSResiduals,SSDeviations,SSTotals,rSquared,rCorrelationCoeff,AkaikeInfoCriterion,RootMeanSquareError,ReliabilityIndex,AverageError,AverageAbsError,ModelingEfficiency FROM " +
                nmfConstantsMSSPM::TableSummaryModelFit +
               "  WHERE ProjectName = '"      + m_ProjectName +
               "' AND ModelName = '"          + m_ModelName +
               "' AND Algorithm = '"          + Algorithm +
               "' AND Minimizer = '"          + Minimizer +
               "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
               "' AND Scaling = '"            + Scaling +
               "' ORDER BY SpeciesName";
    dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords    = (int)dataMap["SpeciesName"].size();
    int NumSpecies    = NumRecords - 2; // -2 because there's a Model row and a Statistic row
    int NumStatistics = fields.size() - 1;
    int NumCols       = NumRecords;
    int NumRows       = NumStatistics;

    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"No records found in table: summarymodelfit");
        return;
    }

    smodel = new QStandardItemModel(NumRows, NumCols);

    for (int i=0; i<NumRows; ++i) {
            speciesName = QString::fromStdString(dataMap["SpeciesName"][i]);
            if (speciesName == "Statistic") {
                for (int j=0; j<NumStatistics; ++j) {
                    item = new QStandardItem(QString::fromStdString(dataMap[fields[j+1]][i]));
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(j, 0, item);
                }
            } else if (speciesName == "Model") {
                for (int j=0; j<NumStatistics; ++j) {
                    val = std::stod(dataMap[fields[j+1]][i]);
                    eeFactor = (val > 999999) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                     val,m_NumSignificantDigits,eeFactor*6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(j, NumRecords, item);
                }
            } else {
                SpeciesList << speciesName;
                for (int j=0; j<NumStatistics; ++j) {
                    val = std::stod(dataMap[fields[j+1]][i]);
                    eeFactor = (val > 999999) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                     val,m_NumSignificantDigits,eeFactor*6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(j, speciesRow, item);
                }
                ++speciesRow;
            }
    }

    vLabels.clear();
    vLabels << "Statistic" << SpeciesList << "Model";
    smodel->setHorizontalHeaderLabels(vLabels);
    SummaryTV->setModel(smodel);
    SummaryTV->resizeColumnsToContents();
}



void
nmfMainWindow::menu_setBees(bool isPressed)
{
    m_isPressedBeesButton = isPressed;
    m_UI->actionShowAllSavedRunsTB->setEnabled(isAtLeastOneFilterPressed());
}

void
nmfMainWindow::menu_setNLopt(bool isPressed)
{
    m_isPressedNLoptButton = isPressed;
    m_UI->actionShowAllSavedRunsTB->setEnabled(isAtLeastOneFilterPressed());
}

double
nmfMainWindow::calculateCarryingCapacityForMSY(
        const int& SpeciesNum,
        const std::vector<double>& EstCarryingCapacities,
        const std::vector<double>& EstGrowthRates,
        const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
        const boost::numeric::ublas::matrix<double>& EstPredationRho)
{

    double carryingCapacity = EstCarryingCapacities[SpeciesNum];

    // if K=0, then we use K=r/(alpha+rho)
    if (carryingCapacity == 0) {
        double alphaVal = 0;
        double rhoVal   = 0;
        if ((EstCompetitionAlpha.size1() != 0) && (EstCompetitionAlpha.size2() != 0)) {
            alphaVal = EstCompetitionAlpha(SpeciesNum,SpeciesNum);
        }
        if ((EstPredationRho.size1() != 0) && (EstPredationRho.size2() != 0)) {
            rhoVal = EstPredationRho(SpeciesNum,SpeciesNum);
        }
        if ((alphaVal != 0) || (rhoVal != 0)) {
            carryingCapacity = EstGrowthRates[SpeciesNum]/(alphaVal+rhoVal);
        }
    }
    return carryingCapacity;
}

void
nmfMainWindow::updateOutputTables(
        std::string&                                 Algorithm,
        std::string&                                 Minimizer,
        std::string&                                 ObjectiveCriterion,
        std::string&                                 Scaling,
        const int&                                   isCompAggProd,
        const QStringList&                           SpeciesList,
        const QStringList&                           GuildList,
        const std::vector<double>&                   EstInitBiomass,
        const std::vector<double>&                   EstGrowthRates,
        const std::vector<double>&                   EstGrowthRateShape,
        const std::vector<double>&                   EstGrowthRateCovariateCoeffs,
        const std::vector<double>&                   EstCarryingCapacities,
        const std::vector<double>&                   EstCarryingCapacityCovariateCoeffs,
        const std::vector<double>&                   EstCatchability,
        const std::vector<double>&                   EstCatchabilityCovariateCoeffs,
        const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
        const boost::numeric::ublas::matrix<double>& EstPredationRho,
        const boost::numeric::ublas::matrix<double>& EstPredationHandling,
        const std::vector<double>&                   EstPredationExponent,
        const std::vector<double>&                   EstSurveyQ,
        const std::vector<double>&                   EstSurveyQCovariateCoeffs,
        const std::vector<double>&                   EstBMSY,
        const std::vector<double>&                   EstMSY,
        const std::vector<double>&                   EstFMSY)
{
    int SpeciesNum;
    double value=0;
    double carryingCapacity;
    std::string cmd;
    std::string errorMsg;
    QString msg;
    std::string isAggProd = std::to_string(isCompAggProd);

    //
    // Clear and then load output data tables...
    //
    QList<std::string> OutputTableNames  = {nmfConstantsMSSPM::TableOutputInitBiomass,
                                            nmfConstantsMSSPM::TableOutputGrowthRate,
                                            nmfConstantsMSSPM::TableOutputGrowthRateShape,
                                            nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs,
                                            nmfConstantsMSSPM::TableOutputCarryingCapacity,
                                            nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs,
                                            nmfConstantsMSSPM::TableOutputCatchability,
                                            nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs,
                                            nmfConstantsMSSPM::TableOutputPredationExponent,
                                            nmfConstantsMSSPM::TableOutputMSY,
                                            nmfConstantsMSSPM::TableOutputMSYBiomass,
                                            nmfConstantsMSSPM::TableOutputMSYFishing,
                                            nmfConstantsMSSPM::TableOutputSurveyQ,
                                            nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs};

    for (std::string tableName : OutputTableNames)
    {
        SpeciesNum = 0;
        cmd = "DELETE FROM " + tableName +
              "  WHERE ProjectName = '"      + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling = '"            + Scaling +
              "' AND isAggProd = "           + isAggProd;
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] UpdateOutputTables: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\n[Error 2] nmfMainWindow::updateOutputTables:  Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n";
            QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }

        cmd = "REPLACE INTO " + tableName +
                " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value) VALUES ";

        for (int i=0; i<SpeciesList.size(); ++i) {
            value = 0;
            if (tableName == nmfConstantsMSSPM::TableOutputInitBiomass) {
                if (! EstInitBiomass.empty()) {
                    value = EstInitBiomass[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputGrowthRate) {
                if (! EstGrowthRates.empty()) {
                    value = EstGrowthRates[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputGrowthRateShape) {
                if (! EstGrowthRateShape.empty()) {
                    value = EstGrowthRateShape[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs) {
                if (! EstGrowthRateCovariateCoeffs.empty()) {
                    value = EstGrowthRateCovariateCoeffs[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputCarryingCapacity) {
                if (! EstCarryingCapacities.empty()) {
                    value = EstCarryingCapacities[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs) {
                if (! EstCarryingCapacityCovariateCoeffs.empty()) {
                    value = EstCarryingCapacityCovariateCoeffs[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputPredationExponent) {
                if (! EstPredationExponent.empty()) {
                    value = EstPredationExponent[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputCatchability) {
                if (! EstCatchability.empty()) {
                    value = EstCatchability[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs) {
                if (! EstCatchabilityCovariateCoeffs.empty()) {
                    value = EstCatchabilityCovariateCoeffs[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputSurveyQ) {
                if (! EstSurveyQ.empty()) {
                    value = EstSurveyQ[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs) {
                if (! EstSurveyQCovariateCoeffs.empty()) {
                    value = EstSurveyQCovariateCoeffs[SpeciesNum++];
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputMSYBiomass) {
                if (! EstCarryingCapacities.empty()) {
                    carryingCapacity = calculateCarryingCapacityForMSY(
                         SpeciesNum,EstCarryingCapacities,EstGrowthRates,
                         EstCompetitionAlpha,EstPredationRho);
                    value = carryingCapacity/2.0;
                    ++SpeciesNum;
                    //value = EstCarryingCapacities[SpeciesNum++]/2.0;
                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputMSY) {
                if (! EstMSY.empty()) {
                    value = EstMSY[SpeciesNum++];
                }
//                if (! EstGrowthRates.empty() and ! EstCarryingCapacities.empty()) {
//                    carryingCapacity = calculateCarryingCapacityForMSY(
//                         SpeciesNum,EstCarryingCapacities,EstGrowthRates,
//                         EstCompetitionAlpha,EstPredationRho);
//                    value = EstGrowthRates[SpeciesNum]*carryingCapacity/4.0;
//                    ++SpeciesNum;
//                }
            } else if (tableName == nmfConstantsMSSPM::TableOutputMSYFishing) {
                if (! EstGrowthRates.empty()) {
                    value = EstGrowthRates[SpeciesNum++]/2.0;
                }
            }

            cmd += "('"   + m_ProjectName +
                    "','" + m_ModelName +
                    "','" + Algorithm +
                    "','" + Minimizer +
                    "','" + ObjectiveCriterion +
                    "','" + Scaling +
                    "',"  + isAggProd +
                    ",'"  + SpeciesList[i].toStdString() +
                    "',"  + QString::number(value,'f',20).toStdString() + "),";
        }

        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 3] UpdateOutputTables: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(this, "Error",
                                 "\n[Error 4] nmfMainWindow::updateOutputTables:  Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            return;
        }
    }

    //
    // Load the Output Interaction and Predation tables (they're matrices)
    //
    QList<std::string> OutputTableNames2  = {nmfConstantsMSSPM::TableOutputCompetitionAlpha,
                                             nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies,
                                             nmfConstantsMSSPM::TableOutputPredationRho,
                                             nmfConstantsMSSPM::TableOutputPredationHandling};
    QList<bool> Skip = {EstCompetitionAlpha.size1()       == 0,
                        EstCompetitionBetaSpecies.size1() == 0,
                        EstPredationRho.size1()           == 0,
                        EstPredationHandling.size1()      == 0};
    int i = 0;
    for (std::string tableName : OutputTableNames2)
    {
        if (Skip[i++]) {
            continue;
        }
        cmd = "DELETE FROM " + tableName +
              "  WHERE ProjectName = '"      + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling = '"            + Scaling +
              "' AND isAggProd = "           + isAggProd;
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 5] nmfMainWindow::updateOutputTables: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\n[Error 6] nmfMainWindow::updateOutputTables: Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
                    QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }
        cmd = "REPLACE INTO " +
               tableName +
              " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value) VALUES ";
        for (int row=0; row<SpeciesList.size(); ++row) {
            for (int col=0; col<SpeciesList.size(); ++col) {
                if (tableName == nmfConstantsMSSPM::TableOutputCompetitionAlpha) {
                    value = EstCompetitionAlpha(row,col);
                } else if (tableName == nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies) {
                    value = EstCompetitionBetaSpecies(row,col);
                } else if (tableName == nmfConstantsMSSPM::TableOutputPredationRho) {
                    value = EstPredationRho(row,col);
                } else if (tableName == nmfConstantsMSSPM::TableOutputPredationHandling) {
                    value = EstPredationHandling(row,col);
                }
                if (std::isnan(std::fabs(value)))
                    value = 0;
                std::ostringstream val;
                val << value;
                cmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + Algorithm +
                       "','" + Minimizer +
                       "','" + ObjectiveCriterion +
                       "','" + Scaling +
                       "',"  + isAggProd +
                       ",'"  + SpeciesList[row].toStdString() +
                       "','" + SpeciesList[col].toStdString() +
//                     "',"  + val.str() + "),";
                       "',"  + QString::number(value,'f',20).toStdString() + "),";
            }
        }

        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 7] nmfMainWindow::updateOutputTables: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(this, "Error",
                                 "\n[Error 8] nmfMainWindow::updateOutputTables: Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            return;
        }
    }

    //
    // Load the output OutputCompetitionBetaGuilds table
    //
    QList<std::string> OutputTableNames3 = {nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds};
    Skip.clear();
    Skip.push_back(EstCompetitionBetaGuilds.size1() == 0);
    i = 0;
    for (std::string tableName : OutputTableNames3)
    {
        if (Skip[i++])
            continue;
        cmd = "DELETE FROM "                 + tableName +
              "  WHERE ProjectName ='"       + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling = '"            + Scaling +
              "' AND isAggProd = "           + isAggProd;
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 9] nmfMainWindow::updateOutputTables: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\n[Error 10] nmfMainWindow::updateOutputTables: Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
                    QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }
        cmd = "REPLACE INTO " +
               tableName +
              " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild,Value) VALUES ";
        for (int row=0; row<SpeciesList.size(); ++row) {
            for (int col=0; col<GuildList.size(); ++col) {
                if (tableName == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) {
                    value = EstCompetitionBetaGuilds(row,col);
                }
                if (std::isnan(std::fabs(value)))
                    value = 0;
                std::ostringstream val;
                val << value;
                cmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + Algorithm +
                       "','" + Minimizer +
                       "','" + ObjectiveCriterion +
                       "','" + Scaling +
                       "',"  + isAggProd +
                       ",'"  + SpeciesList[row].toStdString() +
                       "','" + GuildList[col].toStdString() +
                       "',"  + val.str() + "),";
            }
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 11] nmfMainWindow::updateOutputTables: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(this, "Error",
                                 "\n[Error 12] nmfMainWindow::updateOutputTables: Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            return;
        }
    }

    //
    // Load the output OutputCompetitionBetaGuildsGuilds table
    //
    QList<std::string> OutputTableNames4 = {nmfConstantsMSSPM::TableOutputCompetitionBetaGuildsGuilds};
    Skip.clear();
    Skip.push_back(EstCompetitionBetaGuildsGuilds.size1() == 0);
    i = 0;
    for (std::string tableName : OutputTableNames4)
    {
        if (Skip[i++])
            continue;
        cmd = "DELETE FROM "                 + tableName +
              "  WHERE ProjectName = '"      + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling = '"            + Scaling +
              "' AND isAggProd = "           + isAggProd;
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 10] nmfMainWindow::updateOutputTables: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\n[Error 10] nmfMainWindow::updateOutputTables: Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
                    QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }
        cmd = "REPLACE INTO " +
               tableName +
              " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,GuildA,GuildB,Value) VALUES ";
        for (int row=0; row<GuildList.size(); ++row) {
            for (int col=0; col<GuildList.size(); ++col) {
                value = EstCompetitionBetaGuildsGuilds(row,col);
                if (std::isnan(std::fabs(value))) {
                    value = 0;
                }
                std::ostringstream val;
                val << value;
                cmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + Algorithm +
                       "','" + Minimizer +
                       "','" + ObjectiveCriterion +
                       "','" + Scaling +
                       "',"  + isAggProd +
                       ",'"  + GuildList[row].toStdString() +
                       "','" + GuildList[col].toStdString() +
                       "',"  + val.str() + "),";
            }
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 12] nmfMainWindow::updateOutputTables: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(this, "Error",
                                 "\n[Error 13] nmfMainWindow::updateOutputTables: Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            return;
        }
    }

}

bool
nmfMainWindow::loadUncertaintyData(const bool&          isMonteCarlo,
                                   const int&           NumSpecies,
                                   const std::string&   ForecastName,
                                   std::string          Algorithm,
                                   std::string          Minimizer,
                                   std::string          ObjectiveCriterion,
                                   std::string          Scaling,
                                   std::vector<double>& InitBiomassUncertainty,
                                   std::vector<double>& GrowthRateUncertainty,
                                   std::vector<double>& GrowthRateShapeUncertainty,
                                   std::vector<double>& CarryingCapacityUncertainty,
                                   std::vector<double>& PredationUncertainty,
                                   std::vector<double>& CompetitionUncertainty,
                                   std::vector<double>& BetaSpeciesUncertainty,
                                   std::vector<double>& BetaGuildsUncertainty,
                                   std::vector<double>& BetaGuildsGuildsUncertainty,
                                   std::vector<double>& HandlingUncertainty,
                                   std::vector<double>& ExponentUncertainty,
                                   std::vector<double>& CatchabilityUncertainty,
                                   std::vector<double>& SurveyQUncertainty,
                                   std::vector<double>& HarvestUncertainty,
                                   std::vector<double>& GrowthRateCovCoeffUncertainty,
                                   std::vector<double>& CarryingCapacityCovCoeffUncertainty,
                                   std::vector<double>& CatchabilityCovCoeffUncertainty,
                                   std::vector<double>& SurveyQCovCoeffUncertainty)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    std::string CompetitionForm;

    // Clear data structures
    InitBiomassUncertainty.clear();
    GrowthRateUncertainty.clear();
    GrowthRateShapeUncertainty.clear();
    CarryingCapacityUncertainty.clear();
    PredationUncertainty.clear();
    CompetitionUncertainty.clear();
    BetaSpeciesUncertainty.clear();
    BetaGuildsUncertainty.clear();
    BetaGuildsGuildsUncertainty.clear();
    HandlingUncertainty.clear();
    ExponentUncertainty.clear();
    CatchabilityUncertainty.clear();
    SurveyQUncertainty.clear();
    HarvestUncertainty.clear();
    GrowthRateCovCoeffUncertainty.clear();
    CarryingCapacityCovCoeffUncertainty.clear();
    CatchabilityCovCoeffUncertainty.clear();
    SurveyQCovCoeffUncertainty.clear();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (isMonteCarlo) {
        fields     = {"ProjectName","ModelName","ForecastName","SpeName","Algorithm","Minimizer","ObjectiveCriterion","Scaling",
                      "InitBiomass","GrowthRate","GrowthRateShape","CarryingCapacity","Predation","Competition",
                      "BetaSpecies","BetaGuilds","BetaGuildsGuilds","Handling","Exponent","Catchability","SurveyQ","Harvest",
                      "GrowthRateCovCoeff","CarryingCapacityCovCoeff","CatchabilityCovCoeff","SurveyQCovCoeff"};
        queryStr   = "SELECT ProjectName,ModelName,ForecastName,SpeName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
        queryStr  += "InitBiomass,GrowthRate,GrowthRateShape,CarryingCapacity,Catchability,CompetitionAlpha,CompetitionBetaSpecies,";
        queryStr  += "CompetitionBetaGuilds,CompetitionBetaGuildsGuilds,PredationRho,PredationHandling,PredationExponent,";
        queryStr  += "SurveyQ,Harvest,GrowthRateCovCoeff,CarryingCapacityCovCoeff,CatchabilityCovCoeff,SurveyQCovCoeff FROM " +
                      nmfConstantsMSSPM::TableForecastUncertainty +
                      " WHERE ProjectName = '"       + m_ProjectName +
                      "' AND ModelName = '"          + m_ModelName +
                      "' AND ForecastName = '"       + ForecastName +
                      "' AND Algorithm = '"          + Algorithm +
                      "' AND Minimizer = '"          + Minimizer +
                      "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                      "' AND Scaling = '"            + Scaling +
                      "' ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords != NumSpecies) {
            msg  = "[Error 1] nmfMainWindow::loadUncertaintyData: Incorrect number of records found in table: ForecastUncertainty. ";
            msg += "Found " + std::to_string(NumRecords) + " records expecting " + std::to_string(NumSpecies)+".";
            m_Logger->logMsg(nmfConstants::Error,msg);
            // m_Logger->logMsg(nmfConstants::Error,queryStr);
            QMessageBox::warning(this, "Error",
                                 "\nMissing Forecast data. Please check tabs:\nForecast->Harvest Data and Forecast->Uncertainty Parameters\nand re-save.",
                                 QMessageBox::Ok);
           return false;
        }
        for (int i=0; i<NumSpecies; ++i) {
            InitBiomassUncertainty.emplace_back(std::stod(dataMap["InitBiomass"][i]));
            GrowthRateUncertainty.emplace_back(std::stod(dataMap["GrowthRate"][i]));
            GrowthRateShapeUncertainty.emplace_back(std::stod(dataMap["GrowthRateShape"][i]));
            CarryingCapacityUncertainty.emplace_back(std::stod(dataMap["CarryingCapacity"][i]));
            PredationUncertainty.emplace_back(std::stod(dataMap["Predation"][i]));
            CompetitionUncertainty.emplace_back(std::stod(dataMap["Competition"][i]));
            BetaSpeciesUncertainty.emplace_back(std::stod(dataMap["BetaSpecies"][i]));
            BetaGuildsUncertainty.emplace_back(std::stod(dataMap["BetaGuilds"][i]));
            BetaGuildsGuildsUncertainty.emplace_back(std::stod(dataMap["BetaGuildsGuilds"][i]));
            HandlingUncertainty.emplace_back(std::stod(dataMap["Handling"][i]));
            ExponentUncertainty.emplace_back(std::stod(dataMap["Exponent"][i]));
            CatchabilityUncertainty.emplace_back(std::stod(dataMap["Catchability"][i]));
            SurveyQUncertainty.emplace_back(std::stod(dataMap["SurveyQ"][i]));
            HarvestUncertainty.emplace_back(std::stod(dataMap["Harvest"][i]));
            GrowthRateCovCoeffUncertainty.emplace_back(std::stod(dataMap["GrowthRateCovCoeff"][i]));
            CarryingCapacityCovCoeffUncertainty.emplace_back(std::stod(dataMap["CarryingCapacityCovCoeff"][i]));
            CatchabilityCovCoeffUncertainty.emplace_back(std::stod(dataMap["CatchabilityCovCoeff"][i]));
            SurveyQCovCoeffUncertainty.emplace_back(std::stod(dataMap["SurveyQCovCoeff"][i]));
        }
    } else {
        for (int i=0; i<NumSpecies; ++i) {
            InitBiomassUncertainty.emplace_back(0);
            GrowthRateUncertainty.emplace_back(0);
            GrowthRateShapeUncertainty.emplace_back(0);
            CarryingCapacityUncertainty.emplace_back(0);
            PredationUncertainty.emplace_back(0);
            CompetitionUncertainty.emplace_back(0);
            BetaSpeciesUncertainty.emplace_back(0);
            BetaGuildsUncertainty.emplace_back(0);
            BetaGuildsGuildsUncertainty.emplace_back(0);
            HandlingUncertainty.emplace_back(0);
            ExponentUncertainty.emplace_back(0);
            CatchabilityUncertainty.emplace_back(0);
            SurveyQUncertainty.emplace_back(0);
            HarvestUncertainty.emplace_back(0);
            GrowthRateCovCoeffUncertainty.emplace_back(0);
            CarryingCapacityCovCoeffUncertainty.emplace_back(0);
            CatchabilityCovCoeffUncertainty.emplace_back(0);
            SurveyQCovCoeffUncertainty.emplace_back(0);
        }
    }
    return true;
}


bool
nmfMainWindow::scaleTimeSeriesIfMonteCarlo(const bool& isMonteCarlo,
                                           const std::vector<double>& Uncertainty,
                                           boost::numeric::ublas::matrix<double>& HarvestMatrix,
                                           std::vector<double>& RandomValues)
{
    int NumYears   = HarvestMatrix.size1();
    int NumSpecies = HarvestMatrix.size2();
    double RandomValue = 0;

    RandomValues.clear();

    for (int i=0; i<NumSpecies; ++i) {
        if (isMonteCarlo) {
            calculateMonteCarloValue(Uncertainty[i],0.0,RandomValue);
        }
        RandomValues.push_back(RandomValue);
        for (int j=0; j<NumYears; ++j) {
            HarvestMatrix(j,i) = (1.0 + RandomValue) * HarvestMatrix(j,i);
        }
    }

    return true;
}

double
nmfMainWindow::calculateMonteCarloValue(const double& uncertainty,
                                        const double& dataValue,
                                        double& randomValue)
{
    double retv = dataValue;

    randomValue = 0.0;

    if (uncertainty != 0.0) {
        m_SeedValue = (m_SeedValue > 0) ? ++m_SeedValue : m_SeedValue;
        randomValue = nmfUtils::getRandomNumber(m_SeedValue,-uncertainty,uncertainty);
        retv = (1.0 + randomValue) * dataValue;
//      retv = nmfUtils::getRandomNumber(m_SeedValue,dataValue*(1.0-uncertainty),dataValue*(1.0+uncertainty));
    }

    return retv;
}

void
nmfMainWindow::clearChart()
{
    // Clear the gui plot area
    m_ChartWidget->removeAxis(m_ChartWidget->axes(Qt::Horizontal)[0]);
    m_ChartWidget->removeAxis(m_ChartWidget->axes(Qt::Vertical)[0]);
    m_ChartWidget->setTitle("");
    m_ChartWidget->removeAllSeries();

//    // Clear all gui tables
//    clearOutputTables();
}

bool
nmfMainWindow::clearMonteCarloParametersTable(
        std::string& ForecastName,
        std::string  Algorithm,
        std::string  Minimizer,
        std::string  ObjectiveCriterion,
        std::string  Scaling,
        std::string& MonteCarloParametersTable)
{
    std::string cmd="";
    std::string errorMsg;

    if (! ForecastName.empty()) {
        checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
        cmd = "DELETE FROM " + MonteCarloParametersTable +
              " WHERE ProjectName = '"       + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND ForecastName = '"       + ForecastName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling   = '"          + Scaling + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] clearMonteCarloParametersTable: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(this, "Error",
                                 "\nError in clearMonteCarloParametersTable command:\n\n" + QString::fromStdString(errorMsg) + "\n",
                                 QMessageBox::Ok);
            return false;
        }
    }

    return true;
}




//
// Clear OutputBiomass table with passed Algorithm, Minimizer, ObjectiveCriterion, Scaling
//
bool
nmfMainWindow::clearOutputBiomassTable(std::string& ForecastName,
                                       std::string  Algorithm,
                                       std::string  Minimizer,
                                       std::string  ObjectiveCriterion,
                                       std::string  Scaling,
                                       std::string& isAggProd,
                                       std::string& BiomassTable)
{
    std::string cmd;
    std::string errorMsg;
    QString msg;

    if (ForecastName.empty()) {
        cmd = "DELETE FROM " + BiomassTable +
                " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling   = '"          + Scaling +
                "' AND isAggProd = "           + isAggProd;
    } else {
        checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
        cmd = "DELETE FROM " + BiomassTable +
                " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND ForecastName = '"       + ForecastName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling   = '"          + Scaling +
                "' AND isAggProd = "           + isAggProd;
    }

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] ClearOutputBiomassTable: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        msg = "\nError in ClearOutputBiomassTable command in table(" +
                QString::fromStdString(BiomassTable) + "):\n\n" +
                QString::fromStdString(errorMsg) + "\n";
        QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
        return false;
    }
    return true;
}

bool
nmfMainWindow::setFirstRowEstimatedBiomass(
        const std::string& ForecastName,
        const int& NumSpeciesOrGuilds,
        const boost::numeric::ublas::vector<double>& InitialBiomass,
        boost::numeric::ublas::matrix<double>& EstInitBiomassCovariates,
        boost::numeric::ublas::matrix<double>& EstBiomassSpecies)
{
    double initBiomassCovariateCoeff = 0.0; // RSK possibly estimate this eventually
    nmfStructsQt::ModelDataStruct dataStruct;
    QList<double> OutputInitBiomass;
    std::string msg;

    bool loadOK = loadParameters(dataStruct,nmfConstantsMSSPM::VerboseOff);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::updateTmpMatrixWithOutputInitBiomass: loadParameters() failed");
        return false;
    }

    bool isCheckedEstimateInitBiomass = nmfUtils::isEstimateParameterChecked(dataStruct,"InitBiomass");
    std::string covariateAlgorithmType = m_DatabasePtr->getCovariateAlgorithmType(m_Logger,m_ProjectName,m_ModelName);
    if (isCheckedEstimateInitBiomass && ForecastName.empty()) {
        getOutputParameterVector(nmfConstantsMSSPM::TableOutputInitBiomass,false,ForecastName,OutputInitBiomass);
        if (OutputInitBiomass.size() == NumSpeciesOrGuilds) {
            for (int species=0; species<NumSpeciesOrGuilds; ++species) {
//              EstBiomassSpecies(0,species) = OutputInitBiomass[species]*(1.0+initBiomassCovariateCoeff*EstInitBiomassCovariates(0,species));
                EstBiomassSpecies(0,species) = nmfUtils::applyCovariate(m_Logger,
                            covariateAlgorithmType,OutputInitBiomass[species],
                            initBiomassCovariateCoeff,EstInitBiomassCovariates(0,species));
            }
        } else {
            msg = "Estimated biomass size (" + std::to_string(OutputInitBiomass.size()) +
                  ") not equal to Number of Species (" + std::to_string(NumSpeciesOrGuilds) + ")";
            m_Logger->logMsg(nmfConstants::Error,msg);
            return false;
        }
    } else {
        for (int species=0; species<NumSpeciesOrGuilds; ++species) {
//          EstBiomassSpecies(0,species) = InitialBiomass[species]*(1.0+initBiomassCovariateCoeff*EstInitBiomassCovariates(0,species));
            EstBiomassSpecies(0,species) = nmfUtils::applyCovariate(m_Logger,
                        covariateAlgorithmType,InitialBiomass[species],
                        initBiomassCovariateCoeff,EstInitBiomassCovariates(0,species));
        }
    }
    return true;
}

bool
nmfMainWindow::updateObservedBiomassAndEstSurveyQTable(
        const QStringList& SpeciesList,
        const int& NumYears,
        const std::vector<double>& EstSurveyQ,
        std::vector<double>& EstSurveyQCovariateCoeffs)
{
    bool zeroError = false;
    int NumRecords;
    int NumSpecies = SpeciesList.size();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    double quotient;
    double estSurveyQ;
    double relativeBiomass;
    QString Species;
    int m=0;

    // Get SurveyQ Covariate data
    std::vector<double> surveyQCovariateCoeffs;
    boost::numeric::ublas::matrix<double> surveyQCovariate;
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"SurveyQ",surveyQCovariate);

    // Read BiomassRelative data from table
    fields    = {"ProjectName","ModelName","SpeName","Year","Value"};
    queryStr  = "SELECT ProjectName,ModelName,SpeName,Year,Value FROM " +
                 nmfConstantsMSSPM::TableBiomassRelative +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName +
                "' ORDER BY SpeName,Year";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Value"].size();
    if (NumRecords != NumSpecies*NumYears) {
        errorMsg  = "nmfMainWindow::updateObservedBiomassAndEstSurveyQTable Incorrect number of records found in: BiomassRelativeDividedByEstSurveyQ";
        errorMsg += "\n\nFound " + std::to_string(NumRecords) + " expecting " + std::to_string(NumSpecies)+" * "+std::to_string(NumYears) + ".";
        m_Logger->logMsg(nmfConstants::Error, "nmfMainWindow::updateObservedBiomassAndEstSurveyQTable: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error, queryStr);
        errorMsg  = "\n" + errorMsg + "\n";
        QMessageBox::information(this, tr("Run Failed"), tr(errorMsg.c_str()),QMessageBox::Ok);
        return false;
    }

    // Clear BiomassRelativeDividedByEstSurveyQ table
    std::string cmd = "DELETE FROM " +
                       nmfConstantsMSSPM::TableBiomassRelativeDividedByEstSurveyQ +
                      " WHERE ProjectName = '" + m_ProjectName +
                      "' AND ModelName = '"    + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::updateObservedBiomassAndEstSurveyQTable: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(this, "Error",
                             "\nCouldn't delete all records from updateObservedBiomassAndEstSurveyQTable table.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Write to BiomassRelativeDividedByEstSurveyQ table
    cmd = "INSERT INTO " +
           nmfConstantsMSSPM::TableBiomassRelativeDividedByEstSurveyQ +
          " (ProjectName,ModelName,SpeName,Year,Value) VALUES ";
    double surveyQTerm = 0;
    for (int species=0; species<NumSpecies; ++species) {
        std::string covariateAlgorithmType = m_DataStruct.CovariateAlgorithmType;
        estSurveyQ = (int(EstSurveyQ.size()) == NumSpecies) ? EstSurveyQ[species] : 0;
        if (nmfUtils::isNearlyZero(estSurveyQ)) {
            zeroError = true;
            estSurveyQ = 1;
        }

        if (EstSurveyQCovariateCoeffs.size() > 0) {
            surveyQTerm = nmfUtils::applyCovariate(nullptr,
                                                   covariateAlgorithmType,
                                                   estSurveyQ,
                                                   EstSurveyQCovariateCoeffs[species],
                                                   surveyQCovariate(0,species));
        } else {
           surveyQTerm = estSurveyQ;
        }
        if (EstSurveyQ.size() == 0) {
           surveyQTerm = estSurveyQ;
        }

        Species = SpeciesList[species];
        for (int year=0; year<NumYears; ++year) {
            relativeBiomass = std::stod(dataMap["Value"][m++]);
            if (relativeBiomass == nmfConstantsMSSPM::NoData) {
                quotient = relativeBiomass;
            } else {
                quotient = (surveyQTerm != 0) ? (relativeBiomass/surveyQTerm) : 0; // Scaling up the biomass
            }
            // Write quotient to BiomassRelativeDividedByEstSurveyQ table
            cmd += "('"   + m_ProjectName +
                    "','" + m_ModelName +
                    "','" + Species.toStdString() +
                    "',"  + std::to_string(year) +
//                  ","   + std::to_string(quotient) + "),";
                    ","   + QString::number(quotient,'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
        }
    }

    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::updateObservedBiomassAndEstSurveyQTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }
    if (zeroError) {
        m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::updateObservedBiomassAndEstSurveyQTable: Found 0 in EstSurveyQ table");
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

    return true;
}

void
nmfMainWindow::checkDiagnosticAlgorithmIdentifiersForMultiRun(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling)
{
    readSettings("Runtime");

    if ( m_IsMultiRun && Diagnostic_Tab1_ptr->isSetLastRunMultiDiagnostics())
    {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    }
}

bool
nmfMainWindow::checkForecastAlgorithmIdentifiersForMultiRun(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling)
{
    bool retv = false;
    readSettings("Runtime");

    if (m_IsMultiRun && Forecast_Tab1_ptr->isSetLastRunMultiForecast())
    {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
        retv = true;
    }
    return retv;
}

void
nmfMainWindow::checkModelHarvestTypeMatchesForecastButtonLabel(
        const QString& HarvestForm,
        const QString& ForecastButtonLabel)
{
    if (! HarvestForm.contains(ForecastButtonLabel)) {
        std::string msg = "Model harvest form (" + HarvestForm.toStdString() + ")";
        msg += " is not the same as the Forecast Harvest form (" + ForecastButtonLabel.toStdString() + ")";
        m_Logger->logMsg(nmfConstants::Warning,msg);
    }
}

bool
nmfMainWindow::loadParametersMisc(nmfStructsQt::ModelDataStruct& dataStruct)
{
    int NumSpecies;
    QStringList SpeciesList;

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }
    dataStruct.SpeciesNames.clear();
    for (QString species : SpeciesList) {
        dataStruct.SpeciesNames.push_back(species.toStdString());
    }
    return true;
}

bool
nmfMainWindow::updateOutputBiomassTable(std::string& ForecastName,
                                        int&         StartYear,
                                        int&         RunLength,
                                        bool&        isMonteCarlo,
                                        int&         RunNum,
                                        std::string  Algorithm,
                                        std::string  Minimizer,
                                        std::string  ObjectiveCriterion,
                                        std::string  Scaling,
                                        std::string& isAggProdStr,
                                        std::string& GrowthForm,
                                        std::string& HarvestFormName,
                                        std::string& CompetitionForm,
                                        std::string& PredationForm,
                                        std::string& OutputInitBiomassTable,
                                        std::string& OutputGrowthRateTable,
                                        std::string& OutputGrowthRateShapeTable,
                                        std::string& OutputGrowthRateCovariateCoeffsTable,
                                        std::string& OutputCarryingCapacityTable,
                                        std::string& OutputCarryingCapacityCovariateCoeffsTable,
                                        std::string& OutputCatchabilityTable,
                                        std::string& OutputCatchabilityCovariateCoeffsTable,
                                        std::string& OutputSurveyQTable,
                                        std::string& OutputSurveyQCovariateCoeffsTable,
                                        std::string& OutputOrForecastBiomassTable)
{
    bool   loadOK;
    bool   isCarryingCapacity = (GrowthForm      == "Logistic");
    bool   isCatchability     = (HarvestFormName ==  nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
                                (HarvestFormName ==  nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString());
    bool   isAggProd          = (CompetitionForm == "AGG-PROD");
    bool   isAlpha            = (CompetitionForm == "NO_K");
    bool   isBetaSpecies      = (CompetitionForm == "MS-PROD");
    bool   isBetaGuilds       = (CompetitionForm == "MS-PROD");
    bool   isBetaGuildsGuilds = (CompetitionForm == "AGG-PROD");
    bool   isPredation        = (PredationForm   == "Type I")  ||
                                (PredationForm   == "Type II") ||
                                (PredationForm   == "Type III");
    bool   isHandling         = (PredationForm   == "Type II") ||
                                (PredationForm   == "Type III");
    bool   isExponent         = (PredationForm   == "Type III");
//  bool   isSurveyQ          = m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName);
    int    m;
    int    NumSpeciesOrGuilds;
    int    NumGuilds;
    int    NumRecords;
    int    timeMinus1;
    int    guildNum;
    int    NumYears = RunLength + 1;
//  double eps = 0;
    double EstBiomassTMinus1 = 0;
//  double logEstBiomassVal = 0;
    double SystemCarryingCapacity = 0;
    double GuildCarryingCapacity;
    double MonteCarloValue; // random value in the range: [val-uncertainty,val+uncertainty]
    double surveyQVal;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string CovariateAlgorithmType = m_DatabasePtr->getCovariateAlgorithmType(m_Logger,m_ProjectName,m_ModelName);
    QStringList SpeciesList;
    QStringList GuildList;
    std::vector<double> EstInitBiomass                     = {};
    std::vector<double> EstGrowthRates                     = {};
    std::vector<double> EstGrowthRateShape                 = {};
    std::vector<double> EstGrowthRateCovariateCoeffs       = {};
    std::vector<double> EstCarryingCapacities              = {};
    std::vector<double> EstCarryingCapacityCovariateCoeffs = {};
    std::vector<double> EstPredationExponent               = {};
    std::vector<double> EstCatchability                    = {};
    std::vector<double> EstCatchabilityCovariateCoeffs     = {};
    std::vector<double> EstSurveyQ                         = {};
    std::vector<double> EstSurveyQCovariateCoeffs          = {};
    std::vector<double> InitBiomassRandomValues            = {};
    std::vector<double> GrowthRateRandomValues             = {};
    std::vector<double> GrowthRateShapeRandomValues        = {};
    std::vector<double> CarryingCapacityRandomValues       = {};
    std::vector<double> CatchabilityRandomValues           = {};
    std::vector<double> CompetitionAlphaRandomValues       = {};
    std::vector<double> CompetitionBetaSpeciesRandomValues = {};
    std::vector<double> CompetitionBetaGuildsRandomValues  = {};
    std::vector<double> CompetitionBetaGuildsGuildsRandomValues = {};
    std::vector<double> PredationRhoRandomValues           = {};
    std::vector<double> PredationExponentRandomValues      = {};
    std::vector<double> PredationHandlingRandomValues      = {};
    std::vector<double> SurveyQRandomValues                = {};
    std::vector<double> GrowthRateCovCoeffRandomValues     = {};
    std::vector<double> CarryingCapacityCovCoeffRandomValues = {};
    std::vector<double> CatchabilityCovCoeffRandomValues   = {};
    std::vector<double> SurveyQCovCoeffRandomValues        = {};

    boost::numeric::ublas::matrix<double> InitBiomassCovariate;
    boost::numeric::ublas::matrix<double> GrowthRatesCovariate;
    boost::numeric::ublas::matrix<double> CarryingCapacitiesCovariate;
    boost::numeric::ublas::matrix<double> CatchabilityCovariate;
    boost::numeric::ublas::matrix<double> PredationRhoCovariate;
    boost::numeric::ublas::matrix<double> PredationHandlingCovariate;
    boost::numeric::ublas::matrix<double> PredationExponentCovariate;
    boost::numeric::ublas::matrix<double> CompetitionAlphaCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildGuildCovariate;
    boost::numeric::ublas::matrix<double> EstCompetitionAlpha;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> EstPredationRho;
    boost::numeric::ublas::matrix<double> EstPredationHandling;
    boost::numeric::ublas::matrix<double> Catch;
    boost::numeric::ublas::matrix<double> Effort;
    boost::numeric::ublas::matrix<double> Exploitation;
    boost::numeric::ublas::matrix<double> EstBiomassSpecies;
    boost::numeric::ublas::matrix<double> EstimatedBiomassByGuilds;
    boost::numeric::ublas::matrix<double> NullMatrix;
    boost::numeric::ublas::matrix<double> ObsBiomassBySpeciesOrGuilds;
    boost::numeric::ublas::matrix<double> SurveyQCovariate;

    QList<QList<double> > BiomassData; // A Vector of row vectors
//  QList<double> InitialBiomass;
    boost::numeric::ublas::vector<double> InitialBiomass;
    double GrowthTerm;
    double HarvestTerm;
    double CompetitionTerm;
    double PredationTerm;
    double randomValue = 0.0;
    std::vector<double> InitBiomassUncertainty;
    std::vector<double> GrowthRateUncertainty;
    std::vector<double> GrowthRateShapeUncertainty;
    std::vector<double> CarryingCapacityUncertainty;
    std::vector<double> PredationUncertainty;
    std::vector<double> CompetitionUncertainty;
    std::vector<double> BetaSpeciesUncertainty;
    std::vector<double> BetaGuildsUncertainty;
    std::vector<double> BetaGuildsGuildsUncertainty;
    std::vector<double> HandlingUncertainty;
    std::vector<double> ExponentUncertainty;
    std::vector<double> CatchabilityUncertainty;
    std::vector<double> SurveyQUncertainty;
    std::vector<double> HarvestUncertainty;
    std::vector<double> GrowthRateCovCoeffUncertainty;
    std::vector<double> CarryingCapacityCovCoeffUncertainty;
    std::vector<double> CatchabilityCovCoeffUncertainty;
    std::vector<double> SurveyQCovCoeffUncertainty;
    std::vector<std::string> OutputTableNames;
    nmfGrowthForm*      growthForm      = new nmfGrowthForm(GrowthForm);
    nmfHarvestForm*     harvestFormPtr  = new nmfHarvestForm(HarvestFormName);
    nmfCompetitionForm* competitionForm = new nmfCompetitionForm(CompetitionForm);
    nmfPredationForm*   predationForm   = new nmfPredationForm(PredationForm);
    std::map<int,std::vector<int> > GuildSpecies;
    std::vector<int>                GuildNum;
    boost::numeric::ublas::matrix<double> ObservedBiomassByGuilds;

    // Read units table
    QString tableHarvestCatch    = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);
    QString tableForecastHarvestCatch = QString::fromStdString(nmfConstantsMSSPM::TableForecastHarvestCatch);
    QString tableSpecies         = QString::fromStdString(nmfConstantsMSSPM::TableSpecies);
    QString tableBiomassAbsolute = QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute);
    QString tableBiomassRelative = QString::fromStdString(nmfConstantsMSSPM::TableBiomassRelative);
    std::map<QString,QString> previousUnits;
    m_DatabasePtr->createUnitsMap(m_ProjectName,m_ModelName,previousUnits);

    BiomassData.clear();
    EstInitBiomass.clear();
    EstGrowthRates.clear();
    EstGrowthRateShape.clear();
    EstGrowthRateCovariateCoeffs.clear();
    EstCarryingCapacities.clear();
    EstCarryingCapacityCovariateCoeffs.clear();
    EstPredationExponent.clear();
    EstSurveyQ.clear();
    EstSurveyQCovariateCoeffs.clear();
    EstCatchability.clear();
    EstCatchabilityCovariateCoeffs.clear();
    SpeciesList.clear();
    GuildList.clear();
    SurveyQRandomValues.clear();
    GrowthRateCovCoeffRandomValues.clear();
    CarryingCapacityCovCoeffRandomValues.clear();
    CatchabilityCovCoeffRandomValues.clear();
    SurveyQCovCoeffRandomValues.clear();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::updateOutputBiomassTable");

    // Find Guilds and Species
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList)) {
        return false;
    }
    if (isAggProd) {
        SpeciesList        = GuildList;
        NumSpeciesOrGuilds = NumGuilds;
        ObsBiomassBySpeciesOrGuilds = m_DataStruct.ObservedBiomassByGuilds;
    } else {
        ObsBiomassBySpeciesOrGuilds = m_DataStruct.ObservedBiomassBySpecies;
        if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList)) {
            return false;
        }
        if (ObsBiomassBySpeciesOrGuilds.size2() == 0) {
            QMessageBox::information(this, tr("Warning"), tr("\nNo recent run found. Please run an Estimation prior to running a Forecast."),QMessageBox::Ok);
            return false;
        }
    }
    std::vector<double> HarvestRandomValues(NumSpeciesOrGuilds,0.0);

    // Load uncertainty factors if this is a monte carlo simulation
    loadOK = loadUncertaintyData(isMonteCarlo,NumSpeciesOrGuilds,ForecastName,
                                 Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                 InitBiomassUncertainty,
                                 GrowthRateUncertainty,
                                 GrowthRateShapeUncertainty,
                                 CarryingCapacityUncertainty,
                                 PredationUncertainty,
                                 CompetitionUncertainty,
                                 BetaSpeciesUncertainty,
                                 BetaGuildsUncertainty,
                                 BetaGuildsGuildsUncertainty,
                                 HandlingUncertainty,
                                 ExponentUncertainty,
                                 CatchabilityUncertainty,
                                 SurveyQUncertainty,
                                 HarvestUncertainty,
                                 GrowthRateCovCoeffUncertainty,
                                 CarryingCapacityCovCoeffUncertainty,
                                 CatchabilityCovCoeffUncertainty,
                                 SurveyQCovCoeffUncertainty);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error, "[Error] nmfMainWindow::updateOutputBiomassTable: " + errorMsg);
        return false;
    }

    if (ForecastName.empty()) {
        checkDiagnosticAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    } else {
        checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    }

    // Load appropriate r and K (for given Algorithm, Minimizer, and Objective Criterion)
    OutputTableNames.clear();
    OutputTableNames.push_back(OutputInitBiomassTable);
    OutputTableNames.push_back(OutputGrowthRateTable);
    OutputTableNames.push_back(OutputGrowthRateShapeTable);
    OutputTableNames.push_back(OutputGrowthRateCovariateCoeffsTable);
    OutputTableNames.push_back(OutputCarryingCapacityTable);
    OutputTableNames.push_back(OutputCarryingCapacityCovariateCoeffsTable);
//  if (isCatchability) {
    OutputTableNames.push_back(OutputCatchabilityTable);
    OutputTableNames.push_back(OutputCatchabilityCovariateCoeffsTable);
//  }

    if (isExponent) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputPredationExponent);
    }

    // Add SurveyQ
    OutputTableNames.push_back(OutputSurveyQTable);
    OutputTableNames.push_back(OutputSurveyQCovariateCoeffsTable);

    // Read output table data
    for (unsigned j=0; j<OutputTableNames.size(); ++j) {
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                    OutputTableNames[j] +
                    " WHERE ProjectName = '"       + m_ProjectName +
                    "' AND ModelName = '"          + m_ModelName +
                    "' AND Algorithm = '"          + Algorithm +
                    "' AND Minimizer = '"          + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling = '"            + Scaling +
                    "' AND isAggProd = "           + isAggProdStr +
                    " ORDER BY SpeName";
        dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords != NumSpeciesOrGuilds) {
            errorMsg  = "Run failed. Incorrect number of records found in " + OutputTableNames[j] + ". ";
            errorMsg += "Found " + std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds) + ".";
            errorMsg += "\n\nPlease ensure an Estimation has been run.";
            m_Logger->logMsg(nmfConstants::Error, "[Error 4] " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            errorMsg  = "\n" + errorMsg + "\n";
            QMessageBox::information(this, tr("Run Failed"), tr(errorMsg.c_str()),QMessageBox::Ok);
            return false;
        }

        if (OutputTableNames[j] == OutputInitBiomassTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(InitBiomassUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstInitBiomass.push_back(MonteCarloValue);
                InitBiomassRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputGrowthRateTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(GrowthRateUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstGrowthRates.push_back(MonteCarloValue);
                GrowthRateRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputGrowthRateShapeTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(GrowthRateShapeUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstGrowthRateShape.push_back(MonteCarloValue);
                GrowthRateShapeRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputGrowthRateCovariateCoeffsTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(GrowthRateCovCoeffUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstGrowthRateCovariateCoeffs.push_back(MonteCarloValue);
                GrowthRateCovCoeffRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputCarryingCapacityTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                if (isCarryingCapacity) {
                    MonteCarloValue = calculateMonteCarloValue(CarryingCapacityUncertainty[i],
                                                               std::stod(dataMap["Value"][i]),
                                                               randomValue);
                    EstCarryingCapacities.push_back(MonteCarloValue);
                    CarryingCapacityRandomValues.push_back(randomValue);
                } else {
                    EstCarryingCapacities.push_back(0);
                    CarryingCapacityRandomValues.push_back(0);
                }
            }
        } else if (OutputTableNames[j] == OutputCarryingCapacityCovariateCoeffsTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(CarryingCapacityCovCoeffUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstCarryingCapacityCovariateCoeffs.push_back(MonteCarloValue);
                CarryingCapacityCovCoeffRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputCatchabilityTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(CatchabilityUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstCatchability.push_back(MonteCarloValue);
                CatchabilityRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputCatchabilityCovariateCoeffsTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(CatchabilityCovCoeffUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstCatchabilityCovariateCoeffs.push_back(MonteCarloValue);
                CatchabilityCovCoeffRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputSurveyQTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(SurveyQUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstSurveyQ.push_back(MonteCarloValue);
                SurveyQRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == OutputSurveyQCovariateCoeffsTable) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(SurveyQCovCoeffUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstSurveyQCovariateCoeffs.push_back(MonteCarloValue);
                SurveyQCovCoeffRandomValues.push_back(randomValue);
            }
        } else if (OutputTableNames[j] == nmfConstantsMSSPM::TableOutputPredationExponent) {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                MonteCarloValue = calculateMonteCarloValue(ExponentUncertainty[i],
                                                           std::stod(dataMap["Value"][i]),
                                                           randomValue);
                EstPredationExponent.push_back(MonteCarloValue);
                PredationExponentRandomValues.push_back(randomValue);
            }
        }
    }

    // Load data from OutputCompetitionAlpha, OutputCompetitionBetaSpecies,
    // OutputPredationRho, and OutputPredationHandling
    OutputTableNames.clear();
    if (isAlpha) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionAlpha);
    }
    if (isBetaSpecies) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies);
    }
    if (isPredation) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputPredationRho);
    }
    if (isHandling) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputPredationHandling);
    }

    nmfUtils::initialize(EstCompetitionAlpha,      NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(EstCompetitionBetaSpecies,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(EstPredationRho,          NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(EstPredationHandling,     NumSpeciesOrGuilds,NumSpeciesOrGuilds);

    for (unsigned i=0; i<OutputTableNames.size(); ++i) {

        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " +
                     OutputTableNames[i] +
                    " WHERE ProjectName = '"       + m_ProjectName +
                    "' AND ModelName = '"          + m_ModelName +
                    "' AND Algorithm = '"          + Algorithm +
                    "' AND Minimizer = '"          + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling = '"            + Scaling +
                    "' AND isAggProd = "           + isAggProdStr +
                    "  ORDER BY SpeciesA,SpeciesB";
        dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeciesA"].size();
        if (NumRecords != NumSpeciesOrGuilds*NumSpeciesOrGuilds) {
            m_Logger->logMsg(nmfConstants::Error,
                           "[Error 5] UpdateOutputBiomassTable: Incorrect number of records found in " + OutputTableNames[i] + ". Found " +
                           std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds*NumSpeciesOrGuilds) + ".");
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return false;
        }
        // In row major order
        m = 0;
        for (int row=0; row<NumSpeciesOrGuilds; ++row) {
            for (int col=0; col<NumSpeciesOrGuilds; ++col) {
                if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputCompetitionAlpha) {
                    EstCompetitionAlpha(row,col) = calculateMonteCarloValue(
                                CompetitionUncertainty[col],
                                std::stod(dataMap["Value"][m]),
                                randomValue);
                    CompetitionAlphaRandomValues.push_back(randomValue);
                } else if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies) {
                    EstCompetitionBetaSpecies(row,col) = calculateMonteCarloValue(
                                BetaSpeciesUncertainty[col],
                                std::stod(dataMap["Value"][m]),
                                randomValue);
                    CompetitionBetaSpeciesRandomValues.push_back(randomValue);
                } else if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputPredationRho) {
                    EstPredationRho(row,col) = calculateMonteCarloValue(
                                PredationUncertainty[col],
                                std::stod(dataMap["Value"][m]),
                                randomValue);
                    PredationRhoRandomValues.push_back(randomValue);
                } else if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputPredationHandling) {
                    EstPredationHandling(row,col) = calculateMonteCarloValue(
                                HandlingUncertainty[col],
                                std::stod(dataMap["Value"][m]),
                                randomValue);
                    PredationHandlingRandomValues.push_back(randomValue);
                }
                ++m;
            }
        }
    }

    OutputTableNames.clear();
    if (isBetaGuilds) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds);
    }
    nmfUtils::initialize(EstCompetitionBetaGuilds,NumSpeciesOrGuilds,NumGuilds);
    for (unsigned i=0; i<OutputTableNames.size(); ++i) {
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Guild","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild,Value FROM " +
                     OutputTableNames[i] +
                    " WHERE ProjectName = '"       + m_ProjectName +
                    "' AND ModelName = '"          + m_ModelName +
                    "' AND Algorithm = '"          + Algorithm +
                    "' AND Minimizer = '"          + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling = '"            + Scaling +
                    "' AND isAggProd = "           + isAggProdStr +
                    " ORDER BY SpeName,Guild";
        dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords != NumSpeciesOrGuilds*NumGuilds) {
            m_Logger->logMsg(nmfConstants::Error,
                           "[Error 6] UpdateOutputBiomassTable: Incorrect number of records found in " + OutputTableNames[i] + ". Found " +
                           std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds*NumGuilds) + ".");
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return false;
        }
        // In row major order
        m = 0;
        for (int col=0; col<NumGuilds; ++col) {
            for (int row=0; row<NumSpeciesOrGuilds; ++row) {
                if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) {
                    EstCompetitionBetaGuilds(row,col) = calculateMonteCarloValue(
                            BetaGuildsUncertainty[row],
                            std::stod(dataMap["Value"][m]),
                            randomValue);
                   CompetitionBetaGuildsRandomValues.push_back(randomValue);
                }
                ++m;
            }
        }
    }

    OutputTableNames.clear();
    if (isBetaGuildsGuilds) {
        OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaGuildsGuilds);
    }
    nmfUtils::initialize(EstCompetitionBetaGuildsGuilds,NumGuilds,NumGuilds);
    for (unsigned i=0; i<OutputTableNames.size(); ++i) {
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","GuildA","GuildB","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,GuildA,GuildB,Value FROM " +
                     OutputTableNames[i] +
                    " WHERE ProjectName = '"       + m_ProjectName +
                    "' AND ModelName = '"          + m_ModelName +
                    "' AND Algorithm = '"          + Algorithm +
                    "' AND Minimizer = '"          + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling = '"            + Scaling +
                    "' AND isAggProd = "           + isAggProdStr +
                    " ORDER BY GuildA,GuildB";
        dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["GuildA"].size();
        if (NumRecords != NumGuilds*NumGuilds) {
            m_Logger->logMsg(nmfConstants::Error,
                           "[Error 6.1] UpdateOutputBiomassTable: Incorrect number of records found in " + OutputTableNames[i] + ". Found " +
                           std::to_string(NumRecords) + " expecting " + std::to_string(NumGuilds*NumGuilds) + ".");
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return false;
        }
        // In row major order
        m = 0;
        for (int col=0; col<NumGuilds; ++col) {
            for (int row=0; row<NumGuilds; ++row) {
                if (OutputTableNames[i] == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) {
                    EstCompetitionBetaGuildsGuilds(row,col) = calculateMonteCarloValue(
                            BetaGuildsGuildsUncertainty[row],
                            std::stod(dataMap["Value"][m]),
                            randomValue);
                   CompetitionBetaGuildsGuildsRandomValues.push_back(randomValue);
                }
                ++m;
            }
        }
    }

    // Load the appropriate Harvest tables
    if (HarvestFormName == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        if (! loadHarvestCatchTables(isAggProd,isMonteCarlo,ForecastName,HarvestFormName,
                                     harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
                                     previousUnits,NumSpeciesOrGuilds,RunLength,Catch)) {
            return false;
        }
    } else if (HarvestFormName == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        if (! loadHarvestEffortTables(isAggProd,isMonteCarlo,ForecastName,HarvestFormName,
                                      harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
                                      previousUnits,NumSpeciesOrGuilds,RunLength,Effort)) {
            return false;
        }
    } else if (HarvestFormName == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        if (! loadHarvestEffortFitToCatchTables(isAggProd,isMonteCarlo,ForecastName,HarvestFormName,
                                                harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
                                                previousUnits,NumSpeciesOrGuilds,RunLength,Catch,Effort)) {
            return false;
        }
        harvestFormPtr->setType(nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString());
    } else if (HarvestFormName == "Exploitation (F)") {
        if (! loadHarvestExploitationTables(isAggProd,isMonteCarlo,ForecastName,HarvestFormName,
                                            harvestFormPtr,HarvestRandomValues,HarvestUncertainty,
                                            previousUnits,NumSpeciesOrGuilds,RunLength,Exploitation)) {
            return false;
        }
    }

    // Update the Forecast Monte Carlo Parameters table
    if (! ForecastName.empty()) {
        if (! m_DatabasePtr->updateForecastMonteCarloParameters(
                    0,m_Logger,m_ProjectName,m_ModelName,
                    ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                    SpeciesList,RunNum,
                    GrowthRateRandomValues,
                    GrowthRateShapeRandomValues,
                    CarryingCapacityRandomValues,CatchabilityRandomValues,
                    PredationExponentRandomValues,CompetitionAlphaRandomValues,
                    CompetitionBetaSpeciesRandomValues,CompetitionBetaGuildsRandomValues,
                    CompetitionBetaGuildsGuildsRandomValues,
                    PredationRhoRandomValues,PredationHandlingRandomValues,HarvestRandomValues,
                    GrowthRateCovCoeffRandomValues,CarryingCapacityCovCoeffRandomValues,
                    CatchabilityCovCoeffRandomValues,SurveyQCovCoeffRandomValues)) {
            return false;
        }
    }

    // If not  running a forecast, initial biomass is the first observed biomass.
    // else if running a forecast, initial biomass is the last  observed biomass.
    if (ForecastName == "") {
        if (! getInitialObservedBiomass(InitialBiomass)) {
            return false;
        }
    } else {
        if (! getFinalEstimatedBiomass(InitialBiomass)) {
            std::cout << "Error: getFinalObservedBiomass" << std::endl;
            return false;
        }
    }
    if (previousUnits.find(tableSpecies) != previousUnits.end()) {
        nmfUtilsQt::convertVector(InitialBiomass, previousUnits[tableSpecies],"mt");
    }

    // Check to see if no species names have been loaded. If they haven't, load them now. This is probably
    // due to the user running a Forecast without first running an Estimation.
    if (m_DataStruct.SpeciesNames.size() == 0) {
        loadParametersMisc(m_DataStruct);
    }

    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"InitBiomass",                   InitBiomassCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"GrowthRate",                    GrowthRatesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CarryingCapacity",              CarryingCapacitiesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"Catchability",                  CatchabilityCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"SurveyQ",                       SurveyQCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationRho",                  PredationRhoCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationHandling",             PredationHandlingCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationExponent",             PredationExponentCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionAlpha",              CompetitionAlphaCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaSpeciesSpecies", CompetitionBetaSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildSpecies",   CompetitionBetaGuildSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildGuild",     CompetitionBetaGuildGuildCovariate);

    // Since we may be estimating SurveyQ, need to divide the Observed Biomass by the SurveyQ
    double surveyQTerm;
    for (int species=0; species<NumSpeciesOrGuilds; ++species) {
        surveyQVal  = EstSurveyQ[species];
        for (int time=0; time<NumYears; ++time) {
            if (ObsBiomassBySpeciesOrGuilds(time,species) != nmfConstantsMSSPM::NoData) {
                surveyQTerm = nmfUtils::applyCovariate(nullptr,
                                                       m_DataStruct.CovariateAlgorithmType,surveyQVal,
                                                       EstSurveyQCovariateCoeffs[species],SurveyQCovariate(time,species));
                ObsBiomassBySpeciesOrGuilds(time,species) /= surveyQTerm;
            }
        }
    }

    // Use estimated initial biomass if it was checked to be estimated
    nmfUtils::initialize(EstBiomassSpecies,NumYears,NumSpeciesOrGuilds);
    if (! setFirstRowEstimatedBiomass(ForecastName,NumSpeciesOrGuilds,InitialBiomass,
                                      InitBiomassCovariate,EstBiomassSpecies)) {
        return false;
    }

    // Get guild map
    nmfUtils::initialize(EstimatedBiomassByGuilds,NumYears,NumGuilds);
    if (! m_DatabasePtr->getGuildData(m_Logger,NumGuilds,RunLength,GuildList,GuildSpecies,
                                      GuildNum,ObservedBiomassByGuilds)) {
        return false;
    }
    if (m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName)) {
        if (previousUnits.find(tableBiomassRelative) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(ObservedBiomassByGuilds, previousUnits[tableBiomassRelative],"mt", nmfConstantsMSSPM::DontConvertBlanks);
        }
    } else {
        if (previousUnits.find(tableBiomassAbsolute) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(ObservedBiomassByGuilds, previousUnits[tableBiomassAbsolute],"mt", nmfConstantsMSSPM::DontConvertBlanks);
        }
    }
    EstimatedBiomassByGuilds = ObservedBiomassByGuilds;

    if (isAggProd) {
        for (int i=0; i<NumGuilds; ++i) {
            SystemCarryingCapacity += EstCarryingCapacities[i];
        }
    } else {
        for (int i=0; i<NumGuilds; ++i) {
            for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                SystemCarryingCapacity += EstCarryingCapacities[GuildSpecies[i][j]];
            }
        }
    }

//    std::vector<double> sigmasSquared = nmfUtilsStatistics::calculateSigmasSquared(ObsBiomassBySpeciesOrGuilds);
//    if (sigmasSquared.size() == 0) {
//        return false;
//    }

    harvestFormPtr->modifyHarvestTypeDueToFitToCatch(Forecast_Tab2_ptr->getHarvestType().toStdString());
    for (int time=1; time<NumYears; time++) {
        timeMinus1 = time-1;
        for (int species=0; species<NumSpeciesOrGuilds; ++species)
        {
            // Find the guild carrying capacity for guild: guildNum
            GuildCarryingCapacity = 0;
            if (isAggProd) {
                GuildCarryingCapacity = EstCarryingCapacities[species];
            } else {
                guildNum = GuildNum[species];
                for (unsigned j=0; j<GuildSpecies[guildNum].size(); ++j) {
                    GuildCarryingCapacity += EstCarryingCapacities[GuildSpecies[guildNum][j]];
                }
            }

            EstBiomassTMinus1  = EstBiomassSpecies(timeMinus1,species);

            GrowthTerm      = growthForm->evaluate(CovariateAlgorithmType,
                                                   EstBiomassTMinus1,
                                                   EstGrowthRates[species],
                                                   EstGrowthRateShape[species],
                                                   EstGrowthRateCovariateCoeffs[species],
                                                   GrowthRatesCovariate(timeMinus1,species),
                                                   EstCarryingCapacities[species],
                                                   EstCarryingCapacityCovariateCoeffs[species],
                                                   CarryingCapacitiesCovariate(timeMinus1,species));

             HarvestTerm     = harvestFormPtr->evaluate(CovariateAlgorithmType,
                                                    timeMinus1,species,
                                                    EstBiomassTMinus1,
                                                    Catch,Effort,Exploitation,
                                                    EstCatchability[species],
                                                    EstCatchabilityCovariateCoeffs[species],
                                                    CatchabilityCovariate(timeMinus1,species));
            CompetitionTerm = competitionForm->evaluate(CovariateAlgorithmType,
                                                        timeMinus1, species,
                                                        EstBiomassTMinus1,
                                                        EstGrowthRates,
                                                        GrowthRatesCovariate,
                                                        GuildCarryingCapacity,
                                                        SystemCarryingCapacity,
                                                        EstBiomassSpecies,
                                                        EstimatedBiomassByGuilds,
                                                        EstCompetitionAlpha,
                                                        CompetitionAlphaCovariate,
                                                        EstCompetitionBetaSpecies,
                                                        CompetitionBetaSpeciesCovariate,
                                                        EstCompetitionBetaGuilds,
                                                        CompetitionBetaGuildSpeciesCovariate,
                                                        EstCompetitionBetaGuildsGuilds,
                                                        CompetitionBetaGuildGuildCovariate);
            PredationTerm   = predationForm->evaluate(CovariateAlgorithmType,
                                                      timeMinus1, species,
                                                      EstBiomassSpecies,
                                                      EstBiomassTMinus1,
                                                      EstPredationRho,
                                                      PredationRhoCovariate,
                                                      EstPredationHandling,
                                                      PredationHandlingCovariate,
                                                      EstPredationExponent,
                                                      PredationExponentCovariate);

//nmfUtils::printMatrix("EstPredationRho",EstPredationRho,10,10);


            // Assume log normal error (eps = error term)
//            eps = std::log(ObsBiomassBySpeciesOrGuilds(timeMinus1,species)) - std::log(EstBiomassTMinus1);
//            logEstBiomassVal = std::log(
//                        EstBiomassTMinus1 + GrowthTerm
//                        - HarvestTerm - CompetitionTerm - PredationTerm);
//                       // + eps;
//            // This next assignment statement is needed in order to properly calculate eps on subsequent loop iterations
//            EstBiomassTMinus1 = std::exp(logEstBiomassVal); // + 0.5*sigmasSquared[species]);
//            EstBiomassSpecies(time,species) = EstBiomassTMinus1;

            EstBiomassTMinus1 += GrowthTerm - HarvestTerm - CompetitionTerm - PredationTerm;

            if ( std::isnan(std::fabs(EstBiomassTMinus1)) || (EstBiomassTMinus1 < 0) ) {
                EstBiomassTMinus1 = m_DataStruct.MinimumBiomass[species];
            }
            EstBiomassSpecies(time,species) = EstBiomassTMinus1;


            // update estBiomassGuilds for next time step
            if (isAggProd) {
                for (int i=0; i<NumGuilds; ++i) {
                    EstimatedBiomassByGuilds(time,i) = EstBiomassSpecies(time,i);
                }
            } else {
                for (int i=0; i<NumGuilds; ++i) {
                    for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                        EstimatedBiomassByGuilds(time,i) += EstBiomassSpecies(time,GuildSpecies[i][j]);
                    }
                }
            }

            EstBiomassSpecies(time,species) = nmfUtils::round(EstBiomassSpecies(time,species),6);

        } // end for all species
    } // end for all years

    // Howard's suggestion regarding adding another loop for the stats calculations
//    std::vector<double> sigmasSquared = nmfUtilsStatistics::calculateSigmasSquared(EstBiomassSpecies);
//    if (sigmasSquared.size() == 0) {
//        return false;
//    }
//    for (int i=0; i<(int)EstBiomassSpecies.size1(); ++i) {
//        for (int j=0; j<(int)EstBiomassSpecies.size2(); ++j) {
//            eps = std::log(ObsBiomassBySpeciesOrGuilds(i,j)) - std::log(EstBiomassSpecies(i,j));
//            logEstBiomassVal = std::log(EstBiomassSpecies(i,j)) + eps;
//            EstBiomassSpecies(i,j) = std::exp(logEstBiomassVal + 0.5*sigmasSquared[j]);
//        }
//    }

    m = 0;
    if (ForecastName == "") {
        cmd = "REPLACE INTO " +
               OutputOrForecastBiomassTable +
              " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value) VALUES ";
        for (int species=0; species<NumSpeciesOrGuilds; ++ species) { // Species
            for (int time=StartYear; time<=RunLength; ++time) { // Time in years
                if (std::isnan(EstBiomassSpecies(time,species))) {
                    EstBiomassSpecies(time,species) = -1;
                }
                cmd += "('"   + m_ProjectName +
                        "','" + m_ModelName +
                        "','" + Algorithm +
                        "','" + Minimizer +
                        "','" + ObjectiveCriterion +
                        "','" + Scaling +
                        "',"  + isAggProdStr +
                        ",'"  + SpeciesList[species].toStdString() +
                        "',"  + std::to_string(time) +
                        ","   + QString::number(EstBiomassSpecies(time,species),'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
            }
        }
    } else {
        if (isMonteCarlo) {
            checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
            cmd = "INSERT INTO " + OutputOrForecastBiomassTable +
                  " (ProjectName,ModelName,ForecastName,RunNum,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value) VALUES ";
            for (int species=0; species<NumSpeciesOrGuilds; ++ species) { // Species
                for (int time=0; time<=RunLength; ++time) { // Time in years
                    if (std::isnan(EstBiomassSpecies(time,species)) ||
                        std::isinf(EstBiomassSpecies(time,species)) ||
                        EstBiomassSpecies(time,species) > nmfConstants::MaxBiomass) {
                        EstBiomassSpecies(time,species) = -1;
                    }
                    cmd += "('"   + m_ProjectName +
                            "','" + m_ModelName +
                            "','" + ForecastName +
                            "',"  + std::to_string(RunNum) +
                            ",'"  + Algorithm +
                            "','" + Minimizer +
                            "','" + ObjectiveCriterion +
                            "','" + Scaling +
                            "',"  + isAggProdStr +
                            ",'"  + SpeciesList[species].toStdString() +
                            "',"  + std::to_string(time) +
                            ","   + QString::number(EstBiomassSpecies(time,species),'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
                }
            }
        } else {
            cmd = "INSERT INTO " + OutputOrForecastBiomassTable +
                  " (ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value) VALUES ";
            for (int species=0; species<NumSpeciesOrGuilds; ++ species) { // Species
                for (int time=0; time<=RunLength; ++time) { // Time in years
                    if (std::isnan(EstBiomassSpecies(time,species)) ||
                        std::isinf(EstBiomassSpecies(time,species)) ||
                        EstBiomassSpecies(time,species) > nmfConstants::MaxBiomass) {
                        EstBiomassSpecies(time,species) = -1;
                    }
                    cmd += "('"   + m_ProjectName +
                            "','" + m_ModelName +
                            "','" + ForecastName +
                            "','" + Algorithm +
                            "','" + Minimizer +
                            "','" + ObjectiveCriterion +
                            "','" + Scaling +
                            "',"  + isAggProdStr +
                            ",'"  + SpeciesList[species].toStdString() +
                            "',"  + std::to_string(time) +
                            ","   + QString::number(EstBiomassSpecies(time,species),'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
                }
            }
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 8] UpdateOutputBiomassTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

    return true;
}

void
nmfMainWindow::callback_ResetFilterButtons()
{
    m_isPressedBeesButton     = false;
    m_isPressedNLoptButton    = false;
    m_isPressedGeneticButton  = false;
    m_isPressedGradientButton = false;

    m_UI->actionBeesTB->setChecked(false);
    m_UI->actionNLoptTB->setChecked(false);
    m_UI->actionGeneticTB->setChecked(false);
    m_UI->actionGradientTB->setChecked(false);
}

void
nmfMainWindow::callback_RestoreOutputSpecies()
{
    Output_Controls_ptr->setOutputSpecies(Estimation_Tab1_ptr->getOutputSpecies());
}

void
nmfMainWindow::callback_EnableFilterButtons(bool state)
{
    m_UI->actionBeesTB->setEnabled(state);
    m_UI->actionNLoptTB->setEnabled(state);
    m_UI->actionGeneticTB->setEnabled(state);
    m_UI->actionGradientTB->setEnabled(state);
    m_UI->actionShowAllSavedRunsTB->setEnabled(false);
    m_UI->actionSaveAndShowCurrentRun->trigger();
}

bool
nmfMainWindow::isAtLeastOneFilterPressed()
{
  return (m_isPressedBeesButton ||
          m_isPressedNLoptButton);
}

std::string
nmfMainWindow::getFilterButtonsResult()
{
    std::string queryStr = "";
    std::string conj = "";

    queryStr += " WHERE ProjectName = '" + m_ProjectName +
                "' AND  ModelName = '"   + m_ModelName   + "' ";

    if (m_isPressedBeesButton || m_isPressedNLoptButton) {
        queryStr += " AND (";
    }
    if (m_isPressedBeesButton) {
        queryStr += " Algorithm = 'Bees Algorithm' ";
        conj = "OR";
    }
    if (m_isPressedNLoptButton) {
        queryStr += conj + " Algorithm = 'NLopt Algorithm' ";
        conj = "OR";
    }
    if (m_isPressedBeesButton || m_isPressedNLoptButton) {
        queryStr += ")";
    }

    if (m_isPressedBeesButton || m_isPressedNLoptButton) {
        queryStr += " AND isAggProd=0 ";
    }

    return queryStr;
}


double
nmfMainWindow::convertUnitsStringToValue(QString& ScaleStr)
{
    double ScaleVal = 1.0;

    if (ScaleStr == "000") {
        ScaleVal = 1000.0;
        ScaleStr = "10³ ";
    } else if (ScaleStr == "000 000") {
        ScaleVal = 1000000.0;
        ScaleStr = "10⁶ ";
    } else if (ScaleStr == "000 000 000") {
        ScaleVal = 1000000000.0;
        ScaleStr = "10⁹ ";
    }
    ScaleStr += " ";
    if (ScaleStr == "Default ")
        ScaleStr = "";

    return ScaleVal;
}


bool
nmfMainWindow::callback_ShowChart(QString OutputType,
                                  QString OutputSpecies)
{
    bool isAlpha;
    bool isMsProd;
    bool isRho;
    bool isHandling;
    bool isAggProd;
    bool isExponent;
    bool isAveraged = isAMultiOrMohnsRhoRun();
    int m;
    int ii=0;
    int SpeciesNum;
    int NumSpeciesOrGuilds;
    int NumGuilds;
    int NumRecords;
    int RunLength;
    int StartYear;
    int eeFactor;
    int NumLines = getNumLines();
    double ScaleVal = 1.0;
    double val = 0.0;
    double YMinVal = Output_Controls_ptr->getYMinSliderVal();
    double YMaxVal = nmfConstants::NoValueDouble;
    std::vector<std::string> fields;
    std::string queryStr;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string msg;
    std::string DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    std::string Algorithm          = DefaultAveragingAlgorithm;
    std::string Minimizer          = DefaultAveragingAlgorithm;
    std::string ObjectiveCriterion = DefaultAveragingAlgorithm;
    std::string Scaling            = DefaultAveragingAlgorithm;
    std::string CompetitionForm;
    std::string HarvestForm;
    std::string PredationForm;
    std::string isAggProdStr = "";
    std::string ObsBiomassType = "";
    std::vector<std::string> Algorithms        = {DefaultAveragingAlgorithm};
    std::vector<std::string> Minimizers        = {DefaultAveragingAlgorithm};
    std::vector<std::string> ObjectiveCriteria = {DefaultAveragingAlgorithm};
    std::vector<std::string> Scalings          = {DefaultAveragingAlgorithm};
    QList<double> BMSYValues;
    QList<double> MSYValues;
    QList<double> FMSYValues;
    boost::numeric::ublas::matrix<double> HarvestData;
    boost::numeric::ublas::matrix<double> ObservedBiomass;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomass;
    QStringList hLabels;
    QStringList yearLabels;
    QStringList SpeciesList;
    QStringList GuildList;
    QString ScaleStr = Output_Controls_ptr->getOutputScale();
    QStandardItem* item;
    QStandardItemModel* smodel;
    QString OutputMethod    = Output_Controls_ptr->getOutputDiagnostics();
    QString OutputChartType = Output_Controls_ptr->getOutputChartType();
    std::vector<boost::numeric::ublas::matrix<double> > HarvestVec;
    boost::numeric::ublas::matrix<double> NullMatrix;
    std::vector<double> EstCatchability;
    QLocale locale(QLocale::English);
    QString valueWithComma;
    std::map<QString,QString> previousUnits;
    QString tableBiomassAbsolute = QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute);
    QString tableBiomassRelative = QString::fromStdString(nmfConstantsMSSPM::TableBiomassRelative);
    QString tableHarvestCatch    = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);
    QString BiomassTable = (m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName)) ?
                            tableBiomassRelative : tableBiomassAbsolute;

    clearChart();

    m_DatabasePtr->createUnitsMap(m_ProjectName,m_ModelName,previousUnits);

    if (isAMultiOrMohnsRhoRun() && OutputType.isEmpty() &&
        (OutputChartType == nmfConstantsMSSPM::OutputChartBiomass)) {
        displayAverageBiomass();
        return true;
    }

    if (! isAveraged) {
        m_DatabasePtr->getAlgorithmIdentifiers(
                    this,m_Logger,m_ProjectName,m_ModelName,
                    Algorithm,Minimizer,ObjectiveCriterion,
                    Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    }

    // Get Models data
    fields     = {"ObsBiomassType","RunLength","StartYear","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr   = "SELECT ObsBiomassType,RunLength,StartYear,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm FROM " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName='" + m_ProjectName +
                 "' AND ModelName = '"  + m_ModelName   + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["RunLength"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfMainWindow::showChart: No records found in table Models for Name = "+m_ModelName);
        return false;
    }
    RunLength       = std::stoi(dataMap["RunLength"][0]);
    CompetitionForm = dataMap["WithinGuildCompetitionForm"][0];
    HarvestForm     = dataMap["HarvestForm"][0];
    PredationForm   = dataMap["PredationForm"][0];
    StartYear       = std::stoi(dataMap["StartYear"][0]);
    isAggProd       = (CompetitionForm == "AGG-PROD");
    isMsProd        = (CompetitionForm == "MS-PROD");
    isAlpha         = (CompetitionForm == "NO_K");
    isRho           = (PredationForm != "Null");
    isHandling      = (PredationForm == "Type II") || (PredationForm == "Type III");
    isExponent      = (PredationForm == "Type III");
    isAggProdStr    = (isAggProd) ? "1" : "0";
    ObsBiomassType  = dataMap["ObsBiomassType"][0];
    ObsBiomassType  = (ObsBiomassType.empty() || ObsBiomassType == "Absolute") ?
                nmfConstantsMSSPM::TableBiomassAbsolute :
                nmfConstantsMSSPM::TableBiomassRelativeDividedByEstSurveyQ;
//qDebug() << "Model: " << QString::fromStdString(queryStr);
    if (OutputType.isEmpty()) {
        OutputType = Output_Controls_ptr->getOutputChartType();
    }
    if (OutputSpecies.isEmpty()) {
        OutputSpecies = Output_Controls_ptr->getOutputSpecies();
    }

    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList)) {
        return false;
    }

    if (isAggProd) {
        NumSpeciesOrGuilds  = NumGuilds;
        SpeciesList = GuildList;
        SpeciesNum  = Output_Controls_ptr->getOutputSpeciesIndex();
    } else {
        if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList))
            return false;
        //SpeciesNum = SpeciesHash[OutputSpecies];
        SpeciesNum = Output_Controls_ptr->getSpeciesNumFromName(OutputSpecies);
    }

    // Load various tables
    std::vector<std::string> OutputTableNames = nmfConstantsMSSPM::OutputVectorTables;
    QList<QTableView*> OutputTableViews  = {InitBiomassTV,GrowthRateTV,GrowthRateShapeTV,
                                            CarryingCapacityTV,CatchabilityTV,PredationExponentTV,
                                            SurveyQTV,BMSYTV,MSYTV,FMSYTV};

    if (! isAMohnsRhoMultiRun())
    {
    // Load 1d tables
    m = 0;
    std::string outputTableName;
    std::string outputTableName2;
    int NumOutputVectorTables = (int)nmfConstantsMSSPM::OutputVectorTables.size();
    for (int i=0; i<NumOutputVectorTables; ++i) {
        outputTableName = nmfConstantsMSSPM::OutputVectorTables[i];
        hLabels.clear();
        smodel = new QStandardItemModel( NumSpeciesOrGuilds, 1 );
        if ((NumLines == 1) && (! isAtLeastOneFilterPressed()))  {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                          outputTableName +
                         " WHERE ProjectName = '"       + m_ProjectName +
                         "' AND ModelName = '"          + m_ModelName +
                         "' AND Algorithm = '"          + Algorithm +
                         "' AND Minimizer = '"          + Minimizer +
                         "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                         "' AND Scaling = '"            + Scaling +
                         "' AND isAggProd = "           + isAggProdStr +
                         " ORDER by SpeName";
        } else {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                          outputTableName;
            queryStr  += getFilterButtonsResult();
            queryStr  += " ORDER by Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName";
        }
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords == 0) {
            msg = "[nmfMainWindow::callback_ShowChart] No data found in: " + outputTableName + " for current configuration. ";
            msg += "Please run an Estimation.";
            m_Logger->logMsg(nmfConstants::Normal, msg);
            return false;
        }

        EstCatchability.clear();
        for (int line=0; line<NumLines; ++line) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                val = std::stod(dataMap["Value"][j]);
                eeFactor = (outputTableName == nmfConstantsMSSPM::TableOutputSurveyQ) ? -1 : 1;
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            val,m_NumSignificantDigits,eeFactor*6);
                item = new QStandardItem(valueWithComma);
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(j, 0, item);
                if (outputTableName == nmfConstantsMSSPM::TableOutputMSYBiomass) {
                    BMSYValues.append(std::stod(dataMap["Value"][j+line*NumSpeciesOrGuilds]));
                } else if (outputTableName == nmfConstantsMSSPM::TableOutputMSY) {
                    MSYValues.append(std::stod(dataMap["Value"][j+line*NumSpeciesOrGuilds]));
                } else if (outputTableName == nmfConstantsMSSPM::TableOutputMSYFishing) {
                    FMSYValues.append(std::stod(dataMap["Value"][j+line*NumSpeciesOrGuilds]));
                }
                if (outputTableName == nmfConstantsMSSPM::TableOutputCatchability) {
                    EstCatchability.push_back(val);
                }
            }
        }

        smodel->setVerticalHeaderLabels(SpeciesList);
        hLabels << nmfConstantsMSSPM::OutputVectorTableHeaders[i];

        smodel->setHorizontalHeaderLabels(hLabels);
        OutputTableViews[i]->setModel(smodel);
        OutputTableViews[i]->resizeColumnsToContents();
    }

    loadVisibleTables(isAlpha,isMsProd,isAggProd,isRho,isHandling,OutputTableViews,OutputTableNames);

    QList<bool> isData = {isAlpha,isMsProd,isRho,isHandling};
    for (int ii=0; ii<(int)OutputTableNames.size(); ++ii) {
        if (! isData[ii])
            continue;
        outputTableName = OutputTableNames[ii];
        smodel    = new QStandardItemModel( NumSpeciesOrGuilds, NumSpeciesOrGuilds );
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " + outputTableName;
        queryStr += " WHERE ProjectName = '"       + m_ProjectName +
                    "' AND ModelName  = '"         + m_ModelName +
                    "' AND Algorithm  = '"         + Algorithm +
                    "' AND Minimizer  = '"         + Minimizer +
                    "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                    "' AND Scaling   = '"          + Scaling +
                    "' AND isAggProd = "           + isAggProdStr +
                    " ORDER by SpeciesA,SpeciesB";
        dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeciesA"].size();
        if ((NumRecords != NumSpeciesOrGuilds*NumSpeciesOrGuilds) && (NumRecords != 0)) {
            msg = "[Error 2] nmfMainWindow::showChart: Incorrect number of records found in table " + outputTableName +
                    ", Found " + std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds*NumSpeciesOrGuilds) + ".";
            m_Logger->logMsg(nmfConstants::Error, msg);
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return false;
        }

        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                if (NumRecords == 0) {
                    item = new QStandardItem("");
                } else {
                    val  = std::stod(dataMap["Value"][m++]);
                    item = new QStandardItem(QString::number(val,'e',2));
                }
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
        }

        smodel->setVerticalHeaderLabels(SpeciesList);
        smodel->setHorizontalHeaderLabels(SpeciesList);

        OutputTableViews[ii]->setModel(smodel);
        OutputTableViews[ii]->resizeColumnsToContents();
    }

    OutputTableViews.clear();
    OutputTableViews.append(CompetitionBetaGTV);
    OutputTableNames.clear();
    OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds);
    isData.clear();
    isData.append(isMsProd || isAggProd);
    for (int ii=0; ii<(int)OutputTableNames.size(); ++ii) {
        if (! isData[ii])
            continue;
        smodel    = new QStandardItemModel( NumSpeciesOrGuilds, NumGuilds );
        fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Guild","Value"};
        queryStr  = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild,Value FROM " +
                OutputTableNames[ii];
        queryStr += " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + isAggProdStr +
                " ORDER by SpeName,Guild";
        dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if ((NumRecords != NumSpeciesOrGuilds*NumGuilds) && (NumRecords != 0)) {
            msg  = "[Error 3] nmfMainWindow::showChart: Incorrect number of records found in table " + OutputTableNames[ii] + ". ";
            msg += "Found " + std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds*NumGuilds) + ".";
            m_Logger->logMsg(nmfConstants::Error, msg);
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return false;
        }

        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                if (NumRecords == 0) {
                    item = new QStandardItem("");
                } else {
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                std::stod(dataMap["Value"][m++]),m_NumSignificantDigits,5);
                    item = new QStandardItem(valueWithComma);
                }
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
        }

        smodel->setVerticalHeaderLabels(SpeciesList);
        smodel->setHorizontalHeaderLabels(GuildList);

        OutputTableViews[ii]->setModel(smodel);
        OutputTableViews[ii]->resizeColumnsToContents();
    }

    } // end if

    if (! getOutputBiomass(NumLines,NumSpeciesOrGuilds,RunLength,
                           Algorithms,Minimizers,ObjectiveCriteria,Scalings,
                           isAggProdStr,OutputBiomass)) {
        return false;
    }

    // Load Observed (ie, original) Biomass
    if (isAggProd) {
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      "",ObsBiomassType,"",NumSpeciesOrGuilds,RunLength,
                                                      "",NullMatrix,ObservedBiomass)) {
            return false;
        }
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",ObsBiomassType,
                                               NumSpeciesOrGuilds,RunLength,ObservedBiomass)) {
            return false;
        }
    }

    // Convert Observed Biomass to correct units
    if (previousUnits.find(BiomassTable) != previousUnits.end()) {
        nmfUtilsQt::convertMatrix(ObservedBiomass, previousUnits[BiomassTable], "mt", nmfConstantsMSSPM::DontConvertBlanks);
    }

    OutputTableViews.clear();
    OutputTableViews.append(OutputBiomassTV);
    OutputTableNames.clear();
    OutputTableNames.push_back(nmfConstantsMSSPM::TableOutputBiomass);

    m = 0;
    smodel = new QStandardItemModel( RunLength, NumSpeciesOrGuilds );
    for (int species=0; species<NumSpeciesOrGuilds; ++species) {
        for (int time=0; time<=RunLength; ++time) {
            valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                        OutputBiomass[0](time,species),m_NumSignificantDigits,6);
            item = new QStandardItem(valueWithComma);
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(time, species, item);
        }
    }
    // Create year labels
    for (int time=0; time<=RunLength; ++time) {
        yearLabels << QString::number(StartYear + time);
    }
    smodel->setVerticalHeaderLabels(yearLabels);
    smodel->setHorizontalHeaderLabels(SpeciesList);
    OutputTableViews[ii]->setModel(smodel);
    OutputTableViews[ii]->resizeColumnsToContents();
    ++ii;

    // Calculate ScaleStr and Scaleval
    ScaleVal = convertUnitsStringToValue(ScaleStr);
    if (OutputType == nmfConstantsMSSPM::OutputChartBiomass) {
        showChartBiomassVsTime(NumSpeciesOrGuilds,OutputSpecies,
                               SpeciesNum,RunLength,StartYear,
                               Algorithms,
                               Minimizers,
                               ObjectiveCriteria,
                               Scalings,
                               OutputBiomass,
                               ObservedBiomass,
                               BMSYValues,
                               ScaleStr,ScaleVal,
                               nmfConstantsMSSPM::Clear,
                               YMinVal,
                               nmfConstantsMSSPM::TitleChartBiomass);

        Output_Controls_ptr->clearOutputBMSY();
        if (Output_Controls_ptr->isCheckedOutputBMSY() and (NumLines == 1)) {
            Output_Controls_ptr->setTextOutputBMSY(QString::number(BMSYValues[SpeciesNum]/ScaleVal));
        }
    }
    else if (OutputType == nmfConstantsMSSPM::OutputChartBiomassAndCatch) {
        if (! m_DatabasePtr->getHarvestData(this,m_Logger,m_ProjectName,m_ModelName,
                                            NumSpeciesOrGuilds,
                                            nmfConstantsMSSPM::DontDivideTableByOutputBiomass,
                                            nmfConstantsMSSPM::MultiplyTableByOutputBiomass,
                                            nmfConstantsMSSPM::MultiplyTableByCatchability,
                                            EstCatchability,OutputBiomass,HarvestData)) {
            return false;
        }
        HarvestVec.clear();
        HarvestVec.push_back(HarvestData);
        showChartBiomassVsTime(NumSpeciesOrGuilds,OutputSpecies,
                               SpeciesNum,RunLength,StartYear,
                               Algorithms,
                               Minimizers,
                               ObjectiveCriteria,
                               Scalings,
                               HarvestVec,
                               ObservedBiomass,
                               BMSYValues,
                               ScaleStr,ScaleVal,
                               nmfConstantsMSSPM::Clear,
                               YMinVal,
                               nmfConstantsMSSPM::TitleChartBiomassAndCatch);
        Output_Controls_ptr->clearOutputBMSY();
    }
    else if (OutputType == nmfConstantsMSSPM::OutputChartHarvest) {
        if (! m_DatabasePtr->getHarvestData(this,m_Logger,m_ProjectName,m_ModelName,
                                            NumSpeciesOrGuilds,
                                            nmfConstantsMSSPM::DontDivideTableByOutputBiomass,
                                            nmfConstantsMSSPM::MultiplyTableByOutputBiomass,
                                            nmfConstantsMSSPM::MultiplyTableByCatchability,
                                            EstCatchability,OutputBiomass,HarvestData)) {
            return false;
        }
        HarvestVec.clear();
        HarvestVec.push_back(HarvestData);

        // Convert catch to mt for plot
        if (previousUnits.find(tableHarvestCatch) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(HarvestData, previousUnits[tableHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
        }
        showChartTableVsTime(HarvestForm,
                             NumSpeciesOrGuilds,OutputSpecies,
                             SpeciesNum,RunLength,StartYear,
                             NumLines,
                             HarvestData,
                             HarvestVec,
                             MSYValues,
                             ScaleStr,ScaleVal,
                             YMinVal,YMaxVal);

        Output_Controls_ptr->clearOutputMSY();
        if (Output_Controls_ptr->isCheckedOutputMSY() and (NumLines == 1))
            Output_Controls_ptr->setTextOutputMSY(QString::number(MSYValues[SpeciesNum]/ScaleVal));
    }
    else if (OutputType == nmfConstantsMSSPM::OutputChartExploitation) {
        if (! m_DatabasePtr->getHarvestData(this,m_Logger,m_ProjectName,m_ModelName,
                                            NumSpeciesOrGuilds,
                                            nmfConstantsMSSPM::DivideTableByOutputBiomass,
                                            nmfConstantsMSSPM::DontMultiplyTableByOutputBiomass,
                                            nmfConstantsMSSPM::MultiplyTableByCatchability,
                                            EstCatchability,OutputBiomass,HarvestData)) {
            return false;
        }

        YMinVal = 0.0;
        YMaxVal = 1.0;
        showChartTableVsTime(nmfConstantsMSSPM::OutputChartExploitation.toStdString(),
                             NumSpeciesOrGuilds,OutputSpecies,
                             SpeciesNum,RunLength,StartYear,
                             NumLines,
                             HarvestData,
                             OutputBiomass,
                             FMSYValues,
                             ScaleStr,ScaleVal,
                             YMinVal,YMaxVal);
        Output_Controls_ptr->clearOutputFMSY();
        if (Output_Controls_ptr->isCheckedOutputFMSY() and (NumLines == 1))
            Output_Controls_ptr->setTextOutputFMSY(QString::number(FMSYValues[SpeciesNum]/ScaleVal));
    }
    else if (OutputType == "Diagnostics") {
        if (OutputMethod == "Parameter Profiles") {
            if (! showDiagnosticsChart2d(ScaleStr,ScaleVal,100.0)) {
                return false;
            }
            callback_ShowDiagnosticsChart3d();
        } else if (OutputMethod == "Retrospective Analysis") {
            callback_ShowChartMohnsRho();
        }
    }
    else if (OutputType == "Forecast") {
        double brightnessFactor = Output_Controls_ptr->getOutputBrightnessFactor();
        if (! showForecastChart(isAggProd,Forecast_Tab1_ptr->getForecastName(),
                                StartYear,ScaleStr,ScaleVal,YMinVal,
                                brightnessFactor))
        {
            return false;
        }
    }

    updateOutputCovariateCoefficientsTable(SpeciesList);

    return true;
}


boost::numeric::ublas::matrix<double>
nmfMainWindow::getObservedBiomassByGroup(const int& NumGuilds,
                                         const int& RunLength,
                                         const std::string& group)
{
    boost::numeric::ublas::matrix<double> tmpObservedBiomass;
    boost::numeric::ublas::matrix<double> ObservedBiomass;
    boost::numeric::ublas::matrix<double> NullMatrix;
    std::vector<std::string> fields;
    std::string queryStr;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string ObsBiomassTableName = getObservedBiomassTableName(!nmfConstantsMSSPM::PreEstimation);
    if (group == "Guild") {
        m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                "",ObsBiomassTableName,"",NumGuilds,RunLength,
                                                "",NullMatrix,ObservedBiomass);
    } else if (group == "System") {
        // Find total system observed biomass
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      "",ObsBiomassTableName,"",NumGuilds,RunLength,
                                                      "",NullMatrix,tmpObservedBiomass)) {
            return ObservedBiomass;
        }
        nmfUtils::initialize(ObservedBiomass,RunLength+1,1);
        for (int guild=0; guild<NumGuilds; ++guild) {
            for (int time=0; time<=RunLength; ++time) {
                ObservedBiomass(time,0) += tmpObservedBiomass(time,guild);
            }
        }
    }

    return ObservedBiomass;
}

std::vector<boost::numeric::ublas::matrix<double> >
nmfMainWindow::getOutputBiomassByGroup(
        const int& RunLength,
        const std::vector<boost::numeric::ublas::matrix<double> >& OutputBiomassSpecies,
        const std::string& group)
{
    QStringList SpeciesList;
    QStringList GuildList;
    std::vector<std::string> guildOrder;
    std::set<std::string> guilds;
    std::map<std::string, int> GuildMap;
    int i=0;
    int guildNum;
    int NumSpecies;
    int NumLines = OutputBiomassSpecies.size();
    std::string guildName;
    std::string guild;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomassGuilds;
    boost::numeric::ublas::matrix<double> TmpMatrix;

    if (! getSpeciesWithGuilds(NumSpecies,SpeciesList,GuildList))
        return OutputBiomassGuilds;

    if (group == "Guild") {
        NumSpecies = SpeciesList.size();
        for (int i=0; i<NumSpecies; ++i) {
            guildName = GuildList[i].toStdString();
            guildOrder.push_back(guildName);
            guilds.insert(guildName);
        }
        for (std::string guild : guilds) {
            GuildMap[guild] = i++;
        }
        for (int chart=0; chart<NumLines; ++chart) {
            nmfUtils::initialize(TmpMatrix,RunLength+1,guilds.size());
            for (int species=0; species<int(OutputBiomassSpecies[chart].size2()); ++species) {
                guild = guildOrder[species];
                guildNum = GuildMap[guild];
                for (int time=0; time<=RunLength; ++time) {
                    TmpMatrix(time,guildNum) += OutputBiomassSpecies[chart](time,species);
                }
            }
            OutputBiomassGuilds.push_back(TmpMatrix);
        }
    } else if (group == "System") {
        for (int chart=0; chart<NumLines; ++chart) {
            nmfUtils::initialize(TmpMatrix,RunLength+1,1);
            for (int species=0; species<NumSpecies; ++species) {
                for (int time=0; time<=RunLength; ++time) {
                    TmpMatrix(time,0) += OutputBiomassSpecies[chart](time,species);
                }
            }
            OutputBiomassGuilds.push_back(TmpMatrix);
        }
    }


    return OutputBiomassGuilds;
}

bool
nmfMainWindow::getMSYData(bool isAveraged,
                          const int& NumLines,
                          const int& NumGroups,
                          const std::string& group,
                          QList<double>& BMSYValues,
                          QList<double>& MSYValues,
                          QList<double>& FMSYValues)
{
    double BMSYVal = 0;
    double MSYVal  = 0;
    double FMSYVal = 0;
    int NumGuilds;
    int NumSpecies;
    std::string msg;
    std::string DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    std::string Algorithm          = DefaultAveragingAlgorithm;
    std::string Minimizer          = DefaultAveragingAlgorithm;
    std::string ObjectiveCriterion = DefaultAveragingAlgorithm;
    std::string Scaling            = DefaultAveragingAlgorithm;
    std::vector<std::string> fields;
    std::string queryStr;
    std::string CompetitionForm;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::vector<std::string> OutputMSYTableNames = {nmfConstantsMSSPM::TableOutputMSYBiomass,
                                                    nmfConstantsMSSPM::TableOutputMSY,
                                                    nmfConstantsMSSPM::TableOutputMSYFishing};
    std::string OutputMSYTableName;
    std::string isAggProdStr = "0";
    QList<double> tmpBMSYValues;
    QList<double> tmpMSYValues;
    QList<double> tmpFMSYValues;
    QStringList SpeciesList;
    QStringList GuildList;
    QString Species;
    QMap<QString,QString> GuildMap;
    QMap<QString,int> GuildNumMap; // Num of species per guild
    QMap<QString,double> BMSYMap;
    QMap<QString,double> MSYMap;
    QMap<QString,double> FMSYMap;

    BMSYValues.clear();
    MSYValues.clear();
    FMSYValues.clear();
    GuildNumMap.clear();

    if (! getSpeciesWithGuilds(NumSpecies,SpeciesList,GuildList)) {
        return false;
    }
    int i=0;
    for (QString species : SpeciesList) {
        GuildNumMap[GuildList[i]] += 1;
        GuildMap[species] = GuildList[i++];
    }

    if (! isAveraged) {
        m_DatabasePtr->getAlgorithmIdentifiers(
                    this,m_Logger,m_ProjectName,m_ModelName,
                    Algorithm,Minimizer,ObjectiveCriterion,
                    Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    }

    for (int i=0; i<(int)OutputMSYTableNames.size(); ++i) {
        OutputMSYTableName = OutputMSYTableNames[i];
        if ((NumLines == 1) && (! isAtLeastOneFilterPressed()))  {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                          OutputMSYTableName;
            queryStr  += " WHERE ProjectName = '"       + m_ProjectName +
                         "' AND ModelName = '"          + m_ModelName +
                         "' AND Algorithm = '"          + Algorithm +
                         "' AND Minimizer = '"          + Minimizer +
                         "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                         "' AND Scaling = '"            + Scaling +
                         "' AND isAggProd = "           + isAggProdStr +
                         " ORDER by SpeName";
        } else {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                          OutputMSYTableName +
                          getFilterButtonsResult() +
                         " ORDER by Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName";
        }
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumSpecies = dataMap["SpeName"].size();
        if (NumSpecies == 0) {
//            m_Logger->logMsg(nmfConstants::Error, queryStr);
            msg = "[nmfMainWindow::getMSYData] No data found in: " + OutputMSYTableName + " for current configuration. ";
            msg += "Please run an Estimation.";
            m_Logger->logMsg(nmfConstants::Normal, msg);
//            QMessageBox::information(this,
//                                     tr("No Output Data"),
//                                     tr(msg.c_str()),
//                                     QMessageBox::Ok);
            return false;
        }

        for (int line=0; line<NumLines; ++line) {
            for (int j=0; j<NumSpecies; ++j) {
                Species = QString::fromStdString(dataMap["SpeName"][j]);
                if (group == "Species") {
                    if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYBiomass) {
                        tmpBMSYValues.append(std::stod(dataMap["Value"][j]));
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSY) {
                        tmpMSYValues.append( std::stod(dataMap["Value"][j]));
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYFishing) {
                        tmpFMSYValues.append(std::stod(dataMap["Value"][j]));
                    }
                } else if (group == "Guild") {
                    if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYBiomass) {
                        BMSYMap[GuildMap[Species]] += std::stod(dataMap["Value"][j]);
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSY) {
                        MSYMap[GuildMap[Species]]  += std::stod(dataMap["Value"][j]);
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYFishing) {
                        FMSYMap[GuildMap[Species]] += std::stod(dataMap["Value"][j]);
                    }
                } else if (group == "System") {
                    if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYBiomass) {
                        tmpBMSYValues.append(std::stod(dataMap["Value"][j]));
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSY) {
                        tmpMSYValues.append( std::stod(dataMap["Value"][j]));
                    } else if (OutputMSYTableName == nmfConstantsMSSPM::TableOutputMSYFishing) {
                        tmpFMSYValues.append(std::stod(dataMap["Value"][j]));
                    }
                }

            }
        }
    }

    GuildList.clear();
    Output_Controls_ptr->getGuilds(NumGuilds,GuildList);
    if (group == "Species") {
        for (int i=0; i<NumSpecies;++i) {
            BMSYValues.append(tmpBMSYValues[i]);
            MSYValues.append( tmpMSYValues[i]);
            FMSYValues.append(tmpFMSYValues[i]/NumSpecies);
        }
    } else if (group == "Guild") {
        for (QString guild : GuildList) {
            BMSYValues.append(BMSYMap[guild]);
            MSYValues.append( MSYMap[guild]);
            FMSYValues.append(FMSYMap[guild]/GuildNumMap[guild]);
        }
    } else if (group == "System") {
        for (int i=0; i<NumSpecies;++i) {
            BMSYVal += tmpBMSYValues[i];
            MSYVal  += tmpMSYValues[i];
            FMSYVal += tmpFMSYValues[i];
        }
        BMSYValues.append(BMSYVal);
        MSYValues.append( MSYVal);
        FMSYValues.append(FMSYVal/NumSpecies);
    }

    return true;
}


void
nmfMainWindow::callback_ShowChartBy(QString GroupType,
                                    bool isAveraged)
{
    int NumGuilds;
    int NumGuildsOrSpecies;
    int NumSpecies;
    int RunLength;
    int StartYear;
    int NumLines            = getNumLines();
    int SpeciesNum          = Output_Controls_ptr->getOutputSpeciesIndex();
    double YMinVal          = Output_Controls_ptr->getYMinSliderVal();
    double YMaxVal          = nmfConstants::NoValueDouble;
    QString OutputChartType = Output_Controls_ptr->getOutputChartType();
    QString OutputSpecies   = Output_Controls_ptr->getOutputSpecies();
    QString ScaleStr        = Output_Controls_ptr->getOutputScale();
    double ScaleVal         = convertUnitsStringToValue(ScaleStr);
    QStringList SpeciesList;
    QStringList GuildList;
    std::string CompetitionForm;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string isAggProdStr = "0";
    std::vector<std::string> Algorithms;
    std::vector<std::string> Minimizers;
    std::vector<std::string> ObjectiveCriteria;
    std::vector<std::string> Scalings;
    boost::numeric::ublas::matrix<double> ObservedBiomass;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomassSpecies;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomass;
    QList<double> BMSYValues;
    QList<double> MSYValues;
    QList<double> FMSYValues;
    boost::numeric::ublas::matrix<double> tmpHarvest;
    boost::numeric::ublas::matrix<double> Harvest;
    std::vector<boost::numeric::ublas::matrix<double> > HarvestVec;
    std::vector<double> EstSurveyQ;
    std::string chartLabel = "";

    clearChart();

    if (isAMultiOrMohnsRhoRun() && (OutputChartType == nmfConstantsMSSPM::OutputChartBiomass)) {
        displayAverageBiomass();
        return;
    }

    getInitialYear(StartYear,RunLength);

    if (isAMohnsRhoMultiRun()) {
        // Will never reach this if clause since mohns rho is only for species...not for guilds.
        // Just put this here in case we change the logic in the future.app
        getOutputBiomassEnsemble(RunLength+1,NumSpecies,m_OutputBiomassEnsemble);
    } else {
        if (! isAveraged) {
            m_DatabasePtr->getAlgorithmIdentifiers(
                        this,m_Logger,m_ProjectName,m_ModelName,
                        Algorithm,Minimizer,ObjectiveCriterion,
                        Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

            m_DatabasePtr->getEstimatedParameter(nmfConstantsMSSPM::TableOutputSurveyQ,
                                                 m_ProjectName, m_ModelName,
                                                 Algorithm,Minimizer,ObjectiveCriterion,
                                                 Scaling,isAggProdStr,EstSurveyQ);
            if (EstSurveyQ.size() == 0) {
                m_Logger->logMsg(nmfConstants::Error,"{Error 1} nmfMainWindow::callback_ShowChartBy: No Estimated SurveyQ parameters found.");
                return;
            }
        }
    }

    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList))
        return;
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;
    if (! getOutputBiomass(NumLines,NumSpecies,RunLength,
                           Algorithms,Minimizers,ObjectiveCriteria,Scalings,
                           isAggProdStr,OutputBiomassSpecies)) {
        return;
    }
    NumLines = OutputBiomassSpecies.size();

    // Group-specific code here (i.e., Species, Guild, System) -RSKRSK
    OutputBiomass   = getOutputBiomassByGroup(RunLength,OutputBiomassSpecies,GroupType.toStdString());
    ObservedBiomass = getObservedBiomassByGroup(NumGuilds,RunLength,GroupType.toStdString());
    if (GroupType == "Guild") {
        NumGuildsOrSpecies = NumGuilds;
    } else if (GroupType == "System") {
        NumGuildsOrSpecies  = 1;
        SpeciesNum = 0;
        OutputSpecies = "System";
    }

    boost::numeric::ublas::matrix<double> ObservedBiomassByGuilds;
    std::map<int,std::vector<int> > GuildSpecies;
    std::vector<int>                GuildNum;
    if (! m_DatabasePtr->getGuildData(m_Logger,NumGuilds,RunLength,
                                      GuildList,GuildSpecies,GuildNum,
                                      ObservedBiomassByGuilds)) {
        return;
    }

    if (! isAMohnsRhoMultiRun()) {
        getMSYData(isAveraged,NumLines,NumGuildsOrSpecies,GroupType.toStdString(),
                   BMSYValues,MSYValues,FMSYValues);
    }

    // Draw the appropriate chart
    if (OutputChartType == nmfConstantsMSSPM::OutputChartBiomass) {
        showChartBiomassVsTime(NumGuildsOrSpecies, OutputSpecies,
                               SpeciesNum, RunLength,StartYear,
                               Algorithms,
                               Minimizers,
                               ObjectiveCriteria,
                               Scalings,
                               OutputBiomass,
                               ObservedBiomass,
                               BMSYValues,
                               ScaleStr,ScaleVal,
                               nmfConstantsMSSPM::Clear,
                               YMinVal,
                               nmfConstantsMSSPM::TitleChartBiomass);
        Output_Controls_ptr->clearOutputBMSY();
        if (Output_Controls_ptr->isCheckedOutputBMSY() and (NumLines == 1)) {
            Output_Controls_ptr->setTextOutputBMSY(QString::number(BMSYValues[SpeciesNum]/ScaleVal));
        }
    }
    else if (OutputChartType == nmfConstantsMSSPM::OutputChartHarvest) {
        // Load Harvest as: qE x B(c) (which is what getHarvestDataByGuild sends back)
        if (GroupType == "Guild") {
            m_DatabasePtr->getHarvestDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                 OutputChartType, GroupType,
                                                 NumGuildsOrSpecies,chartLabel,
                                                 OutputBiomassSpecies[0],Harvest);
        } else if (GroupType == "System") {
            m_DatabasePtr->getHarvestDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                 OutputChartType, GroupType,
                                                 NumGuilds,chartLabel,
                                                 OutputBiomassSpecies[0],Harvest);
        }
        HarvestVec.clear();
        HarvestVec.push_back(Harvest);
        showChartTableVsTime(chartLabel,
                             NumGuilds,OutputSpecies,
                             SpeciesNum,RunLength,StartYear,
                             NumLines,
                             Harvest,
                             HarvestVec,
                             MSYValues,
                             ScaleStr,ScaleVal,
                             YMinVal,YMaxVal);

        Output_Controls_ptr->clearOutputMSY();
        if (Output_Controls_ptr->isCheckedOutputMSY() and (NumLines == 1)) {
            Output_Controls_ptr->setTextOutputMSY(QString::number(MSYValues[SpeciesNum]/ScaleVal));
        }
    }
    else if (OutputChartType == nmfConstantsMSSPM::OutputChartExploitation) {
        // Load Harvest as: qE (which is what getHarvestDataByGuild sends back)
        if (GroupType == "Guild") {
            m_DatabasePtr->getHarvestDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                 OutputChartType, GroupType,
                                                 NumGuildsOrSpecies,chartLabel,
                                                 OutputBiomassSpecies[0],Harvest);
        } else if (GroupType == "System") {
            m_DatabasePtr->getHarvestDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                 OutputChartType, GroupType,
                                                 NumGuilds,chartLabel,
                                                 OutputBiomassSpecies[0],Harvest);
        }

        // Hardcode to 0 and 1 for Exploitation Rate
        YMinVal = 0.0;
        YMaxVal = 1.0;
        showChartTableVsTime(OutputChartType.toStdString(),
                             NumGuilds,OutputSpecies,
                             SpeciesNum,RunLength,StartYear,
                             NumLines,
                             Harvest,
                             OutputBiomass,
                             FMSYValues,
                             ScaleStr,ScaleVal,
                             YMinVal,YMaxVal);
        Output_Controls_ptr->clearOutputFMSY();
        if (Output_Controls_ptr->isCheckedOutputFMSY() and (NumLines == 1)) {
            Output_Controls_ptr->setTextOutputFMSY(QString::number(FMSYValues[SpeciesNum]/ScaleVal));
        }
    }

}

void
nmfMainWindow::callback_RefreshOutput()
{
    Output_Controls_ptr->refresh();
}


void
nmfMainWindow::getInitialYear(int& StartYear,
                              int& RunLength)
{
//    std::vector<std::string> fields;
//    std::map<std::string, std::vector<std::string> > dataMap;
//    std::string queryStr;
//    // Find Original Start and end years
//    fields   = {"StartYear","RunLength"};
//    queryStr = "SELECT StartYear,RunLength from " +
//                nmfConstantsMSSPM::TableModels +
//               " WHERE ProjectName = '" + m_ProjectName +
//               "' AND ModelName = '"    + m_ModelName + "'"; //QString::fromStdString(m_ModelName).split("__")[0].toStdString() + "'";
//    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
//    if (dataMap["StartYear"].size() != 0) {
//        InitialYear = std::stoi(dataMap["StartYear"][0]);
//        RunLength   = std::stoi(dataMap["RunLength"][0]);
//    }

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
}

void
nmfMainWindow::callback_UpdateSummaryStatistics()
{
    m_Logger->logMsg(nmfConstants::Normal,"callback_UpdateSummaryStatistics");

    int NumSpeciesOrGuilds;
    int RunLength;
    int StartYear;
    int NumStatistics = nmfConstantsMSSPM::SummaryModelFitStatisticNames.size();
    QStandardItemModel *smodel;
    QStandardItem* item;
    QString tooltip;
    QStringList vLabels;
    QStringList SpeciesList;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    bool isAggProd = (CompetitionForm == "AGG-PROD");

    if (! m_HPC_Algorithm.empty() && ! m_HPC_Minimizer.empty() &&
        ! m_HPC_ObjectiveCriterion.empty() && ! m_HPC_Scaling.empty()) {
        Algorithm          = m_HPC_Algorithm;
        Minimizer          = m_HPC_Minimizer;
        ObjectiveCriterion = m_HPC_ObjectiveCriterion;
        Scaling            = m_HPC_Scaling;
    } else if (m_IsMultiRun || m_UsingHPCFiles) {
        std::string DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
        Algorithm          = DefaultAveragingAlgorithm;
        Minimizer          = DefaultAveragingAlgorithm;
        ObjectiveCriterion = DefaultAveragingAlgorithm;
        Scaling            = DefaultAveragingAlgorithm;
    } else {
        m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    }

    m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList);
    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

    m_Logger->logMsg(nmfConstants::Normal,"Calculating Summary Statistics for Run of length: "+std::to_string(RunLength));

    smodel = new QStandardItemModel( NumStatistics, 1+NumSpeciesOrGuilds+1 );
    for (int i=0; i<NumStatistics; ++i) {
        tooltip = nmfConstantsMSSPM::SummaryModelFitStatisticTooltips[i];
        item    = new QStandardItem(nmfConstantsMSSPM::SummaryModelFitStatisticNames[i]);
        item->setToolTip(tooltip);
        item->setStatusTip(tooltip);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable); // read-only
        smodel->setItem(i, 0, item);
    }

    // This causes all GUIs to reload because it toggles the significant digits
    bool isSigDigChecked = saveSigDigState();

    calculateSummaryStatistics(smodel,isAggProd,Algorithm,Minimizer,
                               ObjectiveCriterion,Scaling,
                               RunLength,NumSpeciesOrGuilds,false);

    vLabels.clear();
    vLabels << "Statistic" << SpeciesList << "Model";
    smodel->setHorizontalHeaderLabels(vLabels);
    SummaryTV->setModel(smodel);
    SummaryTV->resizeColumnsToContents();

    saveSummaryModelFitTable(smodel);

    // This causes all GUIs to reload because it toggles the significant digits
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::setSummaryStatisticsHelp()
{
    QString msg = "";
    msg += "<pre>";
    msg += "<strong><center>Summary Statistics Formulae</center></strong>";
    msg += "<br>SSR  (SSresiduals):            Σ(Oₜ-Eₜ)² over all t years";
    msg += "<br>SSD  (SSdeviations):           Σ(Eₜ-&#332;)² over all t years";
    msg += "<br>SST  (SStotals):               SSresiduals + SSdeviations";
    msg += "<br>r²   (Determination Coeff):    SSdeviations/SStotals";
    msg += "<br>r    (Correlation Coeff):      <u>  Σ[(Oₜ-&#332;)(Eₜ-&#274;)]    </u>";
    msg += "<br>                               √{Σ(Oₜ-&#332;)²Σ(Eₜ-&#274;)²}";
    msg += "<br>AIC  (Akaike Info Criterion):  n*ln(σ²) + 2K, where:";
    msg += "<br>                               K  = number of parameters";
    msg += "<br>                               n  = number of observations";
    msg += "<br>                               σ² = SSresiduals/n";
    msg += "<br>RMSE (Root Mean Sq Error):     √{(Σ(Eₜ-Oₜ)²)/n}";
    msg += "<br>RI   (Reliability Index):      exp[√{(1/n)Σ([log(Oₜ/Eₜ)]²)}]";
    msg += "<br>AE   (Ave Error or Bias):      Σ(Eₜ-Oₜ)/n";
    msg += "<br>AAE  (Ave Abs Error):          Σ|Eₜ-Oₜ|/n";
    msg += "<br>MEF  (Modeling Efficiency):    [Σ(Oₜ-&#332;)²-Σ(Eₜ-Oₜ)²]/Σ(Oₜ-&#332;)²<br>";
    msg += "<br>where O = Observed Biomass";
    msg += "<br>and   E = Estimated Biomass";
    msg += "</pre>";
    SummaryTV->setWhatsThis(msg);
    SummaryGB->setWhatsThis(msg);
    m_UI->MSSPMOutputModelFitSummaryTab->setWhatsThis(msg);
    SummaryTV->setToolTip("For a detailed description of these statistics,\nclick the WhatsThis? icon and click over the table.");
    SummaryTV->setStatusTip("For a detailed description of these statistics,\nclick the WhatsThis? icon and click over the table.");
}

void
nmfMainWindow::saveSummaryModelFitTable(QStandardItemModel *smodel)
{
    int numRows;
    int numCols;
    int numSpecies;
    std::string cmd;
    std::string errorMsg;
    QString valueStr;
    QStringList SpeciesList;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;

    if (smodel) {

        getCurrentAlgorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

        // Delete previous data
        cmd = "DELETE FROM " + nmfConstantsMSSPM::TableSummaryModelFit +
              " WHERE ProjectName = '"       + m_ProjectName +
              "' AND ModelName = '"          + m_ModelName +
              "' AND Algorithm = '"          + Algorithm +
              "' AND Minimizer = '"          + Minimizer +
              "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
              "' AND Scaling = '"            + Scaling + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] saveSummaryModelFitTable: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }

        numRows = smodel->rowCount();
        numCols = smodel->columnCount(); // Species: Statistics, Cod, ..., Flounder, Model (first and last "species" aren't really species)
        numSpecies = numCols - 2;
        // Get species names
        for (int col=0; col<numCols; ++col) {
            SpeciesList << smodel->horizontalHeaderItem(col)->text();
        }

        cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableSummaryModelFit +
                " (ProjectName,ModelName,SpeciesName,Algorithm,Minimizer,ObjectiveCriterion,Scaling," +
                " SSResiduals,SSDeviations,SSTotals," +
                "rSquared,rCorrelationCoeff,AkaikeInfoCriterion,RootMeanSquareError," +
                "ReliabilityIndex,AverageError,AverageAbsError,ModelingEfficiency) VALUES ";
        for (int col=0; col<numCols; ++col) { // -1 because the first column is the StatisticName
            cmd += "('" + m_ProjectName + "','" + m_ModelName + "','" + SpeciesList[col].toStdString() +
                     "','" + Algorithm + "','" + Minimizer +
                     "','" + ObjectiveCriterion + "','" + Scaling + "'";
            for (int row=0; row<numRows; ++row) {
                valueStr = smodel->index(row,col).data().toString().replace(",","");
                cmd += ",'" + valueStr.toStdString() + "'";
            }
            cmd += "),";
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::saveSummaryModelFitTable: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }

        Output_TableWidgets_ptr->setLastVariables(SummaryTV,
                                                 "Summary Model Fit",
                                                  nmfConstantsMSSPM::SummaryModelFitStatisticNames);
    }
}

void
nmfMainWindow::saveSummaryDiagnosticTable(
        QStandardItemModel* smodel,
        std::string& FieldNames)
{
    int numRows;
    int numCols;
    std::string cmd;
    std::string errorMsg;
    QString valueStr;
    //QStandardItem* item;
    QStringList SpeciesList;

    // Remove any spaces from FieldNames
    boost::erase_all(FieldNames," ");

    if (smodel) {
        m_DatabasePtr->clearTable(m_Logger,nmfConstantsMSSPM::TableSummaryDiagnostic,m_ProjectName);
        numRows = smodel->rowCount();
        numCols = smodel->columnCount(); // Species: Statistics, Cod, ..., Flounder, Model (first and last "species" aren't really species)
        // Get species names
        for (int col=0; col<numCols; ++col) {
            SpeciesList << smodel->horizontalHeaderItem(col)->text();
        }

        cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableSummaryDiagnostic +
                " (ProjectName,ModelName,SpeciesName" +
                FieldNames + ") VALUES ";
        for (int col=0; col<numCols; ++col) { // -1 because the first column is the StatisticName
            cmd += "('" + m_ProjectName + "','" + m_ModelName + "','" + SpeciesList[col].toStdString() + "'";
            for (int row=0; row<numRows; ++row) {
                valueStr = smodel->index(row,col).data().toString().replace(",","");
                cmd += ",\"" + valueStr.toStdString() + "\"";
            }
            cmd += "),";
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::saveSummaryDiagnosticTable: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }

        Output_TableWidgets_ptr->setLastVariables(DiagnosticSummaryTV,
                                                 "Summary Diagnostic",
                                                  nmfConstantsMSSPM::SummaryDiagnosticStatisticNames);
    }
}


void
nmfMainWindow::updateDiagnosticSummaryStatistics()
{
    m_Logger->logMsg(nmfConstants::Normal,"Calculating Diagnostic Summary Statistics");

    int NumSpeciesOrGuilds;
    int RunLength;
    int StartYear;
    QStandardItemModel *smodel;
    QStandardItem* item;
    QStringList vLabels;
    QStringList SpeciesList;
    std::string errorMsg;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    bool isAggProd = (CompetitionForm == "AGG-PROD");
    std::string FieldNames = "";

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList);
    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

    QStringList EstParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();
    std::vector<QString> statistics;
    std::vector<QString> tooltips;

    int i = -1;
    QString statisticName;
    for (QString name : EstParamNames) {
        FieldNames += "," + name.toStdString();
//std::cout << "==> " << name.toStdString() << std::endl;
        ++i;
        statisticName = "Mohn's Rho (" + name + ")";
        if (nmfConstantsMSSPM::SummaryDiagnosticStatisticNames.contains(statisticName)) {
            statistics.push_back(statisticName);
            tooltips.push_back(statisticName);
        } else {
            errorMsg  = "nmfMainWindow::updateDiagnosticSummaryStatistics: Mismatch with Statistic name";
            errorMsg += ", \"" + nmfConstantsMSSPM::SummaryDiagnosticStatisticNames[i].toStdString() +
                    "\" != \"Mohn's Rho ("+name.toStdString()+")\"";
            m_Logger->logMsg(nmfConstants::Error,errorMsg);
            //            QMessageBox::critical(this,"Error",QString::fromStdString(errorMsg),QMessageBox::Ok);
        }
    }
    statisticName = nmfConstantsMSSPM::SummaryDiagnosticStatisticNames.back();
    statistics.push_back(statisticName);
    tooltips.push_back(statisticName);
    statisticName.remove("Mohn's Rho (");
    statisticName.remove(")");
    FieldNames += "," + statisticName.toStdString();
    int NumStatistics = statistics.size();

    smodel = new QStandardItemModel( NumStatistics, 1+NumSpeciesOrGuilds+1 );
    for (int i=0; i<NumStatistics; ++i) {
        item = new QStandardItem(statistics[i]);
        item->setToolTip(tooltips[i]);
        item->setStatusTip(tooltips[i]);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable); // read-only
        smodel->setItem(i, 0, item);
    }

    bool isSigDigChecked = saveSigDigState();

    calculateSummaryStatistics(smodel,isAggProd,Algorithm,Minimizer,
                               ObjectiveCriterion,Scaling,
                               RunLength,NumSpeciesOrGuilds,true);

    vLabels.clear();
    vLabels << "Statistic" << SpeciesList << "Model";
    smodel->setHorizontalHeaderLabels(vLabels);
    DiagnosticSummaryTV->setModel(smodel);
    DiagnosticSummaryTV->resizeColumnsToContents();
    saveSummaryDiagnosticTable(smodel,FieldNames);
    restoreSigDigState(isSigDigChecked);
}

void
nmfMainWindow::setDiagnosticSummaryHelp()
{
    QString msg = "";
    msg += "<pre>";
    msg += "<strong><center>Diagnostic Summary Statistics Formulae</center></strong>";
    msg += "<br>Mohn's Rho: [Σ{(&#7824;(y=T-n,T-n)-&#7824;(y=T-n,T))/&#7824;(y=T-n,T)}]/x";
    msg += "<br>    &#7824;(A,B) is the estimated value for year A with terminal year B,";
    msg += "<br>    x is the number of years \"peeled\", and Σ sums from n=1 to x<br>";
    msg += "<br>where O = Observed Biomass";
    msg += "<br>and   E = Estimated Biomass";
    msg += "</pre>";
    DiagnosticSummaryTV->setWhatsThis(msg);
    DiagnosticSummaryTV->setToolTip("For a detailed description of these statistics,\nclick the WhatsThis? icon and click over the table.");
    DiagnosticSummaryTV->setStatusTip("For a detailed description of these statistics,\nclick the WhatsThis? icon and click over the table.");
    DiagnosticSummaryGB->setWhatsThis(msg);
    m_UI->MSSPMOutputDiagnosticSummaryTab->setWhatsThis(msg);
}

void
nmfMainWindow::callback_ShowChartMohnsRho()
{
    clearChart();
    if (isAMohnsRhoMultiRun()) {
        callback_SetChartView2d(true);
        displayAverageBiomass();
    }
}


bool
nmfMainWindow::callback_ShowChartMultiScenario(QStringList SortedForecastLabels)
{
    int NumSpecies  = 0;
    int NumYears    = 0;
    double BrightnessFactor = 0; // Unused for MultiScenario
    QString ScaleStr         = Output_Controls_ptr->getOutputScale();
    QString OutputSpecies    = Output_Controls_ptr->getOutputSpecies();
    int SpeciesNum           = Output_Controls_ptr->getOutputSpeciesIndex();
    int StartYearForecast    = Forecast_Tab1_ptr->getStartForecastYear();
    std::string ScenarioName = Output_Controls_ptr->getOutputScenario().toStdString();
    bool showHistoricalData  = Output_Controls_ptr->isHistoricalDataShown();
    std::vector<boost::numeric::ublas::matrix<double> > ForecastBiomassMultiScenario;
    QStringList ColumnLabelsForLegend;
    double ScaleVal = convertUnitsStringToValue(ScaleStr);
    QString msg;
    double YMinSliderVal = -1;

    clearChart();

    ForecastBiomassMultiScenario.clear();

    // First time in just show a blank canvas.
    if (SortedForecastLabels.isEmpty()) {
        m_ChartView2d->hide();
        msg = "[Warning] No MultiScenario Forecasts found. Please make sure a Forecast has been saved as a MultiScenario Forecast.";
        m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
        msg = "\nNo MultiScenario Forecasts found.\n\nPlease make sure a Forecast has been saved as a MultiScenario Forecast.\n";
        QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
        return false;
    }

    // Get the scenario data
    if (! getMultiScenarioBiomass(
                ScenarioName,
                NumSpecies,
                NumYears,
                SortedForecastLabels,
                ColumnLabelsForLegend,
                ForecastBiomassMultiScenario)) {
        return false;
    }

    // Draw the scenario chart
    showBiomassVsTimeForMultipleRuns(
                "Multi-Forecast Scenario",
                "Year",
                "Multi-Forecast Scenario",
                showHistoricalData,
                StartYearForecast,NumYears, // RSK - check this logic for using NumYears here...
                NumSpecies,OutputSpecies,
                SpeciesNum,NumYears,
                ForecastBiomassMultiScenario,
                ScaleStr,ScaleVal,
                YMinSliderVal,BrightnessFactor,
                nmfConstantsMSSPM::IsNotMonteCarlo,
                nmfConstantsMSSPM::IsNotEnsemble,
                nmfConstantsMSSPM::Clear,
                ColumnLabelsForLegend);

    setCurrentOutputTab("Chart");

    return true;
}


void
nmfMainWindow::loadVisibleTables(const bool& isAlpha,
                                 const bool& isMsProd,
                                 const bool& isAggProd,
                                 const bool& isRho,
                                 const bool& isHandling,
                                 QList<QTableView*>& TableViews,
                                 std::vector<std::string>& TableNames)
{
    QTableView* NullTV = nullptr;

    // Load 2d tables
    TableViews.clear();
    TableNames.clear();

    if (isAlpha) {
        TableViews.append(CompetitionAlphaTV);
        TableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionAlpha);
    } else {
        TableViews.append(NullTV);
        TableNames.push_back("");
    }
    if (isMsProd || isAggProd) {
        TableViews.append(CompetitionBetaSTV);
        TableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies);
    } else {
        TableViews.append(NullTV);
        TableNames.push_back("");
    }
    if (isRho) {
        TableViews.append(PredationRhoTV);
        TableNames.push_back(nmfConstantsMSSPM::TableOutputPredationRho);
    } else {
        TableViews.append(NullTV);
        TableNames.push_back("");
    }
    if (isHandling) {
        TableViews.append(PredationHandlingTV);
        TableNames.push_back(nmfConstantsMSSPM::TableOutputPredationHandling);
    } else {
        TableViews.append(NullTV);
        TableNames.push_back("");
    }
}


bool
nmfMainWindow::showDiagnosticsChart2d(const QString& ScaleStr,
                                      const double&  ScaleVal,
                                      const double&  YMinSliderVal)
{
    int NumSpeciesOrGuilds;
    int NumSpecies;
    int NumGuilds;
    int NumPoints;
    int SpeciesNum = Output_Controls_ptr->getOutputSpeciesIndex();
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::string ParameterName = Output_Controls_ptr->getOutputParameter().toStdString();
    QString msg;
    QStringList SpeciesList;
    QStringList GuildList;
    QStringList SpeciesOrGuildList;
    QStringList ColHeadings = {"Pct Deviation","Fitness"};
//  QStringList ColHeadings = {"Pct Deviation","log(Fitness)"};
    QString OutputSpecies = Output_Controls_ptr->getOutputSpecies();
    boost::numeric::ublas::matrix<double> DiagnosticsValue;
    boost::numeric::ublas::matrix<double> DiagnosticsFitness;
    QStandardItemModel* smodel2;
    QStandardItem* item;
    bool isAggProdBool = isAggProd();
    std::string isAggProdStr = (isAggProdBool) ? "1" : "0";
    QLocale locale(QLocale::English);
    QString valueWithComma;

    if (SpeciesNum < 0) {
        SpeciesNum = 0;
    }

    NumPoints = Diagnostic_Tab1_ptr->getLastRunsNumPoints();

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    checkDiagnosticAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

    // Find guild info
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList)) {
        return false;
    }
    // Find species info
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    if (isAggProdBool) {
        NumSpeciesOrGuilds = NumGuilds;
        SpeciesOrGuildList = GuildList;
    } else {
        NumSpeciesOrGuilds = NumSpecies;
        SpeciesOrGuildList = SpeciesList;
    }

    // Plot Diagnostics data
    if (! getDiagnosticsData(NumPoints,NumSpeciesOrGuilds,
                             Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                             isAggProdStr,DiagnosticsValue,DiagnosticsFitness))
    {
        //Output_Controls_ptr->setOutputParameters2d3dPB(nmfConstantsMSSPM::ChartType2d);
        //m_ChartView2d->hide();
        msg = "No Diagnostic records found. Please make sure a Diagnostic has been run for all parameters.";
        m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
        //return false;
    }

    showDiagnosticsFitnessVsParameter(NumPoints,ParameterName,
                                      "Fitness",
                                      NumSpeciesOrGuilds,OutputSpecies,
                                      SpeciesNum,
                                      DiagnosticsValue,
                                      DiagnosticsFitness,
                                      YMinSliderVal);

    // Update Output->Data table
    double XStart = -Diagnostic_Tab1_ptr->getLastRunsPctVariation();
    double XInc   = -XStart/NumPoints;
    int TotNumPoints = 2*NumPoints+1;
    smodel2 = new QStandardItemModel(TotNumPoints, 2 );
    for (int i=0; i<TotNumPoints; ++i) {
        // The % deviation values
        item = new QStandardItem(QString::number(XStart+i*XInc,'f',6));
        item->setTextAlignment(Qt::AlignCenter);
        smodel2->setItem(i, 0, item);
        // The fitness values
        valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                    DiagnosticsFitness(i,SpeciesNum),m_NumSignificantDigits,6);
        item = new QStandardItem(valueWithComma);
        item->setTextAlignment(Qt::AlignCenter);
        smodel2->setItem(i, 1, item);
    }

    smodel2->setHorizontalHeaderLabels(ColHeadings);
    m_UI->OutputDataTV->setModel(smodel2);
    m_UI->OutputDataTV->resizeColumnsToContents();
    m_UI->OutputDataTV->show();

    return true;
}


bool
nmfMainWindow::showForecastChart(const bool&  isAggProd,
                                 std::string  ForecastName,
                                 const int&   StartYear,
                                 QString&     ScaleStr,
                                 double&      ScaleVal,
                                 double&      YMinSliderValue,
                                 double&      BrightnessFactor)
{
    bool showHistoricalData = Output_Controls_ptr->isHistoricalDataShown();
    int NumRuns    = 0;
    int RunLength  = 0;
    int NumSpecies = 0;
    int NumGuilds  = 0;
    int NumSpeciesOrGuilds;
    int SpeciesNum = Output_Controls_ptr->getOutputSpeciesIndex();
    int StartYearForecast = nmfConstantsMSSPM::Start_Year;
    // int EndYear;
    QString OutputSpecies = Output_Controls_ptr->getOutputSpecies();
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string ForecastHarvestType;
    std::vector<boost::numeric::ublas::matrix<double> > ForecastBiomass;
    boost::numeric::ublas::matrix<double> ForecastBiomassWithHistoricalData;
    std::vector<boost::numeric::ublas::matrix<double> > AppendedMatrices;
    std::vector<boost::numeric::ublas::matrix<double> > ForecastBiomassMonteCarlo;
    QStringList SpeciesOrGuildList;
    QStringList SpeciesOrGuildAbbrevList;
    QStringList SpeciesList;
    QStringList GuildList;
    QStringList yearLabels;
    QStandardItemModel* smodel;
    QStandardItemModel* smodel2;
    QStandardItem* item;
    QStringList ColumnLabelsForLegend;
    QLocale locale(QLocale::English);
    QString valueWithComma;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomass;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;

    // Find guild info
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList))
        return false;
    // Find species info
    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return false;
    if (isAggProd) {
        NumSpeciesOrGuilds = NumGuilds;
        SpeciesOrGuildList = GuildList;
    } else {
        NumSpeciesOrGuilds = NumSpecies;
        SpeciesOrGuildList = SpeciesList;
    }

    // Find Forecast info
    if (! m_DatabasePtr->getForecastInfo(
                m_ProjectName,m_ModelName,ForecastName,RunLength,StartYearForecast,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,ForecastHarvestType,NumRuns)) {
        return false;
    }
    checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

    if (showHistoricalData) {
        int NumLines = 1;
        int StartYear;
        int RunLength;
        std::vector<std::string> Algorithms;
        std::vector<std::string> Minimizers;
        std::vector<std::string> ObjectiveCriteria;
        std::vector<std::string> Scalings;
        std::string              isAggProdStr;
        m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
        if (! getOutputBiomass(NumLines,NumSpeciesOrGuilds,RunLength,
                               Algorithms,Minimizers,ObjectiveCriteria,Scalings,
                               isAggProdStr,OutputBiomass))
        {
            m_Logger->logMsg(nmfConstants::Error,"Returning from within showForecastChart");
            return false;
        }
    }

    // Plot ForecastBiomassMonteCarlo data
    if (NumRuns > 0) {
        if (! m_DatabasePtr->getForecastBiomassMonteCarlo(this,m_Logger,
              m_ProjectName,m_ModelName,ForecastName,NumSpeciesOrGuilds,RunLength,NumRuns,
              Algorithm,Minimizer,ObjectiveCriterion,Scaling,
              ForecastBiomassMonteCarlo)) {
            m_ChartView2d->hide();
            return false;
        }

        ColumnLabelsForLegend.clear();
        ColumnLabelsForLegend << "";
        showBiomassVsTimeForMultipleRuns(
                    "Forecast",
                    "Year",
                    "Biomass",
                    nmfConstantsMSSPM::DontShowHistoricalDelimiterLine,
                    StartYearForecast,RunLength+1, // RSK - this might not be correct check with OutputBiomass[0].size1()
                    NumSpeciesOrGuilds,OutputSpecies,
                    SpeciesNum,RunLength+1,
                    ForecastBiomassMonteCarlo,
                    ScaleStr,ScaleVal,
                    YMinSliderValue,BrightnessFactor,
                    nmfConstantsMSSPM::IsMonteCarlo,
                    nmfConstantsMSSPM::IsNotEnsemble,
                    nmfConstantsMSSPM::Clear,
                    ColumnLabelsForLegend);
    }

    // Plot ForecastBiomass data
    if (! m_DatabasePtr->getForecastBiomass(this,m_Logger,m_ProjectName,m_ModelName,
                                            ForecastName,NumSpeciesOrGuilds,RunLength,
                                            Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                            ForecastBiomass)) {
        return false;
    }
    ColumnLabelsForLegend << "Forecast Biomass";

    // if show historical data, prepend OutputBiomass to ForecastBiomass
    if (showHistoricalData) {
        int totalNumRows = OutputBiomass[0].size1() + ForecastBiomass[0].size1();
        int totalNumCols = OutputBiomass[0].size2() + ForecastBiomass[0].size2();
        nmfUtils::initialize(ForecastBiomassWithHistoricalData,totalNumRows,totalNumCols);
        for (int row=0; row<(int)OutputBiomass[0].size1(); ++row) {
            for (int col=0; col<(int)OutputBiomass[0].size2(); ++col) {
                ForecastBiomassWithHistoricalData(row,col) = OutputBiomass[0](row,col);
            }
        }
        int numRowsOutputBiomass = OutputBiomass[0].size1();
        for (int row=1; row<(int)ForecastBiomass[0].size1(); ++row) { // First Forecast row is same as last observed biomass row
            for (int col=0; col<(int)ForecastBiomass[0].size2(); ++col) {
                ForecastBiomassWithHistoricalData(numRowsOutputBiomass+row-1,col) = ForecastBiomass[0](row,col);
            }
        }
        AppendedMatrices.push_back(ForecastBiomassWithHistoricalData);
        showBiomassVsTimeForMultipleRuns("Forecast w/ Historical Data",
                                         "Year",
                                         "Biomass",
                                         showHistoricalData,
                                         StartYear,OutputBiomass[0].size1(),
                                         NumSpeciesOrGuilds,OutputSpecies,
                                         SpeciesNum,ForecastBiomassWithHistoricalData.size1()-1, //Because we're omitting the 1st forecast point
                                         AppendedMatrices,
                                         ScaleStr,ScaleVal,
                                         YMinSliderValue,BrightnessFactor,
                                         nmfConstantsMSSPM::IsNotMonteCarlo,
                                         nmfConstantsMSSPM::IsNotEnsemble,
                                         nmfConstantsMSSPM::DontClear,
                                         ColumnLabelsForLegend);
    }
    else {
        showBiomassVsTimeForMultipleRuns("Forecast",
                                         "Year",
                                         "Biomass",
                                         showHistoricalData,
                                         StartYearForecast,RunLength+1, // RSK - this might not be correct check with OutputBiomass[0].size1()
                                         NumSpeciesOrGuilds,OutputSpecies,
                                         SpeciesNum,RunLength+1,
                                         ForecastBiomass,
                                         ScaleStr,ScaleVal,
                                         YMinSliderValue,BrightnessFactor,
                                         nmfConstantsMSSPM::IsNotMonteCarlo,
                                         nmfConstantsMSSPM::IsNotEnsemble,
                                         nmfConstantsMSSPM::DontClear,
                                         ColumnLabelsForLegend);
    }


    // Update Output->Estimated Parameters->Output Biomass table
    smodel = new QStandardItemModel( RunLength, NumSpeciesOrGuilds );
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        for (int time=0; time<=RunLength; ++time) {
            if (i == 0) {
                yearLabels << QString::number(StartYearForecast+time);
            }
            valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                        ForecastBiomass[0](time,i),m_NumSignificantDigits,6); //3);
            item = new QStandardItem(valueWithComma);
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(time, i, item);
        }
    }
    smodel->setVerticalHeaderLabels(yearLabels);
    smodel->setHorizontalHeaderLabels(SpeciesOrGuildList);
    OutputBiomassTV->setModel(smodel);

    // Update Output->Data table
    smodel2 = new QStandardItemModel( RunLength, 1 );
    for (int time=0; time<=RunLength; ++time) {
        valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                    ForecastBiomass[0](time,SpeciesNum),m_NumSignificantDigits,3);
        item = new QStandardItem(valueWithComma);
        item->setTextAlignment(Qt::AlignCenter);
        smodel2->setItem(time, 0, item);
    }
    smodel2->setVerticalHeaderLabels(yearLabels);
    SpeciesOrGuildAbbrevList << SpeciesOrGuildList[SpeciesNum];
    smodel2->setHorizontalHeaderLabels(SpeciesOrGuildAbbrevList);
    m_UI->OutputDataTV->setModel(smodel2);
    m_UI->OutputDataTV->resizeColumnsToContents();
    m_UI->OutputDataTV->show();

    return true;
}

std::string
nmfMainWindow::getObservedBiomassTableName(bool isPreEstimation)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string ObsBiomassType;
    std::string relativeBiomassName = (isPreEstimation) ?
                nmfConstantsMSSPM::TableBiomassRelative :
                nmfConstantsMSSPM::TableBiomassRelativeDividedByEstSurveyQ;

    fields    = {"ObsBiomassType"};
    queryStr  = "SELECT ObsBiomassType FROM " +
                 nmfConstantsMSSPM::TableModels +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName   + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["ObsBiomassType"].size();
    if (NumRecords == 0) {
        ObsBiomassType = nmfConstantsMSSPM::TableBiomassAbsolute;
    } else {
        ObsBiomassType = dataMap["ObsBiomassType"][0];
        ObsBiomassType = (ObsBiomassType.empty() || ObsBiomassType == "Absolute") ?
                         nmfConstantsMSSPM::TableBiomassAbsolute : relativeBiomassName;
    }

    return ObsBiomassType;
}

bool
nmfMainWindow::calculateSummaryStatisticsStruct(
        const bool&         skipFirstYear,
        const bool&         isAggProd,
        const std::string&  Algorithm,
        const std::string&  Minimizer,
        const std::string&  ObjectiveCriterion,
        const std::string&  Scaling,
        const int&          RunLength,
        const int&          NumSpeciesOrGuilds,
        const bool&         isMohnsRhoBool,
        const bool&         isAMultiRun,
        boost::numeric::ublas::matrix<double>& EstimatedBiomass,
        StatStruct&         statStruct)
{
    bool ok;
    double val;
    double aicModel;
    int NumYears;
    int FirstYear = (skipFirstYear) ? 1 : 0;
    std::vector<double> SSresiduals;
    std::vector<double> SSdeviations;
    std::vector<double> SStotals;
    std::vector<double> rsquared;
    std::vector<double> aic;
    std::vector<double> rmse;
    std::vector<double> ri;
    std::vector<double> ae;
    std::vector<double> aae;
    std::vector<double> mef;
    std::vector<double> mohnsRhoParameter;
    std::vector<double> mohnsRhoGrowthRate;
    std::vector<double> mohnsRhoGrowthRateShape;
    std::vector<double> mohnsRhoCarryingCapacity;
    std::vector<double> mohnsRhoEstimatedBiomass;
    std::vector<double> meanObserved;
    std::vector<double> meanEstimated;
    std::vector<double> correlationCoeff;
    std::vector<double> EstGrowthRate;
    std::vector<double> EstGrowthRateShape;
    std::vector<double> EstCarryingCapacity;
    std::vector<double> EstCatchability;
    std::vector<double> EstSurveyQ;
    std::vector<double> EstInitBiomass;
    std::vector<double> EstPredationExponent;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    int NumEstParametersTotal = 0;
    int NumEstParametersTotalNonTrivial = 0;
    std::vector<int> NumEstParametersPerSpecies;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomass;
    boost::numeric::ublas::matrix<double> ObservedBiomass;
    boost::numeric::ublas::matrix<double> NullMatrix;
    std::vector<std::string> Algorithms;
    std::vector<std::string> Minimizers;
    std::vector<std::string> ObjectiveCriteria;
    std::vector<std::string> Scalings;
    std::string isAggProdStr = (isAggProd) ? "1" : "0";
    QList<std::vector<double> > stats;
    std::vector<std::string> aveOrSum;
    double total;
    std::string errorMsg;
    std::vector<int> NumYearsWithoutBlanksPerCol = {};
    int TotalNumYearsWithoutBlanks = 0;

//  m_Logger->logMsg(nmfConstants::Normal,"calculateSummaryStatisticsStruct from: "+m_ModelName);

//    NumberOfParameters = Estimation_Tab7_ptr->getNumParameters();
     std::map<std::string,bool> EstimateCheckBoxStates = Estimation_Tab7_ptr->getAllEstimateCheckboxStates();
     ok = m_DatabasePtr->getNumParameters(m_Logger,m_ProjectName,m_ModelName,
                                          EstimateCheckBoxStates,NumSpeciesOrGuilds,
                                          NumEstParametersPerSpecies,
                                          NumEstParametersTotalNonTrivial,
                                          NumEstParametersTotal);
     if (! ok) {
         m_Logger->logMsg(nmfConstants::Error,"getNumParameters returned an error");
         return false;
     }

    // Load Observed (i.e., original) Biomass (need to check if should be loading Absolute or Relative)
    std::string ObsBiomassTableName = getObservedBiomassTableName(!nmfConstantsMSSPM::PreEstimation);
    if (isAggProd) {
        if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                      "",ObsBiomassTableName,"",NumSpeciesOrGuilds,RunLength,
                                                      "",NullMatrix,ObservedBiomass)) {
            return false;
        }
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",ObsBiomassTableName,
                                               NumSpeciesOrGuilds,RunLength,ObservedBiomass)) {
            return false;
        }
    }
    NumYears = ObservedBiomass.size1();

    // Calculate the meanObserved from the observed
    if (! nmfUtilsStatistics::calculateMeanOmittingBlanks(FirstYear,ObservedBiomass,
                                                    NumYearsWithoutBlanksPerCol,TotalNumYearsWithoutBlanks,meanObserved)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfUtilsStatistics::calculateMeanOmittingBlanks divide by zero found, check matrix dimensions");
        return false;
    }

    // Get estimated data
    int NumLines = 1;
    Algorithms.push_back(Algorithm);
    Minimizers.push_back(Minimizer);
    ObjectiveCriteria.push_back(ObjectiveCriterion);
    Scalings.push_back(Scaling);

    if (isAMultiRun) {
        OutputBiomass.push_back(EstimatedBiomass);
    } else {
        if (! getOutputBiomass(NumLines,NumSpeciesOrGuilds,RunLength,
                               Algorithms,Minimizers,ObjectiveCriteria,Scalings,
                               isAggProdStr,OutputBiomass))
        {
            m_Logger->logMsg(nmfConstants::Error,"Returning from within calculateSummaryStatistics");
            return false;
        }
    }

    // Calculate the mean estimated values by species
    if (! nmfUtilsStatistics::calculateMean(FirstYear,OutputBiomass[0],meanEstimated)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfUtilsStatistics::calculateMean divide by zero found, check matrix dimensions");
        return false;
    }

//std::cout << "Warning: TBD nmfMainWindow::calculateSummaryStatisticsStruct: Add matrix parameters to this map" << std::endl;
    // RSK move this later
    std::map<QString,std::string> mapParameterNameToTableName;
    mapParameterNameToTableName["Growth Rate"]              = nmfConstantsMSSPM::TableOutputGrowthRate;
    mapParameterNameToTableName["Growth Rate Shape"]        = nmfConstantsMSSPM::TableOutputGrowthRateShape;
    mapParameterNameToTableName["Carrying Capacity"]        = nmfConstantsMSSPM::TableOutputCarryingCapacity;
    mapParameterNameToTableName["Initial Absolute Biomass"] = nmfConstantsMSSPM::TableOutputInitBiomass;
    mapParameterNameToTableName["Catchability"]             = nmfConstantsMSSPM::TableOutputCatchability;
    mapParameterNameToTableName["SurveyQ"]                  = nmfConstantsMSSPM::TableOutputSurveyQ;
    mapParameterNameToTableName["Predation Exponent"]       = nmfConstantsMSSPM::TableOutputPredationExponent;

    std::vector<std::vector<double> > EstParameters;
    std::vector<double> EstParameter;
    QStringList estParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();
    std::string tableName = "";

// RSK continue here, get output estimated parameters from the new Mohns Rho struct
    for (QString name : estParamNames) {
        tableName = mapParameterNameToTableName[name];
        if (! tableName.empty()) {
            getOutputParameterVector(tableName,EstParameter);
            EstParameters.push_back(EstParameter);
        }
    }

    // Calculate SSresiduals
    nmfUtilsStatistics::calculateSSResiduals(ObservedBiomass,OutputBiomass[0],
            skipFirstYear,SSresiduals,errorMsg);
    if (! errorMsg.empty()) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg);
        return false;
    }

    // Calculate SSdeviations
    ok = nmfUtilsStatistics::calculateSSDeviations(ObservedBiomass,OutputBiomass[0],meanObserved,
            skipFirstYear,SSdeviations,errorMsg);
    if (! errorMsg.empty()) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg);
        return false;
    }

    // Calculate SStotals
    nmfUtilsStatistics::calculateSSTotals(SSdeviations,SSresiduals,SStotals,errorMsg);
    if (! errorMsg.empty()) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg);
        return false;
    }

    // Calculate rsquared (between 0.2 and 0.3 good....closer to 1.0 the better)
    nmfUtilsStatistics::calculateRSquared(SSdeviations,SStotals,rsquared,errorMsg);
    if (! errorMsg.empty()) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg);
        return false;
    }

    // AIC - Akaike Information Criterion (for Least Squares in this case)
    // AIC = n * ln(sigma^2) + 2K
    //   K = number of estimated parameters
    //   n = number of observations (i.e., NumYears)
    //   sigma^2 = SSresiduals/n
    nmfUtilsStatistics::calculateAIC(
              NumEstParametersPerSpecies,NumYearsWithoutBlanksPerCol,SSresiduals,aic);
    nmfUtilsStatistics::calculateAIC(
              NumEstParametersPerSpecies,TotalNumYearsWithoutBlanks,SSresiduals,aicModel);

    // Calculate r
    ok = nmfUtilsStatistics::calculateR(skipFirstYear,NumSpeciesOrGuilds,
                                        meanObserved,meanEstimated,
                                        ObservedBiomass,OutputBiomass[0],correlationCoeff);
    if (! ok) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] calculateSummaryStatistics: Divide by 0 error in r calculations.");
        return false;
    }

    // Calculate RMSE
    ok = nmfUtilsStatistics::calculateRMSE(skipFirstYear,NumSpeciesOrGuilds,
                                           ObservedBiomass,OutputBiomass[0],rmse);
    if (! ok) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 3] calculateSummaryStatistics: Found 0 RunLength in RMSE calculations.");
        return false;
    }

    // Calculate RI
    ok = nmfUtilsStatistics::calculateRI(skipFirstYear,NumSpeciesOrGuilds,
                                         ObservedBiomass,OutputBiomass[0],ri);
    if (! ok) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 4] calculateSummaryStatistics: Found 0 RunLength in RI calculations.");
        return false;
    }

    // Calculate AE
    nmfUtilsStatistics::calculateAE(NumSpeciesOrGuilds,meanObserved,meanEstimated,ae);

    // Calculate AAE
    nmfUtilsStatistics::calculateAAE(skipFirstYear,NumSpeciesOrGuilds,
                                     ObservedBiomass,OutputBiomass[0],aae);

    // Calculate MEF
    ok = nmfUtilsStatistics::calculateMEF(skipFirstYear,NumSpeciesOrGuilds,meanObserved,
                                          ObservedBiomass,OutputBiomass[0],mef);
    if (! ok) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 5] calculateSummaryStatistics: Found 0 denominator in MEF calculations.");
        //return false;
    }

    stats = {SSresiduals, SSdeviations,
             SStotals, rsquared,
             correlationCoeff, aic,
             rmse, ri, ae,
             aae, mef};
    aveOrSum = {"sum","sum",
                "sum","rsq",
                "ave","aic",
                "ave","ave","ave",
                "ave","ave"};

    double SumSSdev=0;
    double SumSStot=1;
    for (int statNum=0; statNum<stats.size(); ++statNum) {
        total = 0;
        // Load the species stats
        for (int species=0; species<NumSpeciesOrGuilds; ++species) {
            val = stats[statNum][species];
            if (val != nmfConstants::NoValueDouble) {
                total += val;
            }
        }
        if (statNum == 1) {SumSSdev = total;}
        if (statNum == 2) {SumSStot = total;}

        // Load the model (i.e., last column) stats, either summed, averaged, or a special case for aic
        if (aveOrSum[statNum] == "sum") {
            stats[statNum].push_back(total);
        } else if (aveOrSum[statNum] == "ave") {
            stats[statNum].push_back(total/NumSpeciesOrGuilds);
        } else if (aveOrSum[statNum] == "aic") {
            stats[statNum].push_back(aicModel);
        } else if (aveOrSum[statNum] == "rsq") {
            stats[statNum].push_back(SumSSdev/SumSStot);
        }
    }

    // Load statistics structure
    statStruct.SSresiduals      = stats[0];
    statStruct.SSdeviations     = stats[1];
    statStruct.SStotals         = stats[2];
    statStruct.rsquared         = stats[3];
    statStruct.correlationCoeff = stats[4];
    statStruct.aic              = stats[5];
    statStruct.rmse             = stats[6];
    statStruct.ri               = stats[7];
    statStruct.ae               = stats[8];
    statStruct.aae              = stats[9];
    statStruct.mef              = stats[10];
    statStruct.title            = "Summary Statistics (" + Algorithm + "): ";

    return true;
}

bool
nmfMainWindow::calculateSummaryStatisticsStructMohnsRho(StatStruct& statStruct)
{
    m_Logger->logMsg(nmfConstants::Normal,"calculateSummaryStatisticsStructMohnsRho from: "+m_ModelName);

    QList<std::vector<double> > stats;
    std::vector<double> mohnsRhoEstimatedBiomass;
    std::vector<std::vector<double> > MohnsRhoVectors;

    QStringList EstParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();

    if (! m_MohnsRhoData.calculateMohnsRhoVectors(EstParamNames,MohnsRhoVectors)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfMainWindow::calculateSummaryStatisticsStructMohnsRho: Error with estimated parameter");
        return false;
    }

    // Calculate Mohn's Rho
    for (std::vector<double> MohnsRhoVector : MohnsRhoVectors) {
        stats.push_back(MohnsRhoVector);
    }
    calculateSummaryStatisticsMohnsRhoBiomass(mohnsRhoEstimatedBiomass);
    stats.push_back(mohnsRhoEstimatedBiomass);

    // Load statistics structure
    for (std::vector<double> stat : stats) {
        statStruct.mohnsRhoParameters.push_back(stat);
    }

    return true;
}

void
nmfMainWindow::calculateSummaryStatistics(QStandardItemModel* smodel,
                                          const bool&         isAggProd,
                                          const std::string&  Algorithm,
                                          const std::string&  Minimizer,
                                          const std::string&  ObjectiveCriterion,
                                          const std::string&  Scaling,
                                          const int&          RunLength,
                                          const int&          NumSpeciesOrGuilds,
                                          const bool&         isMohnsRhoBool)
{
    bool ok = true;
    StatStruct statStruct;
    boost::numeric::ublas::matrix<double> EstimatedBiomass;
    statStruct.title = "Summary Statistics:";
    std::string theAlgorithm = Algorithm;

    // Thought might want to only allow user to view appropriate GB after an appropriate
    // estimation run or diagnostic run...re-thinking this logic.
    // SummaryGB->show();
    // DiagnosticSummaryGB->hide();
    if (isMohnsRhoBool) {
        // SummaryGB->hide();
        // DiagnosticSummaryGB->show();
        ok = calculateSummaryStatisticsStructMohnsRho(statStruct);
    } else {
        // Get the Estimated Biomass
        bool itsAMultiRun = isAMultiRun();
        if (itsAMultiRun) {
            std::string isAggProd = "0";
            std::string AverageType = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
            theAlgorithm = AverageType;
            if (! m_DatabasePtr->getEstimatedBiomass(NumSpeciesOrGuilds,RunLength,
                                                     nmfConstantsMSSPM::TableOutputBiomass,
                                                     m_ProjectName, m_ModelName,
                                                     AverageType,AverageType,AverageType,AverageType,
                                                     isAggProd,EstimatedBiomass)) {
                QMessageBox::warning(this, "Error",
                                     "\nmfMainWindow::calculateSummaryStatistics: Error reading OutputBiomass with Algorithm: "+QString::fromStdString(AverageType)+"\n",
                                     QMessageBox::Ok);
                return;
            }
        }
        ok = calculateSummaryStatisticsStruct(
                    nmfConstantsMSSPM::SkipFirstYear,
                    isAggProd,theAlgorithm,Minimizer,ObjectiveCriterion,Scaling,
                    RunLength,NumSpeciesOrGuilds,isMohnsRhoBool,itsAMultiRun,
                    EstimatedBiomass,statStruct);
    }
    if (ok) {
        loadSummaryStatisticsModel(NumSpeciesOrGuilds,smodel,isMohnsRhoBool,statStruct);
        SummaryGB->setTitle(QString::fromStdString(statStruct.title));
    }
}

void
nmfMainWindow::loadSummaryStatisticsModel(
        const int& NumSpeciesOrGuilds,
        QStandardItemModel* smodel,
        bool isMohnsRhoBool,
        StatStruct& statStruct)
{
    double value;
    QStandardItem* item = nullptr;
    std::vector<std::vector<double> > stats;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    if (isMohnsRhoBool) {
        stats = statStruct.mohnsRhoParameters;
    } else {
        stats = {statStruct.SSresiduals, statStruct.SSdeviations,
                 statStruct.SStotals, statStruct.rsquared,
                 statStruct.correlationCoeff, statStruct.aic,
                 statStruct.rmse, statStruct.ri, statStruct.ae,
                 statStruct.aae, statStruct.mef};
    }

    for (int j=0; j<(int)stats.size(); ++j) {
        // Load the species stats
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            value = stats[j][i];
            if (value >= nmfConstantsMSSPM::ValueToStartEE) {
                item = new QStandardItem(QString::number(value,'G',4));
            } else {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            value,m_NumSignificantDigits,6);
                item = new QStandardItem(valueWithComma);
            }
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() &  ~Qt::ItemIsEditable); // read-only
            smodel->setItem(j, i+1, item);
        }
        // Load the model (i.e., last column) stats
        value = stats[j][NumSpeciesOrGuilds];
        if (value >= nmfConstantsMSSPM::ValueToStartEE) {
            item = new QStandardItem(QString::number(value,'G',4));
        } else {
            valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                        value,m_NumSignificantDigits,6);
            item = new QStandardItem(valueWithComma);
        }
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable); // read-only
        smodel->setItem(j, NumSpeciesOrGuilds+1, item);
    }
}

bool
nmfMainWindow::isAMohnsRhoMultiRun()
{
    return ( m_LastRunWasRetrospectiveAnalysis ||
             Diagnostic_Tab2_ptr->isAMohnsRhoRunForSingleRun() ||
             Diagnostic_Tab2_ptr->isPageVisible());
}

bool
nmfMainWindow::isAMultiRun()
{
    return (Estimation_Tab7_ptr->isAMultiRun() || m_UsingHPCFiles);
}

bool
nmfMainWindow::isAMultiOrMohnsRhoRun()
{
    return (isAMultiRun() || isAMohnsRhoMultiRun());
}

void
nmfMainWindow::showChartTableVsTime(
        const std::string &label,
        const int &NumSpecies,
        const QString &OutputSpecies,
        const int &SpeciesNum,
        const int &RunLength,
        const int &StartYear,
        int &NumLines,
        boost::numeric::ublas::matrix<double> &Harvest,
        std::vector<boost::numeric::ublas::matrix<double> > &Biomass,
        QList<double> &Values,
        QString &ScaleStr,
        double &ScaleVal,
        double &YMinVal,
        double &YMaxVal)
{
    //
    // Draw the line chart
    //
    bool AddScatter = (NumLines == 1);
    bool isCheckedOutputMSY  = Output_Controls_ptr->isCheckedOutputMSY();
    bool isEnabledOutputMSY  = Output_Controls_ptr->isEnabledOutputMSY();
    bool isCheckedOutputFMSY = Output_Controls_ptr->isCheckedOutputFMSY();
    bool isEnabledOutputFMSY = Output_Controls_ptr->isEnabledOutputFMSY();
    bool isAMultiRun         = isAMultiOrMohnsRhoRun();
    int NumLineColors = nmfConstants::LineColors.size();
    bool isMohnsRho = isAMohnsRhoMultiRun();
    QColor ScatterColor;
    QColor ChartColor;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    boost::numeric::ublas::matrix<double> ChartMSYData;
    boost::numeric::ublas::matrix<double> ChartLineData;
    boost::numeric::ublas::matrix<double> ChartScatterData;
    boost::numeric::ublas::matrix<double> SkipScatterData;
    double value;
    std::string valueStr;
    std::string LineStyle = "SolidLine";
    std::string msg;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList Years;
    std::string legendCode = "";
    QStandardItemModel* smodel = new QStandardItemModel( RunLength+1, 1 );
    std::string ChartType;
    std::string MainTitle;
    std::string XLabel;
    std::string YLabel;
    int Theme = 0; // Replace with checkbox values
    bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
    QColor lineColor;
    bool xAxisIsInteger = true;
    QString colorName;
    QStringList HoverLabels;
    std::string FishingLabelNorm  = nmfConstantsMSSPM::OutputChartExploitation.toStdString();
    std::string FishingLabelAve   = nmfConstantsMSSPM::OutputChartExploitation.toStdString() + " (Ave)";
    std::string FishingLabel      = (isAMultiRun) ? FishingLabelAve : FishingLabelNorm;
    std::string FishingLegendNorm = "F Rate";
    std::string FishingLegendAve  = "Ave F Rate";
    std::string FishingLegend     = (isAMultiRun) ? FishingLegendAve : FishingLegendNorm;
    std::string passedInLabel;
    QLocale locale(QLocale::English);
    QString valueWithComma;
    bool showLegend = Output_Controls_ptr->isCheckedOutputLegend();

    ChartMSYData.resize(RunLength+1,1);
    ChartMSYData.clear();
    ChartLineData.resize(RunLength+1,1);
    ChartLineData.clear();
    ChartScatterData.resize(RunLength+1,1);
    ChartScatterData.clear();
    SkipScatterData.resize(RunLength+1,1);
    SkipScatterData.clear();
    RowLabelsForBars.clear();
    ColumnLabelsForLegend.clear();
//    if ((label == nmfConstantsMSSPM::HarvestCatch.toStdString()) ||
//        (label == nmfConstantsMSSPM::HarvestEffort.toStdString())) {
        passedInLabel = label;
//    } else {
//        passedInLabel = (isAMultiRun) ? label+" Ave(C/Bc)" : label+" (C/Bc)";
//    }

    bool isPassedInLabelHarvest = (passedInLabel == nmfConstantsMSSPM::HarvestCatch.toStdString()) ||
                                  (passedInLabel == nmfConstantsMSSPM::HarvestEffort.toStdString());

     if (passedInLabel == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        passedInLabel = QString::fromStdString(passedInLabel).split(" ")[0].toStdString();
    } else if (label == nmfConstantsMSSPM::HarvestExploitation.toStdString()) {
        QStringList parts = QString::fromStdString(passedInLabel).split(" ");
        passedInLabel = parts[0].toStdString() + " " + parts[1].toStdString();
    }

    AddScatter = (isPassedInLabelHarvest) || (passedInLabel == FishingLabel) ? false : AddScatter;
    NumLines   = (isPassedInLabelHarvest) || (passedInLabel == FishingLabel) ?   1   : NumLines;

    ChartType = "Line";
    MainTitle = passedInLabel + " for: " + OutputSpecies.toStdString();
    MainTitle = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";
    MainTitle = Output_Controls_ptr->isCheckedOutputSpecies() ? OutputSpecies.toStdString() : MainTitle;
    XLabel    = "Run Length (Years)";
    YLabel    = (passedInLabel == FishingLabel) ?
                 passedInLabel :
                 passedInLabel + " (" + ScaleStr.toStdString() + "mt)";

    m_ChartWidget->removeAllSeries();

    //  Load Scatter plot points into data structure
    double val = 0;
    for (int species=0; species<NumSpecies; ++species) {
        if (species == SpeciesNum) {
            for (int time=0; time<=RunLength; ++time) {
                val = Biomass[0](time,species);
                if (val != nmfConstantsMSSPM::NoData) {
                    ChartScatterData(time,0) = val/ScaleVal;
                } else {
                    SkipScatterData(time,0) = true;
                }
            }
            break;
        }
    }

    for (int line=0; line<NumLines; ++line)
    {
        ColumnLabelsForLegend.clear();
        ChartLineData.clear();
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                for (int time=0; time<=RunLength; ++time) {
                    if (time == 0) {
                        if (isPassedInLabelHarvest) {
                            legendCode = passedInLabel + " Data";
                        } else if (passedInLabel == FishingLabel) {
                            legendCode = FishingLegend;
                        }
                    }
                    value = -99999;
                    Years << QString::number(StartYear+ time);
                    if (isPassedInLabelHarvest) {
                        value = Harvest(time,species)/ScaleVal;
                        ChartLineData(time,0) = value;
                    } else if (passedInLabel == FishingLabel) {
                        value = Harvest(time,species);
                        ChartLineData(time,0) = value;
                    }
//                  valueWithComma = locale.toString(value);
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                value,m_NumSignificantDigits,6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(time, 0, item);
                    RowLabelsForBars << QString::number(StartYear+time);
                }
                break;
            }
        }

        // Call the chart api function that will populate and annotate the
        // desired chart type.
        ColumnLabelsForLegend << QString::fromStdString(legendCode);
        if (m_ChartWidget != nullptr) {
            nmfChartLineWithScatter* lineWithScatterChart = new nmfChartLineWithScatter();
            lineWithScatterChart->populateChart(m_ChartWidget,
                                     ChartType,
                                     LineStyle,
                                     Output_Controls_ptr->getOutputLineWidthData(),
                                     Output_Controls_ptr->getOutputLineWidthPoint(),
                                     isMohnsRho,
                                     nmfConstantsMSSPM::ShowFirstPoint,
                                     AddScatter,
                                     StartYear,
                                     Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                     xAxisIsInteger,
                                     YMinVal,YMaxVal,
                                     ChartLineData,
                                     ChartScatterData,
                                     SkipScatterData,
                                     RowLabelsForBars,
                                     ColumnLabelsForLegend,
                                     MainTitle,
                                     XLabel,
                                     YLabel,
                                     Output_Controls_ptr->getOutputFontSizeLabel(),
                                     Output_Controls_ptr->getOutputFontSizeNumber(),
                                     Output_Controls_ptr->getOutputFont(),
                                     Output_Controls_ptr->getOutputLineWidthAxis(),
                                     Output_Controls_ptr->getOutputLineWidthAxis(),
                                     {GridLines,GridLines},
                                     Theme,
                                     ChartColor,
                                     ScatterColor,
                                     nmfConstants::LineColors[line%NumLineColors],
                                     nmfConstants::LineColorNames[line%NumLineColors],
                                     {},
                                     showLegend);
        }

        AddScatter = (line+1 == NumLines-1);
    } // end for line

    if ((isCheckedOutputMSY  && (isPassedInLabelHarvest)) ||
        (isCheckedOutputFMSY && (passedInLabel == FishingLabel)))
    {
        LineStyle = "DashedLine";
        ColumnLabelsForLegend.clear();

        if (isCheckedOutputMSY && isEnabledOutputMSY) {
            ColumnLabelsForLegend << "MSY - - - -";
        } else if (isCheckedOutputFMSY && isEnabledOutputFMSY) {
            ColumnLabelsForLegend << "F MSY - - - -";
        }
        for (int line=0; line<NumLines; ++line)
        {
            // Draw the MSY or FMSY line
            for (int i=0; i<NumSpecies; ++i) {
                if (i == SpeciesNum) {
                    for (int j=0; j<=RunLength; ++j) {
                        ChartMSYData(j,0) = Values[i+line*NumSpecies]/ScaleVal;
                    }
                    break;
                }
            }
            colorName = getColorName(line);

            if (m_ChartWidget != nullptr) {
                nmfChartLine* lineChart = new nmfChartLine();
                if (isPassedInLabelHarvest || (passedInLabel == FishingLabel)) {
                    lineColor = QColor(nmfConstants::LineColors[(line+1)%NumLineColors].c_str());
                } else {
                    lineColor = QColor(nmfConstants::LineColors[line%NumLineColors].c_str());
                }
                lineChart->populateChart(m_ChartWidget,
                                         ChartType,
                                         LineStyle,
                                         Output_Controls_ptr->getOutputLineWidthData(),
                                         nmfConstantsMSSPM::ShowFirstPoint,
                                         showLegend,
                                         StartYear,
                                         Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                         xAxisIsInteger,
                                         YMinVal,YMaxVal,
                                         nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                         ChartMSYData,
                                         RowLabelsForBars,
                                         ColumnLabelsForLegend,
                                         ColumnLabelsForLegend, //HoverLabels,
                                         MainTitle,
                                         XLabel,
                                         YLabel,
                                         Output_Controls_ptr->getOutputFontSizeLabel(),
                                         Output_Controls_ptr->getOutputFontSizeNumber(),
                                         Output_Controls_ptr->getOutputFont(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         {GridLines,GridLines},
                                         Theme,
                                         {},
                                         colorName.toStdString(),
                                         1.0);
            }
        }
    }

    //
    // Load ChartData into Output table view
    SpeciesNames << OutputSpecies;
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    smodel->setVerticalHeaderLabels(Years);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->resizeColumnsToContents();
}

void
nmfMainWindow::showDiagnosticsFitnessVsParameter(
        const int&         NumPoints,
        std::string        XLabel,
        std::string        YLabel,
        const int&         NumSpeciesOrGuilds,
        const QString&     OutputSpecies,
        const int&         SpeciesNum,
        boost::numeric::ublas::matrix<double> &DiagnosticsValue,
        boost::numeric::ublas::matrix<double> &DiagnosticsFitness,
        const double& YMinVal)
{
   int TotalNumPoints = 2*NumPoints+1;
   int NumLines = 1;
   QList<QColor> LineColors;
   QStringList RowLabelsForBars;
   QStringList ColumnLabelsForLegend;
   boost::numeric::ublas::matrix<double> ChartLineData;
   boost::numeric::ublas::matrix<double> ChartScatterData;
   ChartLineData.resize(TotalNumPoints,NumLines);
   ChartLineData.clear();
   ChartScatterData.resize(TotalNumPoints,1);
   ChartScatterData.clear();
   RowLabelsForBars.clear();
   ColumnLabelsForLegend.clear();
   std::string LineStyle = "SolidLine";
   QStringList Years;
   std::string ChartType;
   std::string MainTitle;
   int Theme = 0; // Replace with checkbox values
   bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
   double StartXValue = -Diagnostic_Tab1_ptr->getLastRunsPctVariation();
   double XInc        = -StartXValue/NumPoints;
   double YMaxVal     = nmfConstants::NoValueDouble;
   QStringList HoverLabels;

   ChartType = "Line";
   XLabel += " % Deviation";
   MainTitle = YLabel + " vs " + XLabel + " for: " + OutputSpecies.toStdString();
   MainTitle = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";
   MainTitle = Output_Controls_ptr->isCheckedOutputSpecies() ? OutputSpecies.toStdString() : MainTitle;

//   LineColors.push_back(QColor(  0,114,178)); // blue
//   LineColors.push_back(QColor(230,159,  0)); // orange
//   LineColors.push_back(QColor(  0,  0,  0)); // black
//   LineColors.push_back(QColor( 86,180,233)); // sky blue
//   LineColors.push_back(QColor( 90,180,172)); // teal
//   LineColors.push_back(QColor(230,230,230)); // gray

   m_ChartWidget->removeAllSeries();

   ChartLineData.clear();

   for (int line=0; line<NumLines; ++line)
   {
       Years.clear();
       ColumnLabelsForLegend.clear();
       for (int species=0; species<NumSpeciesOrGuilds; ++species) {
           if (species == SpeciesNum) {
               for (int i=0; i<TotalNumPoints; ++i) {
                   ChartLineData(i,line) = DiagnosticsFitness(i,species);
               }
           }
       }
   }

   // Set all line colors to be the first
   for (int i=1; i<int(nmfConstants::LineColors.size()); ++i) {
       LineColors.push_back(QColor(nmfConstants::LineColors[0].c_str()));
   }

   if (m_ChartWidget != nullptr) {
       nmfChartLine* lineChart = new nmfChartLine();
       lineChart->populateChart(m_ChartWidget,
                                ChartType,
                                LineStyle,
                                Output_Controls_ptr->getOutputLineWidthData(),
                                nmfConstantsMSSPM::ShowFirstPoint,
                                nmfConstants::DontShowLegend,
                                StartXValue,
                                Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                nmfConstantsMSSPM::LabelXAxisAsReals,
                                YMinVal,YMaxVal,
                                nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                ChartLineData,
                                RowLabelsForBars,
                                ColumnLabelsForLegend,
                                HoverLabels,
                                MainTitle,
                                XLabel,
                                YLabel,
                                Output_Controls_ptr->getOutputFontSizeLabel(),
                                Output_Controls_ptr->getOutputFontSizeNumber(),
                                Output_Controls_ptr->getOutputFont(),
                                Output_Controls_ptr->getOutputLineWidthAxis(),
                                Output_Controls_ptr->getOutputLineWidthAxis(),
                                {GridLines,GridLines},
                                Theme,
                                LineColors[0],
                                "",
                                XInc);

   }
}

/*
void
nmfMainWindow::showMohnsRhoBiomassVsTime(
        const std::string &label,
        const int &InitialYear,
        const int &StartYear,
        const int &NumSpecies,
        const QString &OutputSpecies,
        const int &SpeciesNum,
        const int MaxNumYears,
        std::vector<boost::numeric::ublas::matrix<double> > &Biomass,  // Catch or Calculated Biomass
        QString &ScaleStr,
        double &ScaleVal,
        double &YMinSliderVal,
        double &brightnessFactor,
        bool  isMonteCarloSimulation,
        bool  clearChart,
        QStringList ColumnLabelsForLegend)
{
    //
    // Draw the line chart
    //
    int NumLines = Biomass.size();
    std::vector<int> NumYearsVec;
    QList<QColor> LineColors;
    QColor lineColor;
    QStringList RowLabelsForBars;
    boost::numeric::ublas::matrix<double> ChartLineData;
    boost::numeric::ublas::matrix<double> ChartScatterData;
    ChartLineData.resize(MaxNumYears,NumLines);
    ChartLineData.clear();
    ChartScatterData.resize(MaxNumYears,1);
    ChartScatterData.clear();
    RowLabelsForBars.clear();
    std::string value;
    std::string LineStyle = "SolidLine";
    QStandardItem *item;
    std::string legendCode = "";
    std::string ChartType;
    std::string MainTitle;
    std::string XLabel;
    std::string YLabel;
    int Theme = 0; // Replace with checkbox values
    int NumColors;
    std::vector<bool> GridLines = {true,true}; // Replace with checkbox values
    std::vector<int> StartYears;
    QStringList HoverLabels;

    for (int i=0; i<NumLines; ++i) {
        NumYearsVec.push_back(Biomass[i].size1());
        StartYears.push_back(StartYear);
    }

    if (Diagnostic_Tab2_ptr->getPeelPosition() == "From Beginning") {
        StartYears.clear();
        int NumYearsPeeled = Diagnostic_Tab2_ptr->getNumPeels();
        for (int i=0; i<NumYearsPeeled; ++i) {
            StartYears.push_back(InitialYear+i+1);
        }
    }

    // Initialize matrix as all of the NumLines may not have the same number of years
    for (int j=0; j<NumLines; ++j) {
        for (int i=0; i<MaxNumYears; ++i) {
            ChartLineData(i,j) = nmfConstants::NoValueDouble;
        }
    }

    ChartType = "Line";
    MainTitle = label + " for: " + OutputSpecies.toStdString();
    XLabel    = "Year";
    YLabel    = label + " (" + ScaleStr.toStdString() + "mt)";

//    LineColors.push_back(QColor(  0,114,178)); // blue
//    LineColors.push_back(QColor(245,130, 49)); // orange
//    LineColors.push_back(QColor(128,  0,  0)); // maroon
//    LineColors.push_back(QColor( 86,180,233)); // sky blue
//    LineColors.push_back(QColor(230,230,230)); // gray
//    LineColors.push_back(QColor(  0,  0,  0)); // black

    NumColors = nmfConstants::LineColors.size();

    if (clearChart) {
        m_ChartWidget->removeAllSeries();
    }

    for (int line=0; line<NumLines; ++line)
    {
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                for (int time=StartYears[line]-InitialYear; time<NumYearsVec[line]; ++time) {
                    legendCode = "Mohn's Rho Analysis";
                    ChartLineData(time,line) = Biomass[line](time,species)/ScaleVal;
                    item = new QStandardItem(QString::fromStdString(value));
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
    }

    // Use same color for all Mho's Rho plots since it's obvious which lines
    // refer to which time periods by their length
    lineColor = QColor(nmfConstants::LineColors[0].c_str());
    for (int i=0; i<NumColors; ++i) {
        LineColors.push_back(lineColor);
    }

    if (m_ChartWidget != nullptr) {
        nmfChartLine* lineChart = new nmfChartLine();
        lineChart->populateChart(m_ChartWidget,
                                 ChartType,
                                 LineStyle,
                                 nmfConstantsMSSPM::ShowFirstPoint,
                                 nmfConstants::DontShowLegend,
                                 StartYear,
                                 nmfConstantsMSSPM::LabelXAxisAsInts,
                                 YMinSliderVal,
                                 nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                 ChartLineData,
                                 RowLabelsForBars,
                                 ColumnLabelsForLegend,
                                 HoverLabels,
                                 MainTitle,
                                 XLabel,
                                 YLabel,
                                 GridLines,
                                 Theme,
                                 LineColors[0],
                                 "",
                                 1.0);
    }
}
*/



void
nmfMainWindow::getMonteCarloUncertaintyData(
        const QString& Species,
        QList<QString>& formattedUncertaintyData)
{
    int RunLength;
    int NumRuns;
    int NumRecords;
    int StartYearForecast;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string queryStr;
    std::string ForecastName = Forecast_Tab1_ptr->getForecastName();
    std::string str;
    std::string ForecastHarvestType;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;
    std::string msg;

    if (! m_DatabasePtr->getForecastInfo(
                m_ProjectName,m_ModelName,ForecastName,RunLength,StartYearForecast,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,ForecastHarvestType,NumRuns)) {
        return;
    }

    fields    = {"ForecastName","RunNum","SpeName","Algorithm","Minimzer",
                 "ObjectiveCriterion","Scaling","GrowthRate","GrowthRateShape",
                 "CarryingCapacity","Harvest"};
    queryStr  = "SELECT ForecastName,RunNum,SpeName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,GrowthRate,GrowthRateShape,CarryingCapacity,Harvest FROM " +
                 nmfConstantsMSSPM::TableForecastMonteCarloParameters +
                " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND ForecastName = '"       + ForecastName +
                "' AND SpeName = '"            + Species.toStdString() +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' ORDER BY RunNum,SpeName";

    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords < NumRuns) {
        msg  = "Couldn't find sufficient uncertainty records in ForecastMonteCarloParameters ";
        msg += "Found " + std::to_string(NumRecords) + " table records, but there are " +
                std::to_string(NumRuns) + " runs.";
        m_Logger->logMsg(nmfConstants::Error,msg);
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        for (int i=0; i<NumRuns; ++i) {
            formattedUncertaintyData << "";
        }
        return;
    }
    formattedUncertaintyData.clear();
    for (int i=0; i<NumRuns; ++i) {
        str  = "(";
        str += QString::number(100.0*std::stod(dataMap["GrowthRate"][i]),'g',1).toStdString()       + "%, " +
               QString::number(100.0*std::stod(dataMap["CarryingCapacity"][i]),'g',1).toStdString() + "%, " +
               QString::number(100.0*std::stod(dataMap["Harvest"][i]),'g',1).toStdString()          + "%";
        str += ")";
        formattedUncertaintyData.push_back(QString::fromStdString(str));
    }
}


void
nmfMainWindow::showBiomassVsTimeForMultipleRuns(
        const std::string& ChartTitle,
        const std::string& XAxisLabel,
        const std::string& YAxisLabel,
        const bool& showHistoricalData,
        const int& StartYear,
        const int& NumObservedYears,
        const int& NumSpecies,
        const QString& OutputSpecies,
        const int& SpeciesNum,
        const int NumYears,
        std::vector<boost::numeric::ublas::matrix<double> >& Biomass,  // Catch or Calculated Biomass
        QString& ScaleStr,
        double& ScaleVal,
        double& YMinVal,
        double& brightnessFactor,
        bool  isMonteCarloSimulation,
        bool  isEnsemble,
        bool  clearChart,
        QStringList ColumnLabelsForLegend)
{
    //
    // Draw the line chart
    //
    bool ShowLegend = Output_Controls_ptr->isCheckedOutputLegend();
    int NumLines = Biomass.size();
    double YMaxVal = nmfConstants::NoValueDouble;
    QList<QColor> LineColors;
    QStringList RowLabelsForBars;
    boost::numeric::ublas::matrix<double> ChartLineData;
    boost::numeric::ublas::matrix<double> ChartScatterData;
    ChartLineData.resize(NumYears,NumLines);
    ChartLineData.clear();
    ChartScatterData.resize(NumYears,1);
    ChartScatterData.clear();
    RowLabelsForBars.clear();
    std::string value;
    std::string LineStyleSolid  = "SolidLine";
    std::string LineStyleDotted = "DottedLine";
    std::string LineStyleDashed = "DashedLine";
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList Years;
    std::string legendCode = "";
    QStandardItemModel* smodel = new QStandardItemModel( NumYears, NumLines );
    std::string ChartType;
    std::string MainTitle;
    std::string XLabel;
    std::string YLabel;
    int Theme = 0; // Replace with checkbox values
//  int NumColors;
    bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
    QStringList HoverLabels;
    QList<QString> formattedUncertaintyData;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    ChartType = "Line";
    MainTitle = ChartTitle + " for: " + OutputSpecies.toStdString();
    MainTitle = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";
    MainTitle = Output_Controls_ptr->isCheckedOutputSpecies() ? OutputSpecies.toStdString() : MainTitle;
    XLabel    = XAxisLabel;
    YLabel    = YAxisLabel + " (" + ScaleStr.toStdString() + "mt)";

//  NumColors = nmfConstants::LineColors.size();
    LineColors.append(QColor(nmfConstants::LineColors[0].c_str()));

    if (clearChart) {
        m_ChartWidget->removeAllSeries();
    }

    formattedUncertaintyData << "(0%, 0%, 0%)";
    if (isMonteCarloSimulation) {
        getMonteCarloUncertaintyData(OutputSpecies,formattedUncertaintyData);
    }

    ChartLineData.clear();
    for (int line=0; line<NumLines; ++line)
    {
        Years.clear();
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                if ((ChartTitle != "Multi-Forecast Scenario") &&
                    (ChartTitle != "Ensemble Biomass")) {
                      HoverLabels << formattedUncertaintyData[line];
                }
                for (int time=0; time<NumYears; ++time) {
                    legendCode = (isMonteCarloSimulation) ? "Monte Carlo Sim "+std::to_string(time+1) : "Forecast Biomass";
                    Years << QString::number(StartYear+time);
                    ChartLineData(time,line) = Biomass[line](time,species)/ScaleVal;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                ChartLineData(time,line),m_NumSignificantDigits,6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(time, line, item);
                }
            }
        }
    }

    // Call the chart api function that will populate and annotate the
    // desired chart type.
    if (isMonteCarloSimulation || isEnsemble) {
        brightnessFactor /= 3.0;
        LineColors[0] = QColor(255-brightnessFactor*255,
                               255-brightnessFactor*255,
                               255-brightnessFactor*255);
    } else {
//    if (! isMonteCarloSimulation && ! isEnsemble) {
        LineColors[0] = QColor(150,150,255);
    }

    if (m_ChartWidget != nullptr) {
        std::string lineColorName = "MonteCarloSimulation";
        nmfChartLine* lineChart = new nmfChartLine();
        if (ChartTitle == "Multi-Forecast Scenario") {
            lineColorName = "Multi-Forecast Scenario";
            HoverLabels   = ColumnLabelsForLegend;
//            ShowLegend    = true;
        } else if (ChartTitle == "Ensemble Biomass") {
            HoverLabels   = ColumnLabelsForLegend;
            ShowLegend = false;
        }

        lineChart->populateChart(m_ChartWidget,
                                 ChartType,
                                 LineStyleSolid,
                                 Output_Controls_ptr->getOutputLineWidthData(),
                                 nmfConstantsMSSPM::ShowFirstPoint,
                                 ShowLegend,
                                 StartYear,
                                 Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                 nmfConstantsMSSPM::LabelXAxisAsInts,
                                 YMinVal,YMaxVal,
                                 nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                 ChartLineData,
                                 RowLabelsForBars,
                                 ColumnLabelsForLegend,
                                 HoverLabels,
                                 MainTitle,
                                 XLabel,
                                 YLabel,
                                 Output_Controls_ptr->getOutputFontSizeLabel(),
                                 Output_Controls_ptr->getOutputFontSizeNumber(),
                                 Output_Controls_ptr->getOutputFont(),
                                 Output_Controls_ptr->getOutputLineWidthAxis(),
                                 Output_Controls_ptr->getOutputLineWidthAxis(),
                                 {GridLines,GridLines},
                                 Theme,
                                 LineColors[0],
                                 lineColorName,
                                 1.0);

        if (showHistoricalData) {
            int NumXValues = (int)ChartLineData.size1();
            lineChart->overlayVerticalLine(
                        m_ChartWidget,
                        LineStyleDotted,
                        Output_Controls_ptr->getOutputLineWidthData(),
                        nmfConstantsMSSPM::ShowFirstPoint,
                        ShowLegend,
                        StartYear,
                        Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                        nmfConstantsMSSPM::LabelXAxisAsInts,
                        StartYear+NumObservedYears-1,
                        YMinVal,YMaxVal,
                        nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                        NumXValues,
                        "Separator for estimated plot on left and forecast(s) on right",
                        XLabel, YLabel,
                        Output_Controls_ptr->getOutputFontSizeLabel(),
                        Output_Controls_ptr->getOutputFontSizeNumber(),
                        Output_Controls_ptr->getOutputFont(),
                        Output_Controls_ptr->getOutputLineWidthAxis(),
                        Output_Controls_ptr->getOutputLineWidthAxis(),
                        nmfConstants::SkyBlue,
                        {GridLines,GridLines},
                        1.0);
        }
    }

    // Load ChartData into Output table view
    SpeciesNames << OutputSpecies;
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    smodel->setVerticalHeaderLabels(Years);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->resizeColumnsToContents();
    m_UI->OutputDataTV->show();
}

std::string
nmfMainWindow::getLegendCode(bool isAveraged,
                             std::string &Algorithm,
                             std::string &Minimizer,
                             std::string &ObjectiveCriterion,
                             std::string &Scaling)
{
    std::string code;
    std::string theMinimizer = Minimizer;

    if (Algorithm == "NLopt Algorithm") {
        code += "NL-";
        if (Minimizer == "GN_ORIG_DIRECT_L")
            code += "GnODL-";
        else if (Minimizer == "GN_DIRECT_L")
            code += "GnDL-";
        else if (Minimizer == "GN_DIRECT_L_RAND")
            code += "GnDLR-";
        else if (Minimizer == "GN_CRS2_LM")
            code += "GnCRS2-";
        else if (Minimizer == "GN_ESCH")
            code += "GnESCH-";
        else if (Minimizer == "GD_StoGO")
            code += "GdStoGO-";
        else if (Minimizer == "LN_COBYLA")
            code += "LnCoby-";
        else if (Minimizer == "LN_NELDERMEAD")
            code += "LnNlMd-";
        else if (Minimizer == "LN_SBPLX")
            code += "LnSBPLX-";
        else if (Minimizer == "LD_LBFGS")
            code += "LdLBFGS-";
        else if (Minimizer == "LD_MMA")
            code += "LdMMA-";
        else
            code += "?-";
    } else if (Algorithm == "Bees Algorithm") {
        code += "BE-";
    } else if (Algorithm == "Genetic Algorithm") {
        code += "GA-";
    } else if (Algorithm == "Gradient-Based Algorithm") {
        code += "GR-";
        if (Minimizer == "Gradient Descent")
            code += "GrDs-";
        else if (Minimizer == "Conjugated Gradient Descent")
            code += "CoGrDs-";
        else if (Minimizer == "Port Minimizer")
            code += "PtMn-";
        else {
            boost::erase_all(theMinimizer,"-");
            code += theMinimizer+"-";
        }
    } else if (isAveraged) {
        code += Algorithm + " Ave"; // This will result in the average algorithm named being displayed
    }

    if (ObjectiveCriterion == "Least Squares")
        code += "LsSq-";
    else if (ObjectiveCriterion == "Maximum Likelihood")
        code += "MLE";
    else if (ObjectiveCriterion == "Model Efficiency")
        code += "ModEff-";

    if (ObjectiveCriterion != "Maximum Likelihood") {
        if (Scaling == "Min Max")
            code += "Mm";
        else if (Scaling == "Mean")
            code += "Mn";
    }

    return code;
}

void
nmfMainWindow::showChartBiomassVsTime(
        const int &NumSpecies,
        const QString &OutputSpecies,
        const int &SpeciesNum,
        const int &RunLength,
        const int &StartYear,
        std::vector<std::string> &Algorithms,
        std::vector<std::string> &Minimizers,
        std::vector<std::string> &ObjectiveCriteria,
        std::vector<std::string> &Scalings,
        std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass,
        boost::numeric::ublas::matrix<double> &ObservedBiomass,
        QList<double> &BMSYValues,
        QString &ScaleStr,
        double &ScaleVal,
        const bool& clearChart,
        double &YMinVal,
        const std::string& title)
{
    //
    // Draw the line chart
    //
    bool showLegend = Output_Controls_ptr->isCheckedOutputLegend();
    QColor ScatterColor;
    QColor ChartColor;
    QString colorName;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    boost::numeric::ublas::matrix<double> ChartBMSYData;
    boost::numeric::ublas::matrix<double> ChartLineData;
    boost::numeric::ublas::matrix<double> ChartScatterData;
    boost::numeric::ublas::matrix<bool>   SkipScatterData;
    int NumLines = OutputBiomass.size();
    bool isMohnsRho = isAMohnsRhoMultiRun();
    double YMaxVal = nmfConstants::NoValueDouble;

    ChartBMSYData.resize(RunLength+1,1); // NumSpecies);
    ChartBMSYData.clear();
    ChartLineData.resize(RunLength+1,NumLines); // NumSpecies);
    ChartLineData.clear();
    ChartScatterData.resize(RunLength+1,1); // NumSpecies);
    ChartScatterData.clear();
    SkipScatterData.resize(RunLength+1,1);
    SkipScatterData.clear();
    RowLabelsForBars.clear();
    ColumnLabelsForLegend.clear();
    double value;
    std::string LineStyle = "SolidLine";
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList Years;
    QStandardItemModel *smodel = new QStandardItemModel( RunLength+1, 1 );
    QLocale locale(QLocale::English);
    QString valueWithComma;
    std::string ChartType;
    std::string MainTitle;
    std::string XLabel;
    std::string YLabel;
    std::vector<std::string> legendCode;
    bool AddScatter = true; //(NumLines == 1);
    int Theme = 0; // Replace with checkbox values
    bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
    bool xAxisIsInteger = true;
    int NumLineColors = nmfConstants::LineColors.size();
    QStringList HoverLabels;
    bool isAveraged = isAMultiOrMohnsRhoRun();
    std::string TitlePrefix = (isAveraged) ? "Average " : "";
    ChartType = "Line";
    MainTitle = TitlePrefix + title + " for: " + OutputSpecies.toStdString();
    MainTitle = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";
    MainTitle = Output_Controls_ptr->isCheckedOutputSpecies() ? OutputSpecies.toStdString() : MainTitle;
    XLabel    = "Year";
    YLabel    = "Biomass (" + ScaleStr.toStdString() + "mt)";
    ScatterColor = QColor(0,191,255); // deepskyblue

    if (clearChart) {
        m_ChartWidget->removeAllSeries();
    }

//    QAbstract3DAxis* axisX = qobject_cast<QAbstract3DAxis*>(m_ChartWidget->axisX());
//    m_ChartWidget->removeAxis(qobject_cast<QtDataVisualization::QAbstract3DAxis*>(axisX));
    //  Load Scatter plot points into data structure
    double val = 0;
    for (int species=0; species<NumSpecies; ++species) {
        if (species == SpeciesNum) {
            for (int time=0; time<=RunLength; ++time) {
                val = ObservedBiomass(time,species);
                // Check for missing biomass data and don't display the first year's observed biomass
                if ((val != nmfConstantsMSSPM::NoData) && (time != 0)) {
                    ChartScatterData(time,0) = val/ScaleVal;
                } else {
                    SkipScatterData(time,0) = true;
                }
            }
            break;
        }
    }

    nmfChartLineWithScatter* lineWithScatterChart = new nmfChartLineWithScatter();

    for (int line=0; line<NumLines; ++line)
    {
        if (title == nmfConstantsMSSPM::TitleChartBiomassAndCatch) {
            legendCode.push_back("Catch");
        } else {
            legendCode.push_back(getLegendCode(isAveraged,
                                               Algorithms[line],
                                               Minimizers[line],
                                               ObjectiveCriteria[line],
                                               Scalings[line]));
        }
    }

    // Load Line plot points into data structure and update the chart
    for (int line=0; line<NumLines; ++line)
    {
        ColumnLabelsForLegend.clear();
        colorName = getColorName(line);
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                for (int time=0; time<=RunLength; ++time) {
                    Years << QString::number(StartYear+time);
                    value = OutputBiomass[line](time,species);
                    ChartLineData(time,line) = value/ScaleVal;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                value,m_NumSignificantDigits,6);
                    item = new QStandardItem(valueWithComma);
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(time, 0, item);
                    RowLabelsForBars << QString::number(StartYear+time);
                }
                break;
            }
        }

        // Call the chart api function that will populate and annotate the
        // desired chart type.
        ColumnLabelsForLegend << QString::fromStdString(legendCode[line]);
        if (m_ChartWidget != nullptr) {
            lineWithScatterChart->populateChart(m_ChartWidget,
                                                ChartType,
                                                LineStyle,
                                                Output_Controls_ptr->getOutputLineWidthData(),
                                                Output_Controls_ptr->getOutputLineWidthPoint(),
                                                isMohnsRho,
                                                nmfConstantsMSSPM::ShowFirstPoint, //HideFirstPoint,
                                                AddScatter,
                                                StartYear,
                                                Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                                xAxisIsInteger,
                                                YMinVal,YMaxVal,
                                                ChartLineData,
                                                ChartScatterData,
                                                SkipScatterData,
                                                RowLabelsForBars,
                                                ColumnLabelsForLegend,
                                                MainTitle,
                                                XLabel,
                                                YLabel,
                                                Output_Controls_ptr->getOutputFontSizeLabel(),
                                                Output_Controls_ptr->getOutputFontSizeNumber(),
                                                Output_Controls_ptr->getOutputFont(),
                                                Output_Controls_ptr->getOutputLineWidthAxis(),
                                                Output_Controls_ptr->getOutputLineWidthAxis(),
                                                {GridLines,GridLines},
                                                Theme,
                                                ChartColor,
                                                ScatterColor,
                                                nmfConstants::LineColors[line%NumLineColors],
                                                colorName.toStdString(),{},
                                                showLegend); //nmfConstantsMSSPM::ShowLegend);
        }

        AddScatter = (line+1 == NumLines-1);
    } // end for line

    if (Output_Controls_ptr->isCheckedOutputBMSY()) {

        nmfChartLegend* legend = new nmfChartLegend(m_ChartWidget);
        std::vector<QString> markerColors;
        LineStyle = "DottedLine";
        ColumnLabelsForLegend.clear();

        for (int line=0; line<NumLines; ++line) {

            ColumnLabelsForLegend << "B MSY";

            // Draw the BMSY line
            for (int i=0; i<NumSpecies; ++i) {
                if (i == SpeciesNum) {
                    for (int j=0; j<=RunLength; ++j) {
                        ChartBMSYData(j,0) = BMSYValues[i+line*NumSpecies]/ScaleVal;
                    }
                    break;
                }
            }

            if (m_ChartWidget != nullptr) {

                colorName = getColorName(line);

                nmfChartLine* lineChart = new nmfChartLine();
                lineChart->populateChart(m_ChartWidget,
                                         ChartType,
                                         LineStyle,
                                         Output_Controls_ptr->getOutputLineWidthData(),
                                         nmfConstantsMSSPM::ShowFirstPoint,
                                         showLegend, //nmfConstants::ShowLegend,
                                         StartYear,
                                         Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                         xAxisIsInteger,
                                         YMinVal,YMaxVal,
                                         nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                         ChartBMSYData,
                                         RowLabelsForBars,
                                         ColumnLabelsForLegend,
                                         ColumnLabelsForLegend, //HoverLabels,
                                         MainTitle,
                                         XLabel,
                                         YLabel,
                                         Output_Controls_ptr->getOutputFontSizeLabel(),
                                         Output_Controls_ptr->getOutputFontSizeNumber(),
                                         Output_Controls_ptr->getOutputFont(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         {GridLines,GridLines},
                                         Theme,
                                         QColor(nmfConstants::LineColors[line%NumLineColors].c_str()),
                                         colorName.toStdString(),
                                         1.0);


                markerColors.push_back(colorName);
            }
        }

        // Add color for Biomass Obs
        markerColors.push_back("Deep Sky Blue");
        // Set up the legend for tooltips.  Needs to be done like this due to the
        // B MSY labels being the same for all of the B MSY dotted line plots.
        legend->setToolTips(markerColors);
        legend->setupConnections();
    }

    //
    // Load ChartData into Output table view
    SpeciesNames << OutputSpecies;
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    smodel->setVerticalHeaderLabels(Years);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->resizeColumnsToContents();
}

void
nmfMainWindow::showChartBiomassVsTimeMultiRunWithScatter(
        const int &NumSpecies,
        const QString &OutputSpecies,
        const int &SpeciesNum,
        const int &RunLength,
        const int &StartYear,
        std::vector<std::string> &Algorithms,
        std::vector<std::string> &Minimizers,
        std::vector<std::string> &ObjectiveCriteria,
        std::vector<std::string> &Scalings,
        std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass,
        boost::numeric::ublas::matrix<double> &ObservedBiomass,
        QList<double> &BMSYValuesAveraged,
        QString &ScaleStr,
        double &ScaleVal,
        const bool& clearChart,
        double &YMinVal,
        const QList<QString>& lineLabels)
{
    //
    // Draw the line chart
    //
    double YMaxVal = nmfConstants::NoValueDouble;
    QColor ScatterColor;
    QColor ChartColor;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    boost::numeric::ublas::matrix<double> ChartBMSYData;
    boost::numeric::ublas::matrix<double> ChartLineData;
    boost::numeric::ublas::matrix<double> ChartScatterData;
    boost::numeric::ublas::matrix<double> SkipScatterData;
    int NumLines = OutputBiomass.size();
    bool isMohnsRho = isAMohnsRhoMultiRun();
    ChartBMSYData.resize(RunLength+1,1); // NumSpecies);
    ChartBMSYData.clear();
    ChartLineData.resize(RunLength+1,NumLines); // NumSpecies);
    ChartLineData.clear();
    ChartScatterData.resize(RunLength+1,1); // NumSpecies);
    ChartScatterData.clear();
    SkipScatterData.resize(RunLength+1,1); // NumSpecies);
    SkipScatterData.clear();
    RowLabelsForBars.clear();
    ColumnLabelsForLegend.clear();
    double value;
    std::string LineStyle = "SolidLine";
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList Years;
    QStandardItemModel *smodel = new QStandardItemModel( RunLength+1, 1 );
    std::string ChartType;
    std::string MainTitle;
    std::string XLabel;
    std::string YLabel;
    std::vector<std::string> legendCode;
    bool AddScatter = true; //(NumLines == 1);
    int Theme = 0; // Replace with checkbox values
    bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
    bool xAxisIsInteger = true;
    int NumLineColors = nmfConstants::LineColors.size();
    QStringList HoverLabels;
    bool isAveraged = isAMultiOrMohnsRhoRun();
    std::string TitlePrefix = (isAveraged) ? "Average " : "";
    ChartType = "Line";
    MainTitle = TitlePrefix + "B(calc) with B(obs) points for: " + OutputSpecies.toStdString();
    MainTitle = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";
    MainTitle = Output_Controls_ptr->isCheckedOutputSpecies() ? OutputSpecies.toStdString() : MainTitle;
    XLabel    = "Year";
    YLabel    = "Biomass (" + ScaleStr.toStdString() + "mt)";
    ScatterColor = QColor(0,191,255); // deepskyblue
    QString lineLabel;
    bool showLegend = Output_Controls_ptr->isCheckedOutputLegend();

    if (clearChart) {
        m_ChartWidget->removeAllSeries();
    }

    //  Load Scatter plot points into data structure
    double val = 0;
    for (int species=0; species<NumSpecies; ++species) {
        if (species == SpeciesNum) {
            for (int time=0; time<=RunLength; ++time) {
                val = ObservedBiomass(time,species);
                if ((val != nmfConstantsMSSPM::NoData) && (time != 0)) {
                    ChartScatterData(time,0) = val/ScaleVal;
                } else {
                    SkipScatterData(time,0) = true;
                }
            }
            break;
        }
    }

    nmfChartLineWithScatter* lineWithScatterChart = new nmfChartLineWithScatter();

    if (lineLabels.size() == 0 || OutputBiomass.size() == 0) {
        return;
    }
    // Load individual run data into appropriate structures
    int ModifiedRunLength = RunLength;
    for (int line=0; line<NumLines; ++line)  {
        legendCode.push_back(getLegendCode(isAveraged,
                                           Algorithms[line],
                                           Minimizers[line],
                                           ObjectiveCriteria[line],
                                           Scalings[line]));

        // Load Line plot points into data structure and update the chart
        lineLabel = lineLabels[line];
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                for (int time=0; time<=ModifiedRunLength; ++time) {
                    Years << QString::number(StartYear+time);
                    value = OutputBiomass[line](time,species);
                    ChartLineData(time,line) = value/ScaleVal;
                    item = new QStandardItem(QString::number(value,'f',6));
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(time, 0, item);
                    RowLabelsForBars << QString::number(StartYear+time);
                }
                break;
            }
        }
        ModifiedRunLength = (isMohnsRho) ? ModifiedRunLength-1 : ModifiedRunLength;
    }

    // Draw set of individual lines in light gray color
    ColumnLabelsForLegend.clear();
//  ColumnLabelsForLegend << QString::fromStdString(legendCode[line]);
    if (m_ChartWidget != nullptr) {
        lineWithScatterChart->populateChart(m_ChartWidget,
                                            ChartType,
                                            LineStyle,
                                            Output_Controls_ptr->getOutputLineWidthData(),
                                            Output_Controls_ptr->getOutputLineWidthPoint(),
                                            isMohnsRho,
                                            nmfConstantsMSSPM::ShowFirstPoint,
                                            AddScatter,
                                            StartYear,
                                            Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                            xAxisIsInteger,
                                            YMinVal,YMaxVal,
                                            ChartLineData,
                                            ChartScatterData,
                                            SkipScatterData,
                                            RowLabelsForBars,
                                            ColumnLabelsForLegend,
                                            MainTitle,
                                            XLabel,
                                            YLabel,
                                            Output_Controls_ptr->getOutputFontSizeLabel(),
                                            Output_Controls_ptr->getOutputFontSizeNumber(),
                                            Output_Controls_ptr->getOutputFont(),
                                            Output_Controls_ptr->getOutputLineWidthAxis(),
                                            Output_Controls_ptr->getOutputLineWidthAxis(),
                                            {GridLines,GridLines},
                                            Theme,
                                            ChartColor,
                                            ScatterColor,
                                            "lightGray", //nmfConstants::LineColors[line%NumLineColors],
                                            "",
                                            lineLabels,
                                            nmfConstantsMSSPM::DontShowLegend);

    }


    if (Output_Controls_ptr->isCheckedOutputBMSY()) {
        double BMSYValue = 0;
        nmfChartLegend* legend = new nmfChartLegend(m_ChartWidget);
        std::vector<QString> markerColors;
        LineStyle = "DottedLine";
        ColumnLabelsForLegend.clear();

        // Draw the BMSY line
        for (int i=0; i<NumSpecies; ++i) {
            if (i == SpeciesNum) {
                BMSYValue = BMSYValuesAveraged[i]/ScaleVal;
                for (int j=0; j<=RunLength; ++j) {
                    ChartBMSYData(j,0) = BMSYValue;
                }
                Output_Controls_ptr->setTextOutputBMSY(QString::number(BMSYValue));
                break;
            }
        }

        for (int line=0; line<NumLines; ++line) {
            ColumnLabelsForLegend << "B MSY";
            if (m_ChartWidget != nullptr) {
                nmfChartLine* lineChart = new nmfChartLine();
                lineChart->populateChart(m_ChartWidget,
                                         ChartType,
                                         LineStyle,
                                         Output_Controls_ptr->getOutputLineWidthData(),
                                         nmfConstantsMSSPM::ShowFirstPoint,
                                         showLegend,
                                         StartYear,
                                         Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                         xAxisIsInteger,
                                         YMinVal,YMaxVal,
                                         nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                         ChartBMSYData,
                                         RowLabelsForBars,
                                         ColumnLabelsForLegend,
                                         ColumnLabelsForLegend, //HoverLabels,
                                         MainTitle,
                                         XLabel,
                                         YLabel,
                                         Output_Controls_ptr->getOutputFontSizeLabel(),
                                         Output_Controls_ptr->getOutputFontSizeNumber(),
                                         Output_Controls_ptr->getOutputFont(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         Output_Controls_ptr->getOutputLineWidthAxis(),
                                         {GridLines,GridLines},
                                         Theme,
                                         QColor(nmfConstants::LineColors[line%NumLineColors].c_str()),
                                         "", //lineLabel.toStdString(),
                                         1.0);
                // markerColors.push_back(lineLabel);
            }
        }

        // Add color for Biomass Obs
        // markerColors.push_back("Deep Sky Blue");
        // Set up the legend for tooltips.  Needs to be done like this due to the
        // B MSY labels being the same for all of the B MSY dotted line plots.
        // legend->setToolTips(markerColors);
        legend->setupConnections();
    } else {
        Output_Controls_ptr->clearOutputBMSY();
    }

    //
    // Load ChartData into Output table view
//    SpeciesNames << OutputSpecies;
//    smodel->setHorizontalHeaderLabels(SpeciesNames);
//    smodel->setVerticalHeaderLabels(Years);
//    m_UI->OutputDataTV->setModel(smodel);

}

// If need to repeat colors, append a digit 1, 2, etc...regarding the number of times
// you sequence through all of the colors
QString
nmfMainWindow::getColorName(int line)
{
    QString colorName;
    int suffix = line/nmfConstants::LineColorNames.size();
    int NumLineColors = nmfConstants::LineColors.size();

    colorName = QString::fromStdString(nmfConstants::LineColorNames[line%NumLineColors]);
    if (suffix > 0) {
        colorName += QString::number(suffix);
    }
    return colorName;
}

void
nmfMainWindow::showChartBcVsTimeSelectedSpecies(QList<int> &RowNumList,
                                                QList<QString> &RowNameList)
{
    std::string LineStyle = "SolidLine";
    QColor ChartColor;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    std::string ChartType = "Line";
    std::string type = (Output_Controls_ptr->getOutputChartType() == "(Abs) Bc vs Time") ?
                        "Abs" : "Rel";
    std::string MainTitle = "Calculated (" + type + ") Biomass for Selected Species";
    std::string XLabel    = "Year";
    std::string YLabel    = "Biomass (mt)";
    bool GridLines = Output_Controls_ptr->isCheckedOutputGridLines();
    int Theme             = 0;              // Replace with combobox value
    int m = 0;
    int NumSpecies = RowNumList.size();
    int RunLength  = m_SpeciesCalculatedBiomassData.size1();
    QStandardItem *item;
    QList<QColor> LineColors;
    double YMinVal = nmfConstants::NoValueDouble;
    double YMaxVal = nmfConstants::NoValueDouble;
    QStringList HoverLabels;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    MainTitle  = Output_Controls_ptr->isCheckedOutputTitle() ? MainTitle : "";

    if (NumSpecies == 0) {
        m_ChartWidget->removeAllSeries();
        return;
    }
    boost::numeric::ublas::matrix<double> SelectedSpeciesCalculatedBiomassData;
    QStandardItemModel* smodel = new QStandardItemModel( RunLength, NumSpecies );
    SelectedSpeciesCalculatedBiomassData.resize(RunLength,NumSpecies);
    SelectedSpeciesCalculatedBiomassData.clear();

    int iprime=0;
    for (unsigned i=0; i<m_SpeciesCalculatedBiomassData.size2(); ++i) {
        if (RowNumList.isEmpty() || RowNumList.contains(i)) {
            ColumnLabelsForLegend << RowNameList[iprime];
            LineColors.append(LINE_COLORS[i % LINE_COLORS.size()]);
            for (int j=0; j<=RunLength; ++j) {
                SelectedSpeciesCalculatedBiomassData(j,iprime) = m_SpeciesCalculatedBiomassData(j,i);
//              valueWithComma = locale.toString(SelectedSpeciesCalculatedBiomassData(j,iprime),'f',3);
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            SelectedSpeciesCalculatedBiomassData(j,iprime),m_NumSignificantDigits,3);
                item = new QStandardItem(valueWithComma);
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(j, iprime, item);
            }
            iprime++;
        } else {
            m += RunLength;
        }
    }

    m_ChartWidget->removeAllSeries();
    // Call the chart api function that will populate and annotate the
    // desired chart type.
    if (m_ChartWidget != nullptr) {
        ChartColor = QColor(0,0,175); // darkblue //Qt::blue;
        nmfChartLine* lineChart = new nmfChartLine();
        lineChart->populateChart(m_ChartWidget,
                                 ChartType,
                                 LineStyle,
                                 Output_Controls_ptr->getOutputLineWidthData(),
                                 nmfConstantsMSSPM::HideFirstPoint,
                                 nmfConstants::DontShowLegend,
                                 0,
                                 Output_Controls_ptr->isCheckedOutputRescaleXAxis(),
                                 false,
                                 YMinVal,YMaxVal,
                                 nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                 SelectedSpeciesCalculatedBiomassData,
                                 RowLabelsForBars,
                                 ColumnLabelsForLegend,
                                 HoverLabels,
                                 MainTitle,
                                 XLabel,
                                 YLabel,
                                 Output_Controls_ptr->getOutputFontSizeLabel(),
                                 Output_Controls_ptr->getOutputFontSizeNumber(),
                                 Output_Controls_ptr->getOutputFont(),
                                 Output_Controls_ptr->getOutputLineWidthAxis(),
                                 Output_Controls_ptr->getOutputLineWidthAxis(),
                                 {GridLines,GridLines},
                                 Theme,
                                 {},
                                 "",
                                 1.0);
    }

    //
    // Load ChartData into Output table view
    smodel->setHorizontalHeaderLabels(ColumnLabelsForLegend);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->resizeColumnsToContents();
    m_UI->OutputDataTV->show();
}

/*
void
nmfMainWindow::showChartBcVsTimeAllSpecies(
        std::string type,
        const int &NumSpecies,
        const QList<int> &RowList,
        const int &RunLength,
        std::map<std::string, std::vector<std::string> > &dataMapSpecies,
        std::map<std::string, std::vector<std::string> > &dataMapCalculatedBiomass,
        QString &ScaleStr,
        double &ScaleVal)
{
    int m;
    QColor ChartColor;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    m_SpeciesCalculatedBiomassData.resize(RunLength+1,NumSpecies);
    m_SpeciesCalculatedBiomassData.clear();
    RowLabelsForBars.clear();
    ColumnLabelsForLegend.clear();
    std::string value;
    QStandardItem *item;
    QStandardItemModel* smodel = new QStandardItemModel( RunLength+1, NumSpecies );
    QList<QColor> LineColors;
    std::string LineStyle = "SolidLine";
    double biomassData;
    double YMinSliderVal = -1;
    QStringList HoverLabels;

    m = 0;
    double val;
    double den=1;
    for (int i=0; i<NumSpecies; ++i) {
        ColumnLabelsForLegend << QString::fromStdString(dataMapSpecies["SpeName"][i]);
        LineColors.append(LINE_COLORS[i % LINE_COLORS.size()]);
        for (int j=0; j<=RunLength; ++j) {
            value = dataMapCalculatedBiomass["Value"][m++];
            val = std::stod(value);
            if (type == "Rel") {
                den = (j   == 0) ? val : den;
                den = (den == 0) ? 1   : den;
            }
            biomassData = (val/den)/ScaleVal;
            m_SpeciesCalculatedBiomassData(j,i) = biomassData;
            item = new QStandardItem(QString::number(biomassData,'f',3));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(j, i, item);
            RowLabelsForBars << QString::number(j);
        }
    }

    std::string ChartType = "Line";
    std::string MainTitle = "Calculated (" + type + ") Biomass for Selected Species";
    std::string XLabel    = "Year";
    std::string YLabel    = "Biomass (" + ScaleStr.toStdString() + "mt)";
    std::vector<bool> GridLines = {true,true}; // Replace with checkbox values
    int Theme             = 0;              // Replace with combobox value

    m_ChartWidget->removeAllSeries();
    // Call the chart api function that will populate and annotate the
    // desired chart type.
    if (m_ChartWidget != nullptr) {
        ChartColor = QColor(0,0,175); // darkblue //Qt::blue;
        nmfChartLine* lineChart = new nmfChartLine();
        lineChart->populateChart(m_ChartWidget,
                                 ChartType,
                                 LineStyle,
                                 nmfConstantsMSSPM::HideFirstPoint,
                                 nmfConstants::DontShowLegend,
                                 0,
                                 false,
                                 YMinSliderVal,
                                 nmfConstantsMSSPM::DontLeaveGapsWhereNegative,
                                 m_SpeciesCalculatedBiomassData,
                                 RowLabelsForBars,
                                 ColumnLabelsForLegend,
                                 HoverLabels,
                                 MainTitle,
                                 XLabel,
                                 YLabel,
                                 GridLines,
                                 Theme,
                                 {},
                                 "",
                                 1.0);
    }

    // Load ChartData into Output table view
    smodel->setHorizontalHeaderLabels(ColumnLabelsForLegend);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->show();
}


void
nmfMainWindow::showChartCatchVsBc(
        const int&     NumSpecies,
        const QString& OutputSpecies,
        const int&     SpeciesNum,
        const int&     RunLength,
        std::map<std::string, std::vector<std::string> >& dataMapCalculatedBiomass,
        std::map<std::string, std::vector<std::string> >& dataMapCatch,
        QString&       ScaleStr,
        double&        ScaleVal)
{
    QScatterSeries* series = new QScatterSeries();

    //
    // Draw the line chart
    //
    int m;
    QColor ChartColor;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    boost::numeric::ublas::matrix<double> ChartData;
    ChartData.resize(0,0);
    ChartData.clear();
    RowLabelsForBars.clear();
    ColumnLabelsForLegend.clear();
    int NumRecordsUsed = 0;
    double valueBiomass;
    double valueCatch;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStandardItemModel* smodel = new QStandardItemModel( RunLength+1, 1 );

    m = 0;
    for (int i=0; i<NumSpecies; ++i) {
        if (i == SpeciesNum) {
            for (int j=0; j<=RunLength; ++j) {
                valueBiomass = std::stod(dataMapCalculatedBiomass["Value"][m])/ScaleVal;
                valueCatch   = std::stod(dataMapCatch["Value"][m++])/ScaleVal;
                //ChartData(j,0) = std::stod(value);
                series->append(valueBiomass,valueCatch);
                item = new QStandardItem(QString::number(valueBiomass,'f',3));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(j, 0, item);
                ++NumRecordsUsed;
                //if (i == 0)
                RowLabelsForBars << QString::number(j);
            }
            break;
        } else {
            m += (RunLength+1);
        }
    }

    std::string ChartType = "";
    std::string MainTitle = "Catch vs Calculated Biomass for: " + OutputSpecies.toStdString();
    std::string XLabel    = "Biomass (" + ScaleStr.toStdString() + "mt)";
    std::string YLabel    = "Catch (" + ScaleStr.toStdString() + "mt)";
    std::vector<bool> GridLines = {true,true}; // Replace with checkbox values
    int Theme             = 0;              // Replace with combobox value

    m_ChartWidget->removeAllSeries();

    // Call the chart api function that will populate and annotate the
    // desired chart type.
    if (m_ChartWidget != nullptr) {
        ChartColor = QColor(0,0,175); // darkblue //Qt::blue;
        nmfChartScatter* scatterChart = new nmfChartScatter();

        // RSK - Hack when X is a float
        m_ChartWidget->addSeries(series);
        scatterChart->populateChart(m_ChartWidget,
                                ChartType,
                                nmfConstantsMSSPM::HideFirstPoint,
                                ChartData,
                                ChartColor,
                                RowLabelsForBars,
                                ColumnLabelsForLegend,
                                MainTitle,
                                XLabel,
                                YLabel,
                                GridLines,
                                Theme);
    }

    // Load ChartData into Output table view
    SpeciesNames << OutputSpecies;
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    m_UI->OutputDataTV->setModel(smodel);
    m_UI->OutputDataTV->show();
}
*/
void
nmfMainWindow::initializeMMode()
{
    initializeMModeMain();
    initializeMModeViewer();
}

void
nmfMainWindow::initializeMModeViewer()
{
    int NumSpecies;
    int RunLength;
    int StartYear;
    QStringList SpeciesList;

    QString imagePath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDirMMode));
std::cout << "REMORA dir: " << imagePath.toStdString() << std::endl;
    m_MModeViewerWidget   = new nmfViewerWidget(this,imagePath,m_Logger);
    MModeViewerDockWidget = new QDockWidget(this);
    MModeViewerDockWidget->setWidget(m_MModeViewerWidget->getMainWidget());
    MModeViewerDockWidget->setObjectName("VIEWMORA");
    MModeViewerDockWidget->setAccessibleName("Viewmora");
    addDockWidget(Qt::RightDockWidgetArea, MModeViewerDockWidget);

    // Setup viewer's initial position and size
    MModeViewerDockWidget->setFloating(true);
    int x = this->x() + (9.0/16.0)*this->width();
    // int y = this->y() - (1.0/16.0)*this->height(); // this could cause viewer to appear offscreen on Windows
    int y = this->y();
    MModeViewerDockWidget->setGeometry(x,y,this->width()/2,this->height());
    //  MModeViewerDockWidget->setStyleSheet("QDockWidget#Viewmora {border: 10px solid red}");
    MModeViewerDockWidget->hide();
    MModeViewerDockWidget->setWindowTitle("Viewmora");
    MModeViewerDockWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(MModeViewerDockWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this,                  SLOT(callback_openCSVFile(QPoint)));

    // Set a custom title for the Remora Viewer dock widget
    QWidget* customTitleBar = new QWidget();
    QHBoxLayout* hlayt  = new QHBoxLayout;
    QPushButton* quitPB = new QPushButton("X");
    quitPB->setFlat(true);
    quitPB->setFixedWidth(25);
    quitPB->setToolTip("Close REMORA Viewer");
    hlayt->addSpacerItem(new QSpacerItem(25,1,QSizePolicy::Fixed,QSizePolicy::Fixed));
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    QLabel* title = new QLabel("<strong>VIEWMORA</strong> (REMORA Image and Data Viewer)");
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize()+4);
    title->setFont(titleFont);
    hlayt->addWidget(title);
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hlayt->addWidget(quitPB);
    customTitleBar->setLayout(hlayt);
    MModeViewerDockWidget->setTitleBarWidget(customTitleBar);

    connect(quitPB, SIGNAL(clicked(bool)),
            this,   SLOT(callback_ManagerModeViewerClose(bool)));

    m_BiomassAbsTV         = new QTableView();
    m_BiomassRelTV         = new QTableView();
    m_FishingMortalityTV   = new QTableView();
    m_HarvestScaleFactorTV = new QTableView();

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;
    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear)) {
        return;
    }

    QStandardItemModel* smodel1 = new QStandardItemModel(RunLength,NumSpecies+1 ); // +1 for Year (1st column)
    QStandardItemModel* smodel2 = new QStandardItemModel(RunLength,NumSpecies+1 );
    QStandardItemModel* smodel3 = new QStandardItemModel(RunLength,NumSpecies+1 );
    QStandardItemModel* smodel4 = new QStandardItemModel(RunLength,NumSpecies+1 );
    m_BiomassAbsTV->setModel(smodel1);
    m_BiomassRelTV->setModel(smodel2);
    m_FishingMortalityTV->setModel(smodel3);
    m_HarvestScaleFactorTV->setModel(smodel4);

    m_MModeViewerWidget->addDataTab("Biomass (absolute)",  m_BiomassAbsTV);
    m_MModeViewerWidget->addDataTab("Biomass (relative)",  m_BiomassRelTV);
    m_MModeViewerWidget->addDataTab(nmfConstantsMSSPM::OutputChartExploitation,   m_FishingMortalityTV);
    m_MModeViewerWidget->addDataTab("Harvest Scale Factor",m_HarvestScaleFactorTV);
}

void
nmfMainWindow::callback_openCSVFile(QPoint pos)
{
    QMenu contextMenu("", MModeViewerDockWidget);
    QAction actionOpenCSVFile("Open CSV File...", this);
    actionOpenCSVFile.setToolTip("Open CSV File associated with current image");
    actionOpenCSVFile.setStatusTip("Open CSV File associated with current image");

    connect(&actionOpenCSVFile, SIGNAL(triggered()),
            this,               SLOT(menu_openCSVFile()));

    contextMenu.addAction(&actionOpenCSVFile);
    contextMenu.exec(MModeViewerDockWidget->mapToGlobal(pos));
}

void
nmfMainWindow::callback_ManagerModeViewerClose(bool state)
{
    m_MModeViewerWidget->stopPlayback();
    MModeDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    menu_toggleManagerModeViewer();
}

void
nmfMainWindow::initializeMModeMain()
{
    int NumSpecies;
    QStringList SpeciesList;

    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList);

    Remora_ptr = new REMORA_UI(this,m_DatabasePtr,m_Logger,m_ProjectDir,
                               m_ProjectName,m_ModelName,SpeciesList);

    // Set up main REMORA dock widget
    MModeDockWidget = new QDockWidget(this);
    MModeDockWidget->setObjectName("REMORA");
    MModeDockWidget->setWidget(Remora_ptr->getTopLevelWidget());
    //MModeDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, MModeDockWidget);

    // Set dock window title
    QFont mmodeFont = MModeDockWidget->font();
    mmodeFont.setPointSize(12);
    mmodeFont.setBold(true);

    MModeDockWidget->hide();
    MModeDockWidget->setWindowTitle("REMORA");

    // Set a custom title for the Remora dock widget
    QWidget* customTitleBar = new QWidget();
    QHBoxLayout* hlayt = new QHBoxLayout;
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    QLabel* title = new QLabel("<strong>REMORA</strong> (<strong>RE</strong>source <strong>M</strong>anagement <strong>O</strong>ption <strong>R</strong>eview and <strong>A</strong>nalysis)");
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize()+4);
    title->setFont(titleFont);
    hlayt->addWidget(title);
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    customTitleBar->setLayout(hlayt);
    MModeDockWidget->setTitleBarWidget(customTitleBar);


}

QMenu*
nmfMainWindow::createPopupMenu()
{
    QStringList actionNames = {"Navigator","Progress","Log","Output"};
    QMenu*      popupMenu   = new QMenu();

    for (QString actionName : actionNames) {
        QAction* action = new QAction(actionName);
        action->setCheckable(true);
        setDefaultDockWidgetsVisibility(actionName,action);
        popupMenu->addAction(action);
        connect(action, SIGNAL(triggered(bool)),
                this,   SLOT(context_Action(bool)));
    }

    return popupMenu;
}

void
nmfMainWindow::setDefaultDockWidgetsVisibility(
        const QString& actionName,
        QAction*       action)
{
    if (actionName == "Navigator") {
        action->setChecked(m_UI->NavigatorDockWidget->isVisible());
    } else if (actionName == "Progress") {
        action->setChecked(m_UI->ProgressDockWidget->isVisible());
    } else if (actionName == "Log") {
        action->setChecked(m_UI->LogDockWidget->isVisible());
    } else if (actionName == "Output") {
        action->setChecked(m_UI->OutputDockWidget->isVisible());
    }
    this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);
}

void
nmfMainWindow::context_Action(bool triggered)
{
    QString  actionName = qobject_cast<QAction* >(QObject::sender())->text();
    QAction* action     = qobject_cast<QAction* >(QObject::sender());
    if (actionName == "Navigator") {
        m_UI->NavigatorDockWidget->setVisible(triggered);
    } else if (actionName == "Progress") {
        m_UI->ProgressDockWidget->setVisible(triggered);
    } else if (actionName == "Log") {
        m_UI->LogDockWidget->setVisible(triggered);
    } else if (actionName == "Output") {
        m_UI->OutputDockWidget->setVisible(triggered);
    }
    action->setChecked(triggered);
    this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);
}

void
nmfMainWindow::menu_showTableNames()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_TableNamesDlg->loadTableNames(m_ProjectDatabase);
    QApplication::restoreOverrideCursor();
    m_TableNamesDlg->show();
}

void
nmfMainWindow::callback_NavigatorSelectionChanged()
{
    QString parentStr;
    QString itemSelected;
    int pageNum;

    // Handle the case if the user hasn't selected anything yet.
    QList<QTreeWidgetItem *> selectedItems = NavigatorTree->selectedItems();

    if (selectedItems.count() > 0) {
        itemSelected = selectedItems[0]->text(0);
        pageNum = itemSelected.at(0).digitValue();
        if (pageNum < 1)
            pageNum = 1;

        parentStr.clear();
        if (selectedItems[0]->parent()) {
            parentStr = selectedItems[0]->parent()->text(0);
        }

        m_UI->SetupInputTabWidget->hide();
        m_UI->EstimationDataInputTabWidget->hide();
        m_UI->DiagnosticsDataInputTabWidget->hide();
        m_UI->ForecastDataInputTabWidget->hide();

        if ((itemSelected == nmfConstantsMSSPM::SectionSetup) ||
            (parentStr    == nmfConstantsMSSPM::SectionSetup)) {
            m_UI->SetupInputTabWidget->show();
            if (pageNum > 0) {
                m_UI->SetupInputTabWidget->blockSignals(true);
                m_UI->SetupInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->SetupInputTabWidget->blockSignals(false);
            }
        } else if ((itemSelected == "Simulation Data Input")              ||
                   (parentStr    == "Simulation Data Input")              ||
                   (itemSelected == nmfConstantsMSSPM::SectionEstimation) ||
                   (parentStr    == nmfConstantsMSSPM::SectionEstimation)) {
                m_UI->EstimationDataInputTabWidget->show();
                // Select appropriate tab
                if (pageNum > 0) {
                    m_UI->EstimationDataInputTabWidget->blockSignals(true);
                    m_UI->EstimationDataInputTabWidget->setCurrentIndex(pageNum-1);
                    m_UI->EstimationDataInputTabWidget->blockSignals(false);
                }
        } else if ((itemSelected == nmfConstantsMSSPM::SectionDiagnostic) ||
                   (parentStr    == nmfConstantsMSSPM::SectionDiagnostic)) {
            m_UI->DiagnosticsDataInputTabWidget->show();
            if (pageNum > 0) {
                m_UI->DiagnosticsDataInputTabWidget->blockSignals(true);
                m_UI->DiagnosticsDataInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->DiagnosticsDataInputTabWidget->blockSignals(false);
                if (pageNum == 1) {
                    setDiagnosticLastRunType();
                }
            }
        } else if ((itemSelected == nmfConstantsMSSPM::SectionForecast) ||
                   (parentStr    == nmfConstantsMSSPM::SectionForecast)) {
            m_UI->ForecastDataInputTabWidget->show();
            if (pageNum > 0) {
                m_UI->ForecastDataInputTabWidget->blockSignals(true);
                m_UI->ForecastDataInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->ForecastDataInputTabWidget->blockSignals(false);
            }
            if (pageNum == 1) {
                setForecastLastRunType();
            }
        }
    }
}

void
nmfMainWindow::setDiagnosticLastRunType()
{
    bool thereWasASingleRun;
    bool thereWasAMultiRun;

    checkForPreviousEstimations(thereWasASingleRun,thereWasAMultiRun);

    if (thereWasASingleRun) {
        if (thereWasAMultiRun) {
            // Update the enabled-ness of the radio buttons but
            // leave checked whichever was checked.
            Diagnostic_Tab1_ptr->setSingleRunRBEnabled(true);
            Diagnostic_Tab1_ptr->setMultiRunRBEnabled(true);
        } else {
            Diagnostic_Tab1_ptr->setMultiRunRBState(false,false);
            Diagnostic_Tab1_ptr->setSingleRunRBState(true,true);
        }
    } else {
        Diagnostic_Tab1_ptr->setSingleRunRBState(false,false);
        Diagnostic_Tab1_ptr->setMultiRunRBState(thereWasAMultiRun,thereWasAMultiRun);
    }
}

void
nmfMainWindow::setForecastLastRunType()
{
    bool thereWasASingleRun;
    bool thereWasAMultiRun;

    checkForPreviousEstimations(thereWasASingleRun,thereWasAMultiRun);

    if (thereWasASingleRun) {
        if (thereWasAMultiRun) {
            // Update the enabled-ness of the radio buttons but
            // leave checked whichever was checked.
            Forecast_Tab1_ptr->setSingleRunRBEnabled(true);
            Forecast_Tab1_ptr->setMultiRunRBEnabled(true);
        } else {
            Forecast_Tab1_ptr->setMultiRunRBState(false,false);
            Forecast_Tab1_ptr->setSingleRunRBState(true,true);
        }
    } else {
        Forecast_Tab1_ptr->setSingleRunRBState(false,false);
        Forecast_Tab1_ptr->setMultiRunRBState(thereWasAMultiRun,thereWasAMultiRun);
    }
}

void
nmfMainWindow::checkForPreviousEstimations(bool& thereWasASingleRun,
                                           bool& thereWasAMultiRun)
{
    std::string cmd;
    std::string errorMsg;
    std::string queryStr;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;

    // Check to see if a single run has been done
    fields   = {"Algorithm"};
    queryStr = "SELECT DISTINCT Algorithm FROM " +
            nmfConstantsMSSPM::TableOutputBiomass +
          " WHERE (Algorithm != 'Unweighted' AND Algorithm != 'AIC Weighted')" +
          " AND ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"  + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    thereWasASingleRun = (dataMap["Algorithm"].size() > 0);

    // Check to see if a multi run has been done
    queryStr = "SELECT DISTINCT Algorithm FROM " +
            nmfConstantsMSSPM::TableOutputBiomass +
          " WHERE (Algorithm = 'Unweighted' OR Algorithm = 'AIC Weighted')" +
          " AND ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"  + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    thereWasAMultiRun = (dataMap["Algorithm"].size() > 0);
}


void
nmfMainWindow::callback_LoadProject()
{
  readSettings();
  loadDatabase();

std::cout << "New Project Dir : " << m_ProjectDir << std::endl;
std::cout << "New Project Name: " << m_ProjectName << std::endl;

  disconnect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
             this,           SLOT(callback_LoadProject()));

  loadGuis();

  connect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
          this,           SLOT(callback_LoadProject()));

  enableApplicationFeatures("AllOtherGroups",setupIsComplete());
//enableApplicationFeatures("AllOtherGroups",false);

//callback_ModelLoaded();
  Setup_Tab4_ptr->callback_ClearModelName();
  Estimation_Tab7_ptr->clearEnsembleFile();

  // Disable Navigator items until user loads/creates a model
  callback_ToDoAfterModelDelete();
  clearChart();
  QMessageBox::information(this, "Load Project", "\nPlease load/create a desired model.\n", QMessageBox::Ok);
}

bool
nmfMainWindow::setupIsComplete()
{
    // Setup is complete iff at least one guild, one species,
    // and one system have been created.
    int NumSpecies;
    int NumGuilds;
    QString LoadedSystem;
    QStringList SpeciesList;
    QStringList GuildList;

    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList);
    m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList);
    LoadedSystem = Setup_Tab4_ptr->getModelName();

    return ((NumSpecies > 0) &&
            (NumGuilds  > 0) &&
            (! LoadedSystem.isEmpty()));
}

void
nmfMainWindow::callback_SaveMainSettings()
{
    saveSettings();
}

void
nmfMainWindow::callback_LoadDatabase(QString databaseName)
{
    m_Logger->logMsg(nmfConstants::Normal, "Loading: " + databaseName.toStdString());
    m_DatabasePtr->nmfSetDatabase(databaseName.toStdString());
}

void
nmfMainWindow::initGUIs()
{
    QUiLoader loader;

    m_UI->EstimationDataInputTabWidget->hide();
    m_UI->DiagnosticsDataInputTabWidget->hide();
    m_UI->ForecastDataInputTabWidget->hide();

    // Initialize Navigator Tree
    QFile file(":/forms/Main/Main_NavigatorTreeWidget.ui");
    file.open(QFile::ReadOnly);
    NavigatorTreeWidget = loader.load(&file,this);
    file.close();
    m_UI->NavigatorDockWidget->setWidget(NavigatorTreeWidget);
    NavigatorTree = m_UI->NavigatorDockWidget->findChild<QTreeWidget *>("NavigatorTree");
    QModelIndex index = NavigatorTree->model()->index(0,0);
    NavigatorTree->setCurrentIndex(index);
    initializeNavigatorTree();

    // Load up Setup pages
    Setup_Tab1_ptr      = new nmfSetup_Tab1(m_UI->SetupInputTabWidget);
    Setup_Tab2_ptr      = new nmfSetup_Tab2(m_UI->SetupInputTabWidget,m_Logger,m_DatabasePtr);
    Setup_Tab3_ptr      = new nmfSetup_Tab3(m_UI->SetupInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Setup_Tab4_ptr      = new nmfSetup_Tab4(m_UI->SetupInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);

    Estimation_Tab1_ptr = new nmfEstimation_Tab1(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab2_ptr = new nmfEstimation_Tab2(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab3_ptr = new nmfEstimation_Tab3(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab4_ptr = new nmfEstimation_Tab4(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab5_ptr = new nmfEstimation_Tab5(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab6_ptr = new nmfEstimation_Tab6(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab7_ptr = new nmfEstimation_Tab7(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Estimation_Tab8_ptr = new nmfEstimation_Tab8(m_UI->EstimationDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);

    Diagnostic_Tab1_ptr = new nmfDiagnostic_Tab1(m_UI->DiagnosticsDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Diagnostic_Tab2_ptr = new nmfDiagnostic_Tab2(m_UI->DiagnosticsDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);

    Forecast_Tab1_ptr   = new nmfForecast_Tab1(m_UI->ForecastDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Forecast_Tab2_ptr   = new nmfForecast_Tab2(m_UI->ForecastDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Forecast_Tab3_ptr   = new nmfForecast_Tab3(m_UI->ForecastDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);
    Forecast_Tab4_ptr   = new nmfForecast_Tab4(m_UI->ForecastDataInputTabWidget,m_Logger,m_DatabasePtr,m_ProjectDir);

    Output_Controls_ptr     = new nmfOutputControls(m_UI->MSSPMOutputControlsGB,m_Logger,m_DatabasePtr,m_ProjectDir);
    Output_TableWidgets_ptr = new nmfOutputTableWidgets(m_UI->OutputDockWidget, m_Logger,m_DatabasePtr,m_ProjectDir);

    // Select first item in Navigator Tree
    callback_SetupTabChanged(0);
}

void
nmfMainWindow::showDockWidgets(bool show)
{
    m_UI->NavigatorDockWidget->setVisible(show);
    m_UI->ProgressDockWidget->setVisible(show);
    m_UI->LogDockWidget->setVisible(show);
    m_UI->OutputDockWidget->setVisible(show);
    this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);
} // end showDockWidgets

void
nmfMainWindow::callback_PreferencesSetStyleSheet(QString style)
{
    if (style == "Dark") {
        QFile fileStyle(":qdarkstyle/style.qss");
        if (! fileStyle.exists()) {
            std::cout << "Error: Unable to set stylesheet, file not found: qdarkstyle/style.qss\n" << std::endl;;
        } else {
            fileStyle.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&fileStyle);
            qApp->setStyleSheet(ts.readAll());
        }
        Estimation_Tab5_ptr->setIsDark(style);
        Setup_Tab4_ptr->setIsDark(style);
        Diagnostic_Tab2_ptr->setWidgetsDark();
    } else {
            Setup_Tab4_ptr->setIsDark(style);
            Estimation_Tab5_ptr->setIsDark(style);
            Diagnostic_Tab2_ptr->setWidgetsLight();
            qApp->setStyleSheet("");
    }
    if (Setup_Tab4_ptr != nullptr) {
        Setup_Tab4_ptr->uncheckHighlightButtons();
        Setup_Tab4_ptr->setHighlightColors();
        Setup_Tab4_ptr->setLineEditColors();
    }
}

void
nmfMainWindow::callback_PreferencesSigDigCB(int state)
{
    QLabel*   sigdigLBL = m_PreferencesWidget->findChild<QLabel  *>("PrefSignificantDigitsLBL");
    QSpinBox* sigdigSB  = m_PreferencesWidget->findChild<QSpinBox*>("PrefSignificantDigitsSB");
    bool enable = (state != Qt::Unchecked);

    sigdigLBL->setEnabled(enable);
    sigdigSB->setEnabled(enable);

    m_NumSignificantDigits = (enable) ? sigdigSB->value() : -1;
}

int
nmfMainWindow::getNumSignificantDigits()
{
    return m_NumSignificantDigits;
}

void
nmfMainWindow::callback_PreferencesOkPB()
{
    QSpinBox* numRowsSB    = m_PreferencesWidget->findChild<QSpinBox *>("PrefNumRowsSB");
    QSpinBox* numColumnsSB = m_PreferencesWidget->findChild<QSpinBox *>("PrefNumColumnsSB");
    QSpinBox* sigdigSB     = m_PreferencesWidget->findChild<QSpinBox *>("PrefSignificantDigitsSB");
    QSpinBox* timeoutSB    = m_PreferencesWidget->findChild<QSpinBox *>("PrefTimeoutSB");

    m_MShotNumRows = numRowsSB->value();
    m_MShotNumCols = numColumnsSB->value();
    m_DBTimeout    = timeoutSB->value();

    m_NumSignificantDigits = (sigdigSB->isEnabled()) ? sigdigSB->value() : -1;

    saveSettings();
    loadAllWidgets();
    m_PreferencesDlg->close();
}

void
nmfMainWindow::setDefaultDockWidgetsVisibility()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("MainWindow");

    bool progressDockVis = settings->value("ProgressDockWidgetIsVisible",false).toBool();
    bool logDockVis      = settings->value("LogDockWidgetIsVisible",false).toBool();

    this->addDockWidget(Qt::BottomDockWidgetArea, m_UI->ProgressDockWidget);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_UI->LogDockWidget);

    m_UI->OutputDockWidget->setVisible(settings->value("OutputDockWidgetIsVisible",false).toBool());
    m_UI->ProgressDockWidget->setVisible(progressDockVis);
    m_UI->LogDockWidget->setVisible(logDockVis);
    if (progressDockVis && logDockVis) {
        this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);
    }
    settings->endGroup();

    delete settings;

    // Turn these off
    m_UI->SetupOutputTE->setVisible(false);
}

void
nmfMainWindow::readSettings(QString Name)
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("MainWindow");
    bool isVisibleOutputWidget = settings->value("OutputDockWidgetIsVisible",false).toBool();

    if (Name == "style") {
        callback_PreferencesSetStyleSheet(settings->value(Name,"").toString());
    } else if (isVisibleOutputWidget) {
        if (Name == "OutputDockWidgetIsFloating") {
            m_UI->OutputDockWidget->setFloating(settings->value("OutputDockWidgetIsFloating",false).toBool());
        } else if (Name == "OutputDockWidgetSize") {
            m_UI->OutputDockWidget->resize(settings->value("OutputDockWidgetSize", QSize(400, 400)).toSize());
        } else if (Name == "OutputDockWidgetPos") {
            m_UI->OutputDockWidget->move(settings->value("OutputDockWidgetPos",    QPoint(200, 200)).toPoint());
        }
    } else if (Name == "ProgressDockWidgetIsVisible") {
        m_UI->ProgressDockWidget->setVisible(settings->value("ProgressDockWidgetIsVisible",false).toBool());
    } else if (Name == "LogDockWidgetIsVisible") {
        //ui->LogDockWidget->setVisible(settings->value("LogDockWidgetIsVisible",false).toBool());
    }
    settings->endGroup();

    if (Name == "Preferences") {
        settings->beginGroup("Preferences");
        m_MShotNumRows = settings->value("MShotNumRows",3).toInt();
        m_MShotNumCols = settings->value("MShotNumCols",4).toInt();
        m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
        m_DBTimeout = settings->value("DBTimeout",nmfConstantsMSSPM::DefaultDBTimeoutDays).toInt();
        settings->endGroup();
    } else if (Name == "Runtime") {
        settings->beginGroup("Runtime");
        m_IsMultiRun    = settings->value("IsMultiRun",false).toBool();
        m_MultiRunType  = settings->value("MultiRunType","").toString().toStdString();
        settings->endGroup();
    }

    delete settings;
}

void
nmfMainWindow::readSettingsGuiOrientation(bool alsoResetPosition)
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("MainWindow");
    resize(settings->value("size", QSize(400, 400)).toSize());
    if (alsoResetPosition) {
        move(settings->value("pos", QPoint(200, 200)).toPoint());
    }

    // Resize the dock widgets
    int NavDockWidth      = settings->value("NavigatorDockWidgetWidth",200).toInt();
    int OutputDockWidth   = settings->value("OutputDockWidgetWidth",200).toInt();
    int ProgressDockWidth = settings->value("ProgressDockWidgetWidth",200).toInt();
    this->resizeDocks({m_UI->NavigatorDockWidget,m_UI->OutputDockWidget,m_UI->ProgressDockWidget},
                      {NavDockWidth,OutputDockWidth,ProgressDockWidth},
                      Qt::Horizontal);
    settings->endGroup();

    delete settings;
}

void
nmfMainWindow::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");
    this->restoreState(settings->value("DOCK_LOCATIONS").toByteArray());

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectDatabase = settings->value("ProjectDatabase","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();

    settings->beginGroup("Diagnostics");
    m_DiagnosticsFontSize  = settings->value("FontSize", 9).toInt();
    m_DiagnosticsVariation = settings->value("Variation",1).toInt();
    m_DiagnosticsNumPoints = settings->value("NumPoints",1).toInt();
    settings->endGroup();

    settings->beginGroup("Forecast");
    m_ForecastFontSize = settings->value("FontSize",9).toInt();
    settings->endGroup();

    settings->beginGroup("Runtime");
    m_IsMultiRun    = settings->value("IsMultiRun",false).toBool();
//    m_UsingHPCFiles = settings->value("IsHPCRun",  false).toBool();
    m_MultiRunType  = settings->value("MultiRunType","").toString().toStdString();

    settings->endGroup();

    settings->beginGroup("Preferences");
    m_MShotNumRows = settings->value("MShotNumRows",3).toInt();
    m_MShotNumCols = settings->value("MShotNumCols",4).toInt();
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    m_ShowSignificantDigitsDlg = settings->value("ShowSignificantDigitsDialog",true).toBool();
    m_DBTimeout = settings->value("DBTimeout",nmfConstantsMSSPM::DefaultDBTimeoutDays).toInt();

    settings->endGroup();

    m_UI->actionToggleSignificantDigits->setChecked(m_NumSignificantDigits >= 0);


    delete settings;

    updateWindowTitle();
}

QString
nmfMainWindow::getCurrentStyle()
{
    if (qApp->styleSheet().isEmpty())
        return "Light";
    else
        return "Dark";
}

void
nmfMainWindow::saveSettings() {
    if (! m_SaveSettings) {
        return;
    }

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");
    settings->setValue("DOCK_LOCATIONS",this->saveState());

    settings->beginGroup("MainWindow");
    settings->setValue("pos", pos());
    settings->setValue("size", size());
    settings->setValue("style",getCurrentStyle());
    settings->setValue("NavigatorDockWidgetWidth",   m_UI->NavigatorDockWidget->width());
    settings->setValue("OutputDockWidgetWidth",      m_UI->OutputDockWidget->width());
    settings->setValue("LogDockWidgetIsVisible",     m_UI->LogDockWidget->isVisible());
    settings->setValue("OutputDockWidgetIsVisible",  m_UI->OutputDockWidget->isVisible());
    settings->setValue("ProgressDockWidgetIsVisible",m_UI->ProgressDockWidget->isVisible());
    settings->setValue("ProgressDockWidgetWidth",    m_UI->ProgressDockWidget->width());
    settings->setValue("OutputDockWidgetIsFloating", m_UI->OutputDockWidget->isFloating());
    settings->setValue("OutputDockWidgetPos",        m_UI->OutputDockWidget->pos());
    settings->setValue("OutputDockWidgetSize",       m_UI->OutputDockWidget->size());
//  settings->setValue("CentralWidgetSize",          centralWidget()->size());
    settings->endGroup();

    settings->beginGroup("SetupTabIsVisible");
    settings->setValue("FontSize", Setup_Tab4_ptr->getFontSize());
    settings->endGroup();

    settings->beginGroup("Runtime");
    settings->setValue("IsMultiRun",   isAMultiRun());
    settings->setValue("IsHPCRun",     m_UsingHPCFiles);
    settings->setValue("MultiRunType", Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm());
    settings->endGroup();

    settings->beginGroup("Preferences");
    settings->setValue("MShotNumRows", m_MShotNumRows);
    settings->setValue("MShotNumCols", m_MShotNumCols);
    settings->setValue("NumSignificantDigits", m_NumSignificantDigits);
    settings->setValue("ShowSignificantDigitsDialog", m_ShowSignificantDigitsDlg);
    settings->setValue("DBTimeout", m_DBTimeout);
    settings->endGroup();

    // Get ModelName
    QString modelName = Setup_Tab4_ptr->getModelName();

    // Save other pages' settings
    Setup_Tab2_ptr->saveSettings(nmfConstantsMSSPM::DontClearModelName);
    Estimation_Tab7_ptr->saveSettings();
    Forecast_Tab1_ptr->saveSettings();
    Forecast_Tab4_ptr->saveSettings();
    Output_Controls_ptr->saveSettings();

    // Restore modelName
    settings->beginGroup("Settings");
    settings->setValue("Name", modelName);
    settings->endGroup();

    settings->beginGroup("Output");
    int width = m_UI->MSSPMOutputControlsGB->size().width();
    settings->setValue("Width", width);
    settings->endGroup();

    delete settings;
}

void
nmfMainWindow::setOutputControlsWidth()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Output");
    int width = settings->value("Width",0).toString().toInt();
    //std::cout << "===> SETTING to: " << width << std::endl;
    // +30 term is needed cause the group box wouldn't return to the saved size without it
    m_UI->MSSPMOutputControlsGB->resize(width+30,m_UI->MSSPMOutputControlsGB->height());
//    m_UI->MSSPMOutputTabWidget->resize(width-30,m_UI->MSSPMOutputTabWidget->height());
    settings->endGroup();

    delete settings;
}

void
nmfMainWindow::closeEvent(QCloseEvent *event)
{
    // Don't save in REMORA mode
    if (runningREMORA()) {
        menu_toggleManagerMode();
    }

    m_ProgressWidget->stopAllRuns(false);
    disconnect(m_ProgressWidget,0,0,0);
    if (m_ProgressChartTimer != nullptr) {
        delete m_ProgressChartTimer;
    }
    saveSettings();
}


bool
nmfMainWindow::isEstimationRunning()
{
    std::string cmd="";
    std::ifstream inputFile(nmfConstantsMSSPM::MSSPMStopRunFile);

    if (inputFile) {
        std::getline(inputFile,cmd);
    }
    inputFile.close();

    return (cmd == "Start");
}

bool
nmfMainWindow::isSufficientBiomass(int& numInsufficientSpecies)
{
    bool retv = true;
    int numBlanks;
    int numRows = m_DataStruct.ObservedBiomassBySpecies.size1();
    int numCols = m_DataStruct.ObservedBiomassBySpecies.size2();

    numInsufficientSpecies = 0;
    for (int species=0; species<numCols; ++species) {
        numBlanks = 0;
        for (int time=0; time<numRows; ++time) {
            if (m_DataStruct.ObservedBiomassBySpecies(time,species) == nmfConstantsMSSPM::NoData) {
                ++numBlanks;
            }
        }
        if (numBlanks > numRows/2) {
            ++numInsufficientSpecies;
            retv = false; // at least one species was found with > 50% blanks
        }
    }

    return retv;
}

bool
nmfMainWindow::passEstimationChecks(
        std::vector<QString>& MultiRunLines,
        int& TotalIndividualRuns)
{
    QString msg;

    // 1. Check to see if there's an Estimation already running
    if (isEstimationRunning()) {
        QMessageBox::information(this,
                                 tr("MSSPM Run In Progress"),
                                 tr("\nStop current Run before beginning another.\n"),
                                 QMessageBox::Ok);
        return false;
    }

    // 2. Check to see if the user is trying to run a Mohn's Rho with a Bees algorithm
    // (This functionality isn't yet allowed.)
    if (isAMohnsRhoMultiRun() && isBeesAlgorithm()) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, "Warning",
                             "\nMohn's Rho not yet implemented for Bees Algorithm.\n",
                             QMessageBox::Ok);
        Diagnostic_Tab2_ptr->setMohnsRhoForSingleRun(false);
        Diagnostic_Tab2_ptr->setMohnsRhoForMultiRun(false);
        return false;
    }

    // 3. Check if more than 50 pct of observed biomass of any species is missing and warn user:
    // Warning: Insufficient data to constrain the model for species: ....
    // Cancel run?
    int numInsufficientSpecies;
    if (! isSufficientBiomass(numInsufficientSpecies)) {
        QApplication::restoreOverrideCursor();
        QString phrase = (numInsufficientSpecies == 1) ? "that" : "those";
        msg  = QString("\nFound ") + QString::number(numInsufficientSpecies) + " species with more than 50% missing biomass. ";
        msg += "There is insufficient data to constrain the model for " + phrase + " species.\n\n";
        msg += "Cancel the current run?";
        QMessageBox::StandardButton reply =
                QMessageBox::warning(this, tr("Warning"), tr(msg.toLatin1()),
                                     QMessageBox::No|QMessageBox::Yes,
                                     QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            return false;
        }
    }

    // 4. Check to make sure it's a properly defined multi run
    bool isAMultiRun = isAMultiOrMohnsRhoRun();
    if (isAMultiRun) {
        m_DatabasePtr->clearTable(m_Logger,nmfConstantsMSSPM::TableOutputBiomassEnsemble);
        if (! nmfUtilsQt::loadMultiRunData(m_DataStruct,MultiRunLines,TotalIndividualRuns)) {
            QApplication::restoreOverrideCursor();
            std::cout << "Error: Couldn't open: " << m_DataStruct.MultiRunSetupFilename << std::endl;
            msg = "\nError: Couldn't run multi-run.\n\nProblem opening: " +
                    QString::fromStdString(m_DataStruct.MultiRunSetupFilename) +
                    "\n\nPlease make sure that the multi-run has been properly configured and set.\n";
            QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);
            return false;
        }

    }

    return true;
}

void
nmfMainWindow::callback_RunEstimation(bool showDiagnosticsChart)
{
    m_UsingHPCFiles = false;
    m_HPC_Algorithm = "";
    m_HPC_Minimizer = "";
    m_HPC_ObjectiveCriterion = "";
    m_HPC_Scaling = "";

    bool isAMultiRun = isAMultiOrMohnsRhoRun();
    int TotalIndividualRuns = 0;
    std::vector<QString> MultiRunLines = {};
    QString multiRunSpeciesFilename;
    QString multiRunModelFilename;
    QString msg;

    saveSettings();
    loadAllWidgets();
    readSettings();

    m_UI->OutputDockWidget->raise();

    // Load all run parameters
    if (isAMultiRun) {
        Estimation_Tab7_ptr->enableEnsembleControls(false);
    }
    bool loadOK = loadParameters(m_DataStruct,nmfConstantsMSSPM::VerboseOn);
    if (isAMultiRun) {
        Estimation_Tab7_ptr->enableEnsembleControls(true);
    }
    if (! loadOK) {
        std::cout << "Run cancelled. LoadParameters returned: " << loadOK << std::endl;
        return;
    }

    // Run Estimation checks
    if (! passEstimationChecks(MultiRunLines,TotalIndividualRuns)) {
        return;
    }

    // Get current algorithm
    std::string Algorithm = Estimation_Tab7_ptr->getCurrentAlgorithm();
    // Disable Monte Carlo Output Widgets
    Output_Controls_ptr->enableBrightnessWidgets(false);
    Output_Controls_ptr->enableControls(false);

    callback_InitializeSubRuns(m_DataStruct.MultiRunModelFilename,TotalIndividualRuns);
    m_ProgressWidget->clearRunBoxes();

    if (isAMultiRun) {
        m_ProgressWidget->setRunBoxes(1,0,m_DataStruct.NLoptNumberOfRuns);
    }

    // Start and initialize the Progress chart and set global variables to defaults
    m_ProgressWidget->stopTimer();
    m_ProgressWidget->startTimer(100);
    m_ProgressWidget->startRun();
    m_RunNumNLopt = 0;
    m_RunNumBees  = 1;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (isAMultiRun) {
        // To-do: Add Bees to regular multi-run
        //      runBeesAlgorithm(showDiagnosticsChart,MultiRunLines,TotalIndividualRuns);
std::cout << "----->>>> TotalIndividualRuns: " << TotalIndividualRuns << std::endl;
        runNLoptAlgorithm(showDiagnosticsChart,MultiRunLines,TotalIndividualRuns); // Run through all runs and do NLopt, skipping over non-NLopt
    } else {
        if (Algorithm == "Bees Algorithm") {
            runBeesAlgorithm( showDiagnosticsChart,MultiRunLines,TotalIndividualRuns);
        } else if (Algorithm == "NLopt Algorithm") {
            runNLoptAlgorithm(showDiagnosticsChart,MultiRunLines,TotalIndividualRuns);
        }
    }

    // Assure the Output Dock widget is visible
    if (! m_UI->OutputDockWidget->isVisible()) {
        m_UI->OutputDockWidget->setVisible(true);
    }

    // Assure Output tab is set to Chart
    setCurrentOutputTab("Chart");
}

bool
nmfMainWindow::isBeesAlgorithm()
{
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    return (Algorithm == "Bees Algorithm");
}

void
nmfMainWindow::callback_ForecastLoaded(std::string ForecastName)
{
    Forecast_Tab2_ptr->loadWidgets();
    Forecast_Tab3_ptr->loadWidgets();
}

void
nmfMainWindow::callback_SaveForecastOutputBiomassData(std::string ForecastName)
{
    bool usingMultiRun=false;
    bool updateOK = true;
    bool isMonteCarlo;
    bool isAggProd;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    int RunLength = 0;
    int NullStartYear = 0;
    int NumRuns = 0;
    int RunNum = 0;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::string isAggProdStr;
    std::string OutputInitBiomassTable                     = nmfConstantsMSSPM::TableOutputInitBiomass;
    std::string OutputGrowthRateTable                      = nmfConstantsMSSPM::TableOutputGrowthRate;
    std::string OutputGrowthRateShapeTable                 = nmfConstantsMSSPM::TableOutputGrowthRateShape;
    std::string OutputGrowthRateCovariateCoeffsTable       = nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs;
    std::string OutputCarryingCapacityTable                = nmfConstantsMSSPM::TableOutputCarryingCapacity;
    std::string OutputCarryingCapacityCovariateCoeffsTable = nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs;
    std::string OutputCatchabilityTable                    = nmfConstantsMSSPM::TableOutputCatchability;
    std::string OutputCatchabilityCovariateCoeffsTable     = nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs;
    std::string OutputSurveyQTable                         = nmfConstantsMSSPM::TableOutputSurveyQ;
    std::string OutputSurveyQCovariateCoeffsTable          = nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs;
    std::string ForecastBiomassTable                       = nmfConstantsMSSPM::TableForecastBiomass;
    std::string ForecastBiomassMonteCarloTable             = nmfConstantsMSSPM::TableForecastBiomassMonteCarlo;
    std::string ForecastMonteCarloParametersTable          = nmfConstantsMSSPM::TableForecastMonteCarloParameters;
    bool isRunningREMORA = runningREMORA();

    // Find Forecast info
    fields    = {"ProjectName","ModelName","ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm","RunLength","StartYear","EndYear","NumRuns"};
    queryStr  = "SELECT ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm,RunLength,StartYear,EndYear,NumRuns FROM " +
                 nmfConstantsMSSPM::TableForecasts +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName +
                "' AND ForecastName = '" + ForecastName + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        RunLength          = std::stoi(dataMap["RunLength"][0]);
        Algorithm          = dataMap["Algorithm"][0];
        Minimizer          = dataMap["Minimizer"][0];
        ObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
        Scaling            = dataMap["Scaling"][0];
        GrowthForm         = dataMap["GrowthForm"][0];
        HarvestForm        = dataMap["HarvestForm"][0];
        CompetitionForm    = dataMap["WithinGuildCompetitionForm"][0];
        PredationForm      = dataMap["PredationForm"][0];
        NumRuns            = std::stoi(dataMap["NumRuns"][0]);
    }
    isAggProd     = (CompetitionForm == "AGG-PROD");
    isAggProdStr  = (isAggProd) ? "1" : "0";
    usingMultiRun = checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    std::string forecastType = (isRunningREMORA) ? "REMORA: " : "Forecast: ";
    std::string msg = (usingMultiRun) ? "Using Estimated Parameters from the last Multi Run" :
                                        "Using Estimated Parameters from the last Single Run";
    m_Logger->logMsg(nmfConstants::Normal,forecastType+msg);

    // Calculate Monte Carlo simulations
    isMonteCarlo = true;
    clearOutputBiomassTable(ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                            isAggProdStr,ForecastBiomassMonteCarloTable);
    clearMonteCarloParametersTable(ForecastName,Algorithm,Minimizer,
                                   ObjectiveCriterion,Scaling,
                                   ForecastMonteCarloParametersTable);
    m_Logger->logMsg(nmfConstants::Normal,"Calculating Monte Carlo Forecasts (i.e., with Uncertainty)");
    for (RunNum=0; RunNum<NumRuns; ++RunNum) {
        updateOK = updateOutputBiomassTable(ForecastName,NullStartYear,RunLength,isMonteCarlo,RunNum,
                                            Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,
                                            GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                            OutputInitBiomassTable,
                                            OutputGrowthRateTable,
                                            OutputGrowthRateShapeTable,
                                            OutputGrowthRateCovariateCoeffsTable,
                                            OutputCarryingCapacityTable,OutputCarryingCapacityCovariateCoeffsTable,
                                            OutputCatchabilityTable,OutputCatchabilityCovariateCoeffsTable,
                                            OutputSurveyQTable,OutputSurveyQCovariateCoeffsTable,
                                            ForecastBiomassMonteCarloTable);
        if (! updateOK) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] callback_SaveOutputBiomassData: Problem with Monte Carlo simulation");
            QApplication::restoreOverrideCursor();
            return;
        }
    }

    // Calculate Forecast Biomass without any uncertainty variation and
    // ensure it appears superimposed over Monte Carlo simulations
    isMonteCarlo = false;
    clearOutputBiomassTable(ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                            isAggProdStr,ForecastBiomassTable);
    m_Logger->logMsg(nmfConstants::Normal,"Calculating Forecast with no Uncertainty");
    updateOK = updateOutputBiomassTable(ForecastName,NullStartYear,RunLength,isMonteCarlo,NumRuns,
                                        Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,
                                        GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                        OutputInitBiomassTable,
                                        OutputGrowthRateTable,
                                        OutputGrowthRateShapeTable,
                                        OutputGrowthRateCovariateCoeffsTable,
                                        OutputCarryingCapacityTable,OutputCarryingCapacityCovariateCoeffsTable,
                                        OutputCatchabilityTable,OutputCatchabilityCovariateCoeffsTable,
                                        OutputSurveyQTable,OutputSurveyQCovariateCoeffsTable,
                                        ForecastBiomassTable);

    // Need to reset the visibility for REMORA
//  menu_toggleManagerMode();
    m_UI->OutputDockWidget->setVisible(!isRunningREMORA);
    m_UI->NavigatorDockWidget->setVisible(!isRunningREMORA);
    m_UI->ProgressDockWidget->setVisible(!isRunningREMORA);
    m_UI->LogDockWidget->setVisible(!isRunningREMORA);
    m_UI->centralWidget->setVisible(!isRunningREMORA);
    MModeDockWidget->setVisible(isRunningREMORA);
}

void
nmfMainWindow::callback_UpdateSeedValue(int isDeterministic)
{
    m_SeedValue = (isDeterministic) ? 1 : -1;
    Forecast_Tab1_ptr->setDeterministic(isDeterministic);
}

void
nmfMainWindow::callback_UpdateUncertaintyTable()
{
    Forecast_Tab3_ptr->loadWidgets();
}

bool
nmfMainWindow::okToRunForecast()
{
    if (! Forecast_Tab1_ptr->isValidForecastName()) {
        return false;
    }
    if (! Forecast_Tab2_ptr->areDataComplete()) {
        return false;
    }
    if (! Forecast_Tab3_ptr->areDataComplete()) {
        return false;
    }
    return true;
}

void
nmfMainWindow::callback_RunForecast(std::string ForecastName,
                                    bool GenerateBiomass)
{
    bool isAggProd;
    int RunLength = 0;
    int StartYear = nmfConstantsMSSPM::Start_Year;
    int EndYear = StartYear;
    int NumRuns = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::string isAggProdStr;
    std::string ForecastHarvestType;
    double ScaleVal = 1.0;
    QString ScaleStr = "";
    double YMinSliderValue  = Output_Controls_ptr->getYMinSliderVal();
//  bool showHistoricalData = Output_Controls_ptr->isHistoricalDataShown();
    double brightnessFactor = Output_Controls_ptr->getOutputBrightnessFactor();

    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::callback_RunForecast start");

    if (! okToRunForecast()) {
        QMessageBox::warning(this, "Error",
                             "\nError: Incomplete Forecast input data. Please re-check all Forecast input data forms.\n",
                             QMessageBox::Ok);
        Forecast_Tab4_ptr->enableRunButton(true);
        return;
    }

    if (runningREMORA()) {
        m_SeedValue = (Remora_ptr->isDeterministic()) ? 1 : -1;
    } else {
        m_SeedValue = Forecast_Tab1_ptr->getSeed();
    }

    // Turn on wait cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Forecast_Tab4_ptr->clearOutputTE();
    Forecast_Tab4_ptr->setOutputTE(QString("<b>Summary:<\b>"));

    // Find Forecast info
    m_DatabasePtr->getForecastInfo(m_ProjectName,m_ModelName,ForecastName,RunLength,StartYear,
                                   Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                   GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                   ForecastHarvestType,NumRuns);
    EndYear = StartYear + RunLength;

    checkForecastAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    std::string suffix = (m_IsMultiRun) ? " Average" : "";

    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::callback_RunForecast: Found Forecast Info for forecast: "+ForecastName);

    isAggProd    = (CompetitionForm == "AGG-PROD");
    isAggProdStr = (isAggProd) ? "1" : "0";
    std::string temp;

    QString lastRunType = (Forecast_Tab1_ptr->isSetLastRunMultiForecast()) ? "Multi Run" : "Single Run";
    QString summary = "<br><tt>";
    summary += "<strong>Run Length:</strong>       " + QString::number(RunLength) + " year(s)<br>";
    summary += "<strong>Year Range:</strong>       " + QString::number(StartYear) + " to " + QString::number(EndYear) + "<br>";
    summary += "<strong>Num Runs:</strong>         " + QString::number(NumRuns) + "<br><br>";
    summary += "<strong>Algorithm:</strong>        " + QString::fromStdString(Algorithm+suffix) + "<br>";
    summary += "<strong>Minimizer:</strong>        " + QString::fromStdString(Minimizer+suffix) + "<br>";
    summary += "<strong>Obj Criterion:</strong>    " + QString::fromStdString(ObjectiveCriterion+suffix) + "<br>";
    if (Scaling.empty()) {
        summary += "<strong>Scaling Alg:</strong>      " + QString::fromStdString(Scaling)        + "<br><br>";
    } else {
        summary += "<strong>Scaling Alg:</strong>      " + QString::fromStdString(Scaling+suffix) + "<br><br>";
    }
    summary += "<strong>Growth Form:</strong>      " + QString::fromStdString(GrowthForm) + "<br>";
    summary += "<strong>Harvest Form:</strong>     " + QString::fromStdString(HarvestForm) + "<br>";
    summary += "<strong>Competition Form:</strong> " + QString::fromStdString(CompetitionForm) + "<br>";
    summary += "<strong>Predation Form:</strong>   " + QString::fromStdString(PredationForm) + "<br><br>";
    if (HarvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        summary += "<strong>Forecast Harvest Data:</strong> " + Forecast_Tab2_ptr->getHarvestType() + "<br><br>";
    }
    summary += "<strong>Used Estimated Parameters from last:</strong> " + lastRunType + "<br>";
    summary += "</tt>";
    Forecast_Tab4_ptr->appendOutputTE(summary);
    saveSettings();

    // Make sure all necessary data from the last Estimation Run have been loaded
    bool loadOK = loadParameters(m_DataStruct,nmfConstantsMSSPM::VerboseOn);
    if (! loadOK) {
        std::cout << "Run cancelled. LoadParameters returned: " << loadOK << std::endl;
        return;
    }

    //
    // Runs the Forecast here (i.e. generates the output biomass)
    //
    if (GenerateBiomass) {
        callback_SaveForecastOutputBiomassData(ForecastName);
    }
    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::callback_RunForecast: Setting Chart Type to Forecast");

    // Set Chart Type to Forecast
    if (! showForecastChart(isAggProd,ForecastName,StartYear,
                            ScaleStr,ScaleVal,YMinSliderValue,
                            brightnessFactor)) {
        QApplication::restoreOverrideCursor();
        Forecast_Tab4_ptr->enableRunButton(true);
        return;
    }

    // Enable Monte Carlo Output Widgets
    Output_Controls_ptr->enableBrightnessWidgets(true);

    // Turn off wait cursor
    QApplication::restoreOverrideCursor();

    // Assure Chart type is Forecast but don't change to a different species.
    int currOutputSpecies = Output_Controls_ptr->getOutputSpeciesIndex();
    Output_Controls_ptr->setOutputType("Forecast");
    Output_Controls_ptr->setOutputSpeciesIndex(currOutputSpecies);
    Output_Controls_ptr->refreshBrightnessSlider();

    // Assure Output tab is set to Chart
    setCurrentOutputTab("Chart");

    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::callback_RunForecast end");

} // end callback_RunForecast

void
nmfMainWindow::callback_SetForecastHarvestModel()
{
    Forecast_Tab2_ptr->setForecastEstimationHarvestModels(
                Estimation_Tab2_ptr->getHarvestModel(),
                Estimation_Tab2_ptr->getHarvestModelFTCCatch(),
                Estimation_Tab2_ptr->getHarvestModelFTCEffort());
}

void
nmfMainWindow::callback_LoadDataStruct()
{
    nmfStructsQt::ModelDataStruct dataStruct;

    bool loadOK = loadParameters(dataStruct,nmfConstantsMSSPM::VerboseOff);
    if (! loadOK) {
        std::cout << "callback_LoadDataStruct cancelled. callback_LoadDataStruct returned: " << loadOK << std::endl;
        return;
    }
    Diagnostic_Tab1_ptr->setDataStruct(dataStruct);
}

void
nmfMainWindow::runBeesAlgorithm(bool showDiagnosticChart,
                                std::vector<QString>& MultiRunLines,
                                int& TotalIndividualRuns)
{
    m_ProgressWidget->clearChartData(nmfConstantsMSSPM::MSSPMProgressChartFile);

    m_DataStruct.showDiagnosticChart = showDiagnosticChart;

    // Create the Bees Estimator object
    m_Estimator_Bees = new Bees_Estimator();

    // Set up connections
    disconnect(m_Estimator_Bees, 0, 0, 0);
    connect(m_Estimator_Bees,  SIGNAL(RunCompleted(std::string,std::string,bool)),
            this,              SLOT(callback_RunCompleted(std::string,std::string,bool)));
    connect(m_Estimator_Bees,  SIGNAL(RepetitionRunCompleted(int,int,int)),
            this,              SLOT(callback_RepetitionRunCompleted(int,int,int)));
    connect(m_Estimator_Bees,  SIGNAL(ErrorFound(std::string)),
            this,              SLOT(callback_ErrorFound(std::string)));
//    connect(m_Estimator_Bees,  SIGNAL(AllSubRunsCompleted(std::string,std::string,QString)),
//            this,              SLOT(callback_AllSubRunsCompleted(std::string,std::string,QString)));
    connect(m_ProgressWidget,  SIGNAL(StopTimer()),
            this,              SLOT(callback_StopTimer()));
    connect(m_ProgressWidget,  SIGNAL(SetAllRunsComplete()),
            this,              SLOT(callback_SetAllRunsComplete()));

//    // Set up progress widget to show fitness vs generation
//    m_ProgressWidget->startTimer(100);
//    m_ProgressWidget->startRun();

    updateProgressChartAnnotation(0,(double)m_DataStruct.BeesMaxGenerations,5.0);
//    ProgressWidgetMSSPM->setMainTitle("Sum of Squares Error per Generation");
//    ProgressWidgetMSSPM->setYAxisTitleScale("Sum of Squares (SSE)",0.0,1.0,0.1);
//    ProgressWidgetMSSPM->setXAxisTitleScale("Generations",0,dataStruct.BeesMaxGenerations,5.0);

    m_RunNumBees = m_RunNumNLopt;
    QFuture<void> future = QtConcurrent::run(
                m_Estimator_Bees,
                &Bees_Estimator::estimateParameters,
                m_DataStruct,
                m_RunNumBees++,
                MultiRunLines,
                TotalIndividualRuns);

    m_ProgressWidget->hideLegend();
    m_isRunning = true;

//    static const QMetaMethod updateProgressSignal = QMetaMethod::fromSignal(&Bees_Estimator::UpdateProgressData);
//    if (! isSignalConnected(updateProgressSignal)) {
//        connect(m_Estimator_Bees, SIGNAL(UpdateProgressData(int,int,QString)),
//                this, SLOT(callback_UpdateProgressData(int,int,QString)));
//    }

    /****************************************************/
    /* Any statements after this point will be executed */
    /* before estimateParameters finishes.              */
    /* So...beware.                                     */
    /****************************************************/

    // Show Progress Chart
    m_UI->ProgressDockWidget->show();
    m_UI->ProgressWidget->setMinimumHeight(250);

} // end runBeesAlgorithm


void
nmfMainWindow::runNLoptAlgorithm(bool showDiagnosticChart,
                                 std::vector<QString>& MultiRunLines,
                                 int& TotalIndividualRuns)
{
    QString multiRunSpeciesFilename;
    QString multiRunModelFilename;
std::cout << "RUNNING" << std::endl;

    bool isAMultiRun          = isAMultiOrMohnsRhoRun();
    bool isSetToDeterministic = Estimation_Tab7_ptr->isSetToDeterministicMinimizer();

    // Force isSetToDeterministic to be true if running Mohns Rho
    m_DataStruct.useApplicationFixedSeedNLopt = isAMohnsRhoMultiRun();

    Output_Controls_ptr->setAveraged(isAMultiRun);
    m_DataStruct.showDiagnosticChart = showDiagnosticChart;

    // Create the NLopt Estimator object
    if (m_Estimator_NLopt == nullptr) {
        m_Estimator_NLopt = new NLopt_Estimator();
    }

    // Set up connections
    disconnect(m_ProgressWidget, 0, 0, 0);
//  connect(m_ProgressWidget,  SIGNAL(StopTheRun()),
//          m_Estimator_NLopt, SLOT(callback_StopTheOptimizer()));
    connect(m_ProgressWidget,  SIGNAL(StopTheTimer()),
            this,              SLOT(callback_StopTheTimer()));
    connect(m_ProgressWidget,  SIGNAL(RedrawValidPointsOnly(bool,bool)),
            this,              SLOT(callback_ReadProgressChartDataFile(bool,bool)));
    disconnect(m_Estimator_NLopt, 0, 0, 0);
    connect(m_Estimator_NLopt, SIGNAL(RunCompleted(std::string,std::string,bool)),
            this,              SLOT(callback_RunCompleted(std::string,std::string,bool)));
    connect(m_Estimator_NLopt, SIGNAL(SubRunCompleted(int,int,std::string,std::string,std::string,std::string,std::string,double)),
            this,              SLOT(callback_SubRunCompleted(int,int,std::string,std::string,std::string,std::string,std::string,double)));
    connect(m_Estimator_NLopt, SIGNAL(AllSubRunsCompleted(std::string,std::string,QString)),
            this,              SLOT(callback_AllSubRunsCompleted(std::string,std::string,QString)));
    connect(m_Estimator_NLopt, SIGNAL(AMohnsRhoMultiRunCompleted()),
            this,              SLOT(callback_AMohnsRhoMultiRunCompleted()));
    connect(m_Estimator_NLopt, SIGNAL(NLoptFailureStopRunsAndReset()),
            m_ProgressWidget,  SLOT(callback_stopPB()));
    connect(m_Estimator_NLopt, SIGNAL(RunCompletedMsg(int,std::string)),
            this,              SLOT(callback_RunCompletedMsg(int,std::string)));
    connect(m_ProgressWidget,  SIGNAL(StopTimer()),
            this,              SLOT(callback_StopTimer()));
    connect(m_ProgressWidget,  SIGNAL(SetAllRunsComplete()),
            this,              SLOT(callback_SetAllRunsComplete()));
    connect(m_Estimator_NLopt,  SIGNAL(UpdateNumberOfParameters(int)),
            Estimation_Tab7_ptr,SLOT(callback_UpdateNumberOfParameters(int)));
    connect(m_Estimator_NLopt,  SIGNAL(LogMsg(std::vector<std::string>)),
            this,               SLOT(callback_LogMsg(std::vector<std::string>)));

    updateProgressChartAnnotation(0,(double)m_DataStruct.NLoptStopAfterIter,5.0);

    // Run the optimizer
    std::pair<bool,bool> boolPair = std::make_pair(isAMultiRun,isSetToDeterministic);
    QFuture<void> future = QtConcurrent::run(
                m_Estimator_NLopt,
                &NLopt_Estimator::estimateParameters,
                m_DataStruct,
                m_RunNumNLopt,
                boolPair,
                MultiRunLines,
                TotalIndividualRuns);

    m_ProgressWidget->hideLegend();
    m_isRunning = true;

    /****************************************************/
    /* Any statements after this point will be executed */
    /* before estimateParameters finishes.              */
    /* So...beware.                                     */
    /****************************************************/

    // Show Progress Chart
    m_UI->ProgressDockWidget->show();
    m_UI->ProgressWidget->setMinimumHeight(250);
}

void
nmfMainWindow::callback_LoadEstimatedParameters()
{
    int inc=0;
    int NumSpeciesOrGuilds;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap,dataMap2;
    std::string queryStr;
    std::string cmd;
    std::string errorMsg;
    std::string InitBiomass,GrowthRate,GrowthRateShape,SpeciesK,Catchability;
    QStringList SpeciesList;

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList)) {
        return;
    }

    QString msg  = "<html><br>OK to overwrite current parameter table(s) with new estimated parameters? ";
    msg += "<br><br>It's <strong>strongly</strong> recommended that you export your Species table";
    msg += " (and all other relevant parameter tables) to their respective backup .csv files prior to clicking Yes.<br></html>";
    QMessageBox::StandardButton okToOverwriteParameterTables =
        QMessageBox::question(this, tr("Overwrite parameter table(s)"), tr(msg.toLatin1()),
                              QMessageBox::No|QMessageBox::Yes,
                              QMessageBox::Yes);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Copy the estimated parameters from the output tables to the appropriate parameter tables
    if (okToOverwriteParameterTables == QMessageBox::Yes) {
        QProgressDialog progress("Loading...", "", 0, 14, this);
        progress.setAttribute(Qt::WA_DeleteOnClose, true);
        progress.setModal(true);
        progress.setMinimumDuration(0);
        progress.show();
        progress.setValue(inc++);
        loadEstimatedParametersIntoSpecies(SpeciesList,InitBiomassTV,     "InitBiomass");
        progress.setValue(inc++); //if (progress.wasCanceled()) { break; }
        loadEstimatedParametersIntoSpecies(SpeciesList,GrowthRateTV,      "GrowthRate");
        progress.setValue(inc++);
        loadEstimatedParametersIntoSpecies(SpeciesList,GrowthRateShapeTV, "GrowthRateShape");
        progress.setValue(inc++);
        loadEstimatedParametersIntoSpecies(SpeciesList,CarryingCapacityTV,"SpeciesK");
        progress.setValue(inc++);
        loadEstimatedParametersIntoSpecies(SpeciesList,CatchabilityTV,    "Catchability");
        progress.setValue(inc++);
        loadEstimatedParametersIntoSpecies(SpeciesList,SurveyQTV,         "SurveyQ");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationRho,             PredationRhoTV,     "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationRhoMin,          PredationRhoTV,     "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationRhoMax,          PredationRhoTV,     "SpeciesA","SpeciesB");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationHandling,        PredationHandlingTV,"SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationHandlingMin,     PredationHandlingTV,"SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationHandlingMax,     PredationHandlingTV,"SpeciesA","SpeciesB");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionAlpha,         CompetitionAlphaTV, "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionAlphaMin,      CompetitionAlphaTV, "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionAlphaMax,      CompetitionAlphaTV, "SpeciesA","SpeciesB");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaSpecies,   CompetitionBetaSTV, "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin,CompetitionBetaSTV, "SpeciesA","SpeciesB");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax,CompetitionBetaSTV, "SpeciesA","SpeciesB");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaGuilds,    CompetitionBetaGTV, "Guild",   "SpeName");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaGuildsMin, CompetitionBetaGTV, "Guild",   "SpeName");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCompetitionBetaGuildsMax, CompetitionBetaGTV, "Guild",   "SpeName");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationExponent,        PredationExponentTV,"SpeName");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationExponentMin,     PredationExponentTV,"SpeName");
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TablePredationExponentMax,     PredationExponentTV,"SpeName");
        progress.setValue(inc++);
        loadEstimatedParameters(SpeciesList,nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges,CovariateCoefficientsTV,
                                "CoeffName","CoeffMinName","CoeffMaxName","CoeffValue","CoeffMinValue","CoeffMaxValue");
        progress.setValue(inc++);
        //  loadEstimatedParameters(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuilds,CompetitionBetaGGTV,"GuildA","GuildB");
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] tbd: implement loading of competitionguildsguilds table");

        // Refresh all pertinent widgets
        Setup_Tab3_ptr->loadWidgets();
        Estimation_Tab1_ptr->loadWidgets();
        Estimation_Tab3_ptr->loadWidgets();
        progress.setValue(inc++);
        Estimation_Tab4_ptr->loadWidgets();
        Estimation_Tab6_ptr->loadWidgets();
        progress.close();
    }

    QApplication::restoreOverrideCursor();

    if (okToOverwriteParameterTables == QMessageBox::Yes) {
        msg = "<html><br>Loading completed!<br><br>Completed loading estimated parameters in all relevant parameter tables. Please ";
        msg += "update the min/max ranges as desired and re-run the model with a local Minimizer Algorithm of your choice.<br></html>";
        QMessageBox::information(this, tr("Load Completed"), tr(msg.toLatin1()),QMessageBox::Ok);
    }
}

void
nmfMainWindow::loadEstimatedParametersIntoSpecies(
        const QStringList& speciesList,
        QTableView* tableView,
        const std::string& parameterName)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string cmd;
    std::string value;
    std::string parameterMinName = parameterName + "Min";
    std::string parameterMaxName = parameterName + "Max";
    int numSpecies = 0;
    QModelIndex index;

    if (! nmfUtilsQt::isEmpty(tableView)) {
        for (QString SpeciesName : speciesList) {
            index = tableView->model()->index(numSpecies,0);
            value = index.data().toString().replace(",","").toStdString();
            cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableSpecies + " (";
            cmd += "SpeName," + parameterName + "," + parameterMinName + "," + parameterMaxName + ") ";
            cmd += "VALUES ('" + SpeciesName.toStdString() + "', " +
                    value + "," + value + "," + value + ") ";
            cmd += "ON DUPLICATE KEY UPDATE ";
            cmd += parameterName    + "= " + value + ", ";
            cmd += parameterMinName + "= " + value + ", ";
            cmd += parameterMaxName + "= " + value + ";";

            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            //std::cout << "cmd: " << cmd << std::endl;
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab7 loadEstimatedParametersIntoSpecies: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(this,"Warning",
                                     "\nCouldn't REPLACE INTO table: " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + "\n",
                                     QMessageBox::Ok);
                QApplication::restoreOverrideCursor();
                return;
            }
            ++numSpecies;
        }
    } else {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] loadEstimatedParametersIntoSpecies: Found empty table: " + tableView->objectName().toStdString());
    }
}

void
nmfMainWindow::loadEstimatedParameters(
        const QStringList& speciesList,
        const std::string& tableName,
        QTableView* tableView,
        const std::string& field1,
        const std::string& field2,
        const std::string& field3,
        const std::string& field4,
        const std::string& field5,
        const std::string& field6)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string cmd;
    QModelIndex index;
    std::string value;
    int numSpecies = 0;
    std::string parameterName1,parameterName2,parameterName3;
    std::vector<std::string> parameterNames;

   if (! nmfUtilsQt::isEmpty(tableView)) {
       // Delete previous data
       cmd = "DELETE FROM " + tableName +
               " WHERE ProjectName = '"       + m_ProjectName +
               "' AND ModelName = '"          + m_ModelName + "'";
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 1] loadEstimationParameters: DELETE error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
       // Get header info
       int numParameters = tableView->model()->columnCount();
       for (int i=0; i<numParameters; ++i) {
           parameterNames.push_back(tableView->model()->headerData(i,Qt::Horizontal).toString().replace(" ","").toStdString());
       }
       // Insert into table
       cmd = "INSERT INTO " + tableName +
             " (ProjectName,ModelName,SpeName," +
               field1 + "," + field2 + "," + field3 + "," +
               field4 + "," + field5 + "," + field6 + ") VALUES ";
       for (QString species : speciesList) {
           for (int i=0; i<numParameters; ++i) {
               index = tableView->model()->index(numSpecies,i);
               value = index.data().toString().replace(",","").toStdString();
               if (value.empty() || (std::stod(value) == 0)) {
                   cmd += "('"   + m_ProjectName +
                           "','" + m_ModelName +
                           "','" + species.toStdString() +
                           "','" + parameterNames[i] +
                           "','" + parameterNames[i] + "Min'" +
                           ",'"  + parameterNames[i] + "Max'" +
                           ",\'\',\'\',\'\'),";
               } else {
                   cmd += "('"   + m_ProjectName +
                           "','" + m_ModelName +
                           "','" + species.toStdString() +
                           "','" + parameterNames[i] +
                           "','" + parameterNames[i] + "Min'" +
                           ",'"  + parameterNames[i] + "Max'" +
                           ","   + value +
                           ","   + value +
                           ","   + value + "),";
               }
           }
           ++numSpecies;
       }
       cmd = cmd.substr(0,cmd.size()-1);
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 2] loadEstimationParameters: Write table error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
   } else {
       m_Logger->logMsg(nmfConstants::Warning,"[Warning] loadEstimationParameters: Found empty table: " + tableName);
   }
}

void
nmfMainWindow::loadEstimatedParameters(
        const QStringList& speciesList,
        const std::string& tableName,
        QTableView* tableView,
        const std::string& field)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string cmd;
    QModelIndex index;
    std::string value;
    int numSpecies = 0;

   if (! nmfUtilsQt::isEmpty(tableView)) {
       // Delete previous data
       cmd = "DELETE FROM " + tableName +
               " WHERE ProjectName = '"       + m_ProjectName +
               "' AND ModelName = '"          + m_ModelName + "'";
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 3] loadEstimationParameters: DELETE error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
       cmd = "INSERT INTO " + tableName +
             " (ProjectName,ModelName," + field + ",Value) VALUES ";
       for (QString species : speciesList) {
           index = tableView->model()->index(numSpecies,0);
           value = index.data().toString().replace(",","").toStdString();
           cmd += "('"   + m_ProjectName +
                   "','" + m_ModelName +
                   "','" + species.toStdString() +
                   "',"   + value + "),";
           ++numSpecies;
       }
       cmd = cmd.substr(0,cmd.size()-1);
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 4] loadEstimationParameters: Write table error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
   } else {
       m_Logger->logMsg(nmfConstants::Warning,"[Warning] loadEstimationParameters: Found empty table: " + tableName);
   }
}

void
nmfMainWindow::loadEstimatedParameters(
        const QStringList& speciesList,
        const std::string& tableName,
        QTableView* tableView,
        const std::string& field1,
        const std::string& field2)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string cmd;
    QModelIndex index;
    std::string value;
    int numSpeciesA,numSpeciesB;

   if (! nmfUtilsQt::isEmpty(tableView)) {
       // Delete previous data
       cmd = "DELETE FROM " + tableName +
               " WHERE ProjectName = '"       + m_ProjectName +
               "' AND ModelName = '"          + m_ModelName + "'";
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 5] loadEstimationParameters: DELETE error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
       cmd = "INSERT INTO " + tableName +
             " (ProjectName,ModelName," + field1 + "," + field2 + ",Value) VALUES ";
       numSpeciesA = 0;
       for (QString speciesA : speciesList) {
           numSpeciesB = 0;
           for (QString speciesB : speciesList) {
               index = tableView->model()->index(numSpeciesA,numSpeciesB);
               value = index.data().toString().replace(",","").toStdString();
               cmd += "('"   + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + speciesA.toStdString() +
                       "','" + speciesB.toStdString() +
                       "',"  + value + "),";
               ++numSpeciesB;
           }
           ++numSpeciesA;
       }
       cmd = cmd.substr(0,cmd.size()-1);
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 6] loadEstimationParameters: Write table error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
   } else {
       m_Logger->logMsg(nmfConstants::Warning,"[Warning] loadEstimationParameters: Found empty table: " + tableName);
   }
}


void
nmfMainWindow::callback_LogMsg(std::vector<std::string> msgVec)
{
    for (std::string msg : msgVec) {
        m_Logger->logMsg(nmfConstants::Warning,msg);
    }
}

void
nmfMainWindow::callback_RunCompletedMsg(int returnCode,
                                        std::string msg)
{
    if (! isAMultiOrMohnsRhoRun()) {
        QString msgStrLog = QString::fromStdString(msg).replace("\n"," ");
        QString msgStrDlg = QString::fromStdString(msg).replace("\n","\n\n");
        int msgType = (returnCode < 0) ? nmfConstants::Error : nmfConstants::Normal;

        // Create log and status bar messages
        m_Logger->logMsg(msgType,msgStrLog.toStdString());
        this->statusBar()->showMessage("Estimation result:  " + msgStrLog);

        // Display the estimation complete window a bit off-center so that user can
        // discern the difference between the current run and the previous run.
        m_EstimationCompleteDlg->loadText(msgStrDlg+"\n");
        m_EstimationCompleteDlg->loadPosition(this->x()+this->width()/8,
                                              this->y()+this->height()/2);
        m_EstimationCompleteDlg->exec();
    }
}

void
nmfMainWindow::callback_StopTimer()
{
//    if (m_Estimator_Bees != nullptr) {
//        m_Estimator_Bees->stopRun("--:--:--","n/a");
//    }
//    if (m_Estimator_NLopt != nullptr) {
//        m_Estimator_NLopt->callback_StopAllRuns();
// //      m_Estimator_NLopt->stopRun("--:--:--","n/a");
//    }
    if (m_ProgressWidget != nullptr) {
        m_ProgressWidget->stopTimer();
        m_ProgressWidget->updateTime();
    }
    enableRunWidgets(true);
    m_isRunning = false;
}

void
nmfMainWindow::callback_SetAllRunsComplete()
{
    // Show output from partial run
    if (m_Estimator_NLopt != nullptr) {
        m_Estimator_NLopt->stopGracefully();
    }
    if (m_Estimator_Bees != nullptr) {
        m_Estimator_Bees->stopRun("--:--:--","n/a");
    }
}

void
nmfMainWindow::callback_SetEstimatedParameterNames()
{
    Setup_Tab4_ptr->setEstimatedParameterNames();
}

void
nmfMainWindow::callback_SetDiagnosticHPCFlag()
{
    Diagnostic_Tab1_ptr->setHPCFlag(m_UsingHPCFiles);
}

void
nmfMainWindow::callback_SetRetrospectiveAnalysis(bool state)
{
    m_LastRunWasRetrospectiveAnalysis = state;
}

void
nmfMainWindow::callback_StopTheTimer()
{
std::cout << "nmfMainWindow::callback_StopTheTimer" << std::endl;
    m_isRunning = false;
    m_ProgressWidget->stopTimer();
    m_ProgressWidget->updateTime();
}


bool
nmfMainWindow::calculateSubRunBiomass(std::vector<double>& EstInitBiomass,
                                      std::vector<double>& EstGrowthRates,
                                      std::vector<double>& EstGrowthRateShape,
                                      std::vector<double>& EstGrowthRateCovariateCoeffs,
                                      std::vector<double>& EstCarryingCapacities,
                                      std::vector<double>& EstCarryingCapacityCovariateCoeffs,
                                      std::vector<double>& EstCatchability,
                                      std::vector<double>& EstCatchabilityCovariateCoeffs,
                                      std::vector<double>& EstPredationExponent,
                                      std::vector<double>& EstSurveyQ,
                                      std::vector<double>& EstSurveyQCovariateCoeffs,
                                      boost::numeric::ublas::matrix<double>& InitBiomassCovariate,
                                      boost::numeric::ublas::matrix<double>& GrowthRateCovariate,
                                      boost::numeric::ublas::matrix<double>& CarryingCapacityCovariate,
                                      boost::numeric::ublas::matrix<double>& CatchabilityCovariate,
                                      boost::numeric::ublas::matrix<double>& SurveyQCovariate,
                                      boost::numeric::ublas::matrix<double>& PredationRhoCovariate,
                                      boost::numeric::ublas::matrix<double>& PredationHandlingCovariate,
                                      boost::numeric::ublas::matrix<double>& PredationExponentCovariate,
                                      boost::numeric::ublas::matrix<double>& CompetitionAlphaCovariate,
                                      boost::numeric::ublas::matrix<double>& CompetitionBetaSpeciesCovariate,
                                      boost::numeric::ublas::matrix<double>& CompetitionBetaGuildSpeciesCovariate,
                                      boost::numeric::ublas::matrix<double>& CompetitionBetaGuildGuildCovariate,
                                      boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
                                      boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
                                      boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
                                      boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
                                      boost::numeric::ublas::matrix<double>& EstPredationRho,
                                      boost::numeric::ublas::matrix<double>& EstPredationHandling,
                                      boost::numeric::ublas::matrix<double>& EstBiomassSpecies)
{
    bool isAggProd;
    bool isBiomassAbsolute;
    int RunLength;
    int InitialYear;
    int timeMinus1;
    int guildNum;
    int NumGuilds;
    int NumSpeciesOrGuilds;
    double GuildCarryingCapacity;
    double EstBiomassVal = 0;
    double SystemCarryingCapacity = 0;
    double GrowthTerm;
    double HarvestTerm;
    double CompetitionTerm;
    double PredationTerm;
    std::string ForecastName = "";  // RSK - may need to refactor this later if going to re-use this method
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::string CovariateAlgorithmType = m_DatabasePtr->getCovariateAlgorithmType(m_Logger,m_ProjectName,m_ModelName);
    nmfGrowthForm*      growthForm;
    nmfHarvestForm*     harvestForm;
    nmfCompetitionForm* competitionForm;
    nmfPredationForm*   predationForm;
    QStringList SpeciesList;
    QStringList GuildList;
    std::vector<int>                GuildNum;
    std::map<int,std::vector<int> > GuildSpecies;
    boost::numeric::ublas::matrix<double> EstimatedBiomassByGuilds;
    boost::numeric::ublas::matrix<double> ObservedBiomassByGuilds;
    boost::numeric::ublas::matrix<double> NullMatrix;
    boost::numeric::ublas::matrix<double> Catch;
    boost::numeric::ublas::matrix<double> Effort;
    boost::numeric::ublas::matrix<double> Exploitation;
    QString tableHarvestCatch    = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);
    QString tableSpecies         = QString::fromStdString(nmfConstantsMSSPM::TableSpecies);
    QString tableBiomassAbsolute = QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute);
    QString tableBiomassRelative = QString::fromStdString(nmfConstantsMSSPM::TableBiomassRelative);
    std::map<QString,QString> previousUnits;

    m_DatabasePtr->createUnitsMap(m_ProjectName,m_ModelName,previousUnits);

    //
    // continue here.....possibly reduce code clutter...
    // RSK try calling    updateOutputBiomassTable(...nmfConstantsMSSPM::TableOutputBiomassEnsemble) instead of doing this....seems like there's repetitive code here
    //
    //
    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return false;
    }
    growthForm      = new nmfGrowthForm(GrowthForm);
    harvestForm     = new nmfHarvestForm(HarvestForm);
    competitionForm = new nmfCompetitionForm(CompetitionForm);
    predationForm   = new nmfPredationForm(PredationForm);
    isAggProd       = (CompetitionForm == "AGG-PROD");

    // Find Guilds and Species
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList)) {
        return false;
    }
    if (isAggProd) {
        SpeciesList        = GuildList;
        NumSpeciesOrGuilds = NumGuilds;
    } else {
        if (! m_DatabasePtr->getSpecies(m_Logger,NumSpeciesOrGuilds,SpeciesList)) {
            return false;
        }
    }

    nmfUtils::initialize(EstBiomassSpecies,RunLength+1,NumSpeciesOrGuilds);

    for (int j=0; j<int(EstInitBiomass.size()); ++j) {
        EstBiomassSpecies(0,j) = EstInitBiomass[j];
    }

    // Get guild map
    nmfUtils::initialize(EstimatedBiomassByGuilds,RunLength+1,NumGuilds);
    if (! m_DatabasePtr->getGuildData(m_Logger,NumGuilds,RunLength,GuildList,GuildSpecies,GuildNum,ObservedBiomassByGuilds)) {
        return false;
    }
    EstimatedBiomassByGuilds = ObservedBiomassByGuilds;

    // Calculate System Carrying Capacity
    if (isAggProd) {
        for (int i=0; i<NumGuilds; ++i) {
            SystemCarryingCapacity += EstCarryingCapacities[i];
        }
    } else {
        for (int i=0; i<NumGuilds; ++i) {
            for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                SystemCarryingCapacity += EstCarryingCapacities[GuildSpecies[i][j]];
            }
        }
    }

    // Get Harvest data
    if (HarvestForm == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        if (isAggProd) {
            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                          ForecastName,nmfConstantsMSSPM::TableHarvestCatch,
                                                          "",NumSpeciesOrGuilds,RunLength,
                                                          "",NullMatrix,Catch)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            }
        } else {
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,nmfConstantsMSSPM::TableHarvestCatch,
                                                   NumSpeciesOrGuilds,RunLength,Catch)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            }
        }
    } else if (HarvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        if (isAggProd) {
            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                          ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                                          "",NumSpeciesOrGuilds,RunLength,
                                                          "",NullMatrix,Effort))
                return false;
        } else {
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                                   NumSpeciesOrGuilds,RunLength,Effort))
                return false;
        }
    } else if (HarvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        if (isAggProd) {
            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                          ForecastName,nmfConstantsMSSPM::TableHarvestCatch,
                                                          "",NumSpeciesOrGuilds,RunLength,
                                                          "",NullMatrix,Catch)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            }
            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                          ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                                          "",NumSpeciesOrGuilds,RunLength,
                                                          "",NullMatrix,Effort))
                return false;
        } else {
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,nmfConstantsMSSPM::TableHarvestCatch,
                                                   NumSpeciesOrGuilds,RunLength,Catch)) {
                QMessageBox::warning(this, "Error",
                                     "\nError: No data found in Catch table for current Forecast.\nCheck Forecast->Harvest Data tab.",
                                     QMessageBox::Ok);
                return false;
            }
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,nmfConstantsMSSPM::TableHarvestEffort,
                                                   NumSpeciesOrGuilds,RunLength,Effort))
                return false;
        }
    } else if (HarvestForm == "Exploitation (F)") {
        if (isAggProd) {
            if (! m_DatabasePtr->getTimeSeriesDataByGuild(m_Logger,m_ProjectName,m_ModelName,
                                                          ForecastName,nmfConstantsMSSPM::TableHarvestExploitation,
                                                          "",NumSpeciesOrGuilds,RunLength,
                                                          "",NullMatrix,Exploitation))
                return false;
        } else {
            if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                                   ForecastName,nmfConstantsMSSPM::TableHarvestExploitation,
                                                   NumSpeciesOrGuilds,RunLength,Exploitation))
                return false;
        }

    }

    // Assure all appropriate dataStruct elements are in metric tons
    if (previousUnits.find(tableHarvestCatch) != previousUnits.end()) {
        nmfUtilsQt::convertMatrix(Catch, previousUnits[tableHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
    }

    for (int time=1; time <= RunLength; time++) {
        timeMinus1 = time-1;
        for (int species=0; species<NumSpeciesOrGuilds; ++species)
        {
            // Find the guild carrying capacity for guild: guildNum
            GuildCarryingCapacity = 0;
            if (isAggProd) {
                GuildCarryingCapacity = EstCarryingCapacities[species];
            } else {
                guildNum = GuildNum[species];
                for (unsigned j=0; j<GuildSpecies[guildNum].size(); ++j) {
                    GuildCarryingCapacity += EstCarryingCapacities[GuildSpecies[guildNum][j]];
                }
            }

            EstBiomassVal = EstBiomassSpecies(timeMinus1,species);

            GrowthTerm      = growthForm->evaluate(CovariateAlgorithmType,
                                                   EstBiomassVal,
                                                   EstGrowthRates[species],
                                                   EstGrowthRateShape[species],
                                                   EstGrowthRateCovariateCoeffs[species],
                                                   GrowthRateCovariate(timeMinus1,species),
                                                   EstCarryingCapacities[species],
                                                   EstCarryingCapacityCovariateCoeffs[species],
                                                   CarryingCapacityCovariate(timeMinus1,species));
            HarvestTerm     = harvestForm->evaluate(CovariateAlgorithmType,
                                                    timeMinus1,species,EstBiomassVal,
                                                    Catch,Effort,Exploitation,
                                                    EstCatchability[species],
                                                    EstCatchabilityCovariateCoeffs[species],
                                                    CatchabilityCovariate(timeMinus1,species));
            CompetitionTerm = competitionForm->evaluate(CovariateAlgorithmType,
                                                        timeMinus1,
                                                        species,
                                                        EstBiomassVal,
                                                        EstGrowthRates,
                                                        GrowthRateCovariate,
                                                        GuildCarryingCapacity,
                                                        SystemCarryingCapacity,
                                                        EstBiomassSpecies,
                                                        EstimatedBiomassByGuilds,
                                                        EstCompetitionAlpha,
                                                        CompetitionAlphaCovariate,
                                                        EstCompetitionBetaSpecies,
                                                        CompetitionBetaSpeciesCovariate,
                                                        EstCompetitionBetaGuilds,
                                                        CompetitionBetaGuildSpeciesCovariate,
                                                        EstCompetitionBetaGuildsGuilds,
                                                        CompetitionBetaGuildGuildCovariate);
            PredationTerm   = predationForm->evaluate(CovariateAlgorithmType,
                                                      timeMinus1,species,
                                                      EstBiomassSpecies,EstBiomassVal,
                                                      EstPredationRho,PredationRhoCovariate,
                                                      EstPredationHandling,PredationHandlingCovariate,
                                                      EstPredationExponent,PredationExponentCovariate);

            EstBiomassVal += GrowthTerm - HarvestTerm - CompetitionTerm - PredationTerm;

            if ( std::isnan(std::fabs(EstBiomassVal)) ||
                (EstBiomassVal < 0) )
            {
                EstBiomassVal = m_DataStruct.MinimumBiomass[species];
            }
            EstBiomassSpecies(time,species) = EstBiomassVal;

            // update estBiomassGuilds for next time step
            if (isAggProd) {
                for (int i=0; i<NumGuilds; ++i) {
                    EstimatedBiomassByGuilds(time,i) = EstBiomassSpecies(time,i);
                }
            } else {
                for (int i=0; i<NumGuilds; ++i) {
                    for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                        EstimatedBiomassByGuilds(time,i) += EstBiomassSpecies(time,GuildSpecies[i][j]);
                    }
                }
            }

            EstBiomassSpecies(time,species) = nmfUtils::round(EstBiomassSpecies(time,species),6);

        }
    }

    return true;
}

void
nmfMainWindow::callback_InitializeSubRuns(
        std::string multiRunModelFilename,
        int totalIndividualRuns)
{
//    int NumSpecies;
//    QStringList SpeciesList;
//    std::vector<double> InitialBiomass;
//    std::vector<double> InitialGrowthRate;
//    std::vector<double> InitialSpeciesK;

    m_finalList.clear();

//    if (! m_DatabasePtr->getSpeciesInitialData(m_Logger,NumSpecies,SpeciesList,InitialBiomass,
//                                               InitialGrowthRate,InitialSpeciesK)) {
//        return;
//    }
//    QString fullPath      = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
//    QString fullModelPath = QDir(fullPath).filePath(QString::fromStdString(multiRunModelFilename));
//    QFile modelFile(fullModelPath);
//    modelFile.remove();
//    if (modelFile.open(QIODevice::Append)) {
//        QTextStream stream(&modelFile);
//        stream << "Run #,MEF,AIC,Algorithm,Minimizer,Objective Criterion,Scaling\n";
//        modelFile.close();
//    }

    // Update Progress Chart
    m_ProgressWidget->setRunBoxes(1,0,totalIndividualRuns);
}

void
nmfMainWindow::getEstimatedParameters(
         const int& run,
         std::vector<double>& EstInitBiomass,
         std::vector<double>& EstGrowthRates,
         std::vector<double>& EstGrowthRateShape,
         std::vector<double>& EstGrowthRateCovariateCoeffs,
         std::vector<double>& EstCarryingCapacities,
         std::vector<double>& EstCarryingCapacityCovariateCoeffs,
         std::vector<double>& EstCatchability,
         std::vector<double>& EstCatchabilityCovariateCoeffs,
         std::vector<double>& EstSurveyQ,
         std::vector<double>& EstSurveyQCovariateCoeffs,
         boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
         boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
         boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
         boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
         boost::numeric::ublas::matrix<double>& EstPredationRho,
         boost::numeric::ublas::matrix<double>& EstPredationHandling,
         std::vector<double>& EstPredationExponent,
         std::vector<double>& EstBMSY,
         std::vector<double>& EstMSY,
         std::vector<double>& EstFMSY)
{
    if (m_UsingHPCFiles) {
        QString hpcDir  = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataHPCDir));
        QString hpcFile = QDir(hpcDir).filePath(QString::fromStdString("InitBiomass_"+std::to_string(run)+".csv"));

        nmfUtilsQt::loadFromHPCFile(hpcFile,EstInitBiomass);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("GrowthRate_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstGrowthRates);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("GrowthRateShape_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstGrowthRateShape);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("GrowthRateCovariateCoeffs_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstGrowthRateCovariateCoeffs);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CarryingCapacity_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCarryingCapacities);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CarryingCapacityCovariateCoeffs_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCarryingCapacityCovariateCoeffs);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("Catchability_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCatchability);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CatchabilityCovariateCoeffs_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCatchabilityCovariateCoeffs);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("SurveyQ_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstSurveyQ);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("SurveyQCovariateCoeffs_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstSurveyQCovariateCoeffs);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CompetitionAlpha_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCompetitionAlpha);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CompetitionBetaSpecies_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCompetitionBetaSpecies);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CompetitionBetaGuilds_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCompetitionBetaGuilds);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("CompetitionBetaGuildsGuilds_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstCompetitionBetaGuildsGuilds);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("PredationRho_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstPredationRho);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("PredationHandling_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstPredationHandling);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("PredationExponent_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstPredationExponent);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("BMSY_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstBMSY);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("MSY_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstMSY);
        hpcFile = QDir(hpcDir).filePath(QString::fromStdString("FMSY_"+std::to_string(run)+".csv"));
        nmfUtilsQt::loadFromHPCFile(hpcFile,EstFMSY);
    } else {
        m_Estimator_NLopt->getEstInitBiomass(EstInitBiomass);
        m_Estimator_NLopt->getEstGrowthRates(EstGrowthRates);
        m_Estimator_NLopt->getEstGrowthRateShape(EstGrowthRateShape);
        m_Estimator_NLopt->getEstGrowthRateCovariateCoeffs(EstGrowthRateCovariateCoeffs);
        m_Estimator_NLopt->getEstCarryingCapacities(EstCarryingCapacities);
        m_Estimator_NLopt->getEstCarryingCapacityCovariateCoeffs(EstCarryingCapacityCovariateCoeffs);
        m_Estimator_NLopt->getEstCatchability(EstCatchability);
        m_Estimator_NLopt->getEstCatchabilityCovariateCoeffs(EstCatchabilityCovariateCoeffs);
        m_Estimator_NLopt->getEstSurveyQ(EstSurveyQ);
        m_Estimator_NLopt->getEstSurveyQCovariateCoeffs(EstSurveyQCovariateCoeffs);

        m_Estimator_NLopt->getEstCompetitionAlpha(EstCompetitionAlpha);
        m_Estimator_NLopt->getEstCompetitionBetaSpecies(EstCompetitionBetaSpecies);
        m_Estimator_NLopt->getEstCompetitionBetaGuilds(EstCompetitionBetaGuilds);
        m_Estimator_NLopt->getEstCompetitionBetaGuildsGuilds(EstCompetitionBetaGuildsGuilds);

        m_Estimator_NLopt->getEstPredation(EstPredationRho);
        m_Estimator_NLopt->getEstHandling(EstPredationHandling);
        m_Estimator_NLopt->getEstExponent(EstPredationExponent);
        m_Estimator_NLopt->getEstBMSY(EstBMSY);
        m_Estimator_NLopt->getEstMSY(EstMSY);
        m_Estimator_NLopt->getEstFMSY(EstFMSY);
    }
}

void
nmfMainWindow::callback_SubRunCompleted(int run,
                                        int numRuns,
                                        std::string Algorithm,
                                        std::string Minimizer,
                                        std::string ObjectiveCriterion,
                                        std::string Scaling,
                                        std::string multiRunModelFilename,
                                        double fitness)
{
//std::cout << "cSRC ---> run: " << run << ", fitness: " << fitness << std::endl;
    int RunLength;
    int NumYears;
    int InitialYear;
    int Peel;
    bool isBiomassAbsolute;
    bool isAMohnsRho = isAMohnsRhoMultiRun();
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    int NumSpecies;
    std::vector<double> InitialBiomass;
    std::vector<double> InitialGrowthRate;
    std::vector<double> InitialGrowthRateShape;
    std::vector<double> InitialSpeciesK;
    std::vector<double> EstInitBiomass;
    std::vector<double> EstGrowthRates;
    std::vector<double> EstGrowthRateShape;
    std::vector<double> EstGrowthRateCovariateCoeffs;
    std::vector<double> EstCarryingCapacities;
    std::vector<double> EstCarryingCapacityCovariateCoeffs;
    std::vector<double> EstPredationExponent;
    std::vector<double> EstCatchability;
    std::vector<double> EstCatchabilityCovariateCoeffs;
    std::vector<double> EstSurveyQ;
    std::vector<double> EstSurveyQCovariateCoeffs;
    std::vector<double> EstBMSY;
    std::vector<double> EstMSY;
    std::vector<double> EstFMSY;
    boost::numeric::ublas::matrix<double> GrowthRateCovariate;
    boost::numeric::ublas::matrix<double> CarryingCapacityCovariate;
    boost::numeric::ublas::matrix<double> CatchabilityRatesCovariate;
    boost::numeric::ublas::matrix<double> SurveyQCovariate;
    boost::numeric::ublas::matrix<double> InitBiomassCovariate;
    boost::numeric::ublas::matrix<double> PredationRhoCovariate;
    boost::numeric::ublas::matrix<double> PredationHandlingCovariate;
    boost::numeric::ublas::matrix<double> PredationExponentCovariate;
    boost::numeric::ublas::matrix<double> CompetitionAlphaCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildGuildCovariate;
    boost::numeric::ublas::matrix<double> EstCompetitionAlpha;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double> EstCompetitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> EstPredationRho;
    boost::numeric::ublas::matrix<double> EstPredationHandling;
    boost::numeric::ublas::matrix<double> CalculatedBiomass;
    QStringList SpeciesList;

    if (run == 0) {
        std::cout << "*** Creating AveragedData object" << std::endl;
        m_AveragedData = nmfUtilsStatisticsAveraging(); // clear struct
    }

    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return;
    }
    NumYears = RunLength + 1;
    bool isAggProd = (CompetitionForm == "AGG-PROD");

    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList);

    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"InitBiomass",                   InitBiomassCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"GrowthRate",                    GrowthRateCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CarryingCapacity",              CarryingCapacityCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"SurveyQ",                       SurveyQCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"Catchability",                  CatchabilityRatesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationRho",                  PredationRhoCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationHandling",             PredationHandlingCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationExponent",             PredationExponentCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionAlpha",              CompetitionAlphaCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaSpeciesSpecies", CompetitionBetaSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildSpecies",   CompetitionBetaGuildSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildGuild",     CompetitionBetaGuildGuildCovariate);

    // Get NLopt estimated data in order to calculate outputbiomass
    getEstimatedParameters(
                run,
                EstInitBiomass,
                EstGrowthRates,
                EstGrowthRateShape,
                EstGrowthRateCovariateCoeffs,
                EstCarryingCapacities,
                EstCarryingCapacityCovariateCoeffs,
                EstCatchability,
                EstCatchabilityCovariateCoeffs,
                EstSurveyQ,
                EstSurveyQCovariateCoeffs,
                EstCompetitionAlpha,
                EstCompetitionBetaSpecies,
                EstCompetitionBetaGuilds,
                EstCompetitionBetaGuildsGuilds,
                EstPredationRho,
                EstPredationHandling,
                EstPredationExponent,
                EstBMSY,
                EstMSY,
                EstFMSY);

    if (! calculateSubRunBiomass(EstInitBiomass,
                                 EstGrowthRates,
                                 EstGrowthRateShape,
                                 EstGrowthRateCovariateCoeffs,
                                 EstCarryingCapacities,
                                 EstCarryingCapacityCovariateCoeffs,
                                 EstCatchability,
                                 EstCatchabilityCovariateCoeffs,
                                 EstPredationExponent,
                                 EstSurveyQ,
                                 EstSurveyQCovariateCoeffs,
                                 InitBiomassCovariate,
                                 GrowthRateCovariate,
                                 CarryingCapacityCovariate,
                                 CatchabilityRatesCovariate,
                                 SurveyQCovariate,
                                 PredationRhoCovariate,
                                 PredationHandlingCovariate,
                                 PredationExponentCovariate,
                                 CompetitionAlphaCovariate,
                                 CompetitionBetaSpeciesCovariate,
                                 CompetitionBetaGuildSpeciesCovariate,
                                 CompetitionBetaGuildGuildCovariate,
                                 EstCompetitionAlpha,
                                 EstCompetitionBetaSpecies,
                                 EstCompetitionBetaGuilds,
                                 EstCompetitionBetaGuildsGuilds,
                                 EstPredationRho,
                                 EstPredationHandling,
                                 CalculatedBiomass)) {
        return;
    }

//for (int ii=0; ii<(int)EstGrowthRates.size(); ++ii) {
//std::cout << "~~~~> EstGrowthRates[" << ii << "]: " << EstGrowthRates[ii] <<
//             ", EstCarryingCapacities[" << ii << "]: " << EstCarryingCapacities[ii] << std::endl;
//}
    updateBiomassEnsembleTable(run,Algorithm,Minimizer,
                               ObjectiveCriterion,Scaling,CalculatedBiomass);

    // Calculate the summary statistics (MEF and AIC for now)
    StatStruct statStruct;

    // Check to see if HPC run and store Summary Statistics accordingly
    getCurrentAlgorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    if (! calculateSummaryStatisticsStruct(
                nmfConstantsMSSPM::SkipFirstYear,isAggProd,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                RunLength,NumSpecies,false,true,
                CalculatedBiomass,statStruct))
    {
        return;
    }

    // Update after every run and arrange species data in appropriate
    // format to write to file
    QString line;
    // Enter values defined per species
    m_NumRuns = run+1;

    m_AveragedData.loadEstData(fitness,
                                statStruct.aic,
                                EstInitBiomass,
                                EstGrowthRates,
                                EstGrowthRateShape,
                                EstGrowthRateCovariateCoeffs,
                                EstCarryingCapacities,
                                EstCarryingCapacityCovariateCoeffs,
                                EstPredationExponent,
                                EstCatchability,
                                EstCatchabilityCovariateCoeffs,
                                EstSurveyQ,
                                EstSurveyQCovariateCoeffs,
                                EstBMSY,
                                EstMSY,
                                EstFMSY,
                                EstCompetitionAlpha,
                                EstCompetitionBetaSpecies,
                                EstCompetitionBetaGuilds,
                                EstCompetitionBetaGuildsGuilds,
                                EstPredationRho,
                                EstPredationHandling,
                                CalculatedBiomass);

    if (isAMohnsRho) {
        if (run == 0) {
            m_MohnsRhoData = nmfUtilsStatisticsMohnsRho();
        }
        Peel = run;
        m_MohnsRhoData.loadEstData(Peel,
                                    EstInitBiomass,
                                    EstGrowthRates,
                                    EstGrowthRateShape,
                                    EstCarryingCapacities,
                                    EstPredationExponent,
                                    EstCatchability,
                                    EstSurveyQ,
                                    EstCompetitionAlpha,
                                    EstCompetitionBetaSpecies,
                                    EstCompetitionBetaGuilds,
                                    EstCompetitionBetaGuildsGuilds,
                                    EstPredationRho,
                                    EstPredationHandling,
                                    CalculatedBiomass);

    }

    // Update Progress Chart
    m_ProgressWidget->setRunBoxes(1,run+1,numRuns);

}


void
nmfMainWindow::updateBiomassEnsembleTable(
        const int& RunNumber,
        const std::string& Algorithm,
        const std::string& Minimizer,
        const std::string& ObjectiveCriterion,
        const std::string& Scaling,
        const boost::numeric::ublas::matrix<double>& CalculatedBiomass)
{
    int RunLength  = CalculatedBiomass.size1();
    int NumSpecies;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Label = ""; // Temporarily keep blank until find a use for this field
    std::string isAggProd = "0";
    QString msg;
    QStringList SpeciesList;

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;


// RSK remove this DELETE statement from here and place outside of loop
    // Delete previous run's data prior to writing the data for the same run
    if (! m_UsingHPCFiles) {
        cmd = "DELETE FROM " + nmfConstantsMSSPM::TableOutputBiomassEnsemble +
                " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Label = '"              + Label +
                "' AND RunNumber = "           + std::to_string(RunNumber) +
                "  AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] updateBiomassEnsembleTable: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            msg = "\n[Error 2] updateBiomassEnsembleTable:  Couldn't delete all records from OutputBiomassEnsemble table.\n";
            QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
            return;
        }
    }

    // Store Calculated Biomass data in: OutputBiomassEnsemble
    cmd = "INSERT INTO " + nmfConstantsMSSPM::TableOutputBiomassEnsemble +
          " (ProjectName,ModelName,Label,RunNumber,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value) VALUES ";
    for (int species=0; species<NumSpecies; ++species) {
        for (int time=0; time<RunLength; ++time) {
            cmd += "('"   + m_ProjectName +
                    "','" + m_ModelName +
                    "','" + Label +
                    "',"  + std::to_string(RunNumber) +
                    ",'"  + Algorithm +
                    "','" + Minimizer +
                    "','" + ObjectiveCriterion +
                    "','" + Scaling +
                    "',"  + isAggProd +
                    ",'"  + SpeciesList[species].toStdString() +
                    "',"  + std::to_string(time) +
                    ","   + QString::number(CalculatedBiomass(time,species),'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 3] updateBiomassEnsembleTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }

}


void
nmfMainWindow::callback_AMohnsRhoMultiRunCompleted()
{
    // Save out biomass average to TableOutputBiomassMohnsRhoOfEnsembles
std::cout << "callback_AMohnsRhoMultiRunCompleted" << std::endl;
    // Check table

}
void
nmfMainWindow::callback_AllSubRunsCompleted(
        std::string elapsedTime,
        std::string summaryMsg,
        QString msgSuffix)
{
    QString convMsg = "";

    m_Logger->logMsg(nmfConstants::Normal,"callback_AllSubRunsCompleted");

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (isAMohnsRhoMultiRun()) {
        // Refresh the output chart page
        Output_Controls_ptr->setForBiomassVsTime();
        Output_Controls_ptr->setForMohnsRho();
        updateDiagnosticSummaryStatistics();
    } else {
        // Calculate the average biomass and display the chart
        calculateAveragedParametersAndUpdateOutputTables();
        displayAverageBiomass();
        callback_UpdateSummaryStatistics();
        Output_Controls_ptr->setForBiomassVsTime();
    }

    Diagnostic_Tab2_ptr->setMohnsRhoForSingleRun(false);
    enableRunWidgets(true);
    Estimation_Tab7_ptr->enableAddToReview(true);

    refreshOutputTables();

    // Update Run Summary page
    createOutputStrMultiRun(elapsedTime,summaryMsg);

    updateRunSummaryPage(convMsg,QString::fromStdString("<html>"+summaryMsg+"</html>"));

    // Need to make sure timer is stopped properly before the next run
    callback_StopTheTimer();

    QApplication::restoreOverrideCursor();

    // Notify user the multi-run has completed
    QString msg  = (isAMohnsRhoMultiRun()) ?
         "\nThe Mohn's Rho Run has successfully completed.\n" :
         "\nThe Multi-Run has successfully completed.\n";
    msg += msgSuffix;
    QMessageBox::information(this, tr("Multi-Run Completed"),
                             tr(msg.toLatin1()),QMessageBox::Ok);
}

void
nmfMainWindow::callback_EnableRunButtons(bool state)
{
    enableRunWidgets(state);
    QApplication::sync();
}

void
nmfMainWindow::enableRunWidgets(bool state)
{
    m_UI->actionRunTB->setEnabled(state);
    Output_Controls_ptr->enableControls(state);
    Estimation_Tab7_ptr->enableRunButton(state);
    Diagnostic_Tab1_ptr->enableRunButton(state);
    Diagnostic_Tab2_ptr->enableRunButton(state);
    Forecast_Tab4_ptr->enableRunButton(state);
}

void
nmfMainWindow::calculateAveragedParametersAndUpdateOutputTables()
{
    int InitialYear = 0;
    int NumGuilds;
    int NumSpecies;
    int RunLength;
    int NumYears;
    bool isBiomassAbsolute;
    QString aveAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm();
    std::string aveAlgorithmStr = aveAlgorithm.toStdString();
    std::vector<double> Fitness;
    std::vector<double> AveInitBiomass;
    std::vector<double> AveGrowthRates;
    std::vector<double> AveGrowthRateShape;
    std::vector<double> AveGrowthRateCovariateCoeffs;
    std::vector<double> AveCarryingCapacities;
    std::vector<double> AveCarryingCapacityCovariateCoeffs;
    std::vector<double> AvePredationExponent;
    std::vector<double> AveCatchability;
    std::vector<double> AveCatchabilityCovariateCoeffs;
    std::vector<double> AveSurveyQ;
    std::vector<double> AveSurveyQCovariateCoeffs;
    std::vector<double> AveBMSY;
    std::vector<double> AveMSY;
    std::vector<double> AveFMSY;
    boost::numeric::ublas::matrix<double> InitBiomassCovariate;
    boost::numeric::ublas::matrix<double> GrowthRateCovariate;
    boost::numeric::ublas::matrix<double> CarryingCapacityCovariate;
    boost::numeric::ublas::matrix<double> CatchabilityCovariate;
    boost::numeric::ublas::matrix<double> SurveyQCovariate;
    boost::numeric::ublas::matrix<double> PredationRhoCovariate;
    boost::numeric::ublas::matrix<double> PredationHandlingCovariate;
    boost::numeric::ublas::matrix<double> PredationExponentCovariate;
    boost::numeric::ublas::matrix<double> CompetitionAlphaCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildGuildCovariate;
    boost::numeric::ublas::matrix<double> AveCompetitionAlpha;
    boost::numeric::ublas::matrix<double> AveCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> AveCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double> AveCompetitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> AvePredationRho;
    boost::numeric::ublas::matrix<double> AvePredationHandling;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    QStringList GuildList;
    QStringList SpeciesList;

    m_AveBiomass.clear();

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildList))
        return;

    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return;
    }
    NumYears = RunLength + 1;

    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"InitBiomass",                   InitBiomassCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"GrowthRate",                    GrowthRateCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CarryingCapacity",              CarryingCapacityCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"SurveyQ",                       SurveyQCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"Catchability",                  CatchabilityCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationRho",                  PredationRhoCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationHandling",             PredationHandlingCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationExponent",             PredationExponentCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionAlpha",              CompetitionAlphaCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaSpeciesSpecies", CompetitionBetaSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildSpecies",   CompetitionBetaGuildSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildGuild",     CompetitionBetaGuildGuildCovariate);

    bool isCompetitionAGGPROD = (CompetitionForm == "AGG-PROD"); // RSK - incorrect since we're talking about averages here
//std::cout << "Warning: TBD nmfMainWindow::calculateAverageBiomass: refine logic here" << std::endl;
    int usingTopValue = 100;
    if (Estimation_Tab7_ptr->getEnsembleUsingBy() == "using Top:") {
        usingTopValue = Estimation_Tab7_ptr->getEnsembleUsingAmountValue();
    }

    bool isUsingAll = Estimation_Tab7_ptr->isEnsembleUsingAll();
    bool isPercent  = Estimation_Tab7_ptr->isEnsembleUsingPct();

    m_AveragedData.calculateAverage(usingTopValue,isUsingAll,isPercent,aveAlgorithm);
    m_AveragedData.getAveData(Fitness,
                               AveInitBiomass,
                               AveGrowthRates,
                               AveGrowthRateShape,
                               AveGrowthRateCovariateCoeffs,
                               AveCarryingCapacities,
                               AveCarryingCapacityCovariateCoeffs,
                               AvePredationExponent,
                               AveCatchability,
                               AveCatchabilityCovariateCoeffs,
                               AveSurveyQ,
                               AveSurveyQCovariateCoeffs,
                               AveBMSY,
                               AveMSY,
                               AveFMSY,
                               AveCompetitionAlpha,
                               AveCompetitionBetaSpecies,
                               AveCompetitionBetaGuilds,
                               AveCompetitionBetaGuildsGuilds,
                               AvePredationRho,
                               AvePredationHandling,
                               m_AveBiomass);
//nmfUtils::printVector("AveCarryingCapacities",6,AveCarryingCapacities);
//nmfUtils::printVector("AveCarryingCapacityCovariateCoeffs",6,AveCarryingCapacityCovariateCoeffs);
//nmfUtils::printVector("AveGrowthRateCovariateCoeffs",6,AveGrowthRateCovariateCoeffs);
//nmfUtils::printVector("AveSurveyQCovariateCoeffs",6,AveSurveyQCovariateCoeffs);

    // Save AveragedBiomass as OutputBiomass
    if (Estimation_Tab7_ptr->getEnsembleAverageBy() == "by Parameter") {
        calculateSubRunBiomass(AveInitBiomass,
                               AveGrowthRates,
                               AveGrowthRateShape,
                               AveGrowthRateCovariateCoeffs,
                               AveCarryingCapacities,
                               AveCarryingCapacityCovariateCoeffs,
                               AveCatchability,
                               AveCatchabilityCovariateCoeffs,
                               AvePredationExponent,
                               AveSurveyQ,
                               AveSurveyQCovariateCoeffs,

                               InitBiomassCovariate,
                               GrowthRateCovariate,
                               CarryingCapacityCovariate,
                               CatchabilityCovariate,
                               SurveyQCovariate,
                               PredationRhoCovariate,
                               PredationHandlingCovariate,
                               PredationExponentCovariate,
                               CompetitionAlphaCovariate,
                               CompetitionBetaSpeciesCovariate,
                               CompetitionBetaGuildSpeciesCovariate,
                               CompetitionBetaGuildGuildCovariate,

                               AveCompetitionAlpha,
                               AveCompetitionBetaSpecies,
                               AveCompetitionBetaGuilds,
                               AveCompetitionBetaGuildsGuilds,
                               AvePredationRho,
                               AvePredationHandling,
                               m_AveBiomass);
    }

    updateOutputBiomassDatabaseTableWithAverageBiomass(m_AveBiomass);
    updateOutputBiomassOutputTableWithAveragedBiomass(m_AveBiomass);
    updateOutputTables(aveAlgorithmStr,
                       aveAlgorithmStr,
                       aveAlgorithmStr,
                       aveAlgorithmStr,
                       isCompetitionAGGPROD,
                       SpeciesList,
                       GuildList,
                       AveInitBiomass,
                       AveGrowthRates,
                       AveGrowthRateShape,
                       AveGrowthRateCovariateCoeffs,
                       AveCarryingCapacities,
                       AveCarryingCapacityCovariateCoeffs,
                       AveCatchability,
                       AveCatchabilityCovariateCoeffs,
                       AveCompetitionAlpha,
                       AveCompetitionBetaSpecies,
                       AveCompetitionBetaGuilds,
                       AveCompetitionBetaGuildsGuilds,
                       AvePredationRho,
                       AvePredationHandling,
                       AvePredationExponent,
                       AveSurveyQ,
                       AveSurveyQCovariateCoeffs,
                       AveBMSY,
                       AveMSY,
                       AveFMSY);
    updateOutputTableViews(SpeciesList,
                           GuildList,
                           AveInitBiomass,
                           AveGrowthRates,
                           AveGrowthRateShape,
                           AveCarryingCapacities,
                           AveCatchability,
                           AvePredationExponent,
                           AveSurveyQ,
                           AveBMSY,
                           AveMSY,
                           AveFMSY,
                           AveCompetitionAlpha,
                           AveCompetitionBetaSpecies,
                           AveCompetitionBetaGuilds,
                           AveCompetitionBetaGuildsGuilds,
                           AvePredationRho,
                           AvePredationHandling);
    m_OutputBiomassEnsemble.clear();
    getOutputBiomassEnsemble(m_AveBiomass.size1(),m_AveBiomass.size2(),m_OutputBiomassEnsemble);

}

void
nmfMainWindow::updateOutputTableViews(
        const QStringList&                           SpeciesList,
        const QStringList&                           GuildList,
        const std::vector<double>&                   EstInitBiomass,
        const std::vector<double>&                   EstGrowthRates,
        const std::vector<double>&                   EstGrowthRateShape,
        const std::vector<double>&                   EstCarryingCapacities,
        const std::vector<double>&                   EstCatchability,
        const std::vector<double>&                   EstPredationExponent,
        const std::vector<double>&                   EstSurveyQ,
        const std::vector<double>&                   EstBMSY,
        const std::vector<double>&                   EstMSY,
        const std::vector<double>&                   EstFMSY,
        const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
        const boost::numeric::ublas::matrix<double>& EstPredationRho,
        const boost::numeric::ublas::matrix<double>& EstPredationHandling)
{
    int NumSpeciesOrGuilds = EstInitBiomass.size();
    double val;
    QStandardItemModel* smodel;
    QStandardItem* item;
    QStringList hLabels;

    QList<std::vector<double> > ListVectors = {EstInitBiomass,
                                               EstGrowthRates,
                                               EstGrowthRateShape,
                                               EstCarryingCapacities,
                                               EstCatchability,
                                               EstPredationExponent,
                                               EstSurveyQ,
                                               EstBMSY,
                                               EstMSY,
                                               EstFMSY};

    QList<QTableView*> VectorTableViews  =
        {InitBiomassTV,GrowthRateTV,GrowthRateShapeTV,
         CarryingCapacityTV,CatchabilityTV,
         PredationExponentTV,SurveyQTV,BMSYTV,MSYTV,FMSYTV};

    QList<boost::numeric::ublas::matrix<double>> ListMatrices =
        {EstPredationRho,EstPredationHandling,
         EstCompetitionAlpha,EstCompetitionBetaSpecies,
         EstCompetitionBetaGuilds,EstCompetitionBetaGuildsGuilds};

    QList<QTableView*> MatrixTableViews =
        {PredationRhoTV,PredationHandlingTV,
         CompetitionAlphaTV,CompetitionBetaSTV,
         CompetitionBetaGTV};
std::cout << "Warning: TBD nmfMainWindow::updateOutputTableViews - add CompetitionBetaGGTV" << std::endl;

    // Load Vector tableviews
    for (int i=0; i<(int)nmfConstantsMSSPM::OutputVectorTables.size(); ++i) {
        hLabels.clear();
        smodel = new QStandardItemModel( NumSpeciesOrGuilds, 1 );
        for (int row=0; row<NumSpeciesOrGuilds; ++row) {
            val  = ListVectors[i][row];
            item = new QStandardItem(QString::number(val,'g',6));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, 0, item);
        }
        smodel->setVerticalHeaderLabels(SpeciesList);
        hLabels << nmfConstantsMSSPM::OutputVectorTableHeaders[i];
        smodel->setHorizontalHeaderLabels(hLabels);
        VectorTableViews[i]->setModel(smodel);
        VectorTableViews[i]->resizeColumnsToContents();
    }

    // Load Matrix tableviews
    for (int i=0; i<MatrixTableViews.size(); ++i) {
        smodel = new QStandardItemModel( NumSpeciesOrGuilds, NumSpeciesOrGuilds );
        if ( (int(ListMatrices[i].size1()) == NumSpeciesOrGuilds) &&
             (int(ListMatrices[i].size2()) == NumSpeciesOrGuilds)) {
            for (int row=0; row<NumSpeciesOrGuilds; ++row) {
                for (int col=0; col<NumSpeciesOrGuilds; ++col) {
                    val  = ListMatrices[i](row,col);
                    item = new QStandardItem(QString::number(val,'e',6));
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(row,col,item);
                }
            }
        }
        smodel->setVerticalHeaderLabels(SpeciesList);
        smodel->setHorizontalHeaderLabels(SpeciesList);
        MatrixTableViews[i]->setModel(smodel);
        MatrixTableViews[i]->resizeColumnsToContents();
    }

    // RSK - hide blank tables todo

    // Load the Covariate Coefficients table
    updateOutputCovariateCoefficientsTable(SpeciesList);
}

void
nmfMainWindow::updateOutputCovariateCoefficientsTable(const QStringList& SpeciesList)
{
    int NumRecords;
    int NumSpecies = SpeciesList.size();
    int NumCovariateParameters;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string parameterName;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    QList<std::string> CovariateTableNames;
    QStringList CovariateParameters;
    QStandardItemModel* smodel;
    QStandardItem* item;

    // Load only the appropriate covariate tables
    fields   = {"ParameterName"};
    queryStr = "SELECT DISTINCT ParameterName FROM " +
                nmfConstantsMSSPM::TableCovariateAssignment +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumCovariateParameters = dataMap["ParameterName"].size();
    for (int i=0; i<NumCovariateParameters; ++i) {
        parameterName = dataMap["ParameterName"][i];
        if (parameterName == "GrowthRate") {
            CovariateTableNames.append(nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs);
            CovariateParameters << "Growth Rate";
        } else if (parameterName == "CarryingCapacity") {
            CovariateTableNames.append(nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs);
            CovariateParameters << "Carrying Capacity";
        } else if (parameterName == "Catchability") {
            CovariateTableNames.append(nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs);
            CovariateParameters << "Catchability";
        } else if (parameterName == "SurveyQ") {
            CovariateTableNames.append(nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs);
            CovariateParameters << "SurveyQ";
        }
    }
    smodel = new QStandardItemModel(NumSpecies,NumCovariateParameters);

    if (isAMultiRun()) {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    } else {
        m_DatabasePtr->getAlgorithmIdentifiers(
                    this,m_Logger,m_ProjectName,m_ModelName,
                    Algorithm,Minimizer,ObjectiveCriterion,
                    Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    }

    // Populate the covariate coefficients output table
    for (int col=0; col<NumCovariateParameters; ++col) {
        // Read table data and write into Covariate Coefficients table
        fields     = {"SpeName","Value"};
        queryStr   = "SELECT SpeName,Value FROM "   + CovariateTableNames[col] +
                     " WHERE ProjectName = '"       + m_ProjectName +
                     "' AND ModelName = '"          + m_ModelName +
                     "' AND Algorithm = '"          + Algorithm +
                     "' AND Minimizer = '"          + Minimizer +
                     "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                     "' AND Scaling = '"            + Scaling +
                     "' ORDER BY SpeName ";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords == NumSpecies) {
            for (int row=0; row<NumSpecies; ++row) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][row]));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(row,col,item);
            }
        }
    }
    smodel->setVerticalHeaderLabels(SpeciesList);
    smodel->setHorizontalHeaderLabels(CovariateParameters);
    CovariateCoefficientsTV->setModel(smodel);
    CovariateCoefficientsTV->resizeColumnsToContents();
}

bool
nmfMainWindow::getOutputBiomassEnsembleLineLabels(QList<QString>& lineLabels)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string min;
    std::string obj;
    std::string sca;
    std::map<std::string,std::string> objMap;
    std::map<std::string,std::string> scaMap;

    objMap["Maximum Likelihood"] = "_MxLk";
    objMap["Least Squares"]      = "_LsSq";
    objMap["Model Efficiency"]   = "_MdEf";
    scaMap["Mean"]               = "_mn";
    scaMap["Min Max"]            = "_mm";

    lineLabels.clear();

    fields   = {"RunNumber","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr = "SELECT DISTINCT RunNumber,Minimizer,ObjectiveCriterion,Scaling FROM " +
                 nmfConstantsMSSPM::TableOutputBiomassEnsemble +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName  +
                "' ORDER BY RunNumber";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    for (int i=0; i<int(dataMap["RunNumber"].size()); ++i) {
        min = dataMap["Minimizer"][i];
        obj = dataMap["ObjectiveCriterion"][i];
        sca = dataMap["Scaling"][i];
        lineLabels.push_back(QString::fromStdString(min+objMap[obj]+scaMap[sca]));
    }

    return true;
}

void
nmfMainWindow::displayAverageBiomass()
{
    // Show the averaged plot plus all of the individual plots
    int NumSpecies;
    int NumGuilds;
    int RunLength;
    int InitialYear;
    bool isBiomassAbsolute;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    QStringList ColumnLabelsForLegend;
    QString OutputSpecies   = Output_Controls_ptr->getOutputSpecies();
    QString ScaleStr        = Output_Controls_ptr->getOutputScale();
    double YMinSliderValue  = Output_Controls_ptr->getYMinSliderVal();
    double BrightnessFactor = Output_Controls_ptr->getOutputBrightnessFactor();
    double ScaleVal  = convertUnitsStringToValue(ScaleStr);
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomassEnsemble;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomassEnsembleAve;
    std::vector<boost::numeric::ublas::matrix<double> > OutputBiomassByGuilds;
    boost::numeric::ublas::matrix<double> ObservedBiomass;
    QString aveAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm();
    QStringList SpeciesList;
    QStringList GuildsList;
    QString Species   = Output_Controls_ptr->getOutputSpecies();
    QString GroupType = Output_Controls_ptr->getOutputGroupType();
    int SpeciesNum    = Output_Controls_ptr->getSpeciesNumFromName(Species);
    bool showHistoricalData = Output_Controls_ptr->isHistoricalDataShown();
    boost::numeric::ublas::matrix<double> AveBiomass;
    QList<QString> lineLabels;
    std::vector<std::string> Algorithms = {};
    std::vector<std::string> Minimizers = {};
    std::vector<std::string> ObjectiveCriteria = {};
    std::vector<std::string> Scalings = {};
    QList<double> BMSYValuesAveraged;
    QList<double> MSYValues;
    QList<double> FMSYValues;

    if (! getOutputBiomassEnsembleLineLabels(lineLabels)) {
        return;
    }

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return;
    }
    if (! m_DatabasePtr->getGuilds(m_Logger,NumGuilds,GuildsList)) {
        return;
    }

    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return;
    }

    // The following may happen if user runs, quits the app, restarts and then wants
    // to view their last run (which had to have been a multi-run for them to be here)
    if (isAMohnsRhoMultiRun()) {
        getOutputBiomassEnsemble(RunLength+1,NumSpecies,m_OutputBiomassEnsemble);
    } else {
        if (m_AveBiomass.size1() == 0) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            if (! getOutputBiomassAveraged(m_AveBiomass)) {
                QApplication::restoreOverrideCursor();
                return;
            }
            getOutputBiomassEnsemble(m_AveBiomass.size1(),m_AveBiomass.size2(),m_OutputBiomassEnsemble);
            QApplication::restoreOverrideCursor();
        }
    }

    // Group-specific code here (i.e., Species, Guild, System)
    if (GroupType == "Species") {
        std::string ObsBiomassTableName = getObservedBiomassTableName(!nmfConstantsMSSPM::PreEstimation);
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",ObsBiomassTableName,
                                               NumSpecies,RunLength,ObservedBiomass)) {
            return;
        }
        OutputBiomassEnsemble = m_OutputBiomassEnsemble;
    } else {
        ObservedBiomass       = getObservedBiomassByGroup(NumGuilds,RunLength,GroupType.toStdString());
        OutputBiomassByGuilds = getOutputBiomassByGroup(RunLength,m_OutputBiomassEnsemble,GroupType.toStdString());
        OutputBiomassEnsemble = OutputBiomassByGuilds;
    }

    if (GroupType == "Guild") {
        NumSpecies = NumGuilds;
    } else if (GroupType == "System") {
        NumSpecies = 1;
        SpeciesNum = 0;
        OutputSpecies = "System";
    }

    if (! isAMohnsRhoMultiRun()) {
        getMSYData(true,1,NumSpecies,GroupType.toStdString(),
                   BMSYValuesAveraged,MSYValues,FMSYValues);
    }

    // Load some vectors with empty strings that aren't currently used
    for (int line=0; line<int(OutputBiomassEnsemble.size()); ++line) {
        Algorithms.push_back("");
        Minimizers.push_back("");
        ObjectiveCriteria.push_back("");
        Scalings.push_back("");
    }

    // Show scatter plot with light gray individual run lines
    showChartBiomassVsTimeMultiRunWithScatter(
                NumSpecies,OutputSpecies,
                SpeciesNum,RunLength,
                InitialYear,
                Algorithms,
                Minimizers,
                ObjectiveCriteria,
                Scalings,
                OutputBiomassEnsemble,
                ObservedBiomass,
                BMSYValuesAveraged,
                ScaleStr,ScaleVal,
                nmfConstantsMSSPM::Clear,
                YMinSliderValue,
                lineLabels);

    //std::cout << "isMohnsRho: " << isAMohnsRhoMultiRun() << std::endl;
    if (! isAMohnsRhoMultiRun()) {
        // Show dark blue average line(s)
        OutputBiomassEnsembleAve.clear();
        OutputBiomassEnsembleAve.push_back(m_AveBiomass);
        ColumnLabelsForLegend.clear();
        ColumnLabelsForLegend << aveAlgorithm + " Average";
        if (GroupType != "Species") {
            OutputBiomassByGuilds = getOutputBiomassByGroup(RunLength,OutputBiomassEnsembleAve,GroupType.toStdString());
            OutputBiomassEnsembleAve = OutputBiomassByGuilds;
        }

        showBiomassVsTimeForMultipleRuns("Ensemble Biomass",
                                         "Year",
                                         "Ensemble Biomass",
                                         showHistoricalData,
                                         InitialYear,ObservedBiomass.size1(),
                                         NumSpecies,OutputSpecies,
                                         SpeciesNum,RunLength+1,
                                         OutputBiomassEnsembleAve,
                                         ScaleStr,ScaleVal,
                                         YMinSliderValue,BrightnessFactor,
                                         nmfConstantsMSSPM::IsNotMonteCarlo,
                                         nmfConstantsMSSPM::IsNotEnsemble,
                                         nmfConstantsMSSPM::DontClear,
                                         ColumnLabelsForLegend);
    }

}


bool
nmfMainWindow::getOutputBiomassAveraged(
        boost::numeric::ublas::matrix<double>& AveBiomass)
{
    int m=0;
    int NumSpecies;
    int RunLength;
    int InitialYear;
    bool isBiomassAbsolute;
    std::string msg;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string AveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    QStringList SpeciesList;

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        return false;
    }
    ++RunLength; // Needed as it's 1 less than what's needed here

    fields   = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Year","Value"};
    queryStr = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year,Value FROM " +
                nmfConstantsMSSPM::TableOutputBiomass +
               " WHERE ProjectName = '" +       m_ProjectName +
               "' AND ModelName = '" +          m_ModelName +
               "' AND Algorithm = '" +          AveragingAlgorithm +
               "' AND Minimizer = '" +          AveragingAlgorithm +
               "' AND ObjectiveCriterion = '" + AveragingAlgorithm +
               "' AND Scaling = '" +            AveragingAlgorithm + "' ORDER BY SpeName,Year";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["Value"].size();

    if (NumRecords == 0) {
        msg = "Info: No multi-run records found.";
        m_Logger->logMsg(nmfConstants::Normal,msg);
        return false;
    } else if (NumRecords != RunLength*NumSpecies) {
        msg = "getOutputBiomassAveraged: NumRecords (" + std::to_string(NumRecords);
        msg += ") is not equal to RunLength (" + std::to_string(RunLength) + ") * NumSpecies (";
        msg += std::to_string(NumSpecies) + ")";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    nmfUtils::initialize(AveBiomass,RunLength,NumSpecies);
    for (int species=0; species<NumSpecies; ++species) {
        for (int time=0; time<RunLength; ++time) {
            AveBiomass(time,species) = std::stod(dataMap["Value"][m++]);
        }
    }
    return true;
}

void
nmfMainWindow::getOutputBiomassEnsemble(
        const int& NumYears,
        const int& NumSpecies,
        std::vector<boost::numeric::ublas::matrix<double> >& OutputBiomassEnsemble)
{
    int m=0;
    int NumValuesPerRun = NumYears*NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    boost::numeric::ublas::matrix<double> aMatrix;

    fields     = {"ProjectName","ModelName","RunNumber","SpeName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,RunNumber,SpeName,Year,Value FROM " +
                  nmfConstantsMSSPM::TableOutputBiomassEnsemble +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' ORDER BY RunNumber,SpeName,Year";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["Value"].size();
    int NumRuns = NumRecords / NumValuesPerRun;
    OutputBiomassEnsemble.clear();
    nmfUtils::initialize(aMatrix,NumYears,NumSpecies);
    for (int i=0; i<NumRuns; ++i) {
        aMatrix.clear();
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<NumYears; ++time) {
//              aMatrix(time,species) = std::stod(dataMap["Value"][m++]);
                aMatrix(time,species) = QString::fromStdString(dataMap["Value"][m++]).toDouble();
            }
        }
        OutputBiomassEnsemble.push_back(aMatrix);
    }
}

void
nmfMainWindow::updateOutputBiomassDatabaseTableWithAverageBiomass(boost::numeric::ublas::matrix<double>& AveragedBiomass)
{
std::cout << "updateOutputBiomassTableWithAverageBiomass " << std::endl;

    int NumSpecies;
    int RunLength = AveragedBiomass.size1();
    std::string cmd;
    std::string errorMsg;
    std::string DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
    std::string Algorithm          = DefaultAveragingAlgorithm;
    std::string Minimizer          = DefaultAveragingAlgorithm;
    std::string ObjectiveCriterion = DefaultAveragingAlgorithm;
    std::string Scaling            = DefaultAveragingAlgorithm;
    QStringList SpeciesList;
    std::string isAggProd = "0";

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;

    // Clear OutputBiomass table with same Algorithm, Minimizer, ObjectiveCriterion, and Scaling
    // and then load OutputBiomass table
    cmd = "DELETE FROM " + nmfConstantsMSSPM::TableOutputBiomass +
          " WHERE ProjectName = '" +       m_ProjectName +
          "' AND ModelName = '" +          m_ModelName +
          "' AND Algorithm = '" +          Algorithm +
          "' AND Minimizer = '" +          Minimizer +
          "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
          "' AND Scaling = '" + Scaling + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] updateOutputBiomassTableWithAverageBiomass: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }

    cmd = "INSERT INTO " + nmfConstantsMSSPM::TableOutputBiomass +
          " (isAggProd,ProjectName,ModelName,Algorithm, Minimizer,ObjectiveCriterion,Scaling,SpeName,Year,Value) VALUES ";
    for (int species=0; species<NumSpecies; ++species) { // Species
        for (int time=0; time<RunLength; ++time) { // Time in years
            cmd += "("  + isAggProd +
                    ",'"  + m_ProjectName +
                    "','" + m_ModelName +
                    "','" + Algorithm +
                    "','" + Minimizer +
                    "','" + ObjectiveCriterion +
                    "','" + Scaling +
                    "','" + SpeciesList[species].toStdString() +
                    "',"  + std::to_string(time) +
                    ","   + QString::number(AveragedBiomass(time,species),'f',nmfConstantsMSSPM::Output_Precision).toStdString() + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] updateOutputBiomassTableWithAverageBiomass: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }
}


void
nmfMainWindow::updateOutputBiomassOutputTableWithAveragedBiomass(
        boost::numeric::ublas::matrix<double>& AveragedBiomass)
{
std::cout << "updateOutputBiomassOutputTableWithAveragedBiomass " << std::endl;

    int NumSpecies;
    int StartYear;
    int RunLength = AveragedBiomass.size1();
    QStringList SpeciesList;
    QStringList YearLabels;
    QString speciesName;
    QString valueWithComma;
    QStandardItem* item;
    QStandardItemModel* smodel;

    getInitialYear(StartYear,RunLength);

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList))
        return;

    smodel = new QStandardItemModel(RunLength, NumSpecies);
    for (int i=0; i<NumSpecies; ++i) {
        for (int time=0; time<=RunLength; ++time) {
            if (i == 0) {
                YearLabels << QString::number(StartYear+time);
            }
            valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                        AveragedBiomass(time,i),m_NumSignificantDigits,6); // 3);
            item = new QStandardItem(valueWithComma);
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(time, i, item);
        }
    }
    smodel->setVerticalHeaderLabels(YearLabels);
    smodel->setHorizontalHeaderLabels(SpeciesList);
    OutputBiomassTV->setModel(smodel);
}

void
nmfMainWindow::updateRunSummaryPage(
        QString convMsg,
        QString summaryMsg)
{
    // General Results Output to Results window
    QString msg = "<p style=\"text-align:center;\"><strong>RUN SUMMARY</strong></p>";
    msg += "<p style=\"font-family:\'Courier New\'\">";

    // Show convergence status
    if (! convMsg.isEmpty()) {
        msg += "<br><strong>Convergence:</strong>&nbsp;" + convMsg + "<br>";
    }

    // Show model forms
    msg += "<br><strong>Growth Form:</strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString::fromStdString(m_DataStruct.GrowthForm);
    msg += "<br><strong>Harvest Form:</strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString::fromStdString(m_DataStruct.HarvestForm);
    msg += "<br><strong>Competition Form:</strong>&nbsp;&nbsp;" + QString::fromStdString(m_DataStruct.CompetitionForm);
    msg += "<br><strong>Predation Form:</strong>&nbsp;&nbsp;&nbsp;&nbsp;" + QString::fromStdString(m_DataStruct.PredationForm);
    if (m_DataStruct.ScalingAlgorithm.empty()) {
        msg += "<br><strong>Scaling Algorithm:</strong>&nbsp;----";
    } else {
        msg += "<br><strong>Scaling Algorithm:</strong>&nbsp;" + QString::fromStdString(m_DataStruct.ScalingAlgorithm);
    }
    msg += "<br><br>" + summaryMsg;
    msg += "</p>";
    Estimation_Tab7_ptr->setOutputTE("");
    Estimation_Tab7_ptr->appendOutputTE(msg);
    m_RunOutputMsg = msg;

    if (m_ProgressWidget->wasStopped()) {
        QString elapsedTime = QString::fromStdString(
                    "Elapsed Time: " + m_ProgressWidget->getElapsedTime());
        Estimation_Tab7_ptr->appendOutputTE(elapsedTime);
    }
}

void
nmfMainWindow::createOutputStrMultiRun(
        const std::string& elapsedTime,
        std::string& summaryMsg)
{
    std::string growthForm      = m_DataStruct.GrowthForm;
    std::string harvestForm     = m_DataStruct.HarvestForm;
    std::string competitionForm = m_DataStruct.CompetitionForm;
    std::string predationForm   = m_DataStruct.PredationForm;
    summaryMsg.clear();

//    std::cout << "Total Parameters: " << m_DataStruct.TotalNumberParameters << std::endl;
//    summaryMsg += "Total Parameters:&nbsp;&nbsp;&nbsp;" + std::to_string(m_DataStruct.TotalNumberParameters);

    summaryMsg += "<br><strong>Estimated Parameter Count:</strong>";
    summaryMsg += "<br>Est'd Parameters:&nbsp;"     + std::to_string(m_DataStruct.TotalNumberEstimatedParameters);
    summaryMsg += "<br>Est'd Parameters (non-trivial): " + std::to_string(m_DataStruct.TotalNumberEstimatedParametersNonTrivial);
    summaryMsg += "<br>";
    for (unsigned i=0; i<m_DataStruct.TotalNumberEstimatedParametersBySpecies.size(); ++i) {
        summaryMsg += "  " + std::to_string(m_DataStruct.TotalNumberEstimatedParametersBySpecies[i]);
    }

std::cout << "***** TBD: Check to see if NLopt run..." << std::endl;
    summaryMsg += "<br><br><strong>Number of Runs:</strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + std::to_string(m_DataStruct.NLoptNumberOfRuns);

    QList<double> outputInitialBiomass;
    QList<double> outputGrowthRates;
    QList<double> outputGrowthRateShapes;
    QList<double> outputGrowthRateCovariateCoeffs;
    QList<double> outputCarryingCapacity;
    QList<double> outputCarryingCapacityCovariateCoeffs;
    QList<double> outputCatchability;
    QList<double> outputCatchabilityCovariateCoeffs;
    QList<double> outputPredationExponent;
    QList<double> outputSurveyQ;
    QList<double> outputSurveyQCovariateCoeffs;
    boost::numeric::ublas::matrix<double>  outputCompetitionAlpha;
    boost::numeric::ublas::matrix<double>  outputCompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double>  outputCompetitionBetaGuilds;
    boost::numeric::ublas::matrix<double>  outputPredationRho;
    boost::numeric::ublas::matrix<double>  outputPredationHandling;

// RSK dup function getOutputParameterVector, combine the two similar functions - todo
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputInitBiomass,                    true,"",outputInitialBiomass);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputGrowthRate,                     true,"",outputGrowthRates);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputGrowthRateShape,                true,"",outputGrowthRateShapes);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs,      true,"",outputGrowthRateCovariateCoeffs);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputCarryingCapacity,               true,"",outputCarryingCapacity);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs,true,"",outputCarryingCapacityCovariateCoeffs);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputCatchability,                   true,"",outputCatchability);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs,    true,"",outputCatchabilityCovariateCoeffs);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputPredationExponent,              true,"",outputPredationExponent);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputSurveyQ,                        true,"",outputSurveyQ);
    getOutputParameterVector(nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs,         true,"",outputSurveyQCovariateCoeffs);
    getOutputParameterMatrix(nmfConstantsMSSPM::TableOutputCompetitionAlpha,               true,"",outputCompetitionAlpha);
    getOutputParameterMatrix(nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies,         true,"",outputCompetitionBetaSpecies);
    getOutputParameterMatrix(nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds,          true,"",outputCompetitionBetaGuilds);
    getOutputParameterMatrix(nmfConstantsMSSPM::TableOutputPredationRho,                   true,"",outputPredationRho);
    getOutputParameterMatrix(nmfConstantsMSSPM::TableOutputPredationHandling,              true,"",outputPredationHandling);

    // Section heading
    summaryMsg += "<br><br><strong>Estimated Parameters:</strong>";

    // Growth parameter(s)
    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Initial Absolute Biomass:    ",  outputInitialBiomass,            false);
    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Growth Rate:          ",         outputGrowthRates,               false);
    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Growth Rate Shape Parameters: ", outputGrowthRateShapes,          false);
    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Growth Rate Covariate Coeffs: ", outputGrowthRateCovariateCoeffs, false);
    if (growthForm == "Logistic") {
        summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Carrying Capacity:  ",                  outputCarryingCapacity,               false);
        summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Carrying Capacity Covariate Coeffs:  ", outputCarryingCapacityCovariateCoeffs,false);
    }

    // Harvest parameter(s)
    if ((harvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
        (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString())) {
        summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Catchability:       ",             outputCatchability,               false);
        summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Catchability Covariate Coeffs:  ", outputCatchabilityCovariateCoeffs,false);
    }

    // Competition parameter(s)
    if (competitionForm == "NO_K") {
        summaryMsg += nmfUtils::convertValues2DToOutputStr("<br>Competition (alpha):",             outputCompetitionAlpha);
    } else if ((competitionForm == "MS-PROD") ||
               (competitionForm == "AGG-PROD")) {
        if (competitionForm == "MS-PROD") {
            summaryMsg += nmfUtils::convertValues2DToOutputStr("<br>Competition (beta::species):", outputCompetitionBetaSpecies);
        }
        summaryMsg += nmfUtils::convertValues2DToOutputStr("<br>Competition (beta::guilds): ",     outputCompetitionBetaGuilds);
    }

    // Predation parameter(s)
    if ((predationForm == "Type I")  || (predationForm == "Type II") || (predationForm == "Type III")) {
        summaryMsg += nmfUtils::convertValues2DToOutputStr("<br>Predation (rho):   ",outputPredationRho);
    }
    if ((predationForm == "Type II") || (predationForm == "Type III")) {
        summaryMsg += nmfUtils::convertValues2DToOutputStr("<br>Predation Handling:", outputPredationHandling);
    }
    if (predationForm == "Type III") {
        summaryMsg += "<br>&nbsp;&nbsp;";
        summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>Predation Exponent", outputPredationExponent,false);
    }

    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>SurveyQ:",                  outputSurveyQ,                false);
    summaryMsg += nmfUtils::convertValues1DToOutputStr("<br>SurveyQ Covariate Coeffs:", outputSurveyQCovariateCoeffs, false);

    summaryMsg += "<br><br><strong>" + elapsedTime + "</strong>";

}

void
nmfMainWindow::callback_RunCompleted(std::string summaryMsg,
                                     std::string convergenceMsg,
                                     bool showDiagnosticChart)
{
    std::string runName = "";
    std::string stopRunFile = nmfConstantsMSSPM::MSSPMStopRunFile;
    std::string state = "";
    std::string msg1,msg2;
    QString convMsg = QString::fromStdString(convergenceMsg);
    convMsg  = convMsg.replace(".",". ").replace("!","! ");
    convMsg += "<br><strong>Tolerance:</strong>&nbsp;&nbsp;&nbsp;" + QString::number(Estimation_Tab7_ptr->getCurrentToleranceStopValue(),'g',3);

    m_Logger->logMsg(nmfConstants::Normal,"callback_RunCompleted");

    m_isRunning = false;

    nmfUtils::isStopped(runName,msg1,msg2,stopRunFile,state);

    m_Logger->logMsg(nmfConstants::Normal,"==>> Run Completed");

    Output_Controls_ptr->setOutputType(nmfConstantsMSSPM::OutputChartBiomass);

    updateRunSummaryPage(convMsg,QString::fromStdString("<html>"+summaryMsg+"</html>"));

    menu_saveAndShowCurrentRun(showDiagnosticChart);
    callback_UpdateSummaryStatistics(); //RSKRSK
    m_ProgressWidget->showLegend();

    Estimation_Tab1_ptr->checkIfRunFromModifySlider();

//    if (m_ProgressWidget->wasStopped()) {
//        QString elapsedTime = QString::fromStdString(
//                    "Elapsed Time: " + m_ProgressWidget->getElapsedTime());
//        Estimation_Tab7_ptr->appendOutputTE(elapsedTime);
//    }

    // Update time to include table writing and chart drawing. This is required since
    // estimating parameters without ranges will finish instantly before the elapsed time
    // can be updated.
    m_ProgressWidget->updateTime();

    Diagnostic_Tab2_ptr->setMohnsRhoForSingleRun(false);

    enableRunWidgets(true);
    Estimation_Tab7_ptr->enableAddToReview(true);

    // Hack to make sure plot is visible if run after a failed Forecast attempt
    Output_Controls_ptr->refreshChart();
    Estimation_Tab7_ptr->callback_ReloadWidgets();
}

void
nmfMainWindow::callback_ShowRunMessage(QString fontName)
{
    QFont font(fontName,11,QFont::Medium,false);

    Estimation_Tab7_ptr->refreshMsg(font,m_RunOutputMsg);
}

void
nmfMainWindow::callback_StoreOutputSpecies()
{
    QString species = Output_Controls_ptr->getOutputSpecies();
    Estimation_Tab1_ptr->setOutputSpecies(species);
}

void
nmfMainWindow::callback_RepetitionRunCompleted(int RunNumber,
                                               int SubRunNum,
                                               int NumSubRuns)
{
    m_ProgressWidget->setRunBoxes(RunNumber,SubRunNum,NumSubRuns);
}

void
nmfMainWindow::callback_ErrorFound(std::string errorMsg)
{
    QMessageBox::critical(this, "Error",
                          QString::fromStdString(errorMsg),
                          QMessageBox::Ok);
    QApplication::restoreOverrideCursor();

}

void
nmfMainWindow::callback_RunRetrospectiveAnalysisEstimation(
        std::vector<std::pair<int,int> > MohnsRhoRanges)
{
    // Ensure that the user is not running a Multi-Run
//  Estimation_Tab7_ptr->enableMultiRunControls(false);

    // 1. Create Mohns Rhos multi-run filename
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));

    // 2. Create the multi-run file
    int numRunsToAdd             = MohnsRhoRanges.size();
    int currentNumberOfRuns      = 0;
    int totalNumberOfRunsDesired = numRunsToAdd;
    Estimation_Tab7_ptr->clearMohnsRhoFile();
//  Estimation_Tab7_ptr->setMohnsRhoFileType("SingleRun");
//  Estimation_Tab7_ptr->setMohnsRhoFileHeader();
    Estimation_Tab7_ptr->addToMultiRunFile(numRunsToAdd,currentNumberOfRuns,
                                           totalNumberOfRunsDesired,fullPath);

    // 3. Run the Mohns Rho runs
    Estimation_Tab7_ptr->runEstimation();
}

void
nmfMainWindow::callback_RunRetrospectiveAnalysisEstimationMultiRun(
        std::vector<std::pair<int,int> > MohnsRhoRanges)
{
    // 1. Create Mohns Rhos multi-run filename
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));

   // 1. Run n averaged runs and write average biomass to output filename
    int currentNumberOfRuns = 0;
    int numOfMultiRuns      = MohnsRhoRanges.size();
    int numRunsPerMultiRun  = Estimation_Tab7_ptr->getEnsembleNumberOfTotalRuns();
    int totalNumberOfRunsDesired = numOfMultiRuns*numRunsPerMultiRun;

    // 2.

    Diagnostic_Tab2_ptr->setMohnsRhoForSingleRun(false);

    Estimation_Tab7_ptr->clearMohnsRhoFile();
//  Estimation_Tab7_ptr->setMohnsRhoFileType("MultiRun");
//  Estimation_Tab7_ptr->setMohnsRhoFileHeader();
    for (int i=0; i<numOfMultiRuns; ++i) {
        currentNumberOfRuns = i*numRunsPerMultiRun;
        Estimation_Tab7_ptr->addToMultiRunFile(numRunsPerMultiRun,
                                               currentNumberOfRuns,
                                               totalNumberOfRunsDesired,
                                               fullPath);
    }
//std::cout << "⬛⬛⬛ numOfMultiRuns: " << numOfMultiRuns << std::endl;
//std::cout << "⬛⬛⬛ fullPath: " << fullPath.toStdString() << std::endl;

    Estimation_Tab7_ptr->runEstimation();


}

void
nmfMainWindow::setCurrentOutputTab(QString outputTab)
{
    m_UI->MSSPMOutputTabWidget->setCurrentIndex(nmfUtilsQt::getTabIndex(m_UI->MSSPMOutputTabWidget,outputTab));
}

bool
nmfMainWindow::calculateSummaryStatisticsMohnsRhoBiomass(
        std::vector<double>& mohnsRhoEstimatedBiomass)
{
    int NumPeels = Diagnostic_Tab2_ptr->getNumPeels();
    int RunLength;
    int StartYear;
    int NumRecords;
    int totRecs;
    int NumSpecies;
    std::string isAggProd;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm;
    std::string msg;
    std::string queryStr;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    QStringList SpeciesList;
    std::vector<std::vector<double> > EstBiomass;

    mohnsRhoEstimatedBiomass.clear();

    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear)) {
        return false;
    }

    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList);

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    isAggProd = (CompetitionForm == "AGG-PROD") ? "1" : "0";

    fields    = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Year","Value"};
    queryStr  = "SELECT Projectname,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year,Value FROM " +
                 nmfConstantsMSSPM::TableOutputBiomassEnsemble;
    queryStr += " WHERE ProjectName = '"       + m_ProjectName +
                "' AND ModelName = '"          + m_ModelName +
                "' AND Algorithm = '"          + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + isAggProd;
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Algorithm"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"calculateSummaryStatisticsMohnsRhoBiomass: Found 0 records in OutputBiomassEnsemble query.");
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }

    // Check num records
    totRecs = NumPeels * NumSpecies * (RunLength+1);
    if (NumRecords != totRecs) {
        msg = "calculateSummaryStatisticsMohnsRhoBiomass: Read " + std::to_string(NumRecords) +
              " records but calculated " + std::to_string(totRecs) + " records.";
        m_Logger->logMsg(nmfConstants::Error,msg);
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }

    int m=0;
    std::vector<double> tmpVec;
    EstBiomass.clear();

    for (int peel=0; peel<NumPeels; ++peel) {
        tmpVec.clear();
        for (int j=0; j<NumSpecies; ++j) {
            for (int year=0; year<=RunLength; ++year) {
                tmpVec.push_back(std::stod(dataMap["Value"][m++]));
            }
        }
        EstBiomass.push_back(tmpVec);
    }

    std::vector<double> mohnsRhoValue;
    if (! nmfUtilsStatistics::calculateMohnsRhoForTimeSeries(
                NumPeels,NumSpecies,EstBiomass,mohnsRhoValue)) {
        return false;
    }

    // Display values
    for (unsigned p=0; p<mohnsRhoValue.size(); ++p) {
        mohnsRhoEstimatedBiomass.push_back(mohnsRhoValue[p]);
    }

    return true;
}


void
nmfMainWindow::callback_SetChartType(std::string type, std::string method)
{
   Output_Controls_ptr->setOutputType(QString::fromStdString(type));
   Output_Controls_ptr->setOutputDiagnostics(QString::fromStdString(method));
   setCurrentOutputTab("Chart");
}

//void
//nmfMainWindow::callback_Hovered(bool status, int index, QBarSet *barset)
//{
//    if (status) {
//        QString msg;
//        msg  = QString::number(barset->at(index)) + " parameters;";
//        msg += m_SpeciesTimeMap[index];
//        QToolTip::showText(QCursor::pos(),msg); //,this,QRect(0,0,100,20),2000); // this doesn't work...perhaps it's fixed in a later Qt version
//    } else {
//        QToolTip::hideText();
//    }
//}

//void
//nmfMainWindow::callback_UpdateProgressData(int SpeciesNum,
//                                           int NumParams,
//                                           QString elapsedTime)
//{
//    QBarSet *barSet;
//    QList<QAbstractSeries *> allSeries = m_ProgressChartBees->series();
//    m_ProgressSeries = qobject_cast<QBarSeries *>(allSeries[0]);
//    barSet = m_ProgressSeries->barSets()[0];
//    m_SpeciesTimeMap[SpeciesNum] = elapsedTime;
//    *barSet << NumParams;
//    m_ProgressChartBees->update();
//} // end outputProgressData


int
nmfMainWindow::getNumMohnsRhoMultiRuns()
{
    int numLines = 0;
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));
    QFile file(fullPath);
//std::cout << "⬛⬛⬛ fullPath: " << fullPath.toStdString() << std::endl;
    if (! file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "Error", file.errorString());
        return 0;
    }

    QString line;
    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
        ++numLines;
    }
    numLines -= 2; // -2 for the 2 header lines
//std::cout << "⬛⬛⬛ numLines: " << numLines << std::endl;
    file.close();

    return numLines;
}


bool
nmfMainWindow::loadCovariateRanges(
        std::map<std::string,nmfStructsQt::CovariateStruct>& growthRateCovariateRanges,
        std::map<std::string,nmfStructsQt::CovariateStruct>& carryingCapacityCovariateRanges,
        std::map<std::string,nmfStructsQt::CovariateStruct>& catchabilityCovariateRanges,
        std::map<std::string,nmfStructsQt::CovariateStruct>& surveyQCovariateRanges)
{
    int NumRecords;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName = nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges;
    std::string SpeName;
    std::string CoeffName;
    std::string CoeffValue;
    std::string CoeffMinValue;
    std::string CoeffMaxValue;
    QStringList SpeciesList;
    nmfStructsQt::CovariateStruct covariateRangeStruct;
    std::vector<nmfStructsQt::CovariateStruct> covariateRangeVector;

    growthRateCovariateRanges.clear();
    carryingCapacityCovariateRanges.clear();
    catchabilityCovariateRanges.clear();
    surveyQCovariateRanges.clear();

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }

    // Get data from database table
    fields     = {"ProjectName","ModelName","SpeName",
                  "CoeffName","CoeffMinName","CoeffMaxName",
                  "CoeffValue","CoeffMinValue","CoeffMaxValue"};
    queryStr   = "SELECT ProjectName,ModelName,SpeName,CoeffName,CoeffMinName,CoeffMaxName,CoeffValue,CoeffMinValue,CoeffMaxValue FROM " +
                  tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' ORDER BY SpeName ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["CoeffName"].size();
    if (NumRecords == 0) {
        return false;
    }

    bool foundSurveyQ = false;
    for (int i=0; i<NumRecords; ++i) {
        SpeName       = dataMap["SpeName"][i];
        CoeffName     = dataMap["CoeffName"][i];
        covariateRangeStruct.CoeffValue    = 0;
        covariateRangeStruct.CoeffMinValue = 0;
        covariateRangeStruct.CoeffMaxValue = 0;
        if (QStringList({"GrowthRate",
                         "CarryingCapacity",
                         "Catchability",
                         "SurveyQ"}).contains(QString::fromStdString(CoeffName))) {
            if (! dataMap["CoeffValue"][i].empty()) {
                CoeffValue    = dataMap["CoeffValue"][i];
                CoeffMinValue = dataMap["CoeffMinValue"][i];
                CoeffMaxValue = dataMap["CoeffMaxValue"][i];
                covariateRangeStruct.CoeffValue    = std::stod(CoeffValue);
                covariateRangeStruct.CoeffMinValue = std::stod(CoeffMinValue);
                covariateRangeStruct.CoeffMaxValue = std::stod(CoeffMaxValue);
            }
            if (CoeffName == "GrowthRate") {
                growthRateCovariateRanges[SpeName] = covariateRangeStruct;
//std::cout << "r cov range[" << SpeName << "]: " << covariateRangeStruct.CoeffValue << "," << covariateRangeStruct.CoeffMinValue << "," << covariateRangeStruct.CoeffMaxValue << std::endl;
            } else if (CoeffName == "CarryingCapacity") {
                carryingCapacityCovariateRanges[SpeName] = covariateRangeStruct;
//std::cout << "K cov range[" << SpeName << "]: " << covariateRangeStruct.CoeffValue << "," << covariateRangeStruct.CoeffMinValue << "," << covariateRangeStruct.CoeffMaxValue << std::endl;
            } else if (CoeffName == "Catchability") {
                catchabilityCovariateRanges[SpeName] = covariateRangeStruct;
//std::cout << "q cov range[" << SpeName << "]: " << covariateRangeStruct.CoeffValue << "," << covariateRangeStruct.CoeffMinValue << "," << covariateRangeStruct.CoeffMaxValue << std::endl;
            } else if (CoeffName == "SurveyQ") {
                foundSurveyQ = true;
                surveyQCovariateRanges[SpeName] = covariateRangeStruct;
//std::cout << "Q cov range[" << SpeName << "]: " << covariateRangeStruct.CoeffValue << "," << covariateRangeStruct.CoeffMinValue << "," << covariateRangeStruct.CoeffMaxValue << std::endl;
            }
        }
    }

    if (! foundSurveyQ) {
        covariateRangeStruct.CoeffValue    = 0;
        covariateRangeStruct.CoeffMinValue = 0;
        covariateRangeStruct.CoeffMaxValue = 0;
        surveyQCovariateRanges["SurveyQ"] = covariateRangeStruct;
//std::cout << "Q0 cov range[SurveyQ]: " << covariateRangeStruct.CoeffValue << "," << covariateRangeStruct.CoeffMinValue << "," << covariateRangeStruct.CoeffMaxValue << std::endl;

    }

    return true;
}

bool
nmfMainWindow::loadCovariateAssignment(std::map<std::string,std::string>& covariateAssignment)
{
    int m=0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string speciesName;
    std::string parameterName;
    std::string covariateName;

    fields     = {"SpeName","ParameterName","CovariateName"};
    queryStr   = "SELECT SpeName,ParameterName,CovariateName FROM " +
                  nmfConstantsMSSPM::TableCovariateAssignment +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = (int)dataMap["SpeName"].size();
    for (int i=0; i<NumRecords; ++i) {
        speciesName   = dataMap["SpeName"][m];
        parameterName = dataMap["ParameterName"][m];
        covariateName = dataMap["CovariateName"][m++];
        covariateAssignment[speciesName+","+parameterName] = covariateName;
//std::cout << "covariateAssignemtn[" << speciesName+","+parameterName <<
//           "] = >" << covariateName << "<" << std::endl;

    }

    return true;
}

bool
nmfMainWindow::loadCovariateMap(std::map<std::string,std::vector<double> >& covariateMap)
{
    int m=0;
    int RunLength = 0;
    int StartYear = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string covariateName;
    std::vector<double> covariateTimeSeries;
//std::cout << "Warning: Using original (i.e., unscaled) covariate time series data" << std::endl;

    getInitialYear(StartYear,RunLength);
    ++RunLength;

    fields     = {"CovariateName","Year","Value","ValueScaled"};
    queryStr   = "SELECT CovariateName,Year,Value,ValueScaled FROM " +
                  nmfConstantsMSSPM::TableCovariate +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumCovariateParameters = (int)dataMap["CovariateName"].size()/RunLength;
    covariateMap.clear();
    for (int i=0; i<NumCovariateParameters; ++i) {
        covariateName = dataMap["CovariateName"][m];
        if (! covariateName.empty()) {
            covariateTimeSeries.clear();
            for (int time=0;time<RunLength;++time) {
                if (dataMap["ValueScaled"][m] == "") {
                    covariateTimeSeries.push_back(0); // nmfConstantsMSSPM::NoData);
                } else {
                    covariateTimeSeries.push_back(std::stod(dataMap["ValueScaled"][m]));
//std::cout << "cov time series[" << time << "]: " << dataMap["ValueScaled"][m] << std::endl;
                }
                ++m;
            }
            covariateMap[covariateName] = covariateTimeSeries;
        }
    }
    return true;
}

void
nmfMainWindow::setUserDefinedSeedVariables(
        nmfStructsQt::ModelDataStruct& dataStruct)
{
    dataStruct.useUserFixedSeedNLopt = false;
    dataStruct.userFixedSeedVal = -1;
    if (Estimation_Tab7_ptr->isUserSeedValueEnabled()) {
        dataStruct.useUserFixedSeedNLopt = true;
        dataStruct.userFixedSeedVal = Estimation_Tab7_ptr->getUserSeedVal();
    }
    dataStruct.incrementFixedSeed = Estimation_Tab7_ptr->isSeedSetToIncrement();
}

bool
nmfMainWindow::loadParameters(nmfStructsQt::ModelDataStruct& dataStruct,
                              const bool& verbose)
{
    bool loadOK;
    bool isFitToCatch = false;
    int RunLength;
    int NumSpecies;
    int NumGuilds;
    int GuildNum;
    int NumCompetitionParameters = 0;
    int NumPredationParameters   = 0;
    int NumHandlingParameters    = 0;
    int NumExponentParameters    = 0;
    int NumBetaSpeciesParameters = 0;
    int NumBetaGuildsParameters  = 0;
    int NumSpeciesOrGuilds;
    std::string Algorithm,Minimizer,ObjectiveCriterion,Scaling;
    std::string GrowthForm,HarvestForm,CompetitionForm,PredationForm;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString msg;
    std::string growthForm;
    std::string harvestForm;
    std::string competitionForm;
    std::string predationForm;
    std::string obsBiomassType;
    std::string ForecastHarvestType = "";
    std::map<std::string,double> initialGuildBiomass;
    std::map<std::string,double> initialGuildBiomassMin;
    std::map<std::string,double> initialGuildBiomassMax;
    std::vector<std::string> Guild;
    std::map<std::string,int> GuildMap;
    std::map<std::string,std::string> GuildSpeciesMap;
    std::string guildName;
    std::map<std::string,std::vector<double> > covariateMap;
    QStringList SpeciesList;
    std::map<std::string,std::string> covariateAssignment;
    std::map<std::string,nmfStructsQt::CovariateStruct> growthRateCovariateRanges;
    std::map<std::string,nmfStructsQt::CovariateStruct> carryingCapacityCovariateRanges;
    std::map<std::string,nmfStructsQt::CovariateStruct> catchabilityCovariateRanges;
    std::map<std::string,nmfStructsQt::CovariateStruct> surveyQCovariateRanges;
    std::map<QString,QString> previousUnits;
    QString tableHarvestCatch    = QString::fromStdString(nmfConstantsMSSPM::TableHarvestCatch);
    QString tableSpecies         = QString::fromStdString(nmfConstantsMSSPM::TableSpecies);
    QString tableBiomassAbsolute = QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute);
    QString tableBiomassRelative = QString::fromStdString(nmfConstantsMSSPM::TableBiomassRelative);

    // Get covariate data for dataStruct
    loadCovariateMap(covariateMap);
    loadCovariateAssignment(covariateAssignment);
    loadCovariateRanges(growthRateCovariateRanges,
                        carryingCapacityCovariateRanges,
                        catchabilityCovariateRanges,
                        surveyQCovariateRanges);

    if (! m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList)) {
        return false;
    }
    dataStruct.SpeciesNames.clear();
    for (QString species : SpeciesList) {
        dataStruct.SpeciesNames.push_back(species.toStdString());
    }

//  dataStruct.NumMohnsRhoMultiRuns = getNumMohnsRhoMultiRuns();
    dataStruct.isMohnsRho = Diagnostic_Tab2_ptr->isAMohnsRhoRunForSingleRun();

    initialGuildBiomass.clear();
    initialGuildBiomassMin.clear();
    initialGuildBiomassMax.clear();
    dataStruct.GuildSpecies.clear();
    dataStruct.GuildNum.clear();
    dataStruct.ObservedBiomassBySpecies.clear();
    dataStruct.ObservedBiomassByGuilds.clear();
    dataStruct.Catch.clear();
    dataStruct.Effort.clear();
    dataStruct.GrowthRate.clear();
    dataStruct.GrowthRateMin.clear();
    dataStruct.GrowthRateMax.clear();
    dataStruct.GrowthRateShape.clear();
    dataStruct.GrowthRateShapeMin.clear();
    dataStruct.GrowthRateShapeMax.clear();
    dataStruct.MinimumBiomass.clear();
    dataStruct.InitBiomass.clear();
    dataStruct.InitBiomassMin.clear();
    dataStruct.InitBiomassMax.clear();
    dataStruct.CarryingCapacity.clear();
    dataStruct.CarryingCapacity.clear();
    dataStruct.CarryingCapacityMax.clear();
    dataStruct.CarryingCapacityMin.clear();
    dataStruct.Exploitation.clear();
    dataStruct.ExploitationRateMax.clear();
    dataStruct.ExploitationRateMin.clear();
    dataStruct.SurveyQ.clear();
    dataStruct.SurveyQMax.clear();
    dataStruct.SurveyQMin.clear();
    dataStruct.Catchability.clear();
    dataStruct.CatchabilityMax.clear();
    dataStruct.CatchabilityMin.clear();
    dataStruct.Competition.clear();
    dataStruct.CompetitionMin.clear();
    dataStruct.CompetitionMax.clear();
    dataStruct.CompetitionBetaSpecies.clear();
    dataStruct.CompetitionBetaSpeciesMin.clear();
    dataStruct.CompetitionBetaSpeciesMax.clear();
    dataStruct.CompetitionBetaGuilds.clear();
    dataStruct.CompetitionBetaGuildsMin.clear();
    dataStruct.CompetitionBetaGuildsMax.clear();
    dataStruct.PredationRho.clear();
    dataStruct.PredationRhoMin.clear();
    dataStruct.PredationRhoMax.clear();
    dataStruct.PredationHandling.clear();
    dataStruct.PredationHandlingMin.clear();
    dataStruct.PredationHandlingMax.clear();
    dataStruct.PredationExponent.clear();
    dataStruct.PredationExponentMin.clear();
    dataStruct.PredationExponentMax.clear();
//  dataStruct.OutputBiomass.clear();
    dataStruct.MinimizerAlgorithm.clear();
    dataStruct.ObjectiveCriterion.clear();
    dataStruct.ScalingAlgorithm.clear();
    dataStruct.EstimateRunBoxes.clear();
    dataStruct.SpeciesWeights.clear();
    dataStruct.CovariateMap                   = covariateMap;
    dataStruct.CovariateAssignment            = covariateAssignment;
    dataStruct.GrowthRateCovariateCoeff       = growthRateCovariateRanges;
    dataStruct.CarryingCapacityCovariateCoeff = carryingCapacityCovariateRanges;
    dataStruct.CatchabilityCovariateCoeff     = catchabilityCovariateRanges;
    dataStruct.SurveyQCovariateCoeff          = surveyQCovariateRanges;

    dataStruct.useApplicationFixedSeedBees    = Estimation_Tab7_ptr->isSetToDeterministicBees();
    dataStruct.EstimationAlgorithm    = Estimation_Tab7_ptr->getCurrentAlgorithm();
    dataStruct.ObjectiveCriterion     = Estimation_Tab7_ptr->getCurrentObjectiveCriterion();
    dataStruct.MinimizerAlgorithm     = Estimation_Tab7_ptr->getCurrentMinimizer();
    dataStruct.ScalingAlgorithm       = Estimation_Tab7_ptr->getCurrentScaling();
    dataStruct.CovariateAlgorithmType = m_DatabasePtr->getCovariateAlgorithmType(m_Logger,m_ProjectName,m_ModelName);
    dataStruct.ParameterLogScale      = Estimation_Tab7_ptr->getLogScale();
    dataStruct.allowConvergedOnly     = Estimation_Tab7_ptr->isAllowConvergedOnly();

    dataStruct.useRandomInitialParameters = Estimation_Tab7_ptr->isEnsembleRandInitParam();
    dataStruct.rangeJitter                = Estimation_Tab7_ptr->getEnsembleRangeJitterValue();
    dataStruct.isRangeJitterRepeatable    = Estimation_Tab7_ptr->isEnsembleRangeJitterRepeatable();

    m_DatabasePtr->createUnitsMap(m_ProjectName,m_ModelName,previousUnits);
    dataStruct.PreviousUnits = previousUnits;

    // Set user defined seed variables
    setUserDefinedSeedVariables(dataStruct);

    // Set the MultiRun Setup output file that will contain all of the
    // MultiRun Run definitions
    QString fullPath     = "";
    QString basePath     = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString multiRunFile = (Diagnostic_Tab2_ptr->isAMohnsRhoRunForSingleRun()) ?
                QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun) :
                QString::fromStdString(Estimation_Tab7_ptr->getEnsembleTimeStampedFilename());
    fullPath = QDir(basePath).filePath(multiRunFile);
//    if (dataStruct.NumMohnsRhoMultiRuns > 0) {
//        fullPath = QDir(basePath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));
//    }

    dataStruct.MultiRunSetupFilename = fullPath.toStdString();
    dataStruct.EstimateRunBoxes = Estimation_Tab7_ptr->getEstimateRunBoxes();

    // Find all algorithm parameters
    fields     = {"ObsBiomassType","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm",
                  "RunLength","Minimizer","ObjectiveCriterion",
                  "BeesNumTotal","BeesNumElite","BeesNumOther","BeesNumEliteSites",
                  "BeesNumBestSites","BeesNumRepetitions","BeesMaxGenerations","BeesNeighborhoodSize",
                  "Scaling","GAGenerations","GAConvergence",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter","NLoptUseInitialPopulationSize",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter","NLoptInitialPopulationSize","NLoptNumberOfRuns"};
    queryStr   = "SELECT ObsBiomassType,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm,RunLength,Minimizer,ObjectiveCriterion,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,Scaling,GAGenerations,GAConvergence,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,NLoptUseInitialPopulationSize,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter,NLoptInitialPopulationSize,NLoptNumberOfRuns FROM " +
                  nmfConstantsMSSPM::TableModels +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);

    obsBiomassType                   = dataMap["ObsBiomassType"][0];
    RunLength                        = std::stoi(dataMap["RunLength"][0]);
    dataStruct.RunLength             = RunLength;
    dataStruct.GrowthForm            = dataMap["GrowthForm"][0];
    dataStruct.HarvestForm           = dataMap["HarvestForm"][0];
    dataStruct.CompetitionForm       = dataMap["WithinGuildCompetitionForm"][0];
    dataStruct.PredationForm         = dataMap["PredationForm"][0];
    dataStruct.BeesNumTotal          = std::stoi(dataMap["BeesNumTotal"][0]);
    dataStruct.BeesNumElite          = std::stoi(dataMap["BeesNumElite"][0]);
    dataStruct.BeesNumOther          = std::stoi(dataMap["BeesNumOther"][0]);
    dataStruct.BeesNumEliteSites     = std::stoi(dataMap["BeesNumEliteSites"][0]);
    dataStruct.BeesNumBestSites      = std::stoi(dataMap["BeesNumBestSites"][0]);
    dataStruct.BeesNumRepetitions    = std::stoi(dataMap["BeesNumRepetitions"][0]);
    dataStruct.BeesMaxGenerations    = std::stoi(dataMap["BeesMaxGenerations"][0]);
    dataStruct.BeesNeighborhoodSize  = std::stof(dataMap["BeesNeighborhoodSize"][0]);
    dataStruct.ScalingAlgorithm      = dataMap["Scaling"][0];
    dataStruct.GAGenerations         = std::stoi(dataMap["GAGenerations"][0]);
    dataStruct.GAConvergence         = std::stoi(dataMap["GAConvergence"][0]);
    dataStruct.MinimizerAlgorithm    = dataMap["Minimizer"][0];
    dataStruct.ObjectiveCriterion    = dataMap["ObjectiveCriterion"][0];
    dataStruct.NLoptUseStopVal       = std::stoi(dataMap["NLoptUseStopVal"][0]);
    dataStruct.NLoptUseStopAfterTime = std::stoi(dataMap["NLoptUseStopAfterTime"][0]);
    dataStruct.NLoptUseStopAfterIter = std::stoi(dataMap["NLoptUseStopAfterIter"][0]);
    dataStruct.NLoptStopVal          = std::stod(dataMap["NLoptStopVal"][0]);
    dataStruct.NLoptStopAfterTime    = std::stoi(dataMap["NLoptStopAfterTime"][0]);
    dataStruct.NLoptStopAfterIter    = std::stoi(dataMap["NLoptStopAfterIter"][0]);
    dataStruct.NLoptNumberOfRuns     = std::stoi(dataMap["NLoptNumberOfRuns"][0]);
    dataStruct.NLoptUseInitialPopulationSize = std::stoi(dataMap["NLoptUseInitialPopulationSize"][0]);
    dataStruct.NLoptInitialPopulationSize    = std::stoi(dataMap["NLoptInitialPopulationSize"][0]);

    growthForm      = dataStruct.GrowthForm;
    harvestForm     = dataStruct.HarvestForm;
    competitionForm = dataStruct.CompetitionForm;
    predationForm   = dataStruct.PredationForm;
    isFitToCatch    = (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString());


    if (growthForm == "Null") {
        msg = "\nPlease enter a non-null growth form.\n";
        QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
        return false;
    }

    bool isAlpha    = (competitionForm == "NO_K");
    bool isRho      = (predationForm   != "Null");
    bool isHandling = (predationForm   == "Type II") || (predationForm == "Type III");
    bool isExponent = (predationForm   == "Type III");
    bool isMSPROD   = (competitionForm == "MS-PROD");
    bool isAGGPROD  = (competitionForm == "AGG-PROD");
    bool isSurveyQ  = (obsBiomassType  == "Relative");
    dataStruct.isRelativeBiomass = isSurveyQ;

    if (isAlpha) {
        dataStruct.Competition.clear();
        dataStruct.CompetitionMin.clear();
        dataStruct.CompetitionMax.clear();
    }
    if (isRho) {
        dataStruct.PredationRho.clear();
        dataStruct.PredationRhoMin.clear();
        dataStruct.PredationRhoMax.clear();
    }
    if (isHandling) {
        dataStruct.PredationHandling.clear();
        dataStruct.PredationHandlingMin.clear();
        dataStruct.PredationHandlingMax.clear();
    }
    if (isExponent) {
        dataStruct.PredationExponent.clear();
        dataStruct.PredationExponentMin.clear();
        dataStruct.PredationExponentMax.clear();
    }
    if (isMSPROD) {
        dataStruct.CompetitionBetaSpecies.clear();
        dataStruct.CompetitionBetaSpeciesMin.clear();
        dataStruct.CompetitionBetaSpeciesMax.clear();
        dataStruct.CompetitionBetaGuilds.clear();
        dataStruct.CompetitionBetaGuildsMin.clear();
        dataStruct.CompetitionBetaGuildsMax.clear();
    } else if (isAGGPROD) {
        dataStruct.CompetitionBetaGuildsGuilds.clear();
        dataStruct.CompetitionBetaGuildsGuildsMin.clear();
        dataStruct.CompetitionBetaGuildsGuildsMax.clear();
    }

    // Get Guild information
    fields    = {"GuildName","GuildK"};
    queryStr  = "SELECT GuildName,GuildK from " + nmfConstantsMSSPM::TableGuilds + " ORDER by GuildName";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    dataStruct.NumGuilds = NumGuilds;
    for (int i=0; i<NumGuilds; ++i) {
        guildName = dataMap["GuildName"][i];
        Guild.push_back(guildName);
        GuildMap[guildName] = i;
    }

    if (isAGGPROD) {
        fields     = {"GuildName","GrowthRateMin","GrowthRateMax","GuildK","GuildKMin",
                      "GuildKMax","CatchabilityMin","CatchabilityMax"};
        queryStr   = "SELECT GuildName,GrowthRateMin,GrowthRateMax,GuildK,GuildKMin,";
        queryStr  += "GuildKMax,CatchabilityMin,CatchabilityMax from " +
                      nmfConstantsMSSPM::TableGuilds +
                     " ORDER BY GuildName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumGuilds  = dataMap["GuildName"].size();
        nmfUtils::initialize(dataStruct.InitBiomass,        NumGuilds);
        nmfUtils::initialize(dataStruct.InitBiomassMin,     NumGuilds);
        nmfUtils::initialize(dataStruct.InitBiomassMax,     NumGuilds);
        nmfUtils::initialize(dataStruct.GrowthRateMin,      NumGuilds);
        nmfUtils::initialize(dataStruct.GrowthRateMax,      NumGuilds);
        nmfUtils::initialize(dataStruct.CarryingCapacity,   NumGuilds);
        nmfUtils::initialize(dataStruct.CarryingCapacityMin,NumGuilds);
        nmfUtils::initialize(dataStruct.CarryingCapacityMax,NumGuilds);
        nmfUtils::initialize(dataStruct.CatchabilityMin,    NumGuilds);
        nmfUtils::initialize(dataStruct.CatchabilityMax,    NumGuilds);
std::cout << "Error: Implement loading for init values of parameter and for Survey Q" << std::endl;
        for (int guild=0; guild<NumGuilds; ++guild) {
            dataStruct.GrowthRateMin(guild)       = std::stod(dataMap["GrowthRateMin"][guild]);
            dataStruct.GrowthRateMax(guild)       = std::stod(dataMap["GrowthRateMax"][guild]);
            dataStruct.CarryingCapacity(guild)    = std::stod(dataMap["GuildK"][guild]);
            dataStruct.CarryingCapacityMin(guild) = std::stod(dataMap["GuildKMin"][guild]);
            dataStruct.CarryingCapacityMax(guild) = std::stod(dataMap["GuildKMax"][guild]);
            dataStruct.CatchabilityMin(guild)     = std::stod(dataMap["CatchabilityMin"][guild]);
            dataStruct.CatchabilityMax(guild)     = std::stod(dataMap["CatchabilityMax"][guild]);
//          dataStruct.SurveyQ(guild)             = std::stod(dataMap["SurveyQ"][guild]);
//          dataStruct.SurveyQMin(guild)          = std::stod(dataMap["SurveyQMin"][guild]);
//          dataStruct.SurveyQMax(guild)          = std::stod(dataMap["SurveyQMax"][guild]);
            guildName = dataMap["GuildName"][guild];
            GuildNum  = GuildMap[guildName];
            dataStruct.GuildSpecies[GuildNum].push_back(guild);
            dataStruct.GuildNum.push_back(GuildNum);
        }
        fields     = {"SpeName","GuildName","MinimumBiomass","InitBiomass","InitBiomassMin","InitBiomassMax"};
        queryStr   = "SELECT SpeName,GuildName,MinimumBiomass,InitBiomass,InitBiomassMin,InitBiomassMax from " +
                      nmfConstantsMSSPM::TableSpecies +
                     " ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumSpecies = dataMap["SpeName"].size();
        for (int species=0; species<NumSpecies; ++species) {
            guildName = dataMap["GuildName"][species];
            GuildSpeciesMap[dataMap["SpeName"][species]] = guildName;
            initialGuildBiomass[guildName]    += std::stod(dataMap["InitBiomass"][species]);
            initialGuildBiomassMin[guildName] += std::stod(dataMap["InitBiomassMin"][species]);
            initialGuildBiomassMax[guildName] += std::stod(dataMap["InitBiomassMax"][species]);

        }
        dataStruct.NumSpecies = NumSpecies;
        checkGuildRanges(NumGuilds,dataStruct);
    } else {
        fields     = {"Weight","SpeName","GuildName","MinimumBiomass","InitBiomass","InitBiomassMin","InitBiomassMax",
                      "GrowthRate","GrowthRateMin","GrowthRateMax","GrowthRateShape","GrowthRateShapeMin","GrowthRateShapeMax",
                      "SpeciesK","SpeciesKMin","SpeciesKMax","Catchability","CatchabilityMin","CatchabilityMax",
                      "SurveyQ","SurveyQMin","SurveyQMax"};
        queryStr   = "SELECT Weight,SpeName,GuildName,MinimumBiomass,InitBiomass,InitBiomassMin,InitBiomassMax,";
        queryStr  += "GrowthRate,GrowthRateMin,GrowthRateMax,GrowthRateShape,GrowthRateShapeMin,GrowthRateShapeMax,";
        queryStr  += "SpeciesK,SpeciesKMin,SpeciesKMax,Catchability,CatchabilityMin,CatchabilityMax,SurveyQ,SurveyQMin,SurveyQMax from " +
                      nmfConstantsMSSPM::TableSpecies +
                     " ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumSpecies = dataMap["SpeName"].size();
        dataStruct.NumSpecies = NumSpecies;
        nmfUtils::initialize(dataStruct.SpeciesWeights,     NumSpecies);
        nmfUtils::initialize(dataStruct.MinimumBiomass,     NumSpecies);
        nmfUtils::initialize(dataStruct.InitBiomass,        NumSpecies);
        nmfUtils::initialize(dataStruct.InitBiomassMin,     NumSpecies);
        nmfUtils::initialize(dataStruct.InitBiomassMax,     NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRate,         NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRateMin,      NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRateMax,      NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRateShape,    NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRateShapeMin, NumSpecies);
        nmfUtils::initialize(dataStruct.GrowthRateShapeMax, NumSpecies);
        nmfUtils::initialize(dataStruct.CarryingCapacity,   NumSpecies);
        nmfUtils::initialize(dataStruct.CarryingCapacityMin,NumSpecies);
        nmfUtils::initialize(dataStruct.CarryingCapacityMax,NumSpecies);
        nmfUtils::initialize(dataStruct.Catchability,       NumSpecies);
        nmfUtils::initialize(dataStruct.CatchabilityMin,    NumSpecies);
        nmfUtils::initialize(dataStruct.CatchabilityMax,    NumSpecies);
        nmfUtils::initialize(dataStruct.SurveyQ,            NumSpecies);
        nmfUtils::initialize(dataStruct.SurveyQMin,         NumSpecies);
        nmfUtils::initialize(dataStruct.SurveyQMax,         NumSpecies);
        for (int species=0; species<NumSpecies; ++species) {
            dataStruct.SpeciesWeights(species)      = std::stod(dataMap["Weight"][species]);
            dataStruct.MinimumBiomass(species)      = std::stod(dataMap["MinimumBiomass"][species]);
            dataStruct.InitBiomass(species)         = std::stod(dataMap["InitBiomass"][species]);
            dataStruct.InitBiomassMin(species)      = std::stod(dataMap["InitBiomassMin"][species]);
            dataStruct.InitBiomassMax(species)      = std::stod(dataMap["InitBiomassMax"][species]);
            dataStruct.GrowthRate(species)          = std::stod(dataMap["GrowthRate"][species]);
            dataStruct.GrowthRateMin(species)       = std::stod(dataMap["GrowthRateMin"][species]);
            dataStruct.GrowthRateMax(species)       = std::stod(dataMap["GrowthRateMax"][species]);
            dataStruct.GrowthRateShape(species)     = std::stod(dataMap["GrowthRateShape"][species]);
            dataStruct.GrowthRateShapeMin(species)  = std::stod(dataMap["GrowthRateShapeMin"][species]);
            dataStruct.GrowthRateShapeMax(species)  = std::stod(dataMap["GrowthRateShapeMax"][species]);
            dataStruct.CarryingCapacity(species)    = std::stod(dataMap["SpeciesK"][species]);
            dataStruct.CarryingCapacityMin(species) = std::stod(dataMap["SpeciesKMin"][species]);
            dataStruct.CarryingCapacityMax(species) = std::stod(dataMap["SpeciesKMax"][species]);
            dataStruct.Catchability(species)        = std::stod(dataMap["Catchability"][species]);
            dataStruct.CatchabilityMin(species)     = std::stod(dataMap["CatchabilityMin"][species]);
            dataStruct.CatchabilityMax(species)     = std::stod(dataMap["CatchabilityMax"][species]);
            if (isSurveyQ) {
                dataStruct.SurveyQ(species)         = std::stod(dataMap["SurveyQ"][species]);
                dataStruct.SurveyQMin(species)      = std::stod(dataMap["SurveyQMin"][species]);
                dataStruct.SurveyQMax(species)      = std::stod(dataMap["SurveyQMax"][species]);
            } else {
                dataStruct.SurveyQ(species)         = 1.0;
                dataStruct.SurveyQMin(species)      = 1.0;
                dataStruct.SurveyQMax(species)      = 1.0;
            }
            guildName = dataMap["GuildName"][species];
            GuildNum  = GuildMap[guildName];
            initialGuildBiomass[guildName]    += std::stod(dataMap["InitBiomass"][species]);
            initialGuildBiomassMin[guildName] += std::stod(dataMap["InitBiomassMin"][species]);
            initialGuildBiomassMax[guildName] += std::stod(dataMap["InitBiomassMax"][species]);
            dataStruct.GuildSpecies[GuildNum].push_back(species);
            dataStruct.GuildNum.push_back(GuildNum);
        }
    }

    NumSpeciesOrGuilds = (isAGGPROD) ? NumGuilds : NumSpecies;

    // Load Interaction coefficients as vector of rows
    if (isAlpha) {
        loadOK = loadInteraction(NumSpeciesOrGuilds, "Competition",
                                 nmfConstantsMSSPM::TableCompetitionAlpha,
                                 nmfConstantsMSSPM::TableCompetitionAlphaMin,
                                 nmfConstantsMSSPM::TableCompetitionAlphaMax,
                                 dataStruct.Competition,
                                 dataStruct.CompetitionMin,
                                 dataStruct.CompetitionMax,
                                 NumCompetitionParameters);
        if (! loadOK) return false;
    }
    if (isRho) {
        loadOK = loadInteraction(NumSpeciesOrGuilds, "Predation",
                                 nmfConstantsMSSPM::TablePredationRho,
                                 nmfConstantsMSSPM::TablePredationRhoMin,
                                 nmfConstantsMSSPM::TablePredationRhoMax,
                                 dataStruct.PredationRho,
                                 dataStruct.PredationRhoMin,
                                 dataStruct.PredationRhoMax,
                                 NumPredationParameters);
        if (! loadOK) return false;
    }
    if (isHandling) {
        loadOK = loadInteraction(NumSpeciesOrGuilds, "Handling",
                                 nmfConstantsMSSPM::TablePredationHandling,
                                 nmfConstantsMSSPM::TablePredationHandlingMin,
                                 nmfConstantsMSSPM::TablePredationHandlingMax,
                                 dataStruct.PredationHandling,
                                 dataStruct.PredationHandlingMin,
                                 dataStruct.PredationHandlingMax,
                                 NumHandlingParameters);
        if (! loadOK) return false;
    }
    if (isExponent) {
        loadOK = loadInteraction(NumSpeciesOrGuilds, "Exponent",
                                 nmfConstantsMSSPM::TablePredationExponent,
                                 nmfConstantsMSSPM::TablePredationExponentMin,
                                 nmfConstantsMSSPM::TablePredationExponentMax,
                                 dataStruct.PredationExponent,
                                 dataStruct.PredationExponentMin,
                                 dataStruct.PredationExponentMax,
                                 NumExponentParameters);
        if (! loadOK) return false;
    }

    if (isMSPROD) {
        loadOK = loadInteraction(NumSpecies, "MSPROD-Species",
                                 nmfConstantsMSSPM::TableCompetitionBetaSpecies,
                                 nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin,
                                 nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax,
                                 dataStruct.CompetitionBetaSpecies,
                                 dataStruct.CompetitionBetaSpeciesMin,
                                 dataStruct.CompetitionBetaSpeciesMax,
                                 NumBetaSpeciesParameters);
        if (! loadOK) return false;
        loadOK = loadInteractionGuilds(NumSpecies, NumGuilds, "Competition-MSPROD",
                                       GuildSpeciesMap,
                                       nmfConstantsMSSPM::TableCompetitionBetaGuilds,
                                       nmfConstantsMSSPM::TableCompetitionBetaGuildsMin,
                                       nmfConstantsMSSPM::TableCompetitionBetaGuildsMax,
                                       dataStruct.CompetitionBetaGuilds,
                                       dataStruct.CompetitionBetaGuildsMin,
                                       dataStruct.CompetitionBetaGuildsMax,
                                       NumBetaGuildsParameters);
        if (! loadOK) return false;
    } else if (isAGGPROD) {
        loadOK = loadInteractionGuildsGuilds(NumGuilds, NumGuilds, "Competition-AGGPROD",
                                             GuildSpeciesMap,
                                             nmfConstantsMSSPM::TableCompetitionBetaGuildsGuilds,
                                             nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMin,
                                             nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMax,
                                             dataStruct.CompetitionBetaGuildsGuilds,
                                             dataStruct.CompetitionBetaGuildsGuildsMin,
                                             dataStruct.CompetitionBetaGuildsGuildsMax,
                                             NumBetaGuildsParameters);
        if (! loadOK) return false;
    }

    // Calculate total number of parameters
    int NumEstParametersTotal = 0;
    int NumEstParametersTotalNonTrivial = 0;
    std::vector<int> NumEstParametersPerSpecies = {};
    std::map<std::string,bool> EstimateCheckBoxStates = Estimation_Tab7_ptr->getAllEstimateCheckboxStates();
    if (! m_DatabasePtr->getNumParameters(m_Logger,m_ProjectName,m_ModelName,
                                          EstimateCheckBoxStates,NumSpeciesOrGuilds,
                                          NumEstParametersPerSpecies,
                                          NumEstParametersTotalNonTrivial,
                                          NumEstParametersTotal)) {
        return false;
    }
    dataStruct.TotalNumberEstimatedParameters           = NumEstParametersTotal;
    dataStruct.TotalNumberEstimatedParametersNonTrivial = NumEstParametersTotalNonTrivial;
    dataStruct.TotalNumberEstimatedParametersBySpecies  = NumEstParametersPerSpecies;

/*
    // Calculate total number of parameters
    dataStruct.TotalNumberParameters = 0;
    dataStruct.TotalNumberParameters += NumSpecies; // Add on for estimating InitBiomass parameter
    if (growthForm == "Linear") {
        dataStruct.TotalNumberParameters += NumSpecies; // r for each Species
        dataStruct.TotalNumberParameters += NumSpecies; // r covariates for each Species
    } else if (growthForm == "Logistic") {
        dataStruct.TotalNumberParameters += NumSpecies; //  r
        dataStruct.TotalNumberParameters += NumSpecies; //  r shape
        dataStruct.TotalNumberParameters += NumSpecies; //  r covariates
        dataStruct.TotalNumberParameters += NumSpecies; //  K
        dataStruct.TotalNumberParameters += NumSpecies; //  K covariates
    }
    if ((harvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
        (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString())) {
        dataStruct.TotalNumberParameters += NumSpecies; // q
        dataStruct.TotalNumberParameters += NumSpecies; // q covariates
    }
    if (predationForm != "Null") {
        dataStruct.TotalNumberParameters += NumPredationParameters;
    }
    if ((predationForm == "Type II") || (predationForm == "Type III")) {
        dataStruct.TotalNumberParameters += NumHandlingParameters;
    }
    if (predationForm == "Type III") {
        dataStruct.TotalNumberParameters += NumExponentParameters;
    }
    if (competitionForm == "NO_K") {
        dataStruct.TotalNumberParameters += NumCompetitionParameters;
    } else if (competitionForm == "MS-PROD") {
        dataStruct.TotalNumberParameters += NumBetaSpeciesParameters;
        dataStruct.TotalNumberParameters += NumBetaGuildsParameters;
    } else if (competitionForm == "AGG-PROD") {
        dataStruct.TotalNumberParameters += NumBetaGuildsParameters;
    }

    // SurveyQ is always estimated...even for absolute biomass which is technically incorrect,
    // but it's set to 1 for absolute biomass and so it just makes the code simpler and should cause no issues.
    dataStruct.TotalNumberParameters += NumSpecies;
    // Also add the parameters for SurveyQ Covariate Coefficients
    dataStruct.TotalNumberParameters += NumSpecies;
*/
    // Set Benchmark type and number of parameters (RSK - improve this later)
    dataStruct.Benchmark = growthForm;
    if (isAlpha || isRho) {
        dataStruct.Benchmark = "LogisticMultiSpecies";
    }
    if (harvestForm == nmfConstantsMSSPM::HarvestCatch.toStdString()) {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableHarvestCatch,
                                               NumSpecies,RunLength,dataStruct.Catch)) {
            return false;
        }
        // m_Logger->logMsg(nmfConstants::Normal,"LoadParameters Read: Catch");
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableHarvestEffort,
                                               NumSpecies,RunLength,dataStruct.Effort)) {
            return false;
        }
        // m_Logger->logMsg(nmfConstants::Normal,"LoadParameters Read: Effort");
    } else if (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString()) {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableHarvestCatch,
                                               NumSpecies,RunLength,dataStruct.Catch)) {
            return false;
        }
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableHarvestEffort,
                                               NumSpecies,RunLength,dataStruct.Effort)) {
            return false;
        }
    } else if (harvestForm == "Exploitation (F)") {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableHarvestExploitation,
                                               NumSpecies,RunLength,dataStruct.Exploitation)) {
            return false;
        }
        // m_Logger->logMsg(nmfConstants::Normal,"LoadParameters Read: Exploitation");
    }

    if (isSurveyQ) {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableBiomassRelative,
                                               NumSpecies,RunLength,dataStruct.ObservedBiomassBySpecies)) {
            return false;
        }
//std::cout << "Loaded OBS: " << dataStruct.ObservedBiomassBySpecies(0,0) << std::endl;
        //  m_Logger->logMsg(nmfConstants::Normal,"LoadParameters Read: Relative Biomass");
    } else {
        if (! m_DatabasePtr->getTimeSeriesData(this,m_Logger,m_ProjectName,m_ModelName,
                                               "",nmfConstantsMSSPM::TableBiomassAbsolute,
                                               NumSpecies,RunLength,dataStruct.ObservedBiomassBySpecies)) {
            return false;
        }
        //  m_Logger->logMsg(nmfConstants::Normal,"LoadParameters Read: Absolute Biomass");
    }
//nmfUtils::printMatrix("dataStruct.ObservedBiomassBySpecies",dataStruct.ObservedBiomassBySpecies,10,8);
    // Load time series by guild observed biomass just load the first year's
    if (isAGGPROD) {
std::cout << "Warning: If loading observed biomass by guild, must account for user wanting relative biomass" << std::endl;
    }
    nmfUtils::initialize(dataStruct.ObservedBiomassByGuilds,RunLength+1,NumGuilds);
    for (int i=0; i<NumGuilds; ++i) {
       dataStruct.ObservedBiomassByGuilds(0,i) = initialGuildBiomass[Guild[i]];
       if (isAGGPROD) {
           dataStruct.InitBiomassMin[i] = initialGuildBiomassMin[Guild[i]];
           dataStruct.InitBiomassMax[i] = initialGuildBiomassMax[Guild[i]];
       }
    }

    // Assure all appropriate dataStruct elements are in metric tons
    // dataStruct.ObservedBiomassBySpecies(row,species)
    if (isSurveyQ) {
        if (previousUnits.find(tableBiomassRelative) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(dataStruct.ObservedBiomassBySpecies,previousUnits[tableBiomassRelative],"mt", nmfConstantsMSSPM::DontConvertBlanks);
            nmfUtilsQt::convertMatrix(dataStruct.ObservedBiomassByGuilds, previousUnits[tableBiomassRelative],"mt", nmfConstantsMSSPM::DontConvertBlanks);
        }
    } else {
        if (previousUnits.find(tableBiomassAbsolute) != previousUnits.end()) {
            nmfUtilsQt::convertMatrix(dataStruct.ObservedBiomassBySpecies,previousUnits[tableBiomassAbsolute],"mt", nmfConstantsMSSPM::DontConvertBlanks);
            nmfUtilsQt::convertMatrix(dataStruct.ObservedBiomassByGuilds, previousUnits[tableBiomassAbsolute],"mt", nmfConstantsMSSPM::DontConvertBlanks);
        }
    }
    if (previousUnits.find(tableHarvestCatch) != previousUnits.end()) {
        nmfUtilsQt::convertMatrix(dataStruct.Catch,                   previousUnits[tableHarvestCatch], "mt", nmfConstantsMSSPM::ConvertAll);
    }
    if (previousUnits.find(tableSpecies) != previousUnits.end()) {
        nmfUtilsQt::convertVector(dataStruct.InitBiomass,             previousUnits[tableSpecies],      "mt");
        nmfUtilsQt::convertVector(dataStruct.InitBiomassMin,          previousUnits[tableSpecies],      "mt");
        nmfUtilsQt::convertVector(dataStruct.InitBiomassMax,          previousUnits[tableSpecies],      "mt");
        nmfUtilsQt::convertVector(dataStruct.CarryingCapacity,        previousUnits[tableSpecies],      "mt");
        nmfUtilsQt::convertVector(dataStruct.CarryingCapacityMin,     previousUnits[tableSpecies],      "mt");
        nmfUtilsQt::convertVector(dataStruct.CarryingCapacityMax,     previousUnits[tableSpecies],      "mt");
    }

    // Load BiomassWeights
    nmfUtils::initialize(dataStruct.FitWeights,NumSpecies,2);
    for (int i=0; i<NumSpecies; ++i) {
        dataStruct.FitWeights(i,0) = 1.0;
        dataStruct.FitWeights(i,1) = 0.0;
    }
    if (isFitToCatch) {
        if (! m_DatabasePtr->getFitWeights(m_Logger,m_ProjectName,m_ModelName,
                                           nmfConstantsMSSPM::TableFitWeights,
                                           NumSpecies,dataStruct.FitWeights)) {
            // Don't return if false. There may not be any data if the user isn't
            // running EffortFitToCatch.
            //return false;
        }
    }

    // If loading for a forecast, set the Forecast Harvest Type
//    if (! m_DatabasePtr->getForecastInfo(
//                m_ProjectName,m_ModelName,ForecastName.toStdString(),
//                RunLength,StartYearForecast,
//                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
//                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
//                ForecastHarvestType,NumRuns)) {
//        //return false; // It might not be a forecast, so no error if fcn returns false
//    }
//    dataStruct.ForecastHarvestType = ForecastHarvestType;

    return true;

} // end loadParameters

void
nmfMainWindow::checkGuildRanges(const int &NumGuilds,
                                const nmfStructsQt::ModelDataStruct& dataStruct)
{
    bool isMissingGrowthData = true;
    bool isMissingKData = true;
    QString msg;

    for (int i=0; i<NumGuilds; ++i) {
        if ((dataStruct.GrowthRateMin[i] != 0) || (dataStruct.GrowthRateMax[i] != 0)) {
            isMissingGrowthData = false;
        }
        if ((dataStruct.CarryingCapacityMin[i] != 0) || (dataStruct.CarryingCapacityMax[i] != 0)) {
            isMissingKData = false;
        }
    }

    if (isMissingGrowthData || isMissingKData) {
        msg = "\nThe following data are missing from the Guilds Population Parameters page:\n\n";
        if (isMissingGrowthData) {
            msg += "  Guild Growth Rate range data\n";
        }
        if (isMissingKData) {
            msg += "  Guild Carrying Capacity range data\n";
        }
        QMessageBox::warning(this, "Warning",msg+"\n", QMessageBox::Ok);
    }
}

bool
nmfMainWindow::loadInteraction(int &NumSpeciesOrGuilds,
                               std::string InteractionType,
                               std::string InitTable,
                               std::string MinTable,
                               std::string MaxTable,
                               std::vector<double> &InitialData,
                               std::vector<double> &MinData,
                               std::vector<double> &MaxData,
                               int &NumInteractionParameters)
{
    int m;
    int NumRecords;
    double initVal;
    double minVal;
    double maxVal;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMapInit,dataMapMin,dataMapMax;
    std::string queryStr;
    QString msg;
    std::vector<double> MinRow;
    std::vector<double> MaxRow;

    NumInteractionParameters = 0;
    fields      = {"ModelName","SpeName","Value"};

    // Get Init data
    queryStr    = "SELECT ModelName,SpeName,Value FROM " +
                   InitTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapInit = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapInit["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds) {
        msg = "[Error 1] LoadInteraction1d: Incorrect number of records found in table: " + QString::fromStdString(InitTable) + ". Found " +
                QString::number(NumRecords) + " expecting " + QString::number(NumSpeciesOrGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " + QString::fromStdString(InteractionType) + " Parameters tab.", QMessageBox::Ok);
        return false;
    }

    // Get Min data
    queryStr    = "SELECT ModelName,SpeName,Value FROM " +
                   MinTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapMin  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMin["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds) {
        msg = "[Error 1] LoadInteraction1d: Incorrect number of records found in table: " + QString::fromStdString(MinTable) + ". Found " +
                QString::number(NumRecords) + " expecting " + QString::number(NumSpeciesOrGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " + QString::fromStdString(InteractionType) + " Parameters tab.", QMessageBox::Ok);
        return false;
    }

    // Get Max data
    queryStr    = "SELECT ModelName,SpeName,Value FROM " +
                   MaxTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapMax  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMax["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds) {
        m_Logger->logMsg(nmfConstants::Error,
                       "[Error 2] LoadInteraction1d: Incorrect number of records found in table: " + MaxTable + ". Found " +
                       std::to_string(NumRecords) + " expecting " + std::to_string(NumSpeciesOrGuilds) + ".");
        return false;
    }
    m = 0;
    for (int row=0; row<NumSpeciesOrGuilds; ++row) {
        initVal = std::stod(dataMapInit["Value"][m]);
        minVal  = std::stod(dataMapMin["Value"][m]);
        maxVal  = std::stod(dataMapMax["Value"][m]);
        ++NumInteractionParameters;
        if ((InteractionType == "Exponent") &&
             Estimation_Tab7_ptr->isEstPredationExponentEnabled() &&
             Estimation_Tab7_ptr->isEstPredationExponentChecked())
        {
            MinData.push_back(minVal);
            MaxData.push_back(maxVal);
        } else {
            if (InteractionType != "Exponent") {
                m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (1) of: "+InteractionType);
            }
            MinData.push_back(initVal); // RSK check this logic
            MaxData.push_back(initVal);
        }
        InitialData.push_back(initVal);
        ++m;
    }
    return true;
}

bool
nmfMainWindow::loadInteraction(int &NumSpeciesOrGuilds,
                               std::string InteractionType,
                               std::string InitTable,
                               std::string MinTable,
                               std::string MaxTable,
                               boost::numeric::ublas::matrix<double> &InitialData,
                               boost::numeric::ublas::matrix<double> &MinData,
                               boost::numeric::ublas::matrix<double> &MaxData,
                               int &NumInteractionParameters)
{
    int m;
    int NumRecords;
    double InitVal;
    double MinVal;
    double MaxVal;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMapInit,dataMapMin,dataMapMax;
    std::string queryStr;
    QString msg;
    std::vector<double> InitRow;
    std::vector<double> MinRow;
    std::vector<double> MaxRow;

    nmfUtils::initialize(InitialData,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(MinData,    NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(MaxData,    NumSpeciesOrGuilds,NumSpeciesOrGuilds);

    NumInteractionParameters = 0;
    fields      = {"ProjectName","ModelName","SpeciesA","SpeciesB","Value"};

    // Get data from the init table
    // Matrix data are store row by row
    queryStr    = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " +
                   InitTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapInit = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapInit["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumSpeciesOrGuilds) {
        m_Logger->logMsg(nmfConstants::Error,
                       "[Error 1] loadInteraction: Incorrect number of records found in table: " +
                       InitTable + ". Found " + std::to_string(NumRecords) + " expecting " +
                       std::to_string(NumSpeciesOrGuilds*NumSpeciesOrGuilds) + ".");
        return false;
    }

    // Get data from the min table
    queryStr    = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " +
                   MinTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapMin  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMin["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumSpeciesOrGuilds) {
        if (NumRecords == 0) {
            msg = "\nMissing data. Please populate table: " + QString::fromStdString(MinTable);
            QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);
        } else {
            msg = "[Error 2] loadInteraction: Incorrect number of records found in table: " +
                    QString::fromStdString(MinTable) + ". Found " +
                    QString::number(NumRecords) + " expecting " +
                    QString::number(NumSpeciesOrGuilds*NumSpeciesOrGuilds) + ".";
            m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " +
                                 QString::fromStdString(InteractionType) + " Parameters tab.",
                                 QMessageBox::Ok);
        }
        return false;
    }

    // Get data from the max table
    queryStr    = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " +
                   MaxTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMapMax  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMax["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumSpeciesOrGuilds) {
        m_Logger->logMsg(nmfConstants::Error,
                       "[Error 3] loadInteraction: Incorrect number of records found in table: " +
                       MaxTable + ". Found " + std::to_string(NumRecords) + " expecting " +
                       std::to_string(NumSpeciesOrGuilds*NumSpeciesOrGuilds) + ".");
        return false;
    }


    m = 0;
    for (int row=0; row<NumSpeciesOrGuilds; ++row) {
        for (int col=0; col<NumSpeciesOrGuilds; ++col) {
            InitVal = std::stod(dataMapInit["Value"][m]);
            MinVal  = std::stod(dataMapMin["Value"][m]);
            MaxVal  = std::stod(dataMapMax["Value"][m]);
            ++NumInteractionParameters;
            ++m;

            if (((InteractionType == "Competition")    && Estimation_Tab7_ptr->isEstCompetitionAlphaEnabled()       && Estimation_Tab7_ptr->isEstCompetitionAlphaChecked())  ||
                ((InteractionType == "Predation")      && Estimation_Tab7_ptr->isEstPredationRhoEnabled()           && Estimation_Tab7_ptr->isEstPredationRhoChecked())      ||
                ((InteractionType == "Handling")       && Estimation_Tab7_ptr->isEstPredationHandlingEnabled()      && Estimation_Tab7_ptr->isEstPredationHandlingChecked()) ||
                ((InteractionType == "MSPROD-Species") && Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesEnabled() && Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesChecked()))
            {
                MinData(row,col) = MinVal;
                MaxData(row,col) = MaxVal;
            } else {
                if ((InteractionType != "Competition") && (InteractionType != "Predation") &&
                    (InteractionType != "Handling")    && (InteractionType != "MSPROD-Species")) {
                    m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (2) of: "+InteractionType);
                }
                MinData(row,col) = InitVal;
                MaxData(row,col) = InitVal;
            }
            InitialData(row,col) = InitVal;
        }
    }

    /*
    m = 0;
    for (int row=0; row<NumSpeciesOrGuilds; ++row) {
        InitRow.clear();
        MinRow.clear();
        MaxRow.clear();
        for (int col=0; col<NumSpeciesOrGuilds; ++col) {
            InitVal = std::stod(dataMapInit["Value"][m]);
            MinVal  = std::stod(dataMapMin["Value"][m]);
            MaxVal  = std::stod(dataMapMax["Value"][m]);
            InitRow.push_back(InitVal);
            MinRow.push_back(MinVal);
            MaxRow.push_back(MaxVal);
            ++NumInteractionParameters;
            ++m;
        }
        if (((InteractionType == "Competition")    && Estimation_Tab7_ptr->isEstCompetitionAlphaEnabled()       && Estimation_Tab7_ptr->isEstCompetitionAlphaChecked())  ||
            ((InteractionType == "Predation")      && Estimation_Tab7_ptr->isEstPredationRhoEnabled()           && Estimation_Tab7_ptr->isEstPredationRhoChecked())      ||
            ((InteractionType == "Handling")       && Estimation_Tab7_ptr->isEstPredationHandlingEnabled()      && Estimation_Tab7_ptr->isEstPredationHandlingChecked()) ||
            ((InteractionType == "MSPROD-Species") && Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesEnabled() && Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesChecked()))
        {
            MinData.push_back(MinRow);
            MaxData.push_back(MaxRow);
        } else {
            if ((InteractionType != "Competition") && (InteractionType != "Predation") &&
                (InteractionType != "Handling")    && (InteractionType != "MSPROD-Species")) {
                m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (2) of: "+InteractionType);
            }
            MinData.push_back(InitRow);
            MaxData.push_back(InitRow);
        }
        InitialData.push_back(InitRow);
    }
    */
    return true;
}


bool
nmfMainWindow::loadInteractionGuilds(int &NumSpecies,
                                     int &NumGuilds,
                                     std::string InteractionType,
                                     std::map<std::string,std::string> &GuildSpeciesMap,
                                     std::string InitTable,
                                     std::string MinTable,
                                     std::string MaxTable,
                                     boost::numeric::ublas::matrix<double> &InitialData,
                                     boost::numeric::ublas::matrix<double> &MinData,
                                     boost::numeric::ublas::matrix<double> &MaxData,
                                     int &NumInteractionParameters)
{
    int m;
    int NumRecords;
    double valInit;
    double valMin;
    double valMax;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMapInit,dataMapMin,dataMapMax;
    std::string queryStr;
    QString msg;
    std::vector<double> InitRow;
    std::vector<double> MinRow;
    std::vector<double> MaxRow;
    int NumSpeciesOrGuilds = (InteractionType == "Competition-AGGPROD") ? NumGuilds : NumSpecies;


    nmfUtils::initialize(InitialData,NumSpeciesOrGuilds,NumGuilds);
    nmfUtils::initialize(MinData,    NumSpeciesOrGuilds,NumGuilds);
    nmfUtils::initialize(MaxData,    NumSpeciesOrGuilds,NumGuilds);

    NumInteractionParameters = 0;
    fields      = {"ModelName","SpeName","Guild","Value"};

    // Load Init data
    queryStr    = "SELECT ModelName,SpeName,Guild,Value FROM " +
                   InitTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapInit = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapInit["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumGuilds) {
        msg = "[Error 0] LoadInteractionGuilds: Incorrect number of records found in table: " +
                QString::fromStdString(InitTable) + ". Found " +
                QString::number(NumRecords) + " expecting " +
                QString::number(NumSpeciesOrGuilds*NumGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " +
                             QString::fromStdString(InteractionType) + " Parameters tab.",
                             QMessageBox::Ok);
        return false;
    }

    // Load Min data
    queryStr    = "SELECT ModelName,SpeName,Guild,Value FROM " +
                    MinTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapMin  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMin["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumGuilds) {
        msg = "[Error 1] LoadInteractionGuilds: Incorrect number of records found in table: " +
                QString::fromStdString(MinTable) + ". Found " +
                QString::number(NumRecords) + " expecting " +
                QString::number(NumSpeciesOrGuilds*NumGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " +
                             QString::fromStdString(InteractionType) + " Parameters tab.",
                             QMessageBox::Ok);
        return false;
    }

    // Load Max data
    queryStr    = "SELECT ModelName,SpeName,Guild,Value FROM " +
                   MaxTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapMax  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMax["Value"].size();
    if (NumRecords != NumSpeciesOrGuilds*NumGuilds) {
        m_Logger->logMsg(nmfConstants::Error,
                       "[Error 2] LoadInteractionGuilds: Incorrect number of records found in table: " +
                       MaxTable + ". Found " + std::to_string(NumRecords) + " expecting " +
                       std::to_string(NumSpeciesOrGuilds*NumGuilds) + ".");
        return false;
    }

    m = 0;
    for (int row=0; row<NumSpeciesOrGuilds; ++row) {
        InitRow.clear();
        MinRow.clear();
        MaxRow.clear();
        for (int col=0; col<NumGuilds; ++col) {
            valInit = std::stod(dataMapInit["Value"][m]);
            valMin  = std::stod(dataMapMin["Value"][m]);
            valMax  = std::stod(dataMapMax["Value"][m]);
            //            InitRow.push_back(valInit);
            //            MinRow.push_back(valMin);
            //            MaxRow.push_back(valMax);
            if ((InteractionType == "Competition-MSPROD") &&
                 Estimation_Tab7_ptr->isEstCompetitionBetaGuildsEnabled() &&
                 Estimation_Tab7_ptr->isEstCompetitionBetaGuildsChecked())
            {
                MinData(row,col) = valMin;
                MaxData(row,col) = valMax;
            } else {
                if (InteractionType != "Competition-MSPROD") {
                    m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (3) of: "+InteractionType);
                }
                MinData(row,col) = valInit;
                MaxData(row,col) = valInit;
            }
            InitialData(row,col) = valInit;

            ++NumInteractionParameters;
            ++m;
        }

        /*
        if ((InteractionType == "Competition-MSPROD") &&
             Estimation_Tab7_ptr->isEstCompetitionBetaGuildsEnabled() &&
             Estimation_Tab7_ptr->isEstCompetitionBetaGuildsChecked())
        {
            MinData.push_back(MinRow);
            MaxData.push_back(MaxRow);
        } else {
            if (InteractionType != "Competition-MSPROD") {
                m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (3) of: "+InteractionType);
            }
            MinData.push_back(InitRow);
            MaxData.push_back(InitRow);
        }
        InitialData.push_back(InitRow);
        */
    }
    return true;
}

bool
nmfMainWindow::loadInteractionGuildsGuilds(int &NumSpecies,
                                           int &NumGuilds,
                                           std::string InteractionType,
                                           std::map<std::string,std::string> &GuildSpeciesMap,
                                           std::string InitTable,
                                           std::string MinTable,
                                           std::string MaxTable,
                                           boost::numeric::ublas::matrix<double> &InitialData,
                                           boost::numeric::ublas::matrix<double> &MinData,
                                           boost::numeric::ublas::matrix<double> &MaxData,
                                           int &NumInteractionParameters)
{
    int m;
    int NumRecords;
    double valInit;
    double valMin;
    double valMax;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMapInit,dataMapMin,dataMapMax;
    std::string queryStr;
    QString msg;
    std::vector<double> InitRow;
    std::vector<double> MinRow;
    std::vector<double> MaxRow;

    NumInteractionParameters = 0;

    nmfUtils::initialize(InitialData,NumGuilds,NumGuilds);
    nmfUtils::initialize(MinData,    NumGuilds,NumGuilds);
    nmfUtils::initialize(MaxData,    NumGuilds,NumGuilds);

    fields      = {"ModelName","GuildA","GuildB","Value"};

    // Get Init data
    queryStr    = "SELECT ModelName,GuildA,GuildB,Value FROM " +
                   InitTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapInit = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapInit["Value"].size();
    if (NumRecords != NumGuilds*NumGuilds) {
        msg = "[Error 0] LoadInteractionGuildsGuilds: Incorrect number of records found in table: " +
                QString::fromStdString(InitTable) + ". Found " +
                QString::number(NumRecords) + " expecting " +
                QString::number(NumGuilds*NumGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " +
                             QString::fromStdString(InteractionType) + " Parameters tab.",
                             QMessageBox::Ok);
        return false;
    }

    // Get Min data
    queryStr    = "SELECT ModelName,GuildA,GuildB,Value FROM " +
                   MinTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapMin  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMin["Value"].size();
    if (NumRecords != NumGuilds*NumGuilds) {
        msg = "[Error 1] LoadInteractionGuildsGuilds: Incorrect number of records found in table: " +
                QString::fromStdString(MinTable) + ". Found " +
                QString::number(NumRecords) + " expecting " +
                QString::number(NumGuilds*NumGuilds) + ".";
        m_Logger->logMsg(nmfConstants::Error, msg.toStdString());
        QMessageBox::warning(this, "Error", "\n"+msg+"\n\nCheck min/max values in " +
                             QString::fromStdString(InteractionType) + " Parameters tab.",
                             QMessageBox::Ok);
        return false;
    }

    // Get Max data
    queryStr    = "SELECT ModelName,GuildA,GuildB,Value FROM " +
                   MaxTable +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
    dataMapMax  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords  = dataMapMax["Value"].size();
    if (NumRecords != NumGuilds*NumGuilds) {
        m_Logger->logMsg(nmfConstants::Error,
                       "[Error 2] LoadInteractionGuildsGuilds: Incorrect number of records found in table: " +
                       MaxTable + ". Found " + std::to_string(NumRecords) + " expecting " +
                       std::to_string(NumGuilds*NumGuilds) + ".");
        return false;
    }

    m = 0;
    for (int row=0; row<NumGuilds; ++row) {
        InitRow.clear();
        MinRow.clear();
        MaxRow.clear();
        for (int col=0; col<NumGuilds; ++col) {
            valInit = std::stod(dataMapInit["Value"][m]);
            valMin  = std::stod(dataMapMin["Value"][m]);
            valMax  = std::stod(dataMapMax["Value"][m]);
            //            InitRow.push_back(valInit);
            //            MinRow.push_back(valMin);
            //            MaxRow.push_back(valMax);

            if ((InteractionType == "Competition-AGGPROD") && Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsEnabled() && Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsChecked()) {
                MinData(row,col) = valMin;
                MaxData(row,col) = valMax;
            } else {
                if (InteractionType != "Competition-AGGPROD") {
                    m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (4) of: "+InteractionType);
                }
                MinData(row,col) = valInit;
                MaxData(row,col) = valInit;
            }
            InitialData(row,col) = valInit;


            ++NumInteractionParameters;
            ++m;
        }
//        if ((InteractionType == "Competition-AGGPROD") && Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsEnabled() && Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsChecked()) {
//            MinData.push_back(MinRow);
//            MaxData.push_back(MaxRow);
//        } else {
//            if (InteractionType != "Competition-AGGPROD") {
//                m_Logger->logMsg(nmfConstants::Warning,"Found un-handled interaction type (4) of: "+InteractionType);
//            }
//            MinData.push_back(InitRow);
//            MaxData.push_back(InitRow);
//        }
//        InitialData.push_back(InitRow);
    }
    return true;
}


void
nmfMainWindow::callback_AddedNewDatabase()
{
    updateWindowTitle();
    enableApplicationFeatures("SetupGroup",true);
    enableApplicationFeatures("AllOtherGroups",false);
}

void
nmfMainWindow::callback_ImportDatabase()
{
    QString msg = "\nPlease Note:\n";
    msg += "\n1. The name of the .sql file will be the name of the database. If you want, ";
    msg += "or are expecting, a particular database name for use in your model, be sure to ";
    msg += "rename the .sql file appropriately prior to importing.\n";
    msg += "\n2. The Project Name must match the name of the Project from the imported database.\n";
    msg += "\n3. File import may take several minutes depending upon file size and computer speed.\n";

    QMessageBox::StandardButton reply =
            QMessageBox::question(this, tr("Import Database"),
                                  tr(msg.toLatin1()), QMessageBox::Ok|QMessageBox::Cancel);
    if (reply == QMessageBox::Ok) {
      importDatabase("",nmfConstantsMSSPM::VerboseOff);
    }
}

void
nmfMainWindow::callback_ProjectSaved()
{
std::cout << "callback_ProjectSaved" << std::endl;
    updateWindowTitle();
    enableApplicationFeatures("SetupGroup",true);
    enableApplicationFeatures("AllOtherGroups",setupIsComplete());
}

void
nmfMainWindow::callback_UpdateWindowTitle()
{
    updateWindowTitle();
}

void
nmfMainWindow::updateWindowTitle()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectDatabase = settings->value("ProjectDatabase","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();

    std::string winTitle = "MSSPM (" + m_ProjectName + " - " + m_ModelName + ")";
    setWindowTitle(QString::fromStdString(winTitle));

    delete settings;
}


void
nmfMainWindow::initializeNavigatorTree()
{
    QTreeWidgetItem *item;

    NavigatorTree->clear();
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,nmfConstantsMSSPM::SectionSetup);
    nmfUtilsQt::addTreeItem(item, "1. Getting Started");
    nmfUtilsQt::addTreeItem(item, "2. Project Setup");
    nmfUtilsQt::addTreeItem(item, "3. Species Setup");
    nmfUtilsQt::addTreeItem(item, "4. Model Setup");
    item->setExpanded(true);

    // Create Estimation navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,nmfConstantsMSSPM::SectionEstimation);
    nmfUtilsQt::addTreeItem(item, "1. Population Parameters");
    nmfUtilsQt::addTreeItem(item, "2. Harvest Data");
    nmfUtilsQt::addTreeItem(item, "3. Competition Parameters");
    nmfUtilsQt::addTreeItem(item, "4. Predation Parameters");
    nmfUtilsQt::addTreeItem(item, "5. Observation Data");
    nmfUtilsQt::addTreeItem(item, "6. Covariate Data");
    nmfUtilsQt::addTreeItem(item, "7. Run Estimation");
    nmfUtilsQt::addTreeItem(item, "8. Model Review");
    item->setExpanded(true);

    // Create Diagnostic navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,nmfConstantsMSSPM::SectionDiagnostic);
    nmfUtilsQt::addTreeItem(item, "1. Parameter Profiles");
    nmfUtilsQt::addTreeItem(item, "2. Retrospective Analysis");
    item->setExpanded(true);

    // Create Simulation navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,nmfConstantsMSSPM::SectionForecast);
    nmfUtilsQt::addTreeItem(item, "1. Setup");
    nmfUtilsQt::addTreeItem(item, "2. Harvest Data");
    nmfUtilsQt::addTreeItem(item, "3. Uncertainty Parameters");
    nmfUtilsQt::addTreeItem(item, "4. Run Forecast");
    item->setExpanded(true);
}

void
nmfMainWindow::callback_Setup_Tab4_ModelPresetsCMB(QString type)
{
    // Call each of the callbacks for the ...FormCMB's so the
    // GUIs in the individual tabs will be correct.
    callback_Setup_Tab4_GrowthFormCMB(getFormGrowth());
    callback_Setup_Tab4_PredationFormCMB(getFormHarvest());
    callback_Setup_Tab4_HarvestFormCMB(getFormHarvest());
    callback_Setup_Tab4_CompetitionFormCMB(getFormCompetition());

    updateModelEquationSummary();
}

void
nmfMainWindow::callback_Setup_Tab4_GrowthFormCMB(QString name)
{
    updateModelEquationSummary();
}

void
nmfMainWindow::callback_Setup_Tab4_PredationFormCMB(QString name)
{
    updateModelEquationSummary();
}

void
nmfMainWindow::callback_Setup_Tab4_HarvestFormCMB(QString name)
{
    Estimation_Tab2_ptr->setHarvestType(name.toStdString());

    updateModelEquationSummary();
}

void
nmfMainWindow::callback_Setup_Tab4_CompetitionFormCMB(QString name)
{
    updateModelEquationSummary();
}

void
nmfMainWindow::setup3dChart()
{
    if (m_Graph3D != nullptr) {
        m_ChartView3d->show();
        return;
    }
    m_Graph3D = new Q3DSurface();
std::cout << "shadow quality: " << m_Graph3D->shadowQuality() << std::endl;
    Q3DTheme *myTheme;
    myTheme = m_Graph3D->activeTheme();
    myTheme->setLabelBorderEnabled(false);

    m_ChartView3d = QWidget::createWindowContainer(m_Graph3D);
    if (!m_Graph3D->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }
    m_ChartView3d->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    OutputChartMainLayt->insertWidget(0,m_ChartView3d);

    // RSK Hopefully this will be implemented in a later Qt version
    // m_graph3D->setSelectionMode(QAbstract3DGraph::SelectionRowAndColumn);

}

bool
nmfMainWindow::selectMinimumSurfacePoint()
{
    int yMin = 0;
    int yMax = 0;

    if (m_Graph3D != nullptr) {

        QString xLabel = Diagnostic_Tab1_ptr->getParameter1Name() + " Percent Deviation";
        QString yLabel = "Fitness";
        QString zLabel = Diagnostic_Tab1_ptr->getParameter2Name() + " Percent Deviation";
        QString xLabelFormat = "%d";
        QString zLabelFormat = "%d";

        boost::numeric::ublas::matrix<double> rowValues;
        boost::numeric::ublas::matrix<double> columnValues;
        boost::numeric::ublas::matrix<double> heightValues;

        bool ok = getSurfaceData(rowValues,columnValues,heightValues,yMin,yMax);
        if (ok) {
            nmfChartSurface surface(m_Graph3D,xLabel,yLabel,zLabel,
                                    xLabelFormat,zLabelFormat,
                                    nmfConstants::DontReverseAxis,
                                    nmfConstants::DontReverseAxis,
                                    rowValues,columnValues,heightValues,
                                    Output_Controls_ptr->isShadowShown(),
                                    true,yMin,yMax);
            surface.selectMinimumPoint();
        }
    }

    return true;
}

bool
nmfMainWindow::callback_ShowDiagnostics()
{
    callback_ShowDiagnosticsChart3d();
    Output_Controls_ptr->setOutputParameters2d3dPB(nmfConstantsMSSPM::ChartType3d);

    return true;
}

bool
nmfMainWindow::callback_ShowDiagnosticsChart3d()
{
    bool ok;
    if (m_Graph3D != nullptr) {

        QString xLabel = Diagnostic_Tab1_ptr->getParameter1Name() + " Percent Deviation";
        QString yLabel = "Fitness";
        QString zLabel = Diagnostic_Tab1_ptr->getParameter2Name() + " Percent Deviation";
        QString xLabelFormat = "%d";
        QString zLabelFormat = "%d";

        boost::numeric::ublas::matrix<double> rowValues;
        boost::numeric::ublas::matrix<double> columnValues;
        boost::numeric::ublas::matrix<double> heightValues;

        int yMin = 0;
        int yMax = 0;
        ok = getSurfaceData(rowValues,columnValues,heightValues,yMin,yMax);
        if (ok) {
            nmfChartSurface surface(m_Graph3D,xLabel,yLabel,zLabel,
                                    xLabelFormat,zLabelFormat,
                                    nmfConstants::DontReverseAxis,
                                    nmfConstants::DontReverseAxis,
                                    rowValues,columnValues,heightValues,
                                    Output_Controls_ptr->isShadowShown(),
                                    true,yMin,yMax);
            surface.selectCenterPoint();
        }
    }

    return true;
}

bool
nmfMainWindow::getSurfaceData(
        boost::numeric::ublas::matrix<double>& rowValues,
        boost::numeric::ublas::matrix<double>& columnValues,
        boost::numeric::ublas::matrix<double>& heightValues,
        int& yMin,
        int& yMax)
{
    int m=0;
    int NumRecords;
    int NumPoints          = Diagnostic_Tab1_ptr->getLastRunsNumPoints();
    int NumPointsAlongEdge = 2*NumPoints+1;
    int nrows = NumPointsAlongEdge;
    int ncols = NumPointsAlongEdge;
    float x,y,z;
    std::string msg;
    std::string queryStr;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::string isAggProdStr;
    std::string SurfaceType;
    std::string currentSpecies = Output_Controls_ptr->getOutputSpecies().toStdString();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::vector<std::string> TableNames = {nmfConstantsMSSPM::TableDiagnosticSurface};

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    checkDiagnosticAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

    isAggProdStr = (CompetitionForm == "AGG-PROD") ? "1" : "0";
    SurfaceType = (Output_Controls_ptr->isSurfaceTypeZScore()) ? "ZScore" : "Absolute";

    fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd",
                  "SpeName","Type","parameter1PctVar","parameter2PctVar","Fitness"};
    queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,";
    queryStr  += "SpeName,Type,parameter1PctVar,parameter2PctVar,Fitness FROM " +
                  TableNames[0] +
                 " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND SpeName = '"            + currentSpecies +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling +
                 "' AND isAggProd = "           + isAggProdStr +
                 "  AND Type = '"               + SurfaceType +
                 "' ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Algorithm"].size();
    if (NumRecords != nrows*ncols) {
        msg  = "[Error 1] nmfMainWindow::getSurfaceData: Incorrect number of records read. Found "+std::to_string(NumRecords);
        msg += " expecting " + std::to_string(nrows*ncols);
        m_Logger->logMsg(nmfConstants::Error,msg);
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return false;
    }

    rowValues.clear();
    columnValues.clear();
    heightValues.clear();
    rowValues.resize(nrows,ncols);
    columnValues.resize(nrows,ncols);
    heightValues.resize(nrows,ncols);
    double theYMax = 0;
    double theYMin = 9999999;
    for (int row = 0; row < nrows; ++row) {
        for (int col = 0; col < ncols; ++col) {
            x = std::stod(dataMap["parameter1PctVar"][m]);
            y = std::stod(dataMap["Fitness"][m]);
            z = std::stod(dataMap["parameter2PctVar"][m]);
            theYMin = (y < theYMin) ? y : theYMin;
            theYMax = (y > theYMax) ? y : theYMax;
            rowValues(row,col)    = x;
            heightValues(row,col) = y;
            columnValues(row,col) = z;
            ++m;
        }
    }
    yMin = theYMin;
    yMax = theYMax;

    return true;
}


void
nmfMainWindow::callback_SetChartView2d(bool setTo2d)
{
    if ((m_ChartView2d == nullptr) || (m_ChartView3d == nullptr))
        return;

    if (setTo2d) {
        m_ChartView3d->hide();
        m_ChartView2d->show();
        m_UI->OutputDataTV->show();
    } else {
        m_ChartView2d->hide();
        m_ChartView3d->show();
        m_UI->OutputDataTV->hide();
        m_ChartView3d->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }
}


void
nmfMainWindow::callback_ModelSaved()
{
//std::cout << "callback_ModelSaved" << std::endl;
    enableApplicationFeatures("AllOtherGroups",setupIsComplete());
    Estimation_Tab7_ptr->clearEnsembleFile();
    Estimation_Tab7_ptr->enableNonEnsembleWidgets(true);

    // Check if model was actually a multi-run and set accordingly
    checkIfModelIsAMultiRun();

    Setup_Tab4_ptr->saveSettings();
}


void
nmfMainWindow::callback_ModelLoaded()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfMainWindow::callback_ModelLoaded start");
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Setup_Tab4_ptr->saveSettings();

    readSettings();

    // The whatsThis text message on the progress chart should change
    // depending upon which Estimation Algorithm is used.
    m_ProgressWidget->shouldYRangeEditsStick(false);
    updateProgressChartAnnotation(0,40.0,2.0);

    // Update the input tables when the user
    // loads a new system name
    Setup_Tab4_ptr->loadWidgets();
    Estimation_Tab2_ptr->loadWidgets();
    Estimation_Tab3_ptr->loadWidgets();
    Estimation_Tab4_ptr->loadWidgets();
    Estimation_Tab5_ptr->loadWidgets();
    Estimation_Tab6_ptr->loadWidgets();
    Estimation_Tab7_ptr->loadWidgets();
    Estimation_Tab8_ptr->loadWidgets();

    Diagnostic_Tab1_ptr->loadWidgets();
    Diagnostic_Tab2_ptr->loadWidgets();

    // Clear Forecast widgets since loaded Forecast may not
    // be compatible with new system loaded.
    Forecast_Tab1_ptr->clearWidgets();

    // Resave covariate assignment
    Estimation_Tab6_ptr->resaveCovariateAssignmentTable();

    // If not Effort Fit to Catch, set and disable Forecast Havest Data button
    QString modelHarvestType = Setup_Tab4_ptr->getHarvestFormCMB()->currentText();

    Forecast_Tab2_ptr->enableHarvestTypeButton(true,"");
    if (modelHarvestType == nmfConstantsMSSPM::HarvestCatch) {
        Forecast_Tab2_ptr->enableHarvestTypeButton(false,nmfConstantsMSSPM::ForecastHarvestTypeCatch);
    } else if (modelHarvestType == nmfConstantsMSSPM::HarvestEffort) {
        Forecast_Tab2_ptr->enableHarvestTypeButton(false,nmfConstantsMSSPM::ForecastHarvestTypeEffort);
    }

    // Clear the progress chart
    m_ProgressWidget->clearChart();

    // Reset output controls
    Output_Controls_ptr->loadSpeciesControlWidget();
    Output_TableWidgets_ptr->loadWidgets();

    // Enable Navigator Tree
    NavigatorTree->setEnabled(true);

    // Set image path for viewer
    QString imagePath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputImages");
    if (m_ViewerWidget != nullptr) {
        m_ViewerWidget->setImagePath(imagePath);
        m_ViewerWidget->callback_RefreshPB();
    }
    Estimation_Tab7_ptr->callback_EnsembleControlsGB(false);

    // Update REMORA
    Remora_ptr->setProjectName(m_ProjectName);
    Remora_ptr->setModelName(m_ModelName);
    if (thereIsOutputBiomass()) {
        Output_Controls_ptr->refresh();
        enableRunWidgets(true);
    } else {
        enableRunWidgets(false);
    }

    // Hack to get enable-ness correct on Tab6 widgets.  Should
    // remove it and fix the logic.
    Estimation_Tab7_ptr->loadWidgets();

    // Reset multi-run and hpc flags
    m_IsMultiRun    = false;
    m_UsingHPCFiles = false;
    m_HPC_Algorithm = "";
    m_HPC_Minimizer = "";
    m_HPC_ObjectiveCriterion = "";
    m_HPC_Scaling = "";

    // Refresh output chart
    callback_ShowChart("","");

    QApplication::restoreOverrideCursor();
}

void
nmfMainWindow::checkIfModelIsAMultiRun()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"EnsembleIsBoxChecked","EnsembleFile"};
    queryStr = "SELECT EnsembleIsBoxChecked,EnsembleFile FROM " +
                nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName + "'";
    dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    m_IsMultiRun = (dataMap["EnsembleIsBoxChecked"][0] == "1");

    if (m_IsMultiRun) {
        std::string ensembleFile = dataMap["EnsembleFile"][0];
        Estimation_Tab7_ptr->callback_EnsembleControlsGB(true);
        Estimation_Tab7_ptr->enableEnsembleControls(true);
        QString filePath             = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
        QString existingEnsembleFile = QDir(filePath).filePath(QString::fromStdString(ensembleFile));
        QString newEnsembleFile      = QDir(filePath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun));
        QFile::remove(newEnsembleFile); // if it exists, it must be removed prior to the copy command
        if (QFile::copy(existingEnsembleFile, newEnsembleFile)) {
            Estimation_Tab7_ptr->callback_EnsembleLoadPB();
        }
    }
}

bool
nmfMainWindow::thereIsOutputBiomass()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"Algorithm"};
    queryStr = "SELECT DISTINCT Algorithm FROM " +
                nmfConstantsMSSPM::TableOutputBiomass +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    return (dataMap["Algorithm"].size());
}

void
nmfMainWindow::updateProgressChartAnnotation(double xMin, double xMax, double xInc)
{
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QString whatsThis;

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (Algorithm == "NLopt Algorithm") {
        if (ObjectiveCriterion == "Least Squares") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the log (base 10) Sum of the Squares ";
            whatsThis += "Error (SSE) vs the Number of NLopt Objective Function Evaluations.</p> ";
            whatsThis += "The smaller the SSE the better the parameter fit.</p>";
            m_ProgressWidget->setYTitle("Log(SSE)");
            m_ProgressWidget->setYInc(0.1);
            m_ProgressWidget->setMainTitle("<b>Log Sum of Squares (SSE) vs Objective Function Evaluations</b>");
        } else if (ObjectiveCriterion == "Maximum Likelihood") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the Maximum Likelihood ";
            whatsThis += "value (i.e., fitness) vs the Number of NLopt Objective Function Evaluations.</p> ";
            whatsThis += "The closer the MLE value is to 0 the better the parameter fit.</p>";
            m_ProgressWidget->setYTitle("Maximum Likelihood (MLE)");
            m_ProgressWidget->setYInc(0.1);
            m_ProgressWidget->setMainTitle("<b>Maximum Likelihood (MLE) vs Objective Function Evaluations</b>");
        } else if (ObjectiveCriterion == "Model Efficiency") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the Model Efficiency ";
            whatsThis += "value (i.e., fitness) vs the Number of NLopt Objective Function Evaluations.</p> ";
            whatsThis += "The closer the MEF value is to 1 the better the parameter fit.</p>";
            m_ProgressWidget->setYTitle("Model Efficiency (MEF)");
            m_ProgressWidget->setYInc(0.1);
            m_ProgressWidget->setMainTitle("<b>Model Efficiency (MEF) vs Objective Function Evaluations</b>");
        }
        m_ProgressWidget->setYRange(0.0,4.0);
        m_ProgressWidget->setXAxisTitleScale("Objective Function Evaluations",xMin,xMax,xInc);
    } else if (Algorithm == "Bees Algorithm") {
        if (ObjectiveCriterion == "Least Squares") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the Sum of the Squares ";
            whatsThis += "Error (SSE) vs the Number of Bee Generations (i.e. Stochastic Cycles).</p> ";
            whatsThis += "The smaller the SSE the better the Parameter fit.</p>";
            m_ProgressWidget->setYTitle("Log(Sum of Squares) (SSE)");
            m_ProgressWidget->setYInc(0.1);
            m_ProgressWidget->setMainTitle("<b>Log(Sum of Squares) (SSE) vs Generation</b>");
        } else if (ObjectiveCriterion == "Maximum Likelihood") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the Maximium Likelihood (MLE) ";
            whatsThis += "value vs the Number of Bee Generations (i.e. Stochastic Cycles).</p> ";
            whatsThis += "The closer the MLE value is to 0 the better the Parameter fit.</p>";
            m_ProgressWidget->setYTitle("Maximum Likelihood (MLE)");
            m_ProgressWidget->setYInc(0.2);
            m_ProgressWidget->setMainTitle("<b>Maximum Likelihood (MLE) vs Generation</b>");
        } else if (ObjectiveCriterion == "Model Efficiency") {
            whatsThis = "<strong><center>Progress Chart</center></strong><p>This chart plots the Model Efficiency (MEF) ";
            whatsThis += "value vs the Number of Bee Generations (i.e. Stochastic Cycles).</p> ";
            whatsThis += "The closer the MEF value is to 1 the better the Parameter fit.</p>";
            m_ProgressWidget->setYTitle("Model Efficiency (MEF)");
            m_ProgressWidget->setYInc(0.2);
            m_ProgressWidget->setMainTitle("<b>Model Efficiency (MEF) vs Generation</b>");
        }
        m_ProgressWidget->setYRange(0.0,1.0);
        m_ProgressWidget->setXAxisTitleScale("Generations",xMin,xMax,xInc);
    }

    m_ProgressWidget->shouldYRangeEditsStick(true);
    m_ProgressWidget->setWhatsThis(whatsThis);
}

void
nmfMainWindow::callback_UpdateModelEquationSummary()
{
  updateModelEquationSummary();
}

void
nmfMainWindow::callback_SetupTabChanged(int tab)
{
    QModelIndex topLevelIndex = NavigatorTree->model()->index(0,0); // first 0 is Setup group in NavigatorTree
    QModelIndex childIndex    = topLevelIndex.model()->index(tab,0,topLevelIndex);
    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_EstimationTabChanged(int tab)
{
    QModelIndex topLevelIndex = NavigatorTree->model()->index(1,0); // 1 is Estimation group in NavigatorTree
    QModelIndex childIndex    = topLevelIndex.model()->index(tab,0,topLevelIndex);
    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_DiagnosticsTabChanged(int tab)
{
    QModelIndex topLevelIndex = NavigatorTree->model()->index(2,0); // 2 is Diagnostics group in NavigatorTree
    QModelIndex childIndex    = topLevelIndex.model()->index(tab,0,topLevelIndex);
    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_DisableMohnsRhoRun()
{
    Diagnostic_Tab2_ptr->setMohnsRhoForSingleRun(false);
    Diagnostic_Tab2_ptr->setMohnsRhoForMultiRun(false);
}

void
nmfMainWindow::callback_ForecastTabChanged(int tab)
{
    QModelIndex topLevelIndex = NavigatorTree->model()->index(3,0); // 3 is Forecast group in NavigatorTree
    QModelIndex childIndex    = topLevelIndex.model()->index(tab,0,topLevelIndex);
    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_ForecastLineBrightnessChanged(double brightnessFactor)
{
    int StartYearForecast = nmfConstantsMSSPM::Start_Year;
    int RunLength;
    int StartYear;
    QString ScaleStr = Output_Controls_ptr->getOutputScale();
    double ScaleVal  = convertUnitsStringToValue(ScaleStr);
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string ForecastName = Forecast_Tab1_ptr->getForecastName();
    double YMinSliderValue   = Output_Controls_ptr->getYMinSliderVal();
    bool showHistoricalData  = Output_Controls_ptr->isHistoricalDataShown();

    fields    = {"ProjectName","ModelName","ForecastName","StartYear"};
    queryStr  = "SELECT ProjectName,ModelName,ForecastName,StartYear FROM " +
                 nmfConstantsMSSPM::TableForecasts +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '"    + m_ModelName +
                "' AND ForecastName = '" + ForecastName + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ForecastName"].size() != 0) {
        StartYearForecast  = std::stoi(dataMap["StartYear"][0]);
    }

    // Add year offset if showing historical data
    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
    StartYearForecast = (showHistoricalData) ? StartYear : StartYearForecast;

    if (! showForecastChart(isAggProd(),ForecastName,
                            StartYearForecast,
                            ScaleStr,ScaleVal,
                            YMinSliderValue,brightnessFactor)) {
        return;
    }
}

void
nmfMainWindow::callback_SelectCenterSurfacePoint()
{
    callback_ShowDiagnosticsChart3d();
}

void
nmfMainWindow::callback_SelectMinimumSurfacePoint()
{
    selectMinimumSurfacePoint();
}

void
nmfMainWindow::callback_SetOutputScenarioForecast()
{
    Forecast_Tab4_ptr->setOutputScenario(Output_Controls_ptr->getOutputScenario().toStdString());
}

void
nmfMainWindow::callback_OutputTypeCMB(QString type,
                                      std::map<QString,QStringList> SortedForecastLabelsMap)
{
   Output_Controls_ptr->setForecastLabels(SortedForecastLabelsMap);
   Output_Controls_ptr->setOutputType(type);
}

void
nmfMainWindow::callback_ClearEstimationTables()
{
    Estimation_Tab1_ptr->clearWidgets();
    Estimation_Tab2_ptr->clearWidgets();
    Estimation_Tab3_ptr->clearWidgets();
    Estimation_Tab4_ptr->clearWidgets();
    Estimation_Tab5_ptr->clearWidgets();
    Estimation_Tab7_ptr->clearWidgets();
    Estimation_Tab8_ptr->clearWidgets();
}

bool
nmfMainWindow::areFieldsValid(std::string tableName,
                              std::string modelName,
                              std::vector<std::string> fields)
{
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    queryStr = "SELECT ";
    for (std::string field : fields) {
        queryStr += field + ",";
    }
    queryStr   = queryStr.substr(0,queryStr.size()-1); // strip last comma
    queryStr  += " FROM " + tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + modelName     + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr,fields,nmfConstantsMSSPM::ShowBlankFields);

    return checkFields(tableName,dataMap,fields);
}


bool
nmfMainWindow::areFieldsValid(std::string tableName,
                              std::vector<std::string> fields)
{
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    queryStr = "SELECT ";
    for (std::string field : fields) {
        queryStr += field + ",";
    }
    queryStr   = queryStr.substr(0,queryStr.size()-1); // strip last comma
    queryStr  += " FROM " + tableName;
    // These aren't needed yet as the nmfConstantsMSSPM::TableSpecies table does not
    // yet have any ProjectName or ModelName fields.
    //   " WHERE ProjectName = '" + m_ProjectName +
    //   "' AND ModelName = '"    + m_ModelName   + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr,fields,nmfConstantsMSSPM::ShowBlankFields);

    return checkFields(tableName,dataMap,fields);
}

bool
nmfMainWindow::checkFields(std::string& tableName,
                           std::map<std::string, std::vector<std::string> >& dataMap,
                           std::vector<std::string>& fields)
{
    std::string msg;
    QString value;
    bool ok;

    // Ensure there are no empty fields
    for (std::string field : fields) {
        if (dataMap[field].size() == 0) {
            return false;
        }
        for (unsigned int i=0; i<dataMap[field].size(); ++i) {
            value = QString::fromStdString(dataMap[field][i]);
            msg = "Error in table: " + tableName +
                    ", " + field + " = " + value.toStdString();
            if (value.isEmpty() || value.contains(',')) {
                m_Logger->logMsg(nmfConstants::Error,msg);
                return false;
            }
            if (value.toDouble(&ok)) {
                if (! ok) {
                    m_Logger->logMsg(nmfConstants::Error,msg);
                    return false;
                }
            }
        }
    }
    return true;
}



std::pair<bool,std::string>
nmfMainWindow::dataAdequateForCurrentModel(QStringList estParamNames)
{
    std::string msg;

    // Log parameter names
    for (int i=0; i<estParamNames.size(); ++i) {
        msg = "Checking valid ranges for parameter: " + estParamNames[i].toStdString();
        m_Logger->logMsg(nmfConstants::Normal,msg);
    }

    if (estParamNames.contains(nmfConstantsMSSPM::ParameterNameInitialBiomass)) {
        // Check Species for valid B₀, B₀ min, and B₀ max values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","InitBiomass","InitBiomassMin","InitBiomassMax"})) {
            return std::make_pair(false,"");
        }
    }

    if (estParamNames.contains("Growth Rate") && !estParamNames.contains("Shape")) {
        // Check Species for valid r, r min, and r max values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","GrowthRate","GrowthRateMin","GrowthRateMax"})) {
            return std::make_pair(false,"");
        }
    }

    if (estParamNames.contains("Growth Rate Shape")) {
        // Check Species for valid p, p min, and p max values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","GrowthRateShape","GrowthRateShapeMin","GrowthRateShapeMax"})) {
            return std::make_pair(false,"");
        }
    }

    if (estParamNames.contains("Carrying Capacity")) {
        // Check Species for valid K, K min, and K max values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","SpeciesK","SpeciesKMin","SpeciesKMax"})) {
            return std::make_pair(false,"");
        }
    }

    if (estParamNames.contains("Catchability")) {
        // Check Species for valid q, q min, and q max values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","Catchability","CatchabilityMin","CatchabilityMax"})) {
            return std::make_pair(false,"");
        }
    }

    if (estParamNames.contains("SurveyQ")) {
        // Check Species for valid SurveyQ, SurveyQMin, and SurveyQMax values
        if (! areFieldsValid(nmfConstantsMSSPM::TableSpecies,{"SpeName","SurveyQ","SurveyQMin","SurveyQMax"})) {
            return std::make_pair(false,"");
        }
    }

    QStringList paramNames = {
        nmfConstantsMSSPM::ParameterNameCompetitionAlpha,
        nmfConstantsMSSPM::ParameterNameCompetitionBetaSpecies,
        nmfConstantsMSSPM::ParameterNamePredationRho,
        nmfConstantsMSSPM::ParameterNamePredationHandling};
    std::vector<std::string> tableNames = {nmfConstantsMSSPM::TableCompetitionAlphaMin,
                                           nmfConstantsMSSPM::TableCompetitionAlphaMax,
                                           nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin,
                                           nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax,
                                           nmfConstantsMSSPM::TablePredationRhoMin,
                                           nmfConstantsMSSPM::TablePredationRhoMax,
                                           nmfConstantsMSSPM::TablePredationHandlingMin,
                                           nmfConstantsMSSPM::TablePredationHandlingMax};
    for (int i=0; i<paramNames.size(); ++i) {
        // Cycle through min/max tables
        if (estParamNames.contains(paramNames[i])) {
            for (int j=0; j<2; ++j) {
                if (! areFieldsValid(tableNames[2*i+j],
                                     m_ModelName,
                                     {"SpeciesA","SpeciesB","Value"}))
                {
                    return std::make_pair(false,tableNames[2*i+j]);
                }
            }
        }

    }

    paramNames.clear();
    tableNames.clear();
    paramNames << nmfConstantsMSSPM::ParameterNameCompetitionBetaGuilds;
    paramNames << nmfConstantsMSSPM::ParameterNameCompetitionBetaGuildsGuilds;
    paramNames << nmfConstantsMSSPM::ParameterNamePredationExponent;
    tableNames.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsMin);
    tableNames.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsMax);
    tableNames.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMin);
    tableNames.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMax);
    tableNames.push_back(nmfConstantsMSSPM::TablePredationExponentMin);
    tableNames.push_back(nmfConstantsMSSPM::TablePredationExponentMax);
    for (int i=0; i<paramNames.size(); ++i) {
        // Cycle through min/max tables
        if (estParamNames.contains(paramNames[i])) {
            for (int j=0; j<2; ++j) {
                if (! areFieldsValid(tableNames[2*i+j],
                                     m_ModelName,
                                     {"SpeName","Value"}))
                {
                    return std::make_pair(false,tableNames[2*i+j]);
                }
            }
        }
    }

    return std::make_pair(true,"");
}

void
nmfMainWindow::callback_CheckAllEstimationTablesAndRun()
{
    QString msg;
    QStringList estParamNames = Setup_Tab4_ptr->getEstimatedParameterNames();
    std::pair<bool,std::string> dataCheck = dataAdequateForCurrentModel(estParamNames);

    if (dataCheck.first) {
        callback_RunEstimation(nmfConstantsMSSPM::DontShowDiagnosticsChart);
        QApplication::restoreOverrideCursor();
    } else {
        QApplication::restoreOverrideCursor();
        msg  = "Invalid or missing data found in one or more input Estimation tables. ";
        msg += "\nPerhaps you didn't enter all of the min/max values in the Species/Guilds tables.";
        m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
        msg += "\n\nPlease check all input tables for complete data.";
        QMessageBox::critical(this, "Error",
                              "\n"+msg+"\n", QMessageBox::Ok);
        enableRunWidgets(false);
    }
}

void
nmfMainWindow::callback_AddToReview()
{
    std::string DefaultAveragingAlgorithm;
    QStringList rowItems;
    bool isAMultiRun = isAMultiOrMohnsRhoRun();
    bool isSigDigChecked = saveSigDigState();
    QStandardItemModel* statisticsModel = qobject_cast<QStandardItemModel*>(SummaryTV->model());
    restoreSigDigState(isSigDigChecked);
    QString dbSnapshotFile = Estimation_Tab8_ptr->generateDatabaseSnapshot();

    int lastColumn = statisticsModel->columnCount() - 1;
    QModelIndex indexSSResiduals = statisticsModel->index(0,lastColumn);
    QModelIndex indexRSquared    = statisticsModel->index(3,lastColumn);
    QModelIndex indexAIC         = statisticsModel->index(5,lastColumn);

    rowItems << Setup_Tab4_ptr->getModelName();     // 0

    rowItems << indexRSquared.data().toString();    // 1
    rowItems << indexSSResiduals.data().toString(); // 2
    rowItems << indexAIC.data().toString();         // 3
    rowItems << getFormGrowth();                    // 4
    rowItems << getFormHarvest();                   // 5
    rowItems << getFormCompetition();               // 6
    rowItems << getFormPredation();                 // 7

    if (m_UsingHPCFiles) {
        QString hpcDir = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataHPCDir));
        DefaultAveragingAlgorithm = Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm().toStdString();
        rowItems << QString::number(nmfUtilsQt::getNumHPCFiles(hpcDir)) + " (HPC)"; // Number of Runs                                                       // 8
        rowItems << QString::fromStdString(DefaultAveragingAlgorithm);
        rowItems << QString::fromStdString(DefaultAveragingAlgorithm);
        rowItems << QString::fromStdString(DefaultAveragingAlgorithm);
        rowItems << QString::fromStdString(DefaultAveragingAlgorithm);
    } else if (isAMultiRun) {
        for (int col=0; col<5; ++col) {
            rowItems << Estimation_Tab7_ptr->getMultiRunColumnData(col);                         // 8-12
        }
    } else {
        rowItems << "1"; // Number of Runs                                                       // 8
        rowItems << QString::fromStdString(Estimation_Tab7_ptr->getCurrentObjectiveCriterion()); // 9
        rowItems << QString::fromStdString(Estimation_Tab7_ptr->getCurrentAlgorithm());          // 10
        rowItems << QString::fromStdString(Estimation_Tab7_ptr->getCurrentMinimizer());          // 11
        rowItems << QString::fromStdString(Estimation_Tab7_ptr->getCurrentScaling());            // 12
    }

    if (m_UsingHPCFiles) {
        rowItems << "";
    } else {
        rowItems << dbSnapshotFile;
    }
    // Next item is for user notes.
    rowItems << "";                                                                              // 13

    // From here on out, the following columns will be hidden
    rowItems << QString::number(Estimation_Tab7_ptr->isSetToDeterministicBees());
    rowItems << QString::number(Estimation_Tab7_ptr->getMaxGenerations());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumBees());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumBestSites());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumEliteSites());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumEliteBees());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumOtherBees());
    rowItems << QString::number(Estimation_Tab7_ptr->getNeighborhoodSize());
    rowItems << QString::number(Estimation_Tab7_ptr->getNumSubRuns());

    rowItems << QString::number(Estimation_Tab7_ptr->isSetToDeterministicMinimizer());
    rowItems << QString::number(Estimation_Tab7_ptr->isStopAfterValue());
    rowItems << QString::number(Estimation_Tab7_ptr->getCurrentToleranceStopValue());
    rowItems << QString::number(Estimation_Tab7_ptr->isStopAfterTime());
    rowItems << QString::number(Estimation_Tab7_ptr->getCurrentStopAfterTime());
    rowItems << QString::number(Estimation_Tab7_ptr->isStopAfterIter());
    rowItems << QString::number(Estimation_Tab7_ptr->getCurrentStopAfterIter());

    rowItems << QString::number(Estimation_Tab7_ptr->isEstInitialBiomassEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstInitialBiomassChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstGrowthRateEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstGrowthRateChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstGrowthRateShapeEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstGrowthRateShapeChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCarryingCapacityEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCarryingCapacityChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCatchabilityEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCatchabilityChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionAlphaEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionAlphaChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaSpeciesChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstCompetitionBetaGuildsGuildsChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationRhoEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationRhoChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationHandlingEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationHandlingChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationExponentEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstPredationExponentChecked());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstSurveyQEnabled());
    rowItems << QString::number(Estimation_Tab7_ptr->isEstSurveyQChecked());

    rowItems << QString::number(isAMultiOrMohnsRhoRun());
    rowItems << Estimation_Tab7_ptr->getEnsembleAveragingAlgorithm();
    rowItems << Estimation_Tab7_ptr->getEnsembleAverageBy();
    rowItems << Estimation_Tab7_ptr->getEnsembleUsingBy();
    rowItems << QString::number(Estimation_Tab7_ptr->getEnsembleUsingAmountValue());
    rowItems << QString::number(Estimation_Tab7_ptr->isEnsembleUsingPct());

    rowItems << QString::fromStdString(Estimation_Tab7_ptr->getEnsembleTimeStampedFilename());
    rowItems << createEstimatedFile(DefaultAveragingAlgorithm);

//  Estimation_Tab7_ptr->updateReviewList(rowItems);
    Estimation_Tab8_ptr->updateModelReviewTable(rowItems);

    if (! m_UsingHPCFiles) {
        m_DatabasePtr->exportDatabase(this, m_ProjectDir, m_Username,
                                      m_Password, m_ProjectDatabase,dbSnapshotFile);
    }
}

void
nmfMainWindow::callback_LoadFromModelReview(nmfStructsQt::ModelReviewStruct modelReview)
{
    // Load snapshoted database
    importDatabase(modelReview.DatabaseSnapshot, nmfConstantsMSSPM::VerboseOff);

    // Save model name and load
    Setup_Tab4_ptr->setModelName(modelReview.ModelName);
    Setup_Tab4_ptr->loadModel();
    callback_ModelSaved();

    // Just need to additionally set:
    // 1. Parameters to be Estimated
    nmfStructsQt::EstimateRunBox runBox;
    std::vector<nmfStructsQt::EstimateRunBox> EstimationRunBoxes;
    runBox.parameter = "InitBiomass";
    runBox.state     = std::make_pair(modelReview.isEstInitialBiomassEnabled == "1",  modelReview.isEstInitialBiomassChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "GrowthRate";
    runBox.state     = std::make_pair(modelReview.isEstGrowthRateEnabled == "1",      modelReview.isEstGrowthRateChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "GrowthRateShape";
    runBox.state     = std::make_pair(modelReview.isEstGrowthRateShapeEnabled == "1", modelReview.isEstGrowthRateShapeChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "CarryingCapacity";
    runBox.state     = std::make_pair(modelReview.isEstCarryingCapacityEnabled == "1",modelReview.isEstCarryingCapacityChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "Catchability";
    runBox.state     = std::make_pair(modelReview.isEstCatchabilityEnabled == "1",    modelReview.isEstCatchabilityChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "CompetitionAlpha";
    runBox.state     = std::make_pair(modelReview.isEstCompetitionAlphaEnabled == "1",modelReview.isEstCompetitionAlphaChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "CompetitionBetaSpeciesSpecies";
    runBox.state     = std::make_pair(modelReview.isEstCompetitionBetaSpeciesEnabled == "1",     modelReview.isEstCompetitionBetaSpeciesChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "CompetitionBetaGuildSpecies";
    runBox.state     = std::make_pair(modelReview.isEstCompetitionBetaGuildsEnabled == "1",      modelReview.isEstCompetitionBetaGuildsChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "CompetitionBetaGuildGuild";
    runBox.state     = std::make_pair(modelReview.isEstCompetitionBetaGuildsGuildsEnabled == "1",modelReview.isEstCompetitionBetaGuildsGuildsChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "PredationRho";
    runBox.state     = std::make_pair(modelReview.isEstPredationRhoEnabled == "1",               modelReview.isEstPredationRhoChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "PredationExponent";
    runBox.state     = std::make_pair(modelReview.isEstPredationExponentEnabled == "1",          modelReview.isEstPredationExponentChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "PredationHandling";
    runBox.state     = std::make_pair(modelReview.isEstPredationHandlingEnabled == "1",          modelReview.isEstPredationHandlingChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    runBox.parameter = "SurveyQ";
    runBox.state     = std::make_pair(modelReview.isEstSurveyQEnabled == "1",                    modelReview.isEstSurveyQChecked == "1");
    EstimationRunBoxes.push_back(runBox);
    Estimation_Tab7_ptr->callback_SetEstimateRunCheckboxes(EstimationRunBoxes);

    // 2. Multi-Run/Ensemble Settings
    bool isAMultiRun = (modelReview.isAMultiRun == "1");
//std::cout << "~~~> isAMultiRun: " << isAMultiRun << std::endl;
    Estimation_Tab7_ptr->enableEnsembleControls(       isAMultiRun);
    Estimation_Tab7_ptr->setEnsembleAveragingAlgorithm(modelReview.ensembleAveragingAlgorithm);
    Estimation_Tab7_ptr->setEnsembleAverageBy(         modelReview.ensembleAverageBy);
    Estimation_Tab7_ptr->setEnsembleUsingBy(           modelReview.ensembleUsingBy);
    Estimation_Tab7_ptr->setEnsembleUsingAmountValue(  modelReview.ensembleUsingAmountValue);
    Estimation_Tab7_ptr->setEnsembleUsingPct(          modelReview.isEnsembleUsingPct == "1");
    if (isAMultiRun) {
        Estimation_Tab7_ptr->loadEnsembleFile(         modelReview.ensembleFilename, nmfConstantsMSSPM::VerboseOn);
    }

    enableRunWidgets(true);

    // 3. Reset Deterministic states
    int stateNLopt = (modelReview.setToDeterministicNLopt == "1") ? Qt::Checked : Qt::Unchecked;
    int stateBees  = (modelReview.setToDeterministicBees  == "1") ? Qt::Checked : Qt::Unchecked;
    Estimation_Tab7_ptr->callback_SetDeterministicCB(stateNLopt);
    Estimation_Tab7_ptr->callback_EnsembleSetDeterministicCB(stateNLopt);
    Estimation_Tab7_ptr->setDeterministicBeesCB(stateBees==Qt::Checked);

    // 4. Reset Model Algorithms
    Estimation_Tab7_ptr->setObjectiveCriterion(modelReview.ObjectiveCriterion);
    Estimation_Tab7_ptr->setAlgorithm(modelReview.EstimationAlgorithm);
    Estimation_Tab7_ptr->setMinimizer(modelReview.MinimizerAlgorithm);
    Estimation_Tab7_ptr->setScaling(modelReview.ScalingAlgorithm);

    // 5. Reset Bees widgets
    Estimation_Tab7_ptr->setMaxGenerations(modelReview.maxGenerations);
    Estimation_Tab7_ptr->setNumBees(modelReview.numBees);
    Estimation_Tab7_ptr->setNumBestSites(modelReview.numBestSites);
    Estimation_Tab7_ptr->setNumEliteSites(modelReview.numEliteSites);
    Estimation_Tab7_ptr->setNumEliteBees(modelReview.numEliteBees);
    Estimation_Tab7_ptr->setNumOtherBees(modelReview.numOtherBees);
    Estimation_Tab7_ptr->setNeighborhoodSize(modelReview.neighborhoodSize);
    Estimation_Tab7_ptr->setNumSubRuns(modelReview.numSubRuns);

    Estimation_Tab7_ptr->loadWidgets();

    // Set to Run Estimation tab
    setPage(nmfConstantsMSSPM::SectionEstimation,
            nmfConstantsMSSPM::PageEstimationRun+1); // +1 needed to get to Run Estimation page, not yet sure why
}

void
nmfMainWindow::updateModelEquationSummary()
{
    bool isAGGPROD      = isAggProd();
    bool isTypeIII      = Setup_Tab4_ptr->isTypeIII();
    QString NewTerm     = "";
    QString ModelType   = "\"Single Species Model\"";
    QString Key         = "";
    QString Equation    = "";
    QString suffix      = " ,<br>where b&#x2081; = b<sub>I</sub>+1, b&#x2082; = b<sub>K</sub>+1";
    QString growth      = getFormGrowth();
    QString predation   = getFormPredation();
    QString harvest     = getFormHarvest();
    QString competition = getFormCompetition();

    // Find the form expressions
    nmfGrowthForm growthForm(growth.toStdString());
    growthForm.setAggProd(isAGGPROD);
    QString growthExpression = QString::fromStdString(growthForm.getExpression());
    QString growthKey        = QString::fromStdString(growthForm.getKey());

    nmfHarvestForm harvestForm(harvest.toStdString());
    harvestForm.setAggProd(isAGGPROD);
    QString harvestExpression = QString::fromStdString(harvestForm.getExpression());
    QString harvestKey        = QString::fromStdString(harvestForm.getKey());

    nmfCompetitionForm competitionForm(competition.toStdString());
    competitionForm.setAggProd(isAGGPROD);
    QString competitionExpression = QString::fromStdString(competitionForm.getExpression());
    QString competitionKey        = QString::fromStdString(competitionForm.getKey());

    nmfPredationForm predationForm(predation.toStdString());
    predationForm.setAggProd(isAGGPROD);
    QString predationExpression = QString::fromStdString(predationForm.getExpression());
    QString predationKey        = QString::fromStdString(predationForm.getKey());

    bool isLightTheme = (qApp->styleSheet().isEmpty());

    // Add highlighting where requested by the appropriated pushbutton being pressed.
    if (Setup_Tab4_ptr->isGrowthFormHighlighted()) {
        if (isLightTheme) {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: yellow\">" + growthExpression + "</FONT>";
        } else {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: blue\">"   + growthExpression + "</FONT>";
        }
    } else {
        NewTerm += growthExpression;
    }

    if (Setup_Tab4_ptr->isHarvestFormHighlighted()) {
        if (isLightTheme) {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: gold\">" + harvestExpression + "</FONT>"; // Gold
        } else {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: #829356\">" + harvestExpression + "</FONT>"; // Green
        }
    } else {
        NewTerm += harvestExpression;
    }

    if (Setup_Tab4_ptr->isCompetitionFormHighlighted()) {
        if (isLightTheme) {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: aqua\">"    + competitionExpression + "</FONT>";
        } else {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: #800080\">" + competitionExpression + "</FONT>"; // Purple
        }
    } else {
        NewTerm += competitionExpression;
    }

    if (Setup_Tab4_ptr->isPredationFormHighlighted()) {
        if (isLightTheme) {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: LightGreen\">" + predationExpression + "</FONT>";
        } else {
            NewTerm += "<FONT style=\"BACKGROUND-COLOR: gray\">" + predationExpression + "</FONT>";
        }
    } else {
        NewTerm += predationExpression;
    }

    Key += growthKey;
    Key += harvestKey;
    Key += competitionKey;
    Key += predationKey;
    if (isAGGPROD) {
        Key += "<br/>Subscripts I, J, and K, where shown, refer to Aggregated Groups (Guilds)<br/>";
    } else {
        Key += "<br/>Subscripts i, j, and k, where shown, refer to Species<br/>";
    }
    Key += "Subscript t refers to Time";

    if (! predationExpression.isEmpty() || ! competitionExpression.isEmpty())
        ModelType = "\"Multi Species Model\"";

    if (NewTerm.isEmpty()) {
        ModelType.clear();
        Equation.clear();
        Key.clear();
    } else {
        Equation  = QString::fromStdString(growthForm.getPrefix());
        Equation += NewTerm;
        if (isAGGPROD && isTypeIII) {
            Equation += suffix;
        }
    }
    Setup_Tab4_ptr->drawEquation(ModelType,Equation,Key);
}

void
nmfMainWindow::callback_LoadSpeciesGuild()
{
    Estimation_Tab1_ptr->setSpeciesGuild(
                Setup_Tab3_ptr->getSpeciesGuild());
}

void
nmfMainWindow::callback_AllowConvergedOnly(bool allowConvergedOnly)
{
    Estimation_Tab7_ptr->setAllowConvergedOnly(allowConvergedOnly);
}
