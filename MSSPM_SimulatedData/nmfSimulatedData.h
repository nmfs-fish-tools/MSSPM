#ifndef nmfSimulatedData_H
#define nmfSimulatedData_H

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "nmfConstants.h"
#include "nmfDatabase.h"
#include "nmfLogger.h"
#include "nmfUtils.h"
#include "nmfGrowthForm.h"
#include "nmfHarvestForm.h"
#include "nmfCompetitionForm.h"
#include "nmfPredationForm.h"

class nmfSimulatedData
{
    std::string  m_ProjectSettingsConfig;
    nmfDatabase* m_Database;
    nmfLogger*   m_Logger;

    bool getCompetitionSystemCarryingCapacity(
            const bool isAggProd,
            const int& NumGuilds,
            std::map<int,std::vector<int> >& GuildSpecies,
            const std::vector<double>& SpeciesK,
            double& SystemCarryingCapacity);
    bool getCompetitionGuildCarryingCapacity(
            const bool isAggProd,
            const int& Species,
            const std::vector<double>& SpeciesK,
            const std::vector<int>& GuildNum,
            std::map<int,std::vector<int> >& GuildSpecies,
            double& GuildCarryingCapacity);

    bool simulateCompetition(const std::string& CompetitionType,
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
                             double& competitionValue);
    bool simulateGrowth(const std::string& GrowthType,
                        const int& Year,
                        const int& Species,
                        const std::vector<double>& GrowthRate,
                        const std::vector<double>& SpeciesK,
                        const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                        double& growthValue);
    bool simulateHarvest(const std::string& HarvestType,
                         const int& Year,
                         const int& Species,
                         const boost::numeric::ublas::matrix<double>& Catch,
                         const boost::numeric::ublas::matrix<double>& Effort,
                         const boost::numeric::ublas::matrix<double>& Exploitation,
                         const std::vector<double>& Catchability,
                         const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                         double& harvestValue);
    bool simulatePredation(const std::string& PredationType,
                           const int& species,
                           const int& year,
                           const boost::numeric::ublas::matrix<double>& Rho,
                           const boost::numeric::ublas::matrix<double>& Handling,
                           const std::vector<double>& Exponent,
                           const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                           double& predationValue);

public:
    nmfSimulatedData(std::string  projectSettingsConfig,
                     nmfDatabase* database,
                     nmfLogger*   logger);
    ~nmfSimulatedData() {}

    /**
     * @brief Creates a simulated Biomass time series with the current model settings
     */
    bool createSimulatedBiomass(QString filename);
};

#endif // nmfSimulatedData_H
