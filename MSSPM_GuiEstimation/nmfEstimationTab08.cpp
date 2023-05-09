#include "nmfEstimationTab08.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab8::nmfEstimation_Tab8(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;
    QStringList horizontalHeaders =
        {"Model Name","r²","SSResiduals","AIC",                                           // 0-3
         "Growth Form","Harvest Form","Competition Form","Predation Form",                // 4-7
         "Num Runs","Objective Criterion","Estimation Algorithm","Minimizer","Scaling",   // 8-12
         "DatabaseSnapshot","Notes",                                                      // 13-14
         "isDeterministicBees","maxGenerations","numBees","numBestSites","numEliteSites", // 15-19
         "numEliteBees","numOtherBees","neighborhoodSize","numSubRuns",                   // 20-23
         "isDeterministicNLopt","isStopAfterValue","StopAfterValue",                      // 24-26
         "isStopAfterTime","StopAfterTime","isStopAfterIter","StopAfterIter",
         "isEstInitBiomassEnabled","isEstInitBiomassChecked",
         "isEstGrowthRateEnabled","isEstGrowthRateChecked",
         "isEstGrowthRateShapeEnabled","isEstGrowthRateShapeChecked",
         "isEstCarryingCapacityEnabled","isEstCarryingCapacityChecked",
         "isEstCatchabilityEnabled","isEstCatchabilityChecked",
         "isEstCompAlphaEnabled","isEstCompAlphaChecked",
         "isEstCompBetaSpeciesEnabled","isEstCompBetaSpeciesChecked",
         "isEstCompBetaGuildsEnabled","isEstCompBetaGuildsChecked",
         "isEstCompBetaGuildsGuildsEnabled","isEstCompBetaGuildsGuildsChecked",
         "isEstPredRhoEnabled","isEstPredRhoChecked",
         "isEstPredHandlingEnabled","isEstPredHandlingChecked",
         "isEstPredExponentEnabled","isEstPredExponentChecked",
         "isEstSurveyQEnabled","isEstSurveyQChecked",
         "isAMultiRun","AveAlg","HowToAverage","UsingWhat",
         "UsingNumber","isUsingPct","EnsembleFile","EstimatedParametersFile"};

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_ProjectDir   = projectDir;
    m_NumColumns   = horizontalHeaders.size();
    m_NumSignificantDigits = -1;
    m_ModelName.clear();
    m_ProjectName.clear();

    readSettings();
    //m_DatabasePtr->clearTable(m_Logger,nmfConstantsMSSPM::TableModelReview,m_ProjectName);

    m_SModel = new QStandardItemModel(0,m_NumColumns);
    m_SModel->setHorizontalHeaderLabels(horizontalHeaders);

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab8::nmfEstimation_Tab8");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab08.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab8_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab8_Widget, tr("8. Model Review"));

    Estimation_Tab8_ModelReviewTV     = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab8_ModelReviewTV");
    Estimation_Tab8_LoadModelPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_LoadModelPB");
    Estimation_Tab8_ImportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_ImportPB");
    Estimation_Tab8_ExportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_ExportPB");
    Estimation_Tab8_GenerateSummaryPB = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_GenerateSummaryPB");
    Estimation_Tab8_PrevPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_PrevPB");
    Estimation_Tab8_DeletePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_DeletePB");
    Estimation_Tab8_LoadPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_LoadPB");
    Estimation_Tab8_SavePB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab8_SavePB");
    Estimation_Tab8_ShowHiddenCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab8_ShowHiddenCB");

    Estimation_Tab8_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab8_ModelReviewTV->setSortingEnabled(true);
    Estimation_Tab8_ModelReviewTV->horizontalHeader()->hide();
    Estimation_Tab8_ModelReviewTV->horizontalHeader()->setSectionsMovable(true);
    Estimation_Tab8_ModelReviewTV->setContextMenuPolicy(Qt::CustomContextMenu);

    initializeContextMenu();
    initializeWhatsThis();
    resizeColumns();
    initializeConnections();

    Estimation_Tab8_PrevPB->setText("\u25C1--");
}

nmfEstimation_Tab8::~nmfEstimation_Tab8()
{
}

void
nmfEstimation_Tab8::adjustColumnsForReadOnly()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());

    // Adjust all but the Notes column to be read only
    for (int row=0; row<smodel->rowCount(); ++row) {
        for (int col=0; col<smodel->columnCount(); ++col) {
            smodel->setItem(row,col,createNewModelReviewItem(col,smodel->index(row,col).data().toString()));
        }
    }
    Estimation_Tab8_ModelReviewTV->setModel(smodel);
    Estimation_Tab8_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab8_ModelReviewTV->resizeRowsToContents();
}

void
nmfEstimation_Tab8::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab8_ModelReviewTV});
}

QStandardItem*
nmfEstimation_Tab8::createNewModelReviewItem(const int& col,
                                             const QString& value)
{
    QStandardItem* item  = new QStandardItem(value);

    if (col == COLUMN_NAME) {
        item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    } else {
        item->setTextAlignment(Qt::AlignCenter);
    }
    if (col != COLUMN_NOTES) {
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }

    return item;
}

QString
nmfEstimation_Tab8::generateDatabaseSnapshot()
{
    std::string timestamp = m_Logger->getTimestamp(nmfConstants::TimestampWithUnderscore);

    return "dbsnap_" + QString::fromStdString(timestamp) + ".sql";
}

QString
nmfEstimation_Tab8::getEstimatedParameters(QStandardItemModel* smodel, const int& row)
{
    QString retv="";
    QString filename = "";
    QString fullPath;
    QString line;

    for (int i = 0; i < smodel->columnCount(); i++)
    {
        if (smodel->headerData(i, Qt::Horizontal).toString().trimmed() == "EstimatedParametersFile") {
            filename = smodel->index(row,i).data().toString().trimmed();
        }
    }

    if (! filename.isEmpty()) {
        fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
        fullPath = QDir(fullPath).filePath(filename);
std::cout << "Reading: " << fullPath.toStdString() << std::endl;
        QFile file(fullPath);
        QStringList parts;
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream inStream(&file);
            line = inStream.readLine().trimmed();
            if (line == "Estimated Parameters") {
                retv += "<table style=\"width:100%\">";
                while ( !inStream.atEnd()) {
                    line = inStream.readLine().trimmed();
                    if (line.isEmpty()) {
                        continue;
                    }
                    if (! line.contains(",")) { // this is a heading
                        retv += "<tr>";
                        retv += "<br><td style=\"text-align:left\"><strong>"+line+"</strong></td>";
                        retv += "<td></td>";
                        retv += "</tr>";
                    } else { // this is data
                        retv += "<tr>";
                        parts = line.split(",");
                        for (int i=0; i<parts.size(); ++i) {
                            retv += "<td style=\"text-align:center\">"+parts[i]+"</td>";
                        }
                        retv += "</tr>";
                    }
                }
                retv += "</table>";
            } else {
                m_Logger->logMsg(nmfConstants::Error,"\nIncorrect file type read in nmfEstimation_Tab8::getEstimatedParameters");
                return "";
            }
        }
    }

    return retv;
}

QString
nmfEstimation_Tab8::getModelEquation(QStandardItemModel* smodel, const int& row)
{
    bool isAggProd = false; // RSK fix this
    QString equation;
    QString key;
std::cout << "FIX AggProd hard coding" << std::endl;

    QString growthType      = smodel->index(row,4).data().toString().trimmed();
    QString harvestType     = smodel->index(row,5).data().toString().trimmed();
    QString competitionType = smodel->index(row,6).data().toString().trimmed();
    QString predationType   = smodel->index(row,7).data().toString().trimmed();

    nmfGrowthForm growthForm(growthType.toStdString());
    growthForm.setAggProd(isAggProd);
    equation  = QString::fromStdString(growthForm.getPrefix());
    equation += QString::fromStdString(growthForm.getExpression());
    key       = QString::fromStdString(growthForm.getKey());

    nmfHarvestForm harvestForm(harvestType.toStdString());
    harvestForm.setAggProd(isAggProd);
    equation += QString::fromStdString(harvestForm.getExpression());
    key      += QString::fromStdString(harvestForm.getKey());

    nmfCompetitionForm competitionForm(competitionType.toStdString());
    competitionForm.setAggProd(isAggProd);
    equation += QString::fromStdString(competitionForm.getExpression());
    key      += QString::fromStdString(competitionForm.getKey());

    nmfPredationForm predationForm(predationType.toStdString());
    predationForm.setAggProd(isAggProd);
    equation += QString::fromStdString(predationForm.getExpression());
    key      += QString::fromStdString(predationForm.getKey());

    return equation + "<br><br>" + key;
}

void
nmfEstimation_Tab8::initializeConnections()
{
    connect(Estimation_Tab8_ModelReviewTV,     SIGNAL(customContextMenuRequested(QPoint)),
            this,                              SLOT(callback_ContextMenu(QPoint)));
    connect(Estimation_Tab8_PrevPB,            SIGNAL(clicked()),
            this,                              SLOT(callback_PrevPB()));
    connect(Estimation_Tab8_LoadPB,            SIGNAL(clicked()),
            this,                              SLOT(callback_LoadPB()));
    connect(Estimation_Tab8_SavePB,            SIGNAL(clicked()),
            this,                              SLOT(callback_SavePB()));
    connect(Estimation_Tab8_LoadModelPB,       SIGNAL(clicked()),
            this,                              SLOT(callback_LoadModelPB()));
    connect(Estimation_Tab8_ImportPB,          SIGNAL(clicked()),
            this,                              SLOT(callback_ImportPB()));
    connect(Estimation_Tab8_ExportPB,          SIGNAL(clicked()),
            this,                              SLOT(callback_ExportPB()));
    connect(Estimation_Tab8_GenerateSummaryPB, SIGNAL(clicked()),
            this,                              SLOT(callback_GenerateSummaryPB()));
    connect(Estimation_Tab8_ShowHiddenCB,      SIGNAL(stateChanged(int)),
            this,                              SLOT(callback_ShowHiddenCB(int)));
    connect(m_hideAction,                      SIGNAL(triggered()),
            this,                              SLOT(callback_HideRows()));
    connect(m_showAction,                      SIGNAL(triggered(bool)),
            this,                              SLOT(callback_ShowRows()));
    connect(m_deleteAction,                    SIGNAL(triggered(bool)),
            this,                              SLOT(callback_DeleteRows()));
}

void
nmfEstimation_Tab8::initializeContextMenu()
{
    // Set up context menu and actions
    m_contextMenu  = new QMenu(Estimation_Tabs);
    m_deleteAction = new QAction("&Delete selected row(s)");
    m_hideAction   = new QAction("&Hide selected row(s)");
    m_showAction   = new QAction("&Show all rows");
    m_sepAction    = new QAction(this);
    m_sepAction->setSeparator(true);
    m_contextMenu->addAction(m_hideAction);
    m_contextMenu->addAction(m_showAction);
    m_contextMenu->addAction(m_sepAction);
    m_contextMenu->addAction(m_deleteAction);
}

void
nmfEstimation_Tab8::initializeWhatsThis()
{
    // Set up What's This message for the table
    QString msg  = "<html><strong><center>Model Review Table</center></strong><br>";
    msg += "When the user adds a model run to this model review table, all data necessary \
for the replication of the model are contained herein. Not all of the \
necessary data are displayed in this table, but they are saved along with the visible data.<br>\
<center>Data Interpretation</center><br>\
r² - the closer to 1, the better the fit<br>\
SSResiduals - the smaller the number, the better the fit<br>\
AIC - the smaller the number, the better the fit<br>\
Notes - user editable field for model details<br>\
</html>";
    Estimation_Tab8_ModelReviewTV->setWhatsThis(msg);
}

bool
nmfEstimation_Tab8::loadModel(QStandardItemModel* smodel, const int& row)
{
    int col = 9;
    nmfStructsQt::ModelReviewStruct modelReview;

    modelReview.ModelName                               = smodel->index(row, 0).data().toString().trimmed(); // Reload this model

    modelReview.ObjectiveCriterion                      = smodel->index(row,col++).data().toString().trimmed();
    modelReview.EstimationAlgorithm                     = smodel->index(row,col++).data().toString().trimmed();
    modelReview.MinimizerAlgorithm                      = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ScalingAlgorithm                        = smodel->index(row,col++).data().toString().trimmed();
    modelReview.DatabaseSnapshot                        = smodel->index(row,col++).data().toString().trimmed();

    modelReview.setToDeterministicBees                  = smodel->index(row,col++).data().toString().trimmed();
    modelReview.maxGenerations                          = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numBees                                 = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numBestSites                            = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numEliteSites                           = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numEliteBees                            = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numOtherBees                            = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.neighborhoodSize                        = smodel->index(row,col++).data().toString().remove(",").toInt();
    modelReview.numSubRuns                              = smodel->index(row,col++).data().toString().remove(",").toInt();

    modelReview.setToDeterministicNLopt                 = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.isStopAfterValue                        = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.stopAfterValue                          = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.isStopAfterTime                         = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.stopAfterTime                           = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.isStopAfterIter                         = smodel->index(row,col++).data().toString().remove(",").trimmed();
    modelReview.stopAfterIter                           = smodel->index(row,col++).data().toString().remove(",").trimmed();

    modelReview.isEstInitialBiomassEnabled              = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstInitialBiomassChecked              = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstGrowthRateEnabled                  = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstGrowthRateChecked                  = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstGrowthRateShapeEnabled             = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstGrowthRateShapeChecked             = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCarryingCapacityEnabled            = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCarryingCapacityChecked            = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCatchabilityEnabled                = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCatchabilityChecked                = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaEnabled            = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaChecked            = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesEnabled      = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesChecked      = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsEnabled       = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsChecked       = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsEnabled = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsChecked = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationRhoEnabled                = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationRhoChecked                = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationHandlingEnabled           = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationHandlingChecked           = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationExponentEnabled           = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstPredationExponentChecked           = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstSurveyQEnabled                     = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEstSurveyQChecked                     = smodel->index(row,col++).data().toString().trimmed();

    modelReview.isAMultiRun                             = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ensembleAveragingAlgorithm              = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ensembleAverageBy                       = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ensembleUsingBy                         = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ensembleUsingAmountValue                = smodel->index(row,col++).data().toString().trimmed();
    modelReview.isEnsembleUsingPct                      = smodel->index(row,col++).data().toString().trimmed();
    modelReview.ensembleFilename                        = smodel->index(row,col++).data().toString().trimmed();

    if (modelReview.DatabaseSnapshot.isEmpty()) {
        QMessageBox::warning(Estimation_Tabs, "Warning",
                             QString::fromStdString("\nCan't import model. Most common reason is due to it being an HPC run.\n"),
                             QMessageBox::Ok);
        return false;
    }

    emit LoadFromModelReview(modelReview);

    return true;
}

void
nmfEstimation_Tab8::loadWidgets()
{
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    int NumColumns = m_ModelReviewFields.size();
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString fileName = QDir(dataPath).filePath("_TempFileJustForRefreshing.csv");
    QString valueStr;
    QString valueWithComma;
    QStandardItem *item;
    QStringList HeaderLabels;

    for (std::string field : m_ModelReviewFields) {
        if (field == "rSquared") {
            HeaderLabels << "r²";
        } else {
            HeaderLabels << QString::fromStdString(field);
        }
    }

    readSettings();

    queryStr = "SELECT ModelName,rSquared,SSResiduals,AIC,\
GrowthForm,HarvestForm,CompetitonForm,PredationForm,numRuns,\
ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,DatabaseSnapshot,Notes,\
isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites,\
numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt,\
isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter,\
StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked,\
isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstGrowthRateShapeEnabled,isEstGrowthRateShapeChecked,\
isEstCarryingCapacityEnabled,isEstCarryingCapacityChecked,\
isEstCatchabilityEnabled,isEstCatchabilityChecked,\
isEstCompAlphaEnabled,isEstCompAlphaChecked,isEstCompBetaSpeciesEnabled,\
isEstCompBetaSpeciesChecked,isEstCompBetaGuildsEnabled,isEstCompBetaGuildsChecked,\
isEstCompBetaGuildsGuildsEnabled,isEstCompBetaGuildsGuildsChecked,isEstPredRhoEnabled,\
isEstPredRhoChecked,isEstPredHandlingEnabled,isEstPredHandlingChecked,\
isEstPredExponentEnabled,isEstPredExponentChecked,isEstSurveyQEnabled,\
isEstSurveyQChecked,isAMultiRun,AveAlg,HowToAverage,UsingWhat,UsingNumber,isUsingPct,\
EnsembleFile,EstimatedParametersFile FROM " +
            nmfConstantsMSSPM::TableModelReview + " WHERE ProjectName = '" + m_ProjectName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, m_ModelReviewFields);
    int NumRecords = dataMap["ModelName"].size();

    if (m_SModel != nullptr) {
        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());
    } else {
        return;
    }
    m_SModel->clear();
    m_SModel->setRowCount(NumRecords);
    m_SModel->setColumnCount(NumColumns);

    if (NumRecords == 0)  {
        return;
    }

    for (int row=0; row<NumRecords; ++row) {
        for (int col=0; col<NumColumns; ++col) {
            valueStr = QString::fromStdString(dataMap[m_ModelReviewFields[col]][row]);
            if ((col == nmfConstantsMSSPM::Model_Review_Column_rSquared) ||
                (col == nmfConstantsMSSPM::Model_Review_Column_AIC)) {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            valueStr.toDouble(),m_NumSignificantDigits,3);
                valueStr = valueWithComma;
            } else if (col == nmfConstantsMSSPM::Model_Review_Column_SSResiduals) {
                valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                            valueStr.toDouble(),m_NumSignificantDigits,-4);
                valueStr = valueWithComma;
            }
            item = new QStandardItem(valueStr);
            item->setTextAlignment(Qt::AlignCenter);
            if (col != nmfConstantsMSSPM::Model_Review_Column_Last_Visible) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // read-only
            }
            m_SModel->setItem(row,col,item);
        }
    }
    m_SModel->setHorizontalHeaderLabels(HeaderLabels);
    Estimation_Tab8_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab8_ModelReviewTV->horizontalHeader()->show();
    Estimation_Tab8_ModelReviewTV->resizeColumnsToContents();

    callback_ShowHiddenCB(Estimation_Tab8_ShowHiddenCB->checkState());

}

void
nmfEstimation_Tab8::loadWidgetsNoSignificantDigits()
{
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    int NumColumns = m_ModelReviewFields.size();
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString fileName = QDir(dataPath).filePath("_TempFileJustForRefreshing.csv");
    QString valueStr;
    QString valueWithComma;
    QStandardItem *item;
    QStringList HeaderLabels;

    for (std::string field : m_ModelReviewFields) {
        if (field == "rSquared") {
            HeaderLabels << "r²";
        } else {
            HeaderLabels << QString::fromStdString(field);
        }
    }

    readSettings();

    queryStr = "SELECT ModelName,rSquared,SSResiduals,AIC,\
GrowthForm,HarvestForm,CompetitonForm,PredationForm,numRuns,\
ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,DatabaseSnapshot,Notes,\
isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites,\
numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt,\
isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter,\
StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked,\
isEstGrowthRateEnabled,isEstGrowthRateChecked,\
isEstGrowthRateShapeEnabled,isEstGrowthRateShapeChecked,\
isEstCarryingCapacityEnabled,isEstCarryingCapacityChecked,\
isEstCatchabilityEnabled,isEstCatchabilityChecked,\
isEstCompAlphaEnabled,isEstCompAlphaChecked,isEstCompBetaSpeciesEnabled,\
isEstCompBetaSpeciesChecked,isEstCompBetaGuildsEnabled,isEstCompBetaGuildsChecked,\
isEstCompBetaGuildsGuildsEnabled,isEstCompBetaGuildsGuildsChecked,isEstPredRhoEnabled,\
isEstPredRhoChecked,isEstPredHandlingEnabled,isEstPredHandlingChecked,\
isEstPredExponentEnabled,isEstPredExponentChecked,isEstSurveyQEnabled,\
isEstSurveyQChecked,isAMultiRun,AveAlg,HowToAverage,UsingWhat,UsingNumber,isUsingPct,\
EnsembleFile,EstimatedParametersFile FROM " +
            nmfConstantsMSSPM::TableModelReview + " WHERE ProjectName = '" + m_ProjectName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, m_ModelReviewFields);
    int NumRecords = dataMap["ModelName"].size();

    if (m_SModel != nullptr) {
        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());
    } else {
        return;
    }
    m_SModel->clear();
    m_SModel->setRowCount(NumRecords);
    m_SModel->setColumnCount(NumColumns);

    if (NumRecords == 0)  {
        return;
    }

    for (int row=0; row<NumRecords; ++row) {
        for (int col=0; col<NumColumns; ++col) {
            valueStr = QString::fromStdString(dataMap[m_ModelReviewFields[col]][row]);
            item = new QStandardItem(valueStr);
            item->setTextAlignment(Qt::AlignCenter);
            m_SModel->setItem(row,col,item);
        }
    }
    m_SModel->setHorizontalHeaderLabels(HeaderLabels);
    Estimation_Tab8_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab8_ModelReviewTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab8::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab8::removeFilesAssociatedWithRow(
        QStandardItemModel* smodel,
        const int& row)
{
    QString fullPath;
    QString filename;
    QString fullFilename;

    // Remove Database Snapshot file saved with this row
    fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("databases");
    filename = smodel->index(row,nmfConstantsMSSPM::Model_Review_Column_DB_Snapshot).data().toString().trimmed();
//qDebug() << "filename: " << filename;
//qDebug() << "field-1: " << smodel->index(row,nmfConstantsMSSPM::Model_Review_Column_DB_Snapshot).data().toString().trimmed();
    fullFilename = QDir(fullPath).filePath(filename);
    if (QFile::remove(fullFilename)) {
        m_Logger->logMsg(nmfConstants::Normal,"Deleted file: "+fullFilename.toStdString());
    } else if (! filename.isEmpty()) {
        m_Logger->logMsg(nmfConstants::Error,"Error deleting Snapshot file: "+fullFilename.toStdString());
    }

    // Remove Estimated Parameters file saved with this row
    fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    filename = smodel->index(row,nmfConstantsMSSPM::Model_Review_Column_Estimated_Parameters).data().toString().trimmed();
    fullFilename = QDir(fullPath).filePath(filename);
    if (QFile::remove(fullFilename)) {
        m_Logger->logMsg(nmfConstants::Normal,"Deleted file: "+fullFilename.toStdString());
    } else {
        m_Logger->logMsg(nmfConstants::Error,"Error deleting Estimated Parameters file: "+fullFilename.toStdString());
    }
}

void
nmfEstimation_Tab8::resizeColumns()
{
    Estimation_Tab8_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab8_ModelReviewTV->resizeRowsToContents();
}

bool
nmfEstimation_Tab8::saveModelReviewTable()
{
    int NumRows;
    int NumCols;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string cmd;
    std::string errorMsg;
    QString valueStr;

    if (m_SModel == nullptr) {
        return false;
    }

    // Clear current ModelReview table
    m_DatabasePtr->clearTable(m_Logger,nmfConstantsMSSPM::TableModelReview,m_ProjectName);

    m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());
    NumRows = m_SModel->rowCount();
    NumCols = m_SModel->columnCount();
    if (NumRows == 0) {
        return false;
    }

//    // Turn off significant digits in order to save with highest precision
//    bool flippedSignificantDigits = false;
//    int remNumSignificantDigits = 0;
//    if (m_NumSignificantDigits >= 0) {
//        remNumSignificantDigits = m_NumSignificantDigits;
//        m_NumSignificantDigits = -1;
//        flippedSignificantDigits = true;
//        loadWidgets();
//    }

    // Save Qt table data to database table
    cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableModelReview +
            " (ProjectName,ModelNumber,ModelName,rSquared,SSResiduals,AIC," +
            "GrowthForm,HarvestForm,CompetitonForm,PredationForm,numRuns," +
            "ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,DatabaseSnapshot,Notes," +
            "isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites," +
            "numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt," +
            "isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter," +
            "StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked," +
            "isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstGrowthRateShapeEnabled,isEstGrowthRateShapeChecked," +
            "isEstCarryingCapacityEnabled,isEstCarryingCapacityChecked," +
            "isEstCatchabilityEnabled,isEstCatchabilityChecked," +
            "isEstCompAlphaEnabled,isEstCompAlphaChecked,isEstCompBetaSpeciesEnabled," +
            "isEstCompBetaSpeciesChecked,isEstCompBetaGuildsEnabled,isEstCompBetaGuildsChecked," +
            "isEstCompBetaGuildsGuildsEnabled,isEstCompBetaGuildsGuildsChecked,isEstPredRhoEnabled," +
            "isEstPredRhoChecked,isEstPredHandlingEnabled,isEstPredHandlingChecked," +
            "isEstPredExponentEnabled,isEstPredExponentChecked,isEstSurveyQEnabled," +
            "isEstSurveyQChecked,isAMultiRun,AveAlg,HowToAverage,UsingWhat,UsingNumber,isUsingPct," +
            "EnsembleFile,EstimatedParametersFile) VALUES ";
    for (int row=0; row<NumRows; ++row) {
        cmd += "('" + m_ProjectName + "','" + std::to_string(row) + "'";
        for (int col=0; col<NumCols; ++col) {
            if ((col == nmfConstantsMSSPM::Model_Review_Column_rSquared)    ||
                (col == nmfConstantsMSSPM::Model_Review_Column_SSResiduals) ||
                (col == nmfConstantsMSSPM::Model_Review_Column_AIC)) {
                valueStr = m_SModel->index(row, col).data().toString().replace(",","");
                cmd += ","  + valueStr.toStdString();
//              cmd += ","  + std::to_string(valueStr.toDouble());
            } else {
                cmd += ",'" + m_SModel->index(row, col).data().toString().toStdString() + "'";
            }
        }
        cmd += "),";
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab8::saveModelReviewTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

//    // Turn on significant digits if they were turned off previously in this method
//    if (flippedSignificantDigits) {
//        m_NumSignificantDigits = remNumSignificantDigits;
//        loadWidgets();
//    }

    return true;
}

void
nmfEstimation_Tab8::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
//  settings->setValue("FontSize",   Estimation_Tab8_FontSizeCMB->currentText());
    settings->endGroup();

    delete settings;
}

bool
nmfEstimation_Tab8::showHiddenFields()
{
    return Estimation_Tab8_ShowHiddenCB->isChecked();
}

void
nmfEstimation_Tab8::updateModelReviewTable(const QStringList& rowList)
{
    int row;
    int NumRows;
    int NumCols = (int)rowList.size();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string cmd;
    std::string errorMsg;
    QString valueStr;

    // Find out how many rows are currently in the modelreview table
    queryStr = "SELECT ModelName FROM " + nmfConstantsMSSPM::TableModelReview +
               " WHERE ProjectName = '" + m_ProjectName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, {"ModelName"});
    NumRows  = (int)dataMap["ModelName"].size();

    // Save Qt table data to database table
    row  = NumRows;
    cmd  = "INSERT INTO " + nmfConstantsMSSPM::TableModelReview +
            " (ProjectName,ModelNumber,ModelName,rSquared,SSResiduals,AIC," +
            "GrowthForm,HarvestForm,CompetitonForm,PredationForm,numRuns," +
            "ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,DatabaseSnapshot,Notes," +
            "isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites," +
            "numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt," +
            "isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter," +
            "StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked," +
            "isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstGrowthRateShapeEnabled,isEstGrowthRateShapeChecked," +
            "isEstCarryingCapacityEnabled,isEstCarryingCapacityChecked," +
            "isEstCatchabilityEnabled,isEstCatchabilityChecked," +
            "isEstCompAlphaEnabled,isEstCompAlphaChecked,isEstCompBetaSpeciesEnabled," +
            "isEstCompBetaSpeciesChecked,isEstCompBetaGuildsEnabled,isEstCompBetaGuildsChecked," +
            "isEstCompBetaGuildsGuildsEnabled,isEstCompBetaGuildsGuildsChecked,isEstPredRhoEnabled," +
            "isEstPredRhoChecked,isEstPredHandlingEnabled,isEstPredHandlingChecked," +
            "isEstPredExponentEnabled,isEstPredExponentChecked,isEstSurveyQEnabled," +
            "isEstSurveyQChecked,isAMultiRun,AveAlg,HowToAverage,UsingWhat,UsingNumber,isUsingPct," +
            "EnsembleFile,EstimatedParametersFile) VALUES ";
        cmd += "('" + m_ProjectName + "','" + std::to_string(row) + "'";
        for (int col=0; col<NumCols; ++col) {
            if ((col == nmfConstantsMSSPM::Model_Review_Column_rSquared)    ||
                (col == nmfConstantsMSSPM::Model_Review_Column_SSResiduals) ||
                (col == nmfConstantsMSSPM::Model_Review_Column_AIC)) {
                valueStr = rowList[col];
//              valueStr.replace(",","");
                cmd += ","  + valueStr.replace(",","").toStdString();
//              cmd += ","  + std::to_string(valueStr.toDouble());
            } else {
                cmd += ",'" + rowList[col].toStdString() + "'";
            }
        }
        cmd += "),";
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab8::updateModelReviewTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }
    callback_LoadPB();
}

void
nmfEstimation_Tab8::callback_ContextMenu(QPoint pos)
{
    m_contextMenu->popup(Estimation_Tab8_ModelReviewTV->viewport()->mapToGlobal(pos));
}

void
nmfEstimation_Tab8::callback_DeleteRows()
{
    int rowIndex;
    QModelIndexList indexes = Estimation_Tab8_ModelReviewTV->selectionModel()->selectedRows();
    int numRowsToDelete = indexes.count();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());

    if (numRowsToDelete > 0) {
        std::string msg  = "\nOK to delete the " + std::to_string(numRowsToDelete) + " selected row(s)?\n";
        QMessageBox::StandardButton reply = QMessageBox::warning(Estimation_Tabs, tr("Delete"),
                                                                 tr(msg.c_str()),QMessageBox::No|QMessageBox::Yes,QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            for (int row=indexes.count(); row>0; row--) {
                rowIndex = indexes.at(row-1).row();
                removeFilesAssociatedWithRow(smodel,rowIndex);
                smodel->removeRow(rowIndex,QModelIndex());
            }
        }
    } else {
        std::string msg  = "\nOK to delete all rows?\n";
        QMessageBox::StandardButton reply = QMessageBox::warning(Estimation_Tabs, tr("Delete"),
                                                                 tr(msg.c_str()),QMessageBox::No|QMessageBox::Yes,QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            int totalNumRows = Estimation_Tab8_ModelReviewTV->model()->rowCount();
            removeFilesAssociatedWithRow(smodel,0);
            smodel->removeRows(0,totalNumRows);
        }
    }

    saveModelReviewTable();
}

void
nmfEstimation_Tab8::callback_ExportPB()
{
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameModelReview));
    QString fileName = QFileDialog::getSaveFileName(Estimation_Tabs, tr("Export Model Review File"),
                               filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,"Model Review","",Estimation_Tab8_ModelReviewTV,
                                       nmfConstantsMSSPM::Dont_Query_User_For_Filename,
                                       nmfConstantsMSSPM::RemoveCommas,
                                       fileName,
                                       nmfConstantsMSSPM::VerboseOn);
    }
}

void
nmfEstimation_Tab8::callback_GenerateSummaryPB()
{
    int row;
    QModelIndex index;
    QModelIndexList indexes = Estimation_Tab8_ModelReviewTV->selectionModel()->selectedRows();
    QString html;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());

    if (indexes.size() == 0) {
        QMessageBox::warning(Estimation_Tabs, "Model Summary", "\nPlease select one model (i.e., row) for which to generate a summary.\n", QMessageBox::Ok);
        return;
    }

    row   = indexes[0].row();
    index = smodel->index(row,0);
    html  = "<html><strong><center>Model Review Summary</center></strong><br>";
    html += "<br><strong>Model Name:</strong>&nbsp;&nbsp;" + index.data().toString() + "<br>";
    html += "<br><strong>Equation:</strong>&nbsp;&nbsp;"   + getModelEquation(smodel,row);
    html += "<br><strong>Estimated Parameters:</strong>"   + getEstimatedParameters(smodel,row);

    // Create and show dialog with the html summary text
    QDialog* dlg       = new QDialog(Estimation_Tabs);
    QVBoxLayout* vlayt = new QVBoxLayout;
    QHBoxLayout* hlayt = new QHBoxLayout;
    QTextEdit* htmlTE  = new QTextEdit(html);
    htmlTE->setFontPointSize(11);
    QPushButton* okPB  = new QPushButton("Ok");
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hlayt->addWidget(okPB);
    hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    vlayt->addWidget(htmlTE);
    vlayt->addLayout(hlayt);
    dlg->setLayout(vlayt);
    dlg->resize(1100,800);
    connect(okPB, SIGNAL(clicked()), dlg, SLOT(reject()));
    dlg->exec();
}

void
nmfEstimation_Tab8::callback_HideRows()
{
    QItemSelectionModel *select = Estimation_Tab8_ModelReviewTV->selectionModel();

    if (select->hasSelection()) { //check if has selection
        QModelIndexList selectedRows = select->selectedRows();
        int numRows = (int)selectedRows.size();
        for (int row=numRows-1; row>=0; --row) {
            Estimation_Tab8_ModelReviewTV->hideRow(selectedRows[row].row());
        }
    }
}

void
nmfEstimation_Tab8::callback_ImportPB()
{
    int numRows = 0;
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameModelReview));
    QString fileName = QFileDialog::getOpenFileName(Estimation_Tabs, tr("Import Model Review File"),
                               filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        int noSigDig = -1;
        bool loadOK = nmfUtilsQt::loadModelFromCSVFile(m_Logger,m_ProjectDir,
                                                       "Model Review",Estimation_Tab8_ModelReviewTV,
                                                       fileName,numRows,noSigDig);
        if (loadOK && (numRows > 2)) {
            callback_ShowHiddenCB(Estimation_Tab8_ShowHiddenCB->checkState());
        } else if (numRows < 2) {
            QMessageBox::warning(Estimation_Tabs, "Warning",
                                 "\nEmpty Model Review file.\n",
                                 QMessageBox::Ok);
            return;
        } else {
            QMessageBox::warning(Estimation_Tabs, "Warning",
                                 "\nPlease make sure you're opening a Model Review file.\n",
                                 QMessageBox::Ok);
            return;
        }
        QMessageBox::information(Estimation_Tabs, "Import Successful",
                                 "\nThe CSV data file has been successfully imported.\n",
                                 QMessageBox::Ok);

        adjustColumnsForReadOnly();
        Estimation_Tab8_ModelReviewTV->horizontalHeader()->show();
    }
}

void
nmfEstimation_Tab8::callback_LoadModelPB()
{
    int row;
    QModelIndexList indexes = Estimation_Tab8_ModelReviewTV->selectionModel()->selectedRows();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab8_ModelReviewTV->model());

    if (indexes.size() == 0) {
        QMessageBox::warning(Estimation_Tabs, "Model Load", "\nPlease select one model (i.e., row) to load.\n", QMessageBox::Ok);
        return;
    }

    row = indexes[0].row();
    std::string msg  = "\nLoading the selected model will load its model data over the currently ";
    msg += "loaded model data. This will cause any unsaved model data to be lost.\n";
    msg += "\nOK to continue?\n";

    QMessageBox::StandardButton reply = QMessageBox::warning(Estimation_Tabs, tr("Model Load"), tr(msg.c_str()),
        QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }

    bool loadOK = loadModel(smodel,row);

    if (loadOK) {
        msg  = "\nModel loaded. Switch to Run page?\n";
        reply = QMessageBox::question(Estimation_Tabs, tr("Model Loaded"), tr(msg.c_str()),
                                      QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            callback_PrevPB();
        }
    }
}

void
nmfEstimation_Tab8::callback_LoadPB()
{
    loadWidgets();
    callback_ShowHiddenCB(Estimation_Tab8_ShowHiddenCB->checkState());
}

void
nmfEstimation_Tab8::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab8::callback_SavePB()
{
    if (saveModelReviewTable()) {
        QMessageBox::information(Estimation_Tabs, "Save",
            "\nSuccessfully saved table: "+QString::fromStdString(nmfConstantsMSSPM::TableModelReview),
             QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab8::callback_ShowHiddenCB(int state)
{
    bool hideField;
    for (int i=0; i<Estimation_Tab8_ModelReviewTV->model()->columnCount(); ++i) {
        hideField = (state==Qt::Unchecked && i>nmfConstantsMSSPM::Model_Review_Column_Last_Visible);
        Estimation_Tab8_ModelReviewTV->setColumnHidden(i,hideField);
    }
    resizeColumns();
}

void
nmfEstimation_Tab8::callback_ShowRows()
{
    int numRows = (int)Estimation_Tab8_ModelReviewTV->model()->rowCount();

    for (int row=0; row<numRows; ++row) {
        Estimation_Tab8_ModelReviewTV->showRow(row);
    }
}
