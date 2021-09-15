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
    m_LastFileLoaded       = "";
    m_LastLabel            = "";
    m_LastTableView        = nullptr;
    m_LastStatisticNames.clear();
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
nmfOutputTableWidgets::reloadLast()
{
    if ((m_LastTableView == nullptr) &&
         m_LastLabel.isEmpty()       &&
         m_LastStatisticNames.isEmpty()) {
            return;
    }

    if (m_LastFileLoaded.isEmpty()) { // Re-load table from database
        loadSummaryTable(m_LastTableView,m_LastLabel,m_LastStatisticNames);

    } else {                          // Re-load last imported file
        int numRows = 0;
        nmfUtilsQt::loadModelFromCSVFile(m_Logger,m_ProjectDir,
                                         m_LastLabel.toStdString(),m_LastTableView,
                                         m_LastFileLoaded,numRows,m_NumSignificantDigits);
        m_LastTableView->resizeColumnsToContents();
        m_LastTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void
nmfOutputTableWidgets::setLastVariables(QTableView* tableView,
                                        const QString& label,
                                        const QStringList& statisticNames)
{
    m_LastFileLoaded     = "";
    m_LastLabel          = label;
    m_LastTableView      = tableView;
    m_LastStatisticNames = statisticNames;
}

void
nmfOutputTableWidgets::loadSummaryTable(QTableView* tableView,
                                        const QString& label,
                                        const QStringList& statisticNames)
{
    int j;
    int NumFields;
    int NumDatabaseTableRows;
    int NumGuiTableCols;
    int NumGuiTableRows = statisticNames.size();
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

    m_LastFileLoaded     = "";
    m_LastLabel          = label;
    m_LastTableView      = tableView;
    m_LastStatisticNames = statisticNames;

    if (label == "Summary Model Fit") {
        fields    = {"ProjectName","ModelName","SpeciesName","SSResiduals","SSDeviations","SSTotals",
                     "rSquared","rCorrelationCoeff","AkaikeInfoCriterion","RootMeanSquareError",
                     "ReliabilityIndex","AverageError","AverageAbsError","ModelingEfficiency"};
        queryStr  = "SELECT ProjectName,ModelName,SpeciesName,SSResiduals,SSDeviations,SSTotals,rSquared,";
        queryStr += "rCorrelationCoeff,AkaikeInfoCriterion,RootMeanSquareError,ReliabilityIndex,AverageError,";
        queryStr += "AverageAbsError,ModelingEfficiency FROM " +
                     nmfConstantsMSSPM::TableSummaryModelFit;
    } else {
        fields    = {"ProjectName","ModelName","SpeciesName","InitialAbsBiomass","GrowthRate",
                     "CarryingCapacity","PredationEffect","EstimatedBiomass"};
        queryStr  = "SELECT ProjectName,ModelName,SpeciesName,InitialAbsBiomass,GrowthRate,";
        queryStr += "CarryingCapacity,PredationEffect,EstimatedBiomass FROM " +
                     nmfConstantsMSSPM::TableSummaryDiagnostic;
    }
    queryStr += " WHERE ProjectName='" + m_ProjectName +
                "' AND ModelName='"    + m_ModelName   + "'";
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
        valueStr = statisticNames[row];
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
nmfOutputTableWidgets::importSummaryTable(QTableView* tableView,
                                          const QString& label,
                                          const std::string& defaultFilenameCSV)
{
    int numRows = 0;
    QString fileName;
    QString filePath;
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString title1 = "Default " + label + " CSV file";
    QString title2 = "Select "  + label + " CSV file";

    m_LastLabel     = label;
    m_LastTableView = tableView;

    // Load default CSV files
    std::string msg = "\nLoad default " + label.toStdString() + " .csv file?";
    QMessageBox::StandardButton reply = QMessageBox::question(
                m_Parent, tr(title1.toLatin1()),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    } else if (reply == QMessageBox::Yes) {
        fileName = QDir(dataPath).filePath(QString::fromStdString(defaultFilenameCSV));
    } else {
        fileName = QFileDialog::getOpenFileName(m_Parent,
                    QObject::tr(title2.toLatin1()), dataPath,
                    QObject::tr("Data Files (*.csv)"));
    }

    if (! fileName.isEmpty()) {
        bool loadOK = nmfUtilsQt::loadModelFromCSVFile(m_Logger,m_ProjectDir,
                                                       label.toStdString(),tableView,
                                                       fileName,numRows,m_NumSignificantDigits);
        if (numRows < 2) {
            QMessageBox::warning(m_Parent, "Warning",
                                 "\nEmpty Summary file found.\n",
                                 QMessageBox::Ok);
            return;
        }
        m_LastFileLoaded = fileName;
    }

    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void
nmfOutputTableWidgets::exportSummaryTable(QTableView* tableView,
                                          const QString& label,
                                          const std::string& defaultFilenameCSV)
{
    QString title = "Export " + label + " Table";
    if (tableView == nullptr) {
        QMessageBox::critical(m_Parent,
                              tr("No table found"),
                              tr("\nThe passed in summary tableview is not defined.\n"),
                              QMessageBox::Ok);
        return;
    }
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(defaultFilenameCSV));
    QString fileName = QFileDialog::getSaveFileName(m_Parent, tr(title.toLatin1()),
                                                    filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,
                                       label.toStdString(),
                                       label.toStdString(),
                                       tableView,
                                       nmfConstantsMSSPM::Dont_Query_User_For_Filename,
                                       nmfConstantsMSSPM::RemoveCommas,
                                       fileName,
                                       nmfConstantsMSSPM::VerboseOn);
    }
}
