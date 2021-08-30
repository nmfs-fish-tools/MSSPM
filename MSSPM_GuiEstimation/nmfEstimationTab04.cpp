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

    m_Logger      = logger;
    m_DatabasePtr = databasePtr;
    m_PredationForm.clear();
    m_ProjectName.clear();
    m_ModelName.clear();

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab4::nmfEstimation_Tab4");

    Estimation_Tabs = tabs;

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab04.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab4_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab4_Widget, tr("4. Predation Parameters"));
    Estimation_Tab4_PredationTV     = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_PredationTV");
    Estimation_Tab4_PredationMinTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_PredationMinTV");
    Estimation_Tab4_PredationMaxTV  = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_PredationMaxTV");
    Estimation_Tab4_MainSP          = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab4_MainSP");
    Estimation_Tab4_MinSP           = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab4_MinSP");
    Estimation_Tab4_MaxSP           = Estimation_Tabs->findChild<QSplitter   *>("Estimation_Tab4_MaxSP");
    Estimation_Tab4_HandlingTV      = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_HandlingTV");
    Estimation_Tab4_HandlingMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_HandlingMinTV");
    Estimation_Tab4_HandlingMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_HandlingMaxTV");
    Estimation_Tab4_ExponentTV      = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_ExponentTV");
    Estimation_Tab4_ExponentMinTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_ExponentMinTV");
    Estimation_Tab4_ExponentMaxTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab4_ExponentMaxTV");
    Estimation_Tab4_PredationLBL    = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_PredationLBL");
    Estimation_Tab4_PredationMinLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_PredationMinLBL");
    Estimation_Tab4_PredationMaxLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_PredationMaxLBL");
    Estimation_Tab4_HandlingLBL     = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_HandlingLBL");
    Estimation_Tab4_HandlingMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_HandlingMinLBL");
    Estimation_Tab4_HandlingMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_HandlingMaxLBL");
    Estimation_Tab4_ExponentLBL     = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_ExponentLBL");
    Estimation_Tab4_ExponentMinLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_ExponentMinLBL");
    Estimation_Tab4_ExponentMaxLBL  = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab4_ExponentMaxLBL");
    Estimation_Tab4_PrevPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_PrevPB");
    Estimation_Tab4_NextPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_NextPB");
    Estimation_Tab4_LoadPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_LoadPB");
    Estimation_Tab4_SavePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_SavePB");
    Estimation_Tab4_ImportPB        = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_ImportPB");
    Estimation_Tab4_ExportPB        = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_ExportPB");
    Estimation_Tab4_TransposePB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_TransposePB");
    Estimation_Tab4_TransposePB2    = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab4_TransposePB2");
    Estimation_Tab4_PredationSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab4_PredationSB");
    Estimation_Tab4_PredationSB2    = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab4_PredationSB2");
    Estimation_Tab4_MinMaxCMB       = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab4_MinMaxCMB");
    Estimation_Tab4_MinMaxCMB2      = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab4_MinMaxCMB2");

    Estimation_Tab4_PredationTV->setToolTip("Initial Values for Predation Effect of Column Predator on Row Prey");
    Estimation_Tab4_PredationMinTV->setToolTip("Minimum Values for Predation Effect of Column Predator on Row Prey");
    Estimation_Tab4_PredationMaxTV->setToolTip("Maximum Values for Predation Effect of Column Predator on Row Prey");
    Estimation_Tab4_HandlingTV->setToolTip("Initial Values for Handling Time of Column Predator on Row Prey");
    Estimation_Tab4_HandlingMinTV->setToolTip("Minimum Values for Handling Time of Column Predator on Row Prey");
    Estimation_Tab4_HandlingMaxTV->setToolTip("Maximum Values for Handling Time of Column Predator on Row Prey");
    Estimation_Tab4_ExponentTV->setToolTip("Initial Values for Predator Exponent Coefficient");
    Estimation_Tab4_ExponentMinTV->setToolTip("Minimum Values for Predator Exponent Coefficient");
    Estimation_Tab4_ExponentMaxTV->setToolTip("Maximum Values for Predator Exponent Coefficient");

    connect(Estimation_Tab4_PrevPB,       SIGNAL(clicked(bool)),
            this,                         SLOT(callback_PrevPB()));
    connect(Estimation_Tab4_NextPB,       SIGNAL(clicked(bool)),
            this,                         SLOT(callback_NextPB()));
    connect(Estimation_Tab4_LoadPB,       SIGNAL(clicked(bool)),
            this,                         SLOT(callback_LoadPB()));
    connect(Estimation_Tab4_SavePB,       SIGNAL(clicked()),
            this,                         SLOT(callback_SavePB()));
    connect(Estimation_Tab4_ImportPB,     SIGNAL(clicked()),
            this,                         SLOT(callback_ImportPB()));
    connect(Estimation_Tab4_ExportPB,     SIGNAL(clicked()),
            this,                         SLOT(callback_ExportPB()));
    connect(Estimation_Tab4_TransposePB,  SIGNAL(clicked()),
            this,                         SLOT(callback_TransposePB()));
    connect(Estimation_Tab4_TransposePB2, SIGNAL(clicked()),
            this,                         SLOT(callback_TransposePB()));
    connect(Estimation_Tab4_MinSP,        SIGNAL(splitterMoved(int,int)),
            this,                         SLOT(callback_MinSplitterMoved(int,int)));
    connect(Estimation_Tab4_MaxSP,        SIGNAL(splitterMoved(int,int)),
            this,                         SLOT(callback_MaxSplitterMoved(int,int)));
    connect(Estimation_Tab4_PredationSB,  SIGNAL(valueChanged(int)),
            this,                         SLOT(callback_PctRangeSB(int)));
    connect(Estimation_Tab4_PredationSB2, SIGNAL(valueChanged(int)),
            this,                         SLOT(callback_PctRangeSB(int)));
    connect(Estimation_Tab4_MinMaxCMB,    SIGNAL(currentTextChanged(QString)),
            this,                         SLOT(callback_MinMaxCMB(QString)));
    connect(Estimation_Tab4_MinMaxCMB2,   SIGNAL(currentTextChanged(QString)),
            this,                         SLOT(callback_MinMaxCMB(QString)));

    Estimation_Tab4_PrevPB->setText("\u25C1--");
    Estimation_Tab4_NextPB->setText("--\u25B7");

    m_TableViews1d.clear();
    m_TableViews1d.push_back(Estimation_Tab4_ExponentTV);
    m_TableViews1d.push_back(Estimation_Tab4_ExponentMinTV);
    m_TableViews1d.push_back(Estimation_Tab4_ExponentMaxTV);

    m_TableViews2d.clear();
    m_TableViews2d.push_back(Estimation_Tab4_PredationTV);
    m_TableViews2d.push_back(Estimation_Tab4_PredationMinTV);
    m_TableViews2d.push_back(Estimation_Tab4_PredationMaxTV);
    m_TableViews2d.push_back(Estimation_Tab4_HandlingTV);
    m_TableViews2d.push_back(Estimation_Tab4_HandlingMinTV);
    m_TableViews2d.push_back(Estimation_Tab4_HandlingMaxTV);

    m_TableViewsTypeI.clear();
    m_TableViewsTypeI.push_back(Estimation_Tab4_PredationTV);
    m_TableViewsTypeI.push_back(Estimation_Tab4_PredationMinTV);
    m_TableViewsTypeI.push_back(Estimation_Tab4_PredationMaxTV);

    m_TableViewsTypeII.clear();
    m_TableViewsTypeII.push_back(Estimation_Tab4_HandlingTV);
    m_TableViewsTypeII.push_back(Estimation_Tab4_HandlingMinTV);
    m_TableViewsTypeII.push_back(Estimation_Tab4_HandlingMaxTV);

    m_TableViewsTypeIII.clear();
    m_TableViewsTypeIII.push_back(Estimation_Tab4_ExponentTV);
    m_TableViewsTypeIII.push_back(Estimation_Tab4_ExponentMinTV);
    m_TableViewsTypeIII.push_back(Estimation_Tab4_ExponentMaxTV);

    m_TableNamesTypeI.clear();
    m_TableNamesTypeI.push_back(nmfConstantsMSSPM::TablePredationRho);
    m_TableNamesTypeI.push_back(nmfConstantsMSSPM::TablePredationRhoMin);
    m_TableNamesTypeI.push_back(nmfConstantsMSSPM::TablePredationRhoMax);

    m_TableNamesTypeII.clear();
    m_TableNamesTypeII.push_back(nmfConstantsMSSPM::TablePredationHandling);
    m_TableNamesTypeII.push_back(nmfConstantsMSSPM::TablePredationHandlingMin);
    m_TableNamesTypeII.push_back(nmfConstantsMSSPM::TablePredationHandlingMax);

    m_TableNamesTypeIII.clear();
    m_TableNamesTypeIII.push_back(nmfConstantsMSSPM::TablePredationExponent);
    m_TableNamesTypeIII.push_back(nmfConstantsMSSPM::TablePredationExponentMin);
    m_TableNamesTypeIII.push_back(nmfConstantsMSSPM::TablePredationExponentMax);

    // Create models and attach to view
    int NumSpecies = getNumSpecies();
    QStandardItemModel* smodel;
    for (unsigned i=0; i<m_TableViews2d.size(); ++i) {
        smodel = new QStandardItemModel(NumSpecies, NumSpecies);
        m_smodels2d.push_back(smodel);
        m_TableViews2d[i]->setModel(smodel);
    }

} // end constructor


nmfEstimation_Tab4::~nmfEstimation_Tab4()
{

}

void
nmfEstimation_Tab4::clearWidgets()
{
        nmfUtilsQt::clearTableView(
            {Estimation_Tab4_PredationTV,
             Estimation_Tab4_PredationMinTV,
             Estimation_Tab4_PredationMaxTV,
             Estimation_Tab4_HandlingTV,
             Estimation_Tab4_HandlingMinTV,
             Estimation_Tab4_HandlingMaxTV,
             Estimation_Tab4_ExponentTV,
             Estimation_Tab4_ExponentMinTV,
             Estimation_Tab4_ExponentMaxTV});
}

void
nmfEstimation_Tab4::callback_MinSplitterMoved(int pos, int index)
{
   Estimation_Tab4_MaxSP->setSizes(Estimation_Tab4_MinSP->sizes());
}

void
nmfEstimation_Tab4::callback_MaxSplitterMoved(int pos, int index)
{
   Estimation_Tab4_MinSP->setSizes(Estimation_Tab4_MaxSP->sizes());
}

void
nmfEstimation_Tab4::readSettings()
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
    if (loadWidgets()) {
        QMessageBox::information(Estimation_Tabs, "Predation Load",
                                 "\nPredation table(s) successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab4::callback_ImportPB()
{
    QString tag="";
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<std::string> allTableNames = getAllTableNames();
    int numTables = allTableNames.size();

    // Load default CSV files
    std::string msg = "\nLoad default Predation .csv files?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Predation CSV Files"),
                                                              tr(msg.c_str()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        loadCSVFiles(allTableNames);
    } else {
        // if no, raise browser and have user select "predationrho*.csv" file.
        QString filename = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr("Select predationrho*.csv file"), inputDataPath,
                    QObject::tr("Data Files (predationrho*.csv)"));
        QFileInfo fi(filename);
        if (nmfUtilsQt::extractTag(fi.baseName(),tag)) {
            for (int i=0; i<numTables; ++i) {
                allTableNames[i] += "_"+tag.toStdString();
            }
            loadCSVFiles(allTableNames);
        } else if (! filename.isEmpty()) {
            QMessageBox::information(Estimation_Tabs, "Predation CSV Import",
                                     "\nPlease make sure to select the filename that begins with: predationrho\n",
                                     QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab4::loadCSVFiles(std::vector<std::string>& allTableNames)
{
    bool loadOK;
    int tableNum=0;
    QString errorMsg;
    QString tableName;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<QTableView*> allTableViews = getAllTableViews();
    std::pair<int,int> nonZeroCell      = std::make_pair(0,0);
    std::pair<int,int> nonZeroCellSaved = std::make_pair(0,0);

    if (allTableViews.size() == allTableNames.size()) {
        QStandardItemModel* smodel  = qobject_cast<QStandardItemModel*>(allTableViews[0]->model());
        QStandardItemModel* smodel1 = qobject_cast<QStandardItemModel*>(allTableViews[1]->model());
        QStandardItemModel* smodel2 = qobject_cast<QStandardItemModel*>(allTableViews[2]->model());
        for (QTableView* tv : allTableViews) {
            tableName = QDir(inputDataPath).filePath(QString::fromStdString(allTableNames[tableNum]+".csv"));
            loadOK = nmfUtilsQt::loadTimeSeries(
                        Estimation_Tabs, tv, inputDataPath, tableName,
                        nmfConstantsMSSPM::FirstLineNotReadOnly,
                        nmfConstantsMSSPM::ScientificNotation,
                        nonZeroCell,errorMsg);
            if (! loadOK) {
                m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
            }
            ++tableNum;
            if (tableNum == 1) {
                nonZeroCellSaved = nonZeroCell;
            }
        }
        resetSpinBox(nonZeroCellSaved,smodel,smodel1,smodel2);
    } else {
        m_Logger->logMsg(nmfConstants::Error,"callback_ImportPB: number of visible tables don't match number of active table names");
    }
}

void
nmfEstimation_Tab4::resetSpinBox(const std::pair<int,int>& nonZeroCell,
                                 const QStandardItemModel* smodel,
                                 const QStandardItemModel* smodel1,
                                 const QStandardItemModel* smodel2)
{
    int nonZeroRow = nonZeroCell.first;
    int nonZeroCol = nonZeroCell.second;

    QModelIndex initValueIndex = smodel->index(nonZeroRow,nonZeroCol);
    QModelIndex minValueIndex  = smodel1->index(nonZeroRow,nonZeroCol);
    QModelIndex maxValueIndex  = smodel2->index(nonZeroRow,nonZeroCol);
    double initValue = initValueIndex.data().toDouble();
    double minValue  = minValueIndex.data().toDouble();
    double maxValue  = maxValueIndex.data().toDouble();
    int minPctValue  = int(nmfUtils::round(100.0*(initValue-minValue)/initValue,0));
    int maxPctValue  = int(nmfUtils::round(100.0*(maxValue-initValue)/initValue,0));

    if (minPctValue == maxPctValue) {
        Estimation_Tab4_PredationSB->setValue(minPctValue);
    } else {
        Estimation_Tab4_PredationSB->blockSignals(true);
        Estimation_Tab4_PredationSB->setValue(0);
        Estimation_Tab4_PredationSB->blockSignals(false);
    }
}


void
nmfEstimation_Tab4::callback_SavePB()
{
    bool systemFound = false;
    bool isTypeIIITable = true;
    bool isNotTypeIIITable = false;
    std::string cmd;
    std::string errorMsg;
    std::string value;
    std::string Algorithm,Minimizer,ObjectiveCriterion;
    std::string Scaling,CompetitionForm;
    std::vector<std::string> SpeNames;
    QString msg;

    readSettings();

    // Find Algorithm type
    systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                Estimation_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab4::callback_SavePB: Couldn't find any System algorithms");
        return;
    }
    if (m_smodels2d.size() == 0) {
        return;
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Get Species names
    for (int j=0; j<m_smodels2d[0]->columnCount(); ++ j) {
        SpeNames.push_back(m_smodels2d[0]->horizontalHeaderItem(j)->text().toStdString());
    }

    if (isTypeI() || isTypeII() || isTypeIII()) { // save rho
        saveTables(isNotTypeIIITable, SpeNames,m_TableViewsTypeI,  m_TableNamesTypeI);
    }

    if (isTypeII() || isTypeIII()) { // save h
        saveTables(isNotTypeIIITable, SpeNames,m_TableViewsTypeII, m_TableNamesTypeII);
    }

    if (isTypeIII()) { // save b
        saveTables(isTypeIIITable,    SpeNames,m_TableViewsTypeIII,m_TableNamesTypeIII);
    }

    QMessageBox::information(Estimation_Tabs, "Predation Updated",
                             "\nPredation tables have been successfully updated.\n",
                             QMessageBox::Ok);

    // Reload so that columns resize appropriately
    loadWidgets();

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
}



void
nmfEstimation_Tab4::saveTables(const bool& isTypeIII,
                               const std::vector<std::string>& SpeNames,
                               const std::vector<QTableView*>& tableViews,
                               const std::vector<std::string>& tableNames)
{
    std::string cmd;
    std::string errorMsg;
    std::string value;
    QModelIndex index;
    QStandardItemModel* smodel1 = qobject_cast<QStandardItemModel*>(tableViews[1]->model());
    QStandardItemModel* smodel2 = qobject_cast<QStandardItemModel*>(tableViews[2]->model());
    for (unsigned int k=0; k<tableViews.size(); ++k) {
        QStandardItemModel* smodel  = qobject_cast<QStandardItemModel*>(tableViews[k]->model());
        if (k == 0) {
            if (! nmfUtilsQt::allMaxCellsGreaterThanMinCells(smodel1,smodel2)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab4::callback_SavePB: At least one Max cell less than a Min cell: " + errorMsg);
                QMessageBox::critical(Estimation_Tabs, "Error",
                                      "\nError: There's at least one Max cell less than a Min cell. Please check tables.\n",
                                      QMessageBox::Ok);
                Estimation_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
        cmd = "DELETE FROM " + tableNames[k] + " WHERE ProjectName = '" + m_ProjectName +
              "' AND ModelName = '" + m_ModelName + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab4::callback_SavePB: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command.  Couldn't delete all records from " +
                                 QString::fromStdString(tableNames[k]) + " table.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }

        if (! isTypeIII) {
            cmd = "INSERT INTO " + tableNames[k] + " (ProjectName,ModelName,SpeciesA,SpeciesB,Value) VALUES ";
            smodel = qobject_cast<QStandardItemModel*>(tableViews[k]->model());
            for (int i=0; i<smodel->rowCount(); ++i) {
                for (int j=0; j<smodel->columnCount(); ++ j) {
                    index = smodel->index(i,j);
                    value = index.data().toString().toStdString();
                    cmd += "('" + m_ProjectName + "','" + m_ModelName + "','" + SpeNames[i] +
                            "','" + SpeNames[j] + "', " + value + "),";
                }
            }
        } else {
            cmd = "INSERT INTO " + tableNames[k] + " (ProjectName,ModelName,SpeName,Value) VALUES ";
            for (int i=0; i<smodel->rowCount(); ++i) {
                for (int j=0; j<smodel->columnCount(); ++ j) {
                    index = smodel->index(i,j);
                    value = index.data().toString().toStdString();
                    cmd += "('" + m_ProjectName + "','" + m_ModelName + "','" + SpeNames[i] +
                            "'," + value + "),";
                }
            }
        }
        cmd = cmd.substr(0,cmd.size()-1);
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab4::callback_SavePB: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                 "\nError in Save command.  Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            Estimation_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
    }
}


void
nmfEstimation_Tab4::callback_ExportPB()
{
    // Save time series data to a .csv file
    QString tableNameWithPath;
    std::vector<std::string> allTableNames = getAllTableNames();
    std::vector<QTableView*> allTableViews = getAllTableViews();
    int numTableNames = int(allTableNames.size());
    int numTableViews = int(allTableViews.size());
    if (numTableNames == numTableViews) {
        std::string msg = "\nOK to use default file names for Predation .csv files and overwrite any previous files?";
        QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Predation CSV Files"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                                  QMessageBox::Yes);
        if (reply == QMessageBox::Cancel) {
            return;
        } else if (reply == QMessageBox::Yes) {
            saveCSVFiles(allTableNames);
        } else {
            bool ok;
            QString tag = QInputDialog::getText(Estimation_Tabs, tr("Predation Files"),
                                                 tr("Enter Predation CSV filename version tag: "), QLineEdit::Normal,
                                                 "", &ok);
            if (ok && !tag.isEmpty()) {
                 for (int i=0; i<numTableNames; ++i) {
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
        std::string msg = "Error: numTableNames(" + std::to_string(numTableNames) +
                ") different size than numTableViews(" + std::to_string(numTableViews) + ")";
        m_Logger->logMsg(nmfConstants::Error,msg);
    }
}

std::vector<std::string>
nmfEstimation_Tab4::getAllTableNames()
{
    std::vector<std::string> allTableNames;

    for (std::vector<std::string> tables : {m_TableNames1d, m_TableNames2d}) {
        for (std::string tableName : tables) {
            allTableNames.push_back(tableName);
        }
    }

    return allTableNames;
}

std::vector<QTableView*>
nmfEstimation_Tab4::getAllTableViews()
{
    std::vector<QTableView*> allTableViews;

    allTableViews.push_back(Estimation_Tab4_PredationTV);
    allTableViews.push_back(Estimation_Tab4_PredationMinTV);
    allTableViews.push_back(Estimation_Tab4_PredationMaxTV);
    if (isTypeII() || isTypeIII()) {
        allTableViews.push_back(Estimation_Tab4_HandlingTV);
        allTableViews.push_back(Estimation_Tab4_HandlingMinTV);
        allTableViews.push_back(Estimation_Tab4_HandlingMaxTV);
    }
    if (isTypeIII()) {
        allTableViews.push_back(Estimation_Tab4_ExponentTV);
        allTableViews.push_back(Estimation_Tab4_ExponentMinTV);
        allTableViews.push_back(Estimation_Tab4_ExponentMaxTV);
    }

    return allTableViews;
}

void
nmfEstimation_Tab4::saveCSVFiles(
        std::vector<std::string>& allTableNames)
{
    bool okSave;
    bool atLeastOneError = false;
    int tableNum = 0;
    QString tableNameWithPath;
    QString savedFilenames;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<QTableView* > allTableViews = getAllTableViews();

    for (QTableView* tv : allTableViews) {
        tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(allTableNames[tableNum]));
        QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tv->model());
        okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
        savedFilenames += tableNameWithPath+"\n";
        if (!okSave) {
            atLeastOneError = true;
        }
        ++tableNum;
    }
    if (atLeastOneError) {
        QMessageBox::information(Estimation_Tabs, "Predation File Save Error",
                                 "\nAll Predation CSV files have not been saved. Check for valid filenames.\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "Predation Files Saved",
                                 "\nPredation CSV files have been successfully saved as:\n\n"+savedFilenames,
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab4::callback_PredationFormChanged(QString predationForm)
{
    QList<int> minSizeList;
    QList<int> maxSizeList;
    int numSections  = Estimation_Tab4_MinSP->sizes().size();
    int defaultWidth = Estimation_Tab4_MinSP->width()/3;

    m_PredationForm = predationForm.toStdString();

    for (int i=0; i<numSections; ++i) {
        minSizeList.push_back(defaultWidth);
        maxSizeList.push_back(defaultWidth);
    }
    Estimation_Tab4_MainSP->setSizes(minSizeList);
    Estimation_Tab4_MinSP->setSizes(minSizeList);
    Estimation_Tab4_MaxSP->setSizes(maxSizeList);

    Estimation_Tab4_PredationTV->setEnabled(false);
    Estimation_Tab4_PredationMinTV->setEnabled(false);
    Estimation_Tab4_PredationMaxTV->setEnabled(false);
    Estimation_Tab4_HandlingTV->setEnabled(false);
    Estimation_Tab4_HandlingMinTV->setEnabled(false);
    Estimation_Tab4_HandlingMaxTV->setEnabled(false);
    Estimation_Tab4_ExponentTV->setEnabled(false);
    Estimation_Tab4_ExponentMinTV->setEnabled(false);
    Estimation_Tab4_ExponentMaxTV->setEnabled(false);
    Estimation_Tab4_PredationLBL->setEnabled(false);
    Estimation_Tab4_PredationMinLBL->setEnabled(false);
    Estimation_Tab4_PredationMaxLBL->setEnabled(false);
    Estimation_Tab4_HandlingLBL->setEnabled(false);
    Estimation_Tab4_HandlingMinLBL->setEnabled(false);
    Estimation_Tab4_HandlingMaxLBL->setEnabled(false);
    Estimation_Tab4_ExponentLBL->setEnabled(false);
    Estimation_Tab4_ExponentMinLBL->setEnabled(false);
    Estimation_Tab4_ExponentMaxLBL->setEnabled(false);

    if (isTypeI()) {
        Estimation_Tab4_PredationTV->setEnabled(true);
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationLBL->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        for (int i=1; i<numSections; ++i) {
            minSizeList[i] = 0;
            maxSizeList[i] = 0;
        }
        Estimation_Tab4_MainSP->setSizes(minSizeList);
        Estimation_Tab4_MinSP->setSizes(minSizeList);
        Estimation_Tab4_MaxSP->setSizes(maxSizeList);
    } else if (isTypeII()) {
        Estimation_Tab4_PredationTV->setEnabled(true);
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationLBL->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        Estimation_Tab4_HandlingTV->setEnabled(true);
        Estimation_Tab4_HandlingMinTV->setEnabled(true);
        Estimation_Tab4_HandlingMaxTV->setEnabled(true);
        Estimation_Tab4_HandlingLBL->setEnabled(true);
        Estimation_Tab4_HandlingMinLBL->setEnabled(true);
        Estimation_Tab4_HandlingMaxLBL->setEnabled(true);
        minSizeList[numSections-1] = 0;
        maxSizeList[numSections-1] = 0;
        Estimation_Tab4_MainSP->setSizes(minSizeList);
        Estimation_Tab4_MinSP->setSizes(minSizeList);
        Estimation_Tab4_MaxSP->setSizes(maxSizeList);
    } else if (isTypeIII()) {
        Estimation_Tab4_PredationTV->setEnabled(true);
        Estimation_Tab4_PredationMinTV->setEnabled(true);
        Estimation_Tab4_PredationMaxTV->setEnabled(true);
        Estimation_Tab4_PredationLBL->setEnabled(true);
        Estimation_Tab4_PredationMinLBL->setEnabled(true);
        Estimation_Tab4_PredationMaxLBL->setEnabled(true);
        Estimation_Tab4_HandlingTV->setEnabled(true);
        Estimation_Tab4_HandlingMinTV->setEnabled(true);
        Estimation_Tab4_HandlingMaxTV->setEnabled(true);
        Estimation_Tab4_HandlingLBL->setEnabled(true);
        Estimation_Tab4_HandlingMinLBL->setEnabled(true);
        Estimation_Tab4_HandlingMaxLBL->setEnabled(true);
        Estimation_Tab4_ExponentTV->setEnabled(true);
        Estimation_Tab4_ExponentMinTV->setEnabled(true);
        Estimation_Tab4_ExponentMaxTV->setEnabled(true);
        Estimation_Tab4_ExponentLBL->setEnabled(true);
        Estimation_Tab4_ExponentMinLBL->setEnabled(true);
        Estimation_Tab4_ExponentMaxLBL->setEnabled(true);
    }

    // Load table names
    m_TableNames1d.clear();
    if (isTypeIII()) {
        m_TableNames1d.push_back(nmfConstantsMSSPM::TablePredationExponent);
        m_TableNames1d.push_back(nmfConstantsMSSPM::TablePredationExponentMin);
        m_TableNames1d.push_back(nmfConstantsMSSPM::TablePredationExponentMax);
    }
    m_TableNames2d.clear();
    m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationRho);
    m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationRhoMin);
    m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationRhoMax);
    if (isTypeII() || isTypeIII()) {
        m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationHandling);
        m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationHandlingMin);
        m_TableNames2d.push_back(nmfConstantsMSSPM::TablePredationHandlingMax);
    }
}

bool
nmfEstimation_Tab4::isNull()
{
    return (m_PredationForm == "Null");
}

bool
nmfEstimation_Tab4::isTypeI()
{
    return (m_PredationForm == "Type I");
}

bool
nmfEstimation_Tab4::isTypeII()
{
    return (m_PredationForm == "Type II");
}

bool
nmfEstimation_Tab4::isTypeIII()
{
    return (m_PredationForm == "Type III");
}

void
nmfEstimation_Tab4::getForms(std::string& predationForm,
                             std::string& competitionForm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get forms
    fields    = {"PredationForm","WithinGuildCompetitionForm"};
    queryStr  = "SELECT PredationForm,WithinGuildCompetitionForm FROM " + nmfConstantsMSSPM::TableModels + " WHERE ";
    queryStr += " ProjectName = '" + m_ProjectName + "' AND ModelName = '" + m_ModelName + "'";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["PredationForm"].size() > 0) {
        predationForm   = dataMap["PredationForm"][0];
        competitionForm = dataMap["WithinGuildCompetitionForm"][0];
    }
}

int
nmfEstimation_Tab4::getNumSpecies()
{
    std::vector<std::string> species;

    m_DatabasePtr->getAllSpecies(m_Logger,species);

    return species.size();
}

bool
nmfEstimation_Tab4::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab4::loadWidgets()");

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
    std::string dataMapStr;
    QStandardItem *item;
    QStringList SpeciesNames;
    QStringList GuildNames;
    QStringList exponentColName;
    QStandardItemModel* smodel;
    std::pair<int,int> nonZeroCell;

    readSettings();

    clearWidgets();

    getForms(predationForm,competitionForm);
    isAggProd = (competitionForm == "AGG-PROD");

    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }
    fields     = {"GuildName"};
    queryStr   = "SELECT GuildName FROM " + nmfConstantsMSSPM::TableGuilds;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds  = dataMap["GuildName"].size();
    for (int j=0; j<NumGuilds; ++j) {
        GuildNames << QString::fromStdString(dataMap["GuildName"][j]);
    }

    exponentColName << QString("b");
    int NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;


    if (isTypeI() || isTypeII() || isTypeIII()) {
        for (unsigned int k=0; k<m_TableNames2d.size(); ++k) {
            if (isAggProd) {
                smodel = new QStandardItemModel(NumGuilds, NumGuilds);
                m_smodels2d[k] = smodel;
                m_TableViews2d[k]->setModel(smodel);
            }
            fields    = {"ProjectName","ModelName","SpeciesA","SpeciesB","Value"};
            queryStr  = "SELECT ProjectName,ModelName,SpeciesA,SpeciesB,Value FROM " + m_TableNames2d[k] +
                        " WHERE ProjectName = '" + m_ProjectName + "' AND ModelName = '" + m_ModelName + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = dataMap["SpeciesA"].size();

            // Load Predation tables
            m = 0;
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                    if (m < NumRecords) {
                        dataMapStr = dataMap["Value"][m++];
                        if ((std::stod(dataMapStr) != 0) && (k<3)) {
                            nonZeroCell = std::make_pair(i,j);
                        }
                        item = new QStandardItem(QString::fromStdString(dataMapStr));
                    } else {
                        item = new QStandardItem("");
                    }
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
            m_TableViews2d[k]->resizeColumnsToContents();
        }
        resetSpinBox(nonZeroCell,m_smodels2d[0],m_smodels2d[1],m_smodels2d[2]);
    }
    if (isTypeIII()) {

        QStandardItemModel* smodel;
        for (unsigned int k=0; k<m_TableNamesTypeIII.size(); ++k) {
            if (isAggProd) {
                smodel = new QStandardItemModel(NumGuilds, 1);
                m_TableViewsTypeIII[k]->setModel(smodel);
            } else {
                smodel = new QStandardItemModel(NumSpeciesOrGuilds, 1);
                m_TableViewsTypeIII[k]->setModel(smodel);
            }
            fields    = {"ProjectName","ModelName","SpeName","Value"};
            queryStr  = "SELECT ProjectName,ModelName,SpeName,Value FROM " + m_TableNamesTypeIII[k] +
                        " WHERE ProjectName = '" + m_ProjectName + "' AND ModelName = '" + m_ModelName + "'";
            dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
            NumRecords = dataMap["SpeName"].size();

            // Load Predation tables
            m = 0;
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                if (m < NumRecords)
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                else
                    item = new QStandardItem("");
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, 0, item);
            }
            if (isAggProd) {
                smodel->setVerticalHeaderLabels(GuildNames);
            } else {
                smodel->setVerticalHeaderLabels(SpeciesNames);
            }
            smodel->setHorizontalHeaderLabels(exponentColName);
            m_TableViewsTypeIII[k]->resizeColumnsToContents();
        }
    }

    // Set enable-ness on widgets
    callback_PredationFormChanged(QString::fromStdString(predationForm));

    return true;
}


std::vector<std::vector<QTableView* > >
nmfEstimation_Tab4::getGroupsOfTableViews()
{
    std::vector<std::vector<QTableView* > > tableViews = {};

    if (isTypeI()) {
        tableViews = {m_TableViewsTypeI};
    } else if (isTypeII()) {
        tableViews = {m_TableViewsTypeI,m_TableViewsTypeII};
    } else if (isTypeIII()) {
        tableViews = {m_TableViewsTypeI,m_TableViewsTypeII,m_TableViewsTypeIII};
    }

    return tableViews;
}

void
nmfEstimation_Tab4::callback_TransposePB()
{
    std::vector<std::vector<QTableView* > > groupsOfTableViews = getGroupsOfTableViews();

    for (int group=0; group<int(groupsOfTableViews.size()); ++group) {
        for (QTableView* tv : groupsOfTableViews[group]) {
            nmfUtilsQt::transposeModel(tv);
            tv->resizeColumnsToContents();
        }
    }
}

void
nmfEstimation_Tab4::callback_PctRangeSB(int pctValue)
{
    double pct = pctValue/100.0;    
    QString rangeType = "";
    QTableView* tableView1 = nullptr;
    QTableView* tableView2 = nullptr;
    QTableView* tableView3 = nullptr;
    QTableView* tableView4 = nullptr;
    QTableView* tableView5 = nullptr;
    QTableView* tableView6 = nullptr;

    // The sender can be 1 of 2 spin boxes...this just keeps them in sync.
    QString senderName = qobject_cast<QSpinBox* >(QObject::sender())->objectName();
    if (senderName == "Estimation_Tab4_PredationSB") {
        rangeType = Estimation_Tab4_MinMaxCMB->currentText();
        Estimation_Tab4_PredationSB2->blockSignals(true);
        Estimation_Tab4_PredationSB2->setValue(pctValue);
        Estimation_Tab4_PredationSB2->blockSignals(false);
    } else {
        rangeType = Estimation_Tab4_MinMaxCMB2->currentText();
        Estimation_Tab4_PredationSB->blockSignals(true);
        Estimation_Tab4_PredationSB->setValue(pctValue);
        Estimation_Tab4_PredationSB->blockSignals(false);
    }

    // Set ranges in min max tables
    if (isTypeI()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeI[1],m_TableViewsTypeI[2],tableView1,tableView2);
        if (nmfUtilsQt::thereAreSelections(tableView1,tableView2)) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeI[0],tableView1,tableView2);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeI[0],tableView1,tableView2);
        }
    } else if (isTypeII()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeI[1], m_TableViewsTypeI[2], tableView1,tableView2);
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeII[1],m_TableViewsTypeII[2],tableView3,tableView4);
        bool thereAreSelectionsTypeI  = nmfUtilsQt::thereAreSelections(tableView1,tableView2);
        bool thereAreSelectionsTypeII = nmfUtilsQt::thereAreSelections(tableView3,tableView4);

        if (thereAreSelectionsTypeI || thereAreSelectionsTypeII) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeI[0],tableView1,tableView2);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeI[0],tableView1,tableView2);
        }
        if (thereAreSelectionsTypeI || thereAreSelectionsTypeII) {
            nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeII[0],tableView3,tableView4);
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeII[0],tableView3,tableView4);
        }
    } else if (isTypeIII()) {
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeI[1],  m_TableViewsTypeI[2],  tableView1,tableView2);
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeII[1], m_TableViewsTypeII[2], tableView3,tableView4);
        assignTableViewsBasedOnRangeType(rangeType,m_TableViewsTypeIII[1],m_TableViewsTypeIII[2],tableView5,tableView6);
        bool thereAreSelectionsTypeI   = nmfUtilsQt::thereAreSelections(tableView1,tableView2);
        bool thereAreSelectionsTypeII  = nmfUtilsQt::thereAreSelections(tableView3,tableView4);
        bool thereAreSelectionsTypeIII = nmfUtilsQt::thereAreSelections(tableView5,tableView6);

        if (thereAreSelectionsTypeI || thereAreSelectionsTypeII || thereAreSelectionsTypeIII) {
            if (thereAreSelectionsTypeI) {
                nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeI[0],  tableView1,tableView2);
            }
            if (thereAreSelectionsTypeII) {
                nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeII[0], tableView3,tableView4);
            }
            if (thereAreSelectionsTypeIII) {
                nmfUtilsQt::setMinMaxOnSelections(pct,m_TableViewsTypeIII[0],tableView5,tableView6);
            }
        } else {
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeI[0],  tableView1,tableView2);
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeII[0], tableView3,tableView4);
            nmfUtilsQt::setMinMax(pct,m_TableViewsTypeIII[0],tableView5,tableView6);
        }
    }
}


void
nmfEstimation_Tab4::assignTableViewsBasedOnRangeType(const QString& rangeType,
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

void
nmfEstimation_Tab4::callback_MinMaxCMB(QString rangeType)
{
    QString senderName = qobject_cast<QComboBox* >(QObject::sender())->objectName();
    if (senderName == "Estimation_Tab4_MinMaxCMB") {
        Estimation_Tab4_MinMaxCMB2->blockSignals(true);
        Estimation_Tab4_MinMaxCMB2->setCurrentText(rangeType);
        Estimation_Tab4_MinMaxCMB2->blockSignals(false);
    } else if (senderName == "Estimation_Tab3_MinMaxCMB2") {
        Estimation_Tab4_MinMaxCMB->blockSignals(true);
        Estimation_Tab4_MinMaxCMB->setCurrentText(rangeType);
        Estimation_Tab4_MinMaxCMB->blockSignals(false);
    }
}
