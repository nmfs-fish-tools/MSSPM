
#include "nmfEstimationTab01.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfEstimation_Tab1::nmfEstimation_Tab1(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_GuildModel   = nullptr;
    m_SpeciesModel = nullptr;
    m_ProjectDir   = projectDir;
    m_StartPosSL   = 50;
    m_runFromModifySlider = false;
    m_OutputSpecies.clear();
    m_SpeciesGuild.clear();
    m_ModelName = "";
    m_ProjectName = "";
    m_NumSignificantDigits = -1;
    m_ColumnMap.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::nmfEstimation_Tab1");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab01.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab1_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab1_Widget, tr("1. Population Parameters"));

    Estimation_Tab1_PopulationTabW      = Estimation_Tabs->findChild<QTabWidget  *>("Estimation_Tab1_PopulationTabW");
    Estimation_Tab1_GuildPopulationTV   = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab1_GuildPopulationTV");
    Estimation_Tab1_SpeciesPopulationTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab1_SpeciesPopulationTV");
    Estimation_Tab1_NextPB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_NextPB");
    Estimation_Tab1_PopulationGB        = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab1_PopulationGB");
    Estimation_Tab1_LoadPB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_LoadPB");
    Estimation_Tab1_ImportPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ImportPB");
    Estimation_Tab1_ExportPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ExportPB");
    Estimation_Tab1_RestorePB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_RestorePB");
    Estimation_Tab1_SavePB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_SavePB");
    Estimation_Tab1_ModifySL            = Estimation_Tabs->findChild<QSlider     *>("Estimation_Tab1_ModifySL");
    Estimation_Tab1_ModifySLGuilds      = Estimation_Tabs->findChild<QSlider     *>("Estimation_Tab1_ModifySLGuilds");
    Estimation_Tab1_ModifyRunPB         = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ModifyRunPB");
    Estimation_Tab1_ModifyPBGuilds      = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ModifyPBGuilds");
    Estimation_Tab1_ModifyRunCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_ModifyRunCB");
    Estimation_Tab1_SpeciesSuppCB       = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_SpeciesSuppCB");
    Estimation_Tab1_GuildSuppCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_GuildSuppCB");
    Estimation_Tab1_SpeciesRangeCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_SpeciesRangeCB");
    Estimation_Tab1_SpeciesRangeCMB     = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab1_SpeciesRangeCMB");
    Estimation_Tab1_SpeciesRangeSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab1_SpeciesRangeSB");
    Estimation_Tab1_GuildRangeCB        = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_GuildRangeCB");
    Estimation_Tab1_MinMaxCMB           = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab1_MinMaxCMB");
    Estimation_Tab1_ModifySL->setValue(m_StartPosSL); // Set midpoint position to start
    // This button not currently necessary
    Estimation_Tab1_RestorePB->hide();

    //nmfTableView* Estimation_Tab1_PopulationTV = new nmfTableView();
    QFont noBoldFont;
    noBoldFont.setBold(false);
    Estimation_Tab1_SpeciesPopulationTV->setFont(noBoldFont);

    // Hide these for now. Maybe add them later.
    Estimation_Tab1_ModifySL->hide();
    Estimation_Tab1_ModifyRunPB->hide();
    Estimation_Tab1_ModifyRunCB->hide();
    Estimation_Tab1_ModifySLGuilds->hide();
    Estimation_Tab1_ModifyPBGuilds->hide();

    connect(Estimation_Tab1_PopulationTabW, SIGNAL(currentChanged(int)),
            this,                           SLOT(callback_CurrentTabChanged(int)));
    connect(Estimation_Tab1_NextPB,         SIGNAL(clicked()),
            this,                           SLOT(callback_NextPB()));
    connect(Estimation_Tab1_LoadPB,         SIGNAL(clicked()),
            this,                           SLOT(callback_LoadPB()));
    connect(Estimation_Tab1_ImportPB,       SIGNAL(clicked()),
            this,                           SLOT(callback_ImportPB()));
    connect(Estimation_Tab1_ExportPB,       SIGNAL(clicked()),
            this,                           SLOT(callback_ExportPB()));
    connect(Estimation_Tab1_RestorePB,      SIGNAL(clicked()),
            this,                           SLOT(callback_RestorePB()));
    connect(Estimation_Tab1_SavePB,         SIGNAL(clicked()),
            this,                           SLOT(callback_SavePB()));
    connect(Estimation_Tab1_ModifySL,       SIGNAL(sliderMoved(int)),
            this,                           SLOT(callback_ModifyMovingSL(int)));
    connect(Estimation_Tab1_ModifySL,       SIGNAL(sliderReleased()),
            this,                           SLOT(callback_ModifyReleasedSL()));
    connect(Estimation_Tab1_GuildRangeCB,   SIGNAL(clicked(bool)),
            this,                           SLOT(callback_GuildRangeCB(bool)));
    connect(Estimation_Tab1_GuildSuppCB,    SIGNAL(clicked(bool)),
            this,                           SLOT(callback_GuildSuppCB(bool)));
    connect(Estimation_Tab1_SpeciesRangeCB, SIGNAL(clicked(bool)),
            this,                           SLOT(callback_SpeciesRangeCB(bool)));
    connect(Estimation_Tab1_SpeciesSuppCB,  SIGNAL(clicked(bool)),
            this,                           SLOT(callback_SpeciesSuppCB(bool)));
    connect(Estimation_Tab1_SpeciesRangeCMB, SIGNAL(activated(QString)),
            this,                            SLOT(callback_SpeciesRangeCMB(QString)));
    connect(Estimation_Tab1_SpeciesRangeSB,  SIGNAL(valueChanged(int)),
            this,                            SLOT(callback_SpeciesRangeSB(int)));


// Program takes too much time to run the diagnostics to do the following in real-time
//    connect(Estimation_Tab1_ModifySL,  SIGNAL(valueChanged(int)),
//            this,                      SLOT(callback_ModifySL(int)));
    connect(Estimation_Tab1_ModifyRunPB, SIGNAL(clicked()),
            this,                        SLOT(callback_ModifyRunPB()));

    Estimation_Tab1_NextPB->setText("--\u25B7");

    Estimation_Tab1_SpeciesRangeCMB->setEnabled(false);
    Estimation_Tab1_SpeciesRangeSB->setEnabled(false);
    Estimation_Tab1_MinMaxCMB->setEnabled(false);

    readSettings();

} // end constructor


nmfEstimation_Tab1::~nmfEstimation_Tab1()
{

}

bool
nmfEstimation_Tab1::checkAndShowEmptyFieldError(bool showPopup,
                                                const std::string& location)
{
    m_Logger->logMsg(nmfConstants::Error,"Found empty field.");
    if (showPopup) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError: Found empty field in " + QString::fromStdString(location) + ". Please fill in all required fields for the model desired.\n",
                             QMessageBox::Ok);
        return false;
    }
    return true;
}

bool
nmfEstimation_Tab1::checkAndShowOutOfRangeError(QString type,
                                                QString name,
                                                QString badParameter,
                                                bool showPopup)
{
    QString msg = "Found out of range field for " + type +
                  ": " + name + ", Field: " + badParameter ;
    m_Logger->logMsg(nmfConstants::Error,msg.toStdString());
    if (showPopup) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\n" + msg + "\n",
                             QMessageBox::Ok);
        return false;
    }
    return true;
}


// RSK this method should work, but it doesn't
void
nmfEstimation_Tab1::callback_CurrentTabChanged(int tab)
{
    bool isSpeciesTab = (tab == 1);

//  Estimation_Tab1_ModifySL->setDisabled(! isSpeciesTab);
//  Estimation_Tab1_ModifyRunPB->setDisabled(! isSpeciesTab);
//    Estimation_Tab1_ModifySL->setEnabled(isSpeciesTab);
//    Estimation_Tab1_ModifyRunPB->setEnabled(isSpeciesTab);
}

void
nmfEstimation_Tab1::callback_ModifyRunPB()
{
    callback_ModifyReleasedSL();
}

void
nmfEstimation_Tab1::callback_ModifyMovingSL(int sliderValue)
{
    int i=0;
    double tableValue;
    double origValue;
    QStandardItem *item;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab1_SpeciesPopulationTV->model());
    QLocale locale(QLocale::English);
    QString valueWithComma;

    sliderValue -= m_StartPosSL;

    // Modify selected data
    foreach (const QModelIndex &index, m_selIndexes) {
        origValue  = m_originalValuesSelected[i++];
        tableValue = origValue + origValue*(sliderValue/50.0);
        valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                    tableValue,m_NumSignificantDigits,2);
        item = new QStandardItem(valueWithComma);
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(index.row(),index.column(),item);

        // Reselect item
        Estimation_Tab1_SpeciesPopulationTV->selectionModel()->blockSignals(true);
        Estimation_Tab1_SpeciesPopulationTV->selectionModel()->select(index,QItemSelectionModel::Select);
        Estimation_Tab1_SpeciesPopulationTV->selectionModel()->blockSignals(false);
    }
}

void
nmfEstimation_Tab1::callback_ModifyReleasedSL()
{
    bool ok = true;
    bool runEstimationAndDiagnostic = Estimation_Tab1_ModifyRunCB->isChecked();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    emit StoreOutputSpecies();

    m_runFromModifySlider = true;

    // Reset slider to middle position
    resetModifySlider();

    // Save population parameters
    if (onGuildTab()) {
        ok = savePopulationParametersGuilds(nmfConstantsMSSPM::DontShowPopupError);
    } else { // if on Species tab
        ok = savePopulationParametersSpecies(nmfConstantsMSSPM::DontShowPopupError);
    }
    if (ok) {
        if (runEstimationAndDiagnostic) {
            emit RunEstimation(nmfConstantsMSSPM::ShowDiagnosticsChart);
        } else {
            emit CheckAllEstimationTablesAndRun();
        }
    }

    QApplication::restoreOverrideCursor();

}

void
nmfEstimation_Tab1::checkIfRunFromModifySlider()
{
    bool runEstimationAndDiagnostic = Estimation_Tab1_ModifyRunCB->isChecked();

    if (m_runFromModifySlider) {

        if (runEstimationAndDiagnostic) {
            // Re-run diagnostics but only after the Estimation has finished running
            emit RunDiagnostics();

            // Make sure diagnostics chart is visible
            emit ShowDiagnostics();
        }

        // Re-get the selected indexes
        getSelectedIndexes();

        emit RestoreOutputSpecies();

        m_runFromModifySlider = false;
    }

    QApplication::restoreOverrideCursor();
}

void
nmfEstimation_Tab1::callback_SelectionChangedTV(const QItemSelection& selection,
                                                const QItemSelection& deselection)
{
    QString valueWithoutComma;

    // Store the selected indexes in class variable
    getSelectedIndexes();

    // Save the values of the selected indexes in case user wants to undo mods from last save
    m_originalValuesSelected.clear();
    foreach (const QModelIndex &index, m_selIndexes){
        valueWithoutComma = index.data(Qt::DisplayRole).toString().remove(",");
        m_originalValuesSelected.push_back(valueWithoutComma.toDouble());
//      m_originalValuesSelected.push_back(index.data(Qt::DisplayRole).toDouble());
    }

}

void
nmfEstimation_Tab1::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

void
nmfEstimation_Tab1::callback_LoadPB()
{
    callback_LoadPBNoEmit();
    emit LoadSetup();
}

void
nmfEstimation_Tab1::callback_LoadPBNoEmit()
{
    loadWidgets();
}

void
nmfEstimation_Tab1::callback_RestorePB()
{
    int m = 0;
    std::string msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab1_SpeciesPopulationTV->model());
    QStandardItem *item;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    int numRows = smodel->rowCount();
    int numCols = smodel->columnCount();
    if (int(m_originalSpeciesValuesAll.size()) != numRows*numCols) {
        msg = "Number of values saved after initial load (" + std::to_string(m_originalSpeciesValuesAll.size()) +
              ") does not equal current numRows*numCols (" + std::to_string(numRows) +
              "*" + std::to_string(numCols);
        m_Logger->logMsg(nmfConstants::Error,msg);
        return;
    }
    for (int i=0; i<numRows; ++i) {
        for (int j=0; j<numCols; ++j) {
            if (j > 0) {
//              valueWithComma = locale.toString(m_originalSpeciesValuesAll[m++].toDouble());
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            m_originalSpeciesValuesAll[m++].toDouble(),m_NumSignificantDigits,6);
                item = new QStandardItem(valueWithComma);
            } else {
                item = new QStandardItem(m_originalSpeciesValuesAll[m++]);
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i,j,item);
        }
    }
    Estimation_Tab1_SpeciesPopulationTV->resizeColumnsToContents();

    resetModifySlider();
    resetVisibleColumns();
    resetSelection();

    // This causes a run...maybe don't do this in case user
    // doesn't automatically want to do a run when they Reload
//    callback_ModifyReleasedSL();
}

void
nmfEstimation_Tab1::resetVisibleColumns()
{
    callback_GuildSuppCB(isChecked(Estimation_Tab1_GuildSuppCB));
    callback_GuildRangeCB(isChecked(Estimation_Tab1_GuildRangeCB));
    callback_SpeciesSuppCB(isChecked(Estimation_Tab1_SpeciesSuppCB));
    callback_SpeciesRangeCB(isChecked(Estimation_Tab1_SpeciesRangeCB));
}

bool
nmfEstimation_Tab1::onGuildTab()
{
    return (Estimation_Tab1_PopulationTabW->currentIndex() == 0);
}

void
nmfEstimation_Tab1::callback_ImportPB()
{
    QString filename = "";

    showCovariateColumns(Estimation_Tab1_SpeciesPopulationTV,true);

    if (onGuildTab()) {
        callback_ImportGuild(nmfConstantsMSSPM::Query_User_For_Filename,filename);
    } else {
        callback_ImportSpecies(nmfConstantsMSSPM::UpdateSetup,
                               nmfConstantsMSSPM::Query_User_For_Filename,
                               filename);
    }

    showCovariateColumns(Estimation_Tab1_SpeciesPopulationTV,false);
}

void
nmfEstimation_Tab1::callback_ImportSpecies(
        bool updateSetup,
        bool queryUserForFilename,
        QString speciesFilename)
{
    importSpeciesData("",updateSetup,queryUserForFilename,speciesFilename);
}

void
nmfEstimation_Tab1::callback_ImportGuild(
        bool queryUserForFilename,
        QString guildsFilename)
{
    importGuildData("",nmfConstantsMSSPM::UpdateSetup,
                    queryUserForFilename,
                    guildsFilename);
}

void
nmfEstimation_Tab1::importGuildData(const QString& tableName,
                                    const bool& updateSetup,
                                    const bool& queryForFilename,
                                    QString guildsFilename)
{
    QString errorMsg;
    QList<QString> SpeciesGuilds;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    std::vector<int> ColumnNumbers = {
        nmfConstantsMSSPM::Column_Supp_Guild_Name,
        nmfConstantsMSSPM::Column_Supp_Guild_GrowthRate,
        nmfConstantsMSSPM::Column_Supp_Guild_CarryingCapacity
    };

    bool loadOK = nmfUtilsQt::loadGuildsSpeciesTableview(
                Estimation_Tabs, m_GuildModel,
                "Guild",inputDataPath, tableName, SpeciesGuilds,
                queryForFilename, guildsFilename, 0, errorMsg);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return;
    }
    Estimation_Tab1_GuildPopulationTV->resizeColumnsToContents();
    if (updateSetup) {
        // Load signal with data to send to Species Setup tab
        QList<QString> GuildNames;
        QList<QString> GrowthRate;
        QList<QString> GuildK;
        for (int row=0; row<m_GuildModel->rowCount(); ++row) {
            GuildNames.push_back( m_GuildModel->item(row,ColumnNumbers[0])->text());
            GrowthRate.push_back( m_GuildModel->item(row,ColumnNumbers[1])->text().remove(","));
            GuildK.push_back(     m_GuildModel->item(row,ColumnNumbers[2])->text().remove(","));
        }
        emit UpdateGuildSetupData(GuildNames,GrowthRate,GuildK);
    }
}

void
nmfEstimation_Tab1::importSpeciesData(const QString& tableName,
                                      bool updateSetup,
                                      const bool& queryForFilename,
                                      QString speciesFilename)
{
    QString errorMsg;
    QList<QString> SpeciesGuilds;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    bool loadOK = nmfUtilsQt::loadGuildsSpeciesTableview(
                Estimation_Tabs, m_SpeciesModel,
                "Species", inputDataPath, tableName, SpeciesGuilds,
                queryForFilename, speciesFilename, m_ColumnMap["SpeName"], errorMsg);

    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return;
    }
    Estimation_Tab1_SpeciesPopulationTV->resizeColumnsToContents();

    if (loadOK && updateSetup) {
        // Load signal with data to send to Species Setup tab
        QList<QString> SpeciesNames;
        QList<QString> SpeciesInitBiomass;
        QList<QString> SpeciesGrowthRate;
        QList<QString> SpeciesK;
        for (int row=0; row<m_SpeciesModel->rowCount(); ++row) {
            SpeciesNames.push_back(      m_SpeciesModel->item(row,m_ColumnMap["SpeName"])->text());
            SpeciesInitBiomass.push_back(m_SpeciesModel->item(row,m_ColumnMap["InitBiomass"])->text().remove(","));
            SpeciesGrowthRate.push_back( m_SpeciesModel->item(row,m_ColumnMap["GrowthRate"])->text());
            SpeciesK.push_back(          m_SpeciesModel->item(row,m_ColumnMap["SpeciesK"])->text().remove(","));
        }

        emit UpdateSpeciesSetupData(SpeciesNames,SpeciesGuilds,SpeciesInitBiomass,
                                    SpeciesGrowthRate,SpeciesK);
        QMessageBox::information(Estimation_Tabs, "Data Updated",
                                 "\nSpecies/Guild table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab1::callback_SavePB()
{
    bool okSave = true;
    std::string msg = "\n";
    std::string type = (onGuildTab()) ? "Guilds" : "Species";

    readSettings();

    // Show warning
    QMessageBox::StandardButton reply;
    msg += "Are you sure you want to overwrite current "+type+" settings\n";
    msg += "with current Population Parameters?\n\nThis cannot be undone.\n";
    reply = QMessageBox::question(Estimation_Tabs, tr("Overwrite Species"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }
//  resetPercentageCMB();

    if (onGuildTab()) {
        okSave = savePopulationParametersGuilds(nmfConstantsMSSPM::ShowPopupError);
    } else { // if on Species tab
        okSave   = savePopulationParametersSpecies(nmfConstantsMSSPM::ShowPopupError);
        if (okSave) {
            savePopulationParameterGuildK();
        }
    }

    if (okSave) {
        loadWidgets();
    }
    resetSelection();
}

void
nmfEstimation_Tab1::callback_GuildRangeCB(bool isBoxChecked)
{
    showNoColumns(Estimation_Tab1_GuildPopulationTV);
    showPrimaryColumns(Estimation_Tab1_GuildPopulationTV);
    showRangeColumns(Estimation_Tab1_GuildPopulationTV,isBoxChecked);
    if (isBoxChecked) {
        if (isChecked(Estimation_Tab1_GuildSuppCB)) {
            showAllColumns(Estimation_Tab1_GuildPopulationTV);
        }
    } else {
        showSuppColumns(Estimation_Tab1_GuildPopulationTV,
                        isChecked(Estimation_Tab1_GuildSuppCB));
    }
}

void
nmfEstimation_Tab1::callback_GuildSuppCB(bool isBoxChecked)
{
    showNoColumns(Estimation_Tab1_GuildPopulationTV);
    showPrimaryColumns(Estimation_Tab1_GuildPopulationTV);
    showSuppColumns(Estimation_Tab1_GuildPopulationTV,isBoxChecked);
    if (isBoxChecked) {
        if (isChecked(Estimation_Tab1_GuildRangeCB)) {
            showAllColumns(Estimation_Tab1_GuildPopulationTV);
        }
    } else {
        showRangeColumns(Estimation_Tab1_GuildPopulationTV,
                         isChecked(Estimation_Tab1_GuildRangeCB));
    }
}

void
nmfEstimation_Tab1::callback_SpeciesRangeCB(bool isBoxChecked)
{
    showNoColumns(Estimation_Tab1_SpeciesPopulationTV);
    showPrimaryColumns(Estimation_Tab1_SpeciesPopulationTV);
    showRangeColumns(Estimation_Tab1_SpeciesPopulationTV,isBoxChecked);
    Estimation_Tab1_SpeciesRangeCMB->setEnabled(isBoxChecked);
    Estimation_Tab1_SpeciesRangeSB->setEnabled(false);
    Estimation_Tab1_SpeciesRangeSB->setEnabled(
       (Estimation_Tab1_SpeciesRangeCMB->currentText()=="Percentage") &&
       (isBoxChecked==true));
    if (isBoxChecked) {
        if (isChecked(Estimation_Tab1_SpeciesSuppCB)) {
            showAllColumns(Estimation_Tab1_SpeciesPopulationTV);
        }
    } else {
        showSuppColumns(Estimation_Tab1_SpeciesPopulationTV,
                        isChecked(Estimation_Tab1_SpeciesSuppCB));
    }
}

void
nmfEstimation_Tab1::callback_SpeciesSuppCB(bool isBoxChecked)
{
    showNoColumns(Estimation_Tab1_SpeciesPopulationTV);
    showPrimaryColumns(Estimation_Tab1_SpeciesPopulationTV);
    showSuppColumns(Estimation_Tab1_SpeciesPopulationTV,isBoxChecked);
    if (isBoxChecked) {
        if (isChecked(Estimation_Tab1_SpeciesRangeCB)) {
            showAllColumns(Estimation_Tab1_SpeciesPopulationTV);
        }
    } else {
        showRangeColumns(Estimation_Tab1_SpeciesPopulationTV,
                         isChecked(Estimation_Tab1_SpeciesRangeCB));
    }
}


bool
nmfEstimation_Tab1::checkSpeciesDataPrimary(bool showPopup)
{
    double WeightVal = 0;
    QString SpeName;
    QString InitBiomass;
    QString GrowthRate;
    QString SpeciesK;
    QString Weight;
    QString msg;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        index = m_SpeciesModel->index(i,m_ColumnMap["Weight"]);
        Weight = index.data().toString().remove(",");
        WeightVal = Weight.toDouble();
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomass"]);
        InitBiomass = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRate"]);
        GrowthRate = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeciesK"]);
        SpeciesK = index.data().toString().remove(",");
        if (nmfUtilsQt::emptyField({Weight,SpeName,InitBiomass,GrowthRate,SpeciesK})) {
            checkAndShowEmptyFieldError(showPopup,"checkSpeciesDataPrimary");
            return false;
        }
        if (WeightVal < 0.0 || WeightVal > 1.0) {
            msg = "\nFound out of range species weight. All species weights must be between 0 and 1, inclusive.\n";
            QMessageBox::critical(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

void
nmfEstimation_Tab1::clearWidgets()
{
    //Estimation_Tab1_PopulationTV->reset();
    nmfUtilsQt::clearTableView({Estimation_Tab1_SpeciesPopulationTV});
}

void
nmfEstimation_Tab1::getSelectedIndexes()
{
    m_selIndexes = Estimation_Tab1_SpeciesPopulationTV->selectionModel()->selectedIndexes();

    // Remove indexes that are in hidden columns
    for (int idx = 0; idx < m_selIndexes.size(); ) {
      const QModelIndex &index = m_selIndexes[idx];
      if (Estimation_Tab1_SpeciesPopulationTV->columnWidth(index.column()) == 0) {
          Estimation_Tab1_SpeciesPopulationTV->selectionModel()->select(index, QItemSelectionModel::Deselect);
          m_selIndexes.removeAt(idx);
      } else {
          ++idx;
      }
    }
}

bool
nmfEstimation_Tab1::isChecked(QCheckBox* cb)
{
    return cb->isChecked();
}

void
nmfEstimation_Tab1::resetModifySlider()
{
//    Estimation_Tab1_ModifySL->blockSignals(true);
//    Estimation_Tab1_ModifySL->setValue(50);
//    Estimation_Tab1_ModifySL->blockSignals(false);
//    Estimation_Tab1_ModifySL->repaint(); // so widget refreshes immediately
}

void
nmfEstimation_Tab1::resetPercentageCMB()
{
    Estimation_Tab1_SpeciesRangeSB->setValue(0);
    Estimation_Tab1_SpeciesRangeCMB->setCurrentIndex(0);
}

void
nmfEstimation_Tab1::resetSelection()
{
    foreach (const QModelIndex &index, m_selIndexes){
        Estimation_Tab1_SpeciesPopulationTV->selectionModel()->select(index,QItemSelectionModel::Select);
    }
}

void
nmfEstimation_Tab1::showNoColumns(QTableView* tv)
{
    for (int i=0; i<tv->model()->columnCount(); ++i) {
        tv->setColumnHidden( i,true);
    }
}

void
nmfEstimation_Tab1::showAllColumns(QTableView* tv)
{
    for (int i=0; i<tv->model()->columnCount(); ++i) {
        tv->setColumnHidden( i,false);
    }
    showCovariateColumns(tv,false); // Hide them for now as covariates are entered in their own tab (Tab 6)
}

void
nmfEstimation_Tab1::showCovariateColumns(QTableView* tv, const bool& show)
{
    QString colName;
    QStringList SpeciesSupp = {"GrowthRateCovarCoeff","SpeciesKCovarCoeff"};
//    QStringList GuildSupp   = {"Catchability"};
//    QStringList SuppColumns = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
//                                  SpeciesSupp : GuildSupp;

    for (int i = 0; i < tv->model()->columnCount(); ++i) {
        colName = tv->model()->headerData(i, Qt::Horizontal).toString();
        if (SpeciesSupp.contains(colName)) {
            tv->setColumnHidden(i,!show);
        }
    }
}


void
nmfEstimation_Tab1::showPrimaryColumns(QTableView* tv)
{
    // These columns are always shown for Species
    QString colName;
    QStringList SpeciesPrimary = {"Weight","SpeName","InitBiomass","GrowthRate","SpeciesK"};
    QStringList GuildPrimary   = {"GuildName","GrowthRate","GuildK"};
    QStringList PrimaryColumns = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                                  SpeciesPrimary : GuildPrimary;

    showNoColumns(tv);
    for (int i = 0; i < tv->model()->columnCount(); ++i) {
        colName = tv->model()->headerData(i, Qt::Horizontal).toString();
        if (PrimaryColumns.contains(colName)) {
            tv->setColumnHidden(i,false);
        }
    }
}

void
nmfEstimation_Tab1::showSuppColumns(QTableView* tv, const bool& show)
{
    QString colName;
    QStringList SpeciesSupp = {"GrowthRateShape","SurveyQ","Catchability","SpeDependence","ExploitationRate"};
    QStringList GuildSupp   = {"Catchability"};
    QStringList SuppColumns = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                                  SpeciesSupp : GuildSupp;

    for (int i = 0; i < tv->model()->columnCount(); ++i) {
        colName = tv->model()->headerData(i, Qt::Horizontal).toString();
        if (SuppColumns.contains(colName)) {
            tv->setColumnHidden(i,!show);
        }
    }
    showCovariateColumns(tv,false); // Hide them for now as covariates are entered in their own tab (Tab 6)
}

void
nmfEstimation_Tab1::showRangeColumns(QTableView* tv, const bool& show)
{
    QString     colName;
    QStringList SpeciesRange     = {"InitBiomassMin","InitBiomassMax",
                                    "GrowthRateMin","GrowthRateMax",
                                    "SpeciesKMin","SpeciesKMax"};
    QStringList SpeciesRangeSupp = {"SurveyQMin","SurveyQMax",
                                    "GrowthRateShapeMin","GrowthRateShapeMax",
                                    "CatchabilityMin","CatchabilityMax"};
    QStringList GuildRange       = {"GrowthRateMin","GrowthRateMax",
                                    "GuildKMin","GuildKMax"};
    QStringList GuildRangeSupp   = {"CatchabilityMin","CatchabilityMax"};
    QStringList RangeColumns = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                               SpeciesRange : GuildRange;
    QStringList SuppColumns  = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                                SpeciesRangeSupp : GuildRangeSupp;
    QCheckBox* cb = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                    Estimation_Tab1_SpeciesSuppCB : Estimation_Tab1_GuildSuppCB;

    for (int i = 0; i < tv->model()->columnCount(); ++i) {
        colName = tv->model()->headerData(i, Qt::Horizontal).toString();
        if ( RangeColumns.contains(colName) ||
             (isChecked(cb) && SuppColumns.contains(colName))) {
            tv->setColumnHidden(i,!show);
        }
    }
}

bool
nmfEstimation_Tab1::saveGuildDataSupplemental(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString GuildName;
    QString Catchability;
    QModelIndex index;

    // Save Guild supplemental fields
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableGuilds) + " SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString().remove(",");
        index = m_GuildModel->index(i,7);
        Catchability = index.data().toString().remove(",");
        cmd += "Catchability=" + Catchability;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,Catchability})) {
            checkAndShowEmptyFieldError(showPopup,"saveGuildDataSupplemental");
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveGuildDataSupplemental: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            }
            return false;
        }
    }

    return true;
}

bool
nmfEstimation_Tab1::saveGuildDataRange(bool showPopup)
{
    QString cmd;
    std::string errorMsg;
    QString GuildName;
    QString BadParameter;
    QString GrowthRate,GrowthRateMin,GrowthRateMax;
    QString GuildK,GuildKMin,GuildKMax;
    QModelIndex index;
    QString valueWithoutComma;

    // Save Guild range fields
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableGuilds) + " SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString().remove(",");
        index = m_GuildModel->index(i,1);
        GrowthRate = index.data().toString().remove(",");
        index = m_GuildModel->index(i,2);
        GrowthRateMin = index.data().toString().remove(",");
        cmd += "GrowthRateMin=" + GrowthRateMin + ",";
        index = m_GuildModel->index(i,3);
        GrowthRateMax = index.data().toString().remove(",");
        cmd += "GrowthRateMax=" + GrowthRateMax + ",";

        // This shouldn't be set here. It should be set automatically when user saves Species data.
        index = m_GuildModel->index(i,4);
        valueWithoutComma = index.data().toString().remove(",");
        GuildK = valueWithoutComma;

        index = m_GuildModel->index(i,5);
        valueWithoutComma = index.data().toString().remove(",");
        GuildKMin = valueWithoutComma;
        cmd += "GuildKMin=" + GuildKMin + ",";

        index = m_GuildModel->index(i,6);
        valueWithoutComma = index.data().toString().remove(",");
        GuildKMax = valueWithoutComma;
        cmd += "GuildKMax=" + GuildKMax;

        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,GrowthRateMin,GrowthRateMax,GuildKMin,GuildKMax})) {
            checkAndShowEmptyFieldError(showPopup,"saveGuildDataRange");
            return false;
        }
        if (nmfUtilsQt::outOfRange({"GrowthRate",GrowthRate,GrowthRateMin,GrowthRateMax,
                                    "GuildK",GuildK,GuildKMin,GuildKMax},BadParameter)) {
            checkAndShowOutOfRangeError("Guild",GuildName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveGuildDataRange: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            }
            return false;
        }
    }

    return true;
}

bool
nmfEstimation_Tab1::saveGuildDataSupplementalAndRange(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString BadParameter;
    QString GuildName;
    QString Catchability,CatchabilityMin,CatchabilityMax;
    QModelIndex index;

    // Save the range and supplemental fields
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableGuilds) + " SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString().remove(",");
        index = m_GuildModel->index(i,7);
        Catchability = index.data().toString().remove(",");
        index = m_GuildModel->index(i,8);
        CatchabilityMin = index.data().toString().remove(",");
        cmd += "CatchabilityMin=" + CatchabilityMin + ",";
        index = m_GuildModel->index(i,9);
        CatchabilityMax = index.data().toString().remove(",");
        cmd += "CatchabilityMax=" + CatchabilityMax;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,CatchabilityMin,CatchabilityMax})) {
            checkAndShowEmptyFieldError(showPopup,"saveGuildDataSupplementalAndRange");
            return false;
        }
        if (nmfUtilsQt::outOfRange({"Catchability",Catchability,CatchabilityMin,CatchabilityMax},BadParameter)) {
            checkAndShowOutOfRangeError("Guild",GuildName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveGuildDataSupplementalAndRange: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            }
            return false;
        }
    }

    return true;
}

bool
nmfEstimation_Tab1::saveGuildDataPrimary(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString GuildName;
    QString GrowthRate;
    QString GuildK;
    QModelIndex index;
    QString valueWithoutComma;

    // Save the default (i.e. non-range and non-supplemental) data
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableGuilds) + " SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString().remove(",");
        index = m_GuildModel->index(i,1);
        GrowthRate = index.data().toString().remove(",");
        cmd += "GrowthRate=" + GrowthRate + ",";
        index = m_GuildModel->index(i,4);
        valueWithoutComma = index.data().toString().remove(",");
        GuildK = valueWithoutComma;
        cmd += "GuildK=" + GuildK;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,GrowthRate,GuildK})) {
            checkAndShowEmptyFieldError(showPopup,"saveGuildDataPrimary");
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveGuildDataPrimary: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            }
            return false;
        }
    }
    return true;
}

bool
nmfEstimation_Tab1::savePopulationParametersGuilds(bool showPopup)
{
    if (isChecked(Estimation_Tab1_GuildSuppCB) &&
        isChecked(Estimation_Tab1_GuildRangeCB)) {
        if (! saveGuildDataSupplementalAndRange(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_GuildSuppCB)) {
        if (! saveGuildDataSupplemental(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_GuildRangeCB)) {
        if (! saveGuildDataRange(showPopup)) {
            return false;
        }
    }

    if (! saveGuildDataPrimary(showPopup)) {
        return false;
    }

    emit ReloadGuilds(showPopup);

    if (showPopup) {
        QMessageBox::information(Estimation_Tabs, "Guild Updated",
                                 "\nGuild table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    return true;
}


bool
nmfEstimation_Tab1::isAnySurveyQZero(const bool& showPopup)
{
    std::string errorMsg,briefMsg;
    double SurveyQ;
    double SurveyQMin,SurveyQMax;
    QModelIndex index;
    QString SpeName;
    QString valueWithoutComma;

    // Check that all SurveyQ values are > 0
    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQ"]);
        valueWithoutComma = index.data().toString().remove(",");
        SurveyQ = valueWithoutComma.toDouble();
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQMin"]);
        valueWithoutComma = index.data().toString().remove(",");
        SurveyQMin = valueWithoutComma.toDouble();
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQMax"]);
        valueWithoutComma = index.data().toString().remove(",");
        SurveyQMax = valueWithoutComma.toDouble();

        if (SurveyQ < nmfConstants::NearlyZero) {
            briefMsg  = "SurveyQ values must be greater than 0.";
            errorMsg  = "\nFound: SurveyQ value(s) less than or equal to 0 for Species: " + SpeName.toStdString();
            errorMsg += "\n\n" + briefMsg + "\n";
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1::isAnySurveyQZero: " + briefMsg);
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs,"Warning", QString::fromStdString(errorMsg),
                                     QMessageBox::Ok);
            }
            return false;
        }
        if ((SurveyQMin < nmfConstants::NearlyZero) || (SurveyQMax < nmfConstants::NearlyZero)) {
            briefMsg  = "All SurveyQ Min and Max values must be greater than 0.";
            errorMsg  = "\nFound: SurveyQ Min or Max value(s) less than or equal to 0 for Species: " + SpeName.toStdString();
            errorMsg += "\n\n" + briefMsg + "\n";
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1::isAnySurveyQZero: " + briefMsg);
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs,"Warning", QString::fromStdString(errorMsg),
                                     QMessageBox::Ok);
            }
            return false;
        }
    }

    return true;
}

bool
nmfEstimation_Tab1::isInitBiomassLessThanSpeciesKMin(const bool& showPopup)
{
    std::string errorMsg,briefMsg;
    double InitBiomass;
    double SpeciesKMin;
    QModelIndex index;
    QString SpeName;
    QString valueWithoutComma;

    // Check that InitBiomass < the SpeciesKMin value in the Species table
    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomass"]);
        valueWithoutComma = index.data().toString().remove(",");
        InitBiomass = valueWithoutComma.toDouble();
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeciesKMin"]);
        valueWithoutComma = index.data().toString().remove(",");
        SpeciesKMin = valueWithoutComma.toDouble();

        if (InitBiomass > SpeciesKMin) {
            briefMsg  = "InitBiomass should be less than SpeciesKMin.";
            errorMsg  = "\nFound: InitBiomass (" + QString::number(InitBiomass,'f',6).toStdString() +
                    ") > SpeciesKMin (" + QString::number(SpeciesKMin,'f',6).toStdString() +
                    ") for Species: " + SpeName.toStdString();
            errorMsg += "\n\n" + briefMsg +
                        "\n\nThis is allowed, but please recheck all species and confirm that this is what's desired.\n";
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1::isInitBiomassLessThanSpeciesKMin: " + briefMsg);
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs,"Warning", QString::fromStdString(errorMsg),
                                     QMessageBox::Ok);
            }
            return true;
        }
    }
    return true;
}

bool
nmfEstimation_Tab1::isOkSpeciesDataSupplemental(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString SpeName;
    QString SurveyQ;
    QString Catchability;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + " SET ";
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQ"]);
        SurveyQ = index.data().toString().remove(",");
        cmd += "SurveyQ=" + SurveyQ + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["Catchability"]);
        Catchability = index.data().toString().remove(",");
        cmd += "Catchability=" + Catchability;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,SurveyQ,Catchability})) {
            checkAndShowEmptyFieldError(showPopup,"saveSpeciesDataSupplemental");
            return false;
        }
        // Not necessary since we're saving in savePopulationParameters()
        //errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        //if (nmfUtilsQt::isAnError(errorMsg)) {
        //    m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataSupplemental: Write table error: " + errorMsg);
        //    m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
        //    if (showPopup) {
        //        QMessageBox::warning(Estimation_Tabs, "Error",
        //                             "\nError in Save command.  Check that all cells are populated.\n",
        //                             QMessageBox::Ok);
        //    }
        //    return false;
        //}
    }
    return true;
}

bool
nmfEstimation_Tab1::isOkSpeciesDataRange(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString BadParameter;
    QString SpeName,InitBiomass,InitBiomassMin,InitBiomassMax;
    QString GrowthRate,GrowthRateMin,GrowthRateMax;
    QString GrowthRateShape,GrowthRateShapeMin,GrowthRateShapeMax;
    QString SpeciesK,SpeciesKMin,SpeciesKMax;
    QModelIndex index;
    QString valueWithoutComma;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + " SET ";
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomass"]);
        valueWithoutComma = index.data().toString().remove(",");
        InitBiomass = valueWithoutComma;
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomassMin"]);
        valueWithoutComma = index.data().toString().remove(",");
        InitBiomassMin = valueWithoutComma;
        cmd += "InitBiomassMin=" + QString::number(InitBiomassMin.toDouble(),'f',6) + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomassMax"]);
        valueWithoutComma = index.data().toString().remove(",");
        InitBiomassMax = valueWithoutComma;
        cmd += "InitBiomassMax=" + QString::number(InitBiomassMax.toDouble(),'f',6) + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRate"]);
        GrowthRate = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRateMin"]);
        GrowthRateMin = index.data().toString().remove(",");
        cmd += "GrowthRateMin=" + GrowthRateMin + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRateMax"]);
        GrowthRateMax = index.data().toString().remove(",");
        cmd += "GrowthRateMax=" + GrowthRateMax + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShape"]);
        GrowthRateShape = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShapeMin"]);
        GrowthRateShapeMin = index.data().toString().remove(",");
        cmd += "GrowthRateShapeMin=" + GrowthRateShapeMin + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShapeMax"]);
        GrowthRateShapeMax = index.data().toString().remove(",");
        cmd += "GrowthRateShapeMax=" + GrowthRateShapeMax + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeciesK"]);
        valueWithoutComma = index.data().toString().remove(",");
        SpeciesK = valueWithoutComma;
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeciesKMin"]);
        valueWithoutComma = index.data().toString().remove(",");
        SpeciesKMin = valueWithoutComma;
        cmd += "SpeciesKMin=" + QString::number(SpeciesKMin.toDouble(),'f',6) + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeciesKMax"]);
        valueWithoutComma = index.data().toString().remove(",");
        SpeciesKMax = valueWithoutComma;
        cmd += "SpeciesKMax=" + QString::number(SpeciesKMax.toDouble(),'f',6);
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,InitBiomassMin,InitBiomassMax,
                                    GrowthRateMin,GrowthRateMax,
                                    GrowthRateShapeMin,GrowthRateShapeMax,
                                    SpeciesKMin,SpeciesKMax})) {
            checkAndShowEmptyFieldError(showPopup,"saveSpeciesDataRange");
            return false;
        }
        if (nmfUtilsQt::outOfRange({"InitBiomass",InitBiomass,InitBiomassMin,InitBiomassMax,
                                    "GrowthRate",GrowthRate,GrowthRateMin,GrowthRateMax,
                                    "GrowthRateShape",GrowthRateShape,GrowthRateShapeMin,GrowthRateShapeMax,
                                    "SpeciesK",SpeciesK,SpeciesKMin,SpeciesKMax},BadParameter)) {
            checkAndShowOutOfRangeError("Species",SpeName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        // Not necessary since we're saving in savePopulationParameters()
        //errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        //if (nmfUtilsQt::isAnError(errorMsg)) {
        //    m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataRange: Write table error: " + errorMsg);
        //    m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
        //    if (showPopup) {
        //        QMessageBox::warning(Estimation_Tabs, "Error",
        //                             "\nError in Save command.  Check that all cells are populated.\n",
        //                             QMessageBox::Ok);
        //    }
        //    return false;
        //}
    }
    return true;
}

bool
nmfEstimation_Tab1::isOkSpeciesDataSupplementalAndRange(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString BadParameter;
    QString SpeName;
    QString SurveyQ,SurveyQMin,SurveyQMax;
    QString Catchability,CatchabilityMin,CatchabilityMax;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + " SET ";
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQ"]);
        SurveyQ = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQMin"]);
        SurveyQMin = index.data().toString().remove(",");
        cmd += "SurveyQMin=" + SurveyQMin + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["SurveyQMax"]);
        SurveyQMax = index.data().toString().remove(",");
        cmd += "SurveyQMax=" + SurveyQMax + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["Catchability"]);
        Catchability = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,m_ColumnMap["CatchabilityMin"]);
        CatchabilityMin = index.data().toString().remove(",");
        cmd += "CatchabilityMin=" + CatchabilityMin + ",";
        index = m_SpeciesModel->index(i,m_ColumnMap["CatchabilityMax"]);
        CatchabilityMax = index.data().toString().remove(",");
        cmd += "CatchabilityMax=" + CatchabilityMax;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,SurveyQMin,SurveyQMax,CatchabilityMin,CatchabilityMax})) {
            checkAndShowEmptyFieldError(showPopup,"saveSpeciesDataSupplementalAndRange");
            return false;
        }
        if (nmfUtilsQt::outOfRange({"SurveyQ",SurveyQ,SurveyQMin,SurveyQMax,
                                    "Catchability",Catchability,CatchabilityMin,CatchabilityMax},BadParameter)) {
            checkAndShowOutOfRangeError("Species",SpeName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        // Not necessary since we're saving in savePopulationParameters()
        //errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        //if (nmfUtilsQt::isAnError(errorMsg)) {
        //    m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataSupplementalAndRange: Write table error: " + errorMsg);
        //    m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
        //    if (showPopup) {
        //        QMessageBox::warning(Estimation_Tabs, "Error",
        //                             "\nError in Save command.  Check that all cells are populated.\n",
        //                             QMessageBox::Ok);
        //    }
        //    return false;
        //}
    }
    return true;
}

bool
nmfEstimation_Tab1::savePopulationParameters(const bool& showPopup)
{
    std::string errorMsg;
    QString cmd;
    QModelIndex index;
    QString SpeName;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + " SET ";

        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        SpeName = index.data().toString().remove(",");

        cmd += "Weight="             + QString::number(m_SpeciesModel->index(i,m_ColumnMap["Weight"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "InitBiomass="        + QString::number(m_SpeciesModel->index(i,m_ColumnMap["InitBiomass"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "InitBiomassMin="     + QString::number(m_SpeciesModel->index(i,m_ColumnMap["InitBiomassMin"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "InitBiomassMax="     + QString::number(m_SpeciesModel->index(i,m_ColumnMap["InitBiomassMax"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRate="         + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRate"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRateMin="      + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRateMin"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRateMax="      + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRateMax"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRateShape="    + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShape"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRateShapeMin=" + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShapeMin"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "GrowthRateShapeMax=" + QString::number(m_SpeciesModel->index(i,m_ColumnMap["GrowthRateShapeMax"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "SpeciesK="           + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SpeciesK"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "SpeciesKMin="        + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SpeciesKMin"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "SpeciesKMax="        + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SpeciesKMax"]).data().toString().remove(",").toDouble(),'f',6) + ",";
        cmd += "SurveyQ="            + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SurveyQ"]).data().toString().remove(",").toDouble(),'f',16) + ",";
        cmd += "SurveyQMin="         + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SurveyQMin"]).data().toString().remove(",").toDouble(),'f',16) + ",";
        cmd += "SurveyQMax="         + QString::number(m_SpeciesModel->index(i,m_ColumnMap["SurveyQMax"]).data().toString().remove(",").toDouble(),'f',16) + ",";
        cmd += "Catchability="       + m_SpeciesModel->index(i,m_ColumnMap["Catchability"]).data().toString().remove(",") + ",";
        cmd += "CatchabilityMin="    + m_SpeciesModel->index(i,m_ColumnMap["CatchabilityMin"]).data().toString().remove(",") + ",";
        cmd += "CatchabilityMax="    + m_SpeciesModel->index(i,m_ColumnMap["CatchabilityMax"]).data().toString().remove(",");
        cmd += " WHERE SpeName = '"  + SpeName + "'";

        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1::savePopulationParameters: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
            if (showPopup) {
                QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            }
            return false;
        }
    }
    return true;
}

bool
nmfEstimation_Tab1::savePopulationParameterGuildK()
{
    // Calculate and save the guildk values and write them to the Guilds file
    // Make sure the user can then save the Guild Parameters in Estimation Tab 1

std::cout << "nmfEstimation_Tab1::savePopulationParameterGuildK tbd" << std::endl;

    return true;
}

bool
nmfEstimation_Tab1::saveSpeciesDataPrimary(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString SpeName,InitBiomass;
    QString GrowthRate;
    QString SpeciesK;
    QModelIndex index;
    QString valueWithoutComma;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE " + QString::fromStdString(nmfConstantsMSSPM::TableSpecies) + " SET ";
        index = m_SpeciesModel->index(i,0);
        SpeName = index.data().toString().remove(",");
        index = m_SpeciesModel->index(i,1);
        valueWithoutComma = index.data().toString().remove(",");
        InitBiomass = valueWithoutComma;
        cmd += "InitBiomass=" + InitBiomass + ",";
        index = m_SpeciesModel->index(i,4);
        GrowthRate = index.data().toString().remove(",");
        cmd += "GrowthRate=" + GrowthRate + ",";
        index = m_SpeciesModel->index(i,8);
        valueWithoutComma = index.data().toString().remove(",");
        SpeciesK = valueWithoutComma;
        cmd += "SpeciesK=" + SpeciesK;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,InitBiomass,GrowthRate,SpeciesK})) {
            checkAndShowEmptyFieldError(showPopup,"saveSpeciesDataPrimary");
            return false;
        }
        // Not necessary since we're saving in savePopulationParameters()
        //errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        //if (nmfUtilsQt::isAnError(errorMsg)) {
        //    m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataPrimary: Write table error: " + errorMsg);
        //    m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd.toStdString());
        //    if (showPopup) {
        //        QMessageBox::warning(Estimation_Tabs, "Error",
        //                             "\nError in Save command.  Check that all cells are populated.\n",
        //                             QMessageBox::Ok);
        //    }
        //    return false;
        //}
    }
    return true;
}


bool
nmfEstimation_Tab1::savePopulationParametersSpecies(bool showPopup)
{
    if (! isAnySurveyQZero(showPopup)) {
        return false;
    }

    if (! isInitBiomassLessThanSpeciesKMin(showPopup)) {
        return false;
    }

    if (isChecked(Estimation_Tab1_SpeciesSuppCB) &&
        isChecked(Estimation_Tab1_SpeciesRangeCB)) {
        if (! isOkSpeciesDataSupplementalAndRange(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_SpeciesSuppCB)) {
        if (! isOkSpeciesDataSupplemental(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_SpeciesRangeCB)) {
        if (! isOkSpeciesDataRange(showPopup)) {
            return false;
        }
    }

    if (! saveSpeciesDataPrimary(showPopup)) {
        return false;
    }


    // update BiomassAbsolute table with the initial Biomass values
    savePopulationParameters(showPopup);
    updateBiomassAbsoluteTable();

    emit ReloadSpecies(showPopup);

    if (showPopup) {
        QMessageBox::information(Estimation_Tabs, "Species Updated",
                                 "\nSpecies table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    return true;
}



void
nmfEstimation_Tab1::updateBiomassAbsoluteTable()
{
    std::string cmd;
    std::string errorMsg;
    QModelIndex index;
    std::string initBiomass;
    std::string species;
    QString valueWithoutComma;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        index = m_SpeciesModel->index(i,m_ColumnMap["SpeName"]);
        species = index.data().toString().remove(",").toStdString();
        index = m_SpeciesModel->index(i,m_ColumnMap["InitBiomass"]);
        valueWithoutComma = index.data().toString().remove(",");
        initBiomass = valueWithoutComma.toStdString();

        // Need to also update the BiomassAbsolute table with the initial Biomass values
        cmd  = "REPLACE INTO " + nmfConstantsMSSPM::TableBiomassAbsolute + " (";
        cmd += "ProjectName,ModelName,SpeName,Year,Value) VALUES ('" +
                m_ProjectName + "','" +
                m_ModelName   + "','" +
                species       + "', 0, " +
                initBiomass   + ");";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (BiomassAbsolute): Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs,"Warning", "\nCouldn't REPLACE INTO " +
                                 QString::fromStdString(nmfConstantsMSSPM::TableBiomassAbsolute) + " table.\n",
                                 QMessageBox::Ok);
            return;
        }
    }

    emit ReloadSetupWidgets();
}


void
nmfEstimation_Tab1::callback_ExportPB()
{
    bool isValidFilename;
    bool isGuildVisible = onGuildTab();
    std::string tableName = (isGuildVisible) ? nmfConstantsMSSPM::TableGuilds :
                                               nmfConstantsMSSPM::TableSpecies;
    QString tableNameStr = QString::fromStdString(tableName);
    QList<QString> GuildName;
    QList<QString> GuildGrowthRate;
    QList<QString> GuildK;
    QList<QString> SpeciesName;
//    QList<QString> SpeciesGuild;
    QList<QString> SpeciesInitialBiomass;
    QList<QString> SpeciesGrowthRate;
    QList<QString> SpeciesK;

    isValidFilename = nmfUtilsQt::getCSVFileName(Estimation_Tabs,tableNameStr);
    if (isValidFilename) {
        if (isGuildVisible) {
            saveGuildsCSVFile(tableNameStr,GuildName,GuildGrowthRate,GuildK);
        } else {
            emit LoadSpeciesGuild();
            callback_SaveSpeciesCSVFile(tableNameStr,SpeciesName,m_SpeciesGuild,
                               SpeciesInitialBiomass,SpeciesGrowthRate,SpeciesK);
        }
    }
}

void
nmfEstimation_Tab1::setSpeciesGuild(QList<QString> SpeciesGuildList)
{
    m_SpeciesGuild = SpeciesGuildList;
}


void
nmfEstimation_Tab1::callback_QueryAndSaveSpeciesCSVFile(
        QString tableNameStr,
        QList<QString> SpeciesName,
        QList<QString> SpeciesGuild,
        QList<QString> SpeciesInitialBiomass,
        QList<QString> SpeciesGrowthRate,
        QList<QString> SpeciesK)
{
    bool isValidFilename = nmfUtilsQt::getCSVFileName(Estimation_Tabs,tableNameStr);
    if (isValidFilename) {
        callback_SaveSpeciesCSVFile(
                tableNameStr,
                SpeciesName,
                SpeciesGuild,
                SpeciesInitialBiomass,
                SpeciesGrowthRate,
                SpeciesK);
    }
}

void
nmfEstimation_Tab1::callback_SaveSpeciesCSVFile(
        QString tableName,
        QList<QString> SpeciesName,
        QList<QString> SpeciesGuild,
        QList<QString> SpeciesInitialBiomass,
        QList<QString> SpeciesGrowthRate,
        QList<QString> SpeciesK)
{
    bool okSave;
    bool isExportFromSetup = (SpeciesName.size() > 0);
    QString type = "Species";
    QStandardItemModel* smodel = m_SpeciesModel;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString tableNameStr  = QDir(inputDataPath).filePath(tableName);

    okSave = nmfUtilsQt::saveSpeciesTableView(Estimation_Tabs,
                                              smodel,
                                              m_ColumnMap,
                                              inputDataPath,tableNameStr,
                                              SpeciesName,SpeciesGuild,SpeciesInitialBiomass,
                                              SpeciesGrowthRate,SpeciesK);

    QFileInfo fileInfo(tableNameStr);
    if (fileInfo.suffix().toLower() != "csv") {
        tableNameStr += ".csv";
    }

    if (okSave) {
        QMessageBox::information(Estimation_Tabs, "File Saved",
                                 "\n"+type+" CSV file has been successfully saved as:\n\n"+tableNameStr+"\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "File Save Error",
                                 "\n"+type+" CSV file has not been saved. Please check for valid filename (i.e., non-blank).\n",
                                 QMessageBox::Ok);
    }

    if (isExportFromSetup) {
        importSpeciesData(tableNameStr,nmfConstantsMSSPM::DontUpdateSetup,
                          nmfConstantsMSSPM::Query_User_For_Filename,"");
    } else {
        importSpeciesData(tableNameStr,nmfConstantsMSSPM::UpdateSetup,
                          nmfConstantsMSSPM::Query_User_For_Filename,"");
    }

}

void
nmfEstimation_Tab1::callback_SaveGuildsCSVFile(
        QList<QString> GuildName,
        QList<QString> GrowthRate,
        QList<QString> GuildK)
{
    QString tableName = QString::fromStdString(nmfConstantsMSSPM::TableGuilds);
    bool isValidFilename = nmfUtilsQt::getCSVFileName(Estimation_Tabs,tableName);

    if (isValidFilename) {
        saveGuildsCSVFile(tableName,GuildName,GrowthRate,GuildK);
    }
}

void
nmfEstimation_Tab1::saveGuildsCSVFile(QString& tableName,
                                      QList<QString>& GuildName,
                                      QList<QString>& GrowthRate,
                                      QList<QString>& GuildK)
{
    bool okSave;
    bool isExportFromSetup = (GuildName.size() > 0);
    QString type;
    QStandardItemModel* smodel;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString tableNameStr  = QDir(inputDataPath).filePath(tableName);

//    if (onGuildTab()) {
        type = "Guilds";
        smodel = m_GuildModel;
//    }
//    else {
//        type = "Species";
//        smodel = m_SpeciesModel;
//    }

    okSave = nmfUtilsQt::saveGuildsTableView(
                Estimation_Tabs,smodel,inputDataPath,tableNameStr,
                GuildName,GrowthRate,GuildK);

    QFileInfo fileInfo(tableNameStr);
    if (fileInfo.suffix().toLower() != "csv") {
        tableNameStr += ".csv";
    }

    if (okSave) {
        QMessageBox::information(Estimation_Tabs, "File Saved",
                                 "\n"+type+" CSV file has been successfully saved as:\n\n"+tableNameStr+"\n",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(Estimation_Tabs, "File Save Error",
                                 "\n"+type+" CSV file has not been saved. Please check for valid filename (i.e., non-blank).\n",
                                 QMessageBox::Ok);
    }

//    if (onGuildTab()) {
        if (isExportFromSetup) {
            importGuildData(tableNameStr,nmfConstantsMSSPM::DontUpdateSetup,
                            nmfConstantsMSSPM::Query_User_For_Filename,"");
        } else {
            importGuildData(tableNameStr,nmfConstantsMSSPM::UpdateSetup,
                            nmfConstantsMSSPM::Query_User_For_Filename,"");
        }
//    }
//    else {
//        if (isExportFromSetup) {
//            importSpeciesData(tableNameStr,nmfConstantsMSSPM::DontUpdateSetup);
//        } else {
//            importSpeciesData(tableNameStr,nmfConstantsMSSPM::UpdateSetup);
//        }
//    }
}

void
nmfEstimation_Tab1::setupHelpGuilds()
{
    QString msg;
    QString prefix = "<html>";
    QString suffix = "</html>";
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab1_GuildPopulationTV->model());

    // set Tool tips here for column headings
    smodel->horizontalHeaderItem(0)->setToolTip("Guild Name");
    smodel->horizontalHeaderItem(1)->setToolTip("Initial Growth Rate");
    smodel->horizontalHeaderItem(2)->setToolTip("Minimum Growth Rate");
    smodel->horizontalHeaderItem(3)->setToolTip("Maximum Growth Rate");
    smodel->horizontalHeaderItem(4)->setToolTip("Initial Carrying Capacity");
    smodel->horizontalHeaderItem(5)->setToolTip("Minimum Carrying Capacity");
    smodel->horizontalHeaderItem(6)->setToolTip("Maximum Carrying Capacity");
    smodel->horizontalHeaderItem(7)->setToolTip("Initial Catchability");
    smodel->horizontalHeaderItem(8)->setToolTip("Minimum Catchability");
    smodel->horizontalHeaderItem(9)->setToolTip("Maximum Catchability");

    msg  = "</html><strong><center>Guild Name</center></strong><br>";
    msg += "The Guild name entered must be unique.";
    smodel->horizontalHeaderItem(0)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Growth Rate</center></strong><br>";
    msg += "The Guild growth rate (r) is a unit-less value typically between 0.0 and 1.0.";
    smodel->horizontalHeaderItem(1)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Growth Rate</center></strong><br>";
    msg += "The minimum growth rate must be less than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(2)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Growth Rate</center></strong><br>";
    msg += "The maximum growth rate must be greater than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(3)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Guild K</center></strong><br>";
    msg += "The Guild carrying capacity (K) is the number of individuals<br>";
    msg += "in a population that can be supported by the habitat's resources.";
    smodel->horizontalHeaderItem(4)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Guild K</center></strong><br>";
    msg += "The minimum carrying capacity (K) must be less than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(5)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Guild K</center></strong><br>";
    msg += "The maximum carrying capacity (K) must be greater than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(6)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Catchability</center></strong><br>";
    msg += "The catchability (q) refers to the efficiency of the fishery. This coefficient";
    msg += "relates the biomass to the fishing mortality.";
    smodel->horizontalHeaderItem(7)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Catchability</center></strong><br>";
    msg += "The minimum catchability must be less than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(8)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Catchability</center></strong><br>";
    msg += "The maximum catchability must be greater than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(9)->setWhatsThis(prefix+msg+suffix);
}

void
nmfEstimation_Tab1::setupHelpSpecies()
{
    QString msg;
    QString prefix = "<html>";
    QString suffix = "</html>";
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab1_SpeciesPopulationTV->model());

    // set Tool tips here for column headings
    smodel->horizontalHeaderItem(m_ColumnMap["Weight"])->setToolTip("Species Fitness Weight");
    smodel->horizontalHeaderItem(m_ColumnMap["SpeName"])->setToolTip("Species Name");
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomass"])->setToolTip("Initial Absolute Biomass");
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomassMin"])->setToolTip("Initial Absolute Biomass Minimum Value");
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomassMax"])->setToolTip("Initial Absolute Biomass Maximum Value");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRate"])->setToolTip("Initial Growth Rate");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateMin"])->setToolTip("Minimum Growth Rate");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateMax"])->setToolTip("Maximum Growth Rate");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShape"])->setToolTip("Growth Rate Shape Parameter");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShapeMin"])->setToolTip("Minimum Growth Rate Shape Parameter Value");
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShapeMax"])->setToolTip("Maximum Growth Rate Shape Parameter Value");
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesK"])->setToolTip("Initial Carrying Capacity");
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesKMin"])->setToolTip("Minimum Carrying Capacity");
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesKMax"])->setToolTip("Maximum Carrying Capacity");

    msg  = "</html><strong><center>Weight</center></strong><br>";
    msg += "The weight of the species is used to define what percentage of the species biomass contributes ";
    msg += "to the calculated fitness value. For example, if species A's weight was 1 and all other species ";
    msg += "had a weight of 0, then only species A's biomass would contribute to the single fitness value calculated.";
    smodel->horizontalHeaderItem(m_ColumnMap["Weight"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Species Name</center></strong><br>";
    msg += "The Species name entered must be unique.";
    smodel->horizontalHeaderItem(m_ColumnMap["SpeName"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Initial Absolute Biomass</center></strong><br>";
    msg += "The initial species biomass is in units of metric tons.";
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomass"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Initial Absolute Biomass</center></strong><br>";
    msg += "The minimum initial absolute biomass must be less than or equal to the initial absolute biomass.";
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomassMin"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Initial Absolute Biomass</center></strong><br>";
    msg += "The maximum initial absolute biomass must be greater than or equal to the initial absolute biomass.";
    smodel->horizontalHeaderItem(m_ColumnMap["InitBiomassMax"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Growth Rate</center></strong><br>";
    msg += "The Species growth rate (r) is a unit-less value typically between 0.0 and 1.0.";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRate"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Growth Rate</center></strong><br>";
    msg += "The minimum growth rate must be less than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateMin"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Growth Rate</center></strong><br>";
    msg += "The maximum growth rate must be greater than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateMax"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Growth Rate Shape Parameter</center></strong><br>";
    msg += "The Species growth rate shape parameter (p) is a unit-less value which acts as an exponent inside the growth term.";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShape"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Growth Rate Shape Parameter</center></strong><br>";
    msg += "The minimum growth rate shape parameter must be less than or equal to the maximum shape parameter.";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShapeMin"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Growth Rate Shape Parameter</center></strong><br>";
    msg += "The maximum growth rate shape parameter must be greater than or equal to the minimum shape paramter.";
    smodel->horizontalHeaderItem(m_ColumnMap["GrowthRateShapeMax"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Species K</center></strong><br>";
    msg += "The Species carrying capacity (K) is the number of individuals<br>";
    msg += "in a population that can be supported by the habitat's resources.";
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesK"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Species K</center></strong><br>";
    msg += "The minimum carrying capacity (K) must be less than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesKMin"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Species K</center></strong><br>";
    msg += "The maximum carrying capacity (K) must be greater than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(m_ColumnMap["SpeciesKMax"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Catchability</center></strong><br>";
    msg += "The catchability (q) refers to the efficiency of the fishery. This coefficient";
    msg += "relates the biomass to the fishing mortality.";
    smodel->horizontalHeaderItem(m_ColumnMap["Catchability"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Catchability</center></strong><br>";
    msg += "The minimum catchability must be less than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(m_ColumnMap["CatchabilityMin"])->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Catchability</center></strong><br>";
    msg += "The maximum catchability must be greater than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(m_ColumnMap["CatchabilityMax"])->setWhatsThis(prefix+msg+suffix);

}


bool
nmfEstimation_Tab1::loadWidgets()
{
    readSettings();

    bool okSpecies = loadSpecies();
    bool okGuilds  = loadGuilds();
    if (! okSpecies || ! okGuilds) {
        return false;
    }

    resetVisibleColumns();
    if (okSpecies) {
        setupHelpSpecies();
    }
    if (okGuilds) {
        setupHelpGuilds();
    }

//    if (Estimation_Tab1_SpeciesRangeSB->isEnabled()) {
//        callback_SpeciesRangeSB(Estimation_Tab1_SpeciesRangeSB->value());
//    }
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadWidgets end");

    return true;
}

bool
nmfEstimation_Tab1::loadGuilds()
{
    int col;
    int NumGuilds;
    int NumPopulationFields;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString value;
    QStandardItem *item;
    QString field;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadGuilds");

    // Load Population tableview
    fields = {"GuildName","GrowthRate","GrowthRateMin","GrowthRateMax",
              "GuildK","GuildKMin","GuildKMax","Catchability","CatchabilityMin","CatchabilityMax"};
    queryStr   = "SELECT GuildName,GrowthRate,GrowthRateMin,GrowthRateMax,"
                 "GuildK,GuildKMin,GuildKMax,Catchability,CatchabilityMin,CatchabilityMax FROM " + nmfConstantsMSSPM::TableGuilds;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields, nmfConstantsMSSPM::ShowBlankFields);
    NumGuilds = dataMap["GuildName"].size();
    QStringList PopulationFieldList = {"GuildName","GrowthRate","GrowthRateMin","GrowthRateMax",
                                       "GuildK","GuildKMin","GuildKMax",
                                       "Catchability","CatchabilityMin","CatchabilityMax"};
    NumPopulationFields = (int)PopulationFieldList.size();
    m_GuildModel = new QStandardItemModel( NumGuilds, NumPopulationFields );
    for (int row=0; row<NumGuilds; ++row) {
        col = 0;
        for (QString field : PopulationFieldList) {
            value = QString::fromStdString(dataMap[field.toStdString()][row]);
            if (PopulationFieldList[col].contains("GuildK")) {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            value.toDouble(),m_NumSignificantDigits,6);
                value = valueWithComma;
            }
//            else {
//                value = QString::fromStdString(dataMap[field.toStdString()][row]);
//            }
            item = new QStandardItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            m_GuildModel->setItem(row, col++, item);
        }
    }
    m_GuildModel->setHorizontalHeaderLabels(PopulationFieldList);
    Estimation_Tab1_GuildPopulationTV->setModel(m_GuildModel);
    Estimation_Tab1_GuildPopulationTV->resizeColumnsToContents();

//    disconnect(Estimation_Tab1_GuildPopulationTV->selectionModel(),0,0,0);
//    connect(Estimation_Tab1_GuildPopulationTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,
//                                                                                         const QItemSelection&)),
//            this, SLOT(callback_SelectionChangedTV(const QItemSelection&,
//                                                   const QItemSelection&)));

    callback_GuildSuppCB(false);
    callback_GuildRangeCB(false);
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadGuilds end");

    return (NumGuilds > 0);
}

// Load all Species as you'll be hiding some
bool
nmfEstimation_Tab1::loadSpecies()
{
    int col;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString value;
    QStandardItem *item;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadSpecies()");

    // Load Population tableview
    fields = {"Weight","SpeName","InitBiomass","InitBiomassMin","InitBiomassMax",
              "GrowthRate","GrowthRateMin","GrowthRateMax",
              "GrowthRateShape","GrowthRateShapeMin","GrowthRateShapeMax",
              "SpeciesK","SpeciesKMin","SpeciesKMax",
              "SurveyQ","SurveyQMin","SurveyQMax","Catchability",
              "CatchabilityMin","CatchabilityMax"};
    queryStr   = "SELECT Weight,SpeName,InitBiomass,InitBiomassMin,InitBiomassMax,";
    queryStr  += "GrowthRate,GrowthRateMin,GrowthRateMax,";
    queryStr  += "GrowthRateShape,GrowthRateShapeMin,GrowthRateShapeMax,";
    queryStr  += "SpeciesK,SpeciesKMin,SpeciesKMax,";
    queryStr  += "SurveyQ,SurveyQMin,SurveyQMax,Catchability,CatchabilityMin,";
    queryStr  += "CatchabilityMax FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields, nmfConstantsMSSPM::ShowBlankFields);
    NumSpecies = dataMap["SpeName"].size();
    QStringList fieldList;
    QLocale locale(QLocale::English);
    QString valueWithComma;

    if (NumSpecies == 0) {
        return false;
    }

    m_originalSpeciesValuesAll.clear();
    m_SpeciesModel = new QStandardItemModel( NumSpecies, fields.size() );
    for (int row=0; row<NumSpecies; ++row) {
        col = 0;
        for (std::string field : fields) {
            if (row == 0) {
                fieldList << QString::fromStdString(field);
            }
            value = QString::fromStdString(dataMap[field][row]);
            m_originalSpeciesValuesAll.push_back(value);
            if ((fields[col].find("InitBiomass") != std::string::npos) ||
                (fields[col].find("SpeciesK")    != std::string::npos)) {
//              valueWithComma = locale.toString(value.toDouble());
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            value.toDouble(),m_NumSignificantDigits,6);
                item = new QStandardItem(valueWithComma);
            } else {
                item = new QStandardItem(value);
            }
            item->setTextAlignment(Qt::AlignCenter);
            m_SpeciesModel->setItem(row, col++, item);
        }
    }
    m_SpeciesModel->setHorizontalHeaderLabels(fieldList);
    Estimation_Tab1_SpeciesPopulationTV->setModel(m_SpeciesModel);
    Estimation_Tab1_SpeciesPopulationTV->resizeColumnsToContents();

    disconnect(Estimation_Tab1_SpeciesPopulationTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,
                                                                                    const QItemSelection&)),
            this, SLOT(callback_SelectionChangedTV(const QItemSelection&,
                                                   const QItemSelection&)));

    connect(Estimation_Tab1_SpeciesPopulationTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,
                                                                                    const QItemSelection&)),
            this, SLOT(callback_SelectionChangedTV(const QItemSelection&,
                                                   const QItemSelection&)));

    callback_SpeciesSuppCB(false);
    callback_SpeciesRangeCB(false);

    nmfUtilsQt::buildColumnMap(m_Logger,m_SpeciesModel,m_ColumnMap);

    return (NumSpecies > 0);
}

void
nmfEstimation_Tab1::setOutputSpecies(QString species)
{
    m_OutputSpecies = species;
}

QString
nmfEstimation_Tab1::getOutputSpecies()
{
    return m_OutputSpecies;
}

void
nmfEstimation_Tab1::callback_SpeciesRangeCMB(QString value)
{
    Estimation_Tab1_SpeciesRangeSB->setEnabled(value == "Percentage");
    Estimation_Tab1_MinMaxCMB->setEnabled(value == "Percentage");

    // RSK - Not sure we want this logic. Comment it out for now.
//    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab1_SpeciesPopulationTV);
//    if (value == "Percentage") {
//        callback_SpeciesRangeSB(Estimation_Tab1_SpeciesRangeSB->value());
//    } else {
//        loadWidgets();
//    }
//    nmfUtilsQt::reselectTableViewCells(Estimation_Tab1_SpeciesPopulationTV,indexes);
}

void
nmfEstimation_Tab1::callback_SpeciesRangeSB(int pct)
{
    // Change parameter min/max values for the selected parameters or if none
    // are selected, for all parameters that have min/max values.
    int col;
    int row;
    int eeFactor;
    double pctVal = double(pct)/100.0;
    QModelIndex index;
    std::set<int> selectedParameters;
    QStandardItem* minItem;
    QStandardItem* maxItem;
    QString rangeType = Estimation_Tab1_MinMaxCMB->currentText();
    // Column number of parameters that have min/max values associated with them
    // Update this as necessary when implementing supplemental parameters.
    std::vector<int> parameters = {
        m_ColumnMap["InitBiomass"],
        m_ColumnMap["GrowthRate"],
        m_ColumnMap["GrowthRateShape"],
        m_ColumnMap["SpeciesK"],
        m_ColumnMap["SurveyQ"],
        m_ColumnMap["Catchability"]};
    QLocale locale(QLocale::English);
    QString valueWithoutComma;
    QString valueWithComma;
    double newValue;

    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab1_SpeciesPopulationTV);

    int numRows = m_SpeciesModel->rowCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col : parameters) {
                index    = m_SpeciesModel->index(row,col);
                valueWithoutComma = index.data().toString().remove(",");
                if ((rangeType == "min/max") || (rangeType == "min only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    minItem = new QStandardItem(valueWithComma);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col+1,minItem);
                }
                if ((rangeType == "min/max") || (rangeType == "max only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    maxItem = new QStandardItem(valueWithComma);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col+2,maxItem);
                }
            }
        }
    } else {
        // Find all columns with a selection
        for (QModelIndex index : indexes) {
            row = index.row();
            col = index.column();
            switch (col) {
                case 3:
                case 6:
                case 9:
                case 12:
                case 15:
                case 18:
                    index = m_SpeciesModel->index(row,col-1);
                    valueWithoutComma = index.data().toString().remove(",");
                    newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    minItem = new QStandardItem(valueWithComma);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,minItem);
                    break;
                case 4:
                case 7:
                case 10:
                case 13:
                case 16:
                case 19:
                    index    = m_SpeciesModel->index(row,col-2);
                    valueWithoutComma = index.data().toString().remove(",");
                    newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    maxItem = new QStandardItem(valueWithComma);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,maxItem);
                    break;
                default:
                    break;
            }
        }
        nmfUtilsQt::reselectTableViewCells(Estimation_Tab1_SpeciesPopulationTV,indexes);
    }
    Estimation_Tab1_SpeciesPopulationTV->resizeColumnsToContents();
}

/*
void
nmfEstimation_Tab1::callback_SpeciesRangeSB(int pct)
{
    // Change parameter min/max values for the selected parameters or if none
    // are selected, for all parameters that have min/max values.
    int col;
    int row;
    int eeFactor;
    double pctVal = double(pct)/100.0;
    QModelIndex index;
    std::set<int> selectedParameters;
    QStandardItem* minItem;
    QStandardItem* maxItem;
    QString rangeType = Estimation_Tab1_MinMaxCMB->currentText();
    // Column number of parameters that have min/max values associated with them
    // Update this as necessary when implementing supplemental parameters.
    std::vector<int> parameters = {1,4,8,12,15};  // RSK - shouldn't be hard-coded.
    QLocale locale(QLocale::English);
    QString valueWithoutComma;
    QString valueWithComma;
    double newValue;

    QModelIndexList indexes = nmfUtilsQt::getSelectedTableViewCells(Estimation_Tab1_SpeciesPopulationTV);

    int numRows = m_SpeciesModel->rowCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col : parameters) {
                index    = m_SpeciesModel->index(row,col);
                valueWithoutComma = index.data().toString().remove(",");
                if ((rangeType == "min/max") || (rangeType == "min only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    minItem = new QStandardItem(valueWithComma);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col+1,minItem);
                }
                if ((rangeType == "min/max") || (rangeType == "max only")) {
                    newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    maxItem = new QStandardItem(valueWithComma);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col+2,maxItem);
                }
            }
        }
    } else {
        // Find all columns with a selection
        for (QModelIndex index : indexes) {
            row = index.row();
            col = index.column();
            switch (col) {
                case 2:
                case 5:
                case 9:
                case 13:
                case 16:
                    index = m_SpeciesModel->index(row,col-1);
                    valueWithoutComma = index.data().toString().remove(",");
                    newValue = valueWithoutComma.toDouble()*(1.0-pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    minItem = new QStandardItem(valueWithComma);
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,minItem);
                    break;
                case 3:
                case 6:
                case 10:
                case 14:
                case 17:
                    index    = m_SpeciesModel->index(row,col-2);
                    valueWithoutComma = index.data().toString().remove(",");
                    newValue = valueWithoutComma.toDouble()*(1.0+pctVal);
                    eeFactor = (newValue < 1e-4) ? -1 : 1;
                    valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                newValue,m_NumSignificantDigits,eeFactor*6);
                    maxItem = new QStandardItem(valueWithComma);
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,maxItem);
                    break;
                default:
                    break;
            }
        }
        nmfUtilsQt::reselectTableViewCells(Estimation_Tab1_SpeciesPopulationTV,indexes);
    }
    Estimation_Tab1_SpeciesPopulationTV->resizeColumnsToContents();
}
*/

void
nmfEstimation_Tab1::readSettings()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::readSettings");

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}

