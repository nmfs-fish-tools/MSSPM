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
    m_IsBiomassAbsolute = isBiomassAbsolute;

    InfoLBL    = new QLabel();
    SavePB     = new QPushButton("Save");
    CancelPB   = new QPushButton("Cancel");
    ErrorLBL1  = new QLabel("Output file:");
    ErrorLBL2  = new QLabel("Enter ±% Observed Biomass error:");
    ErrorLE    = new QLineEdit();

    MainLT     = new QVBoxLayout();
    BtnLT      = new QHBoxLayout();
    ErrorLT1   = new QHBoxLayout();
    ErrorGB    = new QGroupBox("Data Noise:");
    NoneRB     = new QRadioButton("No noise");
    UniformSB  = new QDoubleSpinBox();
    UniformRB  = new QRadioButton("Uniform:");
    NormalRB   = new QRadioButton("Normal:");
    NormalSB   = new QDoubleSpinBox();
    UniformLB  = new QLabel("% error");
    NormalLB   = new QLabel("% CV");
    QHBoxLayout* NoneLT    = new QHBoxLayout();
    QHBoxLayout* NormalLT  = new QHBoxLayout();
    QHBoxLayout* UniformLT = new QHBoxLayout();
    QVBoxLayout* ErrorLT   = new QVBoxLayout();
    NormalSB->setDecimals(1);
    UniformSB->setDecimals(1);
    NormalSB->setSingleStep(0.1);
    UniformSB->setSingleStep(0.1);
    UniformSB->setMinimum(0.0);
    UniformSB->setMaximum(99.9);
    UniformSB->setValue(1.0);
    NormalSB->setMinimum(0.1);
    NormalSB->setMaximum(99.9);
    NormalSB->setValue(1.0);
    NormalSB->setEnabled(false);
    NormalLB->setEnabled(false);
    UniformSB->setEnabled(false);
    UniformLB->setEnabled(false);
    NoneRB->setChecked(true);
    UniformRB->setMinimumWidth(80);
    UniformRB->setMinimumWidth(80);
    NormalRB->setMinimumWidth(80);
    NormalRB->setMinimumWidth(80);

    // Create error group box
    NoneLT->addWidget(NoneRB);
    NormalLT->addWidget(NormalRB);
    NormalLT->addWidget(NormalSB);
    NormalLT->addWidget(NormalLB);
    UniformLT->addWidget(UniformRB);
    UniformLT->addWidget(UniformSB);
    UniformLT->addWidget(UniformLB);
    ErrorLT->addLayout(NoneLT);
    ErrorLT->addLayout(NormalLT);
    ErrorLT->addLayout(UniformLT);
    ErrorGB->setLayout(ErrorLT);

    msg  = "\nThis will create a Simulated Observed Biomass .csv data file with the ";
    msg += "below\nname. The file will be written to the inputData directory of the ";
    msg += "current project.\n\nN.B. The simulated data will be created without using any range data.\n";
    msg += "For example, if you currently have ranges around Predation initial values, the initial values\n";
    msg += "(and not the range data) will be used to generate the simulated data.";
    InfoLBL->setText(msg);

    SavePB->setToolTip("Continue with Simulated Biomass calculations and Save");
    SavePB->setStatusTip("Continue with Simulated Biomass calculations and Save");
    NoneRB->setToolTip("No error will be applied to the data");
    NoneRB->setStatusTip("No error will be applied to the data");
    UniformLB->setToolTip("A randomized ±% error applied to the Simulated Biomass");
    UniformLB->setStatusTip("A randomized ±% error applied to the Simulated Biomass");
    UniformRB->setToolTip("A randomized ±% error applied to the Simulated Biomass");
    UniformRB->setStatusTip("A randomized ±% error applied to the Simulated Biomass");
    UniformSB->setToolTip("A randomized ±% error applied to the Simulated Biomass");
    UniformSB->setStatusTip("A randomized ±% error applied to the Simulated Biomass");
    NormalLB->setToolTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    NormalLB->setStatusTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    NormalRB->setToolTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    NormalRB->setStatusTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    NormalSB->setToolTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    NormalSB->setStatusTip("A Coefficient of Variation (CV) percentage used for the normal (i.e., Gaussian) error on the Simulated Biomass");
    ErrorLE->setToolTip("The name of the Simulated Biomass file");
    ErrorLE->setStatusTip("The name of the Simulated Biomass file");

    ErrorLT1->addWidget(ErrorLBL1);
    ErrorLT1->addWidget(ErrorLE);
    ErrorLT1->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    ErrorLT1->addWidget(ErrorGB);
//  ErrorLT1->addWidget(UniformSB);
//  ErrorLT1->addWidget(UniformLB);
    BtnLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    BtnLT->addWidget(CancelPB);
    BtnLT->addWidget(SavePB);
    BtnLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addWidget(InfoLBL);
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addLayout(ErrorLT1);
    MainLT->addSpacerItem(new QSpacerItem(1,10,QSizePolicy::Fixed,QSizePolicy::Fixed));
    MainLT->addLayout(BtnLT);
    this->setLayout(MainLT);

    connect(SavePB,     SIGNAL(clicked()), this, SLOT(callback_ContinuePB()));
    connect(CancelPB,   SIGNAL(clicked()), this, SLOT(close()));
    connect(NoneRB,     SIGNAL(clicked()), this, SLOT(callback_NoneRB()));
    connect(NormalRB,   SIGNAL(clicked()), this, SLOT(callback_NormalRB()));
    connect(UniformRB,  SIGNAL(clicked()), this, SLOT(callback_UniformRB()));
    connect(NormalSB,   SIGNAL(valueChanged(double)), this, SLOT(callback_NormalSB(double)));
    connect(UniformSB,  SIGNAL(valueChanged(double)), this, SLOT(callback_UniformSB(double)));


    setWindowTitle("Create Simulated Biomass");
    setFileName(0);
    ErrorLE->setMinimumWidth(275);
}

void
SimulatedBiomassDialog::setFileName(double value)
{
    QString suffix = "";
    QString defaultName = "";

    if (UniformRB->isChecked()) {
        suffix = "_uniform_" + QString::number(value,'f',1) + "_pct";
    } else if (NormalRB->isChecked()) {
        suffix = "_normal_"  + QString::number(value,'f',1)  + "_cv";
    }

    defaultName = ( m_IsBiomassAbsolute ?
                   "biomassabsolute_sim" + suffix + ".csv" :
                   "biomassrelative_sim" + suffix + ".csv");

    ErrorLE->setText(defaultName);
}

QString
SimulatedBiomassDialog::getFilename()
{
    return ErrorLE->text();
}

void
SimulatedBiomassDialog::getError(QString& errorType, double& errorValue)
{
    errorType = "None";
    errorValue = 0;

    if (NormalRB->isChecked()) {
        errorType = "Normal";
        errorValue = NormalSB->value();
    } else if (UniformRB->isChecked()) {
        errorType  = "Uniform";
        errorValue = (double)UniformSB->value();
    }
}


void
SimulatedBiomassDialog::callback_ContinuePB()
{
    QString msg;
    QString errorMsg;
    nmfSimulatedData SimulatedData(m_Database,m_Logger,m_ProjectName,m_ModelName,m_DataStruct);
    QString filename = getFilename().toLower();
    QString errorType = "None";
    double errorValue = 0;

    if (filename.isEmpty()) {
        QMessageBox::critical(this,"Error","\nPlease enter a valid filename.\n");
        return;
    }

    getError(errorType,errorValue);

    QString inputDataPath    = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::InputDataDir));
    QString filenameWithPath = QDir(inputDataPath).filePath(filename);
    m_Logger->logMsg(nmfConstants::Normal,"Writing to: "+filenameWithPath.toStdString());
    bool ok = SimulatedData.createSimulatedBiomass(errorType,errorValue,filenameWithPath,errorMsg);
    close();

    if (ok) {
        msg = "\nSuccessfully created simulated biomass time series.\n\nData written to:\n\n";
        msg += filenameWithPath + "\n";
        QMessageBox::information(this, tr("Success"), tr(msg.toLatin1()),
                                 QMessageBox::Ok);
    } else {
        msg = "\nError creating simulated biomass:\n\n";
        msg += errorMsg + "\n";
        QMessageBox::critical(this, tr("Error"), tr(msg.toLatin1()),
                              QMessageBox::Ok);
    }
}

void
SimulatedBiomassDialog::callback_NoneRB()
{
    NormalSB->setEnabled(false);
    NormalLB->setEnabled(false);
    UniformSB->setEnabled(false);
    UniformLB->setEnabled(false);
    setFileName(0);
}

void
SimulatedBiomassDialog::callback_NormalRB()
{
    NormalSB->setEnabled(true);
    NormalLB->setEnabled(true);
    UniformSB->setEnabled(false);
    UniformLB->setEnabled(false);
    setFileName(NormalSB->value());
}

void
SimulatedBiomassDialog::callback_NormalSB(double value)
{
    setFileName(value);
}

void
SimulatedBiomassDialog::callback_UniformRB()
{
    NormalSB->setEnabled(false);
    NormalLB->setEnabled(false);
    UniformSB->setEnabled(true);
    UniformLB->setEnabled(true);
    setFileName(UniformSB->value());
}

void
SimulatedBiomassDialog::callback_UniformSB(double value)
{
    setFileName(value);
}
