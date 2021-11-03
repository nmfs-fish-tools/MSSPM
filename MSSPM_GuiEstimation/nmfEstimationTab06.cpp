
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
    Estimation_Tab6_CovariateTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_CovariateTV");
    Estimation_Tab6_SpeciesParameterTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_SpeciesParameterTV");

    Estimation_Tab6_PrevPB->setText("\u25C1--");
    Estimation_Tab6_NextPB->setText("--\u25B7");

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

    initializeCovariateTable();
    initializeSpeciesParameterTable();

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

    QStandardItemModel* smodelC = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

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

    loadCovariateTable();
    loadCovariateAssignmentTable();
}

void
nmfEstimation_Tab6::loadCovariateAssignmentTable()
{
    int m=0;
    int NumRecords;
    int numRows = m_smodelSP->rowCount();
    int numCols = m_smodelSP->columnCount();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string tableName = nmfConstantsMSSPM::TableCovariateAssignment;
    QComboBox* cbox;
    QModelIndex index;
    QStandardItemModel* smodelSP = qobject_cast<QStandardItemModel*>(Estimation_Tab6_SpeciesParameterTV->model());

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
    std::string tableName = nmfConstantsMSSPM::TableCovariate;

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
    numRows = RunLength+1;

    // Get data from database table
    fields     = {"ProjectName","ModelName","CovariateName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,CovariateName,Year,Value FROM " +
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
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
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
    }
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
        loadCSVFile(firstLineReadOnly,tableName,tableView);
    } else {
        // if no, raise browser and have user select the appropriate Observed Biomass file.
        QString filename = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr("Select "+type.toLatin1()+" file"), inputDataPath,
                    QObject::tr("Data Files (Covariate*.csv covariate*.csv)"));
        QFileInfo fi(filename);
        QString filenameNoPath = fi.baseName();
        loadCSVFile(firstLineReadOnly,filenameNoPath.toStdString(),tableView);
    }
}

void
nmfEstimation_Tab6::loadCSVFile(const bool& firstLineReadOnly,
                                const std::string& tableName,
                                QTableView* tableView)
{
    bool loadOK = false;
    QString errorMsg;
    QString tableNameStr;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::pair<int,int> nonZeroCell;

    tableNameStr = QString::fromStdString(tableName);
    tableNameStr = QDir(inputDataPath).filePath(tableNameStr+".csv");

    if (Estimation_Tab6_CovariateTV->isVisible()) {
        loadOK = nmfUtilsQt::loadTimeSeries(
                    Estimation_Tabs, tableView, inputDataPath, tableNameStr,
                    firstLineReadOnly,
                    nmfConstantsMSSPM::FixedNotation,
                    nonZeroCell,errorMsg);
    } else {
        loadOK = nmfUtilsQt::loadCSVFileComboBoxes(
                    Estimation_Tabs, tableView, inputDataPath, tableNameStr,
                    errorMsg);
    }

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
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
    } else {
        okSave = nmfUtilsQt::saveCSVFileComboBoxes(Estimation_Tabs,Estimation_Tab6_SpeciesParameterTV,
                                                   smodel,inputDataPath,tableNameWithPath);
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
        calculateCovariateScaleFactor(col,min,diff); // N.B. scaleFactor must never be 0
// Currently disabled
//        if (diff == 0.0) {
//            m_Logger->logMsg(nmfConstants::Error,"Couldn't rescale covariate data. Found max = min.");
//            QMessageBox::warning(Estimation_Tabs, "Save Error",
//                                 "\nSave failed. Couldn't rescale covariate data. Found max = min.\n",
//                                 QMessageBox::Ok);
//            return false;
//        }
        for (int row=0; row<numRows; ++row) {
            index = m_smodelC->index(row,col);
            value = index.data().toString();
// Currently disabled
//          valueScaled = QString::number(2.0*(value.toDouble()-min)/diff - 1.0);
            valueScaled = "0.0";
            saveCmd += "('"  + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + m_smodelC->horizontalHeaderItem(col)->text().toStdString() +
                       "', " + m_smodelC->verticalHeaderItem(row)->text().toStdString() +
                       " , " + value.toStdString() +
                       " , " + valueScaled.toStdString() + "),";
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
        value = index.data().toDouble();
        max   = (value > max) ? value : max;
        min   = (value < min) ? value : min;
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

//    loadCovariateAssignmentTable();
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
    } else {
        importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"CovariateAssignment",
                        nmfConstantsMSSPM::TableCovariateAssignment,
                        Estimation_Tab6_SpeciesParameterTV);
    }
}

void
nmfEstimation_Tab6::callback_ExportPB()
{
    if (Estimation_Tab6_CovariateTV->isVisible()) {
        saveTableValuesToCSVFile("Covariate",
                                 nmfConstantsMSSPM::TableCovariate,
                                 m_smodelC);
    } else {
        saveTableValuesToCSVFile("CovariateAssignment",
                                 nmfConstantsMSSPM::TableCovariateAssignment,
                                 m_smodelSP);
    }
}

void
nmfEstimation_Tab6::callback_SavePB()
{
    bool ok;
    if (Estimation_Tab6_CovariateTV->isVisible()) {
        ok = saveCovariateTable();
        if (ok) {
            initializeSpeciesParameterTable();
            loadCovariateAssignmentTable();
            QMessageBox::information(Estimation_Tabs,"Save",
                                     "\nSuccessful save of table: "+QString::fromStdString(nmfConstantsMSSPM::TableCovariate),
                                     QMessageBox::Ok);
        }
    } else {
        ok = saveCovariateAssignmentTable();
        if (ok) {
            QMessageBox::information(Estimation_Tabs,"Save",
                                     "\nSuccessful save of table: "+QString::fromStdString(nmfConstantsMSSPM::TableCovariateAssignment),
                                     QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab6::callback_SetEstimateRunCheckboxes(
        std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes)
{
    m_ParamNames.clear();
    for (int i=0; i<(int)EstimateRunBoxes.size();++i) {
        if (EstimateRunBoxes[i].state.first && EstimateRunBoxes[i].state.second) {
            m_ParamNames.push_back(EstimateRunBoxes[i].parameter);
        }
    }
    sort(m_ParamNames.begin(),m_ParamNames.end());
    initializeSpeciesParameterTable();
}
