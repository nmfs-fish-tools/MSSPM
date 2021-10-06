#include "nmfEstimationTab03.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab3::nmfEstimation_Tab3(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    int NumSpecies;
    int NumGuilds;
    int NumGuildTableViews   = 0;
    int NumSpeciesTableViews = 0;
    QUiLoader loader;
    QStringList GuildList;
    QStringList SpeciesList;

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_SpeciesNames.clear();
    m_GuildNames.clear();
    m_CompetitionForm.clear();
    m_SModels.clear();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab3::nmfEstimation_Tab3");

    Estimation_Tabs = tabs;

    m_AlphaTables.push_back(nmfConstantsMSSPM::TableCompetitionAlpha);
    m_AlphaTables.push_back(nmfConstantsMSSPM::TableCompetitionAlphaMin);
    m_AlphaTables.push_back(nmfConstantsMSSPM::TableCompetitionAlphaMax);
    m_BetaSpeciesTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaSpecies);
    m_BetaSpeciesTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaSpeciesMin);
    m_BetaSpeciesTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaSpeciesMax);
    m_BetaGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuilds);
    m_BetaGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsMin);
    m_BetaGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsMax);
    m_BetaGuildsGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuilds);
    m_BetaGuildsGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMin);
    m_BetaGuildsGuildsTables.push_back(nmfConstantsMSSPM::TableCompetitionBetaGuildsGuildsMax);

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab03.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab3_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab3_Widget, tr("3. Competition Parameters"));

    Estimation_Tab3_CompetitionAlphaTV           = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionAlphaTV");
    Estimation_Tab3_CompetitionAlphaMinTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionAlphaMinTV");
    Estimation_Tab3_CompetitionAlphaMaxTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionAlphaMaxTV");
    Estimation_Tab3_CompetitionBetaSpeciesTV     = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaSpeciesTV");
    Estimation_Tab3_CompetitionBetaSpeciesMinTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaSpeciesMinTV");
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaSpeciesMaxTV");
    Estimation_Tab3_CompetitionBetaGuildsTV      = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsTV");
    Estimation_Tab3_CompetitionBetaGuildsMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsMinTV");
    Estimation_Tab3_CompetitionBetaGuildsMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsMaxTV");
    Estimation_Tab3_CompetitionBetaGuildsGuildsTV    = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsGuildsTV");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV");
    Estimation_Tab3_CompetitionAlphaLBL          = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionAlphaLBL");
    Estimation_Tab3_CompetitionAlphaMinLBL       = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionAlphaMinLBL");
    Estimation_Tab3_CompetitionAlphaMaxLBL       = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionAlphaMaxLBL");
    Estimation_Tab3_CompetitionBetaSpeciesLBL    = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaSpeciesLBL");
    Estimation_Tab3_CompetitionBetaSpeciesMinLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaSpeciesMinLBL");
    Estimation_Tab3_CompetitionBetaSpeciesMaxLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaSpeciesMaxLBL");
    Estimation_Tab3_CompetitionBetaGuildsLBL     = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsLBL");
    Estimation_Tab3_CompetitionBetaGuildsMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsMinLBL");
    Estimation_Tab3_CompetitionBetaGuildsMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsMaxLBL");
    Estimation_Tab3_CompetitionBetaGuildsGuildsLBL    = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsGuildsLBL");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMinLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMinLBL");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMaxLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsGuildsMaxLBL");
    Estimation_Tab3_CompetitionMinSP             = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab3_MinSP");
    Estimation_Tab3_CompetitionMaxSP             = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab3_MaxSP");
    Estimation_Tab3_CompetitionMainSP            = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab3_MainSP");
    Estimation_Tab3_PrevPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_PrevPB");
    Estimation_Tab3_NextPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_NextPB");
    Estimation_Tab3_LoadPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_LoadPB");
    Estimation_Tab3_SavePB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_SavePB");
    Estimation_Tab3_ImportPB                     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_ImportPB");
    Estimation_Tab3_ExportPB                     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_ExportPB");
    Estimation_Tab3_TransposePB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_TransposePB");
    Estimation_Tab3_TransposePB2                 = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_TransposePB2");
    Estimation_Tab3_CompetitionSB                = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab3_CompetitionSB");
    Estimation_Tab3_CompetitionSB2               = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab3_CompetitionSB2");
    Estimation_Tab3_MinMaxCMB                    = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab3_MinMaxCMB");
    Estimation_Tab3_MinMaxCMB2                   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab3_MinMaxCMB2");

    Estimation_Tab3_CompetitionAlphaTV->setToolTip("Initial Value for Alpha Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionAlphaMinTV->setToolTip("Minimum Value for Alpha Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionAlphaMaxTV->setToolTip("Maximum Value for Alpha Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaSpeciesTV->setToolTip("Initial Value for Beta (Species) Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaSpeciesMinTV->setToolTip("Minimum Value for Beta (Species) Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setToolTip("Maximum Value for Beta (Species) Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsTV->setToolTip("Initial Value for Beta (Guilds) Coeff of Column Guilds on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsMinTV->setToolTip("Minimum Value for Beta (Guilds) Coeff of Column Guilds on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsMaxTV->setToolTip("Maximum Value for Beta (Guilds) Coeff of Column Guilds on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsGuildsTV->setToolTip("Initial Value for Beta (Guilds) Coeff of Column Guilds on Row Guilds");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV->setToolTip("Minimum Value for Beta (Guilds) Coeff of Column Guilds on Row Guilds");
    Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV->setToolTip("Maximum Value for Beta (Guilds) Coeff of Column Guilds on Row Guilds");

    connect(Estimation_Tab3_PrevPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_PrevPB()));
    connect(Estimation_Tab3_NextPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_NextPB()));
    connect(Estimation_Tab3_LoadPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_LoadPB()));
    connect(Estimation_Tab3_SavePB,   SIGNAL(clicked()),
            this,                     SLOT(callback_SavePB()));
    connect(Estimation_Tab3_ImportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ImportPB()));
    connect(Estimation_Tab3_ExportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ExportPB()));
    connect(Estimation_Tab3_TransposePB,      SIGNAL(clicked()),
            this,                             SLOT(callback_TransposePB()));
    connect(Estimation_Tab3_TransposePB2,     SIGNAL(clicked()),
            this,                             SLOT(callback_TransposePB()));
    connect(Estimation_Tab3_CompetitionMinSP, SIGNAL(splitterMoved(int,int)),
            this,                             SLOT(callback_MinSplitterMoved(int,int)));
    connect(Estimation_Tab3_CompetitionMaxSP, SIGNAL(splitterMoved(int,int)),
            this,                             SLOT(callback_MaxSplitterMoved(int,int)));
    connect(Estimation_Tab3_CompetitionSB,    SIGNAL(valueChanged(int)),
            this,                             SLOT(callback_PctRangeSB(int)));
    connect(Estimation_Tab3_CompetitionSB2,   SIGNAL(valueChanged(int)),
            this,                             SLOT(callback_PctRangeSB(int)));
    connect(Estimation_Tab3_MinMaxCMB,        SIGNAL(currentTextChanged(QString)),
            this,                             SLOT(callback_MinMaxCMB(QString)));
    connect(Estimation_Tab3_MinMaxCMB2,       SIGNAL(currentTextChanged(QString)),
            this,                             SLOT(callback_MinMaxCMB(QString)));


    Estimation_Tab3_PrevPB->setText("\u25C1--");
    Estimation_Tab3_NextPB->setText("--\u25B7");

    m_TableViewsNoK.clear();
    m_TableViewsNoK.push_back(Estimation_Tab3_CompetitionAlphaTV);
    m_TableViewsNoK.push_back(Estimation_Tab3_CompetitionAlphaMinTV);
    m_TableViewsNoK.push_back(Estimation_Tab3_CompetitionAlphaMaxTV);
    m_TableViewsMsProd.clear();
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaSpeciesTV);
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaSpeciesMinTV);
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaSpeciesMaxTV);
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaGuildsTV);
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaGuildsMinTV);
    m_TableViewsMsProd.push_back(Estimation_Tab3_CompetitionBetaGuildsMaxTV);
    m_TableViewsAggProd.clear();
    m_TableViewsAggProd.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsTV);
    m_TableViewsAggProd.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV);
    m_TableViewsAggProd.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV);

    m_TableViews.clear();
    m_TableViews.push_back(Estimation_Tab3_CompetitionAlphaTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionAlphaMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionAlphaMaxTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMaxTV);
    NumSpeciesTableViews = m_TableViews.size();
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMaxTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV);
    NumGuildTableViews   = m_TableViews.size() - NumSpeciesTableViews;

//  NumSpecies = getSpecies().size();
//  NumGuilds  = getGuilds().size();
    m_DatabasePtr->getGuilds( m_Logger,NumGuilds, GuildList);
    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,SpeciesList);
    QStandardItemModel* smodel;
    for (int i=0; i<NumSpeciesTableViews; ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumSpecies);
        m_SModels.push_back(smodel);
        m_TableViews[i]->setModel(smodel);
    }
    for (int i=0; i<NumGuildTableViews; ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumGuilds);
        m_SModels.push_back(smodel);
        m_TableViews[NumSpeciesTableViews+i]->setModel(smodel);
    }

}


nmfEstimation_Tab3::~nmfEstimation_Tab3()
{

}

void
nmfEstimation_Tab3::clearWidgets()
{
    nmfUtilsQt::clearTableView(
        {Estimation_Tab3_CompetitionAlphaTV,
         Estimation_Tab3_CompetitionAlphaMinTV,
         Estimation_Tab3_CompetitionAlphaMaxTV,
         Estimation_Tab3_CompetitionBetaSpeciesTV,
         Estimation_Tab3_CompetitionBetaSpeciesMinTV,
         Estimation_Tab3_CompetitionBetaSpeciesMaxTV,
         Estimation_Tab3_CompetitionBetaGuildsTV,
         Estimation_Tab3_CompetitionBetaGuildsMinTV,
         Estimation_Tab3_CompetitionBetaGuildsMaxTV,
         Estimation_Tab3_CompetitionBetaGuildsGuildsTV,
         Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV,
         Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV});
}

//QStringList
//nmfEstimation_Tab3::getSpecies()
//{
//    QStringList speciesList;
//    std::vector<std::string> species;
//    m_DatabasePtr->getSpecies(m_Logger,species);
//    for (std::string aSpecies : species) {
//        speciesList << QString::fromStdString(aSpecies);
//    }
//    return speciesList;
//}
//QStringList
//nmfEstimation_Tab3::getGuilds()
//{
//    QStringList guildsList;
//    std::vector<std::string> guilds;
//    m_DatabasePtr->getGuilds(m_Logger,guilds);
//    for (std::string aGuild : guilds) {
//        guildsList << QString::fromStdString(aGuild);
//    }
//    return guildsList;
//}

void
nmfEstimation_Tab3::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab3::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab3::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab3::callback_LoadPB()
{
    if (loadWidgets()) {
        QMessageBox::information(Estimation_Tabs, "Competition Load",
                                 "\nCompetition table(s) successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab3::callback_MinSplitterMoved(int pos, int index)
{
   Estimation_Tab3_CompetitionMaxSP->setSizes(Estimation_Tab3_CompetitionMinSP->sizes());
}

void
nmfEstimation_Tab3::callback_MaxSplitterMoved(int pos, int index)
{
   Estimation_Tab3_CompetitionMinSP->setSizes(Estimation_Tab3_CompetitionMaxSP->sizes());
}

void
nmfEstimation_Tab3::callback_CompetitionFormChanged(QString competitionForm)
{
    QList<int> minSizeList;
    QList<int> maxSizeList;
    int numSections  = Estimation_Tab3_CompetitionMinSP->sizes().size();
    int defaultWidth = Estimation_Tab3_CompetitionMinSP->width()/numSections;

    m_CompetitionForm = competitionForm;
    for (int i=0; i<numSections; ++i) {
        minSizeList.push_back(defaultWidth);
        maxSizeList.push_back(defaultWidth);
    }
    Estimation_Tab3_CompetitionMinSP->setSizes(minSizeList);
    Estimation_Tab3_CompetitionMaxSP->setSizes(maxSizeList);
    Estimation_Tab3_CompetitionMainSP->setSizes(maxSizeList);

    Estimation_Tab3_CompetitionAlphaTV->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaLBL->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMaxLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMaxLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsGuildsLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsGuildsMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsGuildsMaxLBL->setEnabled(false);

    if (isNoK()) {
        Estimation_Tab3_CompetitionAlphaTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaLBL->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxLBL->setEnabled(true);
        for (int i=1; i<numSections; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    } else if (isMsProd()) {
        Estimation_Tab3_CompetitionBetaSpeciesTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMaxLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(true);
        minSizeList[0] = 0;
        maxSizeList[0] = 0;
        minSizeList[numSections-1] = 0;
        maxSizeList[numSections-1] = 0;
    } else if (isAggProd()) {
        Estimation_Tab3_CompetitionBetaGuildsGuildsTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsGuildsMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsGuildsMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsGuildsLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsGuildsMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsGuildsMaxLBL->setEnabled(true);
        for (int i=0; i<numSections-1; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    }
    Estimation_Tab3_CompetitionMinSP->setSizes(minSizeList);
    Estimation_Tab3_CompetitionMaxSP->setSizes(maxSizeList);
    Estimation_Tab3_CompetitionMainSP->setSizes(maxSizeList);
}



void
nmfEstimation_Tab3::callback_ImportPB()
{
    QString tag="";
    QString selectMsg;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<std::string> allTableNames = getAllTableNames();
    int numTables = allTableNames.size();

    // Load default CSV files
    std::string msg = "\nLoad default Competition .csv files?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Competition CSV Files"),
                                                              tr(msg.c_str()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        loadCSVFiles(allTableNames);
    } else {
        std::string filePrefix = (isNoK()) ? nmfConstantsMSSPM::TableCompetitionAlpha :
                                         (isMsProd()) ? nmfConstantsMSSPM::TableCompetitionBetaSpecies :
                                                        nmfConstantsMSSPM::TableCompetitionBetaGuildsGuilds;
        // if no, raise browser and have user select appropriate file.
        selectMsg = "Select "+QString::fromStdString(filePrefix)+"*.csv file";
        QString filename = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr(selectMsg.toLatin1()), inputDataPath,
                    QObject::tr("Data Files (competition*.csv Competition*.csv)"));
        QFileInfo fi(filename);
        if (nmfUtilsQt::extractTag(fi.baseName(),tag)) {
            for (int i=0; i<numTables; ++i) {
                allTableNames[i] += "_"+tag.toStdString();
            }
            loadCSVFiles(allTableNames);
        } else if (! filename.isEmpty()){
            QMessageBox::information(Estimation_Tabs, "Competition CSV Import",
                                     "\nPlease make sure to select the filename that begins with:\n" + QString::fromStdString(filePrefix) + "\n",
                                     QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab3::loadCSVFiles(const std::vector<std::string>& allTableNames)
{
    bool loadOK;
    QString errorMsg;
    QString tableName;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::pair<int,int> nonZeroCell;
    std::vector<QTableView* > tableViews = getTableViews();

    for (int i=0; i<int(allTableNames.size()); ++i) {
        tableName = QString::fromStdString(allTableNames[i]);
        tableName = QDir(inputDataPath).filePath(tableName+".csv");
        loadOK = nmfUtilsQt::loadTimeSeries(
                    Estimation_Tabs, tableViews[i], inputDataPath, tableName,
                    nmfConstantsMSSPM::FirstLineNotReadOnly,
                    nmfConstantsMSSPM::ScientificNotation,
                    nonZeroCell,
                    errorMsg);
        if (! loadOK) {
            m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        }
    }
    resetSpinBox(nonZeroCell);
}

std::vector<std::string>
nmfEstimation_Tab3::getAllTableNames()
{
    std::vector<std::string> allTableNames;
    std::vector<std::vector<std::string> > tableList;

    if (isNoK()) {
        tableList = {m_AlphaTables};
    } else if (isMsProd()) {
        tableList = {m_BetaSpeciesTables,m_BetaGuildsTables};
    } else if (isAggProd()) {
        tableList = {m_BetaGuildsGuildsTables};
    }
    for (std::vector<std::string> tableVec : tableList)
    {
        for (std::string tableName : tableVec) {
            allTableNames.push_back(tableName);
        }
    }

    return allTableNames;
}

void
nmfEstimation_Tab3::callback_SavePB()
{
    bool systemFound = false;
    int  tableInc    = -1;
    std::vector<std::string> allTableNames = getAllTableNames();
    int numTables = allTableNames.size();
    std::string cmd;
    std::string errorMsg;
    std::string value;
    std::string Algorithm,Minimizer,ObjectiveCriterion;
    std::string Scaling,CompetitionForm;
    QString msg;
    QModelIndex index;
    boost::numeric::ublas::matrix<double> CompetitionMatrix[numTables];
    std::vector<std::string> GuildTables;
    std::string VariableNames;
    std::string speciesName;
    std::string guildName;

    readSettings();

    // Find Algorithm type
    systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                Estimation_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Couldn't find any System algorithms");
        return;
    }
    if (m_SModels.size() == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"nmfEstimation_Tab3::callback_SavePB m_SModels is empty.");
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Alpha
    if (isNoK()) {

        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_SModels[tableInc+2],m_SModels[tableInc+3])) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }

        for (unsigned int k=0; k<m_AlphaTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " +
                   m_AlphaTables[tableInc] +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " +
                   m_AlphaTables[tableInc] +
                  " (ProjectName,ModelName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_SModels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_SModels[tableInc]->columnCount(); ++ j) {
                    index = m_SModels[tableInc]->index(i,j);
                    value = index.data().toString().replace(",","").toStdString();
                    cmd += "('"  + m_ProjectName +
                           "','" + m_ModelName   +
                           "','" + m_SpeciesNames[i].toStdString() +
                           "','" + m_SpeciesNames[j].toStdString() +
                           "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB(1): Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }


    // Beta Species
    if (isMsProd()) {
        tableInc = 2;
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_SModels[tableInc+2],m_SModels[tableInc+3])) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }

        for (unsigned int k=0; k<m_BetaSpeciesTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " +
                   m_BetaSpeciesTables[k] +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " +
                   m_BetaSpeciesTables[k] +
                  " (ProjectName,ModelName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_SModels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_SModels[tableInc]->columnCount(); ++ j) {
                    index = m_SModels[tableInc]->index(i,j);
                    value = index.data().toString().replace(",","").toStdString();
                    cmd += "('"  + m_ProjectName +
                           "','" + m_ModelName   +
                           "','" + m_SpeciesNames[i].toStdString() +
                           "','" + m_SpeciesNames[j].toStdString() +
                           "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB(2): Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    // Beta Guilds
    if (isMsProd() || isAggProd()) {
        std::vector<QTableView* > tableViews = getTableViews();
        QStandardItemModel* smodel;
        QStandardItemModel* smodel2 = qobject_cast<QStandardItemModel*>(tableViews[1]->model());
        QStandardItemModel* smodel3 = qobject_cast<QStandardItemModel*>(tableViews[2]->model());
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(smodel2,smodel3)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 5] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        if (isMsProd()) {
            GuildTables   = m_BetaGuildsTables;
            VariableNames = "Guild,SpeName";
        } else {
            GuildTables   = m_BetaGuildsGuildsTables;
            VariableNames = "GuildA,GuildB";
        }
        for (unsigned int k=0; k<GuildTables.size(); ++k) {
            smodel = qobject_cast<QStandardItemModel*>(tableViews[k+3]->model());
            cmd = "DELETE FROM "  +
                   GuildTables[k] +
                  " WHERE ProjectName = '" + m_ProjectName +
                  "' AND ModelName = '"    + m_ModelName   + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 6] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
            cmd = "INSERT INTO " + GuildTables[k] + " (ProjectName,ModelName,"+VariableNames+",Value) VALUES ";
            for (int i=0; i<smodel->rowCount(); ++i) {
                for (int j=0; j<smodel->columnCount(); ++ j) {
                    index = smodel->index(i,j);
                    value = index.data().toString().replace(",","").toStdString();
                    if (isMsProd()) {
                        cmd += "('"  + m_ProjectName +
                               "','" + m_ModelName   +
                               "','" + m_SpeciesNames[i].toStdString() +
                               "','" + m_GuildNames[j].toStdString()   +
                               "', " + value + "),";
                    } else {
                        cmd += "('"  + m_ProjectName +
                               "','" + m_ModelName   +
                               "','" + m_GuildNames[i].toStdString() +
                               "','" + m_GuildNames[j].toStdString() +
                               "', " + value + "),";
                    }
                }
            }

            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB(3): Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }





    QMessageBox::information(Estimation_Tabs, "Competition Updated",
                             "\nCompetition tables have been successfully updated.\n",
                             QMessageBox::Ok);

    // Reload widgets so column sizing will re-adjust
    loadWidgets();

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfEstimation_Tab3::callback_ExportPB()
{
    std::vector<std::string> allTableNames = getAllTableNames();
    int numTables = allTableNames.size();

    // Save time series data to a .csv file
    if (int(allTableNames.size()) == numTables) {
        std::string msg = "\nOK to use default file names for Competition .csv files and overwrite any previous files?";
        QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Competition CSV Files"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                                  QMessageBox::Yes);
        if (reply == QMessageBox::Cancel) {
            return;
        } else if (reply == QMessageBox::Yes) {
            saveCSVFiles(allTableNames);
        } else {
            bool ok;
            QString tag = QInputDialog::getText(Estimation_Tabs, tr("Competition Files"),
                                                 tr("Enter Competition CSV filename version tag: "), QLineEdit::Normal,
                                                 "", &ok);
            if (ok && !tag.isEmpty()) {
                 for (int i=0; i<numTables; ++i) {
                     allTableNames[i] += "_"+tag.toStdString();
                 }
                 saveCSVFiles(allTableNames);
            } else if (tag.isEmpty()) {
                QMessageBox::warning(Estimation_Tabs, "Tag Error",
                                     "\nError: Please enter a valid (i.e., non-blank) tag.\n",
                                     QMessageBox::Ok);
            }
        }
    } else {
        m_Logger->logMsg(nmfConstants::Error,"Error: allTablesNames different size than m_TableViews");
    }
}

std::vector<QTableView* >
nmfEstimation_Tab3::getTableViews()
{
    std::vector<QTableView* > tableViews = {};

    tableViews = m_TableViewsNoK;
    if (isMsProd()) {
        tableViews = m_TableViewsMsProd;
    } else if (isAggProd()) {
        tableViews = m_TableViewsAggProd;
    }
    return tableViews;
}

void
nmfEstimation_Tab3::callback_TransposePB()
{
    std::vector<QTableView*> tableViews = getTableViews();

    for (QTableView* tv : tableViews) {
        nmfUtilsQt::transposeModel(tv);
        tv->resizeColumnsToContents();
    }

    // Reset model vector
    for (int i=0; i<int(m_TableViews.size()); ++i) {
        m_SModels[i] = qobject_cast<QStandardItemModel*>(m_TableViews[i]->model());
    }
}

void
nmfEstimation_Tab3::saveCSVFiles(
        const std::vector<std::string>& allTableNames)
{
    bool okSave;
    bool atLeastOneError = false;
    QString tableNameWithPath;
    QString savedFilenames;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<QTableView* > tableViews = getTableViews();
    QStandardItemModel* smodel;

    for (int i=0; i<int(allTableNames.size()); ++i) {
        tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(allTableNames[i]));
        smodel = qobject_cast<QStandardItemModel*>(tableViews[i]->model());
        okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
        savedFilenames += tableNameWithPath+"\n";
        if (!okSave) {
            atLeastOneError = true;
        }
    }
    if (atLeastOneError) {
        QMessageBox::information(Estimation_Tabs, "Competition Files Saved",
                                 "\nAll Competition CSV files have not been saved. Please check for valid filenames.\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "Competition Files Saved",
                                 "\nCompetition CSV files have been successfully saved as:\n\n"+savedFilenames,
                                 QMessageBox::Ok);
    }

}

void
nmfEstimation_Tab3::callback_MinMaxCMB(QString rangeType)
{
    QString senderName = qobject_cast<QComboBox* >(QObject::sender())->objectName();
    if (senderName == "Estimation_Tab3_MinMaxCMB") {
        Estimation_Tab3_MinMaxCMB2->blockSignals(true);
        Estimation_Tab3_MinMaxCMB2->setCurrentText(rangeType);
        Estimation_Tab3_MinMaxCMB2->blockSignals(false);
    } else if (senderName == "Estimation_Tab3_MinMaxCMB2") {
        Estimation_Tab3_MinMaxCMB->blockSignals(true);
        Estimation_Tab3_MinMaxCMB->setCurrentText(rangeType);
        Estimation_Tab3_MinMaxCMB->blockSignals(false);
    }

}


void
nmfEstimation_Tab3::callback_PctRangeSB(int pctValue)
{
    double pct = pctValue/100.0;
    QString rangeType = "";
    QTableView* tableView1 = nullptr;
    QTableView* tableView2 = nullptr;
    QTableView* tableView3 = nullptr;
    QTableView* tableView4 = nullptr;

    // The sender can be 1 of 2 spin boxes...this just keeps them in sync.
    QString senderName = qobject_cast<QSpinBox* >(QObject::sender())->objectName();
    if (senderName == "Estimation_Tab3_CompetitionSB") {
        rangeType = Estimation_Tab3_MinMaxCMB->currentText();
        Estimation_Tab3_CompetitionSB2->blockSignals(true);
        Estimation_Tab3_CompetitionSB2->setValue(pctValue);
        Estimation_Tab3_CompetitionSB2->blockSignals(false);
    } else {
        rangeType = Estimation_Tab3_MinMaxCMB2->currentText();
        Estimation_Tab3_CompetitionSB->blockSignals(true);
        Estimation_Tab3_CompetitionSB->setValue(pctValue);
        Estimation_Tab3_CompetitionSB->blockSignals(false);
    }

    if (isNoK()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsNoK[1],m_TableViewsNoK[2],tableView1,tableView2);
        if (nmfUtilsQt::thereAreSelections(tableView1,tableView2)) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsNoK[0],tableView1,tableView2);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsNoK[0],tableView1,tableView2);
        }
    } else if (isMsProd()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsMsProd[1],m_TableViewsMsProd[2],tableView1,tableView2);
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsMsProd[4],m_TableViewsMsProd[5],tableView3,tableView4);
        bool thereAreSelectionsBetaSpecies = nmfUtilsQt::thereAreSelections(tableView1,tableView2);
        bool thereAreSelectionsBetaGuilds  = nmfUtilsQt::thereAreSelections(tableView3,tableView4);

        if (thereAreSelectionsBetaSpecies || thereAreSelectionsBetaGuilds) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsMsProd[0],tableView1,tableView2);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsMsProd[0],tableView1,tableView2);
        }
        if (thereAreSelectionsBetaSpecies || thereAreSelectionsBetaGuilds) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsMsProd[3],tableView3,tableView4);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsMsProd[3],tableView3,tableView4);
        }
    } else if (isAggProd()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsAggProd[1],m_TableViewsAggProd[2],tableView1,tableView2);
        if (nmfUtilsQt::thereAreSelections(tableView1,tableView2)) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsAggProd[0],tableView1,tableView2);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsAggProd[0],tableView1,tableView2);
        }
    }
}


void
nmfEstimation_Tab3::assignTableViewsBasedOnRangeType(const QString& rangeType,
                                                     QTableView* possibleTableView1,
                                                     QTableView* possibleTableView2,
                                                     QTableView*& tableView1,
                                                     QTableView*& tableView2)
{
    tableView1 = nullptr;
    tableView2 = nullptr;
    if (rangeType == "min/max") {
        tableView1 = possibleTableView1;
        tableView2 = possibleTableView2;
    } else if (rangeType == "min only") {
        tableView1 = possibleTableView1;
    } else if (rangeType == "max only") {
        tableView2 = possibleTableView2;
    }
}

bool
nmfEstimation_Tab3::isNoK()
{
    return (m_CompetitionForm == "NO_K");
}

bool
nmfEstimation_Tab3::isAggProd()
{
    return (m_CompetitionForm == "AGG-PROD");
}

bool
nmfEstimation_Tab3::isMsProd()
{
    return (m_CompetitionForm == "MS-PROD");
}

bool
nmfEstimation_Tab3::isNull()
{
    return (m_CompetitionForm == "Null");
}

bool
nmfEstimation_Tab3::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab3::loadWidgets()");

    int m;
    int NumSpecies;
    int NumGuilds;
    int NumRecords;
    int tableInc = -1;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string competitionForm = "";
    QStandardItem*      item;
    QStandardItemModel* smodel;
    std::vector<std::string> GuildTables;
    std::string VariableNames;
    std::string speciesName;
    std::string guildName;
    std::string dataMapStr;
    std::pair<int,int> nonZeroCell;
    QStringList vListSpecies;
    QStringList vListGuilds;

    m_SpeciesNames.clear();
    m_GuildNames.clear();

    readSettings();

    if (m_ModelName.empty()) {
        m_Logger->logMsg(nmfConstants::Warning,"nmfEstimation_Tab3::loadWidgets: No Model found.");
    }

    clearWidgets();
    callback_CompetitionFormChanged(m_CompetitionForm);
    if (isNull()) {
        return true;
    }

    // Get Species and Guild names
    m_DatabasePtr->getGuilds( m_Logger,NumGuilds, m_GuildNames);
    m_DatabasePtr->getSpecies(m_Logger,NumSpecies,m_SpeciesNames);
//  m_SpeciesNames = getSpecies();
//  m_GuildNames   = getGuilds();
//  NumSpecies     = m_SpeciesNames.size();
//  NumGuilds      = m_GuildNames.size();
    for (QString species : m_SpeciesNames) {
        vListSpecies << " " + species + " "; // for aesthetics (and because vertical header looks better when displaying years)
    }
    for (QString guild : m_GuildNames) {
        vListGuilds << " " + guild + " "; // for aesthetics (and because vertical header looks better when displaying years)
    }

    if (isNoK()) {
        for (unsigned int k=0; k<m_AlphaTables.size(); ++k) {
            ++tableInc;
            fields    = {"ProjectName","ModelName","SpeciesA","SpeciesB","Value"};
            queryStr  = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " +
                         m_AlphaTables[k] +
                        " WHERE ProjectName = '" + m_ProjectName +
                        "' AND ModelName = '"    + m_ModelName   + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = dataMap["SpeciesA"].size();
            m = 0;
            for (int i=0; i<NumSpecies; ++i) {
                for (int j=0; j<NumSpecies; ++j) {
                    if ((NumRecords > 0) && (m < NumRecords)) {
                        dataMapStr = dataMap["Value"][m++];
                        if (std::stod(dataMapStr) != 0) {
                            nonZeroCell = std::make_pair(i,j);
                        }
                        item = new QStandardItem(QString::fromStdString(dataMapStr));
                    } else {
                        item = new QStandardItem("");
                    }
                    item->setTextAlignment(Qt::AlignCenter);
                    m_SModels[tableInc]->setItem(i, j, item);
                }
            }
            m_SModels[tableInc]->setVerticalHeaderLabels(vListSpecies);
            m_SModels[tableInc]->setHorizontalHeaderLabels(m_SpeciesNames);
            m_TableViews[tableInc]->resizeColumnsToContents();
        }
        resetSpinBox(nonZeroCell);
        return true;
    }

    if (isMsProd()) {
        for (unsigned int k=0; k<m_BetaSpeciesTables.size(); ++k) {
            QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(m_TableViewsMsProd[k]->model());
            tableInc = 3;
            NumRecords = 0;
            if (! m_ModelName.empty()) {
                fields    = {"ProjectName","ModelName","SpeciesA","SpeciesB","Value"};
                queryStr  = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " +
                             m_BetaSpeciesTables[k] +
                            " WHERE ProjectName = '" + m_ProjectName +
                            "' AND ModelName = '"    + m_ModelName   + "'";
                dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
                NumRecords = dataMap["SpeciesA"].size();
            }
            m = 0;
            for (int i=0; i<NumSpecies; ++i) {
                for (int j=0; j<NumSpecies; ++j) {
                    if ((NumRecords > 0) && (m < NumRecords)) {
                        dataMapStr = dataMap["Value"][m++];
                        if (std::stod(dataMapStr) != 0) {
                            nonZeroCell = std::make_pair(i,j);
                        }
                        item = new QStandardItem(QString::fromStdString(dataMapStr));
                    } else {
                        item = new QStandardItem("");
                    }
                    item->setTextAlignment(Qt::AlignCenter);
                    smodel->setItem(i, j, item);
                }
            }
            smodel->setVerticalHeaderLabels(vListSpecies);
            smodel->setHorizontalHeaderLabels(m_SpeciesNames);
            m_TableViewsMsProd[k]->resizeColumnsToContents();

        }
        resetSpinBox(nonZeroCell);
    }

    int NumSpeciesOrGuilds = (isAggProd()) ? NumGuilds : NumSpecies;

    if (isMsProd()) {
        GuildTables   = m_BetaGuildsTables;
        VariableNames = "Guild,SpeName";
        tableInc = 6;
    } else if (isAggProd()) {
        GuildTables   = m_BetaGuildsGuildsTables;
        VariableNames = "GuildA,GuildB";
        tableInc = 9;
    }

    for (unsigned int k=0; k<GuildTables.size(); ++k) {
        if (isMsProd()) {
           smodel = new QStandardItemModel(NumSpecies, NumGuilds);
           m_TableViewsMsProd[k+3]->setModel(smodel);
        } else if (isAggProd()) {
            smodel = new QStandardItemModel(NumGuilds, NumGuilds);
            m_TableViewsAggProd[k]->setModel(smodel);
        }
        NumRecords = 0;
        if (! m_ModelName.empty()) {
            fields    = {"ProjectName","ModelName","SpeName","Guild","Value"};
            queryStr  = "SELECT ProjectName,ModelName," + VariableNames + ",Value FROM " +
                         GuildTables[k] +
                        " WHERE ProjectName = '" + m_ProjectName +
                        "' AND ModelName = '"    + m_ModelName   + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = dataMap["SpeName"].size();
        }
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                if ((NumRecords > 0) && (m < NumRecords)) {
                    dataMapStr = dataMap["Value"][m++];
                    if (std::stod(dataMapStr) != 0) {
                        nonZeroCell = std::make_pair(i,j);
                    }
                    item = new QStandardItem(QString::fromStdString(dataMapStr));
                } else {
                    item = new QStandardItem("");
                }
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
        }
        smodel->setHorizontalHeaderLabels(m_GuildNames);
        if (isMsProd()) {
            smodel->setVerticalHeaderLabels(vListSpecies);
            m_TableViewsMsProd[k+3]->resizeColumnsToContents();
        } else if (isAggProd()) {
            smodel->setVerticalHeaderLabels(vListGuilds);
            m_TableViewsAggProd[k]->resizeColumnsToContents();
        }
    }
    resetSpinBox(nonZeroCell);

    return true;
}


void
nmfEstimation_Tab3::resetSpinBox(const std::pair<int,int>& nonZeroCell)
{
    int index = 0;
    int nonZeroRow = nonZeroCell.first;
    int nonZeroCol = nonZeroCell.second;

    if (isMsProd()) {
        index = 3;
    } else if (isAggProd()) {
        index = 9;
    }
    QModelIndex initValueIndex = m_SModels[index  ]->index(nonZeroRow,nonZeroCol);
    QModelIndex minValueIndex  = m_SModels[index+1]->index(nonZeroRow,nonZeroCol);
    QModelIndex maxValueIndex  = m_SModels[index+2]->index(nonZeroRow,nonZeroCol);
    double initValue   = initValueIndex.data().toString().replace(",","").toDouble();
    double minValue    = minValueIndex.data().toString().replace(",","").toDouble();
    double maxValue    = maxValueIndex.data().toString().replace(",","").toDouble();
    int    minPctValue = int(nmfUtils::round(100.0*(initValue-minValue)/initValue,0));
    int    maxPctValue = int(nmfUtils::round(100.0*(maxValue-initValue)/initValue,0));
    if (minPctValue == maxPctValue) {
        Estimation_Tab3_CompetitionSB->setValue(minPctValue);
    } else {
        Estimation_Tab3_CompetitionSB->blockSignals(true);
        Estimation_Tab3_CompetitionSB->setValue(0);
        Estimation_Tab3_CompetitionSB->blockSignals(false);
    }
}
