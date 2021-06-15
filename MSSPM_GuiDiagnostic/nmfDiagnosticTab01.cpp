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

    m_OutputTableName["Growth Rate (r)"]       = "OutputGrowthRate";
    m_OutputTableName["Carrying Capacity (K)"] = "OutputCarryingCapacity";
    m_OutputTableName["Initial Biomass (B₀)"]  = "OutputInitBiomass";
    m_OutputTableName["Catchability (q)"]      = "OutputCatchability";
    m_OutputTableName["SurveyQ"]               = "OutputSurveyQ";

    m_DiagnosticTableName["Growth Rate (r)"]       = "DiagnosticGrowthRate";
    m_DiagnosticTableName["Carrying Capacity (K)"] = "DiagnosticCarryingCapacity";
    m_DiagnosticTableName["Initial Biomass (B₀)"]  = "DiagnosticInitBiomass";
    m_DiagnosticTableName["Catchability (q)"]      = "DiagnosticCatchability";
    m_DiagnosticTableName["SurveyQ"]               = "DiagnosticSurveyQ";

    // Load ui as a widget from disk
    QFile file(":/forms/Diagnostic/Diagnostic_Tab01.ui");
    file.open(QFile::ReadOnly);
    m_Diagnostic_Tab1_Widget = loader.load(&file,m_Diagnostic_Tabs);
    m_Diagnostic_Tab1_Widget->setObjectName("Diagnostic_Tab1_Widget");
    file.close();

    Diagnostic_Tab1_SurfaceParameter1CMB = m_Diagnostic_Tabs->findChild<QComboBox   *>("Diagnostic_Tab1_SurfaceParameter1CMB");
    Diagnostic_Tab1_SurfaceParameter2CMB = m_Diagnostic_Tabs->findChild<QComboBox   *>("Diagnostic_Tab1_SurfaceParameter2CMB");
    m_Diagnostic_Tab1_PctVarSB           = m_Diagnostic_Tabs->findChild<QSpinBox    *>("Diagnostic_Tab1_PctVarSB");
    m_Diagnostic_Tab1_NumPtsSB           = m_Diagnostic_Tabs->findChild<QSpinBox    *>("Diagnostic_Tab1_NumPtsSB");
    m_Diagnostic_Tab1_RunPB              = m_Diagnostic_Tabs->findChild<QPushButton *>("Diagnostic_Tab1_RunPB");

    // Add the loaded widget as the new tabbed page
    m_Diagnostic_Tabs->addTab(m_Diagnostic_Tab1_Widget, tr("1. Parameter Profiles"));

    // Setup connections
    connect(m_Diagnostic_Tab1_RunPB, SIGNAL(clicked()),
            this,                    SLOT(callback_RunPB()));

    readSettings();

    Diagnostic_Tab1_SurfaceParameter1CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);
    Diagnostic_Tab1_SurfaceParameter2CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);

}

nmfDiagnostic_Tab1::~nmfDiagnostic_Tab1()
{

}

void
nmfDiagnostic_Tab1::callback_UpdateDiagnosticParameterChoices()
{
    int index;
    int NumRecords;
    std::vector<std::string> fields;
    std::string queryStr;
    std::map<std::string, std::vector<std::string> > dataMap;

    // Get Systems data
    fields     = {"SystemName","ObsBiomassType","GrowthForm","HarvestForm","WithinGuildCompetitionForm","PredationForm"};
    queryStr   = "SELECT SystemName,ObsBiomassType,GrowthForm,HarvestForm,WithinGuildCompetitionForm,PredationForm";
    queryStr  += " FROM Systems WHERE SystemName='" + m_ProjectSettingsConfig + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::callback_UpdateDiagnosticParameterChoices: No records found in table Systems for Name = "+m_ProjectSettingsConfig);
        return;
    }

    // Figure out which items should be in the pulldown lists based upon the Model structure
    Diagnostic_Tab1_SurfaceParameter1CMB->clear();
    Diagnostic_Tab1_SurfaceParameter2CMB->clear();
    Diagnostic_Tab1_SurfaceParameter1CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);
    Diagnostic_Tab1_SurfaceParameter2CMB->addItems(nmfConstantsMSSPM::VectorParameterNames);

    // Check for appropriate items in parameter combo boxes.
    if (dataMap["ObsBiomassType"][0] != "Relative") {
        for (QComboBox* cmbox : {Diagnostic_Tab1_SurfaceParameter1CMB,Diagnostic_Tab1_SurfaceParameter2CMB}) {
            index = cmbox->findText("SurveyQ");
            cmbox->removeItem(index);
        }
    }
    if (dataMap["GrowthForm"][0] != "Logistic") {
        for (QComboBox* cmbox : {Diagnostic_Tab1_SurfaceParameter1CMB,Diagnostic_Tab1_SurfaceParameter2CMB}) {
            index = cmbox->findText("Carrying Capacity (K)");
            cmbox->removeItem(index);
        }
    }
    if (dataMap["GrowthForm"][0] == "Null") {
        for (QComboBox* cmbox : {Diagnostic_Tab1_SurfaceParameter1CMB,Diagnostic_Tab1_SurfaceParameter2CMB}) {
            index = cmbox->findText("Growth Rate (r)");
            cmbox->removeItem(index);
        }
    }
    if (dataMap["HarvestForm"][0] != "Effort (qE)") {
        for (QComboBox* cmbox : {Diagnostic_Tab1_SurfaceParameter1CMB,Diagnostic_Tab1_SurfaceParameter2CMB}) {
            index = cmbox->findText("Catchability (q)");
            cmbox->removeItem(index);
        }
    }
}

void
nmfDiagnostic_Tab1::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("Diagnostics");
    m_NumPoints    = settings->value("NumPoints","").toInt();
    m_PctVariation = settings->value("Variation","").toInt();
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
    QString tableName;

    parameterToTableName("output",paramName,tableName);

    return tableName.toStdString();
}


void
nmfDiagnostic_Tab1::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfDiagnostic_Tab1::loadWidgets()");

    readSettings();
}


void
nmfDiagnostic_Tab1::loadEstimatedParameter(const std::string& Algorithm,
                                           const std::string& Minimizer,
                                           const std::string& ObjectiveCriterion,
                                           const std::string& Scaling,
                                           const QString& parameterName,
                                           std::vector<double>& EstParameter)
{
    int NumSpecies;
    QString tableName="";
    std::string queryStr;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;

    parameterToTableName("input",parameterName, tableName);
    if (tableName.isEmpty()) {
        return;
    }

    fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
    queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " + tableName.toStdString();
    queryStr  += " WHERE Algorithm='" + Algorithm + "' AND Minimizer = '" + Minimizer;
    queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
    queryStr  += "' AND Scaling = '" + Scaling;
    queryStr  += "' ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    for (int i=0; i<NumSpecies; ++i) {
        EstParameter.push_back(std::stod(dataMap["Value"][i]));
    }
}

void
nmfDiagnostic_Tab1::parameterToTableName(const std::string whichTable,
                                         const QString&    parameter,
                                               QString&    tableName)
{
    tableName = "";
    if (whichTable == "input") {
        tableName = m_OutputTableName[parameter];
    } else if (whichTable == "output") {
        tableName = m_DiagnosticTableName[parameter];
    }
}

void
nmfDiagnostic_Tab1::getGuildInfo(int& NumGuilds, QStringList& GuildNames)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from Guilds ORDER by GuildName";
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
    queryStr   = "SELECT SpeName FROM Species";
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

    fields     = {"SystemName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm",
                  "WithinGuildCompetitionForm","Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    queryStr   = "SELECT SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,";
    queryStr  += "WithinGuildCompetitionForm,Algorithm,Minimizer,ObjectiveCriterion,Scaling ";
    queryStr  += "FROM Systems where SystemName = '" + m_ProjectSettingsConfig;
    queryStr  += "' AND Algorithm = '" + Algorithm;
    queryStr  += "' AND Minimizer = '" + Minimizer;
    queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
    queryStr  += "' AND Scaling = '" + Scaling + "' ";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found in Systems" << std::endl;
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
nmfDiagnostic_Tab1::callback_RunPB()
{
    int RunLength;
    int InitialYear;
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds=0;
    int pctVariation   = m_Diagnostic_Tab1_PctVarSB->value();
    int numPoints      = m_Diagnostic_Tab1_NumPtsSB->value();
    int totalNumPoints = 2*numPoints;
    double startVal;
    double inc;
    double estParameter;
    double parameter1;
    double parameter2;
    double diagnosticParameterValue;
    double parameter1DiagnosticValue;
    double parameter2DiagnosticValue;
    double fitness;
    double parameter1StartVal;
    double parameter1PctVar;
    double parameter1PctInc;
    double parameter1Inc;
    double parameter2StartVal;
    double parameter2PctVar;
    double parameter2PctInc;
    double parameter2Inc;
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
    DiagnosticTuple aDiagnosticTuple;
    std::string isAggProdStr;
    bool isAggProdBool;
    QString msg;
    bool thereIsCarryingCapacity = false;
    std::pair<QString,double> parameterItem;
    std::pair<QString,double> parameterItem1;
    std::pair<QString,double> parameterItem2;
    QString surfaceParameter1Name = getParameter1Name();
    QString surfaceParameter2Name = getParameter2Name();

    m_Logger->logMsg(nmfConstants::Normal,"");
    m_Logger->logMsg(nmfConstants::Normal,"Start Diagnostic");

    // Ensure the user selects 2 distinct parameters
    if (surfaceParameter1Name == surfaceParameter2Name) {
        QMessageBox::warning(m_Diagnostic_Tabs, "Warning",
                             "\nPlease select 2 different parameters to view as a 3d surface.\n",
                             QMessageBox::Ok);
        return;
    }

    // Get the AlgorithmIdentifiers
    bool systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                m_Diagnostic_Tabs,m_Logger,m_ProjectSettingsConfig,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        QMessageBox::warning(m_Diagnostic_Tabs,tr("No System Found"),
                             tr("\nPlease enter a valid System.\n"),QMessageBox::Ok);
        return;
    }
    isAggProdBool = isAggProd(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    isAggProdStr  = (isAggProdBool) ? "1" : "0";
    if (! m_DatabasePtr->getModelFormData(
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,m_Logger,m_ProjectSettingsConfig)) {
        return;
    }
    thereIsCarryingCapacity = (GrowthForm == "Logistic");

    getSpeciesInfo(NumSpecies,SpeciesNames);
    getGuildInfo(NumGuilds,GuildNames);

    if (isAggProdBool) {
        NumSpeciesOrGuilds  = NumGuilds;
        SpeciesOrGuildNames = GuildNames;
    } else {
        NumSpeciesOrGuilds  = NumSpecies;
        SpeciesOrGuildNames = SpeciesNames;
    }

    m_Diagnostic_Tabs->setCursor(Qt::WaitCursor);

    for (QString parameterName : nmfConstantsMSSPM::VectorParameterNames) {
std::cout << "==> Processing: " << parameterName.toStdString() << std::endl;
        EstParameter.clear();
        DiagnosticTupleVector.clear();

        // Get estimated parameter from appropriate table for all species and load into EstParameter
        loadEstimatedParameter(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                               parameterName,EstParameter);
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
            for (int j=0; j<=totalNumPoints; ++j) {
                try {   // RSK look at this
                    parameterItem = std::make_pair(parameterName,diagnosticParameterValue);
                    fitness = calculateFitness(i,{parameterItem});
                } catch (...) {
                    msg = "Please run an Estimation prior to running this Diagnostic.";
                    m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
                    QMessageBox::warning(m_Diagnostic_Tabs,tr("Warning"),"\n"+msg,QMessageBox::Ok);
                    return;
                }

                if (fitness == -1) {
                    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[i],
                                                   diagnosticParameterValue-estParameter,
                                                   diagnosticParameterValue,
                                                   fitness);
                DiagnosticTupleVector.push_back(aDiagnosticTuple);
                diagnosticParameterValue += inc;
            }
        }

        updateParameterTable(NumSpeciesOrGuilds, totalNumPoints,
                             Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                             isAggProdStr,parameterName,DiagnosticTupleVector);

    }

    // Now save to the 2-parameter table
    // Calculate all parameter increment values and save to table to be
    // used in the 3d plots.
    DiagnosticTupleVector.clear();
    for (int SpeciesNum=0; SpeciesNum<NumSpeciesOrGuilds; ++SpeciesNum) {
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
                if (fitness == -1) {
                    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[SpeciesNum],
                                                   parameter1PctVar,
                                                   parameter2PctVar,
                                                   fitness);

                DiagnosticTupleVector.push_back(aDiagnosticTuple);
                parameter2DiagnosticValue += parameter2Inc;
                parameter2PctVar          += parameter2PctInc;
            }
            parameter1DiagnosticValue += parameter1Inc;
            parameter1PctVar          += parameter1PctInc;
        }
    }
    updateParameterTable(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                         isAggProdStr,DiagnosticTupleVector);


    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);

    emit ResetOutputWidgetsForAggProd();

    saveSettings();

    emit SetChartType("Diagnostics","Parameter Profiles");

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
    unsigned unused1 = 0;
    double unused2[] = {0};
    double retv = 0;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::string msg;
    std::string isAggProdStr;
    std::vector<double> parameters;
    std::vector<double> initBiomassParameters;
    std::vector<double> growthParameters;
    std::vector<double> harvestParameters;
    std::vector<double> competitionParameters;
    std::vector<double> predationParameters;
    std::vector<double> surveyQParameters;

    m_DatabasePtr->getAlgorithmIdentifiers(
                m_Diagnostic_Tabs,m_Logger,m_ProjectSettingsConfig,
                Algorithm,Minimizer,ObjectiveCriterion,
                Scaling,CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    emit LoadDataStruct();

    NumSpecies = m_DataStruct.NumSpecies;
    NumGuilds  = m_DataStruct.NumGuilds;
    isAggProd  = (m_DataStruct.CompetitionForm == "AGG-PROD");
    NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;
    isAggProdStr = std::to_string(isAggProd);

    // Load up parameters
    loadOutputParameters("OutputInitBiomass",NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,initBiomassParameters);
    loadGrowthParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,growthParameters);
    loadHarvestParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,harvestParameters);
    loadCompetitionParameters(isAggProd,NumSpecies,NumGuilds,NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,competitionParameters);
    loadPredationParameters(NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,predationParameters);
    loadOutputParameters("OutputSurveyQ",NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProdStr,surveyQParameters);

    nmfUtils::append(initBiomassParameters,parameters);
    nmfUtils::append(growthParameters,parameters);
    nmfUtils::append(harvestParameters,parameters);
    nmfUtils::append(competitionParameters,parameters);
    nmfUtils::append(predationParameters,parameters);
    nmfUtils::append(surveyQParameters,parameters);

    int initBiomassOffset = 0;
    int growthOffset      = initBiomassOffset + initBiomassParameters.size();
    int harvestOffset     = growthOffset      + growthParameters.size();
    int competitionOffset = harvestOffset     + harvestParameters.size();
    int predationOffset   = competitionOffset + competitionParameters.size();
    int surveyQOffset     = predationOffset   + predationParameters.size();

    for (std::pair<QString,double> ParameterItem : ParameterData) {
        offset = 0;
        if (ParameterItem.first == "Initial Biomass (B₀)") {
            offset = initBiomassOffset;
        } else if (ParameterItem.first == "Growth Rate (r)") {
            offset = growthOffset;
        } else if (ParameterItem.first == "Carrying Capacity (K)") {
            offset = growthOffset+NumSpeciesOrGuilds;
        } else if (ParameterItem.first == "Catchability (q)") {
            offset = harvestOffset;
        } else if (ParameterItem.first == "SurveyQ") {
            offset = surveyQOffset;
        } else {
            msg = "Error: Invalid parameter name: " + ParameterItem.first.toStdString();
            m_Logger->logMsg(nmfConstants::Error,msg);
            return -1;
        }
        parameters[offset+SpeciesOrGuildNum] = ParameterItem.second;
    }

    if (Algorithm == "Bees Algorithm") {
        std::unique_ptr<BeesAlgorithm> beesAlg = std::make_unique<BeesAlgorithm>(m_DataStruct,nmfConstantsMSSPM::VerboseOff);
        retv = beesAlg->evaluateObjectiveFunction(parameters);
    } else if (Algorithm == "NLopt Algorithm") {
        std::unique_ptr<NLopt_Estimator> nlopt_Estimator = std::make_unique<NLopt_Estimator>();
        retv = nlopt_Estimator->objectiveFunction(unused1,&parameters[0],unused2,&m_DataStruct);
        if (retv == -1) {
            msg = "Please run an Estimation prior to running this Diagnostic.";
            m_Logger->logMsg(nmfConstants::Warning,msg);
            QMessageBox::warning(m_Diagnostic_Tabs,tr("Warning"),tr("\n"+QString::fromStdString(msg).toLatin1()),QMessageBox::Ok);
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

    fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
    queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " + TableName;
    queryStr  += "  WHERE Algorithm = '" + Algorithm;
    queryStr  += "' AND Minimizer = '" + Minimizer;
    queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
    queryStr  += "' AND Scaling = '" + Scaling;
    queryStr  += "' AND isAggProd = " + isAggProd;
    queryStr  += "  AND MohnsRhoLabel = '' ORDER BY SpeName";
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
        std::vector<double>& Parameters)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList tableNames = {};

    if (m_DataStruct.GrowthForm == "Linear") {
        tableNames << "OutputGrowthRate";
    } else if (m_DataStruct.GrowthForm == "Logistic") {
        tableNames << "OutputGrowthRate";
        tableNames << "OutputCarryingCapacity";
    }
    for (QString table : tableNames) {
        fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
        queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " + table.toStdString();
        queryStr  += "  WHERE Algorithm = '" + Algorithm;
        queryStr  += "' AND Minimizer = '" + Minimizer;
        queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
        queryStr  += "' AND Scaling = '" + Scaling;
        queryStr  += "' AND isAggProd = " + isAggProd;
        queryStr  += "  AND MohnsRhoLabel = '' ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadGrowthParameters: No records found in " + table.toStdString() + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadGrowthParameters: Found " + std::to_string(NumRecords) +
                             " record(s) in " + table.toStdString() + ", expecting " + std::to_string(NumSpeciesOrGuilds));
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
        std::vector<double>& Parameters)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList tableNames = {};

    if (m_DataStruct.HarvestForm == "Effort (qE)") {
        tableNames << "OutputCatchability";
    }
    for (QString table : tableNames) {
        fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
        queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " + table.toStdString();
        queryStr  += " WHERE Algorithm = '" + Algorithm;
        queryStr  += "' AND Minimizer = '" + Minimizer;
        queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
        queryStr  += "' AND Scaling = '" + Scaling;
        queryStr  += "' AND MohnsRhoLabel = '' ORDER BY SpeName";
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"Error: No records found in " + table.toStdString() + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_Logger->logMsg(nmfConstants::Error,"Error 1: nmfDiagnostic_Tab1::loadHarvestParameters: Incorrect number of records found in " + table.toStdString() + " table.");
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
    QStringList tableNames = {};
    std::string OrderBy="";
    std::string msg;
    std::string isAggProdStr = (isAggProd) ? "1" : "0";

    if (m_DataStruct.CompetitionForm == "NO_K") {
        tableNames << "OutputCompetitionAlpha";
    } else if (m_DataStruct.CompetitionForm == "MS-PROD") {
        tableNames << "OutputCompetitionBetaGuilds";
        tableNames << "OutputCompetitionBetaSpecies";
    } else if (m_DataStruct.CompetitionForm == "AGG-PROD") {
        tableNames << "OutputCompetitionBetaGuilds";
    }
    for (QString table : tableNames) {
        fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " + table.toStdString();
        OrderBy    = " ORDER BY SpeciesA,SpeciesB";
        if (table == "OutputCompetitionBetaGuilds") {
            fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Guild","Value"};
            queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Guild,Value FROM " + table.toStdString();
            OrderBy    = " ORDER BY SpeName,Guild";
        }
        queryStr  += "  WHERE Algorithm = '" + Algorithm;
        queryStr  += "' AND Minimizer = '" + Minimizer;
        queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
        queryStr  += "' AND Scaling = '" + Scaling;
        queryStr  += "' AND isAggProd = " + isAggProdStr;
        queryStr  += "  AND MohnsRhoLabel = '' " + OrderBy;
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            msg = "[Error 1] nmfDiagnostic_Tab1::loadCompetitionParameters: No records found in " + table.toStdString() + " table.";
            m_Logger->logMsg(nmfConstants::Error,msg);
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }

        NumSpecies = NumSpeciesOrGuilds;
        NumGuilds  = NumSpeciesOrGuilds;
        if ((table == "OutputCompetitionBetaGuilds") && (! isAggProd)) {
            NumSpecies = theNumSpecies;
            NumGuilds  = theNumGuilds;
        }
        if (NumRecords != NumSpecies*NumGuilds) {
            msg  = "[Error 2] nmfDiagnostic_Tab1::loadCompetitionParameters: Found " + std::to_string(NumRecords);
            msg += " records. Expecting: " + std::to_string(NumSpecies*NumGuilds);
            msg += " records, in " + table.toStdString() + " table.";
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
        std::vector<double>& Parameters)
{
    bool isExponent;
    int m;
    int NumRecords;
    int TotalRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStringList tableNames = {};
    std::string OrderBy;
    std::string isAggProdStr = (isAggProd(Algorithm,Minimizer,ObjectiveCriterion,Scaling)) ? "1" : "0";

    if (m_DataStruct.PredationForm == "Type I") {
        tableNames << "OutputPredationRho";
    } else if (m_DataStruct.PredationForm == "Type II") {
        tableNames << "OutputPredationRho";
        tableNames << "OutputPredationHandling";
    } else if (m_DataStruct.PredationForm == "Type III") {
        tableNames << "OutputPredationRho";
        tableNames << "OutputPredationHandling";
        tableNames << "OutputPredationExponent";
    }
    for (QString table : tableNames) {
        isExponent = (table == "OutputPredationExponent");
        fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeciesA","SpeciesB","Value"};
        queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeciesA,SpeciesB,Value FROM " + table.toStdString();
        OrderBy    = " ORDER BY SpeciesA,SpeciesB";
        if (isExponent) {
            fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
            queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " + table.toStdString();
            OrderBy    = " ORDER BY SpeName";
        }
        queryStr  += " WHERE Algorithm = '" + Algorithm;
        queryStr  += "' AND Minimizer = '" + Minimizer;
        queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
        queryStr  += "' AND Scaling = '" + Scaling;
        queryStr  += "' AND isAggProd = " + isAggProdStr;
        queryStr  += "  AND MohnsRhoLabel = '' " + OrderBy;
        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadPredationParameters: No records found in " + table.toStdString() + " table.");
            m_Logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        TotalRecords = (isExponent) ? NumSpeciesOrGuilds : NumSpeciesOrGuilds*NumSpeciesOrGuilds;
        if (NumRecords != TotalRecords) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadPredationParameters: Incorrect number of records found in " + table.toStdString() + " table.");
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
   int m;
   QString TableName;
   std::string cmd;
   std::string errorMsg;

   parameterToTableName("output", ParameterName, TableName);
   if (TableName.isEmpty()) {
       return;
   }

   cmd = "DELETE FROM " + TableName.toStdString() +
           "  WHERE Algorithm = '" + Algorithm +
           "' AND Minimizer = '" + Minimizer +
           "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
           "' AND Scaling = '" + Scaling +
           "' AND isAggProd = " + isAggProd;
   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 1] UpdateParameterTable: DELETE error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }
std::cout << "Updating parameter table: " << TableName.toStdString() << std::endl;
   m = 0;
   cmd = "INSERT INTO " + TableName.toStdString() + " (Algorithm, Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Offset,Value,Fitness) VALUES ";
   for (int i=0; i<NumSpeciesOrGuilds; ++i) {
       for (int j=0; j<=NumPoints; ++j) {
           cmd += "('" + Algorithm + "','" + Minimizer +
                   "','" + ObjectiveCriterion +
                   "','" + Scaling +
                   "',"  + isAggProd +
                   ",'"  + std::get<0>(DiagnosticTupleVector[m]).toStdString() +
                   "',"  + std::to_string(std::get<1>(DiagnosticTupleVector[m])) +
                   ","   + std::to_string(std::get<2>(DiagnosticTupleVector[m])) +
                   ","   + std::to_string(std::get<3>(DiagnosticTupleVector[m])) + "),";
//                 ","   + std::to_string(std::log(std::get<3>(DiagnosticTupleVector[m]))) + "),";
           ++m;
       }
   }
   cmd = cmd.substr(0,cmd.size()-1);

   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 2] UpdateParameterTable: Write table error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

}


void
nmfDiagnostic_Tab1::updateParameterTable(const std::string& Algorithm,
                                         const std::string& Minimizer,
                                         const std::string& ObjectiveCriterion,
                                         const std::string& Scaling,
                                         const std::string& isAggProd,
                                         std::vector<DiagnosticTuple>& DiagnosticTupleVector)
{
   int m;
   std::string cmd;
   std::string errorMsg;

   cmd = "DELETE FROM DiagnosticSurface WHERE Algorithm = '" + Algorithm +
           "' AND Minimizer = '" + Minimizer +
           "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
           "' AND Scaling = '" + Scaling +
           "' AND isAggProd = " + isAggProd;
   errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
   if (nmfUtilsQt::isAnError(errorMsg)) {
       m_Logger->logMsg(nmfConstants::Error,"[Error 1a] UpdateParameterTable: DELETE error: " + errorMsg);
       m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

   m = 0;
   cmd = "INSERT INTO DiagnosticSurface (Algorithm, Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,parameter1PctVar,parameter2PctVar,Fitness) VALUES ";
   for (unsigned j=0; j<DiagnosticTupleVector.size(); ++j) {
       cmd += "('"   + Algorithm +
               "','" + Minimizer +
               "','" + ObjectiveCriterion +
               "','" + Scaling +
               "',"  + isAggProd +
               ",'"  + std::get<0>(DiagnosticTupleVector[m]).toStdString() +
               "',"  + std::to_string(std::get<1>(DiagnosticTupleVector[m])) +
               ","   + std::to_string(std::get<2>(DiagnosticTupleVector[m])) +
               ","   + std::to_string(std::get<3>(DiagnosticTupleVector[m])) + "),";
      //       ","   + std::to_string(std::log(std::get<3>(DiagnosticTupleVector[m]))) + "),";
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
