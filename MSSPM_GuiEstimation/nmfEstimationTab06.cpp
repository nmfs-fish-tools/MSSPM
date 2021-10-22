
#include "nmfEstimationTab06.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfEstimation_Tab6::nmfEstimation_Tab6(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_ProjectName  = "";
    m_ModelName    = "";
    m_ParamNames.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    readSettings();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Covariate Data"));


    Estimation_Tab6_AddPB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_AddPB");
    Estimation_Tab6_CovariateTV        = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_CovariateTV");
    Estimation_Tab6_SpeciesParameterTV = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab6_SpeciesParameterTV");


    connect(Estimation_Tab6_AddPB,   SIGNAL(clicked()),
            this,                    SLOT(callback_AddPB()));

    initializeCovariateTable();
    initializeSpeciesParameterTable();

} // end constructor


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{

}

void
nmfEstimation_Tab6::initializeCovariateTable()
{
    m_smodelC = new QStandardItemModel(0,0);
    Estimation_Tab6_CovariateTV->setModel(m_smodelC);
}

void
nmfEstimation_Tab6::initializeSpeciesParameterTable()
{
    std::vector<std::string> SpeciesNames;
    int RunLength;
    int StartYear;
    QStringList verticalHeader;
    QStringList horizontalHeader;
    int NumSpecies;

    QStandardItemModel* smodelC = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

    if (! m_DatabasePtr->getSpecies(m_Logger,SpeciesNames)) {
        return;
    }

    // Set up header data
    NumSpecies = SpeciesNames.size();
    QList<QStandardItem* > items;
    for (int i=0; i<NumSpecies; ++i) {
        verticalHeader << QString::fromStdString(SpeciesNames[i]);
    }
    int numParameters = (int)m_ParamNames.size();
    for (int i=0; i<numParameters; ++i) {
        horizontalHeader << QString::fromStdString(m_ParamNames[i]);
    }
    m_smodelSP = new QStandardItemModel(NumSpecies,numParameters);
    m_smodelSP->setVerticalHeaderLabels(verticalHeader);
    m_smodelSP->setHorizontalHeaderLabels(horizontalHeader);
    Estimation_Tab6_SpeciesParameterTV->setModel(m_smodelSP);

    // Set up item comboboxes
    QComboBox* cbox;
    int numCovariates = smodelC->columnCount();
    for (int row=0; row<m_smodelSP->rowCount(); ++row) {
        for (int col=0; col<m_smodelSP->columnCount(); ++col) {
            cbox = new QComboBox();
            cbox->addItem("-");
            for (int j=0; j<numCovariates; ++j) {
                cbox->addItem(smodelC->horizontalHeaderItem(j)->text());
            }
            QModelIndex index = m_smodelSP->index(row,col);
            Estimation_Tab6_SpeciesParameterTV->setIndexWidget(index,cbox);
        }
    }

}



void
nmfEstimation_Tab6::readSettings()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::readSettings");

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName   = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

//    settings->beginGroup("Preferences");
//    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
//    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab6::addCovariateColumn(QString covariateName)
{
    int RunLength;
    int StartYear;
    QStringList verticalHeader;

    m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);

    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab6_CovariateTV->model());

    smodel->setRowCount(RunLength);
    QList<QStandardItem* > items;
    for (int i=0; i<=RunLength; ++i) {
        items.append(new QStandardItem(QString("")));
        verticalHeader << QString::number(StartYear+i);
    }
    smodel->appendColumn(items);

    QStandardItem* headerItem = new QStandardItem(covariateName);
    smodel->setVerticalHeaderLabels(verticalHeader);
    smodel->setHorizontalHeaderItem(smodel->columnCount()-1,headerItem);
    Estimation_Tab6_CovariateTV->resizeColumnsToContents();

    initializeSpeciesParameterTable();
}

void
nmfEstimation_Tab6::callback_AddPB()
{
    bool ok;
    QString covariateName = QInputDialog::getText(Estimation_Tabs, tr("Environmental Covariate Name"),
                                         tr("Enter Covariate Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !covariateName.isEmpty()) {
        addCovariateColumn(covariateName);
    }

}

void
nmfEstimation_Tab6::callback_SetEstimateRunCheckboxes(
        std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes)
{
    m_ParamNames.clear();
    for (int i=0; i<(int)EstimateRunBoxes.size();++i) {
        if (EstimateRunBoxes[i].state.first && EstimateRunBoxes[i].state.second) {
            m_ParamNames.push_back(EstimateRunBoxes[i].parameter);
        }
    }
    sort(m_ParamNames.begin(),m_ParamNames.end());
    initializeSpeciesParameterTable();
}
