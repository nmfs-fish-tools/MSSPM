
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
    Setup_Tab2_AddDatabasePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_AddDatabasePB");
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
            this,                          SLOT(callback_Setup_Tab2_ProjectDirBrowsePB()));
    connect(Setup_Tab2_DelDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_DelDatabase()));
    connect(Setup_Tab2_SaveProjectPB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_SaveProject()));
    connect(Setup_Tab2_NewProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_NewProject()));
    connect(Setup_Tab2_DelProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_DelProject()));
    connect(Setup_Tab2_RefreshDatabasePB,  SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_RefreshDatabase()));
    connect(Setup_Tab2_BrowseProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_BrowseProject()));
    connect(Setup_Tab2_ReloadProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_ReloadProject()));
    connect(Setup_Tab2_ProjectNameLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectNameAdd()));
    connect(Setup_Tab2_ProjectAuthorLE,    SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectAuthorAdd()));
    connect(Setup_Tab2_ProjectDescLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectDescAdd()));
    connect(Setup_Tab2_ProjectDirLE,       SIGNAL(returnPressed()),
            this,                          SLOT(callback_Setup_Tab2_ProjectDirAdd()));
    connect(Setup_Tab2_AddDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_AddDatabase()));
    connect(Setup_Tab2_ProjectDatabaseCMB, SIGNAL(currentTextChanged(QString)),
            this,                          SLOT(callback_Setup_Tab2_DatabaseChanged(QString)));

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
nmfSetup_Tab2::callback_Setup_Tab2_SaveProject()
{
    QString fullFilename;
    std::string msg;
    QMessageBox::StandardButton reply;
    QString fileSuffix;
    std::ofstream outFile;

    if (! isProjectDataValid()) {
        QMessageBox::warning(Setup_Tabs,
                             tr("Missing Data"),
                             tr("\nPlease enter missing data above."),
                             QMessageBox::Ok);
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
    fileSuffix =  QFileInfo(fullFilename).suffix();
    if (fileSuffix != "prj")
        fullFilename += ".prj";
    if (QFileInfo(fullFilename).exists()) {
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

    // Make sure the outputImages and outputData directories are there
    QDir dir;
    QString outputImagePath = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDir));
    QString outputDataPath  = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputDataDir));
    QString inputDataPath   = QDir(m_ProjectDir).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    dir.mkpath(outputImagePath);
    dir.mkpath(outputDataPath);
    dir.mkpath(inputDataPath);

    // Load the project
    QString fileName = QDir(m_ProjectDir).filePath(m_ProjectName+".prj");
    loadProject(m_Logger,fileName);

    QApplication::restoreOverrideCursor();

} // end callback_Setup_Tab2_SaveProject

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
    m_ProgressDlg->setRange(0,68);
    m_ProgressDlg->show();
    connect(m_ProgressDlg, SIGNAL(canceled()),
            this,          SLOT(callback_progressDlgCancel()));

    // 1 of 68: BetweenGuildsInteractionCoeff
    fullTableName = db + ".BetweenGuildsInteractionCoeff ";
    ExistingTableNames.push_back("BetweenGuildsInteractionCoeff");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(GuildA       varchar(50) NOT NULL,";
    cmd += " GuildB       varchar(50) NOT NULL,";
    cmd += " Value        int(11) NOT NULL,";
    cmd += " PRIMARY KEY (GuildA,GuildB))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 2 of 68: CompetitionAlpha
    // 3 of 68: CompetitionAlphaMax
    // 4 of 68: CompetitionAlphaMin
    for (std::string tableName : {"CompetitionAlpha",
                                  "CompetitionAlphaMax",
                                  "CompetitionAlphaMin"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName     varchar(50) NOT NULL,";
        cmd += " SpeciesA       varchar(50) NOT NULL,";
        cmd += " SpeciesB       varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (SystemName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 2] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }


    // 5 of 68: CompetitionBetaSpecies
    // 6 of 68: CompetitionBetaSpeciesMax
    // 7 of 68: CompetitionBetaSpeciesMin
    for (std::string tableName : {"CompetitionBetaSpecies",
                                  "CompetitionBetaSpeciesMax",
                                  "CompetitionBetaSpeciesMin"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName     varchar(50) NOT NULL,";
        cmd += " SpeciesA       varchar(50) NOT NULL,";
        cmd += " SpeciesB       varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (SystemName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 3] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 8  of 68: CompetitionBetaGuilds
    // 9  of 68: CompetitionBetaGuildsMax
    // 10 of 68: CompetitionBetaGuildsMin
    for (std::string tableName : {"CompetitionBetaGuilds",
                                  "CompetitionBetaGuildsMax",
                                  "CompetitionBetaGuildsMin"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName     varchar(50) NOT NULL,";
        cmd += " Guild          varchar(50) NOT NULL,";
        cmd += " SpeName        varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (SystemName,Guild,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 4] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 11 of 68: CompetitionBetaGuildsGuilds
    // 12 of 68: CompetitionBetaGuildsGuildsMax
    // 13 of 68: CompetitionBetaGuildsGuildsMin
    for (std::string tableName : {"CompetitionBetaGuildsGuilds",
                                  "CompetitionBetaGuildsGuildsMax",
                                  "CompetitionBetaGuildsGuildsMin"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName     varchar(50) NOT NULL,";
        cmd += " GuildA         varchar(50) NOT NULL,";
        cmd += " GuildB         varchar(50) NOT NULL,";
        cmd += " Value          float NOT NULL,";
        cmd += " PRIMARY KEY (SystemName,GuildA,GuildB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 5] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }



    // 14 of 68: PredationExponent
    // 15 of 68: PredationExponentMin
    // 16 of 68: PredationExponentMax
    for (std::string tableName : {"PredationExponent",
                                  "PredationExponentMin",
                                  "PredationExponentMax"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName varchar(50) NOT NULL,";
        cmd += " SpeName    varchar(50) NOT NULL,";
        cmd += " Value      float NOT NULL,";
        cmd += " PRIMARY KEY (SystemName,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 17 of 68: Catch
    // 18 of 68: Effort
    // 19 of 68: Exploitation
    // 20 of 68: BiomassAbsolute
    // 21 of 68: BiomassRelative
    // 22 of 68: BiomassRelativeDividedByEstSurveyQ
    for (std::string tableName : {"HarvestCatch",
                                  "HarvestEffort",
                                  "HarvestExploitation",
                                  "BiomassAbsolute",
                                  "BiomassRelative",
                                  "BiomassRelativeDividedByEstSurveyQ"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Value         double NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 23 of 68: BiomassRelativeScalar
    fullTableName = db + ".BiomassRelativeScalars";
    ExistingTableNames.push_back("BiomassRelativeScalars");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
    cmd += " SystemName    varchar(50) NOT NULL,";
    cmd += " SpeName       varchar(50) NOT NULL,";
    cmd += " Type          varchar(50) NOT NULL,";
    cmd += " Value         float NOT NULL,";
    cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Type))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 7] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 24 of 68: Covariate
    fullTableName = db + ".Covariate";
    ExistingTableNames.push_back("Covariate");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(Year       int(11) NOT NULL,";
    cmd += " Value      float NOT NULL,";
    cmd += " PRIMARY KEY (Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 8] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 25 of 68: CovariateTS
    fullTableName = db + ".CovariateTS";
    ExistingTableNames.push_back("CovariateTS");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(RunNumber   int(11) NOT NULL,";
    cmd += " Value       float NOT NULL,";
    cmd += " PRIMARY KEY (RunNumber))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 9] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 26 of 68: Guilds
    fullTableName = db + ".Guilds";
    ExistingTableNames.push_back("Guilds");
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
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 27 of 68: OutputBiomass
    fullTableName = db + ".OutputBiomass";
    ExistingTableNames.push_back("OutputBiomass");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(MohnsRhoLabel       varchar(50) NOT NULL,";
    cmd += " Algorithm           varchar(50) NOT NULL,";
    cmd += " Minimizer           varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
    cmd += " Scaling             varchar(50) NOT NULL,";
    cmd += " isAggProd           int(11)     NOT NULL,";
    cmd += " SpeName             varchar(50) NOT NULL,";
    cmd += " Year                int(11)     NOT NULL,";
    cmd += " Value               double      NOT NULL,";
    cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 11] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 28 of 68: OutputCompetitionAlpha
    // 29 of 68: OutputCompetitionBetaSpecies
    // 30 of 68: OutputPredation
    // 31 of 68: OutputPredationHandling
    for (std::string tableName : {"OutputCompetitionAlpha",
                                  "OutputCompetitionBetaSpecies",
                                  "OutputPredationRho",
                                  "OutputPredationHandling"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel       varchar(50) NOT NULL,";
        cmd += " Algorithm           varchar(50) NOT NULL,";
        cmd += " Minimizer           varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
        cmd += " Scaling             varchar(50) NOT NULL,";
        cmd += " isAggProd           int(11)     NOT NULL,";
        cmd += " SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               double      NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 12] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 32 of 68: OutputCompetitionBetaGuilds
    for (std::string tableName : {"OutputCompetitionBetaGuilds"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel       varchar(50) NOT NULL,";
        cmd += " Algorithm           varchar(50) NOT NULL,";
        cmd += " Minimizer           varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
        cmd += " Scaling             varchar(50) NOT NULL,";
        cmd += " isAggProd           int(11)     NOT NULL,";
        cmd += " SpeName             varchar(50) NOT NULL,";
        cmd += " Guild               varchar(50) NOT NULL,";
        cmd += " Value               double      NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 13] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 33 of 68: OutputCompetitionBetaGuildsGuilds
    for (std::string tableName : {"OutputCompetitionBetaGuildsGuilds"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel       varchar(50) NOT NULL,";
        cmd += " Algorithm           varchar(50) NOT NULL,";
        cmd += " Minimizer           varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
        cmd += " Scaling             varchar(50) NOT NULL,";
        cmd += " isAggProd           int(11)     NOT NULL,";
        cmd += " GuildA              varchar(50) NOT NULL,";
        cmd += " GuildB              varchar(50) NOT NULL,";
        cmd += " Value               double      NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,GuildA,GuildB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 13.1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 34 of 68: OutputCatchability
    // 35 of 68: OutputGrowthRate
    // 36 of 68: OutputCarryingCapacity
    // 37 of 68: OutputSurveyQ
    // 38 of 68: OutputPredationExponent
    // 39 of 68: OutputMSY
    // 40 of 68: OutputMSYBiomass
    // 41 of 68: OutputMSYFishing
    // 42 of 68: OutputInitBiomass
    for (std::string tableName : {"OutputCatchability",
                                  "OutputGrowthRate",
                                  "OutputCarryingCapacity",
                                  "OutputSurveyQ",
                                  "OutputPredationExponent",
                                  "OutputMSY",
                                  "OutputMSYBiomass",
                                  "OutputMSYFishing",
                                  "OutputInitBiomass"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel       varchar(50) NOT NULL,";
        cmd += " Algorithm           varchar(50) NOT NULL,";
        cmd += " Minimizer           varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
        cmd += " Scaling             varchar(50) NOT NULL,";
        cmd += " isAggProd           int(11)     NOT NULL,";
        cmd += " SpeName             varchar(50) NOT NULL,";
        cmd += " Value               double      NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 14] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 43 of 68: SpatialOverlap
    for (std::string tableName : {"SpatialOverlap"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 15] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }


    // 44 of 68: PredationHandling
    // 45 of 68: PredationHandlingMin
    // 46 of 68: PredationHandlingMax
    // 47 of 68: PredationRho
    // 48 of 68: PredationRhoMax
    // 49 of 68: PredationRhoMin
    // xx of 68: TestCompetition
    for (std::string tableName : {"PredationHandling",
                                  "PredationHandlingMin",
                                  "PredationHandlingMax",
                                  "PredationRho",
                                  "PredationRhoMax",
                                  "PredationRhoMin"})
//                                "TestCompetition"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName          varchar(50) NOT NULL,";
        cmd += " SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (SystemName,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 16] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

//    // 39 of 68: TestData
//    fullTableName = db + ".TestData";
//    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
//    cmd += "(GrowthRate        float NOT NULL,";
//    cmd += " CarryingCapacity  float NOT NULL)";
//    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
//    if (nmfUtilsQt::isAnError(errorMsg)) {
//        nmfUtils::printError("[Error 16] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
//        okToCreateMoreTables = false;
//    } else {
//        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
//        m_ProgressDlg->setValue(++pInc);
//        m_ProgressDlg->update();
//    }
//    if (! okToCreateMoreTables)
//        return;

    // 50 of 68: Species
    fullTableName = db + ".Species";
    ExistingTableNames.push_back("Species");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(SpeName              varchar(100) NOT NULL,";
    cmd += " GuildName            varchar(100) NULL,";
    cmd += " InitBiomass          double NULL,";
    cmd += " SurveyQ              float NULL,";
    cmd += " SpeciesK             double NULL,";
    cmd += " SpeciesKCovarCoeff   float NULL,";
    cmd += " GrowthRate           float NULL,";
    cmd += " GrowthRateCovarCoeff float NULL,";
    cmd += " Habitat              varchar(50) NULL,";
    cmd += " Catchability         float NULL,";
    cmd += " InitBiomassMin       double NULL,";
    cmd += " InitBiomassMax       double NULL,";
    cmd += " SurveyQMin           float NULL,";
    cmd += " SurveyQMax           float NULL,";
    cmd += " GrowthRateMin        float NULL,";
    cmd += " GrowthRateMax        float NULL,";
    cmd += " SpeciesKMin          double NULL,";
    cmd += " SpeciesKMax          double NULL,";
    cmd += " CatchabilityMin      float NULL,";
    cmd += " CatchabilityMax      float NULL,";
    cmd += " SpeDependence        float NULL,";
    cmd += " ExploitationRate     float NULL,";
    cmd += " PRIMARY KEY (SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 18] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 51 of 68: Forecasts
    fullTableName = db + ".Forecasts";
    ExistingTableNames.push_back("Forecasts");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName       varchar(50) NOT NULL,";
    cmd += " PreviousRun        int(11)     NOT NULL,";
    cmd += " Algorithm          varchar(50) NOT NULL,";
    cmd += " Minimizer          varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling            varchar(50) NOT NULL,";
    cmd += " GrowthForm         varchar(50) NOT NULL,";
    cmd += " HarvestForm        varchar(50) NOT NULL,";
    cmd += " WithinGuildCompetitionForm varchar(50) NOT NULL,";
    cmd += " PredationForm      varchar(50) NOT NULL,";
    cmd += " RunLength          int(11)     NOT NULL,";
    cmd += " StartYear          int(11)     NOT NULL,";
    cmd += " EndYear            int(11)     NOT NULL,";
    cmd += " NumRuns            int(11)     NOT NULL,";
    cmd += " IsDeterministic    int(11)     NOT NULL,";
    cmd += " Seed               int(11)     NOT NULL,";
    cmd += " PRIMARY KEY (ForecastName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 19] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 52 of 68: ForecastHarvestExploitation
    // 53 of 68: ForecastHarvestEffort
    // 54 of 68: ForecastHarvestCatch
    for (std::string tableName : {"ForecastHarvestExploitation",
                                  "ForecastHarvestEffort",
                                  "ForecastHarvestCatch"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(ForecastName       varchar(50) NOT NULL,";
        cmd += " Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " Year               int(11)     NOT NULL,";
        cmd += " Value              double      NOT NULL,";
        cmd += " PRIMARY KEY (ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 20] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 55 of 68: ForecastBiomass
    fullTableName = db + ".ForecastBiomass";
    ExistingTableNames.push_back("ForecastBiomass");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName       varchar(50) NOT NULL,";
    cmd += " Algorithm          varchar(50) NOT NULL,";
    cmd += " Minimizer          varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling            varchar(50) NOT NULL,";
    cmd += " isAggProd          int(11)     NOT NULL,";
    cmd += " SpeName            varchar(50) NOT NULL,";
    cmd += " Year               int(11)     NOT NULL,";
    cmd += " Value              float       NOT NULL,";
    cmd += " PRIMARY KEY (ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 22] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 56 of 68: ForecastBiomassMonteCarlo
    fullTableName = db + ".ForecastBiomassMonteCarlo";
    ExistingTableNames.push_back("ForecastBiomassMonteCarlo");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName varchar(50) NOT NULL,";
    cmd += " RunNum       int(11)     NOT NULL,";
    cmd += " Algorithm    varchar(50) NOT NULL,";
    cmd += " Minimizer    varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling      varchar(50) NOT NULL,";
    cmd += " isAggProd    int(11)     NOT NULL,";
    cmd += " SpeName      varchar(50) NOT NULL,";
    cmd += " Year         int(11)     NOT NULL,";
    cmd += " Value        float       NOT NULL,";
    cmd += " PRIMARY KEY (ForecastName,RunNum,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 21] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 57 of 68: ForecastMonteCarloParameters
    fullTableName = db + ".ForecastMonteCarloParameters";
    ExistingTableNames.push_back("ForecastMonteCarloParameters");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName                varchar(50) NOT NULL,";
    cmd += " RunNum                      int(11)     NOT NULL,";
    cmd += " Algorithm                   varchar(50) NOT NULL,";
    cmd += " Minimizer                   varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion          varchar(50) NOT NULL,";
    cmd += " Scaling                     varchar(50) NOT NULL,";
    cmd += " SpeName                     varchar(50) NOT NULL,";
    cmd += " GrowthRate                  float       NULL,";
    cmd += " CarryingCapacity            float       NULL,";
    cmd += " Catchability                float       NULL,";
    cmd += " Exponent                    float       NULL,";
    cmd += " CompetitionAlpha            float       NULL,";
    cmd += " CompetitionBetaSpecies      float       NULL,";
    cmd += " CompetitionBetaGuilds       float       NULL,";
    cmd += " CompetitionBetaGuildsGuilds float       NULL,";
    cmd += " Predation                   float       NULL,";
    cmd += " Handling                    float       NULL,";
    cmd += " Harvest                     float       NULL,";
    cmd += " PRIMARY KEY (ForecastName,RunNum,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 21.1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 58 of 68: ForecastBiomassMultiScenario
    fullTableName = db + ".ForecastBiomassMultiScenario";
    ExistingTableNames.push_back("ForecastBiomassMultiScenario");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ScenarioName  varchar(50) NOT NULL,";
    cmd += " SortOrder     int(11)     NOT NULL,"; // Because user may not want default sort order for ForecastLabel
    cmd += " ForecastLabel varchar(50) NOT NULL,";
    cmd += " SpeName       varchar(50) NOT NULL,";
    cmd += " Year          int(11)     NOT NULL,";
    cmd += " Value         float       NOT NULL,";
    cmd += " PRIMARY KEY (ScenarioName,SortOrder,ForecastLabel,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 22] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 59 of 68: ForecastUncertainty
    fullTableName = db + ".ForecastUncertainty";
    ExistingTableNames.push_back("ForecastUncertainty");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName                varchar(50) NOT NULL,";
    cmd += " SpeName                     varchar(50) NOT NULL,";
    cmd += " Algorithm                   varchar(50) NOT NULL,";
    cmd += " Minimizer                   varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion          varchar(50) NOT NULL,";
    cmd += " Scaling                     varchar(50) NOT NULL,";
    cmd += " InitBiomass                 float       NOT NULL,";
    cmd += " GrowthRate                  float       NOT NULL,";
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
    cmd += " PRIMARY KEY (ForecastName,SpeName,Algorithm,Minimizer,ObjectiveCriterion,Scaling))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 23] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 60 of 68: DiagnosticInitBiomass
    // 61 of 68: DiagnosticGrowthRate
    // 62 of 68: DiagnosticCarryingCapacity
    // 63 of 68: DiagnosticCatchability
    // 64 of 68: DiagnosticSurveyQ
    for (std::string tableName : {"DiagnosticInitBiomass",
                                  "DiagnosticGrowthRate",
                                  "DiagnosticCarryingCapacity",
                                  "DiagnosticCatchability",
                                  "DiagnosticSurveyQ"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " isAggProd          int(11)     NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " Offset             double      NOT NULL,";
        cmd += " Value              double      NOT NULL,";
        cmd += " Fitness            double      NULL,";
        cmd += " PRIMARY KEY (Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Offset))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 24] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 65 of 68: DiagnosticSurface
    for (std::string tableName : {"DiagnosticSurface"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(Algorithm          varchar(50) NOT NULL,";
        cmd += " Minimizer          varchar(50) NOT NULL,";
        cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
        cmd += " Scaling            varchar(50) NOT NULL,";
        cmd += " isAggProd          int(11)     NOT NULL,";
        cmd += " SpeName            varchar(50) NOT NULL,";
        cmd += " parameter1PctVar   double      NOT NULL,";
        cmd += " parameter2PctVar   double      NOT NULL,";
        cmd += " Fitness            double      NULL,";
        cmd += " PRIMARY KEY (Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,parameter1PctVar,parameter2PctVar))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 25] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }
/*
    // 54 of 68: OutputBiomassMohnsRho
    fullTableName = db + ".OutputBiomassMohnsRho";
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(Label              varchar(50) NOT NULL,";
    cmd += " Algorithm          varchar(50) NOT NULL,";
    cmd += " Minimizer          varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling            varchar(50) NOT NULL,";
    cmd += " isAggProd          int(11)     NOT NULL,";
    cmd += " SpeName            varchar(50) NOT NULL,";
    cmd += " Year               int(11)     NOT NULL,";
    cmd += " Value              float       NOT NULL,";
    cmd += " PRIMARY KEY (Label,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 25] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        progressDlg->setValue(++pInc);
        progressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;
*/
    // 66 of 68: Systems
    for (std::string tableName : {"Systems"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
//      fullTableName = db + ".Systems";
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName                  varchar(100) NOT NULL,";
        cmd += " CarryingCapacity            int(11)      NULL,";
        cmd += " ObsBiomassType              varchar(50)  NULL,";
        cmd += " Units                       varchar(50)  NULL,";
        cmd += " GrowthForm                  varchar(25)  NULL,";
        cmd += " PredationForm               varchar(25)  NULL,";
        cmd += " HarvestForm                 varchar(25)  NULL,";
        cmd += " WithinGuildCompetitionForm  varchar(25)  NULL,";
        cmd += " BetweenGuildCompetitionForm varchar(25)  NULL,";
        cmd += " NumberOfParameters          int(11)      NULL,";
        cmd += " StochasticGrowth            varchar(25)  NULL,";
        cmd += " StochasticCatch             varchar(25)  NULL,";
        cmd += " AssessmentType              varchar(50)  NULL,";
        cmd += " CVAssessment                float        NULL,";
        cmd += " IndicatorSpecies            int(11)      NULL,";
        cmd += " ACLBuffer                   float        NULL,";
        cmd += " AssessmentInterval          int(11)      NULL,";
        cmd += " NumberOfRuns                int(11)      NULL,";
        cmd += " HistoricalEnd               int(11)      NULL,";
        cmd += " StartYear                   int(11)      NULL,";
        cmd += " RunLength                   int(11)      NULL,";
        cmd += " TimeStep                    float        NULL,";
        cmd += " Algorithm                   varchar(50)  NULL,";
        cmd += " Minimizer                   varchar(50)  NULL,";
        cmd += " ObjectiveCriterion          varchar(50)  NULL,";
        cmd += " Scaling                     varchar(50)  NULL,";
        cmd += " OutputType                  varchar(50)  NULL,";
        cmd += " GAGenerations               int(11)      NULL,";
        cmd += " GAPopulationSize            int(11)      NULL,";
        cmd += " GAMutationRate              float        NULL,";
        cmd += " GAConvergence               int(11)      NULL,";
        cmd += " BeesNumTotal                int(11)      NULL,";
        cmd += " BeesNumElite                int(11)      NULL,";
        cmd += " BeesNumOther                int(11)      NULL,";
        cmd += " BeesNumEliteSites           int(11)      NULL,";
        cmd += " BeesNumBestSites            int(11)      NULL,";
        cmd += " BeesNumRepetitions          int(11)      NULL,";
        cmd += " BeesMaxGenerations          int(11)      NULL,";
        cmd += " BeesNeighborhoodSize        float        NULL,";
        cmd += " GradMaxIterations           int(11)      NULL,";
        cmd += " GradMaxLineSearches         int(11)      NULL,";
        cmd += " NLoptNumberOfRuns           int(11)      NULL,";
        cmd += " NLoptUseStopVal             int(11)      NULL,";
        cmd += " NLoptUseStopAfterTime       int(11)      NULL,";
        cmd += " NLoptUseStopAfterIter       int(11)      NULL,";
        cmd += " NLoptStopVal                double       NULL,";
        cmd += " NLoptStopAfterTime          int(11)      NULL,";
        cmd += " NLoptStopAfterIter          int(11)      NULL,";
        cmd += " PRIMARY KEY (SystemName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            nmfUtils::printError("[Error 27] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 67 of 68: Application (contains name of application - used to assure app is using correct database)
    for (std::string tableName : {"Application"})
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
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
        m_DatabasePtr->saveApplicationTable(Setup_Tabs,m_Logger,fullTableName);
    }

    // 68 of 68: OutputBiomassEnsemble
    fullTableName = db + ".OutputBiomassEnsemble";
    ExistingTableNames.push_back("OutputBiomassEnsemble");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(Label               varchar(50) NOT NULL,";
    cmd += " RunNumber           int(11)     NOT NULL,";
    cmd += " Algorithm           varchar(50) NOT NULL,";
    cmd += " Minimizer           varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion  varchar(50) NOT NULL,";
    cmd += " Scaling             varchar(50) NOT NULL,";
    cmd += " isAggProd           int(11)     NOT NULL,";
    cmd += " SpeName             varchar(50) NOT NULL,";
    cmd += " Year                int(11)     NOT NULL,";
    cmd += " Value               double      NOT NULL,";
    cmd += " PRIMARY KEY (Label,RunNumber,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        nmfUtils::printError("[Error 29] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;


    m_ProgressDlg->close();

    disconnect(m_ProgressDlg, SIGNAL(canceled()), this, SLOT(callback_progressDlgCancel()));

}



void
nmfSetup_Tab2::callback_Setup_Tab2_AddDatabase()
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
        std::string msg = "\nDatabase created: " + enteredName.toStdString() + "\n";
        QMessageBox::information(Setup_Tabs, "Set up Project",
                                 tr(msg.c_str()));
    }

    emit SaveMainSettings();

    Setup_Tabs->setCursor(Qt::ArrowCursor);

    emit RemoveGuildsAndSpecies();

    // Auto save project
    callback_Setup_Tab2_SaveProject();

    // Set Navigator to Setup enabled only mode
    emit AddedNewDatabase();

    QApplication::restoreOverrideCursor();


} // end callback_Setup_Tab2_AddDatabase


void
nmfSetup_Tab2::callback_Setup_Tab2_DelDatabase()
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
    saveSettings();

} // end callback_Setup_Tab2_DelDatabase


void
nmfSetup_Tab2::callback_Setup_Tab2_DatabaseChanged(QString db)
{
    m_LastProjectDatabase = m_ProjectDatabase;
    m_ProjectDatabase = db;
}


void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDirBrowsePB()
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
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDirAdd()
{
    m_ProjectDir = Setup_Tab2_ProjectDirLE->text();
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectNameAdd()
{
    m_ProjectName = getProjectName();
    m_NewProject = true;
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectAuthorAdd()
{
    m_ProjectAuthor = Setup_Tab2_ProjectAuthorLE->text();
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDescAdd()
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
nmfSetup_Tab2::loadProject(nmfLogger *logger, QString fileName)
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

    saveSettings();
    emit LoadProject();
    readSettings();

//    initDatabase(m_ProjectDatabase);

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
nmfSetup_Tab2::callback_Setup_Tab2_BrowseProject()
{
    QString fileName = QFileDialog::getOpenFileName(Setup_Tabs,
        tr("Load a Project"), m_ProjectDir, tr("Project Files (*.prj)"));

    if (! fileName.isEmpty()) {
        loadProject(m_Logger,fileName);
        //updateOutputWidget();
        m_NewProject = true;
    }

   Setup_Tab2_BrowseProjectPB->clearFocus();

}


void
nmfSetup_Tab2::callback_Setup_Tab2_NewProject()
{
    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();
    Setup_Tab2_ProjectDatabaseCMB->clearEditText();
}


void
nmfSetup_Tab2::callback_Setup_Tab2_DelProject()
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
            callback_Setup_Tab2_NewProject(); // Clear Project fields
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
nmfSetup_Tab2::callback_Setup_Tab2_ReloadProject()
{
    readSettings();
}

void
nmfSetup_Tab2::callback_Setup_Tab2_RefreshDatabase()
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
nmfSetup_Tab2::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

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

