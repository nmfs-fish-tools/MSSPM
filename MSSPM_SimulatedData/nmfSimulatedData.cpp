
#include "nmfSimulatedData.h"


nmfSimulatedData::nmfSimulatedData(
        nmfDatabase*  database,
        nmfLogger*    logger,
        std::string&  projectName,
        std::string&  modelName,
        nmfStructsQt::ModelDataStruct& dataStruct)
{
    m_ProjectName = projectName;
    m_ModelName   = modelName;
    m_Database    = database;
    m_Logger      = logger;
    m_FixedSeed   = 0;
    m_DataStruct  = dataStruct;
}

bool
nmfSimulatedData::createSimulatedBiomass(const int& errorPct,
                                         QString& filename,
                                         QString& errorMsg)
{
    bool retv = true;
    bool isBiomassAbsolute;
    int RunLength=0;
    int InitialYear=0;
    int NumGuilds=0;
    int NumSpecies=0;
    int NumYears = 0;
    int timeMinus1;
    QStringList GuildList;
    QStringList SpeciesList;
    std::vector<double> InitialBiomass;
    std::vector<double> GrowthRate;
    std::vector<double> GrowthRateCovariateCoeffs;
    std::vector<double> SpeciesK;
    std::vector<double> SpeciesKCovariateCoeffs;
    std::vector<double> CatchabilityCovariateCoeffs;
    std::vector<int> GuildNum;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    boost::numeric::ublas::matrix<double> InitBiomassCovariate;
    boost::numeric::ublas::matrix<double> GrowthRateCovariate;
    boost::numeric::ublas::matrix<double> SpeciesKCovariate;
    boost::numeric::ublas::matrix<double> SurveyQCovariate;
    boost::numeric::ublas::matrix<double> CatchabilityCovariate;
    boost::numeric::ublas::matrix<double> PredationRhoCovariate;
    boost::numeric::ublas::matrix<double> PredationHandlingCovariate;
    boost::numeric::ublas::matrix<double> PredationExponentCovariate;
    boost::numeric::ublas::matrix<double> CompetitionAlphaCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildSpeciesCovariate;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildGuildCovariate;
    boost::numeric::ublas::matrix<double> EstBiomassSpecies;
    boost::numeric::ublas::matrix<double> SimulatedBiomassGuild;
    boost::numeric::ublas::matrix<double> Catch;
    boost::numeric::ublas::matrix<double> Effort;
    boost::numeric::ublas::matrix<double> Exploitation;
    boost::numeric::ublas::matrix<double> PredationRho;
    boost::numeric::ublas::matrix<double> PredationHandling;
    boost::numeric::ublas::matrix<double> CompetitionAlpha;
    boost::numeric::ublas::matrix<double> CompetitionBetaSpecies;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuild;
    boost::numeric::ublas::matrix<double> CompetitionBetaGuildGuild;
    boost::numeric::ublas::matrix<double> SimulatedBiomassByGuilds;
    std::vector<double> PredationExponent;
    std::vector<double> Catchability;
    double EstBiomassVal=0;
    double speciesK=0;
    double GrowthTerm=0;
    double HarvestTerm=0;
    double CompetitionTerm=0;
    double PredationTerm=0;
    double SystemCarryingCapacity;
    double GuildCarryingCapacity;
    std::map<int,std::vector<int> > GuildSpecies;
    std::vector<double> SurveyQ;
    double surveyQ;
    double SurveyQCovariateCoeff = 0.0; // RSK estimate this later
    std::string covariateAlgorithmType = m_DataStruct.CovariateAlgorithmType;
qDebug() << "--> " << QString::fromStdString(covariateAlgorithmType);
    // Get some model data
    if (! m_Database->getModelFormData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                RunLength,InitialYear,isBiomassAbsolute)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getModelFormData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }
    NumYears = RunLength + 1; // to include the last year

    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"InitBiomass",                   InitBiomassCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"GrowthRate",                    GrowthRateCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CarryingCapacity",              SpeciesKCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"SurveyQ",                       SurveyQCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"Catchability",                  CatchabilityCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationRho",                  PredationRhoCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationHandling",             PredationHandlingCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"PredationExponent",             PredationExponentCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionAlpha",              CompetitionAlphaCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaSpeciesSpecies", CompetitionBetaSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildSpecies",   CompetitionBetaGuildSpeciesCovariate);
    nmfUtilsQt::getCovariates(m_DataStruct,NumYears,"CompetitionBetaGuildGuild",     CompetitionBetaGuildGuildCovariate);

    // Get SurveyQ data
    if (! m_Database->getSurveyQData(m_Logger,SurveyQ)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getSurveyQData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }
    if (isBiomassAbsolute) {
        for (int i=0; i<(int)SurveyQ.size(); ++i) {
            SurveyQ[i] = 1.0;
        }
    }

    // Get initial data: B(0), r, and K (just to start with the Schaefer model)
    if (! m_Database->getSpeciesInitialData(
                m_Logger,NumSpecies,SpeciesList,InitialBiomass,
                GrowthRate,SpeciesK)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getSpeciesInitialData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }
    // Get the guild data
    //Get initial species covariate data
    if (! m_Database->getSpeciesInitialCovariateData(
                m_Logger,m_ProjectName,m_ModelName,
                GrowthRateCovariateCoeffs,
                SpeciesKCovariateCoeffs,
                CatchabilityCovariateCoeffs)) {
            errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getSpeciesInitialCovariateData";
            m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
            return false;
    }

    if (! m_Database->getGuildData(m_Logger,NumGuilds,NumYears,GuildList,GuildSpecies,
                                   GuildNum,SimulatedBiomassByGuilds)) {
        return false;
    }

    nmfUtils::initialize(EstBiomassSpecies,NumYears,NumSpecies);
    nmfUtils::initialize(Catch,            NumYears,NumSpecies);
    nmfUtils::initialize(Effort,           NumYears,NumSpecies);
    nmfUtils::initialize(Exploitation,     NumYears,NumSpecies);

    // Get harvest data
    if (! m_Database->getHarvestData(HarvestForm,m_Logger,m_ProjectName,m_ModelName,
                                     NumSpecies,NumYears,Catch,Effort,Exploitation,
                                     Catchability)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getHarvestData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }

    // Get predation data
    if (! m_Database->getPredationData(PredationForm,m_Logger,m_ProjectName,m_ModelName,
                                       NumSpecies,PredationRho,PredationHandling,
                                       PredationExponent)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getPredationData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }

    // Get competition data
    if (! m_Database->getCompetitionData(CompetitionForm,m_Logger,m_ProjectName,m_ModelName,
                                         NumSpecies,NumGuilds,CompetitionAlpha,CompetitionBetaSpecies,
                                         CompetitionBetaGuild,CompetitionBetaGuildGuild)) {
        errorMsg = "nmfSimulatedData::createSimulatedBiomass: Error calling getCompetitionData";
        m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return false;
    }

    bool isAggProd = (CompetitionForm == "AGG_PROD");
    double InitBiomassCoeff = 0.0; // RSK this will be estimated eventually

    getSystemCarryingCapacity(isAggProd,NumGuilds,SpeciesK,
                              GuildSpecies,SystemCarryingCapacity);

    // Load initial biomasses for all species prior to starting loop because some
    // models wlll require other species' previous year's biomass
qDebug() << "Init Biomass for species 0: " << InitialBiomass[0];
    for (int species=0; species<NumSpecies; ++species) {
       EstBiomassSpecies(0,species) = InitialBiomass[species];
//     EstBiomassSpecies(0,species) = InitialBiomass[species]*(1.0+InitBiomassCoeff*InitBiomassCovariate(0,species));
//     EstBiomassSpecies(0,species) = nmfUtils::applyCovariate(m_Logger,
//                    covariateAlgorithmType,InitialBiomass[species],
//                    InitBiomassCoeff,InitBiomassCovariate(0,species));
    }

    //
    // Create simulated biomass
    //
    double val;
    nmfGrowthForm  SimGrowthForm(GrowthForm);
    nmfHarvestForm SimHarvestForm(HarvestForm);
    nmfPredationForm SimPredationForm(PredationForm);
    nmfCompetitionForm SimCompetitionForm(CompetitionForm);

    for (int time=1; time<NumYears; ++time) {
        timeMinus1 = time - 1;

        for (int species=0; species<NumSpecies; ++species) {

            if (timeMinus1 == 0) {
                EstBiomassVal = InitialBiomass[species];
//              EstBiomassVal = InitialBiomass[species]*(1.0+InitBiomassCoeff*InitBiomassCovariate(timeMinus1,species));
//              EstBiomassVal = nmfUtils::applyCovariate(m_Logger,
//                          covariateAlgorithmType,InitialBiomass[species],
//                          InitBiomassCoeff,InitBiomassCovariate(timeMinus1,species));
if (time == 1) {
    qDebug() << "EstBiomassVal: " << EstBiomassVal;
    qDebug() << "covariateAlgorithmType: " << QString::fromStdString(covariateAlgorithmType);
    qDebug() << "InitialBiomass[species]: " << InitialBiomass[species];
    qDebug() << "InitBiomassCoeff: " << InitBiomassCoeff;
    qDebug() << "InitBiomassCovariate(timeMinus1,species): " << InitBiomassCovariate(timeMinus1,species);
}
            } else {
                EstBiomassVal = EstBiomassSpecies(timeMinus1,species);
            }
            // Multiply Biomass by SurveyQ in case it's Relative Biomass. SurveyQ will be all 1's
            // if it's Absolute Biomass.
            surveyQ = SurveyQ[species];
//          EstBiomassVal *= surveyQ*(1.0+SurveyQCovariateCoeff*SurveyQCovariate(time,species));
            EstBiomassVal *= nmfUtils::applyCovariate(m_Logger,
                        covariateAlgorithmType,surveyQ,
                        SurveyQCovariateCoeff,SurveyQCovariate(time,species));
if (time == 1) {
  qDebug() << "EstBiomassVal(2): " << EstBiomassVal;
}

            speciesK  = SpeciesK[species];
            if (speciesK == 0) {
                errorMsg = QString("nmfSimulatedData::createSimulatedBiomass: Found SpeciesK=0 for Species (") +
                           SpeciesList[species] + ")";
                m_Logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
                return false;
            }

            getGuildCarryingCapacity(isAggProd,species,SpeciesK,GuildNum,
                                     GuildSpecies,GuildCarryingCapacity);

            GrowthTerm = SimGrowthForm.evaluate(
                        covariateAlgorithmType,
                        EstBiomassVal,
                        GrowthRate[species],
                        GrowthRateCovariateCoeffs[species],
                        GrowthRateCovariate(timeMinus1,species),
                        SpeciesK[species],
                        SpeciesKCovariateCoeffs[species],
                        SpeciesKCovariate(timeMinus1,species));
            HarvestTerm = SimHarvestForm.evaluate(
                        covariateAlgorithmType,
                        timeMinus1,species,EstBiomassVal,
                        Catch,Effort,Exploitation,
                        Catchability[species],
                        CatchabilityCovariateCoeffs[species],
                        CatchabilityCovariate(timeMinus1,species));
            CompetitionTerm = SimCompetitionForm.evaluate(
                        covariateAlgorithmType,
                        timeMinus1,species,EstBiomassVal,
                        GrowthRate,
                        GrowthRateCovariate,
                        GuildCarryingCapacity,
                        SystemCarryingCapacity,
                        EstBiomassSpecies,
                        SimulatedBiomassGuild,
                        CompetitionAlpha,
                        CompetitionAlphaCovariate,
                        CompetitionBetaSpecies,
                        CompetitionBetaSpeciesCovariate,
                        CompetitionBetaGuild,
                        CompetitionBetaGuildSpeciesCovariate,
                        CompetitionBetaGuildGuild,
                        CompetitionBetaGuildGuildCovariate);
            PredationTerm = SimPredationForm.evaluate(
                        covariateAlgorithmType,
                        timeMinus1, species,
                        EstBiomassSpecies,EstBiomassVal,
                        PredationRho,PredationRhoCovariate,
                        PredationHandling,PredationHandlingCovariate,
                        PredationExponent,PredationExponentCovariate);
            val = EstBiomassVal + GrowthTerm - HarvestTerm - CompetitionTerm - PredationTerm;
if (time == 1) {
 qDebug() << val << " = " << EstBiomassVal << " + " << GrowthTerm << " - " << HarvestTerm << " - " << CompetitionTerm << " - " << PredationTerm;
}
            addError(errorPct,val);
            EstBiomassSpecies(time,species) = (val < 0) ? 0 : val;
        }
    }

//    // Multiply Biomass by SurveyQ in case it's Relative Biomass. SurveyQ will be all 1's
//    // if it's Absolute Biomass.
//    for (int species=0; species<(int)EstBiomassSpecies.size2(); ++species) {
//        surveyQ = SurveyQ[species];
//        for (int time=0; time<(int)EstBiomassSpecies.size1(); ++time) {
//            EstBiomassSpecies(time,species) *= surveyQ*(1.0+SurveyQCovariateCoeff*SurveyQCovariate(time,species));
//        }
//    }

    // Write out Biomass data to .csv file
    // Assure that file has a .csv extension
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().toLower() != "csv") {
        filename += ".csv";
    }
    QFile file(filename);
    QString value;
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        int numRows = EstBiomassSpecies.size1();
        int numCols = EstBiomassSpecies.size2();
        for (int col=0; col<numCols; ++col) {
            stream << "," << SpeciesList[col];
        }
        stream << "\n";
        for (int row=0; row<numRows; ++row) {
            stream << QString::number(row+InitialYear);
            for (int col=0; col<numCols; ++col) {
                value = QString::number(EstBiomassSpecies(row,col),'f',6);
                stream << "," << value;
            }
            stream << "\n";
        }
        file.close();
    }

    return retv;
}

void
nmfSimulatedData::addError(const int& errorPct,
                           double& value)
{
    double factor = nmfUtils::getRandomNumber(m_FixedSeed++,-1,1);
    double error  = value * (errorPct/100.0);

    value += factor*error;
    value  = (value < 0) ? 0 : value;
}

void
nmfSimulatedData::getSystemCarryingCapacity(
        const bool isAggProd,
        const int& NumGuilds,
        const std::vector<double>& SpeciesK,
        std::map<int,std::vector<int> >& GuildSpecies,
        double& SystemCarryingCapacity)
{
    SystemCarryingCapacity = 0;
    if (isAggProd) {
        for (int i=0; i<NumGuilds; ++i) {
            SystemCarryingCapacity += SpeciesK[i];
        }
    } else {
        for (int i=0; i<NumGuilds; ++i) {
            for (unsigned j=0; j<(unsigned)GuildSpecies[i].size(); ++j) {
                SystemCarryingCapacity += SpeciesK[GuildSpecies[i][j]];
            }
        }
    }
}

void
nmfSimulatedData::getGuildCarryingCapacity(
        const bool isAggProd,
        const int& species,
        const std::vector<double>& SpeciesK,
        const std::vector<int>& GuildNum,
        std::map<int,std::vector<int> >& GuildSpecies,
        double& GuildCarryingCapacity)
{
    GuildCarryingCapacity = 0;
    if (isAggProd) {
        GuildCarryingCapacity = SpeciesK[species];
    } else {
        int guildNum = GuildNum[species];
        for (unsigned j=0; j<GuildSpecies[guildNum].size(); ++j) {
            GuildCarryingCapacity += SpeciesK[GuildSpecies[guildNum][j]];
        }
    }
}

/*
bool
nmfSimulatedData::simulateGrowth(const std::string& GrowthType,
                                 const int& Year,
                                 const int& Species,
                                 const std::vector<double>& GrowthRate,
                                 const std::vector<double>& SpeciesK,
                                 const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                                 double& growthValue)
{
//std::cout << "GrowthType: " << GrowthType << std::endl;
    nmfGrowthForm GrowthForm(GrowthType);
    growthValue = GrowthForm.evaluate(
                Species,SimulatedBiomass(Year-1,Species),
                GrowthRate,SpeciesK);
    return true;
}

bool
nmfSimulatedData::simulateHarvest(const std::string& HarvestType,
                                  const int& Year,
                                  const int& Species,
                                  const boost::numeric::ublas::matrix<double>& Catch,
                                  const boost::numeric::ublas::matrix<double>& Effort,
                                  const boost::numeric::ublas::matrix<double>& Exploitation,
                                  const std::vector<double>& Catchability,
                                  const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                                  double& harvestValue)
{
    nmfHarvestForm HarvestForm(HarvestType);
    harvestValue = HarvestForm.evaluate(Year,Species,
                                        Catch,Effort,Exploitation,
                                        SimulatedBiomass(Year,Species),
                                        Catchability);
    return true;
}

bool
nmfSimulatedData::simulatePredation(const std::string& PredationType,
                                    const int& Species,
                                    const int& Year,
                                    const boost::numeric::ublas::matrix<double>& PredationRho,
                                    const boost::numeric::ublas::matrix<double>& PredationHandling,
                                    const std::vector<double>& PredationExponent,
                                    const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                                    double& predationValue)
{
    int timeMinus1 = Year-1;
    double BiomassAtTime = SimulatedBiomass(Year,Species);

    nmfPredationForm PredationForm(PredationType);
    predationValue = PredationForm.evaluate(
                timeMinus1, Species,
                PredationRho,PredationHandling,PredationExponent,
                SimulatedBiomass,BiomassAtTime); //SimulatedBiomass(timeMinus1,Species));

    return true;
}

bool
nmfSimulatedData::simulateCompetition(const std::string& CompetitionType,
                                      const int& Species,
                                      const int& Year,
                                      const std::vector<double>& GrowthRate,
                                      const double& SystemCarryingCapacity,
                                      const double& GuildCarryingCapacity,
                                      const boost::numeric::ublas::matrix<double>& CompetitionAlpha,
                                      const boost::numeric::ublas::matrix<double>& CompetitionBetaSpecies,
                                      const boost::numeric::ublas::matrix<double>& CompetitionBetaGuild,
                                      const boost::numeric::ublas::matrix<double>& CompetitionBetaGuildGuild,
                                      const boost::numeric::ublas::matrix<double>& SimulatedBiomassSpecies,
                                      const boost::numeric::ublas::matrix<double>& SimulatedBiomassGuild,
                                      double& competitionValue)
{
    int timeMinus1 = Year-1;
    double BiomassAtTime = SimulatedBiomassSpecies(Year,Species);

    nmfCompetitionForm CompetitionForm(CompetitionType);
    competitionValue = CompetitionForm.evaluate(
                timeMinus1, Species,BiomassAtTime,SystemCarryingCapacity,GrowthRate,
                GuildCarryingCapacity,CompetitionAlpha,CompetitionBetaSpecies,
                CompetitionBetaGuild,CompetitionBetaGuildGuild,
                SimulatedBiomassSpecies,SimulatedBiomassGuild);

    return true;
}
*/




