
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
    m_ProjectSettingsConfig.clear();
    m_SModelAbsoluteBiomass = nullptr;
    m_SModelRelativeBiomass = nullptr;
    m_SModelScalars         = nullptr;
    m_SModelCovariates      = nullptr;

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
    Estimation_Tab5_Rel2AbsScalarTV   = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_Rel2AbsScalarTV");
    Estimation_Tab5_CovariatesTV      = Estimation_Tabs->findChild<QTableView   *>("Estimation_Tab5_CovariatesTV");
    Estimation_Tab5_PrevPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_PrevPB");
    Estimation_Tab5_NextPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_NextPB");
    Estimation_Tab5_LoadPB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_LoadPB");
    Estimation_Tab5_SavePB            = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_SavePB");
    Estimation_Tab5_ImportPB          = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_ImportPB");
    Estimation_Tab5_ExportPB          = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_ExportPB");
    Estimation_Tab5_AbsoluteBiomassRB = Estimation_Tabs->findChild<QRadioButton *>("Estimation_Tab5_AbsoluteBiomassRB");
    Estimation_Tab5_RelativeBiomassRB = Estimation_Tabs->findChild<QRadioButton *>("Estimation_Tab5_RelativeBiomassRB");
    Estimation_Tab5_CalcBiomassPB     = Estimation_Tabs->findChild<QPushButton  *>("Estimation_Tab5_CalcBiomassPB");

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
    connect(Estimation_Tab5_CalcBiomassPB,     SIGNAL(clicked()),
            this,                              SLOT(callback_CalcBiomassPB()));
    connect(Estimation_Tab5_AbsoluteBiomassRB, SIGNAL(clicked()),
            this,                              SLOT(callback_AbsoluteBiomassRB()));
    connect(Estimation_Tab5_RelativeBiomassRB, SIGNAL(clicked()),
            this,                              SLOT(callback_RelativeBiomassRB()));
    connect(Estimation_Tab5_RelativeBiomassTV->horizontalScrollBar(), SIGNAL(sliderMoved(int)),
                                                                      this, SLOT(callback_RelativeBiomassTVScrolled(int)));
    connect(Estimation_Tab5_Rel2AbsScalarTV->horizontalScrollBar(),   SIGNAL(sliderMoved(int)),
            this,                                                     SLOT(callback_Rel2AbsScalarTVScrolled(int)));

    Estimation_Tab5_PrevPB->setText("\u25C1--");
    Estimation_Tab5_NextPB->setText("--\u25B7");

    Estimation_Tab5_CovariatesGB->setVisible(false);
    Estimation_Tab5_Rel2AbsScalarTV->hide();
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
        importScalarValues();
        importRelativeBiomass();
        matchTableColumnWidths();
    }
}

void
nmfEstimation_Tab5::importAbsoluteBiomass()
{
    importTableData(nmfConstantsMSSPM::FirstLineReadOnly,"Absolute Biomass",
                    "BiomassAbsolute",Estimation_Tab5_AbsoluteBiomassTV);
}

void
nmfEstimation_Tab5::importRelativeBiomass()
{
    importTableData(nmfConstantsMSSPM::FirstLineNotReadOnly,"Relative Biomass",
                    "BiomassRelative",Estimation_Tab5_RelativeBiomassTV);
}

void
nmfEstimation_Tab5::importScalarValues()
{
    importTableData(nmfConstantsMSSPM::FirstLineNotReadOnly,"Relative Biomass Scalars",
                    "BiomassRelativeScalars",Estimation_Tab5_Rel2AbsScalarTV);
}

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
                    QObject::tr("Data Files (*.csv)"));
        QFileInfo fi(filename);
        QString filenameNoPath = fi.baseName();
        loadCSVFile(firstLineReadOnly,filenameNoPath.toStdString(),tableView);
//        if (filename.contains("_")) {
//            QFileInfo fi(filename);
//            QString base = fi.baseName();
//            QStringList parts = base.split("_");
//            if (parts.size() == 2) {
//                QString tag = parts[1];
//                tableNameNew = tableName+"_"+tag.toStdString();
//                loadCSVFile(firstLineReadOnly,tableNameNew,tableView);
//            } else {
//                QMessageBox::information(Estimation_Tabs, type+" CSV Import",
//                                         "\nPlease make sure to select the "+type+" filename that contains the appropriate tag\n",
//                                         QMessageBox::Ok);
//            }
//        } else {
//            QMessageBox::critical(Estimation_Tabs, type+" CSV Import",
//                                  "\nError: No tag found in filename\n",
//                                  QMessageBox::Ok);
//        }
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

    tableNameStr = QString::fromStdString(tableName);
    tableNameStr = QDir(inputDataPath).filePath(tableNameStr+".csv");
std::cout << "tableNameStr: " << tableNameStr.toStdString() << std::endl;

    loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, tableView, inputDataPath, tableNameStr,
                firstLineReadOnly, errorMsg);

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }
}

bool
nmfEstimation_Tab5::isAbsoluteBiomassChecked()
{
    return Estimation_Tab5_AbsoluteBiomassRB->isChecked();
}

void
nmfEstimation_Tab5::callback_SavePB()
{
    if (isAbsoluteBiomassChecked()) {
        saveAbsoluteBiomass();
    } else {
        saveScalarValues();
        saveRelativeBiomass();
        matchTableColumnWidths();
    }
}

void
nmfEstimation_Tab5::callback_ExportPB()
{
    if (isAbsoluteBiomassChecked()) {
        saveTableValuesToCSVFile("Absolute Biomass","BiomassAbsolute",m_SModelAbsoluteBiomass);
    } else {
        saveTableValuesToCSVFile("Relative Biomass Scalars","BiomassRelativeScalars",m_SModelScalars);
        saveTableValuesToCSVFile("Relative Biomass","BiomassRelative",m_SModelRelativeBiomass);
        matchTableColumnWidths();
    }
}

void
nmfEstimation_Tab5::matchTableColumnWidths()
{
    int relativeColWidth;
    int scalarColWidth;
    int NumSpecies = m_SModelRelativeBiomass->columnCount();

    Estimation_Tab5_RelativeBiomassTV->verticalHeader()->setFixedWidth(
        Estimation_Tab5_Rel2AbsScalarTV->verticalHeader()->width());

    Estimation_Tab5_RelativeBiomassTV->resizeColumnsToContents();
    Estimation_Tab5_Rel2AbsScalarTV->resizeColumnsToContents();

    for (int col=0; col<NumSpecies; ++col) {
        relativeColWidth = Estimation_Tab5_RelativeBiomassTV->columnWidth(col);
        scalarColWidth   = Estimation_Tab5_Rel2AbsScalarTV->columnWidth(col);
        if (relativeColWidth > scalarColWidth) {
            Estimation_Tab5_Rel2AbsScalarTV->setColumnWidth(col,relativeColWidth);
        } else {
            Estimation_Tab5_RelativeBiomassTV->setColumnWidth(col,scalarColWidth);
        }
    }
}

void
nmfEstimation_Tab5::saveAbsoluteBiomass()
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

    if ((m_SModelAbsoluteBiomass == NULL)||(m_SModelCovariates == NULL)) {
        return;
    }
    SpeciesKMin.clear();

    // Re-load first row of biomass since the init biomass is not allowed to be changed
//    loadWidgetsFirstRow();

    // Get SpeciesKMin values for all Species
    fields     = {"SpeName","SpeciesKMin"};
    queryStr   = "SELECT SpeName,SpeciesKMin from Species ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int species=0; species<NumSpecies; ++species) {
        SpeciesKMin.push_back(std::stod(dataMap["SpeciesKMin"][species]));
        SpeNames.push_back(dataMap["SpeName"][species]);
    }

    // Check that InitBiomass < the SpeciesKMin value in the Species table
    for (int species=0; species<NumSpecies; ++species) {
        index = m_SModelAbsoluteBiomass->index(0,species);
        InitBiomass = index.data().toDouble();
        if (InitBiomass > SpeciesKMin[species]) {            
            errorMsg  = "\nFound: InitBiomass (" + std::to_string(InitBiomass) +
                    ") > SpeciesKMin (" + std::to_string(SpeciesKMin[species]) +
                    ") for Species: " + SpeNames[species];
            errorMsg += "\n\nInitBiomass must be less than SpeciesKMin.\n";
            QMessageBox::warning(Estimation_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
    }

    // Save Relative Biomass data to Database table
    saveTableValuesToDatabase("BiomassAbsolute",m_SModelAbsoluteBiomass);


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

    Estimation_Tab5_CovariatesTV->resizeColumnsToContents();

    QMessageBox::information(Estimation_Tabs, "Absolute Data Updated",
                             "\nAbsolute data table(s) have been successfully updated.\n",
                             QMessageBox::Ok);

    Estimation_Tabs->setCursor(Qt::ArrowCursor);

}

void
nmfEstimation_Tab5::saveRelativeBiomass()
{
    // Save Relative Biomass data to Database table
    saveTableValuesToDatabase("BiomassRelative",m_SModelRelativeBiomass);
}


void
nmfEstimation_Tab5::saveScalarValues()
{
    // Save Relative Biomass Scalar data to database table
    saveTableValuesToDatabase("BiomassRelativeScalars",m_SModelScalars);
}

void
nmfEstimation_Tab5::saveTableValuesToDatabase(
        const std::string& tableName,
        QStandardItemModel* smodel)
{
    bool ok;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string MohnsRhoLabel = "";
    std::vector<std::string> SpeNames;
    int NumSpecies;
    QString value;
    QString msg;
    QModelIndex index;
    std::string cmd;
    std::string errorMsg;

    // Save Relative Biomass data to database table
    if (smodel == NULL) {
        return;
    }
    NumSpecies = smodel->columnCount();

    // Get list of Species names
    for (int species=0; species<NumSpecies; ++species) {
        SpeNames.push_back(smodel->horizontalHeaderItem(species)->text().toStdString());
    }

    // Check data integrity
    for (int j=0; j<smodel->columnCount(); ++j) { // Species
        for (int i=0; i<smodel->rowCount(); ++i) { // Time
            index = smodel->index(i,j);
            value = index.data().toDouble(&ok);
            if (! ok) {
                //value = index.data().toString();
                if (value.contains(',')) {
                    msg = "Invalid value found (" + value +"). No commas or special characters allowed in a number.";
                    m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
                    QMessageBox::warning(Estimation_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
                    return;
                }
            }
        }
    }

    Estimation_Tabs->setCursor(Qt::WaitCursor);

    // Delete existing data
    cmd = "DELETE FROM " + tableName + " WHERE SystemName = '" +
           m_ProjectSettingsConfig + "' AND MohnsRhoLabel = ''";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfEstimation_Tab5::saveTableValuesToDatabase: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError in Save command.  Couldn't delete all records from " + QString::fromStdString(tableName) + " table.\n",
                             QMessageBox::Ok);
        Estimation_Tabs->setCursor(Qt::ArrowCursor);
        return;
    }

    if (tableName == "BiomassRelativeScalars") {
        std::string type1 = "Scalar";
        cmd = "INSERT INTO " + tableName + " (MohnsRhoLabel,SystemName,SpeName,Type,Value) VALUES ";
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<smodel->rowCount(); ++time) {
                index = smodel->index(time,species);
//                value = index.data().toString();
                cmd += "('"   + MohnsRhoLabel +
                        "','" + m_ProjectSettingsConfig +
                        "','" + SpeNames[species] +
                        "','" + type1 +
                        "', " + QString::number(index.data().toDouble(),'f',6).toStdString() + "),";
            }
        }
    } else {
        cmd = "INSERT INTO " + tableName + " (MohnsRhoLabel,SystemName,SpeName,Year,Value) VALUES ";
        for (int species=0; species<NumSpecies; ++species) {
            for (int time=0; time<smodel->rowCount(); ++time) {
                index = smodel->index(time,species);
//                value = index.data().toString();
                cmd += "('"   + MohnsRhoLabel +
                        "','" + m_ProjectSettingsConfig +
                        "','" + SpeNames[species] +
                        "',"  + std::to_string(time) +
                        ", "  + QString::number(index.data().toDouble(),'f',6).toStdString() + "),";
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
        return;
    }

    if (tableName == "BiomassAbsolute") {
        // Need to also update the Species table with the initial Biomass values
        for (int species=0; species<NumSpecies; ++species) {
            index = smodel->index(0,species);
//          value = index.data().toString();
            cmd  = "UPDATE Species SET InitBiomass = " + QString::number(index.data().toDouble(),'f',6).toStdString();  // value.toStdString();
            cmd += " WHERE SpeName = '" + SpeNames[species] + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"[Error 3] nmfEstimation_Tab5::saveTableValuesToDatabase: Write table error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                QMessageBox::warning(Estimation_Tabs,"Warning",
                                     "\nCouldn't REPLACE INTO Species table.\n",
                                     QMessageBox::Ok);
                return;
            }
        }
        emit ReloadSpecies(nmfConstantsMSSPM::ShowPopupError);
    }

    Estimation_Tabs->setCursor(Qt::ArrowCursor);
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

    m_SModelAbsoluteBiomass = qobject_cast<QStandardItemModel*>(Estimation_Tab5_AbsoluteBiomassTV->model());

    fields     = {"SpeName","InitBiomass"};
    queryStr   = "SELECT SpeName,InitBiomass FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);

    for (unsigned j=0; j<dataMap["SpeName"].size(); ++j) {
        item = new QStandardItem(QString::number(std::stod(dataMap["InitBiomass"][j]),'f',6));
        item->setTextAlignment(Qt::AlignCenter);
//        item->setEditable(false); // Make first row read-only
//        item->setBackground(QBrush(QColor(240,240,240)));
        m_SModelAbsoluteBiomass->setItem(0, j, item);
    }

    //        Estimation_Tab5_BiomassTV->setModel(m_SModelBiomass);
    Estimation_Tab5_AbsoluteBiomassTV->resizeColumnsToContents();

    return true;
}

bool
nmfEstimation_Tab5::loadWidgets(QString MohnsRhoLabel)
{
    int NumSpecies;
    int RunLength;
    int StartYear;
    std::vector<std::string> SpeciesNames;
    QStringList VerticalList;
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

    m_SModelAbsoluteBiomass = new QStandardItemModel( RunLength, NumSpecies );
    m_SModelRelativeBiomass = new QStandardItemModel( RunLength, NumSpecies );
    m_SModelScalars         = new QStandardItemModel( 1,         NumSpecies );
    m_SModelCovariates      = new QStandardItemModel( RunLength, 1 );

    // Load tableviews
    loadAbsoluteBiomass(RunLength,StartYear,NumSpecies,SystemName,
                        MohnsRhoLabel,SpeciesNames,SpeciesList,VerticalList);
    loadRelativeBiomass(RunLength,StartYear,NumSpecies,SystemName,
                        MohnsRhoLabel,SpeciesNames,SpeciesList,VerticalList);
    loadCovariates(RunLength,VerticalList);
    loadScalars(1,StartYear,NumSpecies,SystemName,
                MohnsRhoLabel,SpeciesNames,SpeciesList,VerticalList);

    return true;
}

void
nmfEstimation_Tab5::loadScalars(const int& RunLength,
                                const int& StartYear,
                                const int& NumSpecies,
                                const QString& SystemName,
                                const QString& MohnsRhoLabel,
                                const std::vector<std::string>& SpeciesNames,
                                const QStringList& SpeciesList,
                                QStringList& VerticalList)
{
    QStringList VerticalList2;
    QStandardItem *item;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    int m=0;

    fields     = {"MohnsRhoLabel","SystemName","SpeName","Type","Value"};
    queryStr   = "SELECT MohnsRhoLabel,SystemName,SpeName,Type,Value FROM BiomassRelativeScalars WHERE SystemName = '" +
                 SystemName.toStdString() + "' AND MohnsRhoLabel = '" +
                 MohnsRhoLabel.toStdString() + "' ORDER BY SpeName ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();

    if (NumRecords > 0) {
        for (int row=0; row<RunLength; ++row) {
            VerticalList2 << QString::fromStdString(" "+dataMap["Type"][m]+" ");
            for (int col=0; col<NumSpecies; ++col) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
                item->setTextAlignment(Qt::AlignCenter);
                m_SModelScalars->setItem(0,col,item);
            }
        }
    } else {
        for (int col=0; col<NumSpecies; ++col) {
            item = new QStandardItem(QString(""));
            item->setTextAlignment(Qt::AlignCenter);
            m_SModelScalars->setItem(0,col,item);
        }
    }

    m_SModelScalars->setHorizontalHeaderLabels(SpeciesList);
    m_SModelScalars->setVerticalHeaderLabels(VerticalList2);
    Estimation_Tab5_Rel2AbsScalarTV->setModel(m_SModelScalars);
    Estimation_Tab5_Rel2AbsScalarTV->resizeColumnsToContents();

    Estimation_Tab5_RelativeBiomassTV->verticalHeader()->setFixedWidth(
        Estimation_Tab5_Rel2AbsScalarTV->verticalHeader()->width());
}

void
nmfEstimation_Tab5::loadAbsoluteBiomass(const int& RunLength,
                                        const int& StartYear,
                                        const int& NumSpecies,
                                        const QString& SystemName,
                                        const QString& MohnsRhoLabel,
                                        const std::vector<std::string>& SpeciesNames,
                                        const QStringList& SpeciesList,
                                        QStringList& VerticalList)
{
    loadTableValuesFromDatabase(RunLength,StartYear,NumSpecies,SystemName,MohnsRhoLabel,SpeciesNames,
                                SpeciesList,VerticalList,"BiomassAbsolute",m_SModelAbsoluteBiomass,
                                Estimation_Tab5_AbsoluteBiomassTV);
}


void
nmfEstimation_Tab5::loadRelativeBiomass(const int& RunLength,
                                        const int& StartYear,
                                        const int& NumSpecies,
                                        const QString& SystemName,
                                        const QString& MohnsRhoLabel,
                                        const std::vector<std::string>& SpeciesNames,
                                        const QStringList& SpeciesList,
                                        QStringList& VerticalList)
{
    VerticalList.clear();
    loadTableValuesFromDatabase(RunLength,StartYear,NumSpecies,SystemName,MohnsRhoLabel,SpeciesNames,
                                SpeciesList,VerticalList,"BiomassRelative",m_SModelRelativeBiomass,
                                Estimation_Tab5_RelativeBiomassTV);
}


void
nmfEstimation_Tab5::loadTableValuesFromDatabase(
        const int& RunLength,
        const int& StartYear,
        const int& NumSpecies,
        const QString& SystemName,
        const QString& MohnsRhoLabel,
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
    fields     = {"MohnsRhoLabel","SystemName","SpeName","Year","Value"};
    queryStr   = "SELECT MohnsRhoLabel,SystemName,SpeName,Year,Value FROM "+tableName+" WHERE SystemName = '" +
                 SystemName.toStdString() + "' AND MohnsRhoLabel = '" +
                 MohnsRhoLabel.toStdString() + "' ORDER BY SpeName,Year ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    Qt::ItemFlags flags;

    // Load BiomassTV model and set the tableview
    if (NumRecords > 0) {
        int m = 0;
        for (int i=0; i<=RunLength; ++i) {
            VerticalList << " " + QString::number(StartYear+i) + " ";
        }
        for (int j=0; j<NumSpecies; ++j) {
            for (int i=0; i<=RunLength; ++i) {
                if ((m < NumRecords) && (SpeciesNames[j] == dataMap["SpeName"][m])) {
                    item = new QStandardItem(QString::number(std::stod(dataMap["Value"][m++]),'f',6));
                } else {
                    item = new QStandardItem(QString(""));
                }
                item->setTextAlignment(Qt::AlignCenter);
                if (tableName == "BiomassAbsolute") {
                    item->setEditable(i != 0); // Make first row read-only
                    if (i == 0) {
                        item->setBackground(QBrush(QColor(240,240,240)));
                        flags = item->flags();
                        flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEditable); // reset/clear the flag
                        item->setFlags(flags);
                    }
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

        if (tableName == "BiomassAbsolute") {
            callback_UpdateInitialObservedBiomass();
        }
    }
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
    queryStr   = "SELECT Year,Value FROM Covariate";
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

    m_SModelAbsoluteBiomass = new QStandardItemModel( RunLength, NumSpecies );

    for (int i=0; i<=RunLength; ++i) {
        VerticalList << " " + QString::number(StartYear+i) + " ";
    }
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
        item = new QStandardItem(QString::fromStdString(dataMap["InitBiomass"][j]));
        item->setTextAlignment(Qt::AlignCenter);
        m_SModelAbsoluteBiomass->setItem(0, j, item);
        for (int k=1; k<RunLength; ++k) {
            item = new QStandardItem("");
            m_SModelAbsoluteBiomass->setItem(k, j, item);
        }
    }

    m_SModelAbsoluteBiomass->setVerticalHeaderLabels(VerticalList);
    m_SModelAbsoluteBiomass->setHorizontalHeaderLabels(SpeciesNames);
    Estimation_Tab5_AbsoluteBiomassTV->setModel(m_SModelAbsoluteBiomass);
    Estimation_Tab5_AbsoluteBiomassTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab5::callback_AbsoluteBiomassRB()
{
    Estimation_Tab5_AbsoluteBiomassTV->show();
    Estimation_Tab5_RelativeBiomassTV->hide();
    Estimation_Tab5_CalcBiomassPB->setEnabled(false);
    Estimation_Tab5_Rel2AbsScalarTV->hide();
}


void
nmfEstimation_Tab5::callback_RelativeBiomassRB()
{
    Estimation_Tab5_AbsoluteBiomassTV->hide();
    Estimation_Tab5_RelativeBiomassTV->show();
    Estimation_Tab5_Rel2AbsScalarTV->show();
    Estimation_Tab5_CalcBiomassPB->setEnabled(true);
}

void
nmfEstimation_Tab5::callback_CalcBiomassPB()
{
    // Calculate the Absolute Biomass from the Relative Biomass and the Scalar values
    // AbsBiomass = Scalar*RelBiomass
    int NumSpecies = m_SModelAbsoluteBiomass->columnCount();
    int NumYears   = m_SModelAbsoluteBiomass->rowCount();
    double scalar;
    double absBiomass;
    QStandardItem *item;

    for (int col=0; col<NumSpecies; ++col) {
        scalar = m_SModelScalars->item(0,col)->text().toDouble();
        for (int row=0; row<NumYears; ++row) {
            absBiomass = scalar*m_SModelRelativeBiomass->item(row,col)->text().toDouble();
            item = new QStandardItem(QString::number(absBiomass,'f',6));
            item->setTextAlignment(Qt::AlignCenter);
            m_SModelAbsoluteBiomass->setItem(row,col,item);
        }
    }

    QMessageBox::information(Estimation_Tabs, "Absolute Biomass Calculated",
                             "\nAbsolute Biomass has been calculated from the product of the Relative Biomass and their corresponding Scalar values.\n",
                             QMessageBox::Ok);}

bool
nmfEstimation_Tab5::areTablesOK()
{
    return nmfUtilsQt::allCellsArePopulated(
                Estimation_Tabs,
                Estimation_Tab5_AbsoluteBiomassTV,
                nmfConstantsMSSPM::DontShowError);
}


void
nmfEstimation_Tab5::callback_RelativeBiomassTVScrolled(int value)
{
    Estimation_Tab5_Rel2AbsScalarTV->blockSignals(true);
    Estimation_Tab5_Rel2AbsScalarTV->horizontalScrollBar()->setValue(value);
    Estimation_Tab5_Rel2AbsScalarTV->blockSignals(false);
}

void
nmfEstimation_Tab5::callback_Rel2AbsScalarTVScrolled(int value)
{
    Estimation_Tab5_RelativeBiomassTV->blockSignals(true);
    Estimation_Tab5_RelativeBiomassTV->horizontalScrollBar()->setValue(value);
    Estimation_Tab5_RelativeBiomassTV->blockSignals(false);
}

