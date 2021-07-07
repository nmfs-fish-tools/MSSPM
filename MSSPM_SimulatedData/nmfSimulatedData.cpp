
#include "nmfSimulatedData.h"


nmfSimulatedData::nmfSimulatedData(
        std::string&  projectName,
        std::string&  modelName,
        nmfDatabase*  database,
        nmfLogger*    logger)
{
    m_ProjectName = projectName;
    m_ModelName   = modelName;
    m_Database    = database;
    m_Logger      = logger;
}

bool
nmfSimulatedData::createSimulatedBiomass(QString filename,
                                         const int& errorPct)
{
    bool retv = true;
    int RunLength=0;
    int InitialYear=0;
    int NumGuilds=0;
    int NumSpecies=0;
    int timeMinus1;
    QStringList GuildList;
    QStringList SpeciesList;
    std::vector<double> InitialBiomass;
    std::vector<double> GrowthRate;
    std::vector<double> SpeciesK;
    std::vector<int> GuildNum;
    std::string msg;
    std::string GrowthForm;
    std::string HarvestForm;
    std::string CompetitionForm;
    std::string PredationForm;
    QString formCheck = "TBD";
    boost::numeric::ublas::matrix<double> SimulatedBiomass;
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
    double lastYearBiomass=0;
    double bmInitial=0;
    double speciesK=0;
    double growth=0;
    double simGrowthValue=0;
    double simHarvestValue=0;
    double simCompetitionValue=0;
    double simPredationValue=0;
    double SystemCarryingCapacity;
    double GuildCarryingCapacity;
    std::map<int,std::vector<int> > GuildSpecies;

    // First run the Mohns Rho
    if (! m_Database->getModelFormData(GrowthForm,HarvestForm,CompetitionForm,PredationForm,
                                       RunLength,InitialYear,m_Logger,m_ProjectName,m_ModelName)) {
        msg = "nmfSimulatedData::createSimulatedBiomass: Error calling getModelFormData";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    RunLength++; // to include the last year

    // Get initial data: B(0), r, and K (just to start with the Schaefer model)
    if (! m_Database->getSpeciesInitialData(NumSpecies,SpeciesList,InitialBiomass,
                                GrowthRate,SpeciesK,m_Logger)) {
        msg = "nmfSimulatedData::createSimulatedBiomass: Error calling getSpeciesInitialData";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    // Get the guild data

    if (! m_Database->getGuildData(m_Logger,NumGuilds,RunLength,GuildList,GuildSpecies,
                                   GuildNum,SimulatedBiomassByGuilds)) {
        return false;
    }

    nmfUtils::initialize(SimulatedBiomass,RunLength,NumSpecies);
    nmfUtils::initialize(Catch,RunLength,NumSpecies);
    nmfUtils::initialize(Effort,RunLength,NumSpecies);
    nmfUtils::initialize(Exploitation,RunLength,NumSpecies);

    // Get harvest data
    if (! m_Database->getHarvestData(HarvestForm,m_Logger,m_ProjectName,m_ModelName,
                                     NumSpecies,RunLength,Catch,Effort,Exploitation,
                                     Catchability)) {
        msg = "nmfSimulatedData::createSimulatedBiomass: Error calling getHarvestData";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    // Get predation data
    if (! m_Database->getPredationData(PredationForm,m_Logger,m_ProjectName,m_ModelName,
                                       NumSpecies,PredationRho,PredationHandling,
                                       PredationExponent)) {
        msg = "nmfSimulatedData::createSimulatedBiomass: Error calling getPredationData";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    // Get competition data
    if (! m_Database->getCompetitionData(CompetitionForm,m_Logger,m_ProjectName,m_ModelName,
                                         NumSpecies,NumGuilds,CompetitionAlpha,CompetitionBetaSpecies,
                                         CompetitionBetaGuild,CompetitionBetaGuildGuild)) {
        msg = "nmfSimulatedData::createSimulatedBiomass: Error calling getCompetitionData";
        m_Logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    bool isAggProd = (CompetitionForm == "AGG_PROD");

    getCompetitionSystemCarryingCapacity(isAggProd,NumGuilds,GuildSpecies,
                                         SpeciesK,SystemCarryingCapacity);

    // Load initial biomasses for all species prior to starting loop because some
    // models wlll require other species' previous year's biomass
    for (int species=0; species<NumSpecies; ++species) {
        bmInitial = InitialBiomass[species];
        SimulatedBiomass(0,species) = bmInitial;
    }

    //
    // Create simulated biomass
    //
    formCheck = "OK";
    double val;
    nmfGrowthForm  SimGrowthForm(GrowthForm);
    nmfHarvestForm SimHarvestForm(HarvestForm);
    nmfPredationForm SimPredationForm(PredationForm);
    nmfCompetitionForm SimCompetitionForm(CompetitionForm);

    for (int time=1; time<RunLength; ++time) {

        timeMinus1 = time - 1;
        for (int species=0; species<NumSpecies; ++species) {

            lastYearBiomass = SimulatedBiomass(timeMinus1,species);

            growth    = GrowthRate[species];
            speciesK  = SpeciesK[species];
            if (speciesK == 0) {
                msg = "nmfSimulatedData::createSimulatedBiomass: Found SpeciesK=0 for Species (" +
                        SpeciesList[species].toStdString() + ")";
                m_Logger->logMsg(nmfConstants::Error,msg);
                return false;
            }

            getCompetitionGuildCarryingCapacity(isAggProd,species,SpeciesK,GuildNum,
                                                GuildSpecies,GuildCarryingCapacity);

            simGrowthValue = SimGrowthForm.evaluate(species,lastYearBiomass,
                                                    GrowthRate,SpeciesK);
            simHarvestValue = SimHarvestForm.evaluate(timeMinus1,species,
                                                    Catch,Effort,Exploitation,
                                                    lastYearBiomass,Catchability);
            simCompetitionValue = SimCompetitionForm.evaluate(
                                   timeMinus1, species,lastYearBiomass,
                                   SystemCarryingCapacity,
                                   GrowthRate,
                                   GuildCarryingCapacity,
                                   CompetitionAlpha,
                                   CompetitionBetaSpecies,
                                   CompetitionBetaGuild,
                                   CompetitionBetaGuildGuild,
                                   SimulatedBiomass,
                                   SimulatedBiomassGuild);
            simPredationValue = SimPredationForm.evaluate(
                                   timeMinus1, species,
                                   PredationRho,PredationHandling,PredationExponent,
                                   SimulatedBiomass,lastYearBiomass);

            val = lastYearBiomass + simGrowthValue - simHarvestValue - simCompetitionValue - simPredationValue;

std::cout << "sim year: " << time << ", val = " << lastYearBiomass << " + " << simGrowthValue << " - " << simHarvestValue << " - "
          << simCompetitionValue << " - " << simPredationValue <<  " = " << val << std::endl;

            addError(val,errorPct);
            SimulatedBiomass(time,species) = (val < 0) ? 0 : val;
//std::cout << "sim val(" << time << "," << species << "): " << SimulatedBiomass(time,species) << std::endl;
        }
    }

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
        int numRows = SimulatedBiomass.size1();
        int numCols = SimulatedBiomass.size2();
        for (int col=0; col<numCols; ++col) {
            stream << "," << SpeciesList[col];
        }
        stream << "\n";
        for (int row=0; row<numRows; ++row) {
            stream << QString::number(row+InitialYear);
            for (int col=0; col<numCols; ++col) {
                value = QString::number(SimulatedBiomass(row,col),'f',6);
                stream << "," << value;
            }
            stream << "\n";
        }
        file.close();
    }


std::cout << "Model: " << formCheck.toStdString() << std::endl;
    return retv;
}

bool
nmfSimulatedData::addError(double& value, const int& errorPct)
{
    double factor = nmfUtils::getRandomNumber(0,-1,1);
    double error = value * (errorPct/100.0);

    value += factor*error;
    value = (value < 0) ? 0 : value;

    return true;
}

bool
nmfSimulatedData::getCompetitionSystemCarryingCapacity(
        const bool isAggProd,
        const int& NumGuilds,
        std::map<int,std::vector<int> >& GuildSpecies,
        const std::vector<double>& SpeciesK,
        double& SystemCarryingCapacity)
{
    SystemCarryingCapacity = 0;
    if (isAggProd) {
        for (int i=0; i<NumGuilds; ++i) {
            SystemCarryingCapacity += SpeciesK[i];
        }
    } else {
        for (int i=0; i<NumGuilds; ++i) {
            for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                SystemCarryingCapacity += SpeciesK[GuildSpecies[i][j]];
            }
        }
    }
    return true;
}

bool
nmfSimulatedData::getCompetitionGuildCarryingCapacity(
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
    return true;
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




