#include "nmfOutputTableWidgets.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfOutputTableWidgets::nmfOutputTableWidgets(
        QWidget*     Parent,
        nmfLogger*   logger,
        nmfDatabase* databasePtr,
        std::string& projectDir)
{
    m_Parent               = Parent;
    m_Logger               = logger;
    m_DatabasePtr          = databasePtr;
    m_ProjectDir           = projectDir;
    m_NumSignificantDigits = -1;
    m_ProjectName.clear();
    m_ModelName.clear();

    readSettings();
}

void
nmfOutputTableWidgets::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir", "").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}

void
nmfOutputTableWidgets::loadWidgets()
{
    readSettings();
}

void
nmfOutputTableWidgets::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    delete settings;
}

void
nmfOutputTableWidgets::loadSummaryTable(QTableView* tableView)
{
    int j;
    int NumFields;
    int NumDatabaseTableRows;
    int NumGuiTableCols;
    int NumGuiTableRows = nmfConstantsMSSPM::StatisticNames.size();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string species;
    std::string val;
    QStandardItemModel* smodel;
    QStandardItem* item;
    QString valueStr;
    QString valueWithComma;
    QStringList header;

    fields    = {"ProjectName","ModelName","SpeciesName","SSResiduals","SSDeviations","SSTotals",
                 "rSquared","rCorrelationCoeff","AkaikeInfoCriterion","RootMeanSquareError",
                 "ReliabilityIndex","AverageError","AverageAbsError","ModelingEfficiency"};
    queryStr  = "SELECT ProjectName,ModelName,SpeciesName,SSResiduals,SSDeviations,SSTotals,rSquared,";
    queryStr += "rCorrelationCoeff,AkaikeInfoCriterion,RootMeanSquareError,ReliabilityIndex,AverageError,";
    queryStr += "AverageAbsError,ModelingEfficiency FROM " + nmfConstantsMSSPM::TableSummaryModelFit;
    queryStr += " WHERE ProjectName='" + m_ProjectName + "' AND ModelName='" + m_ModelName + "'";
    queryStr += " ORDER BY SpeciesName";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumDatabaseTableRows = (int)dataMap["SpeciesName"].size();
    NumFields = (int)fields.size();

    // Load horizontal header
    header << "Statistic";
    for (int row=0; row<NumDatabaseTableRows; ++row) {
        species = dataMap["SpeciesName"][row];
        if ((species != "Statistic") && (species != "Model")) {
            header << QString::fromStdString(species);
        }
    }
    header << "Model";
    NumGuiTableCols = (int)header.size();

    // Set size of table
    smodel = new QStandardItemModel(NumGuiTableRows, NumGuiTableCols);

    // Load first column of table
    for (int row=0; row<NumGuiTableRows; ++row) {
        valueStr = nmfConstantsMSSPM::StatisticNames[row];
        item = new QStandardItem(valueStr);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable); // read-only
        smodel->setItem(row,0,item);
    }

    // Load rest of columns of table
    for (int k=3; k<NumFields;++k) {
        j = 1;
        for (int row=0; row<NumDatabaseTableRows; ++row) {
            species = dataMap["SpeciesName"][row];
            if (species != "Statistic") {
                valueStr = QString::fromStdString(dataMap[fields[k]][row]);
                item = new QStandardItem(valueStr);
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // read-only
                if (species == "Model") {
                    smodel->setItem(k-3,NumGuiTableCols-1,item);
                } else {
                    smodel->setItem(k-3,j++,item);
                }
            }
        }
    }

    smodel->setHorizontalHeaderLabels(header);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void
nmfOutputTableWidgets::importSummaryTable(QTableView* tableView)
{
    int numRows = 0;
    QString fileName;
    QString filePath;
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");

    // Load default CSV files
    std::string msg = "\nLoad default Summary Model Fit .csv file?";
    QMessageBox::StandardButton reply = QMessageBox::question(
                m_Parent, tr("Default Summary Model Fit CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        fileName = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameSummaryModelFit));
//      fileName = QFileDialog::getOpenFileName(m_Parent, tr("Import Summary Model Fit File"),
//                                              filePath,tr("data (*.csv)"));
    } else {
        fileName = QFileDialog::getOpenFileName(m_Parent,
                    QObject::tr("Select Summary Model Fit CSV file"), dataPath,
                    QObject::tr("Data Files (*.csv)"));
    }

    if (! fileName.isEmpty()) {
        bool loadOK = nmfUtilsQt::loadModelFromCSVFile(m_ProjectDir,
                                                       "Summary Model Fit",tableView,
                                                       fileName,numRows,m_NumSignificantDigits);
        if (numRows < 2) {
            QMessageBox::warning(m_Parent, "Warning",
                                 "\nEmpty Summary Model Fit file.\n",
                                 QMessageBox::Ok);
            return;
        }
//      tableView->horizontalHeader()->show();
    }

    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void
nmfOutputTableWidgets::exportSummaryTable(QTableView* tableView)
{
    if (tableView == nullptr) {
        QMessageBox::critical(m_Parent,
                              tr("No table found"),
                              tr("\nThe SummaryTV tableview is not defined.\n"),
                              QMessageBox::Ok);
        return;
    }
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameSummaryModelFit));
    QString fileName = QFileDialog::getSaveFileName(m_Parent, tr("Export Summary Model Fit Table"),
                                                    filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,
                                       "Summary Model Fit",
                                       "Model Fit Summary",
                                       tableView,
                                       nmfConstantsMSSPM::Dont_Query_User_For_Filename,
                                       nmfConstantsMSSPM::RemoveCommas,
                                       fileName,
                                       nmfConstantsMSSPM::VerboseOn);
    }
}
