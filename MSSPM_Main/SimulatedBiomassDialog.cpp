#include "SimulatedBiomassDialog.h"

#include <QStandardItemModel>
#include <QTableWidget>

SimulatedBiomassDialog::SimulatedBiomassDialog(QWidget *parent,
                                               std::string projectDir,
                                               std::string projectName,
                                               std::string modelName,
                                               nmfDatabase* database,
                                               nmfLogger* logger,
                                               nmfStructsQt::ModelDataStruct& dataStruct) :
    QDialog(parent)
{
    bool isBiomassAbsolute;
    int RunLength=0;
    int InitialYear=0;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    QString msg;

    m_ProjectDir  = projectDir;
    m_ProjectName = projectName;
    m_ModelName   = modelName;
    m_Database    = database;
    m_Logger      = logger;
    m_DataStruct  = dataStruct;

    // Get some model data
    if (! m_Database->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        m_Logger->logMsg(nmfConstants::Error,"SimulatedBiomassDialog::SimulatedBiomassDialog: Error calling getModelFormData");
        return;
    }
    QString defaultName = (isBiomassAbsolute) ?
                "biomassabsolute_sim.csv" :
                "biomassrelative_sim.csv";

    InfoLBL    = new QLabel();
    ContinuePB = new QPushButton("Continue");
    CancelPB   = new QPushButton("Cancel");
    ErrorLBL1  = new QLabel("Output file:");
    ErrorLBL2  = new QLabel("Enter ±% Observed Biomass error:");
    ErrorLBL3  = new QLabel("% error");
    ErrorLE    = new QLineEdit();
    ErrorSB    = new QSpinBox();
    MainLT     = new QVBoxLayout();
    BtnLT      = new QHBoxLayout();
    ErrorLT1   = new QHBoxLayout();

    msg  = "This will create a Simulated Observed Biomass .csv data file with the\n";
    msg += "below name. The file will be written to the inputData directory of the\n";
    msg += "current project.";
    InfoLBL->setText(msg);

    ContinuePB->setToolTip("Continue with Simulated Biomass calculations");
    ContinuePB->setStatusTip("Continue with Simulated Biomass calculations");
    ErrorSB->setToolTip("A randomized ±% error applied to the Simulated Biomass");
    ErrorSB->setStatusTip("A randomized ±% error applied to the Simulated Biomass");
    ErrorLE->setToolTip("The name of the Simulated Biomass file");
    ErrorLE->setStatusTip("The name of the Simulated Biomass file");

    ErrorLT1->addWidget(ErrorLBL1);
    ErrorLT1->addWidget(ErrorLE);
    ErrorLT1->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    ErrorLT1->addWidget(ErrorSB);
    ErrorLT1->addWidget(ErrorLBL3);
    BtnLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    BtnLT->addWidget(CancelPB);
    BtnLT->addWidget(ContinuePB);
    BtnLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addWidget(InfoLBL);
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addLayout(ErrorLT1);
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addLayout(BtnLT);
    this->setLayout(MainLT);

    connect(ContinuePB, SIGNAL(clicked()), this, SLOT(callback_ContinuePB()));
    connect(CancelPB,   SIGNAL(clicked()), this, SLOT(close()));

    setWindowTitle("Created Simulated Biomass");
    ErrorLE->setText(defaultName);
    ErrorLE->setMinimumWidth(275);
}

QString
SimulatedBiomassDialog::getFilename()
{
    return ErrorLE->text();
}

int
SimulatedBiomassDialog::getErrorPct()
{
    return ErrorSB->value();
}


void
SimulatedBiomassDialog::callback_ContinuePB()
{
    QString msg;
    nmfSimulatedData SimulatedData(m_Database,m_Logger,m_ProjectName,m_ModelName,m_DataStruct);
    QString filenameWithPath;
    QString filename = getFilename().toLower();
    int errorPct = getErrorPct();

    if (filename.isEmpty()) {
        QMessageBox::critical(this,"Error","\nPlease enter a valid filename.\n");
        return;
    }

    QString inputDataPath = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    filenameWithPath = QDir(inputDataPath).filePath(filename);
    m_Logger->logMsg(nmfConstants::Normal,"Writing to: "+filenameWithPath.toStdString());
    bool ok = SimulatedData.createSimulatedBiomass(errorPct,filenameWithPath);
    close();

    if (ok) {
        msg = "\nSuccessfully created simulated biomass time series. Data written to:\n\n";
        msg += filenameWithPath + "\n";
        QMessageBox::information(this, tr("Create Simulated Biomass"), tr(msg.toLatin1()),
                                 QMessageBox::Ok);
    }

}
