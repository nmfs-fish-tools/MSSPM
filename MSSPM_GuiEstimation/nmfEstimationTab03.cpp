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

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_SpeciesNames.clear();
    m_GuildNames.clear();
    m_CompetitionForm.clear();
    m_SModels.clear();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab3::nmfEstimation_Tab3");

    Estimation_Tabs = tabs;

    m_AlphaTables.push_back("CompetitionAlphaMin");
    m_AlphaTables.push_back("CompetitionAlphaMax");
    m_BetaSpeciesTables.push_back("CompetitionBetaSpeciesMin");
    m_BetaSpeciesTables.push_back("CompetitionBetaSpeciesMax");
    m_BetaGuildsTables.push_back("CompetitionBetaGuildsMin");
    m_BetaGuildsTables.push_back("CompetitionBetaGuildsMax");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab03.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab3_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab3_Widget, tr("3. Competition Parameters"));

    Estimation_Tab3_CompetitionAlphaMinTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionAlphaMinTV");
    Estimation_Tab3_CompetitionAlphaMaxTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionAlphaMaxTV");
    Estimation_Tab3_CompetitionBetaSpeciesMinTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaSpeciesMinTV");
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaSpeciesMaxTV");
    Estimation_Tab3_CompetitionBetaGuildsMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsMinTV");
    Estimation_Tab3_CompetitionBetaGuildsMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab3_CompetitionBetaGuildsMaxTV");
    Estimation_Tab3_CompetitionAlphaMinLBL       = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionAlphaMinLBL");
    Estimation_Tab3_CompetitionAlphaMaxLBL       = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionAlphaMaxLBL");
    Estimation_Tab3_CompetitionBetaSpeciesMinLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaSpeciesMinLBL");
    Estimation_Tab3_CompetitionBetaSpeciesMaxLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaSpeciesMaxLBL");
    Estimation_Tab3_CompetitionBetaGuildsMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsMinLBL");
    Estimation_Tab3_CompetitionBetaGuildsMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab3_CompetitionBetaGuildsMaxLBL");
    Estimation_Tab3_CompetitionMinSP             = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab3_CompetitionMinSP");
    Estimation_Tab3_CompetitionMaxSP             = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab3_CompetitionMaxSP");
    Estimation_Tab3_PrevPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_PrevPB");
    Estimation_Tab3_NextPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_NextPB");
    Estimation_Tab3_LoadPB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_LoadPB");
    Estimation_Tab3_SavePB                       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab3_SavePB");
    Estimation_Tab3_EstimateCB                   = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab3_EstimateCB");

    Estimation_Tab3_CompetitionAlphaMinTV->setToolTip("Minimum Value for Alpha Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionAlphaMaxTV->setToolTip("Maximum Value for Alpha Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaSpeciesMinTV->setToolTip("Minimum Value for Beta (Species) Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setToolTip("Maximum Value for Beta (Species) Coeff of Column Species on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsMinTV->setToolTip("Minimum Value for Beta (Guilds) Coeff of Column Guilds on Row Species");
    Estimation_Tab3_CompetitionBetaGuildsMaxTV->setToolTip("Maximum Value for Beta (Guilds) Coeff of Column Guilds on Row Species");

    connect(Estimation_Tab3_PrevPB, SIGNAL(clicked()),
            this,                   SLOT(callback_PrevPB()));
    connect(Estimation_Tab3_NextPB, SIGNAL(clicked()),
            this,                   SLOT(callback_NextPB()));
    connect(Estimation_Tab3_LoadPB, SIGNAL(clicked()),
            this,                   SLOT(callback_LoadPB()));
    connect(Estimation_Tab3_SavePB, SIGNAL(clicked()),
            this,                   SLOT(callback_SavePB()));
    connect(Estimation_Tab3_CompetitionMinSP, SIGNAL(splitterMoved(int,int)),
            this,                             SLOT(callback_MinSplitterMoved(int,int)));
    connect(Estimation_Tab3_CompetitionMaxSP, SIGNAL(splitterMoved(int,int)),
            this,                             SLOT(callback_MaxSplitterMoved(int,int)));
    connect(Estimation_Tab3_EstimateCB,       SIGNAL(stateChanged(int)),
            this,                             SLOT(callback_EstimateChecked(int)));

    Estimation_Tab3_PrevPB->setText("\u25C1--");
    Estimation_Tab3_NextPB->setText("--\u25B7");

    m_TableViews.clear();
    m_TableViews.push_back(Estimation_Tab3_CompetitionAlphaMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionAlphaMaxTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMaxTV);
    NumSpeciesTableViews = m_TableViews.size();
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMinTV);
    m_TableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMaxTV);
    NumGuildTableViews   = m_TableViews.size() - NumSpeciesTableViews;

    NumSpecies = getSpecies().size();
    NumGuilds  = getGuilds().size();
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
        {Estimation_Tab3_CompetitionAlphaMinTV,
         Estimation_Tab3_CompetitionAlphaMaxTV,
         Estimation_Tab3_CompetitionBetaSpeciesMinTV,
         Estimation_Tab3_CompetitionBetaSpeciesMaxTV,
         Estimation_Tab3_CompetitionBetaGuildsMinTV,
         Estimation_Tab3_CompetitionBetaGuildsMaxTV});
}

QStringList
nmfEstimation_Tab3::getSpecies()
{
    QStringList speciesList;
    std::vector<std::string> species;

    m_DatabasePtr->getAllSpecies(m_Logger,species);
    for (std::string aSpecies : species) {
        speciesList << QString::fromStdString(aSpecies);
    }

    return speciesList;
}

QStringList
nmfEstimation_Tab3::getGuilds()
{
    QStringList guildsList;
    std::vector<std::string> guilds;

    m_DatabasePtr->getAllGuilds(m_Logger,guilds);
    for (std::string aGuild : guilds) {
        guildsList << QString::fromStdString(aGuild);
    }

    return guildsList;
}

void
nmfEstimation_Tab3::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
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
    loadWidgets();
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
    int defaultWidth = Estimation_Tab3_CompetitionMinSP->width()/3;

    m_CompetitionForm = competitionForm;
    for (int i=0; i<numSections; ++i) {
        minSizeList.push_back(defaultWidth);
        maxSizeList.push_back(defaultWidth);
    }
    Estimation_Tab3_CompetitionMinSP->setSizes(minSizeList);
    Estimation_Tab3_CompetitionMaxSP->setSizes(maxSizeList);

    Estimation_Tab3_CompetitionAlphaMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionAlphaMaxLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaSpeciesMaxLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(false);
    Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(false);

    if (m_CompetitionForm == "NO_K") {
        Estimation_Tab3_CompetitionAlphaMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxLBL->setEnabled(true);
        for (int i=1; i<numSections; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    } else if (m_CompetitionForm == "AGG-PROD") {
        Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(true);
        for (int i=0; i<numSections-1; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    } else if (m_CompetitionForm == "MS-PROD") {
        Estimation_Tab3_CompetitionBetaSpeciesMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaSpeciesMaxLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(true);
        minSizeList[0] = 0;
        maxSizeList[0] = 0;
    }
    Estimation_Tab3_CompetitionMinSP->setSizes(minSizeList);
    Estimation_Tab3_CompetitionMaxSP->setSizes(maxSizeList);
}

void
nmfEstimation_Tab3::callback_SavePB()
{
    bool systemFound = false;
    int  inc         = -1;
    int  tableInc    = -1;
    int  numSpecies  = m_SpeciesNames.size();
    int  numGuilds   = m_GuildNames.size();
    int  numTables   = m_AlphaTables.size() +
                       m_BetaSpeciesTables.size() +
                       m_BetaGuildsTables.size();
    std::string cmd;
    std::string errorMsg;
    std::string value;
    std::string Algorithm,Minimizer,ObjectiveCriterion;
    std::string Scaling,CompetitionForm;
    QString msg;
    QModelIndex index;
    boost::numeric::ublas::matrix<double> MinMax[numTables];

    readSettings();

    // Find Algorithm type
    systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                Estimation_Tabs,m_Logger,m_ProjectSettingsConfig,
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

    for (unsigned i=0; i<m_AlphaTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numSpecies);
    }
    for (unsigned i=0; i<m_BetaSpeciesTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numSpecies);
    }
    for (unsigned i=0; i<m_BetaGuildsTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numGuilds);
    }

    // Alpha
    if (m_CompetitionForm == "NO_K") {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_SModels[tableInc+1],m_SModels[tableInc+2])) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_AlphaTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_AlphaTables[tableInc] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_AlphaTables[tableInc] + " (SystemName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_SModels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_SModels[tableInc]->columnCount(); ++ j) {
                    index = m_SModels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_SpeciesNames[i].toStdString() + "','" +
                            m_SpeciesNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }

        // Check if user is running a Genetic Algorithm that if there's a value in MinMax[1], there's a
        // non-zero value in MinMax[0].
        for (int i=0; i<m_SModels[0]->rowCount(); ++i) {
            for (int j=0; j<m_SModels[0]->columnCount(); ++j) {
                if ((Algorithm == "Genetic Algorithm") &&
                        (MinMax[1](i,j) > 0) && (MinMax[0](i,j) <= 0)) {
                    msg  = "\nWarning: All non-zero Max values must have a non-zero Min value.\n\n";
                    msg += "Please modify current table as needed.\n";
                    QMessageBox::warning(Estimation_Tabs, "Warning Min/Max Data",msg,
                                         QMessageBox::Ok);
                    Estimation_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
            }
        }
    } else {
        tableInc += m_AlphaTables.size();
    }

    // Beta Species
    if (m_CompetitionForm == "MS-PROD") {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_SModels[tableInc+1],m_SModels[tableInc+2])) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_BetaSpeciesTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_BetaSpeciesTables[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_BetaSpeciesTables[k] + " (SystemName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_SModels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_SModels[tableInc]->columnCount(); ++ j) {
                    index = m_SModels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_SpeciesNames[i].toStdString() + "','" +
                            m_SpeciesNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    } else {
        tableInc += m_BetaSpeciesTables.size();
    }

    // Beta Guilds
    if ((m_CompetitionForm == "MS-PROD") || (m_CompetitionForm == "AGG-PROD")) {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_SModels[tableInc+1],m_SModels[tableInc+2])) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 5] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_BetaGuildsTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_BetaGuildsTables[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"[Error 6] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_BetaGuildsTables[k] + " (SystemName,SpeName,Guild,Value) VALUES ";
            for (int i=0; i<m_SModels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_SModels[tableInc]->columnCount(); ++ j) {
                    index = m_SModels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_SpeciesNames[i].toStdString() + "','" +
                            m_GuildNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    QMessageBox::information(Estimation_Tabs, "Competition Min/Max Updated",
                             "\nCompetition Min/Max tables have been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}


void
nmfEstimation_Tab3::callback_EstimateChecked(int state)
{
    int numRows;
    int numCols;
    QModelIndex indexMin;
    QModelIndex indexMax;
    QStandardItemModel* smodelMin;
    QStandardItemModel* smodelMax;

    if (state != Qt::Checked) {
        for (unsigned k=0; k<m_TableViews.size(); k += 2) {
            smodelMin = qobject_cast<QStandardItemModel*>(m_TableViews[k]->model());
            smodelMax = qobject_cast<QStandardItemModel*>(m_TableViews[k+1]->model());
            numRows   = m_TableViews[k]->model()->rowCount();
            numCols   = m_TableViews[k]->model()->columnCount();
            for (int i=0; i<numRows; ++i) {
                for (int j=0; j<numCols;++j) {
                    indexMin = smodelMin->index(i,j);
                    indexMax = smodelMax->index(i,j);
                    smodelMax->setData(indexMax,smodelMin->data(indexMin));
                }
            }
        }
    } else {
        loadWidgets();
    }
}

bool
nmfEstimation_Tab3::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab3::loadWidgets()");

    bool isAggProd = (m_CompetitionForm == "AGG-PROD");
    int m;
    int NumSpecies;
    int NumGuilds;
    int NumCoeffs;
    int tableInc = -1;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string competitionForm = "";
    QStandardItem*      item;
    QStandardItemModel* smodel;

    m_SpeciesNames.clear();
    m_GuildNames.clear();

    readSettings();

    if (m_ProjectSettingsConfig.empty()) {
        m_Logger->logMsg(nmfConstants::Warning,"nmfEstimation_Tab3::loadWidgets: No System found.");
    }

    clearWidgets();

    // Get Species and Guild names
    m_SpeciesNames = getSpecies();
    m_GuildNames   = getGuilds();

    // Get competitionForm
    fields    = {"WithinGuildCompetitionForm"};
    queryStr  = "SELECT WithinGuildCompetitionForm FROM Systems WHERE ";
    queryStr += " SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["WithinGuildCompetitionForm"].size() > 0) {
        competitionForm = dataMap["WithinGuildCompetitionForm"][0];
    }

    for (unsigned int k=0; k<m_AlphaTables.size(); ++k) {
        ++tableInc;
        fields    = {"SystemName","SpeciesA","SpeciesB","Value"};
        queryStr  = "SELECT SystemName,SpeciesA,SpeciesB,Value FROM " + m_AlphaTables[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
        dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumCoeffs = dataMap["SpeciesA"].size();
        m = 0;
        for (int i=0; i<NumSpecies; ++i) {
            for (int j=0; j<NumSpecies; ++j) {
                if ((NumCoeffs > 0) && (m < NumCoeffs))
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                m_SModels[tableInc]->setItem(i, j, item);
            }
        }
        m_SModels[tableInc]->setVerticalHeaderLabels(m_SpeciesNames);
        m_SModels[tableInc]->setHorizontalHeaderLabels(m_SpeciesNames);
        m_TableViews[tableInc]->resizeColumnsToContents();
    }

    for (unsigned int k=0; k<m_BetaSpeciesTables.size(); ++k) {
        ++tableInc;
        NumCoeffs = 0;
        if (! m_ProjectSettingsConfig.empty()) {
            fields    = {"SystemName","SpeciesA","SpeciesB","Value"};
            queryStr  = "SELECT SystemName,SpeciesA,SpeciesB,Value FROM " + m_BetaSpeciesTables[k] +
                        " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumCoeffs = dataMap["SpeciesA"].size();
        }
        m = 0;
        for (int i=0; i<NumSpecies; ++i) {
            for (int j=0; j<NumSpecies; ++j) {
                if ((NumCoeffs > 0) && (m < NumCoeffs))
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                m_SModels[tableInc]->setItem(i, j, item);
            }
        }
        m_SModels[tableInc]->setVerticalHeaderLabels(m_SpeciesNames);
        m_SModels[tableInc]->setHorizontalHeaderLabels(m_SpeciesNames);
        m_TableViews[tableInc]->resizeColumnsToContents();
    }
    int NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;

    for (unsigned int k=0; k<m_BetaGuildsTables.size(); ++k) {
        ++tableInc;
        if (isAggProd) {
           smodel = new QStandardItemModel(NumGuilds, NumGuilds);
           m_SModels[tableInc] = smodel;
           m_TableViews[tableInc]->setModel(smodel);
        }
        NumCoeffs = 0;
        if (! m_ProjectSettingsConfig.empty()) {
            fields    = {"SystemName","SpeName","Guild","Value"};
            queryStr  = "SELECT SystemName,SpeName,Guild,Value FROM " + m_BetaGuildsTables[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumCoeffs = dataMap["SpeName"].size();
        }
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                if ((NumCoeffs > 0) && (m < NumCoeffs))
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                m_SModels[tableInc]->setItem(i, j, item);
            }
        }
        if (isAggProd) {
            m_SModels[tableInc]->setVerticalHeaderLabels(m_GuildNames);
        } else {
            m_SModels[tableInc]->setVerticalHeaderLabels(m_SpeciesNames);
        }
        m_SModels[tableInc]->setHorizontalHeaderLabels(m_GuildNames);
        m_TableViews[tableInc]->resizeColumnsToContents();
    }

    // Set enable-ness on widgets
    callback_CompetitionFormChanged(QString::fromStdString(competitionForm));

    return true;
}

