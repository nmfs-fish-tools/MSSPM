
#include "nmfEstimationTab05.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab5::nmfEstimation_Tab5(QTabWidget  *tabs,
                             nmfLogger   *theLogger,
                             nmfDatabase *theDatabasePtr,
                             std::string &theProjectDir)
{
    QUiLoader loader;

    m_Logger           = theLogger;
    m_DatabasePtr      = theDatabasePtr;
    m_ProjectSettingsConfig.clear();
    m_SModelBiomass    = NULL;
    m_SModelCovariates = NULL;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab5::nmfEstimation_Tab5");

    Estimation_Tabs = tabs;

    m_ProjectDir = theProjectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab05.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab5_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab5_Widget, tr("5. Observation Data"));

    Estimation_Tab5_CovariatesGB = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab5_CovariatesGB");
    Estimation_Tab5_BiomassTV    = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab5_BiomassTV");
    Estimation_Tab5_CovariatesTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab5_CovariatesTV");
    Estimation_Tab5_PrevPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab5_PrevPB");
    Estimation_Tab5_NextPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab5_NextPB");
    Estimation_Tab5_LoadPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab5_LoadPB");
    Estimation_Tab5_SavePB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab5_SavePB");
    Estimation_Tab5_ImportPB     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab5_ImportPB");

    connect(Estimation_Tab5_PrevPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_PrevPB()));
    connect(Estimation_Tab5_NextPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_NextPB()));
    connect(Estimation_Tab5_LoadPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_LoadPB()));
    connect(Estimation_Tab5_SavePB,   SIGNAL(clicked()),
            this,                     SLOT(callback_SavePB()));
    connect(Estimation_Tab5_ImportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ImportPB()));

    Estimation_Tab5_PrevPB->setText("\u25C1--");
    Estimation_Tab5_NextPB->setText("--\u25B7");

    Estimation_Tab5_CovariatesGB->setVisible(false);

} // end constructor


nmfEstimation_Tab5::~nmfEstimation_Tab5()
{

}

void
nmfEstimation_Tab5::clearWidgets()
{
    nmfUtilsQt::clearTableView(
        {Estimation_Tab5_BiomassTV,
         Estimation_Tab5_CovariatesTV});
}

//void
//nmfEstimation_Tab5::callback_SetAlgorithm(QString algorithm)
//{
//    Estimation_Tab5_CovariatesGB->setVisible(algorithm == "Genetic Algorithm");
//}


void
nmfEstimation_Tab5::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab5::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab5::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab5::callback_ImportPB()
{
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    // Load default CSV files
    std::string msg = "\nLoad default Observed Biomass .csv file?";
    std::string tableName = "ObservedBiomass";
    QMessageBox::StandardButton reply = QMessageBox::question(
                Estimation_Tabs, tr("Default Observed Biomass CSV File"),
                tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes, QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        loadCSVFile(tableName);
    } else {
        // if no, raise browser and have user select the appropriate Observed Biomass file.
        QString filename = QFileDialog::getOpenFileName(
                    Estimation_Tabs,
                    QObject::tr("Select Observed Biomass file"), inputDataPath,
                    QObject::tr("Data Files (*.csv)"));
        if (filename.contains("_")) {
            QFileInfo fi(filename);
            QString base = fi.baseName();
            QStringList parts = base.split("_");
            if (parts.size() == 2) {
                QString tag = parts[1];
                tableName += "_"+tag.toStdString();
                loadCSVFile(tableName);
            } else {
                QMessageBox::information(Estimation_Tabs, "Observed Biomass CSV Import",
                                         "\nPlease make sure to select the Observed Biomass filename that contains the appropriate tag\n",
                                         QMessageBox::Ok);
            }
        } else {
            QMessageBox::critical(Estimation_Tabs, "Observed Biomass CSV Import",
                                  "\nError: No tag found in filename\n",
                                  QMessageBox::Ok);
        }
    }
}

void
nmfEstimation_Tab5::loadCSVFile(std::string& tableName)
{
    bool loadOK;
    QString errorMsg;
    QString tableNameStr;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    tableNameStr = QString::fromStdString(tableName);
    tableNameStr = QDir(inputDataPath).filePath(tableNameStr+".csv");
    loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, Estimation_Tab5_BiomassTV, inputDataPath, tableNameStr,
                nmfConstantsMSSPM::FirstLineNotReadOnly,errorMsg);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }
}

void
nmfEstimation_Tab5::callback_SavePB()
{
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> SpeNames;
    std::vector<double> SpeciesKMin;
    int NumSpecies;
    double InitBiomass;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string MohnsRhoLabel = "";
    QString value;
    QString msg;

    if ((m_SModelBiomass == NULL) || (m_SModelCovariates == NULL)) {
        return;
    }
    SpeciesKMin.clear();

    // Re-load first row of biomass since the init biomass is not allowed to be changed
    loadWidgetsFirstRow();

    // Find number of species
    NumSpecies = m_SModelBiomass->columnCount();

    // Get list of Species names
    for (int species=0; species<NumSpecies; ++species) {
        SpeNames.push_back(m_SModelBiomass->horizontalHeaderItem(species)->text().toStdString());
    }

    // Get SpeciesKMin values for all Species
    fields     = {"SpeName","SpeciesKMin"};
    queryStr   = "SELECT SpeName,SpeciesKMin from Species ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int species=0; species<NumSpecies; ++species) {
        SpeciesKMin.push_back(std::stod(dataMap["SpeciesKMin"][species]));
    }
    // Check that InitBiomass < the SpeciesKMin value in the Species table
    for (int species=0; species<NumSpecies; ++species) {
        index = m_SModelBiomass->index(0,species);
        InitBiomass = index.data().toDouble();
        if (InitBiomass > SpeciesKMin[species]) {
            errorMsg  = "\nFound: InitBiomass > SpeciesKMin for Species: " + SpeNames[species];
            errorMsg += "\n\nInitBiomass must be less than SpeciesKMin.\n";
            QMessageBox::warning(Estimation_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
    }


    // Check data integrity
    for (int j=0; j<m_SModelBiomass->columnCount(); ++j) { // Species
        for (int i=0; i<m_SModelBiomass->rowCount(); ++i) { // Time
            index = m_SModelBiomass->index(i,j);
            value = index.data().toString();
            if (value.contains(',')) {
                msg = "Invalid value found. No commas or special characters allowed in a number.";
                m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
                QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
                return;
            }
        }
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    cmd = "DELETE FROM ObservedBiomass WHERE SystemName = '" +
           m_ProjectSettingsConfig +
           "' AND MohnsRhoLabel = ''";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab5::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from ObservedBiomass table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    cmd = "INSERT INTO ObservedBiomass (MohnsRhoLabel,SystemName,SpeName,Year,Value) VALUES ";
    for (int species=0; species<NumSpecies; ++species) {
        for (int time=0; time<m_SModelBiomass->rowCount(); ++time) {
            index = m_SModelBiomass->index(time,species);
            value = index.data().toString();
            cmd += "('"   + MohnsRhoLabel +
                    "','" + m_ProjectSettingsConfig +
                    "','" + SpeNames[species] +
                    "',"  + std::to_string(time) +
                    ", "  + value.toStdString() + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab5::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    // Need to also update the Species table with the initial Biomass values
    for (int species=0; species<NumSpecies; ++species) {
        index = m_SModelBiomass->index(0,species);
        value = index.data().toString();
        cmd  = "UPDATE Species SET InitBiomass = " + value.toStdString();
        cmd += " WHERE SpeName = '" + SpeNames[species] + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab5::callback_SavePB (Species): Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO Species table.\n",
                                 QMessageBox::Ok);
            return;
        }
    }
    emit ReloadSpecies(nmfConstantsMSSPM::ShowPopupError);


    cmd = "DELETE FROM Covariate";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab5::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from CovariteTS table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    cmd = "INSERT INTO Covariate (Year,Value) VALUES ";
    for (int i=0; i<m_SModelCovariates->rowCount(); ++i) { // Time in years
        for (int j=0; j<m_SModelCovariates->columnCount(); ++ j) {

            // RSK - This table isn't currently used.  It was for Kraken. Setting values to "0".
            // index = smodelCovariates->index(i,j);
            // value = index.data().toString().toStdString();
            value = "0";

            cmd += "(" + std::to_string(i) + ", " + value.toStdString() + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 5] nmfEstimation_Tab5::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    Estimation_Tab5_BiomassTV->resizeColumnsToContents();
    Estimation_Tab5_CovariatesTV->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, "Observed Data Updated",
                             "\nObserved data tables have been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);

    // Save time series data to a .csv file
    std::string tableName = "ObservedBiomass";
    msg = "\nOK to use default file name for Observed Biomass .csv file and overwrite any previous file?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Observed Biomass CSV File"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        saveCSVFile(tableName);
    } else {
        bool ok;
        QString tag = QInputDialog::getText(Estimation_Tabs, tr("Observed Biomass Files"),
                                            tr("Enter Observed Biomass CSV filename version tag (omit any '_'): "),
                                            QLineEdit::Normal, "", &ok);
        if (ok && !tag.isEmpty()) {
            tableName += "_"+tag.toStdString();
            saveCSVFile(tableName);
        }
    }
}

void
nmfEstimation_Tab5::saveCSVFile(std::string& tableName)
{
    QString tableNameWithPath;

    // Save time series data to a .csv file
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    nmfUtilsQt::saveTimeSeries(Estimation_Tabs,m_SModelBiomass,inputDataPath,tableNameWithPath);

    QMessageBox::information(Estimation_Tabs, "Observed Biomass File Saved",
                             "\nObserved Biomass CSV file has been successfully saved.\n",
                             QMessageBox::Ok);
}

void
nmfEstimation_Tab5::readSettings()
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

bool
nmfEstimation_Tab5::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab5::loadWidgets()");

    loadWidgets("");

    return true;
}

bool
nmfEstimation_Tab5::loadWidgetsFirstRow()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    Qt::ItemFlags flags;

    m_SModelBiomass = qobject_cast<QStandardItemModel*>(Estimation_Tab5_BiomassTV->model());

    fields     = {"SpeName","InitBiomass"};
    queryStr   = "SELECT SpeName,InitBiomass FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);

    for (unsigned j=0; j<dataMap["SpeName"].size(); ++j) {
        item = new QStandardItem(QString::fromStdString(dataMap["InitBiomass"][j]));
        item->setTextAlignment(Qt::AlignCenter);
//        item->setEditable(false); // Make first row read-only
//        item->setBackground(QBrush(QColor(240,240,240)));
        m_SModelBiomass->setItem(0, j, item);
    }

    //        Estimation_Tab5_BiomassTV->setModel(m_SModelBiomass);
    Estimation_Tab5_BiomassTV->resizeColumnsToContents();




    return true;
}

bool
nmfEstimation_Tab5::loadWidgets(QString MohnsRhoLabel)
{
    int m;
    int NumSpecies;
    int NumRecords;
    int RunLength;
    int StartYear;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    std::vector<std::string> SpeciesNames;
    QStringList VerticalList;
    QStringList CovariatesHeaderList;
    QStringList SpeciesList;
    QString SystemName = QString::fromStdString(m_ProjectSettingsConfig);

    // Strip off the MohnsRho suffix
    auto parts = SystemName.split("__");
    SystemName =  parts[0];

    readSettings();
    if (SystemName.isEmpty())
        return false;

    clearWidgets();

    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectSettingsConfig,RunLength,StartYear)) {
        return false;
    }

    if (! m_DatabasePtr->getAllSpecies(m_Logger,SpeciesNames)) {
        return false;
    }
    NumSpecies = SpeciesNames.size();
    nmfUtilsQt::convertVectorToStrList(SpeciesNames,SpeciesList);

    fields     = {"MohnsRhoLabel","SystemName","SpeName","Year","Value"};
    queryStr   = "SELECT MohnsRhoLabel,SystemName,SpeName,Year,Value FROM ObservedBiomass WHERE SystemName = '" +
                 SystemName.toStdString() + "' AND MohnsRhoLabel = '" +
                 MohnsRhoLabel.toStdString() + "' ORDER BY SpeName,Year ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    Qt::ItemFlags flags;

    if (NumRecords > 0) {
        // Load BiomassTV
        m = 0;
        for (int i=0; i<=RunLength; ++i) {
            VerticalList << " " + QString::number(StartYear+i) + " ";
        }
        m_SModelBiomass = new QStandardItemModel( RunLength, NumSpecies );
        for (int j=0; j<NumSpecies; ++j) {
            for (int i=0; i<=RunLength; ++i) {
                if ((m < NumRecords) && (SpeciesNames[j] == dataMap["SpeName"][m])) {
                    item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                } else {
                    item = new QStandardItem(QString(""));
                }
                item->setTextAlignment(Qt::AlignCenter);
                item->setEditable(i != 0); // Make first row read-only
                if (i == 0) {
                    item->setBackground(QBrush(QColor(240,240,240)));
                    flags = item->flags();
                    flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEditable); // reset/clear the flag
                    item->setFlags(flags);
                }
                m_SModelBiomass->setItem(i, j, item);
            }
        }

        m_SModelBiomass->setVerticalHeaderLabels(VerticalList);
        m_SModelBiomass->setHorizontalHeaderLabels(SpeciesList);
        Estimation_Tab5_BiomassTV->setModel(m_SModelBiomass);
        Estimation_Tab5_BiomassTV->resizeColumnsToContents();
    } else {
        callback_UpdateInitialObservedBiomass();
    }

    // Load Covariate table
    fields     = {"Year","Value"};
    queryStr   = "SELECT Year,Value FROM Covariate";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Year"].size();

    // Load Covariates model
    m_SModelCovariates = new QStandardItemModel( RunLength, 1 );
    for (int i=0; i<=RunLength; ++i) {
        if (i < NumRecords)
            item = new QStandardItem(QString::fromStdString(dataMap["Value"][i]));
        else
            item = new QStandardItem(QString(""));
        item->setTextAlignment(Qt::AlignCenter);
        m_SModelCovariates->setItem(i, 0, item);
    }
    CovariatesHeaderList << "Covariates";
    m_SModelCovariates->setVerticalHeaderLabels(VerticalList);
    m_SModelCovariates->setHorizontalHeaderLabels(CovariatesHeaderList);
    Estimation_Tab5_CovariatesTV->setModel(m_SModelCovariates);
    Estimation_Tab5_CovariatesTV->resizeColumnsToContents();

    return true;
}

void
nmfEstimation_Tab5::callback_UpdateInitialObservedBiomass()
{
    int NumSpecies;
    int RunLength;
    int StartYear;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList SpeciesNames;
    QStringList VerticalList;
    QStandardItem *item;

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectSettingsConfig,RunLength,StartYear);

    // Populate first row of Observed Biomass with Init Biomass from Species
    fields     = {"SpeName","InitBiomass"};
    queryStr   = "SELECT SpeName,InitBiomass FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    m_SModelBiomass = new QStandardItemModel( RunLength, NumSpecies );

    for (int i=0; i<=RunLength; ++i) {
        VerticalList << " " + QString::number(StartYear+i) + " ";
    }
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
        item = new QStandardItem(QString::fromStdString(dataMap["InitBiomass"][j]));
        item->setTextAlignment(Qt::AlignCenter);
        m_SModelBiomass->setItem(0, j, item);
        for (int k=1; k<RunLength; ++k) {
            item = new QStandardItem("");
            m_SModelBiomass->setItem(k, j, item);
        }
    }

    m_SModelBiomass->setVerticalHeaderLabels(VerticalList);
    m_SModelBiomass->setHorizontalHeaderLabels(SpeciesNames);
    Estimation_Tab5_BiomassTV->setModel(m_SModelBiomass);
    Estimation_Tab5_BiomassTV->resizeColumnsToContents();

}

bool
nmfEstimation_Tab5::areTablesOK()
{
    return nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab5_BiomassTV,
                nmfConstantsMSSPM::DontShowError);
}
