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
        {"Model Name","r²","SSResiduals","AIC",
         "Growth Form","Harvest Form","Competition Form","Predation Form",
         "Num Runs","Objective Criterion","Estimation Algorithm","Minimizer","Scaling",
         "Notes",
         "isDeterministicBees","maxGenerations","numBees","numBestSites","numEliteSites", // 14
         "numEliteBees","numOtherBees","neighborhoodSize","numSubRuns",                   // 19
         "isDeterministicNLopt","isStopAfterValue","StopAfterValue",                      // 23
         "isStopAfterTime","StopAfterTime","isStopAfterIter","StopAfterIter",
         "isEstInitBiomassEnabled","isEstInitBiomassChecked",
         "isEstGrowthRateEnabled","isEstGrowthRateChecked",
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
    Estimation_Tab7_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab7_Widget, tr("8. Model Review"));

    Estimation_Tab7_ModelReviewTV     = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab7_ModelReviewTV");
    Estimation_Tab7_LoadModelPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_LoadModelPB");
    Estimation_Tab7_ImportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_ImportPB");
    Estimation_Tab7_ExportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_ExportPB");
    Estimation_Tab7_GenerateSummaryPB = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_GenerateSummaryPB");
    Estimation_Tab7_PrevPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_PrevPB");
    Estimation_Tab7_DeletePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_DeletePB");
    Estimation_Tab7_LoadPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_LoadPB");
    Estimation_Tab7_SavePB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_SavePB");
    Estimation_Tab7_ShowHiddenCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab7_ShowHiddenCB");

    Estimation_Tab7_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab7_ModelReviewTV->setSortingEnabled(true);
    Estimation_Tab7_ModelReviewTV->horizontalHeader()->hide();

    resizeColumns();

    connect(Estimation_Tab7_PrevPB,            SIGNAL(clicked()),
            this,                              SLOT(callback_PrevPB()));
    connect(Estimation_Tab7_DeletePB,          SIGNAL(clicked()),
            this,                              SLOT(callback_DeletePB()));
    connect(Estimation_Tab7_LoadPB,            SIGNAL(clicked()),
            this,                              SLOT(callback_LoadPB()));
    connect(Estimation_Tab7_SavePB,            SIGNAL(clicked()),
            this,                              SLOT(callback_SavePB()));
    connect(Estimation_Tab7_LoadModelPB,       SIGNAL(clicked()),
            this,                              SLOT(callback_LoadModelPB()));
    connect(Estimation_Tab7_ImportPB,          SIGNAL(clicked()),
            this,                              SLOT(callback_ImportPB()));
    connect(Estimation_Tab7_ExportPB,          SIGNAL(clicked()),
            this,                              SLOT(callback_ExportPB()));
    connect(Estimation_Tab7_GenerateSummaryPB, SIGNAL(clicked()),
            this,                              SLOT(callback_GenerateSummaryPB()));
    connect(Estimation_Tab7_ShowHiddenCB,      SIGNAL(stateChanged(int)),
            this,                              SLOT(callback_ShowHiddenCB(int)));

    Estimation_Tab7_PrevPB->setText("\u25C1--");

    callback_ShowHiddenCB(Estimation_Tab7_ShowHiddenCB->isChecked());
}


nmfEstimation_Tab8::~nmfEstimation_Tab8()
{
}

bool
nmfEstimation_Tab8::showHiddenFields()
{
    return Estimation_Tab7_ShowHiddenCB->isChecked();
}

void
nmfEstimation_Tab8::resizeColumns()
{
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab7_ModelReviewTV->resizeRowsToContents();
}

void
nmfEstimation_Tab8::callback_ShowHiddenCB(int state)
{
    bool hideField;
    for (int i=0; i<Estimation_Tab7_ModelReviewTV->model()->columnCount(); ++i) {
        hideField = (state==Qt::Unchecked && i>nmfConstantsMSSPM::Model_Review_Column_Last_Visible);
        Estimation_Tab7_ModelReviewTV->setColumnHidden(i,hideField);
    }
    resizeColumns();
}

void
nmfEstimation_Tab8::callback_LoadModelPB()
{
    int row;
    QModelIndexList indexes = Estimation_Tab7_ModelReviewTV->selectionModel()->selectedRows();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());

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

    loadModel(smodel,row);

    msg  = "\nModel loaded. Switch to Run page?\n";
    reply = QMessageBox::question(Estimation_Tabs, tr("Model Loaded"), tr(msg.c_str()),
        QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        callback_PrevPB();
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
                                                       "Model Review",Estimation_Tab7_ModelReviewTV,
                                                       fileName,numRows,noSigDig);
        if (loadOK && (numRows > 2)) {
            callback_ShowHiddenCB(Estimation_Tab7_ShowHiddenCB->checkState());
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
        adjustColumnsForReadOnly();
        Estimation_Tab7_ModelReviewTV->horizontalHeader()->show();
    }
}

void
nmfEstimation_Tab8::callback_ExportPB()
{
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameModelReview));
    QString fileName = QFileDialog::getSaveFileName(Estimation_Tabs, tr("Export Model Review File"),
                               filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,"Model Review","",Estimation_Tab7_ModelReviewTV,
                                       nmfConstantsMSSPM::Dont_Query_User_For_Filename,
                                       nmfConstantsMSSPM::RemoveCommas,
                                       fileName,
                                       nmfConstantsMSSPM::VerboseOn);
    }
}

void
nmfEstimation_Tab8::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab8::callback_DeletePB()
{
    QModelIndexList indexes = Estimation_Tab7_ModelReviewTV->selectionModel()->selectedRows();
    int numRowsToDelete = indexes.count();

    if (numRowsToDelete > 0) {
        for (int row=indexes.count(); row>0; row--) {
            Estimation_Tab7_ModelReviewTV->model()->removeRow(indexes.at(row-1).row(),QModelIndex());
        }
    } else {
        std::string msg  = "\nOK to delete all rows?\n";
        QMessageBox::StandardButton reply = QMessageBox::warning(Estimation_Tabs, tr("Delete"),
            tr(msg.c_str()),QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            int totalNumRows = Estimation_Tab7_ModelReviewTV->model()->rowCount();
            Estimation_Tab7_ModelReviewTV->model()->removeRows(0,totalNumRows);
        }
    }

    saveModelReviewTable();
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
std::cout << "fp: " << fullPath.toStdString() << std::endl;
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


void
nmfEstimation_Tab8::callback_LoadPB()
{
    loadWidgets();
    callback_ShowHiddenCB(Estimation_Tab7_ShowHiddenCB->checkState());
}

void
nmfEstimation_Tab8::callback_SavePB()
{
    saveModelReviewTable();
}

void
nmfEstimation_Tab8::loadModel(QStandardItemModel* smodel, const int& row)
{
    nmfStructsQt::ModelReviewStruct modelReview;

    modelReview.ModelName                               = smodel->index(row, 0).data().toString().trimmed(); // Reload this model

    modelReview.ObjectiveCriterion                      = smodel->index(row, 9).data().toString().trimmed();
    modelReview.EstimationAlgorithm                     = smodel->index(row,10).data().toString().trimmed();
    modelReview.MinimizerAlgorithm                      = smodel->index(row,11).data().toString().trimmed();
    modelReview.ScalingAlgorithm                        = smodel->index(row,12).data().toString().trimmed();

    modelReview.setToDeterministicBees                  = smodel->index(row,14).data().toString().trimmed();
    modelReview.maxGenerations                          = smodel->index(row,15).data().toString().remove(",").toInt();
    modelReview.numBees                                 = smodel->index(row,16).data().toString().remove(",").toInt();
    modelReview.numBestSites                            = smodel->index(row,17).data().toString().remove(",").toInt();
    modelReview.numEliteSites                           = smodel->index(row,18).data().toString().remove(",").toInt();
    modelReview.numEliteBees                            = smodel->index(row,19).data().toString().remove(",").toInt();
    modelReview.numOtherBees                            = smodel->index(row,20).data().toString().remove(",").toInt();
    modelReview.neighborhoodSize                        = smodel->index(row,21).data().toString().remove(",").toInt();
    modelReview.numSubRuns                              = smodel->index(row,22).data().toString().remove(",").toInt();

    modelReview.setToDeterministicNLopt                 = smodel->index(row,23).data().toString().remove(",").trimmed();
    modelReview.isStopAfterValue                        = smodel->index(row,24).data().toString().remove(",").trimmed();
    modelReview.stopAfterValue                          = smodel->index(row,25).data().toString().remove(",").trimmed();
    modelReview.isStopAfterTime                         = smodel->index(row,26).data().toString().remove(",").trimmed();
    modelReview.stopAfterTime                           = smodel->index(row,27).data().toString().remove(",").trimmed();
    modelReview.isStopAfterIter                         = smodel->index(row,28).data().toString().remove(",").trimmed();
    modelReview.stopAfterIter                           = smodel->index(row,29).data().toString().remove(",").trimmed();

    modelReview.isEstInitialBiomassEnabled              = smodel->index(row,30).data().toString().trimmed();
    modelReview.isEstInitialBiomassChecked              = smodel->index(row,31).data().toString().trimmed();
    modelReview.isEstGrowthRateEnabled                  = smodel->index(row,32).data().toString().trimmed();
    modelReview.isEstGrowthRateChecked                  = smodel->index(row,33).data().toString().trimmed();
    modelReview.isEstCarryingCapacityEnabled            = smodel->index(row,34).data().toString().trimmed();
    modelReview.isEstCarryingCapacityChecked            = smodel->index(row,35).data().toString().trimmed();
    modelReview.isEstCatchabilityEnabled                = smodel->index(row,36).data().toString().trimmed();
    modelReview.isEstCatchabilityChecked                = smodel->index(row,37).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaEnabled            = smodel->index(row,38).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaChecked            = smodel->index(row,39).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesEnabled      = smodel->index(row,40).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesChecked      = smodel->index(row,41).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsEnabled       = smodel->index(row,42).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsChecked       = smodel->index(row,43).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsEnabled = smodel->index(row,44).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsChecked = smodel->index(row,45).data().toString().trimmed();
    modelReview.isEstPredationRhoEnabled                = smodel->index(row,46).data().toString().trimmed();
    modelReview.isEstPredationRhoChecked                = smodel->index(row,47).data().toString().trimmed();
    modelReview.isEstPredationHandlingEnabled           = smodel->index(row,48).data().toString().trimmed();
    modelReview.isEstPredationHandlingChecked           = smodel->index(row,49).data().toString().trimmed();
    modelReview.isEstPredationExponentEnabled           = smodel->index(row,50).data().toString().trimmed();
    modelReview.isEstPredationExponentChecked           = smodel->index(row,51).data().toString().trimmed();
    modelReview.isEstSurveyQEnabled                     = smodel->index(row,52).data().toString().trimmed();
    modelReview.isEstSurveyQChecked                     = smodel->index(row,53).data().toString().trimmed();

    modelReview.isAMultiRun                             = smodel->index(row,54).data().toString().trimmed();
    modelReview.ensembleAveragingAlgorithm              = smodel->index(row,55).data().toString().trimmed();
    modelReview.ensembleAverageBy                       = smodel->index(row,56).data().toString().trimmed();
    modelReview.ensembleUsingBy                         = smodel->index(row,57).data().toString().trimmed();
    modelReview.ensembleUsingAmountValue                = smodel->index(row,58).data().toString().trimmed();
    modelReview.isEnsembleUsingPct                      = smodel->index(row,59).data().toString().trimmed();
    modelReview.ensembleFilename                        = smodel->index(row,60).data().toString().trimmed();
std::cout << "===> modelReview.isAMultiRun: " << modelReview.isAMultiRun.toStdString() << std::endl;
    emit LoadFromModelReview(modelReview);
}

void
nmfEstimation_Tab8::callback_GenerateSummaryPB()
{
    int row;
    QModelIndex index;
    QModelIndexList indexes = Estimation_Tab7_ModelReviewTV->selectionModel()->selectedRows();
    QString html;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());

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
            "ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,Notes," +
            "isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites," +
            "numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt," +
            "isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter," +
            "StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked," +
            "isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstCarryingCapacityEnabled," +
            "isEstCarryingCapacityChecked,isEstCatchabilityEnabled,isEstCatchabilityChecked," +
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
                valueStr.replace(",","");
                cmd += ","  + std::to_string(valueStr.toDouble());
            } else {
                cmd += ",'" + rowList[col].toStdString() + "'";
            }
        }
        cmd += "),";
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab8::saveModelReviewTable: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return;
    }
    callback_LoadPB();
}

//void
//nmfEstimation_Tab8::updateReviewList(const QStringList& rowList)
//{
//    bool hideField;
//    QList<QStandardItem* > itemList;
//    int numCols = rowList.size();
//    QString itemStr;
//    QStringList HeaderLabels;
//    for (std::string field : m_ModelReviewFields) {
//        if (field == "rSquared") {
//            HeaderLabels << "r²";
//        } else {
//            HeaderLabels << QString::fromStdString(field);
//        }
//    }
//    for (int col=0; col<numCols; ++col) {
//        itemStr = rowList[col].trimmed();
//        itemStr.replace("||","\n");
//        itemList.push_back(createNewModelReviewItem(col,itemStr));
//    }
//    if (m_SModel != nullptr) {
//        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());
//    }
//    m_SModel->appendRow(itemList);
//    for (int i=0; i<numCols; ++i) {
//        hideField = (! showHiddenFields()) && (i>nmfConstantsMSSPM::Model_Review_Column_Last_Visible);
//        Estimation_Tab7_ModelReviewTV->setColumnHidden(i,hideField);
//    }
//    m_SModel->setHorizontalHeaderLabels(HeaderLabels);
//    Estimation_Tab7_ModelReviewTV->horizontalHeader()->show();
//    resizeColumns();
//    saveModelReviewTable();
//}


void
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
        return;
    }

    // Clear current ModelReview table
    m_DatabasePtr->clearTable(m_Logger,nmfConstantsMSSPM::TableModelReview,m_ProjectName);

    m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());
    NumRows = m_SModel->rowCount();
    NumCols = m_SModel->columnCount();
    if (NumRows == 0) {
        return;
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
            "ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,Notes," +
            "isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites," +
            "numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt," +
            "isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter," +
            "StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked," +
            "isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstCarryingCapacityEnabled," +
            "isEstCarryingCapacityChecked,isEstCatchabilityEnabled,isEstCatchabilityChecked," +
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
                cmd += ","  + std::to_string(valueStr.toDouble());
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
        return;
    }

//    // Turn on significant digits if they were turned off previously in this method
//    if (flippedSignificantDigits) {
//        m_NumSignificantDigits = remNumSignificantDigits;
//        loadWidgets();
//    }
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
ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,Notes,\
isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites,\
numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt,\
isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter,\
StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked,\
isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstCarryingCapacityEnabled,\
isEstCarryingCapacityChecked,isEstCatchabilityEnabled,isEstCatchabilityChecked,\
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
        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());
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
    Estimation_Tab7_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab7_ModelReviewTV->horizontalHeader()->show();
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
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
ObjectiveCriterion,EstimationAlgorithm,Minimizer,Scaling,Notes,\
isDeterministicBees,maxGenerations,numBees,numBestSites,numEliteSites,\
numEliteBees,numOtherBees,neighborhoodSize,numSubRuns,isDeterministicNLopt,\
isStopAfterValue,StopAfterValue,isStopAfterTime,StopAfterTime,isStopAfterIter,\
StopAfterIter,isEstInitBiomassEnabled,isEstInitBiomassChecked,\
isEstGrowthRateEnabled,isEstGrowthRateChecked,isEstCarryingCapacityEnabled,\
isEstCarryingCapacityChecked,isEstCatchabilityEnabled,isEstCatchabilityChecked,\
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
        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());
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
    Estimation_Tab7_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
}

void
nmfEstimation_Tab8::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab7_ModelReviewTV});
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

void
nmfEstimation_Tab8::adjustColumnsForReadOnly()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());

    // Adjust all but the Notes column to be read only
    for (int row=0; row<smodel->rowCount(); ++row) {
        for (int col=0; col<smodel->columnCount(); ++col) {
            smodel->setItem(row,col,createNewModelReviewItem(col,smodel->index(row,col).data().toString()));
        }
    }
    Estimation_Tab7_ModelReviewTV->setModel(smodel);
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab7_ModelReviewTV->resizeRowsToContents();
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
nmfEstimation_Tab8::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
//    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->endGroup();

    delete settings;
}
