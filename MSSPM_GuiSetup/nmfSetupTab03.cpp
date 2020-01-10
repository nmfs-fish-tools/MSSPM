
#include "nmfSetupTab03.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

nmfSetup_Tab3::nmfSetup_Tab3(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    Setup_Tabs               = tabs;
    m_databasePtr            = databasePtr;
    m_logger                 = logger;
    m_ProjectDir             = projectDir;
    m_smodelSpecies          = nullptr;
    m_smodelOtherPredSpecies = nullptr;
    m_colLabelsSpecies.clear();
    m_colLabelsGuilds.clear();

    readSettings();

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab03.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab3_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab3_Widget->setObjectName("Setup_Tab3_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab3_Widget, tr("3. Species Setup"));

    Setup_Tab3_GuildsTW          = Setup_Tabs->findChild<QTableWidget *>("Setup_Tab3_GuildsTW");
    Setup_Tab3_SpeciesTW         = Setup_Tabs->findChild<QTableWidget *>("Setup_Tab3_SpeciesTW");
    Setup_Tab3_NumGuildsSB       = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab3_NumGuildsSB");
    Setup_Tab3_AddGuildPB        = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_AddGuildPB");
    Setup_Tab3_DelGuildPB        = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_DelGuildPB");
    Setup_Tab3_ReloadGuildsPB    = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ReloadGuildsPB");
    Setup_Tab3_NumSpeciesSB      = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab3_NumSpeciesSB");
    Setup_Tab3_AddSpeciesPB      = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_AddSpeciesPB");
    Setup_Tab3_DelSpeciesPB      = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_DelSpeciesPB");
    Setup_Tab3_ReloadSpeciesPB   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ReloadSpeciesPB");
    Setup_Tab3_SavePB            = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_SavePB");
    Setup_Tab3_PrevPB            = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_PrevPB");
    Setup_Tab3_UpdateSpeciesPB   = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_UpdateSpeciesPB");
    Setup_Tab3_GuildsSpeciesTabW = Setup_Tabs->findChild<QTabWidget   *>("Setup_Tab3_GuildsSpeciesTabW");

    connect(Setup_Tab3_NumGuildsSB,     SIGNAL(valueChanged(int)),
            this,                       SLOT(callback_Setup_Tab3_NumGuilds(int)));
    connect(Setup_Tab3_AddGuildPB,      SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_AddGuildPB()));
    connect(Setup_Tab3_DelGuildPB,      SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_DelGuildPB()));
    connect(Setup_Tab3_ReloadGuildsPB,  SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_ReloadGuildsPB()));
    connect(Setup_Tab3_NumSpeciesSB,    SIGNAL(valueChanged(int)),
            this,                       SLOT(callback_Setup_Tab3_NumSpecies(int)));
    connect(Setup_Tab3_AddSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_AddSpeciesPB()));
    connect(Setup_Tab3_DelSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_DelSpeciesPB()));
    connect(Setup_Tab3_ReloadSpeciesPB, SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_ReloadSpeciesPB()));
    connect(Setup_Tab3_SavePB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_SavePB()));
    connect(Setup_Tab3_PrevPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_PrevPB()));
    connect(Setup_Tab3_SpeciesTW,       SIGNAL(cellClicked(int,int)),
            this,                       SLOT(callback_Setup_Tab3_SpeciesTableChanged(int,int)));
    connect(Setup_Tab3_GuildsTW,        SIGNAL(cellClicked(int,int)),
            this,                       SLOT(callback_Setup_Tab3_GuildsTableChanged(int,int)));
    connect(Setup_Tab3_UpdateSpeciesPB, SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_UpdateSpeciesPB()));

    Setup_Tab3_PrevPB->setText("\u25C1--");
    Setup_Tab3_SavePB->setEnabled(true);
    Setup_Tab3_GuildsTW->setSelectionMode(QAbstractItemView::ContiguousSelection);
    Setup_Tab3_SpeciesTW->setSelectionMode(QAbstractItemView::ContiguousSelection);

    m_colLabelsSpecies << "Name" << "Guild" << "Initial Biomass" << "Initial Biomas Min" << "Initial Biomass Max"
                       << "Growth Rate" << "Growth Rate Min" << "Growth Rate Max" << "Growth Rate Covar Coeff"
                       << "Species K" << "Species K Min" << "Species K Max" << "Species K Covar Coeff"
                       << "Survey Q" << "Survey Q Min" << "Survey Q Max"
                       << "CatchabilityMin" << "CatchabilityMax" << "Dependence";

    m_colLabelsGuilds << "Guild Name" << "Growth Rate" << "Growth Rate Min" << "Growth Rate Max"
                      << "Carrying Capacity" << "Carrying Capacity Min" << "Carrying Capacity Max"
                      << "Catchability Min" << "Catchability Max";
}


nmfSetup_Tab3::~nmfSetup_Tab3()
{
}


void
nmfSetup_Tab3::callback_Setup_Tab3_ReloadGuildsPB()
{
    loadGuilds();

    QMessageBox::information(Setup_Tabs,"Reload",
                             "\nGuilds reloaded.\n",
                             QMessageBox::Ok);
}

void
nmfSetup_Tab3::callback_Setup_Tab3_ReloadSpeciesPB()
{
    loadSpecies();

    QMessageBox::information(Setup_Tabs,"Reload",
                             "\nSpecies reloaded.\n",
                             QMessageBox::Ok);
}


void
nmfSetup_Tab3::callback_Setup_Tab3_DelGuildPB()
{
//  QList<QString> TablesToDeleteFrom = {"Guilds"};

    // Delete row(s) from table.  If the Species has been saved, this should happen automatically when
    // the Species are reloaded.  However, if the user is just entering the data and hasn't yet
    // saved and wants to delete a row, this statement is necessary.
    QList<QTableWidgetItem*> selItems = Setup_Tab3_GuildsTW->selectedItems();
    int numSelItems = selItems.size();
    if (numSelItems == 0) {
        return;
    }
    int firstSelRow = selItems[0]->row();
    int lastSelRow  = selItems[numSelItems-1]->row();
    if (firstSelRow > lastSelRow) {
        std::swap(firstSelRow,lastSelRow);
    }
    int firstRow = firstSelRow;
    int numRowsToDelete = lastSelRow - firstSelRow + 1;
    for (int i=0; i<numRowsToDelete; ++i) {
//      removeFromTable(Setup_Tab3_GuildsTW->item(firstRow,0),TablesToDeleteFrom); // Remove from database table
        Setup_Tab3_GuildsTW->removeRow(firstRow);// Remove from widget  (Refactor this with a model!!)
    }

    // Enable spin box if there are 0 rows left
    Setup_Tab3_NumGuildsSB->setEnabled(Setup_Tab3_GuildsTW->rowCount() == 0);
}


void
nmfSetup_Tab3::callback_Setup_Tab3_DelSpeciesPB()
{
    QList<QString> TablesToDeleteFrom = {"Species","Catch",
                                         "Effort","ObservedBiomass"};

    // Delete row(s) from table.  If the Species has been saved, this should happen automatically when
    // the Species are reloaded.  However, if the user is just entering the data and hasn't yet
    // saved and wants to delete a row, this statement is necessary.
    QList<QTableWidgetItem*> selItems = Setup_Tab3_SpeciesTW->selectedItems();
    int numSelItems = selItems.size();
    if (numSelItems == 0) {
        return;
    }
    int firstSelRow = selItems[0]->row();
    int lastSelRow  = selItems[numSelItems-1]->row();
    if (firstSelRow > lastSelRow) {
        std::swap(firstSelRow,lastSelRow);
    }
    int firstRow = firstSelRow;
    int numRowsToDelete = lastSelRow - firstSelRow + 1;
    for (int i=0; i<numRowsToDelete; ++i) {
//      removeFromTable(Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom); // Remove from database table
        Setup_Tab3_SpeciesTW->removeRow(firstRow);// Remove from widget  (Refactor this with a model!!)
    }

    // Enable spin box if there are 0 rows left
    Setup_Tab3_NumSpeciesSB->setEnabled(Setup_Tab3_SpeciesTW->rowCount() == 0);

} // end callback_Setup_Tab3_DelSpeciesPB

void
nmfSetup_Tab3::removeFromTable(QTableWidgetItem *itemToRemove,
                               QList<QString>& TablesToDeleteFrom)
{
    std::string cmd;
    std::string field;
    std::string errorMsg;
    QString SpeciesOrGuildName;

    if (itemToRemove != NULL) {
        SpeciesOrGuildName = itemToRemove->text();
        if (! SpeciesOrGuildName.isEmpty()) {
            for (QString Table : TablesToDeleteFrom)
            {
                field = (Table == "Guilds") ? "GuildName" : "SpeName";
                cmd = "DELETE FROM " + Table.toStdString() +" WHERE " + field +
                      " = '" + SpeciesOrGuildName.toStdString() + "'";
                errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
                if (errorMsg != " ") {
                    m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 removeFromTable: Delete table error: " + errorMsg);
                    m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                    return;
                }
            }
        }
    }
}

void
nmfSetup_Tab3::callback_Setup_Tab3_AddGuildPB()
{
    int numRows = Setup_Tab3_GuildsTW->rowCount();
    int numCols = numColumnsGuilds();

    if (numRows == 0) {
        Setup_Tab3_GuildsTW->setColumnCount(numCols);
        Setup_Tab3_GuildsTW->setHorizontalHeaderLabels(m_colLabelsGuilds);
        Setup_Tab3_GuildsTW->resizeColumnsToContents();
    }

    Setup_Tab3_GuildsTW->insertRow(numRows);
    populateARowGuilds(numRows,numCols);
}



void
nmfSetup_Tab3::callback_Setup_Tab3_AddSpeciesPB()
{
    int numRows = Setup_Tab3_SpeciesTW->rowCount();
    int numCols = numColumnsSpecies();

    if (numRows == 0) {
        Setup_Tab3_SpeciesTW->setColumnCount(numCols);
        Setup_Tab3_SpeciesTW->setHorizontalHeaderLabels(m_colLabelsSpecies);
        Setup_Tab3_SpeciesTW->resizeColumnsToContents();
    }

    Setup_Tab3_SpeciesTW->insertRow(numRows);
    populateARowSpecies(numRows,numColumnsSpecies());

} // end callback_Setup_Tab3_AddSpeciesPB


void
nmfSetup_Tab3::callback_Setup_Tab3_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}

bool
nmfSetup_Tab3::guildDataIsSaved()
{
    int NumGuilds;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get Guild data from database
    fields = {"GuildName","GrowthRate"};
    queryStr  = "SELECT GuildName,GrowthRate FROM Guilds";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();

    return (NumGuilds != 0);
}

void
nmfSetup_Tab3::callback_Setup_Tab3_SavePB()
{
    bool onGuildPage = (Setup_Tab3_GuildsSpeciesTabW->currentIndex() == 0);

    Setup_Tabs->setCursor(Qt::WaitCursor);

    if (onGuildPage) {
        saveGuildData();
    } else {
        if (guildDataIsSaved()) {
            saveSpeciesData();
        } else {
            QMessageBox::warning(Setup_Tabs,"Please Note",
                                 "\nGuild data must be saved prior to saving Species data.\n",
                                 QMessageBox::Ok);
            m_logger->logMsg(nmfConstants::Warning,"Please save Guild data prior to saving Species data.");
        }
    }

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab3::saveGuildData()
{
    int NumGuilds  = Setup_Tab3_GuildsTW->rowCount();
    std::string cmd;
    std::string errorMsg;
    std::string GuildName,GuildK;
    std::string GuildKMin,GuildKMax;
    std::string GrowthRate,GrowthRateMin,GrowthRateMax,CatchabilityMin,CatchabilityMax;
    double GuildKVal,GuildKMinVal,GuildKMaxVal;
    double GrowthRateVal,GrowthRateMinVal,GrowthRateMaxVal;
    std::vector<std::string> guilds;

    m_logger->logMsg(nmfConstants::Normal,"Saving Guild Data");

    if (! nmfUtilsQt::allCellsArePopulated(Setup_Tabs, Setup_Tab3_GuildsTW, true, false)) {
        m_logger->logMsg(nmfConstants::Error,"Missing fields in Guilds table");
        return;
    }

    // Guilds data integrity check
    // 1. Check Growth Rate Min < Growth Rate Max
    // 2. Check Growth Rate is between its Min,Max values
    // 3. Repeat above 2 steps for Carrying Capacity
    // If not, display error and return.
    for (int i=0; i<NumGuilds; ++i) {
        GuildName        = Setup_Tab3_GuildsTW->item(i,0)->text().toStdString();
        GrowthRateVal    = Setup_Tab3_GuildsTW->item(i,1)->text().toDouble();
        GrowthRateMinVal = Setup_Tab3_GuildsTW->item(i,2)->text().toDouble();
        GrowthRateMaxVal = Setup_Tab3_GuildsTW->item(i,3)->text().toDouble();
        GuildKVal        = Setup_Tab3_GuildsTW->item(i,4)->text().toDouble();
        GuildKMinVal     = Setup_Tab3_GuildsTW->item(i,5)->text().toDouble();
        GuildKMaxVal     = Setup_Tab3_GuildsTW->item(i,6)->text().toDouble();
        if (GrowthRateMinVal > GrowthRateMaxVal) {
            errorMsg = "\nFound: GrowthRateMinVal > GrowthRateMaxVal for Guild: " + GuildName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else if ((GrowthRateVal < GrowthRateMinVal) || (GrowthRateVal > GrowthRateMaxVal)) {
            errorMsg = "\nFound: GrowthRate value outside of its Min,Max range for Guild: " + GuildName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
        if (GuildKMinVal > GuildKMaxVal) {
            errorMsg = "\nFound: GuildKMinVal > GuildKMaxVal for Guild: " + GuildName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else if ((GuildKVal < GuildKMinVal) || (GuildKVal > GuildKMaxVal)) {
            errorMsg = "\nFound: GuildKVal value outside of its Min,Max range for Guild: " + GuildName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
    }

    // Delete current Guilds table
    cmd = "DELETE FROM Guilds";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::saveGuildData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Setup_Tabs, "Error",
                             "\nError in Save command. Couldn't delete all records from Guilds table.\n",
                             QMessageBox::Ok);
        return;
    }

    // Save Guilds table from table widget
    for (int i=0; i<NumGuilds; ++i) {
        GuildName       = Setup_Tab3_GuildsTW->item(i,0)->text().toStdString();
        GrowthRate      = Setup_Tab3_GuildsTW->item(i,1)->text().toStdString();
        GrowthRateMin   = Setup_Tab3_GuildsTW->item(i,2)->text().toStdString();
        GrowthRateMax   = Setup_Tab3_GuildsTW->item(i,3)->text().toStdString();
        GuildK          = Setup_Tab3_GuildsTW->item(i,4)->text().toStdString();
        GuildKMin       = Setup_Tab3_GuildsTW->item(i,5)->text().toStdString();
        GuildKMax       = Setup_Tab3_GuildsTW->item(i,6)->text().toStdString();
        CatchabilityMin = Setup_Tab3_GuildsTW->item(i,7)->text().toStdString();
        CatchabilityMax = Setup_Tab3_GuildsTW->item(i,8)->text().toStdString();
        cmd  = "INSERT INTO Guilds (GuildName,GrowthRate,GrowthRateMin,GrowthRateMax,GuildK,GuildKMin,GuildKMax,CatchabilityMin,CatchabilityMax) ";
        cmd += "VALUES ('" + GuildName + "'," + GrowthRate + "," + GrowthRateMin + "," + GrowthRateMax + "," +
                GuildK + "," + GuildKMin + "," + GuildKMax + "," +
                CatchabilityMin  + "," + CatchabilityMax + ");";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (Guilds): Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Setup_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO Guilds table.\n",
                                 QMessageBox::Ok);
            return;
        }
        guilds.push_back(GuildName);
    }

    loadGuilds();

    // Need to reload all other Estimation GUIs since Guilds may have changed
    emit ReloadWidgets();

    // Remove data from all tables with species different than what's in species
    pruneTablesForGuilds(guilds);

    QMessageBox::information(Setup_Tabs, "Save",
                             tr("\nGuild data saved.\n"));

}


void
nmfSetup_Tab3::saveSpeciesData()
{
    m_logger->logMsg(nmfConstants::Normal,"Saving Species Data");

    int NumSpecies = Setup_Tab3_SpeciesTW->rowCount();
    double systemK=0;
    std::string msg;
    std::string cmd;
    std::string errorMsg;
    std::string GuildName;
    std::string SpeciesName;
    std::string MohnsRhoLabel = "";
    std::string InitBiomass,SurveyQ,SpeciesK,SpeciesKCovarCoeff,GrowthRate,GrowthRateCovarCoeff;
    std::string InitBiomassMin,InitBiomassMax,SurveyQMin,SurveyQMax,GrowthRateMin;
    std::string GrowthRateMax,SpeciesKMin,SpeciesKMax,CatchabilityMin,CatchabilityMax,SpeDependence;
    std::map<std::string,double> guildKMap;
    QString value;
    bool okValue;
    double InitBiomassVal,InitBiomassMinVal,InitBiomassMaxVal;
    double GrowthRateVal,GrowthRateMinVal,GrowthRateMaxVal;
    double SpeciesKVal,SpeciesKMinVal,SpeciesKMaxVal;
    std::vector<std::string> species;

    // Check each cell for missing fields
    int numCols;
    std::vector<std::string> Tables = {"Species"};
    std::vector<int> NumSpeciesOrGuilds= {NumSpecies};
    int tableNum = -1;
    for (QTableWidget *tw : {Setup_Tab3_SpeciesTW}) {
        ++tableNum;
        numCols = tw->columnCount();
        for (int i=0; i<NumSpeciesOrGuilds[tableNum]; ++i) {
            for (int j=0; j<numCols; ++j) {
                if ((Tables[tableNum] == "Guilds") ||
                        ((Tables[tableNum] == "Species") && (j != 1))) // Skip the combobox in Species
                {
                    value = tw->item(i,j)->text();
                    if (value.isEmpty()) {
                        msg = "Missing field(s) in " + Tables[tableNum] + " table";
                        m_logger->logMsg(nmfConstants::Error,msg);
                        QMessageBox::warning(Setup_Tabs,"Warning",
                                             QString::fromStdString("\n"+msg+"\n"),
                                             QMessageBox::Ok);
                        return;
                    } else if (value.contains(',')) {
                        if (! okValue) {
                            msg = "Found an invalid numeric value of: " + value.toStdString();
                            msg += ". No commas or special characters allowed.";
                            m_logger->logMsg(nmfConstants::Error,msg);
                            QMessageBox::warning(Setup_Tabs,"Warning",
                                                 QString::fromStdString("\n"+msg+"\n"),
                                                 QMessageBox::Ok);
                            return;
                        }
                    }
                }
            }
        }
    }

    // Species Data Integrity Checks:
    // 1. Check to make sure that the SpeciesKMin values are > than the InitBiomass values
    // 2. Check min biomass < max biomass
    // 3. Check init biomass is between min and max
    // 4. Repeat above 3 steps for r and K
    // If not, display error and return.
    for (int i=0; i<NumSpecies; ++i) {
        SpeciesName       = Setup_Tab3_SpeciesTW->item(i, 0)->text().toStdString();
        InitBiomassVal    = Setup_Tab3_SpeciesTW->item(i, 2)->text().toDouble();
        InitBiomassMinVal = Setup_Tab3_SpeciesTW->item(i, 3)->text().toDouble();
        InitBiomassMaxVal = Setup_Tab3_SpeciesTW->item(i, 4)->text().toDouble();
        GrowthRateVal     = Setup_Tab3_SpeciesTW->item(i, 5)->text().toDouble();
        GrowthRateMinVal  = Setup_Tab3_SpeciesTW->item(i, 6)->text().toDouble();
        GrowthRateMaxVal  = Setup_Tab3_SpeciesTW->item(i, 7)->text().toDouble();
        SpeciesKVal       = Setup_Tab3_SpeciesTW->item(i, 9)->text().toDouble();
        SpeciesKMinVal    = Setup_Tab3_SpeciesTW->item(i,10)->text().toDouble();
        SpeciesKMaxVal    = Setup_Tab3_SpeciesTW->item(i,11)->text().toDouble();
        if (InitBiomassVal > SpeciesKMinVal) {
            errorMsg = "\nFound: SpeciesKMin < InitBiomass for Species: " + SpeciesName;
            errorMsg += "\n\nSpeciesKMin must be greater than InitBiomass.\n";
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
        if (InitBiomassMinVal > InitBiomassMaxVal) {
            errorMsg = "\nFound: InitBiomassMin > InitBiomassMax for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else if ((InitBiomassVal < InitBiomassMinVal) || (InitBiomassVal > InitBiomassMaxVal)) {
            errorMsg = "\nFound: InitBiomass value outside of its Min,Max range for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
        if (GrowthRateMinVal > GrowthRateMaxVal) {
            errorMsg = "\nFound: GrowthRateMinVal > GrowthRateMaxVal for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else if ((GrowthRateVal < GrowthRateMinVal) || (GrowthRateVal > GrowthRateMaxVal)) {
            errorMsg = "\nFound: GrowthRate value outside of its Min,Max range for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
        if (SpeciesKMinVal > SpeciesKMaxVal) {
            errorMsg = "\nFound: SpeciesKMinVal > SpeciesKMaxVal for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else if ((SpeciesKVal < SpeciesKMinVal) || (SpeciesKVal > SpeciesKMaxVal)) {
            errorMsg = "\nFound: SpeciesKVal value outside of its Min,Max range for Species: " + SpeciesName;
            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        }
    }

    // Delete current Species table
    cmd = "truncate table Species";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::saveSpeciesData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(Setup_Tabs, "Error",
                             "\nError in Save command. Couldn't delete all records from Species table.\n",
                             QMessageBox::Ok);
        return;
    }

    // Save Species table from table widget
    for (int i=0; i<NumSpecies; ++i) {
        SpeciesName          = Setup_Tab3_SpeciesTW->item(i, 0)->text().toStdString();
        GuildName            = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(i,1))->currentText().toStdString();
        InitBiomass          = Setup_Tab3_SpeciesTW->item(i, 2)->text().toStdString();
        InitBiomassMin       = Setup_Tab3_SpeciesTW->item(i, 3)->text().toStdString();
        InitBiomassMax       = Setup_Tab3_SpeciesTW->item(i, 4)->text().toStdString();
        GrowthRate           = Setup_Tab3_SpeciesTW->item(i, 5)->text().toStdString();
        GrowthRateMin        = Setup_Tab3_SpeciesTW->item(i, 6)->text().toStdString();
        GrowthRateMax        = Setup_Tab3_SpeciesTW->item(i, 7)->text().toStdString();
        GrowthRateCovarCoeff = Setup_Tab3_SpeciesTW->item(i, 8)->text().toStdString();
        SpeciesK             = Setup_Tab3_SpeciesTW->item(i, 9)->text().toStdString();
        SpeciesKMin          = Setup_Tab3_SpeciesTW->item(i,10)->text().toStdString();
        SpeciesKMax          = Setup_Tab3_SpeciesTW->item(i,11)->text().toStdString();
        SpeciesKCovarCoeff   = Setup_Tab3_SpeciesTW->item(i,12)->text().toStdString();
        SurveyQ              = Setup_Tab3_SpeciesTW->item(i,13)->text().toStdString();
        SurveyQMin           = Setup_Tab3_SpeciesTW->item(i,14)->text().toStdString();
        SurveyQMax           = Setup_Tab3_SpeciesTW->item(i,15)->text().toStdString();
        CatchabilityMin      = Setup_Tab3_SpeciesTW->item(i,16)->text().toStdString();
        CatchabilityMax      = Setup_Tab3_SpeciesTW->item(i,17)->text().toStdString();
        SpeDependence        = Setup_Tab3_SpeciesTW->item(i,18)->text().toStdString();
        guildKMap[GuildName] += std::stod(SpeciesK);
        systemK += std::stod(SpeciesK);
        cmd  = "INSERT INTO Species (";
        cmd += "SpeName,GuildName,InitBiomass,InitBiomassMin,InitBiomassMax,";
        cmd += "GrowthRate,GrowthRateMin,GrowthRateMax,GrowthRateCovarCoeff,";
        cmd += "SpeciesK,SpeciesKMin,SpeciesKMax,SpeciesKCovarCoeff,";
        cmd += "SurveyQ,SurveyQMin,SurveyQMax,";
        cmd += "CatchabilityMin,CatchabilityMax,SpeDependence) ";
        cmd += "VALUES ('" + SpeciesName + "', '" + GuildName +
                "', "+ InitBiomass + ", " + InitBiomassMin + ", " + InitBiomassMax +
                ", " + GrowthRate + ", " + GrowthRateMin + ", " + GrowthRateMax + ", " + GrowthRateCovarCoeff +
                ", " + SpeciesK + ", " + SpeciesKMin + ", " + SpeciesKMax + ", " + SpeciesKCovarCoeff +
                ", " + SurveyQ + ", " + SurveyQMin + ", " + SurveyQMax +
                ", " + CatchabilityMin + ", " + CatchabilityMax + ", " + SpeDependence + ");";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (Species): Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Setup_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO Species table.\n",
                                 QMessageBox::Ok);
            return;
        }

        // Need to also update the ObservedBiomass table with the initial Biomass values
        cmd  = "REPLACE INTO ObservedBiomass (";
        cmd += "MohnsRhoLabel,SystemName,SpeName,Year,Value) ";
        cmd += "VALUES ('" + MohnsRhoLabel + "','" + m_ProjectSettingsConfig + "','" +
                SpeciesName + "', 0, "+ InitBiomass + ");";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (ObservedBiomass): Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Setup_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO ObservedBiomass table.\n",
                                 QMessageBox::Ok);
            return;
        }

        species.push_back(SpeciesName);
    }
    loadSpecies();

    // Need to reload all other Estimation GUIs since Species may have changed
    emit ReloadWidgets();

    // Remove data from all tables with species different than what's in species
    pruneTablesForSpecies(species);


    QMessageBox::information(Setup_Tabs, "Save",
                             tr("\nSpecies data saved.\n"));

}



void
nmfSetup_Tab3::pruneTablesForGuilds(std::vector<std::string>& Guilds)
{
    std::string cmd;
    std::string errorMsg;
    std::string list = "(";
    std::vector<std::string> GuildATables =
    {
        "BetweenGuildsInteractionCoeff"
    };
    std::vector<std::string> GuildNameTables =
    {
        "CompetitionBetaGuildsMax",
        "CompetitionBetaGuildsMin",
        "OutputCompetitionBetaGuilds"
    };

    // Build list string that lists the valid Guilds names
    for (std::string guild : Guilds) {
        list += "'"+guild+"',";
    }
    list.erase(list.size()-1); // erase last and extraneous comma
    list += ")";

    for (std::string tableName : GuildNameTables) {
        cmd = "DELETE FROM " + tableName + " WHERE Guild NOT IN " + list;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForGuilds(1): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

    for (std::string tableName : GuildATables) {
        cmd = "DELETE FROM " + tableName + " WHERE GuildA NOT IN " + list;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForGuilds(2): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }
}


void
nmfSetup_Tab3::pruneTablesForSpecies(std::vector<std::string>& Species)
{
    std::string cmd;
    std::string errorMsg;
    std::string list = "(";
    std::vector<std::string> SpeciesATables =
    {
        "CompetitionAlpha",
        "CompetitionAlphaMax",
        "CompetitionAlphaMin",
        "CompetitionBetaGuildsMax",
        "CompetitionBetaGuildsMin",
        "CompetitionBetaSpeciesMax",
        "CompetitionBetaSpeciesMin",
        "HandlingTime",
        "HandlingTimeMax",
        "HandlingTimeMin",
        "OutputCompetitionAlpha",
        "OutputCompetitionBetaSpecies",
        "OutputHandling",
        "OutputPredation",
        "PredationLossRatesMax",
        "PredationLossRatesMin",
        "PredationLossRates",
        "SpatialOverlap"
    };
    std::vector<std::string> SpeNameTables =
    {
        "Catch",
        "DiagnosticCarryingCapacity",
        "DiagnosticGRandCC",
        "DiagnosticGrowthRate",
        "Effort",
        "Exploitation",
        "ForecastBiomass",
        "ForecastBiomassMonteCarlo",
        "ForecastBiomassMultiScenario",
        "ForecastCatch",
        "ForecastEffort",
        "ForecastExploitation",
        "ForecastUncertainty",
        "ObservedBiomass",
        "OutputBiomass",
        "OutputCarryingCapacity",
        "OutputCatchability",
        "OutputCompetitionBetaGuilds",
        "OutputExponent",
        "OutputGrowthRate",
        "OutputMSY",
        "OutputMSYBiomass",
        "OutputMSYFishing",
        "PredationExponentMax",
        "PredationExponentMin"};

    // Build list string that lists the valid Species names
    for (std::string species : Species) {
        list += "'"+species+"',";
    }
    list.erase(list.size()-1); // erase last and extraneous comma
    list += ")";

    for (std::string tableName : SpeNameTables) {
        cmd = "DELETE FROM " + tableName + " WHERE SpeName NOT IN " + list;
//std::cout << "cmd: " << cmd << std::endl;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForSpecies(1): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

    for (std::string tableName : SpeciesATables) {
        cmd = "DELETE FROM " + tableName + " WHERE SpeciesA NOT IN " + list;
//std::cout << "cmd: " << cmd << std::endl;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForSpecies(2): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }
}

void
nmfSetup_Tab3::populateARowGuilds(int row, int ncols)
{
    QTableWidgetItem *item;

    for (int j=0; j<ncols; ++j) {
        switch (j) {
            default:
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                Setup_Tab3_GuildsTW->setItem(row,j,item);
                break;
        }
    }
}

void
nmfSetup_Tab3::populateARowSpecies(int row, int ncols)
{
    int NumGuilds;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QComboBox *cbox;
    QTableWidgetItem *item;
    QStringList GuildValues;

    // Get Guild data from database
    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName FROM Guilds";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    for (int i=0; i<NumGuilds; ++i) {
        GuildValues << QString::fromStdString(dataMap["GuildName"][i]);
    }

    for (int j=0;j<ncols; ++j) {
        switch (j) {
            case 1:
                cbox = new QComboBox();
                cbox->addItems(GuildValues);
                Setup_Tab3_SpeciesTW->setCellWidget(row,j,cbox);
                break;
            default:
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                Setup_Tab3_SpeciesTW->setItem(row,j,item);
                break;
            }
    }
    Setup_Tab3_SpeciesTW->resizeColumnsToContents();

}


void
nmfSetup_Tab3::callback_Setup_Tab3_NumGuilds(int numGuilds)
{
    int ncols;
    int nrows;

    Setup_Tab3_GuildsTW->clear();

    ncols = m_colLabelsGuilds.size();
    nrows = numGuilds;

    Setup_Tab3_GuildsTW->setRowCount(nrows);
    Setup_Tab3_GuildsTW->setColumnCount(ncols);

    // Put widgets items in each cell
    for (int i=0; i<nrows; ++i)  {
        populateARowGuilds(i,ncols);
    }
    Setup_Tab3_GuildsTW->setHorizontalHeaderLabels(m_colLabelsGuilds);
    Setup_Tab3_GuildsTW->resizeColumnsToContents();

}


void
nmfSetup_Tab3::callback_Setup_Tab3_NumSpecies(int numSpecies)
{
    int ncols;
    int nrows;

    Setup_Tab3_SpeciesTW->clear();

    ncols = m_colLabelsSpecies.size();
    nrows = numSpecies;

    Setup_Tab3_SpeciesTW->setRowCount(nrows);
    Setup_Tab3_SpeciesTW->setColumnCount(ncols);

    // Put widgets items in each cell
    for (int i=0; i<nrows; ++i)  {
        populateARowSpecies(i,ncols);
    }
    Setup_Tab3_SpeciesTW->setHorizontalHeaderLabels(m_colLabelsSpecies);
    Setup_Tab3_SpeciesTW->resizeColumnsToContents();

}


void
nmfSetup_Tab3::readSettings()
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

void
nmfSetup_Tab3::saveSettings()
{

}


void
nmfSetup_Tab3::loadGuilds()
{
    int NumGuilds;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList GuildColumns;

    clearGuildWidgets();

    GuildColumns << "Guild Name" << "Growth Rate" << "Growth Rate Min" << "Growth Rate Max" <<
                    "Carrying Capacity" << "Carrying Capacity Min" << "Carrying Capacity Max" <<
                    "Catchability Min" << "Catchability Max";

    // Get Guild data from database
    fields = {"GuildName","GrowthRate","GrowthRateMin","GrowthRateMax","GuildK","GuildKMin","GuildKMax","CatchabilityMin","CatchabilityMax"};
    queryStr   = "SELECT GuildName,GrowthRate,GrowthRateMin,GrowthRateMax,GuildK,GuildKMin,GuildKMax,CatchabilityMin,CatchabilityMax FROM Guilds";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds  = dataMap["GuildName"].size();
    if (NumGuilds == 0) {
        // Table hasn't been saved yet.
        return;
    }

    callback_Setup_Tab3_NumGuilds(NumGuilds);

    if (NumGuilds > 0) {
        Setup_Tab3_NumGuildsSB->setValue(NumGuilds);
        for (int i=0; i<NumGuilds; ++i) {
            Setup_Tab3_GuildsTW->item(i,0)->setText(QString::fromStdString(dataMap["GuildName"][i]));
            Setup_Tab3_GuildsTW->item(i,1)->setText(QString::fromStdString(dataMap["GrowthRate"][i]));
            Setup_Tab3_GuildsTW->item(i,2)->setText(QString::fromStdString(dataMap["GrowthRateMin"][i]));
            Setup_Tab3_GuildsTW->item(i,3)->setText(QString::fromStdString(dataMap["GrowthRateMax"][i]));
            Setup_Tab3_GuildsTW->item(i,4)->setText(QString::fromStdString(dataMap["GuildK"][i]));
            Setup_Tab3_GuildsTW->item(i,5)->setText(QString::fromStdString(dataMap["GuildKMin"][i]));
            Setup_Tab3_GuildsTW->item(i,6)->setText(QString::fromStdString(dataMap["GuildKMax"][i]));
            Setup_Tab3_GuildsTW->item(i,7)->setText(QString::fromStdString(dataMap["CatchabilityMin"][i]));
            Setup_Tab3_GuildsTW->item(i,8)->setText(QString::fromStdString(dataMap["CatchabilityMax"][i]));
        }
        Setup_Tab3_NumGuildsSB->setEnabled(false);
    }

    Setup_Tab3_GuildsTW->resizeColumnsToContents();
    Setup_Tab3_NumGuildsSB->setEnabled(NumGuilds == 0);
    Setup_Tab3_NumGuildsSB->setValue(NumGuilds);
}

/*
 * Load up all of the Guilds and Species widgets with data from the database.
 */
void
nmfSetup_Tab3::loadSpecies()
{
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string guild;
    QComboBox *guilds;

    clearSpeciesWidgets();

    fields = {"SpeName","GuildName","InitBiomass","InitBiomassMin","InitBiomassMax",
              "GrowthRate","GrowthRateMin","GrowthRateMax","GrowthRateCovarCoeff",
              "SpeciesK","SpeciesKMin","SpeciesKMax","SpeciesKCovarCoeff",
              "SurveyQ","SurveyQMin","SurveyQMax",
              "CatchabilityMin","CatchabilityMax","SpeDependence"};
    queryStr   = "SELECT SpeName,GuildName,InitBiomass,InitBiomassMin,InitBiomassMax,";
    queryStr  += "GrowthRate,GrowthRateMin,GrowthRateMax,GrowthRateCovarCoeff,";
    queryStr  += "SpeciesK,SpeciesKMin,SpeciesKMax,SpeciesKCovarCoeff,";
    queryStr  += "SurveyQ,SurveyQMin,SurveyQMax,";
    queryStr  += "CatchabilityMin,CatchabilityMax,SpeDependence FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        // Table hasn't been saved yet.
        return;
    }

    callback_Setup_Tab3_NumSpecies(NumSpecies);

    // Load up all of the Species widgets with data from the database.
    if (NumSpecies > 0) {
        Setup_Tab3_NumSpeciesSB->setValue(NumSpecies);
        Setup_Tab3_NumSpeciesSB->setEnabled(false);

        for (int i=0; i<NumSpecies; ++i) {

            // Populate text fields
            Setup_Tab3_SpeciesTW->item(i, 0)->setText(QString::fromStdString(dataMap["SpeName"][i]));
            Setup_Tab3_SpeciesTW->item(i, 2)->setText(QString::fromStdString(dataMap["InitBiomass"][i]));
            Setup_Tab3_SpeciesTW->item(i, 3)->setText(QString::fromStdString(dataMap["InitBiomassMin"][i]));
            Setup_Tab3_SpeciesTW->item(i, 4)->setText(QString::fromStdString(dataMap["InitBiomassMax"][i]));
            Setup_Tab3_SpeciesTW->item(i, 5)->setText(QString::fromStdString(dataMap["GrowthRate"][i]));
            Setup_Tab3_SpeciesTW->item(i, 6)->setText(QString::fromStdString(dataMap["GrowthRateMin"][i]));
            Setup_Tab3_SpeciesTW->item(i, 7)->setText(QString::fromStdString(dataMap["GrowthRateMax"][i]));
            Setup_Tab3_SpeciesTW->item(i, 8)->setText(QString::fromStdString(dataMap["GrowthRateCovarCoeff"][i]));
            Setup_Tab3_SpeciesTW->item(i, 9)->setText(QString::fromStdString(dataMap["SpeciesK"][i]));
            Setup_Tab3_SpeciesTW->item(i,10)->setText(QString::fromStdString(dataMap["SpeciesKMin"][i]));
            Setup_Tab3_SpeciesTW->item(i,11)->setText(QString::fromStdString(dataMap["SpeciesKMax"][i]));
            Setup_Tab3_SpeciesTW->item(i,12)->setText(QString::fromStdString(dataMap["SpeciesKCovarCoeff"][i]));
            Setup_Tab3_SpeciesTW->item(i,13)->setText(QString::fromStdString(dataMap["SurveyQ"][i]));
            Setup_Tab3_SpeciesTW->item(i,14)->setText(QString::fromStdString(dataMap["SurveyQMin"][i]));
            Setup_Tab3_SpeciesTW->item(i,15)->setText(QString::fromStdString(dataMap["SurveyQMax"][i]));
            Setup_Tab3_SpeciesTW->item(i,16)->setText(QString::fromStdString(dataMap["CatchabilityMin"][i]));
            Setup_Tab3_SpeciesTW->item(i,17)->setText(QString::fromStdString(dataMap["CatchabilityMax"][i]));
            Setup_Tab3_SpeciesTW->item(i,18)->setText(QString::fromStdString(dataMap["SpeDependence"][i]));

            // Set Guild combo box
            guild  = dataMap["GuildName"][i];
            guilds = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(i,1));
            guilds->setCurrentText(QString::fromStdString(guild));
        }
    }

    Setup_Tab3_SpeciesTW->resizeColumnsToContents();
    Setup_Tab3_NumSpeciesSB->setEnabled(NumSpecies == 0);
    Setup_Tab3_NumSpeciesSB->setValue(NumSpecies);

}


void
nmfSetup_Tab3::loadWidgets()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab3::loadWidgets()");

    readSettings();

    loadGuilds();
    loadSpecies();
}


void
nmfSetup_Tab3::clearSpeciesWidgets()
{
    Setup_Tab3_SpeciesTW->setRowCount(0);
    Setup_Tab3_SpeciesTW->setColumnCount(0);
}


void
nmfSetup_Tab3::clearGuildWidgets()
{
    Setup_Tab3_GuildsTW->setRowCount(0);
    Setup_Tab3_GuildsTW->setColumnCount(0);
}

int
nmfSetup_Tab3::numColumnsSpecies()
{
    return m_colLabelsSpecies.size();
}

int
nmfSetup_Tab3::numColumnsGuilds()
{
    return m_colLabelsGuilds.size();
}


void
nmfSetup_Tab3::callback_Setup_Tab3_SpeciesTableChanged(int row, int col)
{
    Setup_Tab3_NumSpeciesSB->setEnabled(false);
}


void
nmfSetup_Tab3::callback_Setup_Tab3_GuildsTableChanged(int row, int col)
{
    Setup_Tab3_NumGuildsSB->setEnabled(false);
}


void
nmfSetup_Tab3::callback_Setup_Tab3_UpdateSpeciesPB()
{
    QComboBox* guildCMB;
    QString guildName;
    int numGuilds  = Setup_Tab3_GuildsTW->rowCount();
    int numSpecies = Setup_Tab3_SpeciesTW->rowCount();

    for (int i=0; i<numSpecies; ++i) {
        guildCMB = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(i,1));
        guildCMB->clear();
        for (int j=0; j<numGuilds; ++j) {
            guildName = Setup_Tab3_GuildsTW->item(j,0)->text();
            if (! guildName.isEmpty()) {
                guildCMB->addItem(Setup_Tab3_GuildsTW->item(j,0)->text());
            }
        }
    }
}
