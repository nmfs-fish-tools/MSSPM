
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

    m_Logger      = theLogger;
    m_DatabasePtr = theDatabasePtr;
    m_ModelName.clear();
    m_ProjectName.clear();
    m_SModelAbsoluteBiomass = nullptr;
    m_SModelRelativeBiomass = nullptr;
    m_SModelCovariates      = nullptr;
    // assuming that the default is light.
    m_IsDark = false;
    m_NumSignificantDigits = -1;

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

    Estimation_Tab5_CovariatesGB      = Estimation_Tabs->findChild<QGroupBox    *>("Estimation_Tab5_CovariatesGB");
    Estimation_Tab5_AbsoluteBiomassTV = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_AbsoluteBiomassTV");
    Estimation_Tab5_RelativeBiomassTV = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_RelativeBiomassTV");
//  Estimation_Tab5_Rel2AbsScalarTV   = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_Rel2AbsScalarTV");
    Estimation_Tab5_CovariatesTV      = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_CovariatesTV");
    Estimation_Tab5_PrevPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_PrevPB");
    Estimation_Tab5_NextPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_NextPB");
    Estimation_Tab5_LoadPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_LoadPB");
    Estimation_Tab5_SavePB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_SavePB");
    Estimation_Tab5_ImportPB          = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_ImportPB");
    Estimation_Tab5_ExportPB          = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_ExportPB");
    Estimation_Tab5_ObsBiomassTypeLBL = Estimation_Tabs->findChild<QLabel       *>("Estimation_Tab5_ObsBiomassTypeLBL");

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
    connect(Estimation_Tab5_ExportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ExportPB()));

    Estimation_Tab5_PrevPB->setText("\u25C1--");
    Estimation_Tab5_NextPB->setText("--\u25B7");

    Estimation_Tab5_CovariatesGB->setVisible(false);
//    Estimation_Tab5_Rel2AbsScalarTV->hide();
    Estimation_Tab5_RelativeBiomassTV->hide();

} // end constructor


nmfEstimation_Tab5::~nmfEstimation_Tab5()
{

}

void
nmfEstimation_Tab5::clearWidgets()
{
    nmfUtilsQt::clearTableView(
        {Estimation_Tab5_AbsoluteBiomassTV,
         Estimation_Tab5_CovariatesTV});
}

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
nmfEstimation_Tab5::callback_LoadWidgets()
{
    loadWidgets();
}

void
nmfEstimation_Tab5::callback_LoadPB()
{
    if (loadWidgets()) {
        QMessageBox::information(Estimation_Tabs, "Biomass Load",
                                 "\nBiomass table(s) successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab5::callback_ImportPB()
{
    if (isAbsoluteBiomassChecked()) {
        importAbsoluteBiomass();
    } else {
        importRelativeBiomass();
    }
}

void
nmfEstimation_Tab5::importAbsoluteBiomass()
{
    importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"Absolute Biomass",
                    nmfConstantsMSSPM::TableBiomassAbsolute,Estimation_Tab5_AbsoluteBiomassTV);
}

void
nmfEstimation_Tab5::importRelativeBiomass()
{
    importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"Relative Biomass",
                    nmfConstantsMSSPM::TableBiomassRelative,Estimation_Tab5_RelativeBiomassTV);
}

//void
//nmfEstimation_Tab5::importScalarValues()
//{
//    importTableData(nmfConstantsMSSPM::FirstLineNotReadOnly,"Relative Biomass Scalars",
//                    nmfConstantsMSSPM::TableBiomassRelativeScalars,Estimation_Tab5_Rel2AbsScalarTV);
//}

void
nmfEstimation_Tab5::importTableData(const bool& firstLineReadOnly,
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
                    QObject::tr("Data Files (biomass*.csv)"));
        QFileInfo fi(filename);
        QString filenameNoPath = fi.baseName();
        loadCSVFile(firstLineReadOnly,filenameNoPath.toStdString(),tableView);
    }
}

void
nmfEstimation_Tab5::loadCSVFile(const bool& firstLineReadOnly,
                                const std::string& tableName,
                                QTableView* tableView)
{
    bool loadOK;
    QString errorMsg;
    QString tableNameStr;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::pair<int,int> nonZeroCell;

    tableNameStr = QString::fromStdString(tableName);
    tableNameStr = QDir(inputDataPath).filePath(tableNameStr+".csv");

    loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, tableView, inputDataPath, tableNameStr,
                firstLineReadOnly,
                nmfConstantsMSSPM::FixedNotation,
                nonZeroCell,errorMsg);

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }
}

bool
nmfEstimation_Tab5::isAbsoluteBiomassChecked()
{
    return (! m_DatabasePtr->isARelativeBiomassModel(m_ProjectName,m_ModelName));
}

void
nmfEstimation_Tab5::setObsBiomassType(QString obsBiomassType)
{
    Estimation_Tab5_ObsBiomassTypeLBL->setText(obsBiomassType+":");
}

void
nmfEstimation_Tab5::callback_SavePB()
{
    bool okSave;
    bool shownAlready=false;
    std::vector<std::string> modelsInProject = {};

    if (! m_DatabasePtr->updateAllModelsInProject(Estimation_Tabs,"ObservedBiomass",m_ProjectName,
                                                  m_ModelName,modelsInProject)) {
        return;
    }

    std::string m_ModelNameRem = m_ModelName;
    for (std::string projectModel : modelsInProject)
    {
        m_ModelName = projectModel;
        if (isAbsoluteBiomassChecked()) {
            okSave = saveAbsoluteBiomass();
            if (okSave && ! shownAlready) {
                QMessageBox::information(Estimation_Tabs, "Observed Biomass Updated",
                                         "\nObserved Absolute Biomass table has been successfully updated.\n",
                                         QMessageBox::Ok);
                shownAlready = true;
            }
        } else {
            okSave = saveRelativeBiomass();
            if (okSave && ! shownAlready) {
                QMessageBox::information(Estimation_Tabs, "Observed Biomass Updated",
                                         "\nObserved Relative Biomass table has been successfully updated.\n",
                                         QMessageBox::Ok);
                shownAlready = true;
            }
        }
    }
    m_ModelName = m_ModelNameRem;

}

void
nmfEstimation_Tab5::callback_ExportPB()
{
    if (isAbsoluteBiomassChecked()) {
        saveTableValuesToCSVFile("Absolute Biomass",nmfConstantsMSSPM::TableBiomassAbsolute,m_SModelAbsoluteBiomass);
    } else {
//      saveTableValuesToCSVFile("Relative Biomass Scalars",nmfConstantsMSSPM::TableBiomassRelativeScalars,m_SModelScalars);
        saveTableValuesToCSVFile("Relative Biomass",nmfConstantsMSSPM::TableBiomassRelative,m_SModelRelativeBiomass);
//      matchTableColumnWidths();
    }
}

//void
//nmfEstimation_Tab5::matchTableColumnWidths()
//{
//    int relativeColWidth;
//    int scalarColWidth;
//    int NumSpecies = m_SModelRelativeBiomass->columnCount();

//    Estimation_Tab5_RelativeBiomassTV->verticalHeader()->setFixedWidth(
//        Estimation_Tab5_Rel2AbsScalarTV->verticalHeader()->width());

//    Estimation_Tab5_RelativeBiomassTV->resizeColumnsToContents();
//    Estimation_Tab5_Rel2AbsScalarTV->resizeColumnsToContents();

//    for (int col=0; col<NumSpecies; ++col) {
//        relativeColWidth = Estimation_Tab5_RelativeBiomassTV->columnWidth(col);
//        scalarColWidth   = Estimation_Tab5_Rel2AbsScalarTV->columnWidth(col);
//        if (relativeColWidth > scalarColWidth) {
//            Estimation_Tab5_Rel2AbsScalarTV->setColumnWidth(col,relativeColWidth);
//        } else {
//            Estimation_Tab5_RelativeBiomassTV->setColumnWidth(col,scalarColWidth);
//        }
//    }
//}

bool
nmfEstimation_Tab5::saveAbsoluteBiomass()
{
    int NumSpecies;
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> SpeNames;
    std::vector<double> SpeciesKMin;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString value;
    QString msg;
    QString valueWithoutComma;

    if ((m_SModelAbsoluteBiomass == NULL) || (m_SModelCovariates == NULL)) {
        return false;
    }
    SpeciesKMin.clear();

    // Get SpeciesKMin values for all Species
    fields     = {"SpeName","SpeciesKMin"};
    queryStr   = "SELECT SpeName,SpeciesKMin from " + nmfConstantsMSSPM::TableSpecies + " ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int species=0; species<NumSpecies; ++species) {
        valueWithoutComma = QString::fromStdString(dataMap["SpeciesKMin"][species]).remove(",");
        SpeciesKMin.push_back(valueWithoutComma.toDouble());
        SpeNames.push_back(dataMap["SpeName"][species]);
    }

    // Save Observed Biomass data to Database table
    saveTableValuesToDatabase(nmfConstantsMSSPM::TableBiomassAbsolute,m_SModelAbsoluteBiomass);

/*
    // Put when in when implement covariates
    cmd = "DELETE FROM " + nmfConstantsMSSPM::TableCovariate;
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 4] nmfEstimation_Tab5::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from Covariate table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return false;
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
        return false;
    }

    Estimation_Tab5_CovariatesTV->resizeColumnsToContents();
*/
    QMessageBox::information(Estimation_Tabs, "Absolute Data Updated",
                             "\nAbsolute data table(s) have been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);

    return true;

}

bool
nmfEstimation_Tab5::saveRelativeBiomass()
{
    // Save Relative Biomass data to Database table
    bool ok = saveTableValuesToDatabase(nmfConstantsMSSPM::TableBiomassRelative,m_SModelRelativeBiomass);
    return ok;
}

// RSK - when you delete this also delete the MySQL table and all references to the MySQL table
// i.e. the creation of the table in GuiSetup2
//void
//nmfEstimation_Tab5::saveScalarValues()
//{
//    // Save Relative Biomass Scalar data to database table
//    saveTableValuesToDatabase(nmfConstantsMSSPM::TableBiomassRelativeScalars,m_SModelScalars);
//}

bool
nmfEstimation_Tab5::saveTableValuesToDatabase(
        const std::string& tableName,
        QStandardItemModel* smodel)
{
    bool ok;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::vector<std::string> SpeNames;
    int NumSpecies;
    QString value;
    QString msg;
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;
    QString valueWithoutComma;

    // Save Relative Biomass data to database table
    if (smodel == NULL) {
        return false;
    }
    NumSpecies = smodel->columnCount();

    // Get list of Species names
    for (int species=0; species<NumSpecies; ++species) {
        SpeNames.push_back(smodel->horizontalHeaderItem(species)->text().toStdString());
    }

//    // Check data integrity
//    for (int j=0; j<smodel->columnCount(); ++j) { // Species
//        for (int i=0; i<smodel->rowCount(); ++i) { // Time
//            index = smodel->index(i,j);
//            value = index.data().toDouble(&ok);
//            if (! ok) {
//                //value = index.data().toString();
//                if (value.contains(',')) {
//                    msg = "Invalid value found (" + value +"). No commas or special characters allowed in a number.";
//                    m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
//                    QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
//                    return false;
//                }
//            }
//        }
//    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Delete existing data
    cmd = "DELETE FROM " + tableName + " WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '" + m_ModelName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab5::saveTableValuesToDatabase: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return false;
    }

    if (tableName == nmfConstantsMSSPM::TableBiomassRelativeScalars) {
        std::string type1 = "Scalar";
        cmd = "INSERT INTO " + tableName + " (ProjectName,ModelName,SpeName,Type,Value) VALUES ";
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<smodel->rowCount(); ++time) {
                index = smodel->index(time,species);
                valueWithoutComma = index.data().toString().remove(",");
                cmd += "('"   + m_ProjectName +
                        "','" + m_ModelName +
                        "','" + SpeNames[species] +
                        "','" + type1 +
                        "', " + QString::number(valueWithoutComma.toDouble(),'f',6).toStdString() + "),";
//                      "', " + QString::number(index.data().toDouble(),'f',6).toStdString() + "),";
            }
        }
    } else {
        cmd = "INSERT INTO " + tableName + " (ProjectName,ModelName,SpeName,Year,Value) VALUES ";
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<smodel->rowCount(); ++time) {
                index = smodel->index(time,species);
                valueWithoutComma = index.data().toString().remove(",");
                cmd += "('"   + m_ProjectName +
                        "','" + m_ModelName +
                        "','" + SpeNames[species] +
                        "',"  + std::to_string(time) +
                        ", "  + QString::number(valueWithoutComma.toDouble(),'f',6).toStdString() + "),";
//                      ", "  + QString::number(index.data().toDouble(),'f',6).toStdString() + "),";
            }
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfEstimation_Tab5::saveTableValuesToDatabase: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return false;
    }

    if (tableName == nmfConstantsMSSPM::TableBiomassAbsolute) {
        // Need to also update the Species table with the initial Biomass values
        for (int species=0; species<NumSpecies; ++species) {
            index = smodel->index(0,species);
            valueWithoutComma = index.data().toString().remove(",");
            cmd  = "UPDATE " + nmfConstantsMSSPM::TableSpecies + " SET InitBiomass = " + QString::number(valueWithoutComma.toDouble(),'f',6).toStdString();
            cmd += " WHERE SpeName = '" + SpeNames[species] + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab5::saveTableValuesToDatabase: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs,"Warning",
                                     "\nCouldn't REPLACE INTO Species table.\n",
                                     QMessageBox::Ok);
                return false;
            }
        }
        emit ReloadSpecies(nmfConstantsMSSPM::ShowPopupError);
    }

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
    return true;
}

void
nmfEstimation_Tab5::saveTableValuesToCSVFile(const QString& type,
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
nmfEstimation_Tab5::saveCSVFile(
        const QString& type,
        QStandardItemModel* smodel,
        const std::string& tableName)
{
    bool okSave;
    QString tableNameWithPath;

    // Save time series data to a .csv file
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    tableNameWithPath = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    okSave = nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameWithPath);

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

void
nmfEstimation_Tab5::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}


bool
nmfEstimation_Tab5::loadWidgets()
{
    int NumSpecies;
    int RunLength;
    int StartYear;
    std::vector<std::string> SpeciesNames;
    QStringList VerticalList;
    QStringList SpeciesList;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab5::loadWidgets()");

    readSettings();

    QString ModelName = QString::fromStdString(m_ModelName);
    if (ModelName.isEmpty())
        return false;

    clearWidgets();

    if (! m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear)) {
        return false;
    }
    if (! m_DatabasePtr->getAllSpecies(m_Logger,SpeciesNames)) {
        return false;
    }
    NumSpecies = SpeciesNames.size();
    nmfUtilsQt::convertVectorToStrList(SpeciesNames,SpeciesList);

    m_SModelAbsoluteBiomass = new QStandardItemModel( RunLength, NumSpecies );
    m_SModelRelativeBiomass = new QStandardItemModel( RunLength, NumSpecies );
    m_SModelCovariates      = new QStandardItemModel( RunLength, 1 );

    // Load tableviews
    loadAbsoluteBiomass(RunLength,StartYear,NumSpecies,ModelName,
                        SpeciesNames,SpeciesList,VerticalList);
    loadRelativeBiomass(RunLength,StartYear,NumSpecies,ModelName,
                        SpeciesNames,SpeciesList,VerticalList);
    loadCovariates(RunLength,VerticalList);

    Estimation_Tab5_RelativeBiomassTV->resizeColumnsToContents();
    return true;
}

void
nmfEstimation_Tab5::loadAbsoluteBiomass(const int& RunLength,
                                        const int& StartYear,
                                        const int& NumSpecies,
                                        const QString& ModelName,
                                        const std::vector<std::string>& SpeciesNames,
                                        const QStringList& SpeciesList,
                                        QStringList& VerticalList)
{
    loadTableValuesFromDatabase(RunLength,StartYear,NumSpecies,ModelName,SpeciesNames,
                                SpeciesList,VerticalList,nmfConstantsMSSPM::TableBiomassAbsolute,
                                m_SModelAbsoluteBiomass,Estimation_Tab5_AbsoluteBiomassTV);
}

void
nmfEstimation_Tab5::loadRelativeBiomass(const int& RunLength,
                                        const int& StartYear,
                                        const int& NumSpecies,
                                        const QString& ModelName,
                                        const std::vector<std::string>& SpeciesNames,
                                        const QStringList& SpeciesList,
                                        QStringList& VerticalList)
{
    VerticalList.clear();
    loadTableValuesFromDatabase(RunLength,StartYear,NumSpecies,ModelName,SpeciesNames,
                                SpeciesList,VerticalList,nmfConstantsMSSPM::TableBiomassRelative,
                                m_SModelRelativeBiomass,Estimation_Tab5_RelativeBiomassTV);
}


void
nmfEstimation_Tab5::loadTableValuesFromDatabase(
        const int& RunLength,
        const int& StartYear,
        const int& NumSpecies,
        const QString& ModelName,
        const std::vector<std::string>& SpeciesNames,
        const QStringList& SpeciesList,
        QStringList& VerticalList,
        const std::string& tableName,
        QStandardItemModel* smodel,
        QTableView* tableView)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;

    // Get data from database table
    fields     = {"ProjectName","ModelName","SpeName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,SpeName,Year,Value FROM " + tableName +
                 " WHERE ProjectName = '" + m_ProjectName + "' AND ModelName = '" +
                 ModelName.toStdString() + "' ORDER BY SpeName,Year ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    Qt::ItemFlags flags;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    // Load BiomassTV model and set the tableview
    if (NumRecords > 0) {
        int m = 0;
        for (int i=0; i<=RunLength; ++i) {
            VerticalList << " " + QString::number(StartYear+i) + " ";
        }
        for (int j=0; j<NumSpecies; ++j) {
            for (int i=0; i<=RunLength; ++i) {
                if ((m < NumRecords) && (SpeciesNames[j] == dataMap["SpeName"][m])) {
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                std::stod(dataMap["Value"][m++]),m_NumSignificantDigits,6);
                    item = new QStandardItem(valueWithComma);
                } else {
                    item = new QStandardItem(QString(""));
                }
                item->setTextAlignment(Qt::AlignCenter);
                // Make first row read-only
                item->setEditable(i != 0);
                if (i == 0) {
                    if (m_IsDark){
                        item->setBackground(QBrush(QColor(103,110,113)));
                    } else {
                        item->setBackground(QBrush(QColor(211,211,211)));
                    }
//                  item->setBackground(QBrush(QColor(240,240,240)));
                    flags = item->flags();
                    flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEditable); // reset/clear the flag
                    item->setFlags(flags);
                }
                smodel->setItem(i, j, item);
            }
        }

        smodel->setVerticalHeaderLabels(VerticalList);
        smodel->setHorizontalHeaderLabels(SpeciesList);
        tableView->setModel(smodel);
        tableView->resizeColumnsToContents();
    } else {
        // No database table yet...just fill with blanks for placeholders
        for (int j=0; j<NumSpecies; ++j) {
            for (int i=0; i<=RunLength; ++i) {
                item = new QStandardItem(QString(""));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
            }
        }
        smodel->setVerticalHeaderLabels(VerticalList);
        smodel->setHorizontalHeaderLabels(SpeciesList);
        tableView->setModel(smodel);
        tableView->resizeColumnsToContents();
        updateInitialObservedBiomass(tableName,tableView);
    }
}

void
nmfEstimation_Tab5::callback_UpdateInitialObservedBiomass(QString obsBiomassType)
{
    if (obsBiomassType == "Absolute") {
        updateInitialObservedBiomass(nmfConstantsMSSPM::TableBiomassAbsolute,Estimation_Tab5_AbsoluteBiomassTV);
    } else {
        updateInitialObservedBiomass(nmfConstantsMSSPM::TableBiomassAbsolute,Estimation_Tab5_RelativeBiomassTV);
    }
}

void
nmfEstimation_Tab5::updateInitialObservedBiomass(const std::string& tableName,
                                                 QTableView* tableView)
{
    int NumSpecies;
    int RunLength;
    int StartYear;
    double initAbsBiomass;
    double initSurveyQ;
    double initObsBiomassValue;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList SpeciesNames;
    QStringList VerticalList;
    QStandardItem *item;

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

    // Populate first row of Observed Absolute Biomass with Init Absolute Biomass from nmfConstantsMSSPM::TableSpecies.
    // Populate first row of Observed Relative Biomass with product of Init SurveyQ and Init Absolute Biomass from nmfConstantsMSSPM::TableSpecies.
    fields     = {"SpeName","InitBiomass","SurveyQ"};
    queryStr   = "SELECT SpeName,InitBiomass,SurveyQ FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

//  QStandardItemModel* smodel = new QStandardItemModel( RunLength, NumSpecies );
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());

    for (int i=0; i<=RunLength; ++i) {
        VerticalList << " " + QString::number(StartYear+i) + " ";
    }
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
        initAbsBiomass = std::stod(dataMap["InitBiomass"][j]);
        initSurveyQ    = std::stod(dataMap["SurveyQ"][j]);
        initObsBiomassValue = (tableName == nmfConstantsMSSPM::TableBiomassAbsolute) ? initAbsBiomass : initSurveyQ*initAbsBiomass;
        item = new QStandardItem(QString::number(initObsBiomassValue));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(0, j, item);
        for (int k=1; k<RunLength; ++k) {
            item = new QStandardItem("");
            smodel->setItem(k, j, item);
        }
    }

    smodel->setVerticalHeaderLabels(VerticalList);
    smodel->setHorizontalHeaderLabels(SpeciesNames);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void
nmfEstimation_Tab5::loadCovariates(const int& RunLength,
                                   const QStringList& VerticalList)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;
    QStringList CovariatesHeaderList;

    // Get data from database table
    fields     = {"Year","Value"};
    queryStr   = "SELECT Year,Value FROM " + nmfConstantsMSSPM::TableCovariate;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Year"].size();

    // Update model with database data
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
}

void
nmfEstimation_Tab5::callback_ObservedBiomassType(QString obsBiomassType)
{
    if (obsBiomassType == "Absolute") {
        callback_AbsoluteBiomassRB();
    } else if (obsBiomassType == "Relative") {
        callback_RelativeBiomassRB();
    }
    setObsBiomassType(obsBiomassType+" Biomass");
}

void
nmfEstimation_Tab5::callback_AbsoluteBiomassRB()
{
    Estimation_Tab5_AbsoluteBiomassTV->show();
    Estimation_Tab5_RelativeBiomassTV->hide();
}

void
nmfEstimation_Tab5::callback_RelativeBiomassRB()
{
    Estimation_Tab5_AbsoluteBiomassTV->hide();
    Estimation_Tab5_RelativeBiomassTV->show();
}

bool
nmfEstimation_Tab5::isTableValueOK(QString value)
{
    bool retv = true;
    if (value.isEmpty()) {
        QMessageBox::information(Estimation_Tabs, "Error: Missing Data",
                                 "\nPlease make sure all table cells are populated.\n",
                                 QMessageBox::Ok);
        retv = false;
    }
    return retv;
}

bool
nmfEstimation_Tab5::areTablesOK()
{
    return nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab5_AbsoluteBiomassTV,
                nmfConstantsMSSPM::DontShowError);
}

void
nmfEstimation_Tab5::callback_Rel2AbsScalarTVScrolled(int value)
{
    Estimation_Tab5_RelativeBiomassTV->blockSignals(true);
    Estimation_Tab5_RelativeBiomassTV->horizontalScrollBar()->setValue(value);
    Estimation_Tab5_RelativeBiomassTV->blockSignals(false);
}

void
nmfEstimation_Tab5::setIsDark(QString style){

    m_IsDark = (style=="Dark");
    loadWidgets();

}
