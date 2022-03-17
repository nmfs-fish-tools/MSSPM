
#include "nmfEstimationTab06.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfEstimation_Tab6::nmfEstimation_Tab6(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_ProjectName  = "";
    m_ModelName    = "";
    m_ProjectDir   = projectDir;
    m_ParamNames.clear();
    m_smodelC  = nullptr;
    m_smodelSP = nullptr;
    m_smodelIR = nullptr;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    readSettings();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Covariate Data"));

    Estimation_Tab6_AddPB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_AddPB");
    Estimation_Tab6_ClearPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ClearPB");
    Estimation_Tab6_ImportPB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ImportPB");
    Estimation_Tab6_ExportPB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ExportPB");
    Estimation_Tab6_RenamePB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_RenamePB");
    Estimation_Tab6_DeletePB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_DeletePB");
    Estimation_Tab6_SavePB             = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_SavePB");
    Estimation_Tab6_LoadPB             = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_LoadPB");
    Estimation_Tab6_NextPB             = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_NextPB");
    Estimation_Tab6_PrevPB             = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_PrevPB");
    Estimation_Tab6_SetEstOffPB        = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_SetEstOffPB");
    Estimation_Tab6_ClearRangesPB      = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ClearRangesPB");
    Estimation_Tab6_CovariateTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_CovariateTV");
    Estimation_Tab6_SpeciesParameterTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_SpeciesParameterTV");
    Estimation_Tab6_InitialValuesTV    = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_InitialValuesTV");
    Estimation_Tab6_SpeciesRangeCMB    = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_SpeciesRangeCMB");
    Estimation_Tab6_SpeciesRangeSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_SpeciesRangeSB");
    Estimation_Tab6_MinMaxCMB          = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinMaxCMB");
    Estimation_Tab6_AlgTypeCMB         = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_AlgTypeCMB");
    Estimation_Tab6_ViewNormalizedPB   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ViewNormalizedPB");

    Estimation_Tab6_PrevPB->setText("\u25C1--");
    Estimation_Tab6_NextPB->setText("--\u25B7");
    Estimation_Tab6_SpeciesRangeSB->setEnabled(false);
    Estimation_Tab6_MinMaxCMB->setEnabled(false);

    // Create connections
    connect(Estimation_Tab6_AddPB,    SIGNAL(clicked()),
            this,                     SLOT(callback_AddPB()));
    connect(Estimation_Tab6_ClearPB,  SIGNAL(clicked()),
            this,                     SLOT(callback_ClearPB()));
    connect(Estimation_Tab6_RenamePB, SIGNAL(clicked()),
            this,                     SLOT(callback_RenamePB()));
    connect(Estimation_Tab6_DeletePB, SIGNAL(clicked()),
            this,                     SLOT(callback_DeletePB()));
    connect(Estimation_Tab6_SavePB,   SIGNAL(clicked()),
            this,                     SLOT(callback_SavePB()));
    connect(Estimation_Tab6_LoadPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_LoadPB()));
    connect(Estimation_Tab6_PrevPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_PrevPB()));
    connect(Estimation_Tab6_NextPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_NextPB()));
    connect(Estimation_Tab6_ImportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ImportPB()));
    connect(Estimation_Tab6_ExportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ExportPB()));
    connect(Estimation_Tab6_SetEstOffPB,     SIGNAL(clicked()),
            this,                            SLOT(callback_SetEstOffPB()));
    connect(Estimation_Tab6_SpeciesRangeCMB, SIGNAL(activated(QString)),
            this,                            SLOT(callback_SpeciesRangeCMB(QString)));
    connect(Estimation_Tab6_ClearRangesPB,   SIGNAL(clicked()),
            this,                            SLOT(callback_ClearInitialValuesAndRangePB()));
    connect(Estimation_Tab6_SpeciesRangeSB,  SIGNAL(valueChanged(int)),
            this,                            SLOT(callback_SpeciesRangeSB(int)));
    connect(Estimation_Tab6_ViewNormalizedPB,SIGNAL(toggled(bool)),
            this,                            SLOT(callback_ViewNormalizedPB(bool)));


    initializeCovariateTable();
    initializeSpeciesParameterTable();
    initializeInitialValuesAndRangesTable();

} // end constructor


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{

}

void
nmfEstimation_Tab6::initializeCovariateTable()
{
    m_smodelC = new QStandardItemModel(0,0);
    Estimation_Tab6_CovariateTV->setModel(m_smodelC);
}

void
nmfEstimation_Tab6::initializeSpeciesParameterTable()
{
    std::vector<std::string> SpeciesNames;
    int RunLength;
    int StartYear;
    QStringList verticalHeader;
    QStringList horizontalHeader;
    int NumSpecies;
    std::string paramName;

    QStandardItemModel* smodelC = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());

    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear)) {
        return;
    }

    if (! m_DatabasePtr->getSpecies(m_Logger,SpeciesNames)) {
        return;
    }

    // Set up header data
    NumSpecies = SpeciesNames.size();
    QList<QStandardItem* > items;
    for (int i=0; i<NumSpecies; ++i) {
        verticalHeader << QString::fromStdString(SpeciesNames[i]);
    }
    int numParameters = (int)m_ParamNames.size();
    for (int i=0; i<numParameters; ++i) {
        horizontalHeader << QString::fromStdString(m_ParamNames[i]);
    }

    m_smodelSP = new QStandardItemModel(NumSpecies,numParameters);
    m_smodelSP->setVerticalHeaderLabels(verticalHeader);
    m_smodelSP->setHorizontalHeaderLabels(horizontalHeader);
    Estimation_Tab6_SpeciesParameterTV->setModel(m_smodelSP);

    // Set up item comboboxes
    int numCovariates = smodelC->columnCount();
    for (int row=0; row<m_smodelSP->rowCount(); ++row) {
        for (int col=0; col<m_smodelSP->columnCount(); ++col) {
            QStandardItemModel* model = new QStandardItemModel();
            QStandardItem* item = new QStandardItem("");
            model->setItem(0,item);
            for (int i=1; i<=numCovariates; ++i) {
                QStandardItem* item = new QStandardItem();
                item->setText(smodelC->horizontalHeaderItem(i-1)->text());
// Uncomment these if want to have more than one covariate per species-parameter combination
//              item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
//              item->setData(Qt::Unchecked,Qt::CheckStateRole);
                model->setItem(i,item);
            }
            QComboBox* cbox = new QComboBox();
            cbox->setModel(model);
            QModelIndex index = m_smodelSP->index(row,col);
            Estimation_Tab6_SpeciesParameterTV->setIndexWidget(index,cbox);
        }
    }
    Estimation_Tab6_SpeciesParameterTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab6::initializeInitialValuesAndRangesTable()
{
    std::vector<std::string> SpeciesNames;
    int RunLength;
    int StartYear;
    QStringList verticalHeader;
    QStringList horizontalHeader;
    int NumSpecies;
    QStandardItem* item;
    QComboBox* cbox;
    QModelIndex index;

    loadCovariateAssignmentTable();

    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear)) {
        return;
    }

    if (! m_DatabasePtr->getSpecies(m_Logger,SpeciesNames)) {
        return;
    }

    // Set up header data
    NumSpecies = SpeciesNames.size();
    QList<QStandardItem* > items;
    for (int i=0; i<NumSpecies; ++i) {
        verticalHeader << QString::fromStdString(SpeciesNames[i]);
    }
    int numParameters = (int)m_ParamNames.size();
    int numColumns = numParameters * 3; // Because each column also has a min and max column
    for (int i=0; i<numParameters; ++i) {
        horizontalHeader << QString::fromStdString(m_ParamNames[i]);
        horizontalHeader << QString::fromStdString(m_ParamNames[i]+"Min");
        horizontalHeader << QString::fromStdString(m_ParamNames[i]+"Max");
    }
    m_smodelIR = new QStandardItemModel(NumSpecies,numColumns);

    // Set up item comboboxes
    for (int row=0; row<m_smodelIR->rowCount(); ++row) {
        for (int col=0; col<numColumns; ++col) {
            item = new QStandardItem("");
            item->setTextAlignment(Qt::AlignCenter);

            // Make white background and enable if item selected in Assignment table
            index = m_smodelSP->index(row,col/3);
            cbox  = qobject_cast<QComboBox*>(Estimation_Tab6_SpeciesParameterTV->indexWidget(index));
            if (cbox->currentText().isEmpty()) {
                nmfUtilsQt::setItemEditable(nmfConstantsMSSPM::NotEditable,
                                            nmfConstantsMSSPM::GrayedIfNotEditable,
                                            item);
                item->setText("");
            } else {
                item->setText("0");
            }

            m_smodelIR->setItem(row,col,item);
        }
    }
    m_smodelIR->setVerticalHeaderLabels(verticalHeader);
    m_smodelIR->setHorizontalHeaderLabels(horizontalHeader);
    Estimation_Tab6_InitialValuesTV->setModel(m_smodelIR);
    Estimation_Tab6_InitialValuesTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab6::readSettings()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::readSettings");

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

//    settings->beginGroup("Preferences");
//    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
//    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab6::addCovariateColumn(QString covariateName)
{
    int numRows;
    int RunLength;
    int StartYear;
    QStringList verticalHeader;

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
    numRows = RunLength + 1;

    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());

    smodel->setRowCount(numRows);
    QList<QStandardItem* > items;
    for (int row=0; row<numRows; ++row) {
        items.append(new QStandardItem(QString("")));
        verticalHeader << QString::number(StartYear+row);
    }
    smodel->appendColumn(items);

    QStandardItem* headerItem = new QStandardItem(covariateName);
    smodel->setVerticalHeaderLabels(verticalHeader);
    smodel->setHorizontalHeaderItem(smodel->columnCount()-1,headerItem);
    Estimation_Tab6_CovariateTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab6::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::loadWidgets()");
    loadCovariateAlgorithmType();
    Estimation_Tab6_ViewNormalizedPB->setChecked(false);
    loadCovariateTable();
    loadCovariateAssignmentTable();
    loadCovariateInitialValuesAndRangesTable();
}

void
nmfEstimation_Tab6::loadCovariateAlgorithmType()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get data from database table
    fields     = {"ProjectName","ModelName","CovariateAlgorithmType"};
    queryStr   = "SELECT ProjectName,ModelName,CovariateAlgorithmType FROM " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["CovariateAlgorithmType"].size();
    if (NumRecords == 1) {
        setCovariateAlgorithmType(dataMap["CovariateAlgorithmType"][0]);
    } else {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::loadCovariateAlgorithmType: Incorrect number of Covariate Algorithm Type records found in models table");
    }
}


void
nmfEstimation_Tab6::loadCovariateTable()
{
    int m;
    int numCols;
    int numRows;
    int NumRecords;
    int RunLength;
    int StartYear;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStandardItemModel* smodel;
    QStringList horizontalHeader;
    QStringList verticalHeader;
    std::string tableName  = nmfConstantsMSSPM::TableCovariate;
    std::string valueField = (isViewNormalizeToggled()) ? "ValueScaled" : "Value";

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
    numRows = RunLength+1;

    // Get data from database table
    fields     = {"ProjectName","ModelName","CovariateName","Year","Value","ValueScaled"};
    queryStr   = "SELECT ProjectName,ModelName,CovariateName,Year,Value,ValueScaled FROM " +
                  tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' ORDER BY CovariateName,Year ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["CovariateName"].size();

    if (NumRecords%numRows == 0) {
        smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());
        smodel->setRowCount(numRows);
        numCols = NumRecords/numRows;
        smodel->setColumnCount(numCols);
        m = 0;
        for (int col=0; col<numCols; ++col) {
            horizontalHeader << QString::fromStdString(dataMap["CovariateName"][m]);
            for (int row=0; row<numRows; ++row) {
                item = new QStandardItem(QString::fromStdString(dataMap[valueField][m++]));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(row,col,item);
                if (col == 0) {
                    verticalHeader << QString::fromStdString(dataMap["Year"][row]);
                }
            }
        }
        // Setup headers
        smodel->setHorizontalHeaderLabels(horizontalHeader);
        smodel->setVerticalHeaderLabels(verticalHeader);

        Estimation_Tab6_CovariateTV->setModel(smodel);
        Estimation_Tab6_CovariateTV->resizeColumnsToContents();
        Estimation_Tab6_CovariateTV->clearSelection();
    }
}

void
nmfEstimation_Tab6::loadCovariateAssignmentTable()
{
    int m=0;
    int NumRecords;
    int numRows = 0;
    int numCols = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName  = nmfConstantsMSSPM::TableCovariateAssignment;
    QComboBox* cbox;
    QModelIndex index;
    QStandardItemModel* smodelSP = qobject_cast<QStandardItemModel*>(Estimation_Tab6_SpeciesParameterTV->model());

    if (m_smodelSP == nullptr) {
        return;
    } else {
        numRows = m_smodelSP->rowCount();
        numCols = m_smodelSP->columnCount();
    }

    // Get data from database table
    fields     = {"ProjectName","ModelName","SpeName","ParameterName","CovariateName"};
    queryStr   = "SELECT ProjectName,ModelName,SpeName,ParameterName,CovariateName FROM " +
                  tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName +
                 "' ORDER BY SpeName,ParameterName ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["CovariateName"].size();

    if (NumRecords == numRows*numCols) {
        for (int row=0; row<numRows; ++row) {
            for (int col=0; col<numCols; ++col) {
                index = smodelSP->index(row,col);
                cbox  = qobject_cast<QComboBox*>(Estimation_Tab6_SpeciesParameterTV->indexWidget(index));
                cbox->setCurrentText(QString::fromStdString(dataMap["CovariateName"][m++]));
                Estimation_Tab6_SpeciesParameterTV->setIndexWidget(index,cbox);
            }
        }
        Estimation_Tab6_SpeciesParameterTV->resizeColumnsToContents();
    }
}

void
nmfEstimation_Tab6::loadInitialValuesAndRangesForEditableCells()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName = nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges;
    std::string coeffValue;
    std::string newValue;
    QStandardItem* item;
    QStandardItemModel* smodelIR = qobject_cast<QStandardItemModel*>(Estimation_Tab6_InitialValuesTV->model());

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
        return;
    }

    std::string SpeName;
    std::string CoeffName;
    std::string CoeffMinName;
    std::string CoeffMaxName;
    std::string CoeffValue;
    std::string CoeffMinValue;
    std::string CoeffMaxValue;
    QString SpeNameIR;
    for (int i=0; i<NumRecords; ++i) {
        if (! dataMap["CoeffValue"][i].empty()) {
            SpeName       = dataMap["SpeName"][i];
            CoeffName     = dataMap["CoeffName"][i];
            CoeffMinName  = dataMap["CoeffMinName"][i];
            CoeffMaxName  = dataMap["CoeffMaxName"][i];
            CoeffValue    = dataMap["CoeffValue"][i];
            CoeffMinValue = dataMap["CoeffMinValue"][i];
            CoeffMaxValue = dataMap["CoeffMaxValue"][i];
            // Find corresponding cells in the smodelIR array for the preceding variables
            for (int row=0; row<smodelIR->rowCount(); ++row) {
                if (smodelIR->verticalHeaderItem(row)->text().toStdString() == SpeName) {
                    for (int col=0; col<smodelIR->columnCount(); ++col) {
                        item = smodelIR->item(row,col);
                        if ((smodelIR->horizontalHeaderItem(col)->text().toStdString()        == CoeffName)    &&
                             item->isEditable()) {
                             item->setText(QString::fromStdString(CoeffValue));
                        } else if ((smodelIR->horizontalHeaderItem(col)->text().toStdString() == CoeffMinName) &&
                                   item->isEditable()) {
                            item->setText(QString::fromStdString(CoeffMinValue));
                        } else if ((smodelIR->horizontalHeaderItem(col)->text().toStdString() == CoeffMaxName) &&
                                   item->isEditable()) {
                            item->setText(QString::fromStdString(CoeffMaxValue));
                        }
                    }
                }
            }
        }
    }
}

void
nmfEstimation_Tab6::loadCovariateInitialValuesAndRangesTable()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName = nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges;
    std::string coeffValue;
    std::string newValue;
    std::vector<std::string> SpeciesNames;
    QStandardItem* item;
    QStandardItemModel* smodelIR = qobject_cast<QStandardItemModel*>(Estimation_Tab6_InitialValuesTV->model());

    if (! m_DatabasePtr->getSpecies(m_Logger,SpeciesNames)) {
        return;
    }
    int NumSpecies = SpeciesNames.size();

    // Get data from database table
    fields     = {"ProjectName","ModelName","SpeName",
                  "CoeffName","CoeffMinName","CoeffMaxName",
                  "CoeffValue","CoeffMinValue","CoeffMaxValue"};
    queryStr   = "SELECT ProjectName,ModelName,SpeName,CoeffName,CoeffMinName,CoeffMaxName,CoeffValue,CoeffMinValue,CoeffMaxValue FROM " +
                  tableName +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName   +
                 "' ORDER BY SpeName ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    int NumRecords = dataMap["CoeffName"].size();
    if (NumRecords == 0) {
        return;
    }
    int NumParameters = NumRecords/NumSpecies;

    int m = -1;
    int modelCol = 0;
    for (int species=0; species<NumSpecies; ++species) {
        modelCol = -3;
        for (int param=0; param<NumParameters; ++param) {
            ++m;
            modelCol += 3;
            coeffValue = dataMap["CoeffValue"][m];
            if (! QString::fromStdString(coeffValue).trimmed().isEmpty()) {                
                item = new QStandardItem(QString::fromStdString(coeffValue));
                item->setTextAlignment(Qt::AlignCenter);
                smodelIR->setItem(species,modelCol+0,item);
                item = new QStandardItem(QString::fromStdString(dataMap["CoeffMinValue"][m]));
                item->setTextAlignment(Qt::AlignCenter);
                smodelIR->setItem(species,modelCol+1,item);
                item = new QStandardItem(QString::fromStdString(dataMap["CoeffMaxValue"][m]));
                item->setTextAlignment(Qt::AlignCenter);
                smodelIR->setItem(species,modelCol+2,item);
            } else {
                item = new QStandardItem("");
                nmfUtilsQt::setItemEditable(nmfConstantsMSSPM::NotEditable,nmfConstantsMSSPM::GrayedIfNotEditable,item);
                smodelIR->setItem(species,modelCol+0,item);
                item = new QStandardItem("");
                nmfUtilsQt::setItemEditable(nmfConstantsMSSPM::NotEditable,nmfConstantsMSSPM::GrayedIfNotEditable,item);
                smodelIR->setItem(species,modelCol+1,item);
                item = new QStandardItem("");
                nmfUtilsQt::setItemEditable(nmfConstantsMSSPM::NotEditable,nmfConstantsMSSPM::GrayedIfNotEditable,item);
                smodelIR->setItem(species,modelCol+2,item);
            } // end if...else...
        } // end for param
    } // end for species

    Estimation_Tab6_InitialValuesTV->setModel(smodelIR);
    Estimation_Tab6_InitialValuesTV->resizeColumnsToContents();
}

bool
nmfEstimation_Tab6::isViewNormalizeToggled()
{
    return Estimation_Tab6_ViewNormalizedPB->isChecked();
}

std::string
nmfEstimation_Tab6::getCovariateAlgorithmType()
{
    return Estimation_Tab6_AlgTypeCMB->currentText().toStdString();
}

void
nmfEstimation_Tab6::importTableData(const bool& firstLineReadOnly,
                                    const QString& type,
                                    const std::string& tableName,
                                    QTableView* tableView)
{
    std::string tableNameNew;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // Load default CSV files
    std::string msg = "\nLoad default " + type.toStdString() + " .csv file?";
    QMessageBox::StandardButton reply = QMessageBox::question(
                Estimation_Tabs, tr("Default "+type.toLatin1()+" CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        loadCSVFile(firstLineReadOnly,inputDataPath,QString::fromStdString(tableName),tableView);
    } else {
        // if no, raise browser and have user select the appropriate Observed Biomass file.
        QString filenameWithPath = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr("Select "+type.toLatin1()+" file"), inputDataPath,
                    QObject::tr("Data Files (Covariate*.csv covariate*.csv)"));
        QFileInfo fi(filenameWithPath);
        QString filenameNoPath = fi.fileName();
        QString absolutePath   = fi.absolutePath();
        if (absolutePath == inputDataPath) {
            loadCSVFile(firstLineReadOnly,inputDataPath,filenameNoPath,tableView);
        } else {
            loadCSVFile(firstLineReadOnly,absolutePath,filenameNoPath,tableView);
        }
    }
}

void
nmfEstimation_Tab6::loadCSVFile(const bool& firstLineReadOnly,
                                const QString& filePath,
                                const QString& tableName,
                                QTableView* tableView)
{
    bool loadOK = false;
    QString errorMsg;
    QString tableNameStr = tableName.contains(".csv") ? tableName : tableName+".csv";
    std::pair<int,int> nonZeroCell;

    tableNameStr = QDir(filePath).filePath(tableNameStr);

    if (Estimation_Tab6_CovariateTV->isVisible()) {
        loadOK = nmfUtilsQt::loadTimeSeries(
                    Estimation_Tabs, tableView, filePath,
                    tableNameStr, firstLineReadOnly,
                    nmfConstantsMSSPM::FixedNotation,
                    nmfConstantsMSSPM::AllowBlanks,
                    nonZeroCell,errorMsg);
        nmfUtilsQt::removeCommas(tableView);
    } else if (Estimation_Tab6_SpeciesParameterTV->isVisible()) {
        loadOK = nmfUtilsQt::loadCSVFileComboBoxes(
                    Estimation_Tabs, tableView, filePath,
                    tableNameStr, errorMsg);
    } else if (Estimation_Tab6_InitialValuesTV->isVisible()) {
        loadOK = nmfUtilsQt::loadCVSFileCovariate(
                    Estimation_Tabs, tableView, filePath,
                    tableNameStr, errorMsg);
    }

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        QMessageBox::warning(Estimation_Tabs,"Time Series Load Error","\n"+errorMsg+"\n", QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "Import Successful",
                                 "\nThe CSV data file has been successfully imported.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab6::saveTableValuesToCSVFile(const QString& type,
                                             const std::string& tableName,
                                             QStandardItemModel* smodel)
{
    QString msg;
    std::string tableNameNew;

    // Save time series data to a .csv file
    msg = "\nOK to use default file name for "+type+" .csv file and overwrite any previous file?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs,
                                                              tr("Default "+type.toLatin1()+" CSV File"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        saveCSVFile(type,smodel,tableName);
    } else {
        bool ok;
        QString tag = QInputDialog::getText(Estimation_Tabs, tr(type.toLatin1()+" Files"),
                                            tr("Enter "+type.toLatin1()+" CSV filename version tag (omit any '_'): "),
                                            QLineEdit::Normal, "", &ok);
        if (ok && !tag.isEmpty()) {
            tableNameNew = tableName+"_"+tag.toStdString();
            saveCSVFile(type,smodel,tableNameNew);
        } else if (tag.isEmpty()) {
            QMessageBox::warning(Estimation_Tabs, "Tag Error",
                                 "\nError: Please enter a valid (i.e., non-blank) tag.\n",
                                 QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab6::saveCSVFile(
        const QString& type,
        QStandardItemModel* smodel,
        const std::string& tableName)
{
    bool okSave = false;;
    QString tableNameWithPath;

    // Save time series data to a .csv file
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));

    if (Estimation_Tab6_CovariateTV->isVisible()) {
        okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
    } else if (Estimation_Tab6_SpeciesParameterTV->isVisible()) {
        okSave = nmfUtilsQt::saveCSVFileComboBoxes(Estimation_Tabs,Estimation_Tab6_SpeciesParameterTV,
                                                   smodel,inputDataPath,tableNameWithPath);
    } else if (Estimation_Tab6_InitialValuesTV->isVisible()) {
        okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);
    }

    if (okSave) {
        QMessageBox::information(Estimation_Tabs, type+" File Saved",
                                 "\n"+type+" CSV file has been successfully saved as:\n\n"+tableNameWithPath+"\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, type+" File Save Error",
                                 "\nError: Please enter a valid "+type+" filename (i.e., non-blank)\n",
                                 QMessageBox::Ok);
    }
}

bool
nmfEstimation_Tab6::saveCovariateAlgorithmType()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string errorMsg;
    std::string updateCmd;

    updateCmd = "UPDATE " + nmfConstantsMSSPM::TableModels +
                "  SET CovariateAlgorithmType = '" + getCovariateAlgorithmType() +
                "' WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '" + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(updateCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveCovariateAlgorithmType: Problem updating models table");
        return false;
    }
    return true;
}

bool
nmfEstimation_Tab6::saveCovariateTable()
{
    int numRows = m_smodelC->rowCount();
    int numCols = m_smodelC->columnCount();
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::string tableName = nmfConstantsMSSPM::TableCovariate;
    QString msg;
    QModelIndex index;
    QString value;
    QString valueScaled;
    double min = 0;
    double diff = 0;

    m_Logger->logMsg(nmfConstants::Warning,"Saving scaled covariate data is currently disabled");

    // Check covariate table for blanks
//    if (! nmfUtilsQt::checkTableForBlanks(Estimation_Tab6_CovariateTV)) {
//        msg = "No blanks allowed in covariate table.";
//        m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
//        QMessageBox::warning(Estimation_Tabs, "Save Error", "\n"+msg, QMessageBox::Ok);
//        Estimation_Tabs->setCursor(Qt::ArrowCursor);
//        return false;
//    }

    // Delete the current entry here
    deleteCmd = "DELETE FROM " + tableName +
            " WHERE ProjectName = '" + m_ProjectName +
            "' AND ModelName = '" + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in nmfEstimation_Tab6:saveCovariateTable: Couldn't delete all records from " +
                QString::fromStdString(tableName) + " table";
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveCovariateTable: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd  = "INSERT INTO " + tableName +
               " (ProjectName,ModelName,CovariateName,Year,Value,ValueScaled) VALUES ";
    for (int col=0; col<numCols; ++col) {
        calculateCovariateScaleFactor(col,min,diff);
        for (int row=0; row<numRows; ++row) {
            index = m_smodelC->index(row,col);
            value = index.data().toString();
            valueScaled.clear();
            if (! value.isEmpty()) {
                // valueScaled = (diff == 0.0) ? "0.0" : QString::number(2.0*(value.toDouble()-min)/diff - 1.0); // scales from -1 to 1
                   valueScaled = (diff == 0.0) ? "0.0" : QString::number((value.toDouble()-min)/diff); // scales from 0 to 1
            }
            saveCmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + m_smodelC->horizontalHeaderItem(col)->text().toStdString() +
                       "', " + m_smodelC->verticalHeaderItem(row)->text().toStdString() +
                       " ,\""  + value.toStdString() +
                       "\",\"" + valueScaled.toStdString() + "\"),";
        }
    }
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveCovariateTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in nmfEstimation_Tab6::saveCovariateTable command\n",
                             QMessageBox::Ok);
        return false;
    }

    loadCovariateTable();
    return true;
}

void
nmfEstimation_Tab6::setCovariateAlgorithmType(std::string& algType)
{
    Estimation_Tab6_AlgTypeCMB->setCurrentText(QString::fromStdString(algType));
}

void
nmfEstimation_Tab6::calculateCovariateScaleFactor(
        const int& col,
        double& min,
        double& diff)
{
    double max =  std::numeric_limits<double>::lowest();
    double value;
    QModelIndex index;

    min = -max;
    for (int row=0; row<m_smodelC->rowCount(); ++row) {
        index = m_smodelC->index(row,col);
        if (! index.data().toString().isEmpty()) {
            value = index.data().toDouble();
            max   = (value > max) ? value : max;
            min   = (value < min) ? value : min;
        }
    }
    diff = max - min;
}


bool
nmfEstimation_Tab6::saveCovariateAssignmentTable()
{
    int numRows = m_smodelSP->rowCount();
    int numCols = m_smodelSP->columnCount();
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::string tableName = nmfConstantsMSSPM::TableCovariateAssignment;
    QString msg;
    QModelIndex index;
    QString value;
    QComboBox* cbox;

    // Delete the current entry here
    deleteCmd = "DELETE FROM " + tableName +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '" + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in nmfEstimation_Tab6:saveCovariateAssignmentTable: Couldn't delete all records from " +
                QString::fromStdString(tableName) + " table";
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveCovariateAssignmentTable: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd  = "INSERT INTO " + tableName +
               " (ProjectName,ModelName,SpeName,ParameterName,CovariateName) VALUES ";
    for (int row=0; row<numRows; ++row) {
        for (int col=0; col<numCols; ++col) {
            index = m_smodelSP->index(row,col);
            cbox  = qobject_cast<QComboBox*>(Estimation_Tab6_SpeciesParameterTV->indexWidget(index));
            value = cbox->currentText();
//          index = cbox->model()->index(0,0);
//          value = cbox->model()->data(index).toString();
            saveCmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + m_smodelSP->verticalHeaderItem(row)->text().toStdString() +
                       "','" + m_smodelSP->horizontalHeaderItem(col)->text().toStdString() +
                       "','" + value.toStdString() + "'),";
        }
    }
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveCovariateAssignmentTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in nmfEstimation_Tab6::saveCovariateAssignmentTable command\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfEstimation_Tab6::okSaveInitialValuesAndRangesChecks()
{
    bool ok;
    int numRows = m_smodelIR->rowCount();
    int numCols = m_smodelIR->columnCount();
    double value,valueMin,valueMax;
    QStandardItem* item;

    for (int row=0; row<numRows; ++row) {
        for (int col=0; col<numCols; ++col) {
            item = m_smodelIR->item(row,col);
            if (item->isEditable()) {
                // 1. Check that no editable cells are blank
                if (item->text().isEmpty()) {
                    return false;
                }
                // 2. Check that all entered values are numeric
                value = item->text().toDouble(&ok);
                if (! ok) {
                    return false;
                }
            }
        }
    }

    // If we made it here we know that there are numbers in every editable cell
    for (int row=0; row<numRows; ++row) {
        for (int col=0; col<numCols; col+=3) {
            item = m_smodelIR->item(row,col);
            if (item->isEditable()) {
                value    = item->text().toDouble();
                valueMin = m_smodelIR->item(row,col+1)->text().toDouble();
                valueMax = m_smodelIR->item(row,col+2)->text().toDouble();
                // 3. Check that all min <= value and all max >= value
                if ((value < valueMin) || (value > valueMax)) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool
nmfEstimation_Tab6::saveInitialValuesAndRangesTable()
{
    int numRows = m_smodelIR->rowCount();
    int numCols = m_smodelIR->columnCount();
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::string tableName = nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges;
    QString msg;
    QString value,valueMin,valueMax;

    if (! okSaveInitialValuesAndRangesChecks()) {
        msg  = "\nFound missing, malformed, or out of range data. Please check that all editable cells ";
        msg += "are populated and that range values contain their respective initial values.\n";
        QMessageBox::critical(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Delete the current entry here
    deleteCmd = "DELETE FROM " + tableName +
                " WHERE ProjectName = '" + m_ProjectName +
                "' AND ModelName = '" + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in nmfEstimation_Tab6:saveInitialValuesAndRangesTable: Couldn't delete all records from " +
                QString::fromStdString(tableName) + " table";
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveInitialValuesAndRangesTable: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd  = "INSERT INTO " + tableName +
               " (ProjectName,ModelName,SpeName,CoeffName,CoeffMinName,CoeffMaxName,CoeffValue,CoeffMinValue,CoeffMaxValue) VALUES ";
    for (int row=0; row<numRows; ++row) {
        for (int col=0; col<numCols; col+=3) {
            value    = m_smodelIR->index(row,col+0).data().toString();
            valueMin = m_smodelIR->index(row,col+1).data().toString();
            valueMax = m_smodelIR->index(row,col+2).data().toString();
            saveCmd += "('"  + m_ProjectName +
                       "','" + m_ModelName   +
                       "','" + m_smodelIR->verticalHeaderItem(row)->text().trimmed().toStdString()     +
                       "','" + m_smodelIR->horizontalHeaderItem(col+0)->text().trimmed().toStdString() +
                       "','" + m_smodelIR->horizontalHeaderItem(col+1)->text().trimmed().toStdString() +
                       "','" + m_smodelIR->horizontalHeaderItem(col+2)->text().trimmed().toStdString() +
                       "','" + value.trimmed().toStdString()    +
                       "','" + valueMin.trimmed().toStdString() +
                       "','" + valueMax.trimmed().toStdString() + "'),";
        }
    }
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::saveInitialValuesAndRangesTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in nmfEstimation_Tab6::saveInitialValuesAndRangesTable command\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

void
nmfEstimation_Tab6::callback_AddPB()
{
    bool ok;
    QString covariateName = QInputDialog::getText(Estimation_Tabs, tr("Environmental Covariate Name"),
                                         tr("Enter Covariate Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !covariateName.isEmpty()) {
        covariateName.replace(",",";");
        addCovariateColumn(covariateName);
    }
}

void
nmfEstimation_Tab6::callback_ClearPB()
{
    int numRows = m_smodelSP->rowCount();
    int numCols = m_smodelSP->columnCount();
    QModelIndex index;
    QComboBox* cbox;

    for (int row=0; row<numRows; ++row) {
        for (int col=0; col<numCols; ++col) {
            index = m_smodelSP->index(row,col);
            cbox  = qobject_cast<QComboBox*>(Estimation_Tab6_SpeciesParameterTV->indexWidget(index));
            cbox->setCurrentIndex(0);
        }
    }
}

void
nmfEstimation_Tab6::callback_DeletePB()
{
    QItemSelectionModel *selectionModel = Estimation_Tab6_CovariateTV->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();
    if (indexes.size() <= 0) {
        return;
    }
    int selectedCol = indexes[0].column();
    QString currentCovariateName = m_smodelC->horizontalHeaderItem(selectedCol)->text();
    QString msg = "\nOK to delete column: " + currentCovariateName + " ?";
    if (QMessageBox::question(Estimation_Tab6_CovariateTV,
                              "Column Delete",
                              msg.toLatin1(),
                              QMessageBox::No|QMessageBox::Yes,
                              QMessageBox::Yes) == QMessageBox::Yes) {

         m_smodelC->removeColumn(selectedCol);
         saveCovariateTable();
    }
    initializeSpeciesParameterTable();
    initializeInitialValuesAndRangesTable();
    loadInitialValuesAndRangesForEditableCells();
    saveInitialValuesAndRangesTable();
}

void
nmfEstimation_Tab6::callback_RenamePB()
{
    bool ok;
    QItemSelectionModel *selectionModel = Estimation_Tab6_CovariateTV->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();
    if (indexes.size() <= 0) {
        return;
    }
    int selectedCol = indexes[0].column();

    QString currentCovariateName = m_smodelC->horizontalHeaderItem(selectedCol)->text();
    QString newCovariateName = QInputDialog::getText(Estimation_Tabs, tr("Rename Environmental Covariate"),
                                         tr("Enter New Covariate Name:"), QLineEdit::Normal,
                                         currentCovariateName, &ok);
    if (ok && !newCovariateName.isEmpty()) {
        QStandardItem* item = new QStandardItem();
        item->setText(newCovariateName);
        m_smodelC->setHorizontalHeaderItem(selectedCol,item);
        saveCovariateTable();
    }
    initializeSpeciesParameterTable();
    initializeInitialValuesAndRangesTable();
}

void
nmfEstimation_Tab6::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab6::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab6::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab6::callback_ImportPB()
{
    if (Estimation_Tab6_CovariateTV->isVisible()) {
        importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"Covariate",
                        nmfConstantsMSSPM::TableCovariate,
                        Estimation_Tab6_CovariateTV);
    } else if (Estimation_Tab6_SpeciesParameterTV->isVisible()) {
        importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"CovariateAssignment",
                        nmfConstantsMSSPM::TableCovariateAssignment,
                        Estimation_Tab6_SpeciesParameterTV);
    } else if (Estimation_Tab6_InitialValuesTV->isVisible()) {
        importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"CovariateInitialValuesAndRanges",
                        nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges,
                        Estimation_Tab6_InitialValuesTV);
    }
}

void
nmfEstimation_Tab6::callback_ExportPB()
{
    if (Estimation_Tab6_CovariateTV->isVisible()) {
        saveTableValuesToCSVFile("Covariate",
                                 nmfConstantsMSSPM::TableCovariate,
                                 m_smodelC);
    } else if (Estimation_Tab6_SpeciesParameterTV->isVisible()) {
        saveTableValuesToCSVFile("CovariateAssignment",
                                 nmfConstantsMSSPM::TableCovariateAssignment,
                                 m_smodelSP);
    } else if (Estimation_Tab6_InitialValuesTV->isVisible()) {
        saveTableValuesToCSVFile("CovariateInitialValuesAndRanges",
                                 nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges,
                                 m_smodelIR);
    }
}

void
nmfEstimation_Tab6::callback_SavePB()
{
    bool ok;
    if (Estimation_Tab6_CovariateTV->isVisible()) {
        saveCovariateAlgorithmType();
        ok = saveCovariateTable();
        if (ok) {
            initializeSpeciesParameterTable();
            initializeInitialValuesAndRangesTable();
            loadCovariateAssignmentTable();
            loadCovariateInitialValuesAndRangesTable();

            QMessageBox::information(Estimation_Tabs,"Save",
                                     "\nSuccessful save of table: "+QString::fromStdString(nmfConstantsMSSPM::TableCovariate),
                                     QMessageBox::Ok);
        }
    } else if (Estimation_Tab6_SpeciesParameterTV->isVisible()) {
        ok = saveCovariateAssignmentTable();
        if (ok) {
            initializeInitialValuesAndRangesTable();
            loadInitialValuesAndRangesForEditableCells();
            saveInitialValuesAndRangesTable();
            QMessageBox::information(Estimation_Tabs,"Save",
                                     "\nSuccessful save of table: "+QString::fromStdString(nmfConstantsMSSPM::TableCovariateAssignment),
                                     QMessageBox::Ok);
        }
    } else if (Estimation_Tab6_InitialValuesTV->isVisible()) {
        ok = saveInitialValuesAndRangesTable();
        if (ok) {
            QMessageBox::information(Estimation_Tabs,"Save",
                                     "\nSuccessful save of table: "+QString::fromStdString(nmfConstantsMSSPM::TableCovariateInitialValuesAndRanges),
                                     QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab6::callback_SetEstimateRunCheckboxes(
        std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes)
{
    std::string paramName;
    m_ParamNames.clear();
    for (int i=0; i<(int)EstimateRunBoxes.size();++i) {
        if (EstimateRunBoxes[i].state.first && EstimateRunBoxes[i].state.second) {
            paramName = EstimateRunBoxes[i].parameter;
            // Hard-coded: Only allow Growth Rate, Carrying Capacity, and
            // Catchability for estimatable Covariate Cofficients
            if ((paramName == "GrowthRate")       ||
                (paramName == "CarryingCapacity") ||
                (paramName == "Catchability"))
            {
                m_ParamNames.push_back(paramName);
            }
        }
    }

    sort(m_ParamNames.begin(),m_ParamNames.end());
    initializeSpeciesParameterTable();
    initializeInitialValuesAndRangesTable();
    loadCovariateAssignmentTable();
    loadCovariateInitialValuesAndRangesTable();
}

void
nmfEstimation_Tab6::callback_SpeciesRangeCMB(QString value)
{
    Estimation_Tab6_SpeciesRangeSB->setEnabled(value == "Percentage");
    Estimation_Tab6_MinMaxCMB->setEnabled(value == "Percentage");
}

void
nmfEstimation_Tab6::callback_SpeciesRangeSB(int pct)
{
    // Change parameter min/max values for the selected parameters or if none
    // are selected, for all parameters that have min/max values.
    int col;
    int row;
    double pctVal = double(pct)/100.0;
    QModelIndex initialIndex,minIndex,maxIndex;
    std::set<int> selectedParameters;
    QString rangeType = Estimation_Tab6_MinMaxCMB->currentText();
    QLocale locale(QLocale::English);
    QString valueWithoutComma;
    QString valueWithComma;
    double newValue;
    QStandardItem* item;
    QStandardItem* minItem;
    QStandardItem* maxItem;

    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab6_InitialValuesTV);

    int numRows = m_smodelIR->rowCount();
    int numCols = m_smodelIR->columnCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col=0; col<numCols; col+=3) {
                item = m_smodelIR->item(row,col);
                if (item->isEditable()) {
                    initialIndex = m_smodelIR->index(row,col);
                    valueWithoutComma = initialIndex.data().toString().remove(",");
                    if ((rangeType == "min/max") || (rangeType == "min only")) {
                        newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                        minItem = new QStandardItem(QString::number(newValue));
                        minItem->setTextAlignment(Qt::AlignCenter);
                        m_smodelIR->setItem(row,col+1,minItem);
                    }
                    if ((rangeType == "min/max") || (rangeType == "max only")) {
                        newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                        maxItem = new QStandardItem(QString::number(newValue));
                        maxItem->setTextAlignment(Qt::AlignCenter);
                        m_smodelIR->setItem(row,col+2,maxItem);
                    }
                }
            }
        }
    } else {
        // Find all columns with a selection and that have a mod%3 = 0 which means we're on an initial value cell
        for (QModelIndex index : indexes) {
            row = index.row();
            col = index.column();
            item = m_smodelIR->item(row,col);
            if (item->isEditable() and (col%3 == 1)) {
                initialIndex = m_smodelIR->index(row,col-1);
                minIndex     = m_smodelIR->index(row,col);
                valueWithoutComma = initialIndex.data().toString().remove(",");
                if ((rangeType == "min/max") || (rangeType == "min only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                    minItem = new QStandardItem(QString::number(newValue));
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_smodelIR->setItem(row,col,minItem);
                }
            } else if (item->isEditable() and (col%3 == 2)) {
                initialIndex = m_smodelIR->index(row,col-2);
                maxIndex     = m_smodelIR->index(row,col);
                valueWithoutComma = initialIndex.data().toString().remove(",");
                if ((rangeType == "min/max") || (rangeType == "max only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                    maxItem = new QStandardItem(QString::number(newValue));
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_smodelIR->setItem(row,col,maxItem);
                }
            }
        }
        nmfUtilsQt::reselectTableViewCells(Estimation_Tab6_InitialValuesTV,indexes);
    }
    Estimation_Tab6_InitialValuesTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab6::callback_ViewNormalizedPB(bool checked)
{
    bool showNormalizedValues = (checked == true);

    loadCovariateTable();

    Estimation_Tab6_SavePB->setEnabled(!showNormalizedValues);
    Estimation_Tab6_LoadPB->setEnabled(!showNormalizedValues);
    Estimation_Tab6_ImportPB->setEnabled(!showNormalizedValues);
    Estimation_Tab6_ExportPB->setEnabled(!showNormalizedValues);
}

void
nmfEstimation_Tab6::callback_ClearInitialValuesAndRangePB()
{
    // Set parameter min/max values to 0 for all selected cells or to all cells if none are selected
    int col;
    int row;
    std::set<int> selectedParameters;
    QLocale locale(QLocale::English);
    QString initialValue;
    QStandardItem* item;
    QStandardItem* initialItem;
    QStandardItem* minItem;
    QStandardItem* maxItem;

    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab6_InitialValuesTV);

    int numRows = m_smodelIR->rowCount();
    int numCols = m_smodelIR->columnCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col=0; col<numCols; col+=3) {
                item = m_smodelIR->item(row,col);
                if (item->isEditable()) {
                    initialItem = new QStandardItem("0");
                    minItem = new QStandardItem("0");
                    maxItem = new QStandardItem("0");
                    initialItem->setTextAlignment(Qt::AlignCenter);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_smodelIR->setItem(row,col,  initialItem);
                    m_smodelIR->setItem(row,col+1,minItem);
                    m_smodelIR->setItem(row,col+2,maxItem);
                }
            }
        }
    } else {
        // Find all columns with a selection and that have a mod%3 = 0 which means we're on an initial value cell
        for (QModelIndex index : indexes) {
            row    = index.row();
            col    = index.column();
            item = m_smodelIR->item(row,col);
            if (item->isEditable()) {
               item->setText("0");
            }
        }
        nmfUtilsQt::reselectTableViewCells(Estimation_Tab6_InitialValuesTV,indexes);
    }
    Estimation_Tab6_InitialValuesTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab6::callback_SetEstOffPB()
{
    // Set parameter min/max values to the initial cell value for the selected parameters. If none
    // are selected, set for all parameters that have min/max values.
    int col;
    int row;
    int initialCol=0;
    int minCol=0;
    int maxCol=0;
    int modCol;
    QModelIndex initialIndex,minIndex,maxIndex;
    std::set<int> selectedParameters;
    QString rangeType = Estimation_Tab6_MinMaxCMB->currentText();
    QLocale locale(QLocale::English);
    QString initialValue;
    QStandardItem* item;
    QStandardItem* minItem;
    QStandardItem* maxItem;

    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab6_InitialValuesTV);

    int numRows = m_smodelIR->rowCount();
    int numCols = m_smodelIR->columnCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col=0; col<numCols; col+=3) {
                item = m_smodelIR->item(row,col);
                if (item->isEditable()) {
                    initialIndex = m_smodelIR->index(row,col);
                    initialValue = initialIndex.data().toString();
                    minItem = new QStandardItem(initialValue);
                    maxItem = new QStandardItem(initialValue);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_smodelIR->setItem(row,col+1,minItem);
                    m_smodelIR->setItem(row,col+2,maxItem);
                }
            }
        }
    } else {
        // Find all columns with a selection and that have a mod%3 = 0 which means we're on an initial value cell
        for (QModelIndex index : indexes) {
            row    = index.row();
            col    = index.column();
            modCol = col%3;
            item = m_smodelIR->item(row,col);
            if (item->isEditable() and ((modCol == 0) || (modCol == 1) || (modCol == 2))) {
               if (modCol == 0) {
                   initialCol = col;
                   minCol     = col+1;
                   maxCol     = col+2;
               } else if (modCol == 1) {
                   initialCol = col-1;
                   minCol     = col;
                   maxCol     = col+1;
               } else if (modCol == 2) {
                   initialCol = col-2;
                   minCol     = col-1;
                   maxCol     = col;
               }
               initialIndex = m_smodelIR->index(row,initialCol);
               initialValue = initialIndex.data().toString();
               minIndex     = m_smodelIR->index(row,minCol);
               maxIndex     = m_smodelIR->index(row,maxCol);
               minItem      = new QStandardItem(initialValue);
               maxItem      = new QStandardItem(initialValue);
               minItem->setTextAlignment(Qt::AlignCenter);
               maxItem->setTextAlignment(Qt::AlignCenter);
               m_smodelIR->setItem(row,minCol,minItem);
               m_smodelIR->setItem(row,maxCol,maxItem);
            }
        }
        nmfUtilsQt::reselectTableViewCells(Estimation_Tab6_InitialValuesTV,indexes);
    }
    Estimation_Tab6_InitialValuesTV->resizeColumnsToContents();
}
