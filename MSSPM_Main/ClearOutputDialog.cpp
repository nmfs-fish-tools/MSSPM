#include "ClearOutputDialog.h"
#include <QStandardItemModel>
#include <QTableWidget>

ClearOutputDialog::ClearOutputDialog(QWidget*     parent,
                                     nmfDatabase* databasePtr) :
    QDialog(parent)
{
    m_databasePtr         = databasePtr;

    MainLAYT              = new QVBoxLayout();
    BtnLAYT               = new QHBoxLayout();
    MainLBL               = new QLabel("\nSelect Algorithm(s) to clear from Output, Diagnostic,\nand Forecast tables:");
    AlgorithmLBL          = new QLabel("\nAlgorithm:");
    MinimizerLBL          = new QLabel("Minimizer:");
    ObjectiveCriterionLBL = new QLabel("Objective Criterion:");
    ScalingLBL            = new QLabel("Scaling Algorithm:");
    AlgorithmCMB          = new QComboBox();
    MinimizerCMB          = new QComboBox();
    ObjectiveCriterionCMB = new QComboBox();
    ScalingCMB            = new QComboBox();
    CancelPB              = new QPushButton("Cancel");
    OkPB                  = new QPushButton("OK");

    OkPB->setToolTip("Clear Output from above Algorithms");
    OkPB->setStatusTip("Clear Output from above Algorithms");

    MainLAYT->addWidget(MainLBL);
    MainLAYT->addWidget(AlgorithmLBL);
    MainLAYT->addWidget(AlgorithmCMB);
    MainLAYT->addWidget(MinimizerLBL);
    MainLAYT->addWidget(MinimizerCMB);
    MainLAYT->addWidget(ObjectiveCriterionLBL);
    MainLAYT->addWidget(ObjectiveCriterionCMB);
    MainLAYT->addWidget(ScalingLBL);
    MainLAYT->addWidget(ScalingCMB);
    MainLAYT->addSpacerItem(new QSpacerItem(10,20,QSizePolicy::Fixed,QSizePolicy::Expanding));

    BtnLAYT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    BtnLAYT->addWidget(CancelPB);
    BtnLAYT->addWidget(OkPB);
    BtnLAYT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));

    MainLAYT->addLayout(BtnLAYT);
    this->setLayout(MainLAYT);

    connect(AlgorithmCMB, SIGNAL(currentIndexChanged(QString)),
            this,         SLOT(callback_AlgorithmCMB(QString)));
    connect(MinimizerCMB, SIGNAL(currentIndexChanged(QString)),
            this,         SLOT(callback_MinimizerCMB(QString)));
    connect(ObjectiveCriterionCMB, SIGNAL(currentIndexChanged(QString)),
            this,                  SLOT(callback_ObjectiveCriterionCMB(QString)));
    connect(CancelPB,     SIGNAL(clicked()), this, SLOT(close()));
    connect(OkPB,         SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle("Clear Selected Output");
    loadWidgets();
}

void
ClearOutputDialog::loadWidgets()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString algorithm;

    AlgorithmCMB->clear();
    MinimizerCMB->clear();
    ObjectiveCriterionCMB->clear();
    ScalingCMB->clear();

    AlgorithmCMB->blockSignals(true);
    MinimizerCMB->blockSignals(true);
    ObjectiveCriterionCMB->blockSignals(true);
    ScalingCMB->blockSignals(true);

    AlgorithmCMB->addItem("All");
    fields   = {"Algorithm"};
    queryStr = "SELECT Algorithm from OutputGrowthRate ";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["Algorithm"].size(); ++i) {
        algorithm = QString::fromStdString(dataMap["Algorithm"][i]);
        if (AlgorithmCMB->findText(algorithm) == -1) {
            AlgorithmCMB->addItem(algorithm);
        }
    }
    MinimizerCMB->addItem("All");
    ObjectiveCriterionCMB->addItem("All");
    ScalingCMB->addItem("All");

    AlgorithmCMB->blockSignals(false);
    MinimizerCMB->blockSignals(false);
    ObjectiveCriterionCMB->blockSignals(false);
    ScalingCMB->blockSignals(false);
}

void
ClearOutputDialog::callback_AlgorithmCMB(QString algorithm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString minimizer;

    MinimizerCMB->clear();
    ObjectiveCriterionCMB->clear();
    ScalingCMB->clear();

    // Load Minimizer combo box with the selected algorithm
    MinimizerCMB->blockSignals(true);
    ObjectiveCriterionCMB->blockSignals(true);
    ScalingCMB->blockSignals(true);

    MinimizerCMB->addItem("All");
    ObjectiveCriterionCMB->addItem("All");
    ScalingCMB->addItem("All");

    fields    = {"Algorithm","Minimizer"};
    queryStr  = "SELECT Algorithm,Minimizer from OutputGrowthRate ";
    queryStr += "WHERE Algorithm='" + algorithm.toStdString() + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["Minimizer"].size(); ++i) {
        minimizer = QString::fromStdString(dataMap["Minimizer"][i]);
        if (! minimizer.isEmpty() && (MinimizerCMB->findText(minimizer) == -1)) {
            MinimizerCMB->addItem(minimizer);
        }
    }
    ScalingCMB->blockSignals(false);
    ObjectiveCriterionCMB->blockSignals(false);
    MinimizerCMB->blockSignals(false);

    callback_MinimizerCMB("All");
}

void
ClearOutputDialog::callback_MinimizerCMB(QString minimizer)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString objectiveCriterion;
    QString algorithm = AlgorithmCMB->currentText();

    ObjectiveCriterionCMB->clear();
    ScalingCMB->clear();

    // Load Objective Criterion combo box with the selected algorithm and minimizer
    ObjectiveCriterionCMB->blockSignals(true);
    ScalingCMB->blockSignals(true);
    ObjectiveCriterionCMB->addItem("All");
    ScalingCMB->addItem("All");
    fields    = {"Algorithm","Minimizer","ObjectiveCriterion"};
    queryStr  = "SELECT Algorithm,Minimizer,ObjectiveCriterion from OutputGrowthRate ";
    queryStr += "WHERE Algorithm='" + algorithm.toStdString() + "'";
    if (minimizer != "All") {
        queryStr += " AND  Minimizer='" + minimizer.toStdString() + "'";
    }
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["ObjectiveCriterion"].size(); ++i) {
        objectiveCriterion = QString::fromStdString(dataMap["ObjectiveCriterion"][i]);
        if (! objectiveCriterion.isEmpty() && (ObjectiveCriterionCMB->findText(objectiveCriterion) == -1)) {
            ObjectiveCriterionCMB->addItem(objectiveCriterion);
        }
    }
    ScalingCMB->blockSignals(false);
    ObjectiveCriterionCMB->blockSignals(false);

    callback_ObjectiveCriterionCMB("All");
}


void
ClearOutputDialog::callback_ObjectiveCriterionCMB(QString objectiveCriterion)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString scaling;
    QString algorithm = AlgorithmCMB->currentText();
    QString minimizer = MinimizerCMB->currentText();

    ScalingCMB->clear();

    // Load Objective Criterion combo box with the selected algorithm and minimizer
    ScalingCMB->blockSignals(true);
    ScalingCMB->addItem("All");
    fields    = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr  = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling from OutputGrowthRate ";
    queryStr += "WHERE Algorithm='" + algorithm.toStdString();
    if (minimizer != "All") {
        queryStr += "' AND Minimizer='" + minimizer.toStdString();
    }
    if (objectiveCriterion != "All") {
        queryStr += "' AND ObjectiveCriterion='" + objectiveCriterion.toStdString();
    }
    queryStr += "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["Scaling"].size(); ++i) {
        scaling = QString::fromStdString(dataMap["Scaling"][i]);
        if (! scaling.isEmpty() && (ScalingCMB->findText(scaling) == -1)) {
            ScalingCMB->addItem(scaling);
        }
    }
    ScalingCMB->blockSignals(false);
}

std::string
ClearOutputDialog::getAlgorithm()
{
    return AlgorithmCMB->currentText().toStdString();
}

std::string
ClearOutputDialog::getMinimizer()
{
    return MinimizerCMB->currentText().toStdString();
}

std::string
ClearOutputDialog::getObjectiveCriterion()
{
    return ObjectiveCriterionCMB->currentText().toStdString();
}

std::string
ClearOutputDialog::getScaling()
{
    return ScalingCMB->currentText().toStdString();
}
