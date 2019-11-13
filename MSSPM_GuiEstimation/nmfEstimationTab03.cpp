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

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_speciesNames.clear();
    m_guildNames.clear();
    m_competitionForm.clear();
    m_smodels.clear();

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab3::nmfEstimation_Tab3");

    Estimation_Tabs = tabs;

    m_alphaTables.push_back("CompetitionAlphaMin");
    m_alphaTables.push_back("CompetitionAlphaMax");
    m_betaSpeciesTables.push_back("CompetitionBetaSpeciesMin");
    m_betaSpeciesTables.push_back("CompetitionBetaSpeciesMax");
    m_betaGuildsTables.push_back("CompetitionBetaGuildsMin");
    m_betaGuildsTables.push_back("CompetitionBetaGuildsMax");

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
            this,                             SLOT(callback_minSplitterMoved(int,int)));
    connect(Estimation_Tab3_CompetitionMaxSP, SIGNAL(splitterMoved(int,int)),
            this,                             SLOT(callback_maxSplitterMoved(int,int)));
    connect(Estimation_Tab3_EstimateCB,       SIGNAL(stateChanged(int)),
            this,                             SLOT(callback_EstimateChecked(int)));

    Estimation_Tab3_PrevPB->setText("\u25C1--");
    Estimation_Tab3_NextPB->setText("--\u25B7");

    m_QTableViews.clear();
    m_QTableViews.push_back(Estimation_Tab3_CompetitionAlphaMinTV);
    m_QTableViews.push_back(Estimation_Tab3_CompetitionAlphaMaxTV);
    m_QTableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMinTV);
    m_QTableViews.push_back(Estimation_Tab3_CompetitionBetaSpeciesMaxTV);
    NumSpeciesTableViews = m_QTableViews.size();
    m_QTableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMinTV);
    m_QTableViews.push_back(Estimation_Tab3_CompetitionBetaGuildsMaxTV);
    NumGuildTableViews   = m_QTableViews.size() - NumSpeciesTableViews;

    NumSpecies = getNumSpecies();
    NumGuilds  = getNumGuilds();
    QStandardItemModel* smodel;
    for (int i=0; i<NumSpeciesTableViews; ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumSpecies);
        m_smodels.push_back(smodel);
        m_QTableViews[i]->setModel(smodel);
    }
    for (int i=0; i<NumGuildTableViews; ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumGuilds);
        m_smodels.push_back(smodel);
        m_QTableViews[NumSpeciesTableViews+i]->setModel(smodel);
    }

} // end constructor


nmfEstimation_Tab3::~nmfEstimation_Tab3()
{

}

void
nmfEstimation_Tab3::clearWidgets()
{
    Estimation_Tab3_CompetitionAlphaMinTV->reset();
    Estimation_Tab3_CompetitionAlphaMaxTV->reset();
    Estimation_Tab3_CompetitionBetaSpeciesMinTV->reset();
    Estimation_Tab3_CompetitionBetaSpeciesMaxTV->reset();
    Estimation_Tab3_CompetitionBetaGuildsMinTV->reset();
    Estimation_Tab3_CompetitionBetaGuildsMaxTV->reset();
}

int
nmfEstimation_Tab3::getNumSpecies()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"SpeName"};
    queryStr = "SELECT SpeName FROM Species";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);

    return dataMap["SpeName"].size();
}

int
nmfEstimation_Tab3::getNumGuilds()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"GuildName"};
    queryStr = "SELECT GuildName FROM Guilds";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);

    return dataMap["GuildName"].size();
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
nmfEstimation_Tab3::callback_minSplitterMoved(int pos, int index)
{
   Estimation_Tab3_CompetitionMaxSP->setSizes(Estimation_Tab3_CompetitionMinSP->sizes());
}

void
nmfEstimation_Tab3::callback_maxSplitterMoved(int pos, int index)
{
   Estimation_Tab3_CompetitionMinSP->setSizes(Estimation_Tab3_CompetitionMaxSP->sizes());
}

void
nmfEstimation_Tab3::getAlgorithm(std::string &Algorithm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get current algorithm and run its estimation routine
    fields     = {"Algorithm"};
    queryStr   = "SELECT Algorithm FROM Systems WHERE SystemName='" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["Algorithm"].size() == 0) {
        m_logger->logMsg(nmfConstants::Error,"Error: No algorithm type found in Systems table. Please click Save on Setup Tab 3.");
        return;
    }
    Algorithm = dataMap["Algorithm"][0];
}

void
nmfEstimation_Tab3::callback_CompetitionFormChanged(QString competitionForm)
{
    QList<int> minSizeList;
    QList<int> maxSizeList;
    int numSections  = Estimation_Tab3_CompetitionMinSP->sizes().size();
    int defaultWidth = Estimation_Tab3_CompetitionMinSP->width()/3;

    m_competitionForm = competitionForm;
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

    if (m_competitionForm == "NO_K") {
        Estimation_Tab3_CompetitionAlphaMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionAlphaMaxLBL->setEnabled(true);
        for (int i=1; i<numSections; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    } else if (m_competitionForm == "AGG-PROD") {
        Estimation_Tab3_CompetitionBetaGuildsMinTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxTV->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMinLBL->setEnabled(true);
        Estimation_Tab3_CompetitionBetaGuildsMaxLBL->setEnabled(true);
        for (int i=0; i<numSections-1; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
    } else if (m_competitionForm == "MS-PROD") {
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
    int inc        = -1;
    int tableInc   = -1;
    int numSpecies = m_speciesNames.size();
    int numGuilds  = m_guildNames.size();
    int numTables  = m_alphaTables.size() +
                     m_betaSpeciesTables.size() +
                     m_betaGuildsTables.size();
    std::string cmd;
    std::string errorMsg;
    std::string value;
    QString msg;
    QModelIndex index;
    boost::numeric::ublas::matrix<double> MinMax[numTables];

    readSettings();

    // Find Algorithm type
    std::string Algorithm;
    getAlgorithm(Algorithm);

    if (m_smodels.size() == 0) {
        std::cout << "Warning: callback_SavePB m_smodels is empty." << std::endl;
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    for (unsigned i=0; i<m_alphaTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numSpecies);
    }
    for (unsigned i=0; i<m_betaSpeciesTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numSpecies);
    }
    for (unsigned i=0; i<m_betaGuildsTables.size(); ++i) {
        nmfUtils::initialize(MinMax[++inc], numSpecies, numGuilds);
    }

    // Alpha
    if (m_competitionForm == "NO_K") {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_smodels[tableInc+1],m_smodels[tableInc+2])) {
            m_logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_alphaTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_alphaTables[tableInc] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_alphaTables[tableInc] + " (SystemName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_smodels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_smodels[tableInc]->columnCount(); ++ j) {
                    index = m_smodels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_speciesNames[i].toStdString() + "','" +
                            m_speciesNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }

        // Check if user is running a Genetic Algorithm that if there's a value in MinMax[1], there's a
        // non-zero value in MinMax[0].
        for (int i=0; i<m_smodels[0]->rowCount(); ++i) {
            for (int j=0; j<m_smodels[0]->columnCount(); ++j) {
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
        tableInc += m_alphaTables.size();
    }

    // Beta Species
    if (m_competitionForm == "MS-PROD") {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_smodels[tableInc+1],m_smodels[tableInc+2])) {
            m_logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_betaSpeciesTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_betaSpeciesTables[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_betaSpeciesTables[k] + " (SystemName,SpeciesA,SpeciesB,Value) VALUES ";
            for (int i=0; i<m_smodels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_smodels[tableInc]->columnCount(); ++ j) {
                    index = m_smodels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_speciesNames[i].toStdString() + "','" +
                            m_speciesNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    } else {
        tableInc += m_betaSpeciesTables.size();
    }

    // Beta Guilds
    if ((m_competitionForm == "MS-PROD") || (m_competitionForm == "AGG-PROD")) {
        if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_smodels[tableInc+1],m_smodels[tableInc+2])) {
            m_logger->logMsg(nmfConstants::Error,"[Error 5] nmfEstimation_Tab3::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
            QMessageBox::critical(Estimation_Tabs, "Error",
                                 "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
        for (unsigned int k=0; k<m_betaGuildsTables.size(); ++k) {
            ++tableInc;
            cmd = "DELETE FROM " + m_betaGuildsTables[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"[Error 6] nmfEstimation_Tab3::callback_SavePB: DELETE error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from Competition table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_betaGuildsTables[k] + " (SystemName,SpeName,Guild,Value) VALUES ";
            for (int i=0; i<m_smodels[tableInc]->rowCount(); ++i) {
                for (int j=0; j<m_smodels[tableInc]->columnCount(); ++ j) {
                    index = m_smodels[tableInc]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax[tableInc](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + m_speciesNames[i].toStdString() + "','" +
                            m_guildNames[j].toStdString() + "', " + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab3::callback_SavePB: Write table error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
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
        for (unsigned k=0; k<m_QTableViews.size(); k += 2) {
            smodelMin = qobject_cast<QStandardItemModel*>(m_QTableViews[k]->model());
            smodelMax = qobject_cast<QStandardItemModel*>(m_QTableViews[k+1]->model());
            numRows   = m_QTableViews[k]->model()->rowCount();
            numCols   = m_QTableViews[k]->model()->columnCount();
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
std::cout << "nmfEstimation_Tab3::loadWidgets()" << std::endl;

    bool isAggProd = (m_competitionForm == "AGG-PROD");
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

    m_speciesNames.clear();
    m_guildNames.clear();

    readSettings();

    if (m_ProjectSettingsConfig.empty()) {
        m_logger->logMsg(nmfConstants::Warning,"nmfEstimation_Tab3::loadWidgets: No System found.");
    }

    // Get Species names
    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        m_speciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }

    // Get Guild names
    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName FROM Guilds";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    for (int j=0; j<NumGuilds; ++j) {
        m_guildNames << QString::fromStdString(dataMap["GuildName"][j]);
    }

    // Get competitionForm
    fields    = {"WithinGuildCompetitionForm"};
    queryStr  = "SELECT WithinGuildCompetitionForm FROM Systems WHERE ";
    queryStr += " SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["WithinGuildCompetitionForm"].size() > 0) {
        competitionForm = dataMap["WithinGuildCompetitionForm"][0];
    }

    for (unsigned int k=0; k<m_alphaTables.size(); ++k) {
        ++tableInc;
        fields    = {"SystemName","SpeciesA","SpeciesB","Value"};
        queryStr  = "SELECT SystemName,SpeciesA,SpeciesB,Value FROM " + m_alphaTables[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
        dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumCoeffs = dataMap["SpeciesA"].size();
        m = 0;
        for (int i=0; i<NumSpecies; ++i) {
            for (int j=0; j<NumSpecies; ++j) {
                if ((NumCoeffs > 0) && (m < NumCoeffs))
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                m_smodels[tableInc]->setItem(i, j, item);
            }
        }
        m_smodels[tableInc]->setVerticalHeaderLabels(m_speciesNames);
        m_smodels[tableInc]->setHorizontalHeaderLabels(m_speciesNames);
        m_QTableViews[tableInc]->resizeColumnsToContents();
    }

    for (unsigned int k=0; k<m_betaSpeciesTables.size(); ++k) {
        ++tableInc;
        NumCoeffs = 0;
        if (! m_ProjectSettingsConfig.empty()) {
            fields    = {"SystemName","SpeciesA","SpeciesB","Value"};
            queryStr  = "SELECT SystemName,SpeciesA,SpeciesB,Value FROM " + m_betaSpeciesTables[k] +
                        " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
                m_smodels[tableInc]->setItem(i, j, item);
            }
        }
        m_smodels[tableInc]->setVerticalHeaderLabels(m_speciesNames);
        m_smodels[tableInc]->setHorizontalHeaderLabels(m_speciesNames);
        m_QTableViews[tableInc]->resizeColumnsToContents();
    }
    int NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;

    for (unsigned int k=0; k<m_betaGuildsTables.size(); ++k) {
        ++tableInc;
        if (isAggProd) {
           smodel = new QStandardItemModel(NumGuilds, NumGuilds);
           m_smodels[tableInc] = smodel;
           m_QTableViews[tableInc]->setModel(smodel);
        }
        NumCoeffs = 0;
        if (! m_ProjectSettingsConfig.empty()) {
            fields    = {"SystemName","SpeName","Guild","Value"};
            queryStr  = "SELECT SystemName,SpeName,Guild,Value FROM " + m_betaGuildsTables[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
                m_smodels[tableInc]->setItem(i, j, item);
            }
        }
        if (isAggProd) {
            m_smodels[tableInc]->setVerticalHeaderLabels(m_guildNames);
        } else {
            m_smodels[tableInc]->setVerticalHeaderLabels(m_speciesNames);
        }
        m_smodels[tableInc]->setHorizontalHeaderLabels(m_guildNames);
        m_QTableViews[tableInc]->resizeColumnsToContents();
    }

    // Set enable-ness on widgets
    callback_CompetitionFormChanged(QString::fromStdString(competitionForm));

    return true;
}

