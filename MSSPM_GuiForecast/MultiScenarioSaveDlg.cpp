#include "MultiScenarioSaveDlg.h"

#include "nmfConstants.h"

#include <QStandardItemModel>
#include <QTableWidget>
#include <QPushButton>
#include <QListWidget>
#include <QStringList>

MultiScenarioSaveDlg::MultiScenarioSaveDlg(QTabWidget*  parent,
                                                 nmfDatabase* databasePtr,
                                                 nmfLogger*   logger,
                                                 std::string& ProjectName,
                                                 std::string& ModelName,
                                                 std::map<QString,QStringList>& SortedForecastLabelsMap,
                                                 std::string& currentScenario,
                                                 std::string  forecastName) : QDialog(parent)
{
    m_DatabasePtr  = databasePtr;
    m_Logger       = logger;
    m_ScenarioName = currentScenario;
    m_ForecastName = forecastName;
    m_ModelName    = ModelName;
    m_ProjectName  = ProjectName;
    if (SortedForecastLabelsMap.empty()) {
        m_OrderedForecastLabelsMap.clear();
    } else {
        m_OrderedForecastLabelsMap = SortedForecastLabelsMap;
    }

    MainVLT          = new QVBoxLayout();
    BtnHLT           = new QHBoxLayout();
    ScenarioHLT      = new QHBoxLayout();
    ForecastHLT      = new QHBoxLayout();
    ShowSortHLT      = new QHBoxLayout();
    ForecastLabelHLT = new QHBoxLayout();
    ScenarioNameLBL  = new QLabel("\nScenario Name:");
    ForecastLabelLBL = new QLabel("Forecast Label:");
    ShowSortLBL      = new QLabel("Sort Window:");
    ScenarioNameCMB  = new QComboBox();
    ForecastLabelCMB = new QComboBox();
    NewScenarioPB    = new QPushButton("New...");
    SetForecastPB    = new QPushButton("Set");
    DelScenarioPB    = new QPushButton("Del");
    DelForecastPB    = new QPushButton("Del");
    CancelPB         = new QPushButton("Cancel");
    SetOrderPB       = new QPushButton("Set Order and Close");
    OkPB             = new QPushButton("OK");
    ShowSortPB       = new QPushButton(QString::fromUtf8("\u25BE"));
    RenScenarioPB    = new QPushButton("Ren...");
    RenForecastPB    = new QPushButton("Ren...");
    ForecastLabelLW  = new QListWidget();

    MainVLT->addWidget(ScenarioNameLBL,0);
    ScenarioHLT->addWidget(ScenarioNameCMB);
    ScenarioHLT->addWidget(NewScenarioPB);
    ScenarioHLT->addWidget(DelScenarioPB);
    ScenarioHLT->addWidget(RenScenarioPB);
    MainVLT->addLayout(ScenarioHLT,0);
    MainVLT->addWidget(ForecastLabelLBL,0);
    ForecastHLT->addWidget(ForecastLabelCMB);
    ForecastHLT->addWidget(SetForecastPB);
    ForecastHLT->addWidget(DelForecastPB);
    ForecastHLT->addWidget(RenForecastPB);
    ShowSortHLT->addWidget(ShowSortLBL);
    ShowSortHLT->addWidget(ShowSortPB);
    ShowSortHLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    MainVLT->addLayout(ForecastHLT,0);
    MainVLT->addSpacerItem(new QSpacerItem(10,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainVLT->addLayout(ShowSortHLT,0);
    ForecastLabelHLT->addWidget(ForecastLabelLW,0);
    ForecastLabelHLT->setStretch(0,10);
    MainVLT->addLayout(ForecastLabelHLT,10);
    MainVLT->addSpacerItem(new QSpacerItem(10,20,QSizePolicy::Fixed,QSizePolicy::Expanding));
    BtnHLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    BtnHLT->addWidget(CancelPB);
    BtnHLT->addWidget(SetOrderPB);
    BtnHLT->addWidget(OkPB);
    BtnHLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    MainVLT->addLayout(BtnHLT,0);
    this->setLayout(MainVLT);

    ForecastLabelLW->setDragDropMode(QAbstractItemView::DragDrop);
    ForecastLabelLW->setDefaultDropAction(Qt::MoveAction);
    ForecastLabelLW->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ShowSortPB->setFixedWidth(15);
    ShowSortPB->setFixedHeight(15);
    ForecastLabelLW->setFixedHeight(5);
    ForecastLabelLW->setEnabled(false);

    NewScenarioPB->setFixedWidth(50);
    SetForecastPB->setFixedWidth(50);
    DelScenarioPB->setFixedWidth(30);
    DelForecastPB->setFixedWidth(30);
    RenScenarioPB->setFixedWidth(40);
    RenForecastPB->setFixedWidth(40);
    ScenarioNameCMB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ScenarioNameCMB->setMinimumWidth(150);
    ForecastLabelCMB->setMinimumWidth(150);
    DelScenarioPB->setToolTip("Delete the selected Scenario Name");
    DelScenarioPB->setStatusTip("Delete the selected Scenario Name");
    DelForecastPB->setToolTip("Delete the selected Forecast from the selected Scenario");
    DelForecastPB->setStatusTip("Delete the selected Forecast from the selected Scenario");
    NewScenarioPB->setToolTip("Create a new Scenario Name");
    NewScenarioPB->setStatusTip("Create a new Scenario Name");
    RenScenarioPB->setToolTip("Rename the currently selected Scenario Name");
    RenScenarioPB->setStatusTip("Rename the currently selected Scenario Name");
    SetForecastPB->setToolTip("Set the Forecast Label to be the current Forecast Name");
    SetForecastPB->setStatusTip("Set the Forecast Label to be the current Forecast Name");
    RenForecastPB->setToolTip("Rename the currently selected Forecast Label");
    RenForecastPB->setStatusTip("Rename the currently selected Forecast Label");
    ForecastLabelCMB->setEditable(true);
    OkPB->setToolTip("Save Forecast to Specified Scenario");
    OkPB->setStatusTip("Save Forecast to Specified Scenario");
    SetOrderPB->setToolTip("Resets the Forecast Label order to that of the above list");
    SetOrderPB->setStatusTip("Resets the Forecast Label order to that of the above list");
    ShowSortPB->setToolTip("Show/Hide Forecast Label Sort List");
    ShowSortPB->setStatusTip("Show/Hide Forecast Label Sort List");

    connect(ScenarioNameCMB, SIGNAL(currentIndexChanged(QString)),
            this,            SLOT(callback_ScenarioNameCMB(QString)));
    connect(NewScenarioPB,   SIGNAL(clicked()),
            this,            SLOT(callback_NewScenarioPB()));
    connect(SetForecastPB,   SIGNAL(clicked()),
            this,            SLOT(callback_SetForecastPB()));
    connect(DelScenarioPB,   SIGNAL(clicked()),
            this,            SLOT(callback_DelScenarioPB()));
    connect(RenScenarioPB,   SIGNAL(clicked()),
            this,            SLOT(callback_RenScenarioPB()));
    connect(DelForecastPB,   SIGNAL(clicked()),
            this,            SLOT(callback_DelForecastPB()));
    connect(RenForecastPB,   SIGNAL(clicked()),
            this,            SLOT(callback_RenForecastPB()));
    connect(CancelPB,        SIGNAL(clicked()),
            this,            SLOT(close()));
    connect(OkPB,            SIGNAL(clicked()),
            this,            SLOT(callback_OkPB()));
    connect(SetOrderPB,      SIGNAL(clicked()),
            this,            SLOT(callback_SetOrderPB()));
    connect(ShowSortPB,      SIGNAL(clicked()),
            this,            SLOT(callback_ShowSortPB()));

    setWindowTitle("Modify Scenario");

    loadWidgets();

    ScenarioNameCMB->setCurrentText(QString::fromStdString(m_ScenarioName));
}


void
MultiScenarioSaveDlg::callback_ShowSortPB()
{
    if (ForecastLabelLW->isEnabled()) {
        ForecastLabelLW->setFixedHeight(5);
        ForecastLabelLW->setEnabled(false);
    } else {
        ForecastLabelLW->setEnabled(true);
        ForecastLabelLW->setFixedHeight(200);
    }
}

void
MultiScenarioSaveDlg::loadWidgets()
{
    loadScenarioMap();

    ScenarioNameCMB->clear();
    ScenarioNameCMB->blockSignals(true);
    for(auto it  = m_OrderedForecastLabelsMap.begin();
             it != m_OrderedForecastLabelsMap.end(); ++it)
    {
        ScenarioNameCMB->addItem(it->first);
    }
    ScenarioNameCMB->blockSignals(false);

    callback_ScenarioNameCMB(QString::fromStdString(getScenarioName()));

}

void
MultiScenarioSaveDlg::loadScenarioMap()
{
    m_DatabasePtr->createScenarioMap(
                m_ProjectName,
                m_ModelName,
                m_OrderedForecastLabelsMap);
}

std::string
MultiScenarioSaveDlg::getScenarioName()
{
    return ScenarioNameCMB->currentText().toStdString();
}

std::string
MultiScenarioSaveDlg::getForecastLabel()
{
    return ForecastLabelCMB->currentText().toStdString();
}

bool
MultiScenarioSaveDlg::getForecastData(
        const std::string& forecastName,
        std::vector<std::string>& Species,
        std::vector<int>& Years,
        boost::numeric::ublas::matrix<double>& ForecastBiomass)
{
    int m;
    int NumRecords = 0;
    int NumYears = 0;
    int NumSpecies = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QString msg;

    m_DatabasePtr->getAlgorithmIdentifiers(
                this,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);


    // Find number of years in forecast
    fields     = {"ProjectName","ModelName","ForecastName","RunLength"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,RunLength FROM " +
                  nmfConstantsMSSPM::TableForecasts +
                 "  WHERE ProjectName = '"      + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName   +
                 "' AND ForecastName = '"       + forecastName  +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    if (NumRecords == 0) {
        msg = "\nNo records found in Forecasts for ForecastName = '" +
                QString::fromStdString(forecastName) + "'";
        QMessageBox::warning(this,tr("Warning"),tr(msg.toLatin1()),QMessageBox::Yes);
        return false;
    }
    NumYears = std::stod(dataMap["RunLength"][0]);

    // Find number of species in forecast
    fields     = {"ProjectName","ModelName","ForecastName","isAggProd","SpeName","Year","Value"};
    queryStr   = "SELECT ProjectName,ModelName,ForecastName,isAggProd,SpeName,Year,Value FROM " +
                  nmfConstantsMSSPM::TableForecastBiomass +
                 " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND ForecastName = '"       + forecastName +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling + "'";
    queryStr  += " ORDER BY SpeName,Year";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ForecastName"].size();
    if (NumRecords == 0) {
        msg = "\nNo records found in ForecastBiomass for ForecastName = '" +
                QString::fromStdString(forecastName) + "'";
        QMessageBox::warning(this,tr("Warning"),tr(msg.toLatin1()),QMessageBox::Yes);
        return false;
    }
    NumSpecies = NumRecords/(NumYears+1);

    // Initialize and load ForecastBiomass matrix
    nmfUtils::initialize(ForecastBiomass,NumYears+1,NumSpecies);
    m = 0;
    for (int i=0; i<NumSpecies; ++i) { // Species
        Species.push_back(dataMap["SpeName"][m]);
        for (int j=0; j<=NumYears; ++j) { // Time in years
            if (i == 0) {
                Years.push_back(std::stoi(dataMap["Year"][m]));
            }
            ForecastBiomass(j,i) = std::stod(dataMap["Value"][m]);
            ++m;
        }
    }
    return true;
}

void
MultiScenarioSaveDlg::getOrderedForecastLabels(
        std::map<QString,QStringList>& orderedForecastLabelsMap)
{
    orderedForecastLabelsMap = m_OrderedForecastLabelsMap;
}

void
MultiScenarioSaveDlg::callback_SetOrderPB()
{
    std::string cmd;
    std::string errorMsg;

    for (int sortOrder=0; sortOrder<ForecastLabelLW->count(); ++sortOrder) {
        // Update database table to reflect new sort order
        cmd = "UPDATE " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
              " SET SortOrder = " + std::to_string(sortOrder) +
              " WHERE ProjectName = '"  + m_ProjectName +
              "' AND ModelName = '"     + m_ModelName +
              "' AND ScenarioName = '"  + getScenarioName() +
              "' AND ForecastLabel = '" + ForecastLabelLW->item(sortOrder)->text().toStdString() + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] callback_SetOrderPB: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        }
    }

    // Reload map to reflect new sort order
    loadScenarioMap();

    close();
}

void
MultiScenarioSaveDlg::callback_OkPB()
{
   bool dataWritten  = false;
   bool okToWriteFile = true;
// bool ForecastAlreadyInMap = false;
   int NumRecords;
   int NumYears;
   int NumSpecies;
   std::vector<std::string> fields;
   std::map<std::string, std::vector<std::string> > dataMap;
   std::string cmd;
   std::string errorMsg;
   std::string queryStr;
   std::string Scenario = getScenarioName();
   std::string Forecast = getForecastLabel();
   std::string SortOrder = "0";
   std::vector<std::string> Species;
   std::vector<int> Years;
   boost::numeric::ublas::matrix<double> ForecastBiomass;
   QMessageBox::StandardButton reply;
   QString msg;

   // Check that the scenario and forecast aren't blank.
   if (QString::fromStdString(Scenario).trimmed() == "" ||
       QString::fromStdString(Forecast).trimmed() == "")
   {
       QMessageBox::critical(this,
                             tr("Illegal Name"),
                             tr("\nNeither the Scenario name nor the Forecast Label may be blank.\n"),
                             QMessageBox::Ok);
       return;
   }


   if (! getForecastData(m_ForecastName,Species,Years,ForecastBiomass)) {
       close();
       return;
   }
   NumSpecies = Species.size();
   NumYears   = Years.size();

   // Check that current Scenario and Forecast doesn't already exist.
   // If it does, ask user if they want to overwrite it.
   fields     = {"ProjectName","ModelName","ScenarioName","ForecastLabel","SpeName","Year","Value"};
   queryStr   = "SELECT ProjectName,ModelName,ScenarioName,ForecastLabel,SpeName,Year,Value FROM " +
                 nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
               " WHERE ProjectName = '"  + m_ProjectName +
               "' AND ModelName = '"     + m_ModelName +
               "' AND ScenarioName = '"  + Scenario +
               "' AND ForecastLabel = '" + Forecast + "'";
   dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
   NumRecords = dataMap["ScenarioName"].size();
   if (NumRecords > 0) {
       msg = "\nForecast Label already used in the specified Scenario.\n\nOK to overwrite?";
       reply = QMessageBox::question(this, tr("Forecast Label Found"), tr(msg.toLatin1()),
                                     QMessageBox::No|QMessageBox::Yes,
                                     QMessageBox::Yes);
       okToWriteFile = (reply == QMessageBox::Yes);
//     ForecastAlreadyInMap = true;
   }

   if (okToWriteFile)
   {
       cmd  = "DELETE FROM " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
              "  WHERE ProjectName = '" + m_ProjectName +
              "' AND ModelName = '"     + m_ModelName +
              "' AND ScenarioName = '"  + Scenario +
              "' AND ForecastLabel = '" + Forecast + "'";
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 1] MultiScenarioSaveDlg::callback_OkPB: DELETE error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }

       cmd = "INSERT INTO " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
             " (ProjectName,ModelName,ScenarioName,SortOrder,ForecastLabel,SpeName,Year,Value) VALUES ";
       for (int i=0; i<NumSpecies; ++i) {   // Species
           for (int j=0; j<NumYears; ++j) { // Time in years
               cmd += "('"   + m_ProjectName +
                       "','" + m_ModelName +
                       "','" + Scenario +
                       "',"  + SortOrder +
                       ",'"  + Forecast +
                       "','" + Species[i] +
                       "',"  + std::to_string(Years[j]) +
                       ","   + QString::number(ForecastBiomass(j,i)).toStdString() + "),";
           }
       }
       cmd = cmd.substr(0,cmd.size()-1); // Remove last comma
       errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
       if (nmfUtilsQt::isAnError(errorMsg)) {
           m_Logger->logMsg(nmfConstants::Error,"[Error 2] MultiScenarioSaveDlg::callback_OkPB: Write table error: " + errorMsg);
           m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
           return;
       }
       dataWritten = true;

       loadScenarioMap();
   }

   if (dataWritten) {
       accept();
   }
}

void
MultiScenarioSaveDlg::callback_NewScenarioPB()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Scenario Name"),
                                         tr("Enter new Scenario name:"),
                                         QLineEdit::Normal, "Scenario1", &ok);
    if (ok) {
        if (text.isEmpty() || text.trimmed() == "") {
            QMessageBox::warning(this, "Error", tr("\nScenario Name may not be blank.\n"), QMessageBox::Ok);
        } else if (ScenarioNameCMB->findText(text) == -1) { // -1 means item not found
            ScenarioNameCMB->addItem(text);
            ScenarioNameCMB->model()->sort(0);
            ScenarioNameCMB->setCurrentText(text);
        } else {
            QMessageBox::warning(this, "Error", tr("\nScenario Name already exists. Please try another name."), QMessageBox::Ok);
        }
    }

}

void
MultiScenarioSaveDlg::callback_SetForecastPB()
{
    ForecastLabelCMB->setCurrentText(QString::fromStdString(m_ForecastName));
}

void
MultiScenarioSaveDlg::callback_ScenarioNameCMB(QString scenario)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString ForecastLabel;
    QStringList tmpList;

    tmpList.clear();
    ForecastLabelLW->clear();
    ForecastLabelCMB->clear();

    if (m_OrderedForecastLabelsMap[scenario].isEmpty()) {
        fields     = {"ForecastLabel"};
        queryStr   = "SELECT DISTINCT SortOrder,ForecastLabel FROM " +
                      nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
                     " WHERE ProjectName = '" + m_ProjectName +
                     "' AND ModelName = '"    + m_ModelName +
                     "' AND ScenarioName = '" + scenario.toStdString() +
                     "' ORDER BY SortOrder,ForecastLabel";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["ForecastLabel"].size();
        for (int i=0; i<NumRecords; ++i) {
            ForecastLabel = QString::fromStdString(dataMap["ForecastLabel"][i]);
            QListWidgetItem *item = new QListWidgetItem(ForecastLabel);
            ForecastLabelLW->addItem(item);
            ForecastLabelCMB->addItem(ForecastLabel);
            tmpList << ForecastLabel;
        }
        m_OrderedForecastLabelsMap[scenario] = tmpList;
    } else {
        NumRecords = m_OrderedForecastLabelsMap[scenario].size();
        for (int i=0; i<NumRecords; ++i) {
            ForecastLabel = m_OrderedForecastLabelsMap[scenario][i];
            QListWidgetItem *item = new QListWidgetItem(ForecastLabel);
            ForecastLabelLW->addItem(item);
            ForecastLabelCMB->addItem(ForecastLabel);
        }
    }
}

void
MultiScenarioSaveDlg::callback_DelScenarioPB()
{
    std::string scenario = getScenarioName();
    std::string cmd;
    std::string errorMsg;
    QString msg;
    QMessageBox::StandardButton reply;

    msg  = "\nOK to delete Multi-Scenario: " + QString::fromStdString(scenario) + " ?";
    msg += "\n\nN.B. All forecasts associated with this Multi-Scenario will also be deleted.\n";
    reply = QMessageBox::question(this, tr("OK to Delete"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        cmd  = "DELETE FROM " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName +
               "' AND ScenarioName = '" + scenario + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] MultiScenarioSaveDlg::callback_DelScenarioPB: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }

        loadWidgets();
        close();
    }
}

void
MultiScenarioSaveDlg::printMap(QString msg)
{
    std::cout << "\n" << msg.toStdString() << std::endl;
    for(auto it = m_OrderedForecastLabelsMap.begin();
        it != m_OrderedForecastLabelsMap.end(); ++it)
    {
        std::cout << it->first.toStdString() << std::endl;
        for (QString forecast : it->second) {
            std::cout << "  " << forecast.toStdString() << std::endl;
        }
    }
}


void
MultiScenarioSaveDlg::callback_DelForecastPB()
{
    bool okToDelete = false;
    std::string cmd;
    std::string errorMsg;
    std::string scenario = getScenarioName();
    std::string forecast = getForecastLabel();
    QString msg;
    QMessageBox::StandardButton reply;

    msg = "\nWill delete all Multi-Scenario data related to:\n\nScenario Name: " +
            QString::fromStdString(scenario) +
            "\nForecast Label: " + QString::fromStdString(forecast) +
            "\n\nOK to Delete?\n";

    reply = QMessageBox::question(this, tr("Delete Forecast Label"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    okToDelete = (reply == QMessageBox::Yes);
    if (okToDelete) {
        cmd  = "DELETE FROM " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
               " WHERE ProjectName = '"  + m_ProjectName +
               "' AND ModelName = '"     + m_ModelName +
               "' AND ScenarioName = '"  + scenario +
               "' AND ForecastLabel = '" + forecast + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] MultiScenarioSaveDlg::callback_DelForecastPB: DELETE error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
        msg = "\nMulti-Scenario Forecast Label data successfully deleted.\n";
        QMessageBox::information(this,tr("Forecast Data Deleted"),tr(msg.toLatin1()),QMessageBox::Ok);

        loadWidgets();
        close();
    }
}


void
MultiScenarioSaveDlg::callback_RenScenarioPB()
{
    bool ok;
    QString NewScenarioName = QInputDialog::getText(this, tr("Rename Scenario"),
                                                    tr("Enter New Scenario Name:"),
                                                    QLineEdit::Normal, "", &ok);
    if (ok) {
        if (NewScenarioName.isEmpty()) {
            QMessageBox::critical(this,tr("Error"),
                                  tr("\nInvalid Scenario Name Entered"),
                                  QMessageBox::Ok);
            return;
        } else {
            renameScenarioName(QString::fromStdString(getScenarioName()),
                               NewScenarioName);
        }

        loadWidgets();
        emit RefreshOutput();
    }
}

void
MultiScenarioSaveDlg::callback_RenForecastPB()
{
    bool ok;
    QString NewForecastLabel = QInputDialog::getText(this, tr("Rename Forecast Label"),
                                                     tr("Enter New Forecast Label:"),
                                                     QLineEdit::Normal, "", &ok);
    if (ok) {
        if (NewForecastLabel.isEmpty()) {
            QMessageBox::critical(this,tr("Error"),
                                  tr("\nInvalid Forecast Label Entered"),
                                  QMessageBox::Ok);
            return;
        } else {
            renameForecastLabel(QString::fromStdString(getScenarioName()),
                                QString::fromStdString(getForecastLabel()),
                                NewForecastLabel);
        }

        loadWidgets();
        emit RefreshOutput();
    }
}


void
MultiScenarioSaveDlg::renameScenarioName(QString oldScenario,
                                            QString newScenario)
{
    std::string cmd;
    std::string errorMsg;

    // Finally, update tables
    cmd = "UPDATE " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
          " SET ScenarioName = '"   + newScenario.toStdString() +
          "' WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"     + m_ModelName +
          "' AND ScenarioName = '"  + oldScenario.toStdString() + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] renameScenarioName: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
    }

    loadScenarioMap();
}

void
MultiScenarioSaveDlg::renameForecastLabel(QString scenario,
                                             QString oldForecast,
                                             QString newForecast)
{
    std::string cmd;
    std::string errorMsg;

    // Second, update tables
    cmd = "UPDATE " + nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
          " SET ForecastLabel = '"  + newForecast.toStdString() +
          "' WHERE ProjectName = '" + m_ProjectName +
          "' AND ModelName = '"     + m_ModelName +
          "' AND ScenarioName = '"  + scenario.toStdString() +
          "' AND ForecastLabel = '" + oldForecast.toStdString() + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] renameForecastLabel: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
    }
    loadScenarioMap();
}

