
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
    Estimation_Tab1_RestorePB           = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_RestorePB");
    Estimation_Tab1_SavePB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_SavePB");
    Estimation_Tab1_ModifySL            = Estimation_Tabs->findChild<QSlider     *>("Estimation_Tab1_ModifySL");
    Estimation_Tab1_ModifyRunPB         = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ModifyRunPB");
    Estimation_Tab1_ModifyRunCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_ModifyRunCB");
    Estimation_Tab1_SpeciesSuppCB       = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_SpeciesSuppCB");
    Estimation_Tab1_GuildSuppCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_GuildSuppCB");
    Estimation_Tab1_SpeciesRangeCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_SpeciesRangeCB");
    Estimation_Tab1_SpeciesRangeCMB     = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab1_SpeciesRangeCMB");
    Estimation_Tab1_SpeciesRangeSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab1_SpeciesRangeSB");
    Estimation_Tab1_GuildRangeCB        = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab1_GuildRangeCB");
    Estimation_Tab1_ModifySL->setValue(m_StartPosSL); // Set midpoint position to start

    //nmfTableView* Estimation_Tab1_PopulationTV = new nmfTableView();
    QFont noBoldFont;
    noBoldFont.setBold(false);
    Estimation_Tab1_SpeciesPopulationTV->setFont(noBoldFont);

    connect(Estimation_Tab1_PopulationTabW, SIGNAL(currentChanged(int)),
            this,                           SLOT(callback_CurrentTabChanged(int)));
    connect(Estimation_Tab1_NextPB,         SIGNAL(clicked()),
            this,                           SLOT(callback_NextPB()));
    connect(Estimation_Tab1_LoadPB,         SIGNAL(clicked()),
            this,                           SLOT(callback_LoadPB()));
    connect(Estimation_Tab1_ImportPB,       SIGNAL(clicked()),
            this,                           SLOT(callback_ImportPB()));
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

} // end constructor


nmfEstimation_Tab1::~nmfEstimation_Tab1()
{

}

bool
nmfEstimation_Tab1::checkAndShowEmptyFieldError(bool showPopup)
{
    m_Logger->logMsg(nmfConstants::Error,"Found empty field.");
    if (showPopup) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(Estimation_Tabs, "Error",
                             "\nError: Found empty field. Please fill in all required fields for the model desired.\n",
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
    Estimation_Tab1_ModifySL->setEnabled(isSpeciesTab);
    Estimation_Tab1_ModifyRunPB->setEnabled(isSpeciesTab);
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

    sliderValue -= m_StartPosSL;

    // Modify selected data
    foreach (const QModelIndex &index, m_selIndexes) {
        origValue  = m_originalValuesSelected[i++];
        tableValue = origValue + origValue*(sliderValue/50.0);
        item = new QStandardItem(QString::number(tableValue,'f',2));
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
    // Store the selected indexes in class variable
    getSelectedIndexes();

    // Save the values of the selected indexes in case user wants to undo mods from last save
    m_originalValuesSelected.clear();
    foreach (const QModelIndex &index, m_selIndexes){
        m_originalValuesSelected.push_back(index.data(Qt::DisplayRole).toDouble());
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
    loadWidgets();
}

void
nmfEstimation_Tab1::callback_RestorePB()
{
    int m = 0;
    std::string msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab1_SpeciesPopulationTV->model());
    QStandardItem *item;

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
            item = new QStandardItem(m_originalSpeciesValuesAll[m++]);
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
    QString errorMsg;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QTableView* tableView = (onGuildTab()) ? Estimation_Tab1_GuildPopulationTV :
                                             Estimation_Tab1_SpeciesPopulationTV;

    bool loadOK = nmfUtilsQt::loadTimeSeries(
                Estimation_Tabs, tableView, inputDataPath, "",
                nmfConstantsMSSPM::FirstLineNotReadOnly,errorMsg);
    if (! loadOK) {
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }

    // Load signal with data to send to Species Setup tab
    QList<QString> SpeNames;
    QList<QString> InitBiomass;
    QList<QString> GrowthRate;
    QList<QString> SpeciesK;
    for (int row=0; row<m_SpeciesModel->rowCount(); ++row) {
        SpeNames.push_back(   m_SpeciesModel->item(row,nmfConstantsMSSPM::Column_Supp_SpeName)->text());
        InitBiomass.push_back(m_SpeciesModel->item(row,nmfConstantsMSSPM::Column_Supp_InitBiomass)->text());
        GrowthRate.push_back( m_SpeciesModel->item(row,nmfConstantsMSSPM::Column_Supp_GrowthRate)->text());
        SpeciesK.push_back(   m_SpeciesModel->item(row,nmfConstantsMSSPM::Column_Supp_SpeciesK)->text());
    }
    emit UpdateSpeciesSetupData(SpeNames,InitBiomass,GrowthRate,SpeciesK);
}

void
nmfEstimation_Tab1::callback_SavePB()
{
    bool ok = true;
    std::string msg = "\n";
    std::string type = (onGuildTab()) ? "Guilds" : "Species";

    // Show warning
    QMessageBox::StandardButton reply;
    msg += "Are you sure you want to overwrite current "+type+" settings\n";
    msg += "with current Population Parameters? This cannot be undone.\n";
    reply = QMessageBox::question(Estimation_Tabs, tr("Overwrite Species"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }


    if (onGuildTab()) {
        ok = savePopulationParametersGuilds(nmfConstantsMSSPM::ShowPopupError);
    } else { // if on Species tab
        ok = savePopulationParametersSpecies(nmfConstantsMSSPM::ShowPopupError);
    }
    if (ok) {
        loadWidgets();
    }
    resetSelection();

    saveCSVFile();

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
    Estimation_Tab1_ModifySL->blockSignals(true);
    Estimation_Tab1_ModifySL->setValue(50);
    Estimation_Tab1_ModifySL->blockSignals(false);
    Estimation_Tab1_ModifySL->repaint(); // so widget refreshes immediately
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
}

void
nmfEstimation_Tab1::showPrimaryColumns(QTableView* tv)
{
    // These columns are always shown for Species
    QString colName;
    QStringList SpeciesPrimary = {"SpeName","InitBiomass","GrowthRate","SpeciesK"};
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
nmfEstimation_Tab1::showSuppColumns(QTableView* tv,bool show)
{
    QString colName;
    QStringList SpeciesSupp = {"SurveyQ","GrowthRateCovarCoeff","SpeciesKCovarCoeff",
                              "Catchability","SpeDependence","ExploitationRate"};
    QStringList GuildSupp   = {"Catchability"};
    QStringList SuppColumns = (tv == Estimation_Tab1_SpeciesPopulationTV) ?
                                  SpeciesSupp : GuildSupp;

    for (int i = 0; i < tv->model()->columnCount(); ++i) {
        colName = tv->model()->headerData(i, Qt::Horizontal).toString();
        if (SuppColumns.contains(colName)) {
            tv->setColumnHidden(i,!show);
        }
    }
}

void
nmfEstimation_Tab1::showRangeColumns(QTableView* tv,bool show)
{
    QString     colName;
    QStringList SpeciesRange     = {"InitBiomassMin","InitBiomassMax",
                                    "GrowthRateMin","GrowthRateMax",
                                    "SpeciesKMin","SpeciesKMax"};
    QStringList SpeciesRangeSupp = {"SurveyQMin","SurveyQMax",
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
        cmd  = "UPDATE Guilds SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString();
        index = m_GuildModel->index(i,7);
        Catchability = index.data().toString();
        cmd += "Catchability=" + Catchability;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,Catchability})) {
            checkAndShowEmptyFieldError(showPopup);
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

    // Save Guild range fields
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE Guilds SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString();
        index = m_GuildModel->index(i,1);
        GrowthRate = index.data().toString();
        index = m_GuildModel->index(i,2);
        GrowthRateMin = index.data().toString();
        cmd += "GrowthRateMin=" + GrowthRateMin + ",";
        index = m_GuildModel->index(i,3);
        GrowthRateMax = index.data().toString();
        cmd += "GrowthRateMax=" + GrowthRateMax + ",";
        index = m_GuildModel->index(i,4);
        GuildK = index.data().toString();
        index = m_GuildModel->index(i,5);
        GuildKMin = index.data().toString();
        cmd += "GuildKMin=" + GuildKMin + ",";
        index = m_GuildModel->index(i,6);
        GuildKMax = index.data().toString();
        cmd += "GuildKMax=" + GuildKMax;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,GrowthRateMin,GrowthRateMax,GuildKMin,GuildKMax})) {
            checkAndShowEmptyFieldError(showPopup);
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
        cmd  = "UPDATE Guilds SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString();
        index = m_GuildModel->index(i,7);
        Catchability = index.data().toString();
        index = m_GuildModel->index(i,8);
        CatchabilityMin = index.data().toString();
        cmd += "CatchabilityMin=" + CatchabilityMin + ",";
        index = m_GuildModel->index(i,9);
        CatchabilityMax = index.data().toString();
        cmd += "CatchabilityMax=" + CatchabilityMax;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,CatchabilityMin,CatchabilityMax})) {
            checkAndShowEmptyFieldError(showPopup);
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

    // Save the default (i.e. non-range and non-supplemental) data
    for (int i=0; i<m_GuildModel->rowCount(); ++i) {
        cmd  = "UPDATE Guilds SET ";
        index = m_GuildModel->index(i,0);
        GuildName = index.data().toString();
        index = m_GuildModel->index(i,1);
        GrowthRate = index.data().toString();
        cmd += "GrowthRate=" + GrowthRate + ",";
        index = m_GuildModel->index(i,4);
        GuildK = index.data().toString();
        cmd += "GuildK=" + GuildK;
        cmd += " WHERE GuildName = '" + GuildName + "'";
        if (nmfUtilsQt::emptyField({GuildName,GrowthRate,GuildK})) {
            checkAndShowEmptyFieldError(showPopup);
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
nmfEstimation_Tab1::saveSpeciesDataPrimary(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString SpeName,InitBiomass;
    QString GrowthRate;
    QString SpeciesK;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE Species SET ";
        index = m_SpeciesModel->index(i,0);
        SpeName = index.data().toString();
        index = m_SpeciesModel->index(i,1);
        InitBiomass = index.data().toString();
        cmd += "InitBiomass=" + InitBiomass + ",";
        index = m_SpeciesModel->index(i,4);
        GrowthRate = index.data().toString();
        cmd += "GrowthRate=" + GrowthRate + ",";
        index = m_SpeciesModel->index(i,8);
        SpeciesK = index.data().toString();
        cmd += "SpeciesK=" + SpeciesK;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,InitBiomass,GrowthRate,SpeciesK})) {
            checkAndShowEmptyFieldError(showPopup);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataPrimary: Write table error: " + errorMsg);
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
nmfEstimation_Tab1::saveSpeciesDataSupplemental(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString SpeName;
    QString GrowthRateCovarCoeff;
    QString SpeciesKCovarCoeff;
    QString SurveyQ;
    QString Catchability;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE Species SET ";
        index = m_SpeciesModel->index(i,0);
        SpeName = index.data().toString();
        index = m_SpeciesModel->index(i,7);
        GrowthRateCovarCoeff = index.data().toString();
        cmd += "GrowthRateCovarCoeff=" + GrowthRateCovarCoeff + ",";
        index = m_SpeciesModel->index(i,11);
        SpeciesKCovarCoeff = index.data().toString();
        cmd += "SpeciesKCovarCoeff=" + SpeciesKCovarCoeff + ",";
        index = m_SpeciesModel->index(i,12);
        SurveyQ = index.data().toString();
        cmd += "SurveyQ=" + SurveyQ + ",";
        index = m_SpeciesModel->index(i,15);
        Catchability = index.data().toString();
        cmd += "Catchability=" + Catchability;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,GrowthRateCovarCoeff,SpeciesKCovarCoeff,SurveyQ,Catchability})) {
            checkAndShowEmptyFieldError(showPopup);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataSupplemental: Write table error: " + errorMsg);
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
nmfEstimation_Tab1::saveSpeciesDataRange(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString BadParameter;
    QString SpeName,InitBiomass,InitBiomassMin,InitBiomassMax;
    QString GrowthRate,GrowthRateMin,GrowthRateMax;
    QString SpeciesK,SpeciesKMin,SpeciesKMax;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE Species SET ";
        index = m_SpeciesModel->index(i,0);
        SpeName = index.data().toString();
        index = m_SpeciesModel->index(i,1);
        InitBiomass = index.data().toString();
        index = m_SpeciesModel->index(i,2);
        InitBiomassMin = index.data().toString();
        cmd += "InitBiomassMin=" + InitBiomassMin + ",";
        index = m_SpeciesModel->index(i,3);
        InitBiomassMax = index.data().toString();
        cmd += "InitBiomassMax=" + InitBiomassMax + ",";
        index = m_SpeciesModel->index(i,4);
        GrowthRate = index.data().toString();
        index = m_SpeciesModel->index(i,5);
        GrowthRateMin = index.data().toString();
        cmd += "GrowthRateMin=" + GrowthRateMin + ",";
        index = m_SpeciesModel->index(i,6);
        GrowthRateMax = index.data().toString();
        cmd += "GrowthRateMax=" + GrowthRateMax + ",";
        index = m_SpeciesModel->index(i,8);
        SpeciesK = index.data().toString();
        index = m_SpeciesModel->index(i,9);
        SpeciesKMin = index.data().toString();
        cmd += "SpeciesKMin=" + SpeciesKMin + ",";
        index = m_SpeciesModel->index(i,10);
        SpeciesKMax = index.data().toString();
        cmd += "SpeciesKMax=" + SpeciesKMax;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,InitBiomassMin,InitBiomassMax,
                                    GrowthRateMin,GrowthRateMax,
                                    SpeciesKMin,SpeciesKMax})) {
            checkAndShowEmptyFieldError(showPopup);
            return false;
        }
        if (nmfUtilsQt::outOfRange({"InitBiomass",InitBiomass,InitBiomassMin,InitBiomassMax,
                                    "GrowthRate",GrowthRate,GrowthRateMin,GrowthRateMax,
                                    "SpeciesK",SpeciesK,SpeciesKMin,SpeciesKMax},BadParameter)) {
            checkAndShowOutOfRangeError("Species",SpeName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataRange: Write table error: " + errorMsg);
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
nmfEstimation_Tab1::saveSpeciesDataSupplementalAndRange(bool showPopup)
{
    std::string errorMsg;
    QString cmd;
    QString BadParameter;
    QString SpeName;
    QString SurveyQ,SurveyQMin,SurveyQMax;
    QString Catchability,CatchabilityMin,CatchabilityMax;
    QModelIndex index;

    for (int i=0; i<m_SpeciesModel->rowCount(); ++i) {
        cmd  = "UPDATE Species SET ";
        index = m_SpeciesModel->index(i,0);
        SpeName = index.data().toString();
        index = m_SpeciesModel->index(i,12);
        SurveyQ = index.data().toString();
        index = m_SpeciesModel->index(i,13);
        SurveyQMin = index.data().toString();
        cmd += "SurveyQMin=" + SurveyQMin + ",";
        index = m_SpeciesModel->index(i,14);
        SurveyQMax = index.data().toString();
        cmd += "SurveyQMax=" + SurveyQMax + ",";
        index = m_SpeciesModel->index(i,15);
        Catchability = index.data().toString();
        index = m_SpeciesModel->index(i,16);
        CatchabilityMin = index.data().toString();
        cmd += "CatchabilityMin=" + CatchabilityMin + ",";
        index = m_SpeciesModel->index(i,17);
        CatchabilityMax = index.data().toString();
        cmd += "CatchabilityMax=" + CatchabilityMax;
        cmd += " WHERE SpeName = '" + SpeName + "'";
        if (nmfUtilsQt::emptyField({SpeName,SurveyQMin,SurveyQMax,CatchabilityMin,CatchabilityMax})) {
            checkAndShowEmptyFieldError(showPopup);
            return false;
        }
        if (nmfUtilsQt::outOfRange({"SurveyQ",SurveyQ,SurveyQMin,SurveyQMax,
                                    "Catchability",Catchability,CatchabilityMin,CatchabilityMax},BadParameter)) {
            checkAndShowOutOfRangeError("Species",SpeName,BadParameter,nmfConstantsMSSPM::ShowPopupError);
            return false;
        }
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd.toStdString());
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 saveSpeciesDataSupplementalAndRange: Write table error: " + errorMsg);
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
nmfEstimation_Tab1::savePopulationParametersSpecies(bool showPopup)
{
    if (isChecked(Estimation_Tab1_SpeciesSuppCB) &&
        isChecked(Estimation_Tab1_SpeciesRangeCB)) {
        if (! saveSpeciesDataSupplementalAndRange(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_SpeciesSuppCB)) {
        if (! saveSpeciesDataSupplemental(showPopup)) {
            return false;
        }
    }

    if (isChecked(Estimation_Tab1_SpeciesRangeCB)) {
        if (! saveSpeciesDataRange(showPopup)) {
            return false;
        }
    }

    if (! saveSpeciesDataPrimary(showPopup)) {
        return false;
    }

    emit ReloadSpecies(showPopup);

    if (showPopup) {
        QMessageBox::information(Estimation_Tabs, "Species Updated",
                                 "\nSpecies table has been successfully updated.\n",
                                 QMessageBox::Ok);
    }

    return true;
}

void
nmfEstimation_Tab1::callback_SaveCSVFile()
{
    saveCSVFile();
}

void
nmfEstimation_Tab1::saveCSVFile()
{
    QString msg;

    // Save time series data to a .csv file
    std::string tableName = "Species";
    msg = "\nOK to use default file name for Species .csv file and overwrite any previous file?";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Default Species CSV File"),
                                                              tr(msg.toLatin1()),
                                                              QMessageBox::No|QMessageBox::Yes,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        saveCSVFile(tableName);
    } else {
        bool ok;
        QString tag = QInputDialog::getText(Estimation_Tabs, tr("Species Files"),
                                            tr("Enter Species CSV filename version tag (omit any '_'): "),
                                            QLineEdit::Normal, "", &ok);
        if (ok && !tag.isEmpty()) {
            tableName += "_"+tag.toStdString();
            saveCSVFile(tableName);
        }
    }
}

void
nmfEstimation_Tab1::saveCSVFile(std::string tableName)
{
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString tableNameStr = QDir(inputDataPath).filePath(QString::fromStdString(tableName));
    QStandardItemModel* smodel = (onGuildTab()) ? m_GuildModel : m_SpeciesModel;

    nmfUtilsQt::saveTimeSeries(Estimation_Tabs,smodel,inputDataPath,tableNameStr);

    QFileInfo fileInfo(tableNameStr);
    if (fileInfo.suffix().toLower() != "csv") {
        tableNameStr += ".csv";
    }
    QMessageBox::information(Estimation_Tabs, "File Saved",
                             "\nSpecies CSV file has been successfully saved:\n\n"+tableNameStr+"\n",
                             QMessageBox::Ok);
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
    smodel->horizontalHeaderItem( 0)->setToolTip("Species Name");
    smodel->horizontalHeaderItem( 1)->setToolTip("Initial Biomass");
    smodel->horizontalHeaderItem( 2)->setToolTip("Initial Biomass Minimum Value");
    smodel->horizontalHeaderItem( 3)->setToolTip("Initial Biomass Maximum Value");
    smodel->horizontalHeaderItem( 4)->setToolTip("Initial Growth Rate");
    smodel->horizontalHeaderItem( 5)->setToolTip("Minimum Growth Rate");
    smodel->horizontalHeaderItem( 6)->setToolTip("Maximum Growth Rate");
    smodel->horizontalHeaderItem( 8)->setToolTip("Initial Carrying Capacity");
    smodel->horizontalHeaderItem( 9)->setToolTip("Minimum Carrying Capacity");
    smodel->horizontalHeaderItem(10)->setToolTip("Maximum Carrying Capacity");

    msg  = "</html><strong><center>Species Name</center></strong><br>";
    msg += "The Species name entered must be unique.";
    smodel->horizontalHeaderItem(0)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Initial Biomass</center></strong><br>";
    msg += "The initial species biomass is in units of metric tons.";
    smodel->horizontalHeaderItem(1)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Initial Biomass</center></strong><br>";
    msg += "The minimum initial biomass must be less than or equal to the initial biomass.";
    smodel->horizontalHeaderItem(2)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Initial Biomass</center></strong><br>";
    msg += "The maximum initial biomass must be greater than or equal to the initial biomass.";
    smodel->horizontalHeaderItem(3)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Growth Rate</center></strong><br>";
    msg += "The Species growth rate (r) is a unit-less value typically between 0.0 and 1.0.";
    smodel->horizontalHeaderItem(4)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Growth Rate</center></strong><br>";
    msg += "The minimum growth rate must be less than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(5)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Growth Rate</center></strong><br>";
    msg += "The maximum growth rate must be greater than or equal to the initial growth rate";
    smodel->horizontalHeaderItem(6)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Species K</center></strong><br>";
    msg += "The Species carrying capacity (K) is the number of individuals<br>";
    msg += "in a population that can be supported by the habitat's resources.";
    smodel->horizontalHeaderItem(8)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Species K</center></strong><br>";
    msg += "The minimum carrying capacity (K) must be less than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(9)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Species K</center></strong><br>";
    msg += "The maximum carrying capacity (K) must be greater than or equal to the initial carrying capacity.";
    smodel->horizontalHeaderItem(10)->setWhatsThis(prefix+msg+suffix);

    msg  = "</html><strong><center>Catchability</center></strong><br>";
    msg += "The catchability (q) refers to the efficiency of the fishery. This coefficient";
    msg += "relates the biomass to the fishing mortality.";
    smodel->horizontalHeaderItem(15)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Minimum Catchability</center></strong><br>";
    msg += "The minimum catchability must be less than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(16)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Maximum Catchability</center></strong><br>";
    msg += "The maximum catchability must be greater than or equal to the initial catchability.";
    smodel->horizontalHeaderItem(17)->setWhatsThis(prefix+msg+suffix);
}


bool
nmfEstimation_Tab1::loadWidgets()
{
    bool okSpecies = loadSpecies();
    bool okGuilds  = loadGuilds();

    resetVisibleColumns();
    if (okSpecies) {
        setupHelpSpecies();
    }
    if (okGuilds) {
        setupHelpGuilds();
    }
    return true;
}

bool
nmfEstimation_Tab1::loadGuilds()
{
    int j;
    int NumGuilds;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString value;
    QStandardItem *item;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadGuilds()");

    // Load Population tableview
    fields = {"GuildName","GrowthRate","GrowthRateMin","GrowthRateMax",
              "GuildK","GuildKMin","GuildKMax","Catchability","CatchabilityMin","CatchabilityMax"};
    queryStr   = "SELECT GuildName,GrowthRate,GrowthRateMin,GrowthRateMax,"
                 "GuildK,GuildKMin,GuildKMax,Catchability,CatchabilityMin,CatchabilityMax FROM Guilds";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields, nmfConstantsMSSPM::ShowBlankFields);
    NumGuilds = dataMap["GuildName"].size();
    QStringList PopulationFieldList = {"GuildName","GrowthRate","GrowthRateMin","GrowthRateMax",
                                       "GuildK","GuildKMin","GuildKMax",
                                       "Catchability","CatchabilityMin","CatchabilityMax"};
//    m_originalSpeciesValuesAll.clear();
    m_GuildModel = new QStandardItemModel( NumGuilds, PopulationFieldList.size() );
    for (int i=0; i<NumGuilds; ++i) {
        j = 0;
        for (QString field : PopulationFieldList)
        {
            value = QString::fromStdString(dataMap[field.toStdString()][i]);
//            m_originalSpeciesValuesAll.push_back(value);
            item = new QStandardItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            m_GuildModel->setItem(i, j++, item);
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

    return (NumGuilds > 0);
}

// Load all Species as you'll be hiding some
bool
nmfEstimation_Tab1::loadSpecies()
{
    int j;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString value;
    QStandardItem *item;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::loadSpecies()");

    // Load Population tableview
    fields = {"SpeName","InitBiomass","InitBiomassMin","InitBiomassMax",
              "GrowthRate","GrowthRateMin","GrowthRateMax","GrowthRateCovarCoeff",
              "SpeciesK","SpeciesKMin","SpeciesKMax","SpeciesKCovarCoeff",
              "SurveyQ","SurveyQMin","SurveyQMax","Catchability","CatchabilityMin","CatchabilityMax"};
    queryStr   = "SELECT SpeName,InitBiomass,InitBiomassMin,InitBiomassMax,";
    queryStr  += "GrowthRate,GrowthRateMin,GrowthRateMax,GrowthRateCovarCoeff,";
    queryStr  += "SpeciesK,SpeciesKMin,SpeciesKMax,SpeciesKCovarCoeff,";
    queryStr  += "SurveyQ,SurveyQMin,SurveyQMax,Catchability,CatchabilityMin,";
    queryStr  += "CatchabilityMax FROM Species";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields, nmfConstantsMSSPM::ShowBlankFields);
    NumSpecies = dataMap["SpeName"].size();
    QStringList fieldList;


    m_originalSpeciesValuesAll.clear();
    m_SpeciesModel = new QStandardItemModel( NumSpecies, fields.size() );
    for (int i=0; i<NumSpecies; ++i) {
        j = 0;
        for (std::string field : fields)
        {
            if (i == 0) {
                fieldList << QString::fromStdString(field);
            }
            value = QString::fromStdString(dataMap[field][i]);
            m_originalSpeciesValuesAll.push_back(value);
            item = new QStandardItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            m_SpeciesModel->setItem(i, j++, item);
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

QModelIndexList
nmfEstimation_Tab1::getSelectedVisibleCells()
{
    QItemSelectionModel *selectionModel = Estimation_Tab1_SpeciesPopulationTV->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();
    return indexes;
}

void
nmfEstimation_Tab1::reselectVisibleCells(QModelIndexList indexes)
{
    if (indexes.size() > 0) {
        QItemSelectionModel *selectionModel = Estimation_Tab1_SpeciesPopulationTV->selectionModel();
        for (QModelIndex index : indexes) {
            selectionModel->select(index,QItemSelectionModel::Select);
        }
    }
}

void
nmfEstimation_Tab1::callback_SpeciesRangeCMB(QString value)
{
    Estimation_Tab1_SpeciesRangeSB->setEnabled(value == "Percentage");

    QModelIndexList indexes = getSelectedVisibleCells();

    if (value == "Percentage") {
        callback_SpeciesRangeSB(Estimation_Tab1_SpeciesRangeSB->value());
    } else {
        loadWidgets();
    }

    reselectVisibleCells(indexes);
}

void
nmfEstimation_Tab1::callback_SpeciesRangeSB(int pct)
{
    // Change parameter min/max values for the selected parameters or if none
    // are selected, for all parameters that have min/max values.
    int col;
    int row;
    double pctVal = pct/100.0;
    double parameterValue;
    QModelIndex index,minIndex,maxIndex;
    std::set<int> selectedParameters;
    QStandardItem* minItem;
    QStandardItem* maxItem;
    // Column number of parameters that have min/max values associated with them
    // Update this as necessary when implementing supplemental parameters.
    std::vector<int> parameters = {1,4,8};  // Probably shouldn't be hard-coded.

    QModelIndexList indexes = getSelectedVisibleCells();

    int numRows = m_SpeciesModel->rowCount();
    if (indexes.size() == 0) { // mean no selections, so set all min/max parameter values
        for (int row=0; row<numRows; ++row) {
            for (int col : parameters) {
                index    = m_SpeciesModel->index(row,col);
                minIndex = m_SpeciesModel->index(row,col+1);
                maxIndex = m_SpeciesModel->index(row,col+2);
                parameterValue = index.data().toDouble();
                minItem = new QStandardItem(QString::number(parameterValue*(1.0-pctVal)));
                minItem->setTextAlignment(Qt::AlignCenter);
                maxItem = new QStandardItem(QString::number(parameterValue*(1.0+pctVal)));
                maxItem->setTextAlignment(Qt::AlignCenter);
                m_SpeciesModel->setItem(row,col+1,minItem);
                m_SpeciesModel->setItem(row,col+2,maxItem);
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
                    index    = m_SpeciesModel->index(row,col-1);
                    minIndex = m_SpeciesModel->index(row,col);
                    parameterValue = index.data().toDouble();
                    minItem = new QStandardItem(QString::number(parameterValue*(1.0-pctVal)));
                    minItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,minItem);
                    break;
                case 3:
                case 6:
                case 10:
                    index    = m_SpeciesModel->index(row,col-2);
                    maxIndex = m_SpeciesModel->index(row,col);
                    parameterValue = index.data().toDouble();
                    maxItem = new QStandardItem(QString::number(parameterValue*(1.0+pctVal)));
                    maxItem->setTextAlignment(Qt::AlignCenter);
                    m_SpeciesModel->setItem(row,col,maxItem);
                    break;
                default:
                    break;
            }
        }
        reselectVisibleCells(indexes);
    }
}
