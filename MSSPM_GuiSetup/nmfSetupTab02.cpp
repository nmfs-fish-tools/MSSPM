
#include "nmfSetupTab02.h"
#include "nmfConstants.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"


nmfSetup_Tab2::nmfSetup_Tab2(QTabWidget* tabs,
                           nmfLogger*    logger,
                           nmfDatabase*  databasePtr)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_Logger      = logger;
    m_DatabasePtr = databasePtr;

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab02.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab2_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab2_Widget->setObjectName("Setup_Tab2_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab2_Widget, tr("2. Project Setup"));

    m_ProjectName.clear();
    m_ProjectDir.clear();
    m_ProjectAuthor.clear();
    m_ProjectDescription.clear();
    m_LastProjectDatabase.clear();
    m_ProjectDatabase.clear();
    m_ValidDatabases = {};
    m_NewProject = false;

    Setup_Tab2_ProjectNameLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectNameLE");
    Setup_Tab2_ProjectDirBrowsePB = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectDirPB");
    Setup_Tab2_ProjectDirLE       = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDirLE");
    Setup_Tab2_ProjectDatabaseCMB = Setup_Tabs->findChild<QComboBox   *>("Setup_Tab2_ProjectDatabaseCMB");
    Setup_Tab2_ProjectAuthorLE    = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectAuthorLE");
    Setup_Tab2_ProjectDescLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDescriptionLE");
    Setup_Tab2_NewDatabasePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_NewDatabasePB");
    Setup_Tab2_ImportDatabasePB   = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_ImportDatabasePB");
    Setup_Tab2_DelDatabasePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_DelDatabasePB");
    Setup_Tab2_RefreshDatabasePB  = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_RefreshDatabasePB");
    Setup_Tab2_SaveProjectPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_SaveProjectPB");
    Setup_Tab2_NewProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_NewProjectPB");
    Setup_Tab2_BrowseProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectPB");
    Setup_Tab2_DelProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_DelProjectPB");
    Setup_Tab2_ReloadProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_ReloadProjectPB");
    Setup_Tab2_ProjectDataGB      = Setup_Tabs->findChild<QGroupBox   *>("Setup_Tab2_ProjectDataGB");

    Setup_Tab2_ProjectNameLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDirLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectAuthorLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDescLE->setClearButtonEnabled(true);

    connect(Setup_Tab2_ProjectDirBrowsePB, SIGNAL(clicked()),
            this,                          SLOT(callback_ProjectDirBrowsePB()));
    connect(Setup_Tab2_DelDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_DelDatabase()));
    connect(Setup_Tab2_SaveProjectPB,      SIGNAL(clicked()),
            this,                          SLOT(callback_SaveProject()));
    connect(Setup_Tab2_NewProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_NewProject()));
    connect(Setup_Tab2_DelProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_DelProject()));
    connect(Setup_Tab2_RefreshDatabasePB,  SIGNAL(clicked()),
            this,                          SLOT(callback_RefreshDatabase()));
    connect(Setup_Tab2_BrowseProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_BrowseProject()));
    connect(Setup_Tab2_ReloadProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_ReloadProject()));
    connect(Setup_Tab2_ProjectNameLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectNameAdd()));
    connect(Setup_Tab2_ProjectAuthorLE,    SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectAuthorAdd()));
    connect(Setup_Tab2_ProjectDescLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectDescAdd()));
    connect(Setup_Tab2_ProjectDirLE,       SIGNAL(returnPressed()),
            this,                          SLOT(callback_ProjectDirAdd()));
    connect(Setup_Tab2_NewDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_NewDatabase()));
    connect(Setup_Tab2_ImportDatabasePB,   SIGNAL(clicked()),
            this,                          SLOT(callback_ImportDatabase()));
    connect(Setup_Tab2_ProjectDatabaseCMB, SIGNAL(currentTextChanged(QString)),
            this,                          SLOT(callback_DatabaseChanged(QString)));

    readSettings();

    loadDatabaseNames("");

}


nmfSetup_Tab2::~nmfSetup_Tab2()
{
}


bool
nmfSetup_Tab2::isProjectDataValid()
{
    return (! (getProjectName().isEmpty()                             ||
               Setup_Tab2_ProjectDirLE->text().isEmpty()              ||
               Setup_Tab2_ProjectDatabaseCMB->currentText().isEmpty() ||
               Setup_Tab2_ProjectAuthorLE->text().isEmpty()           ||
               Setup_Tab2_ProjectDescLE->text().isEmpty()));

}




void
nmfSetup_Tab2::callback_SaveProject()
{
    saveProject(nmfConstantsMSSPM::VerboseOn);
}

void
nmfSetup_Tab2::saveProject(bool isVerbose)
{
    QString fullFilename;
    std::string msg;
    QMessageBox::StandardButton reply;
    QString fileSuffix;
    std::ofstream outFile;

    if (! isProjectDataValid()) {
        if (isVerbose) {
            QMessageBox::warning(Setup_Tabs,
                                 tr("Missing Data"),
                                 tr("\nPlease enter missing data above."),
                                 QMessageBox::Ok);
        }
        return;
    }

    m_ProjectName     = getProjectName();
    m_ProjectDatabase = getProjectDatabase();
    if (! isProjectNameValid(m_ProjectName)) {
        return;
    }

    // Build the filename, add the .prj prefix if one isn't already there and
    // check to see if the file already exists.
    fullFilename = QDir(m_ProjectDir).filePath(m_ProjectName);
    fileSuffix   = QFileInfo(fullFilename).suffix();
    if (fileSuffix != "prj")
        fullFilename += ".prj";
    if (QFileInfo(fullFilename).exists() && isVerbose) {
        msg  = "\nThe project file already exists:\n\n" + fullFilename.toStdString() + "\n\n";
        msg += "OK to overwrite it?\n";
        reply = QMessageBox::question(Setup_Tabs,
                                      tr("File Found"),
                                      tr(msg.c_str()),
                                      QMessageBox::Yes|QMessageBox::No,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Write out the project file
    outFile = std::ofstream(fullFilename.toLatin1());
    outFile << "#\n";
    outFile << "# This is an MSSPM Project file.\n";
    outFile << "#\n";
    outFile << "# It defines the Project database as well as other\n";
    outFile << "# information for the Project. The following data are:\n";
    outFile << "# Project directory, Project database, Author, and Description.\n";
    outFile << "#\n";
    outFile << m_ProjectDir.toStdString() << "\n";
    outFile << m_ProjectDatabase.toStdString() << "\n";
    outFile << m_ProjectAuthor.toStdString() << "\n";
    outFile << m_ProjectDescription.toStdString() << "\n";
    outFile.close();

    m_LastProjectDatabase = m_ProjectDatabase;

    emit SaveMainSettings();

    emit ProjectSaved();

    // Make sure all the necessary directories are created
    QDir dir;
    QString outputImagePath  = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDir));
    QString outputDataPath   = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataDir));
    QString inputDataPath    = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString layoutDataPath   = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::LayoutDir));
    QString databaseDataPath = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::DatabaseDir));
    dir.mkpath(outputImagePath);
    dir.mkpath(outputDataPath);
    dir.mkpath(inputDataPath);
    dir.mkpath(layoutDataPath);
    dir.mkpath(databaseDataPath);

    // Load the project
    QString fileName = QDir(m_ProjectDir).filePath(m_ProjectName+".prj");
    loadProject(m_Logger,fileName,nmfConstantsMSSPM::DontClearModelName);

    QApplication::restoreOverrideCursor();

    if (isVerbose) {
        emit DisableNavigatorUntilModelLoad();
        msg = "\nSaved Project: " + m_ProjectName.toStdString() + "\n";
        msg += "\nPlease load a model.\n";
        QMessageBox::information(Setup_Tabs, "Project Saved", tr(msg.c_str()));
    }
}

void
nmfSetup_Tab2::callback_progressDlgCancel()
{
    disconnect(m_ProgressDlg, SIGNAL(canceled()),
               this,          SLOT(callback_progressDlgCancel()));
}


bool
nmfSetup_Tab2::isProjectNameValid(QString projectName)
{
    std::string msg;
    bool isValid = false;
    bool isAtLeastOneChar = (! projectName.isEmpty());

    // Use regex to validate name
    QRegularExpression re("^[a-zA-Z0-9_]*$");
    QRegularExpressionMatch match = re.match(m_ProjectName);
    isValid = match.hasMatch();

    if ((! isAtLeastOneChar) || (! isValid)) {
        msg = "Invalid project name: " + projectName.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        msg += "\n\nA project name must contain at least one character and consist";
        msg += "\nonly of letters, numbers, and underscore characters.";
        QMessageBox::warning(Setup_Tabs,"Invalid Project Name",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
    }

    return (isAtLeastOneChar && isValid);
}

void
nmfSetup_Tab2::clearProject()
{
    setProjectAuthor("");
    setProjectDescription("");
    setProjectDirectory("");
    setProjectName("");
}


void
nmfSetup_Tab2::createTables(QString databaseName)
{
    int pInc = 0;
    std::string fullTableName;
    std::string cmd;
    std::string msg;
    std::string errorMsg;
    std::string db = databaseName.toStdString();
    bool okToCreateMoreTables = true;
    std::vector<std::string> ExistingTableNames;

    m_ProgressDlg = new QProgressDialog("\nCreating Tables...\n",
                                      "Cancel", 0, 35, Setup_Tabs);
    m_ProgressDlg->setWindowModality(Qt::WindowModal);
    m_ProgressDlg->setValue(pInc);
    m_ProgressDlg->setRange(0,87);
    m_ProgressDlg->show();
    connect(m_ProgressDlg, SIGNAL(canceled()),
            this,          SLOT(callback_progressDlgCancel()));

    // 1 of 87: BetweenGuildsInteractionCoeff
    std::string tableName = nmfConstantsMSSPM::TableBetweenGuildsInteractionCoeff;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName  varchar(50) NOT NULL,";
    cmd += " ModelName    varchar(50) NOT NULL,";
    cmd += " GuildA       varchar(50) NOT NULL,";
    cmd += " GuildB       varchar(50) NOT NULL,";
    cmd += " Value        int(11) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,GuildA,GuildB))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 2 of 87: CompetitionAlpha
    // 3 of 87: CompetitionAlphaMax
    // 4 of 87: CompetitionAlphaMin
    for (std::string tableName : {nmfConstantsMSSPM::TableCompetitionAlpha,
                                  nmfConstantsMSSPM::TableCompetitionAlphaMax,
                                  nmfConstantsMSSPM::TableCompetitionAlphaMin})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName    varchar(50) NOT NULL,";
        cmd += " ModelName      varchar(50) NOT NULL,";
        cmd += " SpeciesA       varchar(50) NOT NULL,";
        cmd += " SpeciesB       varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 2] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }


    // 5 of 87: CompetitionBetaSpecies
    // 6 of 87: CompetitionBetaSpeciesMax
    // 7 of 87: CompetitionBetaSpeciesMin
    for (std::string tableName : {nmfConstantsMSSPM::TableCompetitionBetaSpecies,
                                  nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax,
                                  nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName    varchar(50) NOT NULL,";
        cmd += " ModelName      varchar(50) NOT NULL,";
        cmd += " SpeciesA       varchar(50) NOT NULL,";
        cmd += " SpeciesB       varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 3] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 8  of 87: CompetitionBetaGuilds
    // 9  of 87: CompetitionBetaGuildsMax
    // 10 of 87: CompetitionBetaGuildsMin
    for (std::string tableName : {nmfConstantsMSSPM::TableCompetitionBetaGuilds,
                                  nmfConstantsMSSPM::TableCompetitionBetaGuildsMax,
                                  nmfConstantsMSSPM::TableCompetitionBetaGuildsMin})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName    varchar(50) NOT NULL,";
        cmd += " ModelName      varchar(50) NOT NULL,";
        cmd += " Guild          varchar(50) NOT NULL,";
        cmd += " SpeName        varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Guild,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 4] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 11 of 87: CompetitionBetaGuildsGuilds
    // 12 of 87: CompetitionBetaGuildsGuildsMax
    // 13 of 87: CompetitionBetaGuildsGuildsMin
    for (std::string tableName : {nmfConstantsMSSPM::TableCompetitionBetaGuildsGuilds,
                                  nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMax,
                                  nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMin})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName    varchar(50) NOT NULL,";
        cmd += " ModelName      varchar(50) NOT NULL,";
        cmd += " GuildA         varchar(50) NOT NULL,";
        cmd += " GuildB         varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,GuildA,GuildB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 5] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 14 of 87: PredationExponent
    // 15 of 87: PredationExponentMin
    // 16 of 87: PredationExponentMax
    for (std::string tableName : {nmfConstantsMSSPM::TablePredationExponent,
                                  nmfConstantsMSSPM::TablePredationExponentMin,
                                  nmfConstantsMSSPM::TablePredationExponentMax})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName varchar(50) NOT NULL,";
        cmd += " ModelName   varchar(50) NOT NULL,";
        cmd += " SpeName     varchar(50) NOT NULL,";
        cmd += " Value       float NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 17 of 87: Catch
    //     18 of 87: CatchFTC
    // 18 of 87: Effort
    //     20 of 87: EffortFTC
    // 19 of 87: Exploitation
    // 20 of 87: BiomassAbsolute
    // 21 of 87: BiomassRelative
    // 22 of 87: BiomassRelativeDividedByEstSurveyQ
    for (std::string tableName : {nmfConstantsMSSPM::TableHarvestCatch,
//                                nmfConstantsMSSPM::TableHarvestCatchFTC,
                                  nmfConstantsMSSPM::TableHarvestEffort,
//                                nmfConstantsMSSPM::TableHarvestEffortFTC,
                                  nmfConstantsMSSPM::TableHarvestExploitation,
                                  nmfConstantsMSSPM::TableBiomassAbsolute,
                                  nmfConstantsMSSPM::TableBiomassRelative,
                                  nmfConstantsMSSPM::TableBiomassRelativeDividedByEstSurveyQ})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName   varchar(50) NOT NULL,";
        cmd += " ModelName     varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Value         double NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 23 of 87: BiomassRelativeScalar
    tableName = nmfConstantsMSSPM::TableBiomassRelativeScalars;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName   varchar(50) NOT NULL,";
    cmd += " ModelName     varchar(50) NOT NULL,";
    cmd += " SpeName       varchar(50) NOT NULL,";
    cmd += " Type          varchar(50) NOT NULL,";
    cmd += " Value         float NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName,Type))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 7] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 24 of 87: Covariate
    tableName = nmfConstantsMSSPM::TableCovariate;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName   varchar(50) NOT NULL,";
    cmd += " ModelName     varchar(50) NOT NULL,";
    cmd += " CovariateName varchar(50) NOT NULL,";
    cmd += " Year          int(11)     NOT NULL,";
    cmd += " Value         varchar(50) NULL,";
    cmd += " ValueScaled   varchar(50) NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,CovariateName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 9a] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 25 of 87: CovariateAssignment
    tableName = nmfConstantsMSSPM::TableCovariateAssignment;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName     varchar(50) NOT NULL,";
    cmd += " ModelName       varchar(50) NOT NULL,";
    cmd += " SpeName         varchar(50) NOT NULL,";
    cmd += " ParameterName   varchar(50) NOT NULL,";
    cmd += " CovariateName   varchar(50) NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName,ParameterName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 9b] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 26 of 87: CovariateInitialValuesAndRanges
    tableName = nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName     varchar(50) NOT NULL,";
    cmd += " ModelName       varchar(50) NOT NULL,";
    cmd += " SpeName         varchar(50) NOT NULL,";
    cmd += " CoeffName       varchar(50) NOT NULL,";
    cmd += " CoeffMinName    varchar(50) NOT NULL,";
    cmd += " CoeffMaxName    varchar(50) NOT NULL,";
    cmd += " CoeffValue      varchar(50) NULL,";
    cmd += " CoeffMinValue   varchar(50) NULL,";
    cmd += " CoeffMaxValue   varchar(50) NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName,CoeffName,CoeffMinName,CoeffMaxName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 9c] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 27 of 87: Guilds
    tableName = nmfConstantsMSSPM::TableGuilds;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(GuildName       varchar(100) NOT NULL,";
    cmd += " GrowthRate      float NULL,";
    cmd += " GrowthRateMin   float NULL,";
    cmd += " GrowthRateMax   float NULL,";
    cmd += " GuildK          double NULL,";
    cmd += " GuildKMin       double NULL,";
    cmd += " GuildKMax       double NULL,";
    cmd += " Catchability    float NULL,";
    cmd += " CatchabilityMin float NULL,";
    cmd += " CatchabilityMax float NULL,";
    cmd += " PRIMARY KEY (GuildName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 10] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 28 of 87: OutputBiomass
    tableName = nmfConstantsMSSPM::TableOutputBiomass;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50)    NOT NULL,";
    cmd += " ModelName           varchar(50)    NOT NULL,";
    cmd += " Algorithm           varchar(50)    NOT NULL,";
    cmd += " Minimizer           varchar(50)    NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
    cmd += " Scaling             varchar(50)    NOT NULL,";
    cmd += " isAggProd           int(11)        NOT NULL,";
    cmd += " SpeName             varchar(50)    NOT NULL,";
    cmd += " Year                int(11)        NOT NULL,";
    cmd += " Value               DECIMAL(20,10) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 11] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 29 of 87: OutputCompetitionAlpha
    // 30 of 87: OutputCompetitionBetaSpecies
    // 31 of 87: OutputPredationRho
    // 32 of 87: OutputPredationHandling
    for (std::string tableName : {nmfConstantsMSSPM::TableOutputCompetitionAlpha,
                                  nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies,
                                  nmfConstantsMSSPM::TableOutputPredationRho,
                                  nmfConstantsMSSPM::TableOutputPredationHandling})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50)    NOT NULL,";
        cmd += " ModelName           varchar(50)    NOT NULL,";
        cmd += " Algorithm           varchar(50)    NOT NULL,";
        cmd += " Minimizer           varchar(50)    NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
        cmd += " Scaling             varchar(50)    NOT NULL,";
        cmd += " isAggProd           int(11)        NOT NULL,";
        cmd += " SpeciesA            varchar(50)    NOT NULL,";
        cmd += " SpeciesB            varchar(50)    NOT NULL,";
        cmd += " Value               DECIMAL(20,10) NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 12] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 33 of 87: OutputCompetitionBetaGuilds
    for (std::string tableName : {nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50)    NOT NULL,";
        cmd += " ModelName           varchar(50)    NOT NULL,";
        cmd += " Algorithm           varchar(50)    NOT NULL,";
        cmd += " Minimizer           varchar(50)    NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
        cmd += " Scaling             varchar(50)    NOT NULL,";
        cmd += " isAggProd           int(11)        NOT NULL,";
        cmd += " SpeName             varchar(50)    NOT NULL,";
        cmd += " Guild               varchar(50)    NOT NULL,";
        cmd += " Value               DECIMAL(20,10) NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 13] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 34 of 87: OutputCompetitionBetaGuildsGuilds
    for (std::string tableName : {nmfConstantsMSSPM::TableOutputCompetitionBetaGuildsGuilds})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50)    NOT NULL,";
        cmd += " ModelName           varchar(50)    NOT NULL,";
        cmd += " Algorithm           varchar(50)    NOT NULL,";
        cmd += " Minimizer           varchar(50)    NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
        cmd += " Scaling             varchar(50)    NOT NULL,";
        cmd += " isAggProd           int(11)        NOT NULL,";
        cmd += " GuildA              varchar(50)    NOT NULL,";
        cmd += " GuildB              varchar(50)    NOT NULL,";
        cmd += " Value               DECIMAL(20,10) NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,GuildA,GuildB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 13.1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 35 of 87: OutputCatchability
    // 36 of 87: OutputCatchabilityCovariateCoeffs
    // 37 of 87: OutputGrowthRate
    // 38 of 87: OutputGrowthRateShape
    // 39 of 87: OutputGrowthRateCovariateCoeffs
    // 40 of 87: OutputCarryingCapacity
    // 41 of 87: OutputCarryingCapacityCovariateCoeffs
    // 42 of 87: OutputSurveyQ
    // 43 of 87: OutputSurveyQCovariateCoeffs
    // 44 of 87: OutputPredationExponent
    // 45 of 87: OutputMSY
    // 46 of 87: OutputMSYBiomass
    // 47 of 87: OutputMSYFishing
    // 48 of 87: OutputInitBiomass
    for (std::string tableName : {nmfConstantsMSSPM::TableOutputCatchability,
                                  nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs,
                                  nmfConstantsMSSPM::TableOutputGrowthRate,
                                  nmfConstantsMSSPM::TableOutputGrowthRateShape,
                                  nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs,
                                  nmfConstantsMSSPM::TableOutputCarryingCapacity,
                                  nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs,
                                  nmfConstantsMSSPM::TableOutputSurveyQ,
                                  nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs,
                                  nmfConstantsMSSPM::TableOutputPredationExponent,
                                  nmfConstantsMSSPM::TableOutputMSY,
                                  nmfConstantsMSSPM::TableOutputMSYBiomass,
                                  nmfConstantsMSSPM::TableOutputMSYFishing,
                                  nmfConstantsMSSPM::TableOutputInitBiomass})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50)    NOT NULL,";
        cmd += " ModelName           varchar(50)    NOT NULL,";
        cmd += " Algorithm           varchar(50)    NOT NULL,";
        cmd += " Minimizer           varchar(50)    NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
        cmd += " Scaling             varchar(50)    NOT NULL,";
        cmd += " isAggProd           int(11)        NOT NULL,";
        cmd += " SpeName             varchar(50)    NOT NULL,";
        cmd += " Value               DECIMAL(20,10) NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 14] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 49 of 87: SpatialOverlap
    for (std::string tableName : {nmfConstantsMSSPM::TableSpatialOverlap})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50) NOT NULL,";
        cmd += " ModelName           varchar(50) NOT NULL,";
        cmd += " SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 15] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }


    // 50 of 87: PredationHandling
    // 51 of 87: PredationHandlingMin
    // 52 of 87: PredationHandlingMax
    // 53 of 87: PredationRho
    // 54 of 87: PredationRhoMax
    // 55 of 87: PredationRhoMin
    // xx of 87: TestCompetition
    for (std::string tableName : {nmfConstantsMSSPM::TablePredationHandling,
                                  nmfConstantsMSSPM::TablePredationHandlingMin,
                                  nmfConstantsMSSPM::TablePredationHandlingMax,
                                  nmfConstantsMSSPM::TablePredationRho,
                                  nmfConstantsMSSPM::TablePredationRhoMax,
                                  nmfConstantsMSSPM::TablePredationRhoMin})
//                                nmfConstantsMSSPM::TableTestCompetition})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName         varchar(50) NOT NULL,";
        cmd += " ModelName           varchar(50) NOT NULL,";
        cmd += " SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               float       NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 16] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 56 of 87: Species
    tableName = nmfConstantsMSSPM::TableSpecies;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(SpeName              varchar(100) NOT NULL,";
    cmd += " GuildName            varchar(100) NULL,";
    cmd += " InitBiomass          double NULL,";
    cmd += " SurveyQ              float  NULL,";
    cmd += " SpeciesK             double NULL,";
    cmd += " SpeciesKCovarCoeff   float  NULL,";
    cmd += " GrowthRate           float  NULL,";
    cmd += " GrowthRateShape      float  NULL,";
    cmd += " GrowthRateCovarCoeff float  NULL,";
    cmd += " Habitat              varchar(50)  NULL,";
    cmd += " Catchability         float  NULL,";
    cmd += " InitBiomassMin       double NULL,";
    cmd += " InitBiomassMax       double NULL,";
    cmd += " SurveyQMin           float  NULL,";
    cmd += " SurveyQMax           float  NULL,";
    cmd += " GrowthRateMin        float  NULL,";
    cmd += " GrowthRateMax        float  NULL,";
    cmd += " GrowthRateShapeMin   float  NULL,";
    cmd += " GrowthRateShapeMax   float  NULL,";
    cmd += " SpeciesKMin          double NULL,";
    cmd += " SpeciesKMax          double NULL,";
    cmd += " CatchabilityMin      float  NULL,";
    cmd += " CatchabilityMax      float  NULL,";
    cmd += " SpeDependence        float  NULL,";
    cmd += " ExploitationRate     float  NULL,";
    cmd += " Weight               float  NULL,";
    cmd += " PRIMARY KEY (SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 18] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 57 of 87: Forecasts
    tableName = nmfConstantsMSSPM::TableForecasts;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50) NOT NULL,";
    cmd += " ModelName           varchar(50) NOT NULL,";
    cmd += " ForecastName        varchar(50) NOT NULL,";
    cmd += " PreviousRun         int(11)     NULL,";
    cmd += " Algorithm           varchar(50) NULL,";
    cmd += " Minimizer           varchar(50) NULL,";
    cmd += " ObjectiveCriterion  varchar(50) NULL,";
    cmd += " Scaling             varchar(50) NULL,";
    cmd += " GrowthForm          varchar(50) NULL,";
    cmd += " HarvestForm         varchar(50) NULL,";
    cmd += " WithinGuildCompetitionForm varchar(50) NULL,";
    cmd += " PredationForm       varchar(50) NULL,";
    cmd += " ForecastHarvestType varchar(50) NULL,";
    cmd += " RunLength           int(11)     NULL,";
    cmd += " StartYear           int(11)     NULL,";
    cmd += " EndYear             int(11)     NULL,";
    cmd += " NumRuns             int(11)     NULL,";
    cmd += " IsDeterministic     int(11)     NULL,";
    cmd += " Seed                int(11)     NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 19] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 58 of 87: ForecastHarvestExploitation
    // 59 of 87: ForecastHarvestEffort
    // 60 of 87: ForecastHarvestEffortFTC
    // 61 of 87: ForecastHarvestCatch
    // 62 of 87: ForecastHarvestCatchFTC
    for (std::string tableName : {nmfConstantsMSSPM::TableForecastHarvestExploitation,
                                  nmfConstantsMSSPM::TableForecastHarvestEffort,
                                  nmfConstantsMSSPM::TableForecastHarvestEffortFTC,
                                  nmfConstantsMSSPM::TableForecastHarvestCatch,
                                  nmfConstantsMSSPM::TableForecastHarvestCatchFTC})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName        varchar(50) NOT NULL,";
        cmd += " ModelName          varchar(50) NOT NULL,";
        cmd += " ForecastName       varchar(50) NOT NULL,";
        cmd += " Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " Year               int(11)     NOT NULL,";
        cmd += " Value              double      NOT NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 20] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 63 of 87: ForecastBiomass
    tableName = nmfConstantsMSSPM::TableForecastBiomass;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName        varchar(50) NOT NULL,";
    cmd += " ModelName          varchar(50) NOT NULL,";
    cmd += " ForecastName       varchar(50) NOT NULL,";
    cmd += " Algorithm          varchar(50) NOT NULL,";
    cmd += " Minimizer          varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling            varchar(50) NOT NULL,";
    cmd += " isAggProd          int(11)     NOT NULL,";
    cmd += " SpeName            varchar(50) NOT NULL,";
    cmd += " Year               int(11)     NOT NULL,";
    cmd += " Value              float       NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 22] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 64 of 87: ForecastBiomassMonteCarlo
    tableName = nmfConstantsMSSPM::TableForecastBiomassMonteCarlo;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName  varchar(50) NOT NULL,";
    cmd += " ModelName    varchar(50) NOT NULL,";
    cmd += " ForecastName varchar(50) NOT NULL,";
    cmd += " RunNum       int(11)     NOT NULL,";
    cmd += " Algorithm    varchar(50) NOT NULL,";
    cmd += " Minimizer    varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling      varchar(50) NOT NULL,";
    cmd += " isAggProd    int(11)     NOT NULL,";
    cmd += " SpeName      varchar(50) NOT NULL,";
    cmd += " Year         int(11)     NOT NULL,";
    cmd += " Value        float       NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName,RunNum,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 21] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 65 of 87: ForecastMonteCarloParameters
    tableName = nmfConstantsMSSPM::TableForecastMonteCarloParameters;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName                 varchar(50) NOT NULL,";
    cmd += " ModelName                   varchar(50) NOT NULL,";
    cmd += " ForecastName                varchar(50) NOT NULL,";
    cmd += " RunNum                      int(11)     NOT NULL,";
    cmd += " Algorithm                   varchar(50) NOT NULL,";
    cmd += " Minimizer                   varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion          varchar(50) NOT NULL,";
    cmd += " Scaling                     varchar(50) NOT NULL,";
    cmd += " SpeName                     varchar(50) NOT NULL,";
    cmd += " GrowthRate                  double      NULL,";
    cmd += " GrowthRateShape             double      NULL,";
    cmd += " CarryingCapacity            double      NULL,";
    cmd += " Catchability                double      NULL,";
    cmd += " Exponent                    double      NULL,";
    cmd += " CompetitionAlpha            double      NULL,";
    cmd += " CompetitionBetaSpecies      double      NULL,";
    cmd += " CompetitionBetaGuilds       double      NULL,";
    cmd += " CompetitionBetaGuildsGuilds double      NULL,";
    cmd += " Predation                   double      NULL,";
    cmd += " Handling                    double      NULL,";
    cmd += " Harvest                     double      NULL,";
    cmd += " GrowthRateCovCoeff          double      NULL,";
    cmd += " CarryingCapacityCovCoeff    double      NULL,";
    cmd += " CatchabilityCovCoeff        double      NULL,";
    cmd += " SurveyQCovCoeff             double      NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName,RunNum,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 21.1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 66 of 87: ForecastBiomassMultiScenario
    tableName = nmfConstantsMSSPM::TableForecastBiomassMultiScenario;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName   varchar(50) NOT NULL,";
    cmd += " ModelName     varchar(50) NOT NULL,";
    cmd += " ScenarioName  varchar(50) NOT NULL,";
    cmd += " SortOrder     int(11)     NOT NULL,"; // Because user may not want default sort order for ForecastLabel
    cmd += " ForecastLabel varchar(50) NOT NULL,";
    cmd += " SpeName       varchar(50) NOT NULL,";
    cmd += " Year          int(11)     NOT NULL,";
    cmd += " Value         float       NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ScenarioName,SortOrder,ForecastLabel,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 22] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 67 of 87: ForecastUncertainty
    tableName = nmfConstantsMSSPM::TableForecastUncertainty;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName                 varchar(50) NOT NULL,";
    cmd += " ModelName                   varchar(50) NOT NULL,";
    cmd += " ForecastName                varchar(50) NOT NULL,";
    cmd += " SpeName                     varchar(50) NOT NULL,";
    cmd += " Algorithm                   varchar(50) NOT NULL,";
    cmd += " Minimizer                   varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion          varchar(50) NOT NULL,";
    cmd += " Scaling                     varchar(50) NOT NULL,";
    cmd += " InitBiomass                 float       NOT NULL,";
    cmd += " GrowthRate                  float       NOT NULL,";
    cmd += " GrowthRateShape             float       NOT NULL,";
    cmd += " CarryingCapacity            float       NOT NULL,";
    cmd += " Catchability                float       NOT NULL,";
    cmd += " CompetitionAlpha            float       NOT NULL,";
    cmd += " CompetitionBetaSpecies      float       NOT NULL,";
    cmd += " CompetitionBetaGuilds       float       NOT NULL,";
    cmd += " CompetitionBetaGuildsGuilds float       NOT NULL,";
    cmd += " PredationRho                float       NOT NULL,";
    cmd += " PredationHandling           float       NOT NULL,";
    cmd += " PredationExponent           float       NOT NULL,";
    cmd += " SurveyQ                     float       NOT NULL,";
    cmd += " Harvest                     float       NOT NULL,";
    cmd += " GrowthRateCovCoeff          float       NOT NULL,";
    cmd += " CarryingCapacityCovCoeff    float       NOT NULL,";
    cmd += " CatchabilityCovCoeff        float       NOT NULL,";
    cmd += " SurveyQCovCoeff             float       NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,ForecastName,SpeName,Algorithm,Minimizer,ObjectiveCriterion,Scaling))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 23] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 68 of 87: DiagnosticInitBiomass
    // 69 of 87: DiagnosticGrowthRate
    // 70 of 87: DiagnosticGrowthRateShape
    // 71 of 87: DiagnosticCarryingCapacity
    // 72 of 87: DiagnosticCatchability
    // 73 of 87: DiagnosticSurveyQ
    // 74 of 87: DiagnosticGrowthRateCovCoeff
    // 75 of 87: DiagnosticCarryingCapacityCovCoeff
    // 76 of 87: DiagnosticCatchabilityCovCoeff
    // 77 of 87: DiagnosticSurveyQCovCoeff
    for (std::string tableName : {nmfConstantsMSSPM::TableDiagnosticInitBiomass,
                                  nmfConstantsMSSPM::TableDiagnosticGrowthRate,
                                  nmfConstantsMSSPM::TableDiagnosticGrowthRateShape,
                                  nmfConstantsMSSPM::TableDiagnosticCarryingCapacity,
                                  nmfConstantsMSSPM::TableDiagnosticCatchability,
                                  nmfConstantsMSSPM::TableDiagnosticSurveyQ,
                                  nmfConstantsMSSPM::TableDiagnosticGrowthRateCovCoeff,
                                  nmfConstantsMSSPM::TableDiagnosticCarryingCapacityCovCoeff,
                                  nmfConstantsMSSPM::TableDiagnosticCatchabilityCovCoeff,
                                  nmfConstantsMSSPM::TableDiagnosticSurveyQCovCoeff})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName        varchar(50) NOT NULL,";
        cmd += " ModelName          varchar(50) NOT NULL,";
        cmd += " Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " isAggProd          int(11)     NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " Offset             double      NOT NULL,";
        cmd += " Value              double      NOT NULL,";
        cmd += " Fitness            double      NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Offset,Value))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 24] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 78 of 87: DiagnosticSurface
    for (std::string tableName : {nmfConstantsMSSPM::TableDiagnosticSurface})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName        varchar(50) NOT NULL,";
        cmd += " ModelName          varchar(50) NOT NULL,";
        cmd += " Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " isAggProd          int(11)     NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " Type               varchar(50) NOT NULL,";
        cmd += " parameter1PctVar   double      NOT NULL,";
        cmd += " parameter2PctVar   double      NOT NULL,";
        cmd += " Fitness            double      NULL,";
        cmd += " PRIMARY KEY (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Type,parameter1PctVar,parameter2PctVar))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 25] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 79 of 87: Models
    for (std::string tableName : {nmfConstantsMSSPM::TableModels})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ProjectName                     varchar(50)  NOT NULL,";
        cmd += " ModelName                       varchar(50)  NOT NULL,";
        cmd += " CarryingCapacity                double       NULL,";
        cmd += " ObsBiomassType                  varchar(50)  NULL,";
        cmd += " Units                           varchar(50)  NULL,";
        cmd += " GrowthForm                      varchar(25)  NULL,";
        cmd += " PredationForm                   varchar(25)  NULL,";
        cmd += " HarvestForm                     varchar(25)  NULL,";
        cmd += " WithinGuildCompetitionForm      varchar(25)  NULL,";
        cmd += " BetweenGuildCompetitionForm     varchar(25)  NULL,";
        cmd += " CovariateAlgorithmType          varchar(25)  NULL,";
        cmd += " NumberOfParameters              int(11)      NULL,";
        cmd += " StochasticGrowth                varchar(25)  NULL,";
        cmd += " StochasticCatch                 varchar(25)  NULL,";
        cmd += " AssessmentType                  varchar(50)  NULL,";
        cmd += " CVAssessment                    float        NULL,";
        cmd += " IndicatorSpecies                int(11)      NULL,";
        cmd += " ACLBuffer                       float        NULL,";
        cmd += " AssessmentInterval              int(11)      NULL,";
        cmd += " NumberOfRuns                    int(11)      NULL,";
        cmd += " HistoricalEnd                   int(11)      NULL,";
        cmd += " StartYear                       int(11)      NULL,";
        cmd += " RunLength                       int(11)      NULL,";
        cmd += " TimeStep                        float        NULL,";
        cmd += " Algorithm                       varchar(50)  NULL,";
        cmd += " Minimizer                       varchar(50)  NULL,";
        cmd += " ObjectiveCriterion              varchar(50)  NULL,";
        cmd += " Scaling                         varchar(50)  NULL,";
        cmd += " Seed                            int(11)      NULL,";
        cmd += " UseFixedSeedBees                int(11)      NULL,";
        cmd += " UseFixedSeedMinimizer           int(11)      NULL,";
        cmd += " MinimizerType                   varchar(50)  NULL,";
        cmd += " LogScale                        int(11)      NULL,";
        cmd += " GAGenerations                   int(11)      NULL,";
        cmd += " GAPopulationSize                int(11)      NULL,";
        cmd += " GAMutationRate                  float        NULL,";
        cmd += " GAConvergence                   int(11)      NULL,";
        cmd += " BeesNumTotal                    int(11)      NULL,";
        cmd += " BeesNumElite                    int(11)      NULL,";
        cmd += " BeesNumOther                    int(11)      NULL,";
        cmd += " BeesNumEliteSites               int(11)      NULL,";
        cmd += " BeesNumBestSites                int(11)      NULL,";
        cmd += " BeesNumRepetitions              int(11)      NULL,";
        cmd += " BeesMaxGenerations              int(11)      NULL,";
        cmd += " BeesNeighborhoodSize            float        NULL,";
        cmd += " GradMaxIterations               int(11)      NULL,";
        cmd += " GradMaxLineSearches             int(11)      NULL,";
        cmd += " NLoptNumberOfRuns               int(11)      NULL,";
        cmd += " NLoptUseStopVal                 int(11)      NULL,";
        cmd += " NLoptUseStopAfterTime           int(11)      NULL,";
        cmd += " NLoptUseStopAfterIter           int(11)      NULL,";
        cmd += " NLoptUseInitialPopulationSize   int(11)      NULL,";
        cmd += " NLoptStopVal                    double       NULL,";
        cmd += " NLoptStopAfterTime              int(11)      NULL,";
        cmd += " NLoptStopAfterIter              int(11)      NULL,";
        cmd += " NLoptInitialPopulationSize      double       NULL,";

        cmd += " EstimateInitialBiomass          int(11)      NULL,";
        cmd += " EstimateGrowthRate              int(11)      NULL,";
        cmd += " EstimateGrowthRateShape         int(11)      NULL,";
        cmd += " EstimateCarryingCapacity        int(11)      NULL,";
        cmd += " EstimateCatchability            int(11)      NULL,";
        cmd += " EstimateCompetition             int(11)      NULL,";
        cmd += " EstimateCompetitionSpecies      int(11)      NULL,";
        cmd += " EstimateCompetitionGuilds       int(11)      NULL,";
        cmd += " EstimateCompetitionGuildsGuilds int(11)      NULL,";
        cmd += " EstimatePredation               int(11)      NULL,";
        cmd += " EstimatePredationHandling       int(11)      NULL,";
        cmd += " EstimatePredationExponent       int(11)      NULL,";
        cmd += " EstimateSurveyQ                 int(11)      NULL,";

        cmd += " EnsembleIsBoxChecked            int(11)      NULL,";
        cmd += " EnsembleAverageAlg              varchar(20)  NULL,";
        cmd += " EnsembleAverageBy               varchar(20)  NULL,";
        cmd += " EnsembleUsingWhat               varchar(20)  NULL,";
        cmd += " EnsembleUsingValue              int(11)      NULL,";
        cmd += " EnsembleIsUsingPct              int(11)      NULL,";
        cmd += " EnsembleRandInitParam           varchar(25)  NULL,";
        cmd += " EnsembleRangeJitter             double       NULL,";
        cmd += " EnsembleRangeJitterRepeatable   int(11)      NULL,";
        cmd += " EnsembleFile                    varchar(50)  NULL,";
      //cmd += " EnsembleParametersFile          varchar(50)  NULL,";

        cmd += " PRIMARY KEY (ProjectName,ModelName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 27] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
    }

    // 80 of 87: Application (contains name of application - used to assure app is using correct database)
    for (std::string tableName : {nmfConstantsMSSPM::TableApplication})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(Name varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (Name))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 28] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
        }
        if (! okToCreateMoreTables) {
            m_ProgressDlg->close();
            return;
        }
        m_DatabasePtr->saveApplicationTable(Setup_Tabs,m_Logger,fullTableName);
    }

    // 81 of 87: OutputBiomassEnsemble
    tableName = nmfConstantsMSSPM::TableOutputBiomassEnsemble;
    fullTableName = db + "." + tableName;
    ExistingTableNames .push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50)    NOT NULL,";
    cmd += " ModelName           varchar(50)    NOT NULL,";
    cmd += " Label               varchar(50)    NOT NULL,";
    cmd += " RunNumber           int(11)        NOT NULL,";
    cmd += " Algorithm           varchar(50)    NOT NULL,";
    cmd += " Minimizer           varchar(50)    NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
    cmd += " Scaling             varchar(50)    NOT NULL,";
    cmd += " isAggProd           int(11)        NOT NULL,";
    cmd += " SpeName             varchar(50)    NOT NULL,";
    cmd += " Year                int(11)        NOT NULL,";
    cmd += " Value               DECIMAL(20,10) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,Label,RunNumber,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 29] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 82 of 87: ModelReview
    tableName = nmfConstantsMSSPM::TableModelReview;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName                      varchar(50)  NOT NULL,";
    cmd += " ModelNumber                      varchar(10)  NOT NULL,";
    cmd += " ModelName                        varchar(50)  NOT NULL,";
    cmd += " rSquared                         double       NOT NULL,";
    cmd += " SSResiduals                      double       NOT NULL,";
    cmd += " AIC                              double       NOT NULL,";
    cmd += " GrowthForm                       varchar(30)  NOT NULL,";
    cmd += " HarvestForm                      varchar(30)  NOT NULL,";
    cmd += " CompetitonForm                   varchar(30)  NOT NULL,";
    cmd += " PredationForm                    varchar(30)  NOT NULL,";
    cmd += " numRuns                          varchar(20)  NOT NULL,";
    cmd += " ObjectiveCriterion               varchar(60)  NOT NULL,";
    cmd += " EstimationAlgorithm              varchar(60)  NOT NULL,";
    cmd += " Minimizer                        varchar(60)      NULL,";
    cmd += " Scaling                          varchar(60)      NULL,";
    cmd += " DatabaseSnapshot                 varchar(60)  NOT NULL,";
    cmd += " Notes                            varchar(100)     NULL,";
    cmd += " isDeterministicBees              varchar(10)  NOT NULL,";
    cmd += " maxGenerations                   varchar(10)  NOT NULL,";
    cmd += " numBees                          varchar(10)  NOT NULL,";
    cmd += " numBestSites                     varchar(10)  NOT NULL,";
    cmd += " numEliteSites                    varchar(10)  NOT NULL,";
    cmd += " numEliteBees                     varchar(10)  NOT NULL,";
    cmd += " numOtherBees                     varchar(10)  NOT NULL,";
    cmd += " neighborhoodSize                 varchar(10)  NOT NULL,";
    cmd += " numSubRuns                       varchar(10)  NOT NULL,";
    cmd += " isDeterministicNLopt             varchar(10)  NOT NULL,";
    cmd += " isStopAfterValue                 varchar(10)  NOT NULL,";
    cmd += " StopAfterValue                   varchar(10)  NOT NULL,";
    cmd += " isStopAfterTime                  varchar(10)  NOT NULL,";
    cmd += " StopAfterTime                    varchar(10)  NOT NULL,";
    cmd += " isStopAfterIter                  varchar(10)  NOT NULL,";
    cmd += " StopAfterIter                    varchar(10)  NOT NULL,";
    cmd += " isEstInitBiomassEnabled          varchar(10)  NOT NULL,";
    cmd += " isEstInitBiomassChecked          varchar(10)  NOT NULL,";
    cmd += " isEstGrowthRateEnabled           varchar(10)  NOT NULL,";
    cmd += " isEstGrowthRateChecked           varchar(10)  NOT NULL,";
    cmd += " isEstGrowthRateShapeEnabled      varchar(10)  NOT NULL,";
    cmd += " isEstGrowthRateShapeChecked      varchar(10)  NOT NULL,";
    cmd += " isEstCarryingCapacityEnabled     varchar(10)  NOT NULL,";
    cmd += " isEstCarryingCapacityChecked     varchar(10)  NOT NULL,";
    cmd += " isEstCatchabilityEnabled         varchar(10)  NOT NULL,";
    cmd += " isEstCatchabilityChecked         varchar(10)  NOT NULL,";
    cmd += " isEstCompAlphaEnabled            varchar(10)  NOT NULL,";
    cmd += " isEstCompAlphaChecked            varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaSpeciesEnabled      varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaSpeciesChecked      varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaGuildsEnabled       varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaGuildsChecked       varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaGuildsGuildsEnabled varchar(10)  NOT NULL,";
    cmd += " isEstCompBetaGuildsGuildsChecked varchar(10)  NOT NULL,";
    cmd += " isEstPredRhoEnabled              varchar(10)  NOT NULL,";
    cmd += " isEstPredRhoChecked              varchar(10)  NOT NULL,";
    cmd += " isEstPredHandlingEnabled         varchar(10)  NOT NULL,";
    cmd += " isEstPredHandlingChecked         varchar(10)  NOT NULL,";
    cmd += " isEstPredExponentEnabled         varchar(10)  NOT NULL,";
    cmd += " isEstPredExponentChecked         varchar(10)  NOT NULL,";
    cmd += " isEstSurveyQEnabled              varchar(10)  NOT NULL,";
    cmd += " isEstSurveyQChecked              varchar(10)  NOT NULL,";
    cmd += " isAMultiRun                      varchar(10)  NOT NULL,";
    cmd += " AveAlg                           varchar(30)  NOT NULL,";
    cmd += " HowToAverage                     varchar(30)  NOT NULL,";
    cmd += " UsingWhat                        varchar(30)  NOT NULL,";
    cmd += " UsingNumber                      varchar(10)  NOT NULL,";
    cmd += " isUsingPct                       varchar(10)  NOT NULL,";
    cmd += " EnsembleFile                     varchar(50)      NULL,";
    cmd += " EstimatedParametersFile          varchar(50)  NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelNumber,ModelName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 30] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 83 of 87: Model Fit Summary
    tableName = nmfConstantsMSSPM::TableSummaryModelFit;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50) NOT NULL,";
    cmd += " ModelName           varchar(50) NOT NULL,";
    cmd += " SpeciesName         varchar(50) NOT NULL,";
    cmd += " Algorithm           varchar(50) NOT NULL,";
    cmd += " Minimizer           varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
    cmd += " Scaling             varchar(50) NOT NULL,";
    cmd += " SSResiduals         varchar(50) NOT NULL,";
    cmd += " SSDeviations        varchar(50) NOT NULL,";
    cmd += " SSTotals            varchar(50) NOT NULL,";
    cmd += " rSquared            varchar(50) NOT NULL,";
    cmd += " rCorrelationCoeff   varchar(50) NOT NULL,";
    cmd += " AkaikeInfoCriterion varchar(50) NOT NULL,";
    cmd += " RootMeanSquareError varchar(50) NOT NULL,";
    cmd += " ReliabilityIndex    varchar(50) NOT NULL,";
    cmd += " AverageError        varchar(50) NOT NULL,";
    cmd += " AverageAbsError     varchar(50) NOT NULL,";
    cmd += " ModelingEfficiency  varchar(50) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesName,Algorithm,Minimizer,ObjectiveCriterion,Scaling))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 31] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 84 of 87: Mohn's Rho Diagnostics Summary
    tableName = nmfConstantsMSSPM::TableSummaryDiagnostic;
    fullTableName = db + "." + tableName;
    ExistingTableNames.push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName                 varchar(50) NOT NULL,";
    cmd += " ModelName                   varchar(50) NOT NULL,";
    cmd += " SpeciesName                 varchar(50) NOT NULL,";
    cmd += " InitialAbsoluteBiomass      varchar(50) NOT NULL,";
    cmd += " GrowthRate                  varchar(50) NULL,";
    cmd += " GrowthRateShape             varchar(50) NULL,";
    cmd += " CarryingCapacity            varchar(50) NULL,";
    cmd += " Catchability                varchar(50) NULL,";
    cmd += " CompetitionAlpha            varchar(50) NULL,";
    cmd += " CompetitionBetaSpecies      varchar(50) NULL,";
    cmd += " CompetitionBetaGuilds       varchar(50) NULL,";
    cmd += " CompetitionBetaGuildsGuilds varchar(50) NULL,";
    cmd += " PredationRho                varchar(50) NULL,";
    cmd += " PredationHandling           varchar(50) NULL,";
    cmd += " PredationExponent           varchar(50) NULL,";
    cmd += " SurveyQ                     varchar(50) NULL,";
    cmd += " EstimatedBiomass            varchar(50) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeciesName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 32] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 85 of 87: OutputBiomassMohnsRhoOfEnsembles
    tableName = nmfConstantsMSSPM::TableOutputBiomassMohnsRhoOfEnsembles;
    fullTableName = db + "." + tableName;
    ExistingTableNames .push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50)    NOT NULL,";
    cmd += " ModelName           varchar(50)    NOT NULL,";
    cmd += " Label               varchar(50)    NOT NULL,";
    cmd += " RunNumber           int(11)        NOT NULL,";
    cmd += " Algorithm           varchar(50)    NOT NULL,";
    cmd += " Minimizer           varchar(50)    NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50)    NOT NULL,";
    cmd += " Scaling             varchar(50)    NOT NULL,";
    cmd += " isAggProd           int(11)        NOT NULL,";
    cmd += " SpeName             varchar(50)    NOT NULL,";
    cmd += " Year                int(11)        NOT NULL,";
    cmd += " Value               DECIMAL(20,10) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,Label,RunNumber,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 33] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 86 of 87: Units
    tableName = nmfConstantsMSSPM::TableUnits;
    fullTableName = db + "." + tableName;
    ExistingTableNames .push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName         varchar(50) NOT NULL,";
    cmd += " ModelName           varchar(50) NOT NULL,";
    cmd += " TableName           varchar(50) NOT NULL,";
    cmd += " Units               varchar(50) NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,TableName,Units))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 34] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    // 87 of 87: FitWeights
    tableName = nmfConstantsMSSPM::TableFitWeights;
    fullTableName = db + "." + tableName;
    ExistingTableNames .push_back(tableName);
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ProjectName       varchar(50) NOT NULL,";
    cmd += " ModelName         varchar(50) NOT NULL,";
    cmd += " SpeName           varchar(50) NOT NULL,";
    cmd += " Biomass           double NOT NULL,";
    cmd += " Catch             double NOT NULL,";
    cmd += " PRIMARY KEY (ProjectName,ModelName,SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 35] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        nmfUtilsQt::updateProgressDlg(m_Logger,m_ProgressDlg,"Created table: "+fullTableName,pInc);
    }
    if (! okToCreateMoreTables) {
        m_ProgressDlg->close();
        return;
    }

    m_ProgressDlg->close();

    disconnect(m_ProgressDlg, SIGNAL(canceled()), this, SLOT(callback_progressDlgCancel()));

}


void
nmfSetup_Tab2::callback_NewDatabase()
{
    bool ok;
    std::string msg;
    std::string cmd;
    std::string errorMsg="";

    QString enteredName = QInputDialog::getText(Setup_Tabs,
                                         tr("Add Database"),
                                         tr("Enter new database name:"),
                                         QLineEdit::Normal,
                                         "", &ok);
    nmfUtilsQt::checkForAndReplaceInvalidCharacters(enteredName);

    if (! ok || enteredName.isEmpty())
        return;

    if (m_ValidDatabases.find(enteredName.toStdString()) != m_ValidDatabases.end()) // Means it found it
    {
        msg = "\nDatabase already exists. Please enter another database name.";
        QMessageBox::critical(Setup_Tabs,tr("Invalid Database Name"),tr(msg.c_str()),QMessageBox::Ok);
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // OK to now add the database and create the necessary table definitions
    cmd = "CREATE database " + enteredName.toStdString();
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        QApplication::restoreOverrideCursor();
        if (QString::fromStdString(errorMsg).contains("database exists")) {
            QMessageBox::critical(Setup_Tabs,tr("Invalid Database Name"),
                                  tr("\nDatabase exists from another application. Please choose a different name."),
                                  QMessageBox::Ok);
            msg = "Error: Database " + enteredName.toStdString() + " exists from another application. Please choose a different name.";
            m_Logger->logMsg(nmfConstants::Error,msg);
            return;
        }
    } else {
        createTables(enteredName);
        loadDatabaseNames(enteredName);
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(enteredName);
        QApplication::restoreOverrideCursor();
        std::string msg = "\nDatabase created: " + enteredName.toStdString() + "\n";
        QMessageBox::information(Setup_Tabs, "Set up Project",
                                 tr(msg.c_str()));
    }

    emit SaveMainSettings();

    Setup_Tabs->setCursor(Qt::ArrowCursor);

    emit RemoveGuildsAndSpecies();

    // Auto save project
    callback_SaveProject();

    // Set Navigator to Setup enabled only mode
    emit AddedNewDatabase();

    QApplication::restoreOverrideCursor();


}

void
nmfSetup_Tab2::callback_ImportDatabase()
{
    QString remProjectDir         = m_ProjectDir;
    QString remProjectName        = m_ProjectName;
    QString remProjectAuthor      = m_ProjectAuthor;
    QString remProjectDescription = m_ProjectDescription;

    emit ImportDatabase();

    m_ProjectDir         = remProjectDir;
    m_ProjectName        = remProjectName;
    m_ProjectAuthor      = remProjectAuthor;
    m_ProjectDescription = remProjectDescription;

    setProjectDirectory(remProjectDir);
    setProjectName(remProjectName);
    setProjectAuthor(remProjectAuthor);
    setProjectDescription(remProjectDescription);
}

void
nmfSetup_Tab2::callback_DelDatabase()
{
    bool deleteOK=true;
    std::string cmd;
    std::string msg;
    std::string errorMsg="";
    QString databaseToDelete = Setup_Tab2_ProjectDatabaseCMB->currentText();
    int databaseIndexToDelete = Setup_Tab2_ProjectDatabaseCMB->currentIndex();
    QMessageBox::StandardButton reply;

    msg = "\nOK to permanently delete the database: " +
            databaseToDelete.toStdString() + " ?\n";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete Database"),
         tr(msg.c_str()), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Remove database from mysql and reload widget
        cmd = "DROP database " + databaseToDelete.toStdString();
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            Setup_Tabs->setCursor(Qt::ArrowCursor);
            nmfUtils::printError("Function: callback_Setup_Tab2_DelDatabase ",errorMsg);
            deleteOK = false;
        }
        loadDatabaseNames("");
        Setup_Tab2_ProjectDatabaseCMB->setCurrentIndex(databaseIndexToDelete-1);

        QApplication::restoreOverrideCursor();

        if (deleteOK) {
            msg = "\nDatabase deleted: " + databaseToDelete.toStdString() + "\n";
            QMessageBox::information(Setup_Tabs, "Delete",
                                     tr(msg.c_str()));
        }

        emit RemoveGuildsAndSpecies();
        emit ClearEstimationTables();

    }
    saveSettings(nmfConstantsMSSPM::ClearModelName);

} // end callback_Setup_Tab2_DelDatabase


void
nmfSetup_Tab2::callback_DatabaseChanged(QString db)
{
    m_LastProjectDatabase = m_ProjectDatabase;
    m_ProjectDatabase = db;
}


void
nmfSetup_Tab2::callback_ProjectDirBrowsePB()
{
    // Launch file dialog
    QString dir = QFileDialog::getExistingDirectory(
                Setup_Tabs,
                tr("Open Project Directory"),
                m_ProjectDir+"/..",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (! dir.isEmpty()) {
        setProjectDirectory(dir);
        m_ProjectDir = dir;
    }
}

void
nmfSetup_Tab2::callback_ProjectDirAdd()
{
    m_ProjectDir = Setup_Tab2_ProjectDirLE->text();
}

void
nmfSetup_Tab2::callback_ProjectNameAdd()
{
    m_ProjectName = getProjectName();
    m_NewProject = true;
}

void
nmfSetup_Tab2::callback_ProjectAuthorAdd()
{
    m_ProjectAuthor = Setup_Tab2_ProjectAuthorLE->text();
}

void
nmfSetup_Tab2::callback_ProjectDescAdd()
{
    m_ProjectDescription = Setup_Tab2_ProjectDescLE->text();
}

void
nmfSetup_Tab2::enableProjectData()
{
    Setup_Tab2_ProjectDataGB->setChecked(true);
}

void
nmfSetup_Tab2::enableSetupTabs(bool enable)
{
    for (int i=2; i<Setup_Tabs->count(); ++i) {
        Setup_Tabs->setTabEnabled(i,enable);
    }
}


bool
nmfSetup_Tab2::loadProject(nmfLogger *logger,
                           QString fileName,
                           bool clearModelName)
{    
    std::string msg;

    if (fileName.isEmpty() || (fileName == ".")) {
        QMessageBox::information(Setup_Tabs,tr("No Project Loaded"),
                              tr("\nPlease create/load a Project.\n"),
                              QMessageBox::Ok);
        return false;
    }

    // If the file doesn't have an extension, add .prj
    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName += ".prj";
    }

    // Read file and load data......
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly)) {
        msg = "Error nmfSetupTab2::loadProject: Cannot open file for reading: "+fileName.toStdString();
        logger->logMsg(nmfConstants::Normal,msg);
        msg = "\nError nmfSetupTab2::loadProject: Cannot open file for reading:\n\n"+fileName.toStdString();
        QMessageBox::critical(Setup_Tabs,tr("Project Error"),
                              tr(msg.c_str()),
                              QMessageBox::Ok);
        return false;
    }
    QTextStream in(&file);
    logger->logMsg(nmfConstants::Normal,"Reading Project File: " + fileName.toStdString());

    // Skip initial comments
    QString line = in.readLine();
    while (line.trimmed()[0] == "#") {
        line = in.readLine();
    }

    m_ProjectDir         = line;
    m_ProjectDatabase    = in.readLine();
    m_ProjectAuthor      = in.readLine();
    m_ProjectDescription = in.readLine();

    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectDirLE->clear();
    //Setup_Tab2_ProjectDatabaseCMB->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();

    QFileInfo filenameNoPath(fileName);
    setProjectName(filenameNoPath.baseName());
    setProjectDirectory(m_ProjectDir);
    Setup_Tab2_ProjectDatabaseCMB->setCurrentText(m_ProjectDatabase);
    setProjectAuthor(m_ProjectAuthor);
    setProjectDescription(m_ProjectDescription);
    m_ProjectName = filenameNoPath.baseName();

    file.close();

    initDatabase(m_ProjectDatabase);

    saveSettings(clearModelName);
    emit LoadProject();
    readSettings();

    logger->logMsg(nmfConstants::Normal,"loadProject end");

    return true;

} // end loadProject



void
nmfSetup_Tab2::initDatabase(QString database)
{
    std::string msg;
    std::string errorMsg;
    std::string dbName = database.toStdString();

    errorMsg = m_DatabasePtr->nmfUpdateDatabase("USE "+dbName);
    m_Logger->logMsg(nmfConstants::Normal,"Initializing database: "+dbName);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "[Error 1] nmfSetup_Tab2::initDatabase: " + errorMsg;
        m_Logger->logMsg(nmfConstants::Error,msg);
    }
}


void
nmfSetup_Tab2::callback_BrowseProject()
{
    QString fileName = QFileDialog::getOpenFileName(Setup_Tabs,
        tr("Load a Project"), m_ProjectDir, tr("Project Files (*.prj)"));

    if (! fileName.isEmpty()) {
        loadProject(m_Logger,fileName,nmfConstantsMSSPM::ClearModelName);
        //updateOutputWidget();
        m_NewProject = true;
    }

   Setup_Tab2_BrowseProjectPB->clearFocus();

}


void
nmfSetup_Tab2::callback_NewProject()
{
    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();
    Setup_Tab2_ProjectDatabaseCMB->clearEditText();
}


void
nmfSetup_Tab2::callback_DelProject()
{
    std::string msg;
    QString projectPrefix = getProjectName();
    std::string ProjectToDelete = projectPrefix.toStdString()+".prj";
    QString fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));

    if (! isProjectNameValid(projectPrefix)) {
        return;
    }
    if (! QFileInfo(fileToDelete).exists())
    {
        msg = "Project file doesn't exist:\n\n" + fileToDelete.toStdString();
        QMessageBox::warning(Setup_Tabs,"Delete Error",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
        return;
    }

    msg = "\nOK to delete the Project file: " + ProjectToDelete + " ?\n";
    QMessageBox::StandardButton reply = QMessageBox::question(Setup_Tabs, tr("Delete Project"),
         tr(msg.c_str()), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));
        QFile file(fileToDelete);
        if (file.remove()) {
            QMessageBox::information(Setup_Tabs,
                                     tr("Remove Project"),
                                     tr("\nProject file removed successfully."),
                                     QMessageBox::Ok);
            callback_NewProject(); // Clear Project fields
        } else {
            msg = "\nCouldn't remove Project file: " + ProjectToDelete;
            QMessageBox::critical(Setup_Tabs,
                                  tr("Remove Project"),
                                  tr(msg.c_str()),
                                  QMessageBox::Ok);
        }
    }
}

void
nmfSetup_Tab2::callback_ReloadProject()
{
    readSettings();
}

void
nmfSetup_Tab2::callback_RefreshDatabase()
{
    loadDatabaseNames(Setup_Tab2_ProjectDatabaseCMB->currentText());
    QMessageBox::information(Setup_Tabs, "Refresh", "\nDatabase list refreshed.\n", QMessageBox::Ok);
}


QString
nmfSetup_Tab2::getProjectName()
{
    return Setup_Tab2_ProjectNameLE->text();
}

QString
nmfSetup_Tab2::getProjectDatabase()
{
    return Setup_Tab2_ProjectDatabaseCMB->currentText();
}

QString
nmfSetup_Tab2::getProjectDir()
{
    return Setup_Tab2_ProjectDirLE->text();
}

QString
nmfSetup_Tab2::getProjectAuthor()
{
    return Setup_Tab2_ProjectAuthorLE->text();
}
QString
nmfSetup_Tab2::getProjectDescription()
{
    return Setup_Tab2_ProjectDescLE->text();
}

void
nmfSetup_Tab2::readSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("SetupTab");
    m_ProjectName        = settings->value("ProjectName","").toString();
    m_ProjectDir         = settings->value("ProjectDir","").toString();
    m_ProjectDatabase    = settings->value("ProjectDatabase","").toString();
    m_ProjectAuthor      = settings->value("ProjectAuthor","").toString();
    m_ProjectDescription = settings->value("ProjectDescription","").toString();
    settings->endGroup();

    // Load class variables into appropriate widgets.
    setProjectName(m_ProjectName);
    setProjectDirectory(m_ProjectDir);
    setProjectAuthor(m_ProjectAuthor);
    setProjectDescription(m_ProjectDescription);
    Setup_Tab2_ProjectDatabaseCMB->setCurrentText(m_ProjectDatabase);

    delete settings;

} // end readSettings


void
nmfSetup_Tab2::saveSettings(bool clearModelName)
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    if (clearModelName) {
        settings->beginGroup("Settings");
        settings->setValue("Name",               "");
        settings->endGroup();
    }

    settings->beginGroup("SetupTab");
    settings->setValue("ProjectName",        getProjectName());
    settings->setValue("ProjectDir",         getProjectDir());
    settings->setValue("ProjectDatabase",    getProjectDatabase());
    settings->setValue("ProjectAuthor",      getProjectAuthor());
    settings->setValue("ProjectDescription", getProjectDescription());
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab2::setProjectName(QString name)
{
    Setup_Tab2_ProjectNameLE->setText(name);
}

void
nmfSetup_Tab2::setProjectDirectory(QString dir)
{
    Setup_Tab2_ProjectDirLE->setText(dir);
}

void
nmfSetup_Tab2::setProjectAuthor(QString author)
{
    Setup_Tab2_ProjectAuthorLE->setText(author);
}
void
nmfSetup_Tab2::setProjectDescription(QString desc)
{
    Setup_Tab2_ProjectDescLE->setText(desc);
}


void
nmfSetup_Tab2::loadDatabaseNames(QString NameToSelect)
{
    QList<QString> authenticatedDatabases = {};
    m_ValidDatabases.clear();
    Setup_Tab2_ProjectDatabaseCMB->clear();

    // Get the list of authenticated databases and add them to the appropriate widget
    if (m_DatabasePtr->getListOfAuthenticatedDatabaseNames(authenticatedDatabases)) {
        foreach (QString item, authenticatedDatabases) {
            m_ValidDatabases.insert(item.toStdString());
            Setup_Tab2_ProjectDatabaseCMB->addItem(item);
        }
    }

    if (! NameToSelect.isEmpty()) {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(NameToSelect);
    } else {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentIndex(0);
    }
}

void
nmfSetup_Tab2::setProjectDatabase(QString dbName)
{
    if (nmfUtils::isOSWindows()) {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(dbName.toLower());
    } else {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(dbName);
    }
}

void
nmfSetup_Tab2::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab2::loadWidgets()");

    readSettings();
    loadDatabaseNames(m_ProjectDatabase);

}

