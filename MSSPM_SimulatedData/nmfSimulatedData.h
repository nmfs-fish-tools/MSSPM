/**
 * @file nmfSimulatedData.h
 * @brief Class definition for the Simulated Data functionality
 *
 * This file contains the class definition for the Simulated Biomass class. It
 * allows the user to generated simulated biomass which is then used to test
 * MSSPM's estimation algorithms.
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 */
#ifndef nmfSimulatedData_H
#define nmfSimulatedData_H

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "nmfCompetitionForm.h"
#include "nmfConstants.h"
#include "nmfDatabase.h"
#include "nmfGrowthForm.h"
#include "nmfHarvestForm.h"
#include "nmfLogger.h"
#include "nmfPredationForm.h"
#include "nmfUtils.h"
#include "nmfStructsQt.h"

/**
 * @brief Creates simulated biomass
 *
 * This class is responsible for the creation of the simulated biomass
 * data used to test MSSPM's estimations algorithms.
 *
 */
class nmfSimulatedData
{
    std::string  m_ProjectName;
    std::string  m_ModelName;
    nmfDatabase* m_Database;
    nmfLogger*   m_Logger;
    int          m_FixedSeed;
    nmfStructsQt::ModelDataStruct m_DataStruct;

    /**
     * @brief Adds a lognormal (i.e., Gaussian) error to the passed value
     * @param cv : coefficient of variation percentage
     * @param value : the value which receives the lognormal error
     */
    void addLognormalError(const double& cv,
                           double& value);
    /**
     * @brief Adds a uniform percent error to the passed value
     * @param errorPct : percent error to add to value
     * @param value : the value which receives the percent error
     */
    void addUniformError(const double& errorPct,
                         double& value);
    /**
     * @brief Sends back the system carrying capacity
     * @param isAggProd : true if is an AggProd, otherwise false
     * @param NumGuilds : number of guilds
     * @param SpeciesK : vector of species carrying capacities
     * @param GuildSpecies : map of species number per guild
     * @param SystemCarryingCapacity : the system carrying capacity
     */
    void getSystemCarryingCapacity(
            const bool isAggProd,
            const int& NumGuilds,
            const std::vector<double>& SpeciesK,
            std::map<int,std::vector<int> >& GuildSpecies,
            double& SystemCarryingCapacity);
    /**
     * @brief Sends back the guild carrying capacity for the passed guild number
     * @param isAggProd : true if is an AggProd, otherwise false
     * @param Species : current species
     * @param SpeciesK : vector of species carrying capacities
     * @param GuildNum : current guild number
     * @param GuildSpecies : map of species number per guild
     * @param GuildCarryingCapacity : value of guild carrying capacity
     */
    void getGuildCarryingCapacity(
            const bool isAggProd,
            const int& Species,
            const std::vector<double>& SpeciesK,
            const std::vector<int>& GuildNum,
            std::map<int,std::vector<int> >& GuildSpecies,
            double& GuildCarryingCapacity);
/*
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
*/
public:
    /**
     * @brief Class constructor
     * @param database : pointer to database library
     * @param logger : point to logger used to log any error messages
     * @param projectName : name of current project
     * @param modelName : name of current model
     * @param dataStruct : model data (contains the covariate data)
     */
    nmfSimulatedData(
            nmfDatabase* database,
            nmfLogger*   logger,
            std::string& projectName,
            std::string& modelName,
            nmfStructsQt::ModelDataStruct& dataStruct);
    ~nmfSimulatedData() {}

    /**
     * @brief Creates a simulated Biomass time series with the current model settings
     * @param errorType : type of error: None, Normal, Uniform
     * @param errorValue : error value to be used to calculate noise
     * @param filename : filename where to write out simulated biomass
     * @param errorMsg : error message to pass back to the calling routine
     * @return true if data were read correctly, false otherwise
     */
    bool createSimulatedBiomass(
            const QString& errorType,
            const double& errorValue,
            QString& filename,
            QString& errorMsg);
};

#endif // nmfSimulatedData_H
