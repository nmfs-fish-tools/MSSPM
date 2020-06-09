
#include "nmfSetupTab02.h"
#include "nmfConstants.h"
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

    m_ProjectName = getProjectName();
    m_ProjectDatabase = getProjectDatabase();

    if (! isProjectNameValid(m_ProjectName)) {
        return;
    }

    Setup_Tabs->setCursor(Qt::WaitCursor);

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
            Setup_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
    }

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
    QString imagePath = QDir(m_ProjectDir).filePath("outputImages");
    QString dataPath  = QDir(m_ProjectDir).filePath("outputData");
    QDir dir;
    dir.mkpath(imagePath);
    dir.mkpath(dataPath);

    // Load the project
    QString fileName = QDir(m_ProjectDir).filePath(m_ProjectName+".prj");
    loadProject(m_Logger,fileName);

    Setup_Tabs->setCursor(Qt::ArrowCursor);

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
    m_ProgressDlg->setRange(0,51);
    m_ProgressDlg->show();
    connect(m_ProgressDlg, SIGNAL(canceled()),
            this,          SLOT(callback_progressDlgCancel()));

    // 1 of 51: BetweenGuildsInteractionCoeff
    fullTableName = db + ".BetweenGuildsInteractionCoeff ";
    ExistingTableNames.push_back("BetweenGuildsInteractionCoeff");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(GuildA       varchar(50) NOT NULL,";
    cmd += " GuildB       varchar(50) NOT NULL,";
    cmd += " Value        int(11) NOT NULL,";
    cmd += " PRIMARY KEY (GuildA,GuildB))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 2 of 51: CompetitionAlpha
    // 3 of 51: HandlingTime
    for (std::string tableName : {"CompetitionAlpha",
                                  "HandlingTime"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SpeciesA       varchar(50) NOT NULL,";
        cmd += " SpeciesB       varchar(50) NOT NULL,";
        cmd += " Value          int(11) NOT NULL,";
        cmd += " PRIMARY KEY (SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 4 of 51: CompetitionAlphaMax
    // 5 of 51: CompetitionAlphaMin
    for (std::string tableName : {"CompetitionAlphaMax",
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
        if (errorMsg != " ") {
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


    // 6 of 51: CompetitionBetaSpeciesMax
    // 7 of 51: CompetitionBetaSpeciesMin
    for (std::string tableName : {"CompetitionBetaSpeciesMax",
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
        if (errorMsg != " ") {
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

    // 8 of 51: CompetitionBetaGuildsMax
    // 9 of 51: CompetitionBetaGuildsMin
    for (std::string tableName : {"CompetitionBetaGuildsMax",
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
        if (errorMsg != " ") {
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

    // 10 of 51: PredationExponentMin
    // 11 of 51: PredationExponentMax
    for (std::string tableName : {"PredationExponentMin",
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
        if (errorMsg != " ") {
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

    // 12 of 51: Catch
    // 13 of 51: Effort
    // 14 of 51: Exploitation
    // 15 of 51: ObservedBiomass
    for (std::string tableName : {"Catch",
                                  "Effort",
                                  "Exploitation",
                                  "ObservedBiomass"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Value         float NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
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


    // 16 of 51: Covariate
    fullTableName = db + ".Covariate";
    ExistingTableNames.push_back("Covariate");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(Year       int(11) NOT NULL,";
    cmd += " Value      float NOT NULL,";
    cmd += " PRIMARY KEY (Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 17 of 51: CovariateTS
    fullTableName = db + ".CovariateTS";
    ExistingTableNames.push_back("CovariateTS");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(RunNumber   int(11) NOT NULL,";
    cmd += " Value       float NOT NULL,";
    cmd += " PRIMARY KEY (RunNumber))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 7] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 18 of 51: Guilds
    fullTableName = db + ".Guilds";
    ExistingTableNames.push_back("Guilds");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(GuildName       varchar(100) NOT NULL,";
    cmd += " GrowthRate      float NULL,";
    cmd += " GrowthRateMin   float NULL,";
    cmd += " GrowthRateMax   float NULL,";
    cmd += " GuildK          float NULL,";
    cmd += " GuildKMin       float NULL,";
    cmd += " GuildKMax       float NULL,";
    cmd += " Catchability    float NULL,";
    cmd += " CatchabilityMin float NULL,";
    cmd += " CatchabilityMax float NULL,";
    cmd += " PRIMARY KEY (GuildName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 8] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 19 of 51: OutputBiomass
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
    cmd += " Value               float       NOT NULL,";
    cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Year))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 9] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 20 of 51: OutputCompetitionAlpha
    // 21 of 51: OutputCompetitionBetaSpecies
    // 22 of 51: OutputPredation
    // 23 of 51: OutputHandling
    for (std::string tableName : {"OutputCompetitionAlpha",
                                  "OutputCompetitionBetaSpecies",
                                  "OutputPredation",
                                  "OutputHandling"})
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
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 11] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 24 of 51: OutputCompetitionBetaGuilds
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
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
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

    // 25 of 51: OutputCatchability
    // 26 of 51: OutputGrowthRate
    // 27 of 51: OutputCarryingCapacity
    // 28 of 51: OutputExponent
    // 29 of 51: OutputMSY
    // 30 of 51: OutputMSYBiomass
    // 31 of 51: OutputMSYFishing
    for (std::string tableName : {"OutputCatchability",
                                  "OutputGrowthRate",
                                  "OutputCarryingCapacity",
                                  "OutputExponent",
                                  "OutputMSY",
                                  "OutputMSYBiomass",
                                  "OutputMSYFishing"})
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
        cmd += " Value               float NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
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

    // 32 of 51: PredationLossRates
    // 33 of 51: SpatialOverlap
    for (std::string tableName : {"PredationLossRates",
                                  "SpatialOverlap"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SpeciesA            varchar(50) NOT NULL,";
        cmd += " SpeciesB            varchar(50) NOT NULL,";
        cmd += " Value               float NULL,";
        cmd += " PRIMARY KEY (SpeciesA,SpeciesB))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
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

    // 34 of 51: PredationLossRatesMax
    // 35 of 51: PredationLossRatesMin
    // 36 of 51: HandlingTimeMin
    // 37 of 51: HandlingTimeMin
    // xx of 51: TestCompetition
    for (std::string tableName : {"HandlingTimeMin",
                                  "HandlingTimeMax",
                                  "PredationLossRatesMax",
                                  "PredationLossRatesMin"})
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
        if (errorMsg != " ") {
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

//    // 39 of 51: TestData
//    fullTableName = db + ".TestData";
//    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
//    cmd += "(GrowthRate        float NOT NULL,";
//    cmd += " CarryingCapacity  float NOT NULL)";
//    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
//    if (errorMsg != " ") {
//        nmfUtils::printError("[Error 16] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
//        okToCreateMoreTables = false;
//    } else {
//        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
//        m_ProgressDlg->setValue(++pInc);
//        m_ProgressDlg->update();
//    }
//    if (! okToCreateMoreTables)
//        return;

    // 38 of 51: Species
    fullTableName = db + ".Species";
    ExistingTableNames.push_back("Species");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(SpeName              varchar(100) NOT NULL,";
    cmd += " GuildName            varchar(100) NULL,";
    cmd += " InitBiomass          float NULL,";
    cmd += " SurveyQ              int(11) NULL,";
    cmd += " SpeciesK             float NULL,";
    cmd += " SpeciesKCovarCoeff   float NULL,";
    cmd += " GrowthRate           float NULL,";
    cmd += " GrowthRateCovarCoeff float NULL,";
    cmd += " Habitat              varchar(50) NULL,";
    cmd += " Catchability         float NULL,";
    cmd += " InitBiomassMin       float NULL,";
    cmd += " InitBiomassMax       float NULL,";
    cmd += " SurveyQMin           int(11) NULL,";
    cmd += " SurveyQMax           int(11) NULL,";
    cmd += " GrowthRateMin        float NULL,";
    cmd += " GrowthRateMax        float NULL,";
    cmd += " SpeciesKMin          int(11) NULL,";
    cmd += " SpeciesKMax          int(11) NULL,";
    cmd += " CatchabilityMin      float NULL,";
    cmd += " CatchabilityMax      float NULL,";
    cmd += " SpeDependence        float NULL,";
    cmd += " ExploitationRate     float NULL,";
    cmd += " PRIMARY KEY (SpeName))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 17] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 39 of 51: Forecasts
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
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 18] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 40 of 51: ForecastExploitation
    // 41 of 51: ForecastEffort
    // 42 of 51: ForecastCatch
    for (std::string tableName : {"ForecastExploitation",
                                  "ForecastEffort",
                                  "ForecastCatch"})
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
        cmd += " Value              float       NOT NULL,";
        cmd += " PRIMARY KEY (ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 19] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 43 of 51: ForecastBiomass
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
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 20] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 44 of 51: ForecastBiomassMonteCarlo
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
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 20] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 45 of 51: ForecastBiomassMultiScenario
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
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 21] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 46 of 51: ForecastUncertainty
    fullTableName = db + ".ForecastUncertainty";
    ExistingTableNames.push_back("ForecastUncertainty");
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(ForecastName       varchar(50) NOT NULL,";
    cmd += " SpeName            varchar(50) NOT NULL,";
    cmd += " Algorithm          varchar(50) NOT NULL,";
    cmd += " Minimizer          varchar(50) NOT NULL,";
    cmd += " ObjectiveCriterion varchar(50) NOT NULL,";
    cmd += " Scaling            varchar(50) NOT NULL,";
    cmd += " GrowthRate         float       NOT NULL,";
    cmd += " CarryingCapacity   float       NOT NULL,";
    cmd += " Predation          float       NOT NULL,";
    cmd += " Competition        float       NOT NULL,";
    cmd += " BetaSpecies        float       NOT NULL,";
    cmd += " BetaGuilds         float       NOT NULL,";
    cmd += " Handling           float       NOT NULL,";
    cmd += " Exponent           float       NOT NULL,";
    cmd += " Catchability       float       NOT NULL,";
    cmd += " Harvest            float       NOT NULL,";
    cmd += " PRIMARY KEY (ForecastName,SpeName,Algorithm,Minimizer,ObjectiveCriterion,Scaling))";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 22] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        m_ProgressDlg->setValue(++pInc);
        m_ProgressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 47 of 51: DiagnosticGrowthRate
    // 48 of 51: DiagnosticCarryingCapacity
    for (std::string tableName : {"DiagnosticGrowthRate",
                                  "DiagnosticCarryingCapacity"})
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
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 23] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 49 of 51: DiagnosticGRandCC (Growth Rate and CarryingCapacity
    for (std::string tableName : {"DiagnosticGRandCC"})
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
        cmd += " rPctVariation      double      NOT NULL,";
        cmd += " KPctVariation      double      NOT NULL,";
        cmd += " Fitness            double      NULL,";
        cmd += " PRIMARY KEY (Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,rPctVariation,KPctVariation))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
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
/*
    // 52 of 51: OutputBiomassMohnsRho
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
    if (errorMsg != " ") {
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
    // 50 of 51: Systems
    for (std::string tableName : {"Systems"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
//      fullTableName = db + ".Systems";
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(SystemName                  varchar(100) NOT NULL,";
        cmd += " CarryingCapacity            int(11)      NULL,";
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
        cmd += " NLoptUseStopVal             int(11)      NULL,";
        cmd += " NLoptUseStopAfterTime       int(11)      NULL,";
        cmd += " NLoptUseStopAfterIter       int(11)      NULL,";
        cmd += " NLoptStopVal                double       NULL,";
        cmd += " NLoptStopAfterTime          int(11)      NULL,";
        cmd += " NLoptStopAfterIter          int(11)      NULL,";
        cmd += " PRIMARY KEY (SystemName))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 26] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_Logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            m_ProgressDlg->setValue(++pInc);
            m_ProgressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 51 of 51: Application (contains name of application - used to assure app is using correct database)
    for (std::string tableName : {"Application"})
    {
        ExistingTableNames.push_back(tableName);
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(Name varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (Name))";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 27] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
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

    Setup_Tabs->setCursor(Qt::WaitCursor);

    // OK to now add the database and create the necessary table definitions
    cmd = "CREATE database " + enteredName.toStdString();
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("Function: callback_Setup_Tab2_AddDatabase ",errorMsg);
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
        if (errorMsg != " ") {
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

    saveSettings();
    emit LoadProject();
    readSettings();

    initDatabase(m_ProjectDatabase);

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
    if (errorMsg != " ") {
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
nmfSetup_Tab2::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab2::loadWidgets()");

    readSettings();
    loadDatabaseNames(m_ProjectDatabase);

}

