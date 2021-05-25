#include "nmfEstimationTab07.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab7::nmfEstimation_Tab7(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;
    QStringList horizontalHeaders =
        {"Model Name","r²","SSResiduals","AIC",
         "Growth Form","Harvest Form","Competition Form","Predation Form",
         "Num Runs","Objective Criterion","Estimation Algorithm","Minimizer","Scaling",
         "Notes","isDeterministic","isStopAfterValue","StopAfterValue",
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
    m_ProjectSettingsConfig.clear();
    m_SModel = new QStandardItemModel(0,m_NumColumns);
    m_SModel->setHorizontalHeaderLabels(horizontalHeaders);

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab7::nmfEstimation_Tab7");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab07.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab7_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab7_Widget, tr("7. Model Review"));

    Estimation_Tab7_ModelReviewTV     = Estimation_Tabs->findChild<QTableView  *>("Estimation_Tab7_ModelReviewTV");
    Estimation_Tab7_LoadPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_LoadPB");
    Estimation_Tab7_ImportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_ImportPB");
    Estimation_Tab7_ExportPB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_ExportPB");
    Estimation_Tab7_GenerateSummaryPB = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_GenerateSummaryPB");
    Estimation_Tab7_PrevPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_PrevPB");
    Estimation_Tab7_DeletePB          = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab7_DeletePB");
    Estimation_Tab7_ShowHiddenCB      = Estimation_Tabs->findChild<QCheckBox *>("Estimation_Tab7_ShowHiddenCB");

    Estimation_Tab7_ModelReviewTV->setModel(m_SModel);
    Estimation_Tab7_ModelReviewTV->setSortingEnabled(true);
    Estimation_Tab7_ModelReviewTV->horizontalHeader()->hide();

    resizeColumns();

    connect(Estimation_Tab7_PrevPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_PrevPB()));
    connect(Estimation_Tab7_DeletePB, SIGNAL(clicked()),
            this,                     SLOT(callback_DeletePB()));
    connect(Estimation_Tab7_LoadPB,   SIGNAL(clicked()),
            this,                     SLOT(callback_LoadPB()));
    connect(Estimation_Tab7_ImportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ImportPB()));
    connect(Estimation_Tab7_ExportPB, SIGNAL(clicked()),
            this,                     SLOT(callback_ExportPB()));
    connect(Estimation_Tab7_GenerateSummaryPB, SIGNAL(clicked()),
            this,                              SLOT(callback_GenerateSummaryPB()));
    connect(Estimation_Tab7_ShowHiddenCB,      SIGNAL(stateChanged(int)),
            this,                              SLOT(callback_ShowHiddenCB(int)));

    Estimation_Tab7_PrevPB->setText("\u25C1--");

}


nmfEstimation_Tab7::~nmfEstimation_Tab7()
{
}

bool
nmfEstimation_Tab7::showHiddenFields()
{
    return Estimation_Tab7_ShowHiddenCB->isChecked();
}

void
nmfEstimation_Tab7::resizeColumns()
{
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab7_ModelReviewTV->resizeRowsToContents();
}

void
nmfEstimation_Tab7::callback_ShowHiddenCB(int state)
{
    bool hideField;
    for (int i=0; i<Estimation_Tab7_ModelReviewTV->model()->columnCount(); ++i) {
        hideField = (state==Qt::Unchecked && i>nmfConstantsMSSPM::Last_Review_Column_Visible);
        Estimation_Tab7_ModelReviewTV->setColumnHidden(i,hideField);
    }
    resizeColumns();
}

void
nmfEstimation_Tab7::callback_LoadPB()
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
nmfEstimation_Tab7::callback_ImportPB()
{
    int numRows = 0;
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::ModelReviewFilename));
    QString fileName = QFileDialog::getOpenFileName(Estimation_Tabs, tr("Import Model Review File"),
                               filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        bool loadOK = nmfUtilsQt::loadModelFromCSVFile(m_ProjectDir,"Model Review",Estimation_Tab7_ModelReviewTV,fileName,numRows);
        if (loadOK && numRows>2) {
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
nmfEstimation_Tab7::callback_ExportPB()
{
    QString dataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString filePath = QDir(dataPath).filePath(QString::fromStdString(nmfConstantsMSSPM::ModelReviewFilename));
    QString fileName = QFileDialog::getSaveFileName(Estimation_Tabs, tr("Export Model Review File"),
                               filePath,tr("data (*.csv)"));

    if (! fileName.isEmpty()) {
        nmfUtilsQt::saveModelToCSVFile(m_ProjectDir,"Model Review","",Estimation_Tab7_ModelReviewTV,
                                       nmfConstantsMSSPM::Dont_Query_User_For_Filename,fileName);
    }
}

void
nmfEstimation_Tab7::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab7::callback_DeletePB()
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
}

QString
nmfEstimation_Tab7::getModelEquation(QStandardItemModel* smodel, const int& row)
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
nmfEstimation_Tab7::getEstimatedParameters(QStandardItemModel* smodel, const int& row)
{
    QString retv="";
    QString filename = "";
    QString fullPath;
    QString line;

    for (int i = 0; i < smodel->columnCount(); i++)
    {
        if (smodel->headerData(i, Qt::Horizontal).toString() == "EstimatedParametersFile") {
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
                m_Logger->logMsg(nmfConstants::Error,"\nIncorrect file type read in nmfEstimation_Tab7::getEstimatedParameters");
                return "";
            }
        }
    }

    return retv;
}

void
nmfEstimation_Tab7::loadModel(QStandardItemModel* smodel, const int& row)
{
    nmfStructsQt::ModelReviewStruct modelReview;

    modelReview.ModelName          = smodel->index(row, 0).data().toString().trimmed(); // Reload this model
    modelReview.setToDeterministic = smodel->index(row,14).data().toString().trimmed();
    modelReview.isStopAfterValue   = smodel->index(row,15).data().toString().trimmed();
    modelReview.stopAfterValue     = smodel->index(row,16).data().toString().trimmed();
    modelReview.isStopAfterTime    = smodel->index(row,17).data().toString().trimmed();
    modelReview.stopAfterTime      = smodel->index(row,18).data().toString().trimmed();
    modelReview.isStopAfterIter    = smodel->index(row,19).data().toString().trimmed();
    modelReview.stopAfterIter      = smodel->index(row,20).data().toString().trimmed();

    modelReview.isEstInitialBiomassEnabled              = smodel->index(row,21).data().toString().trimmed();
    modelReview.isEstInitialBiomassChecked              = smodel->index(row,22).data().toString().trimmed();
    modelReview.isEstGrowthRateEnabled                  = smodel->index(row,23).data().toString().trimmed();
    modelReview.isEstGrowthRateChecked                  = smodel->index(row,24).data().toString().trimmed();
    modelReview.isEstCarryingCapacityEnabled            = smodel->index(row,25).data().toString().trimmed();
    modelReview.isEstCarryingCapacityChecked            = smodel->index(row,26).data().toString().trimmed();
    modelReview.isEstCatchabilityEnabled                = smodel->index(row,27).data().toString().trimmed();
    modelReview.isEstCatchabilityChecked                = smodel->index(row,28).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaEnabled            = smodel->index(row,29).data().toString().trimmed();
    modelReview.isEstCompetitionAlphaChecked            = smodel->index(row,30).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesEnabled      = smodel->index(row,31).data().toString().trimmed();
    modelReview.isEstCompetitionBetaSpeciesChecked      = smodel->index(row,32).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsEnabled       = smodel->index(row,33).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsChecked       = smodel->index(row,34).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsEnabled = smodel->index(row,35).data().toString().trimmed();
    modelReview.isEstCompetitionBetaGuildsGuildsChecked = smodel->index(row,36).data().toString().trimmed();
    modelReview.isEstPredationRhoEnabled                = smodel->index(row,37).data().toString().trimmed();
    modelReview.isEstPredationRhoChecked                = smodel->index(row,38).data().toString().trimmed();
    modelReview.isEstPredationHandlingEnabled           = smodel->index(row,39).data().toString().trimmed();
    modelReview.isEstPredationHandlingChecked           = smodel->index(row,40).data().toString().trimmed();
    modelReview.isEstPredationExponentEnabled           = smodel->index(row,41).data().toString().trimmed();
    modelReview.isEstPredationExponentChecked           = smodel->index(row,42).data().toString().trimmed();

    modelReview.isAMultiRun                             = smodel->index(row,43).data().toString().trimmed();
    modelReview.ensembleAveragingAlgorithm              = smodel->index(row,44).data().toString().trimmed();
    modelReview.ensembleAverageBy                       = smodel->index(row,45).data().toString().trimmed();
    modelReview.ensembleUsingBy                         = smodel->index(row,46).data().toString().trimmed();
    modelReview.ensembleUsingAmountValue                = smodel->index(row,47).data().toString().trimmed();
    modelReview.isEnsembleUsingPct                      = smodel->index(row,48).data().toString().trimmed();
    modelReview.ensembleFilename                        = smodel->index(row,49).data().toString().trimmed();

    emit LoadFromModelReview(modelReview);
}

void
nmfEstimation_Tab7::callback_GenerateSummaryPB()
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
nmfEstimation_Tab7::updateReviewList(const QStringList& rowList)
{
    bool hideField;
    QList<QStandardItem* > itemList;
    int numCols = rowList.size();
    QString itemStr;

    for (int col=0; col<numCols; ++col) {
        itemStr = rowList[col].trimmed();
        itemStr.replace("||","\n");
        itemList.push_back(createNewModelReviewItem(col,itemStr));
    }
    if (m_SModel != nullptr) {
        m_SModel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());
    }
    m_SModel->appendRow(itemList);
    for (int i=0; i<numCols; ++i) {
        hideField = (! showHiddenFields()) && (i>nmfConstantsMSSPM::Last_Review_Column_Visible);
        Estimation_Tab7_ModelReviewTV->setColumnHidden(i,hideField);
    }
    Estimation_Tab7_ModelReviewTV->horizontalHeader()->show();
    resizeColumns();
}

void
nmfEstimation_Tab7::clearWidgets()
{
    nmfUtilsQt::clearTableView({Estimation_Tab7_ModelReviewTV});
}

QStandardItem*
nmfEstimation_Tab7::createNewModelReviewItem(const int& col,
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
nmfEstimation_Tab7::adjustColumnsForReadOnly()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Estimation_Tab7_ModelReviewTV->model());

    // Adjust all but the Notes column to be read only
    for (int row=0; row<smodel->rowCount(); ++row) {
        for (int col=0; col<smodel->columnCount(); ++col) {
//            if (col != COLUMN_NOTES) {
                smodel->setItem(row,col,createNewModelReviewItem(col,smodel->index(row,col).data().toString()));
//            }
        }
    }
    Estimation_Tab7_ModelReviewTV->setModel(smodel);
    Estimation_Tab7_ModelReviewTV->resizeColumnsToContents();
    Estimation_Tab7_ModelReviewTV->resizeRowsToContents();
}


void
nmfEstimation_Tab7::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Estimation");
//    m_EstimationOutputFile = settings->value("OutputFile","").toString().toStdString();
    settings->endGroup();

    delete settings;

}

void
nmfEstimation_Tab7::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
//    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->endGroup();

    delete settings;
}
