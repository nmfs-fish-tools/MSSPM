#include "nmfEstimationTab04.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab4::nmfEstimation_Tab4(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_predationForm.clear();

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab4::nmfEstimation_Tab4");

    Estimation_Tabs = tabs;

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab04.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab4_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab4_Widget, tr("4. Predation Parameters"));
    Estimation_Tab4_PredationMinTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_PredationMinTV");
    Estimation_Tab4_PredationMaxTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_PredationMaxTV");
    Estimation_Tab4_PredationMinSP  = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab4_PredationMinSP");
    Estimation_Tab4_PredationMaxSP  = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab4_PredationMaxSP");
    Estimation_Tab4_HandlingMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_HandlingMinTV");
    Estimation_Tab4_HandlingMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_HandlingMaxTV");
    Estimation_Tab4_ExponentMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_ExponentMinTV");
    Estimation_Tab4_ExponentMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_ExponentMaxTV");
    Estimation_Tab4_PredationMinLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_PredationMinLBL");
    Estimation_Tab4_PredationMaxLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_PredationMaxLBL");
    Estimation_Tab4_HandlingMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_HandlingMinLBL");
    Estimation_Tab4_HandlingMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_HandlingMaxLBL");
    Estimation_Tab4_ExponentMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_ExponentMinLBL");
    Estimation_Tab4_ExponentMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_ExponentMaxLBL");
    Estimation_Tab4_PrevPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_PrevPB");
    Estimation_Tab4_NextPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_NextPB");
    Estimation_Tab4_LoadPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_LoadPB");
    Estimation_Tab4_SavePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_SavePB");
    Estimation_Tab4_EstimateCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab4_EstimateCB");

    Estimation_Tab4_PredationMinTV->setToolTip("Minimum Value for Predation Effect of Column Predator on Row Prey");
    Estimation_Tab4_PredationMaxTV->setToolTip("Maximum Value for Predation Effect of Column Predator on Row Prey");
    Estimation_Tab4_HandlingMinTV->setToolTip("Minimum Value for Handling Time of Column Predator on Row Prey");
    Estimation_Tab4_HandlingMaxTV->setToolTip("Maximum Value for Handling Time of Column Predator on Row Prey");
    Estimation_Tab4_ExponentMinTV->setToolTip("Minimum Value for Predator Exponent Coefficient");
    Estimation_Tab4_ExponentMaxTV->setToolTip("Maximum Value for Predator Exponent Coefficient");

    connect(Estimation_Tab4_PrevPB,         SIGNAL(clicked(bool)),
            this,                           SLOT(callback_PrevPB()));
    connect(Estimation_Tab4_NextPB,         SIGNAL(clicked(bool)),
            this,                           SLOT(callback_NextPB()));
    connect(Estimation_Tab4_LoadPB,         SIGNAL(clicked(bool)),
            this,                           SLOT(callback_LoadPB()));
    connect(Estimation_Tab4_SavePB,         SIGNAL(clicked()),
            this,                           SLOT(callback_SavePB()));
    connect(Estimation_Tab4_PredationMinSP, SIGNAL(splitterMoved(int,int)),
            this,                           SLOT(callback_minSplitterMoved(int,int)));
    connect(Estimation_Tab4_PredationMaxSP, SIGNAL(splitterMoved(int,int)),
            this,                           SLOT(callback_maxSplitterMoved(int,int)));
    connect(Estimation_Tab4_EstimateCB,     SIGNAL(stateChanged(int)),
            this,                           SLOT(callback_EstimateChecked(int)));

    Estimation_Tab4_PrevPB->setText("\u25C1--");
    Estimation_Tab4_NextPB->setText("--\u25B7");

    m_QTableViews1d.clear();
    m_QTableViews1d.push_back(Estimation_Tab4_ExponentMinTV);
    m_QTableViews1d.push_back(Estimation_Tab4_ExponentMaxTV);
    m_QTableViews2d.clear();
    m_QTableViews2d.push_back(Estimation_Tab4_PredationMinTV);
    m_QTableViews2d.push_back(Estimation_Tab4_PredationMaxTV);
    m_QTableViews2d.push_back(Estimation_Tab4_HandlingMinTV);
    m_QTableViews2d.push_back(Estimation_Tab4_HandlingMaxTV);

    // Create models and attach to views
    int NumSpecies = getNumSpecies();
    QStandardItemModel* smodel;
    for (unsigned i=0; i<m_QTableViews1d.size(); ++i) {
        smodel = new QStandardItemModel(NumSpecies, 1);
        m_smodels1d.push_back(smodel);
        m_QTableViews1d[i]->setModel(smodel);
    }
    for (unsigned i=0; i<m_QTableViews2d.size(); ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumSpecies);
        m_smodels2d.push_back(smodel);
        m_QTableViews2d[i]->setModel(smodel);
    }

} // end constructor


nmfEstimation_Tab4::~nmfEstimation_Tab4()
{

}

void
nmfEstimation_Tab4::clearWidgets()
{
        nmfUtilsQt::clearTableView(
            {Estimation_Tab4_PredationMinTV,
             Estimation_Tab4_PredationMaxTV,
             Estimation_Tab4_HandlingMinTV,
             Estimation_Tab4_HandlingMaxTV,
             Estimation_Tab4_ExponentMinTV,
             Estimation_Tab4_ExponentMaxTV});
}

void
nmfEstimation_Tab4::callback_minSplitterMoved(int pos, int index)
{
   Estimation_Tab4_PredationMaxSP->setSizes(Estimation_Tab4_PredationMinSP->sizes());
}

void
nmfEstimation_Tab4::callback_maxSplitterMoved(int pos, int index)
{
   Estimation_Tab4_PredationMinSP->setSizes(Estimation_Tab4_PredationMaxSP->sizes());
}

void
nmfEstimation_Tab4::readSettings()
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
nmfEstimation_Tab4::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab4::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab4::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab4::getAlgorithm(std::string &Algorithm)
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
nmfEstimation_Tab4::callback_SavePB()
{
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::string value;
    std::vector<std::string> SpeNames;
    boost::numeric::ublas::matrix<double> MinMax1d[m_smodels1d.size()];
    boost::numeric::ublas::matrix<double> MinMax2d[m_smodels2d.size()];
    QString msg;

    readSettings();

    // Find Algorithm type
    std::string Algorithm;
    getAlgorithm(Algorithm);

    if (m_smodels2d.size() == 0) {
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Get Species names
    for (int j=0; j<m_smodels2d[0]->columnCount(); ++ j) {
        SpeNames.push_back(m_smodels2d[0]->horizontalHeaderItem(j)->text().toStdString());
    }

    // Initialize
    for (unsigned i=0; i<m_Tables2d.size(); ++i) {
        nmfUtils::initialize(MinMax2d[i], SpeNames.size(), SpeNames.size());
    }

    // Save 1d tables
    if (m_predationForm == "Type III")
    {
        for (unsigned i=0; i<m_Tables1d.size(); ++i) {
            nmfUtils::initialize(MinMax1d[i], SpeNames.size(), 1);
        }

        for (unsigned int k=0; k<m_Tables1d.size(); ++k) {
            if (k%2 == 0) { // if it's an even number (there can be 2 or 4 of these types of tables)
                if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_smodels1d[k],m_smodels1d[k+1])) {
                    m_logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab4::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
                    QMessageBox::critical(Estimation_Tabs, "Error",
                                          "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                          QMessageBox::Ok);
                    Estimation_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
            }
            cmd = "DELETE FROM " + m_Tables1d[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab4::callback_SavePB: DELETE error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Couldn't delete all records from " +
                                     QString::fromStdString(m_Tables1d[k]) + " table.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }

            cmd = "INSERT INTO " + m_Tables1d[k] + " (SystemName,SpeName,Value) VALUES ";
            for (int i=0; i<m_smodels1d[k]->rowCount(); ++i) {
                for (int j=0; j<m_smodels1d[k]->columnCount(); ++ j) {
                    index = m_smodels1d[k]->index(i,j);
                    value = index.data().toString().toStdString();
                    MinMax1d[k](i,j) = index.data().toDouble();
                    cmd += "('" + m_ProjectSettingsConfig + "','" + SpeNames[i] + "'," + value + "),";
                }
            }
            cmd = cmd.substr(0,cmd.size()-1);
            errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
            if (errorMsg != " ") {
                m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab4::callback_SavePB: Write table error: " + errorMsg);
                m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    // Save 2d tables
    for (unsigned int k=0; k<m_Tables2d.size(); ++k)
    {
        if (k%2 == 0) { // if it's an even number (there can be 2 or 4 of these types of tables)
            if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(m_smodels2d[k],m_smodels2d[k+1])) {
                m_logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab4::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
                QMessageBox::critical(Estimation_Tabs, "Error",
                                     "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
        cmd = "DELETE FROM " + m_Tables2d[k] + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab4::callback_SavePB: DELETE error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command.  Couldn't delete all records from " +
                                 QString::fromStdString(m_Tables2d[k]) + " table.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }

        cmd = "INSERT INTO " + m_Tables2d[k] + " (SystemName,SpeciesA,SpeciesB,Value) VALUES ";
        for (int i=0; i<m_smodels2d[k]->rowCount(); ++i) {
            for (int j=0; j<m_smodels2d[k]->columnCount(); ++ j) {
                index = m_smodels2d[k]->index(i,j);
                value = index.data().toString().toStdString();
                MinMax2d[k](i,j) = index.data().toDouble();
                cmd += "('" + m_ProjectSettingsConfig + "','" + SpeNames[i] + "','" + SpeNames[j] + "', " + value + "),";
            }
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab4::callback_SavePB: Write table error: " + errorMsg);
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
    for (int i=0; i<m_smodels2d[0]->rowCount(); ++i) {
        for (int j=0; j<m_smodels2d[0]->columnCount(); ++j) {
            if ((Algorithm == "Genetic Algorithm") &&
                    (MinMax2d[1](i,j) > 0) && (MinMax2d[0](i,j) <= 0)) {
                msg  = "\nWarning: All non-zero Max values must have a non-zero Min value.\n\n";
                msg += "Please modify current table as needed.\n";
                QMessageBox::warning(Estimation_Tabs, "Warning Min/Max Data",msg,
                                     QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
    }

    QMessageBox::information(Estimation_Tabs, "Predation Min/Max Updated",
                             "\nPredation Min/Max tables have been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}


void
nmfEstimation_Tab4::callback_PredationFormChanged(QString predationForm)
{
    QList<int> minSizeList;
    QList<int> maxSizeList;
    int numSections  = Estimation_Tab4_PredationMinSP->sizes().size();
    int defaultWidth = Estimation_Tab4_PredationMinSP->width()/3;

    m_predationForm = predationForm.toStdString();

    for (int i=0; i<numSections; ++i) {
        minSizeList.push_back(defaultWidth);
        maxSizeList.push_back(defaultWidth);
    }
    Estimation_Tab4_PredationMinSP->setSizes(minSizeList);
    Estimation_Tab4_PredationMaxSP->setSizes(maxSizeList);

    Estimation_Tab4_PredationMinTV->setEnabled(false);
    Estimation_Tab4_PredationMaxTV->setEnabled(false);
    Estimation_Tab4_HandlingMinTV->setEnabled(false);
    Estimation_Tab4_HandlingMaxTV->setEnabled(false);
    Estimation_Tab4_ExponentMinTV->setEnabled(false);
    Estimation_Tab4_ExponentMaxTV->setEnabled(false);
    Estimation_Tab4_PredationMinLBL->setEnabled(false);
    Estimation_Tab4_PredationMaxLBL->setEnabled(false);
    Estimation_Tab4_HandlingMinLBL->setEnabled(false);
    Estimation_Tab4_HandlingMaxLBL->setEnabled(false);
    Estimation_Tab4_ExponentMinLBL->setEnabled(false);
    Estimation_Tab4_ExponentMaxLBL->setEnabled(false);

    if (m_predationForm == "Type I") {
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        for (int i=1; i<numSections; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
        Estimation_Tab4_PredationMinSP->setSizes(minSizeList);
        Estimation_Tab4_PredationMaxSP->setSizes(maxSizeList);
    } else if (m_predationForm == "Type II") {
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        Estimation_Tab4_HandlingMinTV->setEnabled(true);
        Estimation_Tab4_HandlingMaxTV->setEnabled(true);
        Estimation_Tab4_HandlingMinLBL->setEnabled(true);
        Estimation_Tab4_HandlingMaxLBL->setEnabled(true);
        minSizeList[numSections-1] = 0;
        maxSizeList[numSections-1] = 0;
        Estimation_Tab4_PredationMinSP->setSizes(minSizeList);
        Estimation_Tab4_PredationMaxSP->setSizes(maxSizeList);
    } else if (m_predationForm == "Type III") {
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        Estimation_Tab4_HandlingMinTV->setEnabled(true);
        Estimation_Tab4_HandlingMaxTV->setEnabled(true);
        Estimation_Tab4_HandlingMinLBL->setEnabled(true);
        Estimation_Tab4_HandlingMaxLBL->setEnabled(true);
        Estimation_Tab4_ExponentMinTV->setEnabled(true);
        Estimation_Tab4_ExponentMaxTV->setEnabled(true);
        Estimation_Tab4_ExponentMinLBL->setEnabled(true);
        Estimation_Tab4_ExponentMaxLBL->setEnabled(true);
    }

    // Load table names
    m_Tables1d.clear();
    if (m_predationForm == "Type III") {
        m_Tables1d.push_back("PredationExponentMin");
        m_Tables1d.push_back("PredationExponentMax");
    }
    m_Tables2d.clear();
    m_Tables2d.push_back("PredationLossRatesMin");
    m_Tables2d.push_back("PredationLossRatesMax");
    if ((m_predationForm == "Type II") || (m_predationForm == "Type III")) {
        m_Tables2d.push_back("HandlingTimeMin");
        m_Tables2d.push_back("HandlingTimeMax");
    }
}

void
nmfEstimation_Tab4::callback_EstimateChecked(int state)
{
    int numRows;
    int numCols;
    QModelIndex indexMin;
    QModelIndex indexMax;
    QStandardItemModel* smodelMin;
    QStandardItemModel* smodelMax;
    std::vector<QTableView*>  tableViews;

    tableViews.insert(tableViews.end(),m_QTableViews1d.begin(),m_QTableViews1d.end());
    tableViews.insert(tableViews.end(),m_QTableViews2d.begin(),m_QTableViews2d.end());
    if (state != Qt::Checked) {
        for (unsigned k=0; k<tableViews.size(); k += 2) {
            smodelMin = qobject_cast<QStandardItemModel*>(tableViews[k]->model());
            smodelMax = qobject_cast<QStandardItemModel*>(tableViews[k+1]->model());
            numRows   = tableViews[k]->model()->rowCount();
            numCols   = tableViews[k]->model()->columnCount();
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


void
nmfEstimation_Tab4::getForms(std::string& predationForm, std::string& competitionForm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get forms
    fields    = {"PredationForm","WithinGuildCompetitionForm"};
    queryStr  = "SELECT PredationForm,WithinGuildCompetitionForm FROM Systems WHERE ";
    queryStr += " SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["PredationForm"].size() > 0) {
        predationForm   = dataMap["PredationForm"][0];
        competitionForm = dataMap["WithinGuildCompetitionForm"][0];
    }
}


int
nmfEstimation_Tab4::getNumSpecies()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"SpeName"};
    queryStr = "SELECT SpeName FROM Species";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);

    return dataMap["SpeName"].size();
}

bool
nmfEstimation_Tab4::loadWidgets()
{
std::cout << "nmfEstimation_Tab4::loadWidgets()" << std::endl;

    bool isAggProd;
    int m;
    int NumSpecies;
    int NumGuilds;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string predationForm   = "";
    std::string competitionForm = "";
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList GuildNames;
    QStringList exponentColName;
    QStandardItemModel* smodel;

    readSettings();

    clearWidgets();

    getForms(predationForm,competitionForm);
    isAggProd = (competitionForm == "AGG-PROD");

    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }
    fields     = {"GuildName"};
    queryStr   = "SELECT GuildName FROM Guilds";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds  = dataMap["GuildName"].size();
    for (int j=0; j<NumGuilds; ++j) {
        GuildNames << QString::fromStdString(dataMap["GuildName"][j]);
    }

    exponentColName << QString("b");
    int NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;

    // Load the 1d tables
    for (unsigned int k=0; k<m_Tables1d.size(); ++k) {
        if (isAggProd) {
            smodel = new QStandardItemModel(NumGuilds, 1);
            m_smodels1d[k] = smodel;
            m_QTableViews1d[k]->setModel(smodel);
        }
        fields    = {"SystemName","SpeName","Value"};
        queryStr  = "SELECT SystemName,SpeName,Value FROM " + m_Tables1d[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
        dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();

        // Load Predation tables
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            if (m < NumRecords)
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
            else
                item = new QStandardItem("");
            item->setTextAlignment(Qt::AlignCenter);
            m_smodels1d[k]->setItem(i, 0, item);
        }
        if (isAggProd) {
            m_smodels1d[k]->setVerticalHeaderLabels(GuildNames);
        } else {
            m_smodels1d[k]->setVerticalHeaderLabels(SpeciesNames);
        }
        m_smodels1d[k]->setHorizontalHeaderLabels(exponentColName);
        m_QTableViews1d[k]->resizeColumnsToContents();
    }

    // Load the 2d tables
    for (unsigned int k=0; k<m_Tables2d.size(); ++k) {
        if (isAggProd) {
            smodel = new QStandardItemModel(NumGuilds, NumGuilds);
            m_smodels2d[k] = smodel;
            m_QTableViews2d[k]->setModel(smodel);
        }
        fields    = {"SystemName","SpeciesA","SpeciesB","Value"};
        queryStr  = "SELECT SystemName,SpeciesA,SpeciesB,Value FROM " + m_Tables2d[k] +
                    " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
        dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeciesA"].size();

        // Load Predation tables
        m = 0;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                if (m < NumRecords)
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                m_smodels2d[k]->setItem(i, j, item);
            }
        }
        if (isAggProd) {
            m_smodels2d[k]->setVerticalHeaderLabels(GuildNames);
            m_smodels2d[k]->setHorizontalHeaderLabels(GuildNames);
        } else {
            m_smodels2d[k]->setVerticalHeaderLabels(SpeciesNames);
            m_smodels2d[k]->setHorizontalHeaderLabels(SpeciesNames);
        }
        m_QTableViews2d[k]->resizeColumnsToContents();
    }

    // Set enable-ness on widgets
    callback_PredationFormChanged(QString::fromStdString(predationForm));

    return true;
}
