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
    m_logger          = logger;
    m_databasePtr     = databasePtr;
    m_projectDir      = projectDir;
    m_NumPoints       = 1;
    m_PctVariation    = 1;

    // Load ui as a widget from disk
    QFile file(":/forms/Diagnostic/Diagnostic_Tab01.ui");
    file.open(QFile::ReadOnly);
    m_Diagnostic_Tab1_Widget = loader.load(&file,m_Diagnostic_Tabs);
    m_Diagnostic_Tab1_Widget->setObjectName("Diagnostic_Tab1_Widget");
    file.close();

    // Define widgets
    m_Diagnostic_Tab1_ParameterCMB = m_Diagnostic_Tabs->findChild<QComboBox   *>("Diagnostic_Tab1_ParameterCMB");
    m_Diagnostic_Tab1_ParameterLBL = m_Diagnostic_Tabs->findChild<QLabel      *>("Diagnostic_Tab1_ParameterLBL");
    m_Diagnostic_Tab1_PctVarSB     = m_Diagnostic_Tabs->findChild<QSpinBox    *>("Diagnostic_Tab1_PctVarSB");
    m_Diagnostic_Tab1_NumPtsSB     = m_Diagnostic_Tabs->findChild<QSpinBox    *>("Diagnostic_Tab1_NumPtsSB");
    m_Diagnostic_Tab1_RunPB        = m_Diagnostic_Tabs->findChild<QPushButton *>("Diagnostic_Tab1_RunPB");

    // Add the loaded widget as the new tabbed page
    m_Diagnostic_Tabs->addTab(m_Diagnostic_Tab1_Widget, tr("1. Parameter Profiles"));

    // Setup connections
    connect(m_Diagnostic_Tab1_RunPB, SIGNAL(clicked()),
            this,                    SLOT(callback_Diagnostic_Tab1_RunPB()));

    readSettings();

    // Temporarily hide widgets since Run button will run diagnostics on all parameters
    m_Diagnostic_Tab1_ParameterLBL->setText("The following settings apply to all parameters:");
    m_Diagnostic_Tab1_ParameterLBL->setMinimumWidth(300);
    m_Diagnostic_Tab1_ParameterCMB->hide();

}

nmfDiagnostic_Tab1::~nmfDiagnostic_Tab1()
{

}


void
nmfDiagnostic_Tab1::readSettings() {
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Settings");
    m_ProjectSettingsConfig = settings.value("Name","").toString().toStdString();
    settings.endGroup();
    settings.beginGroup("Diagnostics");
    m_NumPoints    = settings.value("NumPoints","").toInt();
    m_PctVariation = settings.value("Variation","").toInt();
    settings.endGroup();
}

void
nmfDiagnostic_Tab1::saveSettings()
{
    QSettings settings("NOAA", "MSSPM");

    settings.beginGroup("Diagnostics");
    settings.setValue("Variation", m_Diagnostic_Tab1_PctVarSB->value());
    settings.setValue("NumPoints", m_Diagnostic_Tab1_NumPtsSB->value());
    settings.endGroup();
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
   readSettings();
}

void
nmfDiagnostic_Tab1::clearWidgets()
{

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

    fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","SpeName","Value"};
    queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Value FROM " + tableName.toStdString();
    queryStr  += " WHERE Algorithm='" + Algorithm + "' AND Minimizer = '" + Minimizer;
    queryStr  += "' AND ObjectiveCriterion = '" + ObjectiveCriterion;
    queryStr  += "' AND Scaling = '" + Scaling;
    queryStr  += "' ORDER BY SpeName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
    if (whichTable == "input") {
        if (parameter.contains("Growth Rate")) {
            tableName = "OutputGrowthRate";
        } else if (parameter.contains("Carrying Capacity")) {
            tableName = "OutputCarryingCapacity";
        }
    } else if (whichTable == "output") {
        if (parameter.contains("Growth Rate")) {
            tableName = "DiagnosticGrowthRate";
        } else if (parameter.contains("Carrying Capacity")) {
            tableName = "DiagnosticCarryingCapacity";
        }
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
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
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
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found in Systems" << std::endl;
        std::cout << queryStr << std::endl;
        return false;
    }
    CompetitionStr = dataMap["WithinGuildCompetitionForm"][0];

    return (CompetitionStr == "AGG-PROD");
}

void
nmfDiagnostic_Tab1::callback_Diagnostic_Tab1_RunPB()
{
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds=0;
    int pctVariation   = m_Diagnostic_Tab1_PctVarSB->value();
    int numPoints      = m_Diagnostic_Tab1_NumPtsSB->value();
    int totalNumPoints = 2*numPoints;
    double startVal;
    double inc;
    double parameter;
    double rParameter;
    double KParameter;
    double diagnosticParameter;
    double rDiagnosticParam;
    double KDiagnosticParam;
    double fitness;
    double rStartVal;
    double rPctVar;
    double rPctInc;
    double rInc;
    double KStartVal;
    double KPctVar;
    double KPctInc;
    double KInc;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::vector<double> EstParameter;
    std::vector<double> rEstParameter;
    std::vector<double> KEstParameter;
    QStringList SpeciesNames;
    QStringList GuildNames;
    QStringList SpeciesOrGuildNames;
    std::vector<DiagnosticTuple> DiagnosticTupleVector;
    DiagnosticTuple aDiagnosticTuple;
    std::string isAggProdStr;
    bool isAggProdBool;
    QString msg;

    m_logger->logMsg(nmfConstants::Normal,"");
    m_logger->logMsg(nmfConstants::Normal,"Start Diagnostic");

    // Get the AlgorithmIdentifiers
    bool systemFound = algorithmIdentifiers(Algorithm,Minimizer,
                                            ObjectiveCriterion,
                                            Scaling,CompetitionForm);
    if (! systemFound) {
        QMessageBox::warning(m_Diagnostic_Tabs,
                             tr("No System Found"),
                             tr("\nPlease enter a valid System.\n"),
                             QMessageBox::Ok);
        return;
    }
    isAggProdBool = isAggProd(Algorithm,Minimizer,ObjectiveCriterion,Scaling);
    isAggProdStr  = (isAggProdBool) ? "1" : "0";

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

    // Hardcode parameter names for diagnostics. Save to the 1-parameter tables.
    QStringList ParameterNames = {"Growth Rate (r)","Carrying Capacity (K)"};
    for (QString parameterName : ParameterNames) {

        EstParameter.clear();
        DiagnosticTupleVector.clear();

        // Get estimated parameter from appropriate table for all species and load into EstParameter
        loadEstimatedParameter(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                               parameterName,EstParameter);
        if (parameterName == "Growth Rate (r)") {
            rEstParameter = EstParameter;
        } else {
            KEstParameter = EstParameter;
        }

        // Calculate all parameter increment values and save to table
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            parameter = EstParameter[i];
            startVal  = parameter * (1.0-pctVariation/100.0);
            inc       = (parameter - startVal)/numPoints;
            diagnosticParameter = startVal;
            for (int j=0; j<=totalNumPoints; ++j) {
                try {
                    fitness = calculateFitness(i,parameterName,diagnosticParameter);
                } catch (...) {
                    msg = "Please run Estimation prior to running this Diagnostics.";
                    m_logger->logMsg(nmfConstants::Warning,msg.toStdString());
                    return;
                }

                if (fitness == -1) {
                    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[i],
                                                   diagnosticParameter-parameter,
                                                   diagnosticParameter,
                                                   fitness);
                DiagnosticTupleVector.push_back(aDiagnosticTuple);
                diagnosticParameter += inc;
            }
        }

        updateParameterTable(NumSpeciesOrGuilds, totalNumPoints,
                             Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                             isAggProdStr,parameterName,DiagnosticTupleVector);

    }

    // Now save to the 2-parameter table
    // Calculate all parameter increment values and save to table
    DiagnosticTupleVector.clear();
    for (int SpeciesNum=0; SpeciesNum<NumSpeciesOrGuilds; ++SpeciesNum) {
        rParameter       =  rEstParameter[SpeciesNum];
        rStartVal        =  rParameter * (1.0-pctVariation/100.0);
        rInc             = (rParameter - rStartVal)/numPoints;
        rPctVar          = -pctVariation;
        rPctInc          = -rPctVar/numPoints;
        rDiagnosticParam =  rStartVal;
        for (int j=0; j<=totalNumPoints; ++j) {
            KParameter       =  KEstParameter[SpeciesNum];
            KStartVal        =  KParameter * (1.0-pctVariation/100.0);
            KInc             = (KParameter - KStartVal)/numPoints;
            KPctVar          = -pctVariation;
            KPctInc          = -KPctVar/numPoints;
            KDiagnosticParam =  KStartVal;
            for (int k=0; k<=totalNumPoints; ++k) {
                fitness = calculateFitness(SpeciesNum,rDiagnosticParam,KDiagnosticParam);
                if (fitness == -1) {
                    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);
                    return;
                }
                aDiagnosticTuple = std::make_tuple(SpeciesOrGuildNames[SpeciesNum],
                                                   rPctVar,
                                                   KPctVar,
                                                   fitness);
                DiagnosticTupleVector.push_back(aDiagnosticTuple);
                KDiagnosticParam += KInc;
                KPctVar          += KPctInc;
            }
            rDiagnosticParam += rInc;
            rPctVar          += rPctInc;
        }
    }
    updateParameterTable(Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                         isAggProdStr,DiagnosticTupleVector);

    m_Diagnostic_Tabs->setCursor(Qt::ArrowCursor);

    emit ResetOutputWidgetsForAggProd();

    saveSettings();

    emit SetChartType("Diagnostics","Parameter Profiles");

}

bool
nmfDiagnostic_Tab1::algorithmIdentifiers(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling,
        std::string& CompetitionForm)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get current algorithm and run its estimation routine
    fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","WithinGuildCompetitionForm"};
    queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,WithinGuildCompetitionForm FROM Systems WHERE SystemName='" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["Algorithm"].size() == 0) {
        m_logger->logMsg(nmfConstants::Error,"Error: No algorithm type found in Systems table.");
        return false;
    }
    Algorithm          = dataMap["Algorithm"][0];
    Minimizer          = dataMap["Minimizer"][0];
    ObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    Scaling            = dataMap["Scaling"][0];
    CompetitionForm    = dataMap["WithinGuildCompetitionForm"][0];

    return true;
}

double
nmfDiagnostic_Tab1::calculateFitness(int     SpeciesOrGuildNum,
                                     QString ParameterName,
                                     double  ParameterValue)
{
    bool isAggProd;
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds;
    unsigned unused1;
    double unused2[1];
    double retv = 0;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::string msg;
    std::vector<double> parameters;

    algorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm);

    emit LoadDataStruct();

    NumSpecies = m_theDataStruct.NumSpecies;
    NumGuilds  = m_theDataStruct.NumGuilds;
    isAggProd  = (m_theDataStruct.CompetitionForm == "AGG-PROD");
    NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;

    // Load up parameters
    loadGrowthParameters(     NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,std::to_string(isAggProd),parameters);
    loadHarvestParameters(    NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);
    loadCompetitionParameters(isAggProd,NumSpecies,NumGuilds,NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);
    loadPredationParameters(  NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);

    if (ParameterName.contains("Growth Rate")) {
        parameters[SpeciesOrGuildNum] = ParameterValue;
    } else if (ParameterName.contains("Carrying Capacity")) {
        parameters[NumSpeciesOrGuilds+ SpeciesOrGuildNum] = ParameterValue; // NumSpeciesOrGuilds+ offset since need to skip over Growth Rate parameters
    } else {
        msg = "Error: Invalid parameter name: " + ParameterName.toStdString();
        m_logger->logMsg(nmfConstants::Error,msg);
        return -1;
    }

    if (Algorithm == "Bees Algorithm") {

        BeesAlgorithm *beesAlg = new BeesAlgorithm(m_theDataStruct,nmfConstantsMSSPM::VerboseOff);
        return beesAlg->evaluateObjectiveFunction(parameters);

    } else if (Algorithm == "NLopt Algorithm") {

        NLopt_Estimator *nlopt_Estimator = new NLopt_Estimator();
        retv = nlopt_Estimator->objectiveFunction(unused1,&parameters[0],unused2,&m_theDataStruct);
        if (retv == -1) {
            m_logger->logMsg(nmfConstants::Warning,"Please run Estimation prior to running this Diagnostic");
        }
        return retv;

    }
}


double
nmfDiagnostic_Tab1::calculateFitness(int     SpeciesOrGuildNum,
                                     double  rParameter,
                                     double  KParameter)
{
    bool isAggProd;
    int NumSpecies;
    int NumGuilds;
    int NumSpeciesOrGuilds;
    unsigned unused1;
    double unused2[1];
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::vector<double> parameters;

    algorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm);

    emit LoadDataStruct();

    NumSpecies = m_theDataStruct.NumSpecies;
    NumGuilds  = m_theDataStruct.NumGuilds;
    isAggProd  = (m_theDataStruct.CompetitionForm == "AGG-PROD");
    NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;

    // Load up parameters
    loadGrowthParameters(     NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,std::to_string(isAggProd),parameters);
    loadHarvestParameters(    NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);
    loadCompetitionParameters(isAggProd,NumSpecies,NumGuilds,NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);
    loadPredationParameters(  NumSpeciesOrGuilds,Algorithm,Minimizer,ObjectiveCriterion,Scaling,parameters);

    // Modify both r and K parameters
    parameters[SpeciesOrGuildNum]                     = rParameter;
    parameters[NumSpeciesOrGuilds+ SpeciesOrGuildNum] = KParameter; // NumSpeciesOrGuilds+ offset since need to skip over Growth Rate parameters

    if (Algorithm == "Bees Algorithm") {
        std::unique_ptr<BeesAlgorithm> beesAlg(new BeesAlgorithm(m_theDataStruct,nmfConstantsMSSPM::VerboseOff));
        return beesAlg->evaluateObjectiveFunction(parameters);
    } else if (Algorithm == "NLopt Algorithm") {
        std::unique_ptr<NLopt_Estimator> nlopt_Estimator(new NLopt_Estimator());
        return nlopt_Estimator->objectiveFunction(unused1,&parameters[0],unused2,&m_theDataStruct);
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

    if (m_theDataStruct.GrowthForm == "Linear") {
        tableNames << "OutputGrowthRate";
    } else if (m_theDataStruct.GrowthForm == "Logistic") {
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
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadGrowthParameters: No records found in " + table.toStdString() + " table.");
            m_logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadGrowthParameters: Found " + std::to_string(NumRecords) +
                             " record(s) in " + table.toStdString() + ", expecting " + std::to_string(NumSpeciesOrGuilds));
            m_logger->logMsg(nmfConstants::Error,queryStr);
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

    if (m_theDataStruct.HarvestForm == "QE") {
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
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_logger->logMsg(nmfConstants::Error,"Error: No records found in " + table.toStdString() + " table.");
            m_logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        if (NumRecords != NumSpeciesOrGuilds) {
            m_logger->logMsg(nmfConstants::Error,"Error 1: nmfDiagnostic_Tab1::loadHarvestParameters: Incorrect number of records found in " + table.toStdString() + " table.");
            m_logger->logMsg(nmfConstants::Error,queryStr);
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

    if (m_theDataStruct.CompetitionForm == "NO_K") {
        tableNames << "OutputCompetitionAlpha";
    } else if (m_theDataStruct.CompetitionForm == "MS-PROD") {
        tableNames << "OutputCompetitionBetaGuilds";
        tableNames << "OutputCompetitionBetaSpecies";
    } else if (m_theDataStruct.CompetitionForm == "AGG-PROD") {
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
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            msg = "[Error 1] nmfDiagnostic_Tab1::loadCompetitionParameters: No records found in " + table.toStdString() + " table.";
            m_logger->logMsg(nmfConstants::Error,msg);
            m_logger->logMsg(nmfConstants::Error,queryStr);
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
            m_logger->logMsg(nmfConstants::Error,msg);
            m_logger->logMsg(nmfConstants::Error,queryStr);
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

    if (m_theDataStruct.PredationForm == "Type I") {
        tableNames << "OutputPredation";
    } else if (m_theDataStruct.PredationForm == "Type II") {
        tableNames << "OutputPredation";
        tableNames << "OutputHandling";
    } else if (m_theDataStruct.PredationForm == "Type III") {
        tableNames << "OutputPredation";
        tableNames << "OutputHandling";
        tableNames << "OutputExponent";
    }
    for (QString table : tableNames) {
        isExponent = (table == "OutputExponent");
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
        dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["Algorithm"].size();
        if (NumRecords == 0) {
            m_logger->logMsg(nmfConstants::Error,"[Error 1] nmfDiagnostic_Tab1::loadPredationParameters: No records found in " + table.toStdString() + " table.");
            m_logger->logMsg(nmfConstants::Error,queryStr);
            return;
        }
        TotalRecords = (isExponent) ? NumSpeciesOrGuilds : NumSpeciesOrGuilds*NumSpeciesOrGuilds;
        if (NumRecords != TotalRecords) {
            m_logger->logMsg(nmfConstants::Error,"[Error 2] nmfDiagnostic_Tab1::loadPredationParameters: Incorrect number of records found in " + table.toStdString() + " table.");
            m_logger->logMsg(nmfConstants::Error,queryStr);
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
nmfDiagnostic_Tab1::setDataStruct(Data_Struct& theDataStruct)
{
    m_theDataStruct = theDataStruct;
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

   cmd = "DELETE FROM " + TableName.toStdString() +
           "  WHERE Algorithm = '" + Algorithm +
           "' AND Minimizer = '" + Minimizer +
           "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
           "' AND Scaling = '" + Scaling +
           "' AND isAggProd = " + isAggProd;
   errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
   if (errorMsg != " ") {
       m_logger->logMsg(nmfConstants::Error,"[Error 1] UpdateParameterTable: DELETE error: " + errorMsg);
       m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

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
           ++m;
       }
   }
   cmd = cmd.substr(0,cmd.size()-1);
   errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
   if (errorMsg != " ") {
       m_logger->logMsg(nmfConstants::Error,"[Error 2] UpdateParameterTable: Write table error: " + errorMsg);
       m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
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

   cmd = "DELETE FROM DiagnosticGRandCC WHERE Algorithm = '" + Algorithm +
           "' AND Minimizer = '" + Minimizer +
           "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
           "' AND Scaling = '" + Scaling +
           "' AND isAggProd = " + isAggProd;
   errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
   if (errorMsg != " ") {
       m_logger->logMsg(nmfConstants::Error,"[Error 1a] UpdateParameterTable: DELETE error: " + errorMsg);
       m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

   m = 0;
   cmd = "INSERT INTO DiagnosticGRandCC (Algorithm, Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,rPctVariation,KPctVariation,Fitness) VALUES ";
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
       ++m;
   }
   cmd = cmd.substr(0,cmd.size()-1);
   errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
   if (errorMsg != " ") {
       m_logger->logMsg(nmfConstants::Error,"[Error 2a] UpdateParameterTable: Write table error: " + errorMsg);
       m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
       return;
   }

}
