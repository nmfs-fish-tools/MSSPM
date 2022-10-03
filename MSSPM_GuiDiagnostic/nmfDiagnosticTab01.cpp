#include "nmfDiagnosticTab01.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

nmfDiagnostic_Tab1::nmfDiagnostic_Tab1(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    // Define class variables
    m_Diagnostic_Tabs = tabs;
    m_Logger          = logger;
    m_DatabasePtr     = databasePtr;
    m_ProjectDir      = projectDir;
    m_NumPoints       = 1;
    m_PctVariation    = 1;
    m_IsMultiRun      = false;
    m_MultiRunType.clear();
    m_ProjectName.clear();
    m_ModelName.clear();

    // RSK - create constants for these strings...
    m_OutputTableName["Growth Rate (r)"]                                 = nmfConstantsMSSPM::TableOutputGrowthRate;
    m_OutputTableName["Carrying Capacity (K)"]                           = nmfConstantsMSSPM::TableOutputCarryingCapacity;
    m_OutputTableName["Initial Biomass (B₀)"]                            = nmfConstantsMSSPM::TableOutputInitBiomass;
    m_OutputTableName["Catchability (q)"]                                = nmfConstantsMSSPM::TableOutputCatchability;
    m_OutputTableName["SurveyQ"]                                         = nmfConstantsMSSPM::TableOutputSurveyQ;
    m_OutputTableName["Growth Rate (r) Covariate Coefficient"]           = nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs;
    m_OutputTableName["Carrying Capacity (K) Covariate Coefficient"]     = nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs;
    m_OutputTableName["Catchability (q) Covariate Coefficient"]          = nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs;
    m_OutputTableName["SurveyQ Covariate Coefficient"]                   = nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs;

    m_DiagnosticTableName["Growth Rate (r)"]                             = nmfConstantsMSSPM::TableDiagnosticGrowthRate;
    m_DiagnosticTableName["Carrying Capacity (K)"]                       = nmfConstantsMSSPM::TableDiagnosticCarryingCapacity;
    m_DiagnosticTableName["Initial Biomass (B₀)"]                        = nmfConstantsMSSPM::TableDiagnosticInitBiomass;
    m_DiagnosticTableName["Catchability (q)"]                            = nmfConstantsMSSPM::TableDiagnosticCatchability;
    m_DiagnosticTableName["SurveyQ"]                                     = nmfConstantsMSSPM::TableDiagnosticSurveyQ;

    m_DiagnosticTableName["Growth Rate (r) Covariate Coefficient"]       = nmfConstantsMSSPM::TableDiagnosticGrowthRateCovCoeff;
    m_DiagnosticTableName["Carrying Capacity (K) Covariate Coefficient"] = nmfConstantsMSSPM::TableDiagnosticCarryingCapacityCovCoeff;
    m_DiagnosticTableName["Catchability (q) Covariate Coefficient"]      = nmfConstantsMSSPM::TableDiagnosticCatchabilityCovCoeff;
    m_DiagnosticTableName["SurveyQ Covariate Coefficient"]               = nmfConstantsMSSPM::TableDiagnosticSurveyQCovCoeff;

    // Load ui as a widget from disk
    QFile file(":/forms/Diagnostic/Diagnostic_Tab01.ui");
    file.open(QFile::ReadOnly);
    m_Diagnostic_Tab1_Widget = loader.load(&file,m_Diagnostic_Tabs);
    m_Diagnostic_Tab1_Widget->setObjectName("Diagnostic_Tab1_Widget");
    file.close();

    Diagnostic_Tab1_SurfaceParameter1CMB = m_Diagnostic_Tabs->findChild<QComboBox    *>("Diagnostic_Tab1_SurfaceParameter1CMB");
    Diagnostic_Tab1_SurfaceParameter2CMB = m_Diagnostic_Tabs->findChild<QComboBox    *>("Diagnostic_Tab1_SurfaceParameter2CMB");
    m_Diagnostic_Tab1_PctVarSB           = m_Diagnostic_Tabs->findChild<QSpinBox     *>("Diagnostic_Tab1_PctVarSB");
    m_Diagnostic_Tab1_NumPtsSB           = m_Diagnostic_Tabs->findChild<QSpinBox     *>("Diagnostic_Tab1_NumPtsSB");
    m_Diagnostic_Tab1_RunPB              = m_Diagnostic_Tabs->findChild<QPushButton  *>("Diagnostic_Tab1_RunPB");
    m_Diagnostic_Tab1_UseLastSingleRunRB = m_Diagnostic_Tabs->findChild<QRadioButton *>("Diagnostic_Tab1_UseLastSingleRunRB");
    m_Diagnostic_Tab1_UseLastMultiRunRB  = m_Diagnostic_Tabs->findChild<QRadioButton *>("Diagnostic_Tab1_UseLastMultiRunRB");

    // Add the loaded widget as the new tabbed page
    m_Diagnostic_Tabs->addTab(m_Diagnostic_Tab1_Widget, tr("1. Parameter Profiles"));

    // Setup connections
    connect(m_Diagnostic_Tab1_RunPB, SIGNAL(clicked()),
            this,                    SLOT(callback_RunPB()));
    connect(m_Diagnostic_Tab1_UseLastSingleRunRB, SIGNAL(clicked()),
            this,                                 SLOT(callback_UseLastSingleRunRB()));
    connect(m_Diagnostic_Tab1_UseLastMultiRunRB,  SIGNAL(clicked()),
            this,                                 SLOT(callback_UseLastMultiRunRB()));

    saveSettings();
    readSettings();

    Diagnostic_Tab1_SurfaceParameter1CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);
    Diagnostic_Tab1_SurfaceParameter2CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);

}

nmfDiagnostic_Tab1::~nmfDiagnostic_Tab1()
{

}

void
nmfDiagnostic_Tab1::enableRunButton(bool state)
{
    QString label = (state == true) ? "Run" : "Running...";
    m_Diagnostic_Tab1_RunPB->setEnabled(state);
    m_Diagnostic_Tab1_RunPB->setText(label);
}

void
nmfDiagnostic_Tab1::setSingleRunRBState(bool isEnabled,
                                        bool isChecked)
{
    m_Diagnostic_Tab1_UseLastSingleRunRB->setEnabled(isEnabled);
    m_Diagnostic_Tab1_UseLastSingleRunRB->setChecked(isChecked);
}

void
nmfDiagnostic_Tab1::setMultiRunRBState(bool isEnabled,
                                       bool isChecked)
{
    m_Diagnostic_Tab1_UseLastMultiRunRB->setEnabled(isEnabled);
    m_Diagnostic_Tab1_UseLastMultiRunRB->setChecked(isChecked);
}

void
nmfDiagnostic_Tab1::setSingleRunRBEnabled(bool isEnabled)
{
    m_Diagnostic_Tab1_UseLastSingleRunRB->setEnabled(isEnabled);
}

void
nmfDiagnostic_Tab1::setMultiRunRBEnabled(bool isEnabled)
{
    m_Diagnostic_Tab1_UseLastMultiRunRB->setEnabled(isEnabled);
}

void
nmfDiagnostic_Tab1::callback_UseLastSingleRunRB()
{
    m_IsMultiRun = false;
    saveSettings();
}

void
nmfDiagnostic_Tab1::callback_UseLastMultiRunRB()
{
    m_IsMultiRun = true;
    saveSettings();
}


void
nmfDiagnostic_Tab1::callback_UpdateDiagnosticParameterChoices()
{
    m_DatabasePtr->loadEstimatedVectorParameters(m_Logger,m_ProjectName,m_ModelName,
                                                 Diagnostic_Tab1_SurfaceParameter1CMB);
    m_DatabasePtr->loadEstimatedVectorParameters(m_Logger,m_ProjectName,m_ModelName,
                                                 Diagnostic_Tab1_SurfaceParameter2CMB);
}

void
nmfDiagnostic_Tab1::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Diagnostics");
    m_NumPoints    = settings->value("NumPoints","").toInt();
    m_PctVariation = settings->value("Variation","").toInt();
    settings->endGroup();

    settings->beginGroup("Runtime");
    m_IsMultiRun    = settings->value("IsMultiRun",false).toBool();
    m_MultiRunType  = settings->value("MultiRunType","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
nmfDiagnostic_Tab1::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Diagnostics");
    settings->setValue("Variation", m_Diagnostic_Tab1_PctVarSB->value());
    settings->setValue("NumPoints", m_Diagnostic_Tab1_NumPtsSB->value());
    settings->endGroup();

    settings->beginGroup("Runtime");
    settings->setValue("IsMultiRun", m_IsMultiRun);
    settings->endGroup();

    delete settings;
}

int
nmfDiagnostic_Tab1::getLastRunsNumPoints()
{
    readSettings();
    return m_NumPoints;
}


int
nmfDiagnostic_Tab1::getLastRunsPctVariation()
{
    readSettings();
    return m_PctVariation;
}


void
nmfDiagnostic_Tab1::setVariation(int variation)
{
    if (m_Diagnostic_Tab1_PctVarSB != NULL) {
        m_Diagnostic_Tab1_PctVarSB->setValue(variation);
    }
}

void
nmfDiagnostic_Tab1::setNumPoints(int numPoints)
{
    if (m_Diagnostic_Tab1_NumPtsSB != NULL) {
        m_Diagnostic_Tab1_NumPtsSB->setValue(numPoints);
    }
}


std::string
nmfDiagnostic_Tab1::getTableName(QString paramName)
{
    std::string tableName;

    parameterToTableName("output",paramName,tableName);

    return tableName;
}


void
nmfDiagnostic_Tab1::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfDiagnostic_Tab1::loadWidgets()");

    readSettings();

    callback_UpdateDiagnosticParameterChoices();

    //readSettings();
}


bool
nmfDiagnostic_Tab1::loadEstimatedParameter(const std::string& Algorithm,
                                           const std::string& Minimizer,
                                           const std::string& ObjectiveCriterion,
                                           const std::string& Scaling,
                                           const QString& parameterName,
                                           std::vector<double>& EstParameter)
{
    int NumSpecies;
    std::string tableName="";
    std::string queryStr;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;

    parameterToTableName("input",parameterName, tableName);
    if (tableName.empty()) {
        return false;
    }

    fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
    queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " +
                  tableName +
                 " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND Algorithm ='"           + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling +
                 "' ORDER BY SpeName";

    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    EstParameter.clear();
    for (int i=0; i<NumSpecies; ++i) {
        EstParameter.push_back(std::stod(dataMap["Value"][i]));
    }
    return (EstParameter.size() > 0);
}

void
nmfDiagnostic_Tab1::parameterToTableName(const std::string  whichTable,
                                         const QString&     parameter,
                                               std::string& tableName)
{
    tableName = "";
    if (whichTable == "input") {
        tableName = m_OutputTableName[parameter.toStdString()];
    } else if (whichTable == "output") {
        tableName = m_DiagnosticTableName[parameter.toStdString()];
    }
}

void
nmfDiagnostic_Tab1::getGuildInfo(int& NumGuilds, QStringList& GuildNames)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from " + nmfConstantsMSSPM::TableGuilds + " ORDER by GuildName";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    for (int i=0; i<NumGuilds; ++i) {
        GuildNames << QString::fromStdString(dataMap["GuildName"][i]);
    }
}


void
nmfDiagnostic_Tab1::getSpeciesInfo(int& NumSpecies, QStringList& SpeciesNames)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields = {"SpeName"};
    queryStr   = "SELECT SpeName FROM " + nmfConstantsMSSPM::TableSpecies;
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int j=0; j<NumSpecies; ++j) {
        SpeciesNames << QString::fromStdString(dataMap["SpeName"][j]);
    }
}

bool
nmfDiagnostic_Tab1::isAggProd(std::string Algorithm,
                              std::string Minimizer,
                              std::string ObjectiveCriterion,
                              std::string Scaling)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string CompetitionStr;

    fields     = {"ProjectName","ModelName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm",
                  "WithinGuildCompetitionForm","Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr   = "SELECT ProjectName,ModelName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,";
    queryStr  += "WithinGuildCompetitionForm,Algorithm,Minimizer,ObjectiveCriterion,Scaling ";
    queryStr  += "FROM " + nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '"       + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling + "' ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == 0) {
        std::cout << "Error nmfDiagnostic_Tab1::isAggProd: No records found in Models" << std::endl;
        std::cout << queryStr << std::endl;
        return false;
    }
    CompetitionStr = dataMap["WithinGuildCompetitionForm"][0];

    return (CompetitionStr == "AGG-PROD");
}

QString
nmfDiagnostic_Tab1::getParameter1Name()
{
    return Diagnostic_Tab1_SurfaceParameter1CMB->currentText();
}

QString
nmfDiagnostic_Tab1::getParameter2Name()
{
    return Diagnostic_Tab1_SurfaceParameter2CMB->currentText();
}


void
nmfDiagnostic_Tab1::checkAlgorithmIdentifiersForMultiRun(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling)
{
    readSettings();
    if (isSetLastRunMultiDiagnostics())
    {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    }
}

bool
nmfDiagnostic_Tab1::isSingleRunRBChecked()
{
    return m_Diagnostic_Tab1_UseLastSingleRunRB->isChecked();
}
bool
nmfDiagnostic_Tab1::isSingleRunRBEnabled()
{
    return m_Diagnostic_Tab1_UseLastSingleRunRB->isEnabled();
}
bool
nmfDiagnostic_Tab1::isMultiRunRBChecked()
{
    return m_Diagnostic_Tab1_UseLastMultiRunRB->isChecked();
}
bool
nmfDiagnostic_Tab1::isMultiRunRBEnabled()
{
    return m_Diagnostic_Tab1_UseLastMultiRunRB->isEnabled();
}

bool
nmfDiagnostic_Tab1::isSetLastRunMultiDiagnostics()
{
    return (m_Diagnostic_Tab1_UseLastMultiRunRB->isEnabled() &&
            m_Diagnostic_Tab1_UseLastMultiRunRB->isChecked());
}

void
nmfDiagnostic_Tab1::callback_RunPB()
{
    bool isBiomassAbsolute;
    int RunLength;
    int InitialYear;
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds=0;
    int pctVariation   = m_Diagnostic_Tab1_PctVarSB->value();
    int numPoints      = m_Diagnostic_Tab1_NumPtsSB->value();
    int totalNumPoints = 2*numPoints;
    int pInc = 0;
    double sum = 0;
    double tempVal = 0;
    int m=0;
    double startVal;
    double inc;
    double estParameter;
    double fitness,zscoreFitness;
    double sigma;
    double centerFitness = 0;
    double diagnosticParameterValue;
    double parameter1,               parameter2;
    double parameter1DiagnosticValue,parameter2DiagnosticValue;
    double parameter1StartVal,       parameter2StartVal;
    double parameter1PctVar,         parameter2PctVar;
    double parameter1PctInc,         parameter2PctInc;
    double parameter1Inc,            parameter2Inc;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    std::vector<double> EstParameter;
    std::vector<double> surfaceParameter1;
    std::vector<double> surfaceParameter2;
    QStringList SpeciesNames;
    QStringList GuildNames;
    QStringList SpeciesOrGuildNames;
    std::vector<DiagnosticTuple> DiagnosticTupleVector;
    std::vector<DiagnosticTuple> ZScoreDiagnosticTupleVector;
    DiagnosticTuple aDiagnosticTuple;
    std::string isAggProdStr;
    bool isAggProdBool;
    QString msg;
    std::pair<QString,double> parameterItem;
    std::pair<QString,double> parameterItem1;
    std::pair<QString,double> parameterItem2;

    emit CheckMSYBoxes(false);
    emit EnableRunButtons(false);
    emit LoadDataStruct();

    QStringList vectorParameterNames = m_DatabasePtr->getVectorParameterNames(m_Logger,m_ProjectName,m_ModelName);
    QString surfaceParameter1Name = getParameter1Name();
    QString surfaceParameter2Name = getParameter2Name();
    QProgressDialog* progressDlg = new QProgressDialog(
                "\nProcessing parameter variations...\n",
                "Cancel", 0, vectorParameterNames.size(),
                m_Diagnostic_Tabs);

    m_Logger->logMsg(nmfConstants::Normal,"");
    m_Logger->logMsg(nmfConstants::Normal,"Start Diagnostic");

    // Ensure the user selects 2 distinct parameters
    if (surfaceParameter1Name == surfaceParameter2Name) {
        QMessageBox::warning(m_Diagnostic_Tabs, "Warning",
                             "\nPlease select 2 different parameters to view as a 3d surface.\n",
                             QMessageBox::Ok);
        emit EnableRunButtons(true);
        progressDlg->close();
        return;
    }

    // Get the AlgorithmIdentifiers
    bool systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                m_Diagnostic_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        QMessageBox::warning(m_Diagnostic_Tabs,tr("No System Found"),
                             tr("\nPlease enter a valid System.\n"),QMessageBox::Ok);
        emit EnableRunButtons(true);
        progressDlg->close();
        return;
    }

    isAggProdBool = isAggProd(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    isAggProdStr  = (isAggProdBool) ? "1" : "0";
    if (! m_DatabasePtr->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        emit EnableRunButtons(true);
        progressDlg->close();
        return;
    }

    getSpeciesInfo(NumSpecies,SpeciesNames);
    getGuildInfo(NumGuilds,GuildNames);

    if (isAggProdBool) {
        NumSpeciesOrGuilds  = NumGuilds;
        SpeciesOrGuildNames = GuildNames;
    } else {
        NumSpeciesOrGuilds  = NumSpecies;
        SpeciesOrGuildNames = SpeciesNames;
    }

    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setValue(pInc);
    progressDlg->show();
    progressDlg->setCancelButton(0);
    progressDlg->setRange(0,nmfConstantsMSSPM::VectorParameterNames.size()+NumSpeciesOrGuilds+1); // +1 for final comment
    progressDlg->update();
    QCoreApplication::processEvents();

    m_Diagnostic_Tabs->setCursor(Qt::WaitCursor);

    checkAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    if (isSetLastRunMultiDiagnostics()) {
        m_Logger->logMsg(nmfConstants::Normal,"Using Estimated Parameters from last Multi Run: " + Algorithm);
    } else {
        m_Logger->logMsg(nmfConstants::Normal,"Using Estimated Parameters from last Single Run");
    }

    for (QString parameterName : vectorParameterNames) {
        nmfUtilsQt::updateProgressDlg(m_Logger,progressDlg,"Processing parameter: "+parameterName.toStdString(),pInc);

        EstParameter.clear();
        DiagnosticTupleVector.clear();

        // Get estimated parameter from appropriate table for all species and load into EstParameter
        if (! loadEstimatedParameter(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                                     parameterName,EstParameter)) {
            msg = "Warning (1): No estimated parameters found. Please run an Estimation prior to running this Diagnostic.";
            m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
            QMessageBox::warning(m_Diagnostic_Tabs,tr("Warning"),"\n"+msg,QMessageBox::Ok);
            progressDlg->close();
            emit EnableRunButtons(true);
            m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
qDebug() << "loaded name: " << parameterName;
qDebug() << EstParameter;
        if (parameterName == surfaceParameter1Name) {
            surfaceParameter1 = EstParameter;
        } else if (parameterName == surfaceParameter2Name) {
            surfaceParameter2 = EstParameter;
        }

        // Calculate all parameter increment values and save to table
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            estParameter = EstParameter[i];
            startVal     =  estParameter * (1.0-pctVariation/100.0);
            inc          = (estParameter - startVal)/numPoints;
            diagnosticParameterValue = startVal;

            // Skip the current species if there's no estimated data for it. This could be
            // the case for the covariate coefficient parameters if non have been estimated
            // for certain species.
            if (1) { // ((estParameter != 0) && (startVal != 0)) { // RSK revisit this logic
                for (int j=0; j<=totalNumPoints; ++j) {
if ((i == 8 or i == 9) && parameterName == "SurveyQ") {
  qDebug() << "\nparameter name: " << i << parameterName << estParameter << startVal << inc << diagnosticParameterValue; // << fitness;
}

                    if ((estParameter == 0) || (startVal == 0)) {
                        aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[i],j,0,0);
                        DiagnosticTupleVector.push_back(aDiagnosticTuple);
                    } else {
                      try {
                          parameterItem = std::make_pair(parameterName,diagnosticParameterValue);
                          fitness = calculateFitness(i,{parameterItem});
if ((i == 8 or i == 9) && parameterName == "SurveyQ") {
qDebug() << "after calculateFitness call, fitness: " << fitness;
}
                      } catch (...) {
                          msg = "Warning (2): Please run an Estimation prior to running this Diagnostic.";
                          m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
                          QMessageBox::warning(m_Diagnostic_Tabs,tr("Warning"),"\n"+msg,QMessageBox::Ok);
                          progressDlg->close();
                          emit EnableRunButtons(true);
                          m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                          return;
                      }

                      if (fitness == -1) {
                          m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                          progressDlg->close();
                          emit EnableRunButtons(true);
                          m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                          return;
                      }

                      aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[i],
                                                         diagnosticParameterValue-estParameter,
                                                         diagnosticParameterValue,
                                                         fitness);

                      DiagnosticTupleVector.push_back(aDiagnosticTuple);
                      diagnosticParameterValue += inc;
                    } // end else
                } // end for
            } // end if

        } // end for
//std::cout << "Upt: before updateParameterTable" << std::endl;
        updateParameterTable(NumSpeciesOrGuilds, totalNumPoints,
                             Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                             isAggProdStr,parameterName,DiagnosticTupleVector);
    }

    // Now save to the 2-parameter table
    // Calculate all parameter increment values and save to table to be
    // used in the 3d plots.
    sigma = 0;
    DiagnosticTupleVector.clear();
    ZScoreDiagnosticTupleVector.clear();
    centerFitness = 0;
    int numSurfacePoints = (totalNumPoints+1)*(totalNumPoints+1);
    std::vector<double> fitnesses;
    for (int SpeciesNum=0; SpeciesNum<NumSpeciesOrGuilds; ++SpeciesNum) {
        fitnesses.clear();
        nmfUtilsQt::updateProgressDlg(m_Logger,progressDlg,"Calculating 2d parameter surfaces for: "+SpeciesOrGuildNames[SpeciesNum].toStdString(),pInc);
        parameter1                =  surfaceParameter1[SpeciesNum];
        parameter1StartVal        =  parameter1 * (1.0-pctVariation/100.0);
        parameter1Inc             = (parameter1 - parameter1StartVal)/numPoints;
        parameter1PctVar          = -pctVariation;
        parameter1PctInc          = -2*parameter1PctVar/totalNumPoints;
        parameter1DiagnosticValue =  parameter1StartVal;
        for (int j=0; j<=totalNumPoints; ++j) {
            parameter2                =  surfaceParameter2[SpeciesNum];
            parameter2StartVal        =  parameter2 * (1.0-pctVariation/100.0);
            parameter2Inc             = (parameter2 - parameter2StartVal)/numPoints;
            parameter2PctVar          = -pctVariation;
            parameter2PctInc          = -2*parameter2PctVar/totalNumPoints;
            parameter2DiagnosticValue =  parameter2StartVal;
            parameterItem1 = std::make_pair(surfaceParameter1Name,parameter1DiagnosticValue);
            for (int k=0; k<=totalNumPoints; ++k) {
                parameterItem2 = std::make_pair(surfaceParameter2Name,parameter2DiagnosticValue);
                fitness = calculateFitness(SpeciesNum,{parameterItem1,parameterItem2});
                fitnesses.push_back(fitness);
                if ((j == numPoints) && (k == numPoints)) {
                    centerFitness = fitness;
                }
                if (fitness == -1) {
                    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                    emit EnableRunButtons(true);
                    return;
                }
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[SpeciesNum],
                                                   parameter1PctVar,parameter2PctVar,
                                                   fitness);
                DiagnosticTupleVector.push_back(aDiagnosticTuple);
                parameter2DiagnosticValue += parameter2Inc;
                parameter2PctVar          += parameter2PctInc;
            }
            parameter1DiagnosticValue += parameter1Inc;
            parameter1PctVar          += parameter1PctInc;
        }

        //
        // Update the ZScore vector
        //
        // Calculate sigma = sqrt( sum(x-centerFitness)^2 / n )
        m=0;
        sum = 0;
        tempVal = 0;
        for (int j=0; j<=totalNumPoints; ++j) {
            for (int k=0; k<=totalNumPoints; ++k) {
                tempVal = (fitnesses[m++]-centerFitness);
                sum += tempVal*tempVal;
            }
        }
        sigma = std::sqrt(sum/(double)numSurfacePoints);

        // Calculate ZScore = (x-centerFitness)/sigma
        m = 0;
        parameter1PctVar = -pctVariation;
        parameter1PctInc = -2*parameter1PctVar/totalNumPoints;
        for (int j=0; j<=totalNumPoints; ++j) {
            parameter2PctVar = -pctVariation;
            parameter2PctInc = -2*parameter2PctVar/totalNumPoints;
            for (int k=0; k<=totalNumPoints; ++k) {
                zscoreFitness = (sigma == 0) ? 0 : (fitnesses[m++]-centerFitness)/sigma;
//              zscoreFitness = (fitnesses[m++]-centerFitness)/sigma;
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[SpeciesNum],
                                                   parameter1PctVar,parameter2PctVar,
                                                   zscoreFitness);
//qDebug() << "tuple::fitness(z): " << std::get<3>(aDiagnosticTuple);
                ZScoreDiagnosticTupleVector.push_back(aDiagnosticTuple);
                parameter2PctVar += parameter2PctInc;
            }
            parameter1PctVar += parameter1PctInc;
        }
    }

    nmfUtilsQt::updateProgressDlg(m_Logger,progressDlg,"Saving diagnostic data to database. One moment please.",pInc);
    updateParameterTable(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                         isAggProdStr,"Absolute",DiagnosticTupleVector);
    updateParameterTable(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                         isAggProdStr,"ZScore",ZScoreDiagnosticTupleVector);
    progressDlg->close();

    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);

    emit ResetOutputWidgetsForAggProd();

    saveSettings();

    emit SetChartType("Diagnostics","Parameter Profiles");
    emit EnableRunButtons(true);
}



double
nmfDiagnostic_Tab1::calculateFitness(const int& SpeciesOrGuildNum,
                                     const std::vector<std::pair<QString,double> >& ParameterData)
{
    bool isAggProd;
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds;
    int offset = 0;
    int GrowthRateOffset,GrowthRateCovarOffset,CarryingCapacityOffset,CarryingCapacityCovarOffset;
    int CatchabilityOffset,CatchabilityCovarOffset;
    int SurveyQOffset,SurveyQCovarOffset;
    int InitBiomassOffset = 0;
    double unused2[] = {0};
    double retv = 0;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::string msg;
    std::string isAggProdStr;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string PredationForm;
    std::vector<double> parameters;
    std::vector<double> initBiomassParameters;
    std::vector<double> growthParameters;
    std::vector<double> harvestParameters;
    std::vector<double> competitionParameters;
    std::vector<double> predationParameters;
    std::vector<double> surveyQParameters;
    std::vector<double> growthRateCovCoeffParameters;
    std::vector<double> carryingCapacityCovCoeffParameters;
    std::vector<double> catchabilityCovCoeffParameters;
    std::vector<double> surveyQCovCoeffParameters;

    m_DatabasePtr->getAlgorithmIdentifiers(
                m_Diagnostic_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    checkAlgorithmIdentifiersForMultiRun(Algorithm,Minimizer,ObjectiveCriterion,Scaling);

    NumSpecies = m_DataStruct.NumSpecies;
    NumGuilds  = m_DataStruct.NumGuilds;
    isAggProd  = (m_DataStruct.CompetitionForm == "AGG-PROD");
    NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;
    isAggProdStr = std::to_string(isAggProd);

    // Load up parameters (covariates are included from within the following load methods)
    loadOutputParameters(nmfConstantsMSSPM::TableOutputInitBiomass,NumSpeciesOrGuilds,
                         Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,initBiomassParameters);
    offset = initBiomassParameters.size();
    loadGrowthParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,growthParameters,
                         offset,GrowthRateOffset,GrowthRateCovarOffset,CarryingCapacityOffset,CarryingCapacityCovarOffset);
//std::cout << "GrowthRateOffset: " << GrowthRateOffset << std::endl;
//std::cout << "GrowthRateCovarOffset: " << GrowthRateCovarOffset << std::endl;
//std::cout << "CarryingCapacityOffset: " << CarryingCapacityOffset << std::endl;
//std::cout << "CarryingCapacityCovarOffset: " << CarryingCapacityCovarOffset << std::endl;

    offset = CarryingCapacityCovarOffset;
    loadHarvestParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                          harvestParameters,offset,CatchabilityOffset,CatchabilityCovarOffset);
    offset = CatchabilityCovarOffset;
    loadCompetitionParameters(isAggProd,NumSpecies,NumGuilds,NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,competitionParameters);
    offset += competitionParameters.size();
    loadPredationParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,predationParameters);
    offset += predationParameters.size();
//    SurveyQOffset = offset;
    loadOutputParameters(nmfConstantsMSSPM::TableOutputSurveyQ,NumSpeciesOrGuilds,
                         Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,surveyQParameters);
    SurveyQCovarOffset = offset + surveyQParameters.size();
    loadOutputParameters(nmfConstantsMSSPM::TableOutputSurveyQCovariateCoeffs,NumSpeciesOrGuilds,
                         Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,surveyQCovCoeffParameters);
//qDebug() << "RSK - TBD implement surveyQCovCoeff logic";
//qDebug() << "SurveyQCovarOffset: " << SurveyQCovarOffset;

    // Append all of the Estimated Parameters to: parameters
    InitBiomassOffset  = parameters.size();
    nmfUtils::append(initBiomassParameters,parameters);
    nmfUtils::append(growthParameters,     parameters);
    nmfUtils::append(harvestParameters,    parameters);
    nmfUtils::append(competitionParameters,parameters);
    nmfUtils::append(predationParameters,  parameters);
    SurveyQOffset      = parameters.size();
    nmfUtils::append(surveyQParameters,    parameters);
    SurveyQCovarOffset = parameters.size();
    nmfUtils::append(surveyQCovCoeffParameters, parameters);

//std::cout << "ParameterData size: " << ParameterData.size() << std::endl;
    // Modify the estimated parameter data with the diagnostic "tweaks" to the estimated parameters
    for (std::pair<QString,double> ParameterItem : ParameterData) {
//std::cout << "parameter first: " << ParameterItem.first.toStdString() << std::endl;
        offset = 0; // RSK bug is here I think...
        if (ParameterItem.first        == "Initial Biomass (B₀)") {
            offset = InitBiomassOffset;
        } else if (ParameterItem.first == "Growth Rate (r)") { // RSK revisit this...make all the constants more consistent
            offset = GrowthRateOffset;
        } else if (ParameterItem.first == "Carrying Capacity (K)") {
            offset = CarryingCapacityOffset;
        } else if (ParameterItem.first == "Catchability (q)") {
            offset = CatchabilityOffset+NumSpeciesOrGuilds;
        } else if (ParameterItem.first == nmfConstantsMSSPM::ParameterNameSurveyQ) {
            offset = SurveyQOffset;
//std::cout << "SurveyQ offset: " << offset << std::endl;
        } else if (ParameterItem.first == nmfConstantsMSSPM::ParameterNameGrowthRateCovCoeff) {
            offset = GrowthRateCovarOffset;
        } else if (ParameterItem.first == nmfConstantsMSSPM::ParameterNameCarryingCapacityCovCoeff) {
            offset = CarryingCapacityCovarOffset;
        } else if (ParameterItem.first == nmfConstantsMSSPM::ParameterNameCatchabilityCovCoeff) {
            offset = CatchabilityCovarOffset;
        } else if (ParameterItem.first == nmfConstantsMSSPM::ParameterNameSurveyQCovCoeff) {
//qDebug() << "found SQCovCoeff";
            offset = SurveyQCovarOffset;
        } else {
            msg = "Error: Invalid parameter name: " + ParameterItem.first.toStdString();
            m_Logger->logMsg(nmfConstants::Error,msg);
            return -1;
        }
        parameters[offset+SpeciesOrGuildNum] = ParameterItem.second;
    }

//for (int i=150;i<160;++i) {
//std::cout << "  parameters[159]: " << parameters[159] << std::endl;
//}



    m_DatabasePtr->getAlgorithmIdentifiers(
                m_Diagnostic_Tabs,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (Algorithm == "Bees Algorithm") {
        std::unique_ptr<BeesAlgorithm> beesAlg = std::make_unique<BeesAlgorithm>(m_DataStruct,nmfConstantsMSSPM::VerboseOff);
        retv = beesAlg->evaluateObjectiveFunction(parameters);
    } else if (Algorithm == "NLopt Algorithm") {
        std::unique_ptr<NLopt_Estimator> nlopt_Estimator = std::make_unique<NLopt_Estimator>();
        nlopt_Estimator->initialize(m_DataStruct);
        retv = nlopt_Estimator->objectiveFunction(SpeciesOrGuildNum /* unused1 */ ,&parameters[0],unused2,&m_DataStruct);
        if (retv == -1) {
            msg = "Warning (3): Please run an Estimation prior to running this Diagnostic.";
            m_Logger->logMsg(nmfConstants::Warning,msg);
            QMessageBox::warning(m_Diagnostic_Tabs,tr("Warning"),tr("\n"+QString::fromStdString(msg).toLatin1()),QMessageBox::Ok);
        } else if (std::isnan(retv)) {
            msg = "Warning (4): Found nan fitness value";
            m_Logger->logMsg(nmfConstants::Warning,msg);
        }
    } else {
        retv = -1;
    }

    return retv;
}

void
nmfDiagnostic_Tab1::loadOutputParameters(
        const std::string&   TableName,
        const int&           NumSpeciesOrGuilds,
        const std::string&   Algorithm,
        const std::string&   Minimizer,
        const std::string&   ObjectiveCriterion,
        const std::string&   Scaling,
        const std::string&   isAggProd,
        std::vector<double>& Parameters)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
    queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                  TableName +
                 "  WHERE ProjectName = '"      + m_ProjectName +
                 "' AND ModelName = '"          + m_ModelName +
                 "' AND Algorithm = '"          + Algorithm +
                 "' AND Minimizer = '"          + Minimizer +
                 "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                 "' AND Scaling = '"            + Scaling +
                 "' AND isAggProd = "           + isAggProd +
                 "  ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Algorithm"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadOutputParameters: No records found in " + TableName + " table.");
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return;
    }
    if (NumRecords != NumSpeciesOrGuilds) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadOutputParameters: Found " + std::to_string(NumRecords) +
                         " record(s) in " + TableName + ", expecting " + std::to_string(NumSpeciesOrGuilds));
        m_Logger->logMsg(nmfConstants::Error,queryStr);
        return;
    }
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        Parameters.push_back(std::stod(dataMap["Value"][i]));
    }
}

void
nmfDiagnostic_Tab1::loadGrowthParameters(
        const int&           NumSpeciesOrGuilds,
        const std::string&   Algorithm,
        const std::string&   Minimizer,
        const std::string&   ObjectiveCriterion,
        const std::string&   Scaling,
        const std::string&   isAggProd,
        std::vector<double>& Parameters,
        const int&           CurrentOffset,
        int&                 GrowthRateOffset,
        int&                 GrowthRateCovarOffset,
        int&                 CarryingCapacityOffset,
        int&                 CarryingCapacityCovarOffset)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::vector<std::string> tableNames = {};

    GrowthRateOffset            = CurrentOffset;
    GrowthRateCovarOffset       = CurrentOffset;
    CarryingCapacityOffset      = CurrentOffset;
    CarryingCapacityCovarOffset = CurrentOffset;
    if (m_DataStruct.GrowthForm == "Linear") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputGrowthRate);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs);
        GrowthRateCovarOffset       = CurrentOffset + NumSpeciesOrGuilds;
        CarryingCapacityOffset      = GrowthRateCovarOffset;
        CarryingCapacityCovarOffset = GrowthRateCovarOffset;
    } else if (m_DataStruct.GrowthForm == "Logistic") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputGrowthRate);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputGrowthRateCovariateCoeffs);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCarryingCapacity);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCarryingCapacityCovariateCoeffs);
        GrowthRateCovarOffset       = CurrentOffset         + NumSpeciesOrGuilds;
        CarryingCapacityOffset      = GrowthRateCovarOffset + NumSpeciesOrGuilds;
        CarryingCapacityCovarOffset = CarryingCapacityOffset + NumSpeciesOrGuilds;
    }
    for (std::string table : tableNames) {
        fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
        queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                      table +
                     "  WHERE ProjectName = '"      + m_ProjectName +
                     "' AND ModelName = '"          + m_ModelName +
                     "' AND Algorithm = '"          + Algorithm +
                     "' AND Minimizer = '"          + Minimizer +
                     "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                     "' AND Scaling = '"            + Scaling +
                     "' AND isAggProd = "           + isAggProd +
                     "  ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadGrowthParameters: No records found in " + table + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadGrowthParameters: Found " + std::to_string(NumRecords) +
                             " record(s) in " + table + ", expecting " + std::to_string(NumSpeciesOrGuilds));
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            Parameters.push_back(std::stod(dataMap["Value"][i]));
        }
    }
}

void
nmfDiagnostic_Tab1::loadHarvestParameters(
        const int&           NumSpeciesOrGuilds,
        const std::string&   Algorithm,
        const std::string&   Minimizer,
        const std::string&   ObjectiveCriterion,
        const std::string&   Scaling,
        std::vector<double>& Parameters,
        const int&           CurrentOffset,
        int&                 CatchabilityOffset,
        int&                 CatchabilityCovarOffset)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::vector<std::string> tableNames = {};

    CatchabilityOffset      = CurrentOffset;
    CatchabilityCovarOffset = CurrentOffset;
    if ((m_DataStruct.HarvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
        (m_DataStruct.HarvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString())) {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCatchability);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCatchabilityCovariateCoeffs);
        CatchabilityOffset      = CurrentOffset      + NumSpeciesOrGuilds;
        CatchabilityCovarOffset = CatchabilityOffset + NumSpeciesOrGuilds;
    }

    for (std::string table : tableNames) {
        fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
        queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " +
                      table +
                     " WHERE ProjectName = '"       + m_ProjectName +
                     "' AND ModelName = '"          + m_ModelName +
                     "' AND Algorithm = '"          + Algorithm +
                     "' AND Minimizer = '"          + Minimizer +
                     "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                     "' AND Scaling = '"            + Scaling +
                     "' ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"Error(1) nmfDiagnostic_Tab1::loadHarvestParameters: No records found in " + table + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_Logger->logMsg(nmfConstants::Error,"Error(2) nmfDiagnostic_Tab1::loadHarvestParameters: Incorrect number of records found in " + table + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            Parameters.push_back(std::stod(dataMap["Value"][i]));
        }
    }
}


void
nmfDiagnostic_Tab1::loadCompetitionParameters(
        const bool&          isAggProd,
        const int&           theNumSpecies,
        const int&           theNumGuilds,
        const int&           NumSpeciesOrGuilds,
        const std::string&   Algorithm,
        const std::string&   Minimizer,
        const std::string&   ObjectiveCriterion,
        const std::string&   Scaling,
        std::vector<double>& Parameters)
{
    int m;
    int NumSpecies=0;
    int NumGuilds=0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::vector<std::string> tableNames = {};
    std::string OrderBy="";
    std::string msg;
    std::string isAggProdStr = (isAggProd) ? "1" : "0";

    if (m_DataStruct.CompetitionForm == "NO_K") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionAlpha);
    } else if (m_DataStruct.CompetitionForm == "MS-PROD") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaSpecies);
    } else if (m_DataStruct.CompetitionForm == "AGG-PROD") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds);
    }
    for (std::string tableName : tableNames) {
        fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " +
                      tableName;
        OrderBy    = " ORDER BY SpeciesA,SpeciesB";
        if (tableName == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Guild","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild,Value FROM " +
                          tableName;
            OrderBy    = " ORDER BY SpeName,Guild";
        }
        queryStr  += "  WHERE ProjectName = '"      + m_ProjectName +
                     "' AND ModelName = '"          + m_ModelName +
                     "' AND Algorithm = '"          + Algorithm +
                     "' AND Minimizer = '"          + Minimizer +
                     "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                     "' AND Scaling = '"            + Scaling +
                     "' AND isAggProd = "           + isAggProdStr +
                     "  "                           + OrderBy;
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            msg = "[Error 1] nmfDiagnostic_Tab1::loadCompetitionParameters: No records found in " + tableName + " table.";
            m_Logger->logMsg(nmfConstants::Error,msg);
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }

        NumSpecies = NumSpeciesOrGuilds;
        NumGuilds  = NumSpeciesOrGuilds;
        if ((tableName == nmfConstantsMSSPM::TableOutputCompetitionBetaGuilds) && (! isAggProd)) {
            NumSpecies = theNumSpecies;
            NumGuilds  = theNumGuilds;
        }
        if (NumRecords != NumSpecies*NumGuilds) {
            msg  = "[Error 2] nmfDiagnostic_Tab1::loadCompetitionParameters: Found " + std::to_string(NumRecords);
            msg += " records. Expecting: " + std::to_string(NumSpecies*NumGuilds);
            msg += " records, in " + tableName + " table.";
            m_Logger->logMsg(nmfConstants::Error,msg);
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        m = 0;
        for (int i=0; i<NumSpecies; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                Parameters.push_back(std::stod(dataMap["Value"][m++]));
            }
        }
    }
}


void
nmfDiagnostic_Tab1::loadPredationParameters(
        const int&           NumSpeciesOrGuilds,
        const std::string&   Algorithm,
        const std::string&   Minimizer,
        const std::string&   ObjectiveCriterion,
        const std::string&   Scaling,
        const std::string&   isAggProdStr,
        std::vector<double>& Parameters)
{
    bool isExponent;
    int m;
    int NumRecords;
    int TotalRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::vector<std::string> tableNames = {};
    std::string OrderBy;

    if (m_DataStruct.PredationForm == "Type I") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationRho);
    } else if (m_DataStruct.PredationForm == "Type II") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationRho);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationHandling);
    } else if (m_DataStruct.PredationForm == "Type III") {
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationRho);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationHandling);
        tableNames.push_back(nmfConstantsMSSPM::TableOutputPredationExponent);
    }
    for (std::string table : tableNames) {
        isExponent = (table == nmfConstantsMSSPM::TableOutputPredationExponent);
        fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " +
                      table;
        OrderBy    = " ORDER BY SpeciesA,SpeciesB";
        if (isExponent) {
            fields     = {"ProjectName","ModelName","Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " +
                          table;
            OrderBy    = " ORDER BY SpeName";
        }
        queryStr  += " WHERE ProjectName = '"       + m_ProjectName +
                     "' AND ModelName = '"          + m_ModelName +
                     "' AND Algorithm = '"          + Algorithm +
                     "' AND Minimizer = '"          + Minimizer +
                     "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                     "' AND Scaling = '"            + Scaling +
                     "' AND isAggProd = "           + isAggProdStr +
                     "  "                           + OrderBy;
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadPredationParameters: No records found in " + table + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        TotalRecords = (isExponent) ? NumSpeciesOrGuilds : NumSpeciesOrGuilds*NumSpeciesOrGuilds;
        if (NumRecords != TotalRecords) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadPredationParameters: Incorrect number of records found in " + table + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        m = 0;
        if (isExponent) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                Parameters.push_back(std::stod(dataMap["Value"][m++]));
            }
        } else {
            for (int i=0; i<NumSpeciesOrGuilds; ++i) {
                for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                    Parameters.push_back(std::stod(dataMap["Value"][m++]));
                }
            }
        }
    }
}


void
nmfDiagnostic_Tab1::setDataStruct(nmfStructsQt::ModelDataStruct& theDataStruct)
{
    m_DataStruct = theDataStruct;
}


void
nmfDiagnostic_Tab1::updateParameterTable(const int&         NumSpeciesOrGuilds,
                                         const int&         NumPoints,
                                         const std::string& Algorithm,
                                         const std::string& Minimizer,
                                         const std::string& ObjectiveCriterion,
                                         const std::string& Scaling,
                                         const std::string& isAggProd,
                                         const QString&     ParameterName,
                                         std::vector<DiagnosticTuple>& DiagnosticTupleVector)
{
   std::string TableName;
   std::string cmd;
   std::string errorMsg;

   parameterToTableName("output", ParameterName, TableName);
   if (TableName.empty()) {
       return;
   }
   if (DiagnosticTupleVector.size() == 0) {
       m_Logger->logMsg(nmfConstants::Error,"nmfDiagnostic_Tab1::updateParameterTable: DiagnosticTupleVector is empty");
       return;
   }
//std::cout << "Upt: " << TableName << std::endl;

   cmd = "DELETE FROM " + TableName +
         "  WHERE ProjectName = '"      + m_ProjectName +
         "' AND ModelName = '"          + m_ModelName +
         "' AND Algorithm = '"          + Algorithm +
         "' AND Minimizer = '"          + Minimizer +
         "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
         "' AND Scaling = '"            + Scaling +
         "' AND isAggProd = "           + isAggProd;

   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 1] UpdateParameterTable: DELETE error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

   cmd = "INSERT INTO " + TableName +
         " (ProjectName,ModelName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Offset,Value,Fitness) VALUES ";
   for (int m=0; m<(int)DiagnosticTupleVector.size(); ++m) {
       cmd += "('"   + m_ProjectName +
               "','" + m_ModelName +
               "','" + Algorithm +
               "','" + Minimizer +
               "','" + ObjectiveCriterion +
               "','" + Scaling +
               "',"  + isAggProd +
               ",'"  + std::get<0>(DiagnosticTupleVector[m]).toStdString() +
               "',"  + QString::number(std::get<1>(DiagnosticTupleVector[m])).toStdString() +
               ","   + QString::number(std::get<2>(DiagnosticTupleVector[m])).toStdString() +
               ","   + QString::number(std::get<3>(DiagnosticTupleVector[m])).toStdString() + "),";
   }
   cmd = cmd.substr(0,cmd.size()-1);
   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Warning,"[Warning 2] UpdateParameterTable: Write table error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Warning,"This may be due to no covariate diagnostic data available.");
       m_Logger->logMsg(nmfConstants::Warning,"cmd: " + cmd);
       return;
   }

}


void
nmfDiagnostic_Tab1::updateParameterTable(const std::string& Algorithm,
                                         const std::string& Minimizer,
                                         const std::string& ObjectiveCriterion,
                                         const std::string& Scaling,
                                         const std::string& isAggProd,
                                         const std::string& SurfaceType,
                                         std::vector<DiagnosticTuple>& DiagnosticTupleVector)
{
   int m;
   std::string cmd;
   std::string errorMsg;

   cmd = "DELETE FROM " + nmfConstantsMSSPM::TableDiagnosticSurface +
           " WHERE ProjectName = '"       + m_ProjectName +
           "' AND ModelName = '"          + m_ModelName +
           "' AND Algorithm = '"          + Algorithm +
           "' AND Minimizer = '"          + Minimizer +
           "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
           "' AND Scaling = '"            + Scaling +
           "' AND isAggProd = "           + isAggProd +
           "  AND Type = '"               + SurfaceType + "'";
   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 1a] UpdateParameterTable: DELETE error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

   m = 0;
   cmd = "INSERT INTO " + nmfConstantsMSSPM::TableDiagnosticSurface +
         " (ProjectName,ModelName,Algorithm, Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Type,parameter1PctVar,parameter2PctVar,Fitness) VALUES ";
   for (unsigned j=0; j<DiagnosticTupleVector.size(); ++j) {
       cmd += "('"   + m_ProjectName +
               "','" + m_ModelName +
               "','" + Algorithm +
               "','" + Minimizer +
               "','" + ObjectiveCriterion +
               "','" + Scaling +
               "',"  + isAggProd +
               ",'"  + std::get<0>(DiagnosticTupleVector[m]).toStdString() +
               "','" + SurfaceType +
               "',"  + QString::number(std::get<1>(DiagnosticTupleVector[m])).toStdString() +
               ","   + QString::number(std::get<2>(DiagnosticTupleVector[m])).toStdString() +
               ","   + QString::number(std::get<3>(DiagnosticTupleVector[m])).toStdString() + "),";
       ++m;
   }
   cmd = cmd.substr(0,cmd.size()-1);

   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 2a] UpdateParameterTable: Write table error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

}
