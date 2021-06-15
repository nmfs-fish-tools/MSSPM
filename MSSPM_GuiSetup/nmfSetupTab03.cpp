
#include "nmfSetupTab03.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

#include <QCheckBox>

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

    Setup_Tab3_GuildsTW           = Setup_Tabs->findChild<QTableWidget *>("Setup_Tab3_GuildsTW");
    Setup_Tab3_SpeciesTW          = Setup_Tabs->findChild<QTableWidget *>("Setup_Tab3_SpeciesTW");
    Setup_Tab3_NumGuildsSB        = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab3_NumGuildsSB");
    Setup_Tab3_AddGuildPB         = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_AddGuildPB");
    Setup_Tab3_DelGuildPB         = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_DelGuildPB");
    Setup_Tab3_ReloadGuildsPB     = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ReloadGuildsPB");
    Setup_Tab3_NumSpeciesSB       = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab3_NumSpeciesSB");
    Setup_Tab3_AddSpeciesPB       = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_AddSpeciesPB");
    Setup_Tab3_DelSpeciesPB       = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_DelSpeciesPB");
    Setup_Tab3_ReloadSpeciesPB    = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ReloadSpeciesPB");
    Setup_Tab3_ImportPB           = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ImportPB");
    Setup_Tab3_ExportPB           = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_ExportPB");
    Setup_Tab3_LoadPB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_LoadPB");
    Setup_Tab3_SavePB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_SavePB");
    Setup_Tab3_PrevPB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_PrevPB");
    Setup_Tab3_UpdateSpeciesPB    = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_UpdateSpeciesPB");
    Setup_Tab3_GuildsSpeciesTabW  = Setup_Tabs->findChild<QTabWidget   *>("Setup_Tab3_GuildsSpeciesTabW");
    Setup_Tab3_CalcGuildsPB       = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_CalcGuildsPB");

    connect(Setup_Tab3_NumGuildsSB,     SIGNAL(valueChanged(int)),
            this,                       SLOT(callback_NumGuildsSB(int)));
    connect(Setup_Tab3_AddGuildPB,      SIGNAL(clicked()),
            this,                       SLOT(callback_AddGuildPB()));
    connect(Setup_Tab3_DelGuildPB,      SIGNAL(clicked()),
            this,                       SLOT(callback_DelGuildPB()));
    connect(Setup_Tab3_ReloadGuildsPB,  SIGNAL(clicked()),
            this,                       SLOT(callback_ReloadGuildsPB()));
    connect(Setup_Tab3_NumSpeciesSB,    SIGNAL(valueChanged(int)),
            this,                       SLOT(callback_NumSpeciesSB(int)));
    connect(Setup_Tab3_AddSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_AddSpeciesPB()));
    connect(Setup_Tab3_DelSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_DelSpeciesPB()));
    connect(Setup_Tab3_ReloadSpeciesPB, SIGNAL(clicked()),
            this,                       SLOT(callback_ReloadSpeciesPB()));
    connect(Setup_Tab3_ImportPB,        SIGNAL(clicked()),
            this,                       SLOT(callback_ImportPB()));
    connect(Setup_Tab3_ExportPB,        SIGNAL(clicked()),
            this,                       SLOT(callback_ExportPB()));
    connect(Setup_Tab3_LoadPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_LoadPB()));
    connect(Setup_Tab3_SavePB,          SIGNAL(clicked()),
            this,                       SLOT(callback_SavePB()));
    connect(Setup_Tab3_PrevPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_PrevPB()));
    connect(Setup_Tab3_SpeciesTW,       SIGNAL(cellClicked(int,int)),
            this,                       SLOT(callback_SpeciesTableChanged(int,int)));
    connect(Setup_Tab3_GuildsTW,        SIGNAL(cellClicked(int,int)),
            this,                       SLOT(callback_GuildsTableChanged(int,int)));
    connect(Setup_Tab3_UpdateSpeciesPB, SIGNAL(clicked()),
            this,                       SLOT(callback_UpdateSpeciesPB()));
    connect(Setup_Tab3_CalcGuildsPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_CalcGuildsPB()));



//    Setup_Tab3_LoadPB->hide();

    Setup_Tab3_PrevPB->setText("\u25C1--");
    Setup_Tab3_SavePB->setEnabled(true);
    Setup_Tab3_GuildsTW->setSelectionMode(QAbstractItemView::ContiguousSelection);
    Setup_Tab3_SpeciesTW->setSelectionMode(QAbstractItemView::ContiguousSelection);

    m_colLabelsSpecies << "Name" << "Guild" << "Initial Absolute Biomass" << "Growth Rate" << "Species K" ;
    m_colLabelsGuilds << "GuildName" << "GrowthRate" << "GuildK";

}


nmfSetup_Tab3::~nmfSetup_Tab3()
{
}

bool
nmfSetup_Tab3::onGuildPage()
{
    return (Setup_Tab3_GuildsSpeciesTabW->currentIndex() == 0);
}

void
nmfSetup_Tab3::callback_ReloadGuildsPB()
{
    callback_ReloadGuildsPB(nmfConstantsMSSPM::ShowPopupError);
}

void
nmfSetup_Tab3::callback_ReloadGuildsPB(bool showPopup)
{
    loadGuilds();

    if (showPopup) {
        QMessageBox::information(Setup_Tabs,"Reload",
                                 "\nGuilds reloaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfSetup_Tab3::callback_LoadPB()
{
    callback_LoadPBNoEmit();
    emit LoadEstimation();
}

void
nmfSetup_Tab3::callback_LoadPBNoEmit()
{
    if (onGuildPage()) {
        callback_ReloadGuildsPB();
    } else {
        callback_ReloadSpeciesPB();
    }
}

void
nmfSetup_Tab3::callback_ReloadSpeciesPB()
{
    callback_ReloadSpeciesPB(nmfConstantsMSSPM::ShowPopupError);
}

void
nmfSetup_Tab3::callback_ReloadSpeciesPB(bool showPopup)
{
    loadSpecies();

    if (showPopup) {
        QMessageBox::information(Setup_Tabs,"Reload",
                                 "\nSpecies reloaded.\n",
                                 QMessageBox::Ok);
    }
}


//void
//nmfSetup_Tab3::callback_Setup_Tab3_DelGuildPB()
//{
// //  QList<QString> TablesToDeleteFrom = {"Guilds"};
//    // Delete row(s) from table.  If the Species has been saved, this should happen automatically when
//    // the Species are reloaded.  However, if the user is just entering the data and hasn't yet
//    // saved and wants to delete a row, this statement is necessary.
//    QList<QTableWidgetItem*> selItems = Setup_Tab3_GuildsTW->selectedItems();
//    int numSelItems = selItems.size();
//    if (numSelItems == 0) {
//        return;
//    }
//    int firstSelRow = selItems[0]->row();
//    int lastSelRow  = selItems[numSelItems-1]->row();
//    if (firstSelRow > lastSelRow) {
//        std::swap(firstSelRow,lastSelRow);
//    }
//    int firstRow = firstSelRow;
//    int numRowsToDelete = lastSelRow - firstSelRow + 1;
//    for (int i=0; i<numRowsToDelete; ++i) {
// //      removeFromTable("Guilds",Setup_Tab3_GuildsTW->item(firstRow,0),TablesToDeleteFrom); // Remove from database table
//        Setup_Tab3_GuildsTW->removeRow(firstRow);// Remove from widget  (Refactor this with a model!!)
//    }
//    // Enable spin box if there are 0 rows left
//    Setup_Tab3_NumGuildsSB->setEnabled(Setup_Tab3_GuildsTW->rowCount() == 0);
//}

void
nmfSetup_Tab3::callback_DelGuildPB()
{
    std::string msg;
    QMessageBox::StandardButton reply;
    std::set<int> rowsToDelete;
    std::set<int>::reverse_iterator rowsToDeleteReverseIter;
    std::vector<std::string> AllTables;
    m_databasePtr->getAllTables(AllTables);

    // Delete row(s) from table.  If the Guilds have been saved, this should happen automatically when
    // the Guilds are reloaded.  However, if the user is just entering the data and hasn't yet
    // saved and wants to delete a row, this statement is necessary.
    QList<QTableWidgetItem*> selItems = Setup_Tab3_GuildsTW->selectedItems();
    int numSelItems = selItems.size();
    if (numSelItems == 0) {
        return;
    }

    // Find row numbers to delete
    for (int i=0; i<numSelItems; ++i) {
        rowsToDelete.insert(selItems[i]->row());
    }
    int numRowsToDelete = rowsToDelete.size();

    // Query user and make sure they want to delete the species.
    msg  = "\nOK to delete the " + std::to_string(numRowsToDelete) + " selected guild(s)?\n";
    msg += "All associated data in all tables in this database will also be deleted.\n";
    msg += "\nThis cannot be undone.\n";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);

    if (reply == QMessageBox::Yes) {
        Setup_Tabs->setCursor(Qt::WaitCursor);
        rowsToDeleteReverseIter = rowsToDelete.rbegin();
        for (int i=0; i<numRowsToDelete; ++i) {
//std::cout << "-----> Deleting guild: " << Setup_Tab3_GuildsTW->item(*rowsToDeleteReverseIter,0)->text().toStdString() << std::endl;
            removeFromTables("Guilds",Setup_Tab3_GuildsTW->item(*rowsToDeleteReverseIter,0),AllTables);
            Setup_Tab3_GuildsTW->removeRow(*rowsToDeleteReverseIter++);// Remove from widget  (RSK Refactor this with a model!!)
        }
        // Enable spin box if there are 0 rows left
        Setup_Tab3_NumGuildsSB->setEnabled(Setup_Tab3_GuildsTW->rowCount() == 0);
        Setup_Tabs->setCursor(Qt::ArrowCursor);
    }

    emit ReloadWidgets();
}


void
nmfSetup_Tab3::callback_DelSpeciesPB()
{
    std::string msg;
    QMessageBox::StandardButton reply;
    std::set<int> rowsToDelete;
    std::set<int>::reverse_iterator rowsToDeleteReverseIter;
    std::vector<std::string> AllTables;
    m_databasePtr->getAllTables(AllTables);

    // Delete row(s) from table.  If the Species has been saved, this should happen automatically when
    // the Species are reloaded.  However, if the user is just entering the data and hasn't yet
    // saved and wants to delete a row, this statement is necessary.
    QList<QTableWidgetItem*> selItems = Setup_Tab3_SpeciesTW->selectedItems();
    int numSelItems = (int)selItems.size();
    if (numSelItems == 0) {
        return;
    }

    // Find row numbers to delete
    for (int i=0; i<numSelItems; ++i) {
        rowsToDelete.insert(selItems[i]->row());
    }
    int numRowsToDelete = rowsToDelete.size();

    // Query user and make sure they want to delete the species.
    msg  = "\nOK to delete the " + std::to_string(numRowsToDelete) + " selected species?\n";
    msg += "All associated data in all tables in this database will also be deleted.\n";
    msg += "\nThis cannot be undone.\n";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        Setup_Tabs->setCursor(Qt::WaitCursor);
        rowsToDeleteReverseIter = rowsToDelete.rbegin();
        for (int i=0; i<numRowsToDelete; ++i) {
//std::cout << "-----> Deleting species: " << Setup_Tab3_SpeciesTW->item(*rowsToDeleteReverseIter,0)->text().toStdString() << std::endl;
            removeFromTables("Species",Setup_Tab3_SpeciesTW->item(*rowsToDeleteReverseIter,0),AllTables); // Remove from database table
            Setup_Tab3_SpeciesTW->removeRow(*rowsToDeleteReverseIter++);// Remove from widget  (RSK Refactor this with a model!!)
        }
        // Enable spin box if there are 0 rows left
        Setup_Tab3_NumSpeciesSB->setEnabled(Setup_Tab3_SpeciesTW->rowCount() == 0);
        Setup_Tabs->setCursor(Qt::ArrowCursor);
    }

    emit ReloadWidgets();
}


void
nmfSetup_Tab3::removeFromTables(const QString& type,
                                const QTableWidgetItem *itemToRemove,
                                const std::vector<std::string>& TablesToDeleteFrom)
{
    std::string cmd;
    std::string prefix;
    std::string errorMsg;
    std::string SpeciesOrGuildName;

    Setup_Tabs->setCursor(Qt::WaitCursor);

    if (itemToRemove != NULL) {
        SpeciesOrGuildName = itemToRemove->text().toStdString();
        if (! SpeciesOrGuildName.empty()) {
            for (std::string Table : TablesToDeleteFrom)
            {
                prefix = "DELETE FROM " + Table + " WHERE ";
                if (type == "Species") {
                    cmd = prefix + "SpeName   = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                    cmd = prefix + "SpeciesA  = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                    cmd = prefix + "SpeciesB  = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                } else if (type == "Guilds") {
                    cmd = prefix + "GuildName = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                    cmd = prefix + "GuildA    = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                    cmd = prefix + "GuildB    = '" + SpeciesOrGuildName + "'";
                    executeDelete(cmd);
                }
            }
        }
    }

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

/*
if (nmfUtilsQt::isAnError(errorMsg)) {
    m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 removeFromTable: Delete table error: " + errorMsg);
    m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
    return;
}
*/

void
nmfSetup_Tab3::executeDelete(std::string cmd)
{
    std::string errorMsg;

//std::cout << "delete cmd: " << cmd << std::endl;
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
}

void
nmfSetup_Tab3::callback_AddGuildPB()
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
nmfSetup_Tab3::callback_AddSpeciesPB()
{
    int numRows = Setup_Tab3_SpeciesTW->rowCount();
    int numCols = numColumnsSpecies();

    if (numRows == 0) {
        Setup_Tab3_SpeciesTW->setColumnCount(numCols);
        Setup_Tab3_SpeciesTW->setHorizontalHeaderLabels(m_colLabelsSpecies);
        Setup_Tab3_SpeciesTW->resizeColumnsToContents();
        setupHelp();
    }

    Setup_Tab3_SpeciesTW->insertRow(numRows);
    populateARowSpecies(numRows,numColumnsSpecies());

} // end callback_Setup_Tab3_AddSpeciesPB


void
nmfSetup_Tab3::callback_PrevPB()
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
nmfSetup_Tab3::callback_ImportPB()
{
    if (onGuildPage()) {
        emit LoadGuildSupplemental();
    } else {
        emit LoadSpeciesSupplemental();
    }
}

void
nmfSetup_Tab3::callback_UpdateGuildTable(QList<QString> GuildNames,
                                         QList<QString> GrowthRate,
                                         QList<QString> GuildK)
{
    int col;
    int index = 0;
    QTableWidgetItem *item;

    callback_ReloadSpeciesPB(nmfConstantsMSSPM::DontShowPopupError);

    QList<int> columns = {nmfConstantsMSSPM::Column_Guild_Name,
                          nmfConstantsMSSPM::Column_Guild_GrowthRate,
                          nmfConstantsMSSPM::Column_Guild_GuildK};
    for (QList<QString> list : {GuildNames,GrowthRate,GuildK}) {
        col = columns[index++];
        for (int row=0; row<GuildNames.size(); ++row) {
            item = new QTableWidgetItem();
            item->setText(list[row]);
            item->setTextAlignment(Qt::AlignCenter);
            Setup_Tab3_GuildsTW->setItem(row,col,item);
        }
    }
}

void
nmfSetup_Tab3::callback_UpdateSpeciesTable(QList<QString> SpeciesNames,
                                           QList<QString> SpeciesGuild,
                                           QList<QString> SpeciesInitBiomass,
                                           QList<QString> SpeciesGrowthRate,
                                           QList<QString> SpeciesK)
{
    int col;
    int index = 0;
    QTableWidgetItem *item;
    QComboBox* cbox;

    callback_ReloadSpeciesPB(nmfConstantsMSSPM::DontShowPopupError);

    QList<int> columns = {nmfConstantsMSSPM::Column_Species_SpeName,
                          nmfConstantsMSSPM::Column_Species_Guild,
                          nmfConstantsMSSPM::Column_Species_InitBiomass,
                          nmfConstantsMSSPM::Column_Species_GrowthRate,
                          nmfConstantsMSSPM::Column_Species_SpeciesK};
    for (QList<QString> list : {SpeciesNames,SpeciesGuild,SpeciesInitBiomass,SpeciesGrowthRate,SpeciesK}) {
        col = columns[index++];
        for (int row=0; row<SpeciesNames.size(); ++row) {
            if (col == nmfConstantsMSSPM::Column_Species_Guild) {
                cbox = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(row,col));
                cbox->setCurrentText(list[row]);
            } else {
                item = new QTableWidgetItem();
                item->setText(list[row]);
                item->setTextAlignment(Qt::AlignCenter);
                Setup_Tab3_SpeciesTW->setItem(row,col,item);
            }
        }
    }
}

void
nmfSetup_Tab3::loadCSVFile(QTableWidget* tableWidget,
                           const QStringList& guildValues)
{
    QString errorMsg;
    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));

    bool loadOK = nmfUtilsQt::loadTableWidgetData(
                Setup_Tabs, tableWidget, inputDataPath,
                guildValues, errorMsg);
    if (! loadOK) {
        m_logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
    }
}

void
nmfSetup_Tab3::callback_SavePB()
{
    Setup_Tabs->setCursor(Qt::WaitCursor);

    if (onGuildPage()) {
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
    std::string GrowthRate,GrowthRateMin,GrowthRateMax,Catchability,CatchabilityMin,CatchabilityMax;
    //double GuildKVal; //,GuildKMinVal,GuildKMaxVal;
    //double GrowthRateVal; //,GrowthRateMinVal,GrowthRateMaxVal;
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
    QStringList GuildList;
    for (int i=0; i<NumGuilds; ++i) {
        GuildName        = Setup_Tab3_GuildsTW->item(i,0)->text().toStdString();
        //GrowthRateVal    = Setup_Tab3_GuildsTW->item(i,1)->text().toDouble();
        //GuildKVal        = Setup_Tab3_GuildsTW->item(i,2)->text().toDouble();

//        GrowthRateMinVal = Setup_Tab3_GuildsTW->item(i,2)->text().toDouble();
//        GrowthRateMaxVal = Setup_Tab3_GuildsTW->item(i,3)->text().toDouble();
//        GuildKMinVal     = Setup_Tab3_GuildsTW->item(i,5)->text().toDouble();
//        GuildKMaxVal     = Setup_Tab3_GuildsTW->item(i,6)->text().toDouble();
        if (GuildList.contains(QString::fromStdString(GuildName))) {
            errorMsg = "\nFound duplicate Guild name: " + GuildName;
            errorMsg += "\n\nEach Guild name must be unique.\n";
            QMessageBox::warning(Setup_Tabs,"Error", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else {
            GuildList << QString::fromStdString(GuildName);
        }
//        if (GrowthRateMinVal > GrowthRateMaxVal) {
//            errorMsg = "\nFound: GrowthRateMinVal > GrowthRateMaxVal for Guild: " + GuildName;
//            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
//                                 QMessageBox::Ok);
//            return;
//        } else if ((GrowthRateVal < GrowthRateMinVal) || (GrowthRateVal > GrowthRateMaxVal)) {
//            errorMsg = "\nFound: GrowthRate value outside of its Min,Max range for Guild: " + GuildName;
//            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
//                                 QMessageBox::Ok);
//            return;
//        }
//        if (GuildKMinVal > GuildKMaxVal) {
//            errorMsg = "\nFound: GuildKMinVal > GuildKMaxVal for Guild: " + GuildName;
//            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
//                                 QMessageBox::Ok);
//            return;
//        } else if ((GuildKVal < GuildKMinVal) || (GuildKVal > GuildKMaxVal)) {
//            errorMsg = "\nFound: GuildKVal value outside of its Min,Max range for Guild: " + GuildName;
//            QMessageBox::warning(Setup_Tabs,"Warning", QString::fromStdString(errorMsg),
//                                 QMessageBox::Ok);
//            return;
//        }
    }

    // Delete current Guilds table
    cmd = "DELETE FROM Guilds";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
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
        GuildK          = Setup_Tab3_GuildsTW->item(i,2)->text().toStdString();
//        GrowthRateMin   = Setup_Tab3_GuildsTW->item(i,2)->text().toStdString();
//        GrowthRateMax   = Setup_Tab3_GuildsTW->item(i,3)->text().toStdString();
//        GuildKMin       = Setup_Tab3_GuildsTW->item(i,5)->text().toStdString();
//        GuildKMax       = Setup_Tab3_GuildsTW->item(i,6)->text().toStdString();
//        CatchabilityMin = Setup_Tab3_GuildsTW->item(i,7)->text().toStdString();
//        CatchabilityMax = Setup_Tab3_GuildsTW->item(i,8)->text().toStdString();
//        cmd  = "INSERT INTO Guilds (GuildName,GrowthRate,GrowthRateMin,GrowthRateMax,GuildK,GuildKMin,GuildKMax,CatchabilityMin,CatchabilityMax) ";
        cmd  = "INSERT INTO Guilds (GuildName,GrowthRate,GuildK) ";
//        cmd += "VALUES ('" + GuildName + "'," + GrowthRate + "," + GrowthRateMin + "," + GrowthRateMax + "," +
//                GuildK + "," + GuildKMin + "," + GuildKMax + "," +
//                CatchabilityMin  + "," + CatchabilityMax + ");";
        cmd += "VALUES ('" + GuildName + "'," + GrowthRate +  "," + GuildK + ");";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
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
    std::string GrowthRateMax,SpeciesKMin,SpeciesKMax,Catchability,CatchabilityMin,CatchabilityMax,SpeDependence;
    std::map<std::string,double> guildKMap;
    QString value;
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
//                      if (! okValue) {
                            msg = "Found an invalid numeric value of: " + value.toStdString();
                            msg += ". No commas or special characters allowed.";
                            m_logger->logMsg(nmfConstants::Error,msg);
                            QMessageBox::warning(Setup_Tabs,"Warning",
                                                 QString::fromStdString("\n"+msg+"\n"),
                                                 QMessageBox::Ok);
                            return;
//                      }
                    }
                }
            }
        }
    }

    // Species Data Integrity Checks:
    // 1. Check to make sure each Species name is unique.
    // 2. Check to make sure that the SpeciesKMin values are > than the InitBiomass values
    // 3. Check min biomass < max biomass
    // 4. Check init biomass is between min and max
    // 5. Repeat above 3 steps for r and K
    // If not, display error and return.
    QStringList SpeciesList;
    for (int i=0; i<NumSpecies; ++i) {
        SpeciesName       = Setup_Tab3_SpeciesTW->item(i, 0)->text().toStdString();
        if (SpeciesList.contains(QString::fromStdString(SpeciesName))) {
            errorMsg = "\nFound duplicate Species name: " + SpeciesName;
            errorMsg += "\n\nEach Species name must be unique.\n";
            QMessageBox::warning(Setup_Tabs,"Error", QString::fromStdString(errorMsg),
                                 QMessageBox::Ok);
            return;
        } else {
            SpeciesList << QString::fromStdString(SpeciesName);
        }
    }

    // Save Species table from table widget
    for (int i=0; i<NumSpecies; ++i) {
        SpeciesName          = Setup_Tab3_SpeciesTW->item(i, 0)->text().toStdString();
        GuildName            = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(i,1))->currentText().toStdString();
        InitBiomass          = Setup_Tab3_SpeciesTW->item(i, 2)->text().toStdString();
        GrowthRate           = Setup_Tab3_SpeciesTW->item(i, 3)->text().toStdString();
        SpeciesK             = Setup_Tab3_SpeciesTW->item(i, 4)->text().toStdString();

        guildKMap[GuildName] += std::stod(SpeciesK);
        systemK += std::stod(SpeciesK);
        cmd  = "INSERT INTO Species (";
        cmd += "SpeName,GuildName,InitBiomass,GrowthRate,SpeciesK) ";
        cmd += " VALUES ('" + SpeciesName + "', '" + GuildName +
                "', "+ InitBiomass + ", " + GrowthRate + ", " + SpeciesK + ") ";
        cmd += "ON DUPLICATE KEY UPDATE ";
        cmd += "GuildName = '"  + GuildName   + "', ";
        cmd += "InitBiomass = " + InitBiomass + ", ";
        cmd += "GrowthRate = "  + GrowthRate  + ", ";
        cmd += "SpeciesK = "    + SpeciesK    + ";";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (Species): Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Setup_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO Species table.\n",
                                 QMessageBox::Ok);
            return;
        }

        // Need to also update the BiomassAbsolute table with the Initial Absolute Biomass values
        cmd  = "REPLACE INTO BiomassAbsolute (";
        cmd += "MohnsRhoLabel,SystemName,SpeName,Year,Value) ";
        cmd += "VALUES ('" + MohnsRhoLabel + "','" + m_ProjectSettingsConfig + "','" +
                SpeciesName + "', 0, "+ InitBiomass + ");";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3 callback_Setup_Tab3_SavePB (BiomassAbsolute): Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Setup_Tabs,"Warning",
                                 "\nCouldn't REPLACE INTO BiomassAbsolute table.\n",
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
nmfSetup_Tab3::callback_ExportPB()
{
    QList<QString> GuildName;
    QList<QString> GuildGrowthRate;
    QList<QString> GuildK;
    QList<QString> SpeciesName;
    QList<QString> SpeciesGuild;
    QList<QString> SpeciesInitialBiomass;
    QList<QString> SpeciesGrowthRate;
    QList<QString> SpeciesK;

    // Need to sync Estimation Tab1 tableview with Setup Tab3 table widget
std::cout << "onGuildPage: " << onGuildPage() << std::endl;
    if (onGuildPage()) {
        for (int col=0; col<Setup_Tab3_GuildsTW->columnCount(); ++col) {
            for (int row=0; row<Setup_Tab3_GuildsTW->rowCount(); ++row) {
                if (col == nmfConstantsMSSPM::Column_Guild_Name) {
                    GuildName.push_back(Setup_Tab3_GuildsTW->item(row,col)->text());
                } else if (col == nmfConstantsMSSPM::Column_Guild_GrowthRate) {
                    GuildGrowthRate.push_back(Setup_Tab3_GuildsTW->item(row,col)->text());
                } else if (col == nmfConstantsMSSPM::Column_Guild_GuildK) {
                    GuildK.push_back(Setup_Tab3_GuildsTW->item(row,col)->text());
                }
            }
        }
        emit SaveGuildSupplemental(GuildName,GuildGrowthRate,GuildK);
    } else {
        QComboBox* cbox;
        for (int col=0; col<Setup_Tab3_SpeciesTW->columnCount(); ++col) {
            for (int row=0; row<Setup_Tab3_SpeciesTW->rowCount(); ++row) {
                if (col == nmfConstantsMSSPM::Column_Species_SpeName) {
                    SpeciesName.push_back(Setup_Tab3_SpeciesTW->item(row,col)->text());
                } else if (col == nmfConstantsMSSPM::Column_Species_Guild) {
                    cbox = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(row,col));
                    SpeciesGuild.push_back(cbox->currentText());
                } else if (col == nmfConstantsMSSPM::Column_Species_InitBiomass) {
                    SpeciesInitialBiomass.push_back(Setup_Tab3_SpeciesTW->item(row,col)->text());
                } else if (col == nmfConstantsMSSPM::Column_Species_GrowthRate) {
                    SpeciesGrowthRate.push_back(Setup_Tab3_SpeciesTW->item(row,col)->text());
                } else if (col == nmfConstantsMSSPM::Column_Species_SpeciesK) {
                    SpeciesK.push_back(Setup_Tab3_SpeciesTW->item(row,col)->text());
                }
            }
        }
        emit SaveSpeciesSupplemental(SpeciesName,SpeciesGuild,SpeciesInitialBiomass,
                                     SpeciesGrowthRate,SpeciesK);
    }
}

QList<QString>
nmfSetup_Tab3::getSpeciesGuild()
{
    QComboBox* cbox;
    QList<QString> SpeciesGuild;

    int col = nmfConstantsMSSPM::Column_Species_Guild;
    for (int row=0; row<Setup_Tab3_SpeciesTW->rowCount(); ++row) {
        cbox = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(row,col));
        SpeciesGuild.push_back(cbox->currentText());
    }

    return SpeciesGuild;
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
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForGuilds(1): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

    for (std::string tableName : GuildATables) {
        cmd = "DELETE FROM " + tableName + " WHERE GuildA NOT IN " + list;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
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
        "CompetitionBetaSpeciesMax",
        "CompetitionBetaSpeciesMin",
        "PredationHandling",
        "PredationHandlingMax",
        "PredationHandlingMin",
        "OutputCompetitionAlpha",
        "OutputCompetitionBetaSpecies",
        "OutputPredationHandling",
        "OutputPredationRho",
        "PredationRhoMax",
        "PredationRhoMin",
        "PredationRho",
        "SpatialOverlap"
    };
    std::vector<std::string> SpeNameTables =
    {
        "HarvestCatch",
        "CompetitionBetaGuildsMax",
        "CompetitionBetaGuildsMin",
        "DiagnosticCarryingCapacity",
        "DiagnosticSurface",
        "DiagnosticGrowthRate",
        "HarvestEffort",
        "HarvestExploitation",
        "ForecastBiomass",
        "ForecastBiomassMonteCarlo",
        "ForecastBiomassMultiScenario",
        "ForecastHarvestCatch",
        "ForecastHarvestEffort",
        "ForecastHarvestExploitation",
        "ForecastUncertainty",
        "BiomassAbsolute",
        "BiomassRelative",
        "BiomassRelativeScalars",
        "OutputBiomass",
        "OutputCarryingCapacity",
        "OutputCatchability",
        "OutputCompetitionBetaGuilds",
        "OutputPredationExponent",
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
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::pruneTablesForSpecies(1): Delete record error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

    for (std::string tableName : SpeciesATables) {
        cmd = "DELETE FROM " + tableName + " WHERE SpeciesA NOT IN " + list;
//std::cout << "cmd: " << cmd << std::endl;
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
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
nmfSetup_Tab3::callback_NumGuildsSB(int numGuilds)
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
nmfSetup_Tab3::callback_NumSpeciesSB(int numSpecies)
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
    setupHelp();

}

void
nmfSetup_Tab3::setupHelp()
{
    QString msg;
    QString prefix = "<html>";
    QString suffix = "</html>";

    // set Tool tips here for column headings
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(0)->setToolTip("Species Name");
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(1)->setToolTip("Guild Name");
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(2)->setToolTip("Species Initial Absolute Biomass");
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(3)->setToolTip("Species Growth Rate");
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(4)->setToolTip("Species Carrying Capacity");

    msg  = "</html><strong><center>Species Name</center></strong><br>";
    msg += "The Species name entered must be unique.";
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(0)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Guild Name</center></strong><br>";
    msg += "The user must create Guilds prior to being able to select one here.";
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(1)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Initial Absolute Biomass</center></strong><br>";
    msg += "The initial species absolute biomass is in units of metric tons.";
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(2)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Growth Rate</center></strong><br>";
    msg += "The Species growth rate (r) is a unit-less value typically between 0.0 and 1.0.";
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(3)->setWhatsThis(prefix+msg+suffix);
    msg  = "</html><strong><center>Species K</center></strong><br>";
    msg += "The Species carrying capacity (K) is the number of individuals<br>";
    msg += "in a population that can be supported by the habitat's resources.";
    Setup_Tab3_SpeciesTW->horizontalHeaderItem(4)->setWhatsThis(prefix+msg+suffix);
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

//    GuildColumns << "Guild Name" << "Growth Rate" << "Growth Rate Min" << "Growth Rate Max" <<
//                    "Carrying Capacity" << "Carrying Capacity Min" << "Carrying Capacity Max" <<
//                    "Catchability Min" << "Catchability Max";
    GuildColumns << "GuildName" << "GrowthRate" << "GuildK";
    // Get Guild data from database
    fields = {"GuildName","GrowthRate","GuildK"};
    queryStr   = "SELECT GuildName,GrowthRate,GuildK FROM Guilds";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds  = dataMap["GuildName"].size();
    if (NumGuilds == 0) {
        // Table hasn't been saved yet.
        return;
    }

    callback_NumGuildsSB(NumGuilds);

    if (NumGuilds > 0) {
        Setup_Tab3_NumGuildsSB->setValue(NumGuilds);
        for (int i=0; i<NumGuilds; ++i) {
            Setup_Tab3_GuildsTW->item(i,0)->setText(QString::fromStdString(dataMap["GuildName"][i]));
            Setup_Tab3_GuildsTW->item(i,1)->setText(QString::fromStdString(dataMap["GrowthRate"][i]));
            Setup_Tab3_GuildsTW->item(i,2)->setText(QString::fromStdString(dataMap["GuildK"][i]));
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

    fields = {"SpeName","GuildName","InitBiomass","GrowthRate","SpeciesK"};
    queryStr   = "SELECT SpeName,GuildName,InitBiomass,GrowthRate,SpeciesK FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        // Table hasn't been saved yet.
        return;
    }

    callback_NumSpeciesSB(NumSpecies);

    // Load up all of the Species widgets with data from the database.
    if (NumSpecies > 0) {
        Setup_Tab3_NumSpeciesSB->setValue(NumSpecies);
        Setup_Tab3_NumSpeciesSB->setEnabled(false);

        for (int i=0; i<NumSpecies; ++i) {

            // Populate text fields
            Setup_Tab3_SpeciesTW->item(i,0)->setText(QString::fromStdString(dataMap["SpeName"][i]));
            Setup_Tab3_SpeciesTW->item(i,2)->setText(QString::fromStdString(dataMap["InitBiomass"][i]));
            Setup_Tab3_SpeciesTW->item(i,3)->setText(QString::fromStdString(dataMap["GrowthRate"][i]));
            Setup_Tab3_SpeciesTW->item(i,4)->setText(QString::fromStdString(dataMap["SpeciesK"][i]));

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
nmfSetup_Tab3::callback_SpeciesTableChanged(int row, int col)
{
    Setup_Tab3_NumSpeciesSB->setEnabled(false);
}


void
nmfSetup_Tab3::callback_GuildsTableChanged(int row, int col)
{
    Setup_Tab3_NumGuildsSB->setEnabled(false);
}

void
nmfSetup_Tab3::callback_CalcGuildsPB()
{
    QString guildName;
    QComboBox* guildCMB;
    std::map<QString,double> guildMap;

    // Load up the guild map with carrying capacity totals per guild
    int numSpecies = Setup_Tab3_SpeciesTW->rowCount();
    for (int i=0; i<numSpecies; ++i) {
        guildCMB = qobject_cast<QComboBox *>(Setup_Tab3_SpeciesTW->cellWidget(i,1));
        guildMap[guildCMB->currentText()] += Setup_Tab3_SpeciesTW->item(i, 4)->text().toDouble();
    }

    // Transfer data from the guild map to the guild table
    int numGuilds  = Setup_Tab3_GuildsTW->rowCount();
    for (int i=0; i<numGuilds; ++i) {
        guildName = Setup_Tab3_GuildsTW->item(i,0)->text();
        Setup_Tab3_GuildsTW->item(i,2)->setText(QString::number(guildMap[guildName],'f',6));
    }
    Setup_Tab3_GuildsTW->resizeColumnsToContents();
}

void
nmfSetup_Tab3::callback_UpdateSpeciesPB()
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
