/**
 * @file Bees_Estimator.h
 * @brief Class definition for the Bees_Estimator API
 *
 * This file contains the class definition for the Bees_Estimator API. This
 * API acts as an interface class between the Main calling program and the lower-level
 * Bess Algorithm implementation.
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

#pragma once

#include "boost/random.hpp"

#include <cmath>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <memory>
#include "time.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"

#include "BeesAlgorithm.h"
#include "BeesStats.h"

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <thread>

typedef boost::uniform_real<> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&,NumberDistribution> Generator;

/**
 * @brief This class acts as an interface class to the Bees algorithm implementation.
 */
class Bees_Estimator : public QObject
{

    Q_OBJECT

private:
    std::vector<double>                   m_InitialCarryingCapacities;
    double                                m_EstSystemCarryingCapacity;
    std::vector<double>                   m_EstInitBiomass;
    std::vector<double>                   m_EstGrowthRates;
    std::vector<double>                   m_EstGrowthRateCovariateCoeffs;
    std::vector<double>                   m_EstCarryingCapacities;
    std::vector<double>                   m_EstCarryingCapacityCovariateCoeffs;
    std::vector<double>                   m_EstCatchabilityCovariateCoeffs;
//  std::vector<double>                   m_EstExploitationRates;
    std::vector<double>                   m_EstCatchability;
    std::vector<double>                   m_EstExponent;
    std::vector<double>                   m_EstSurveyQ;
    boost::numeric::ublas::matrix<double> m_EstAlpha;
    boost::numeric::ublas::matrix<double> m_EstBetaSpecies;
    boost::numeric::ublas::matrix<double> m_EstBetaGuilds;
    boost::numeric::ublas::matrix<double> m_EstBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> m_EstPredation;
    boost::numeric::ublas::matrix<double> m_EstHandling;

    void createOutputStr(const int&         numEstParameters,
                         const int&         numTotalParameters,
                         const int&         numSubRuns,
                         const double&      bestFitness,
                         const double&      fitnessStdDev,
                         const nmfStructsQt::ModelDataStruct& beeStruct,
                         std::string&       bestFitnessStr);
    std::string convertValues1DToOutputStr(const std::string& label,
                                           const std::vector<double>& valuesVec,
                                           const bool& includeTotal);
    std::string convertValues2DToOutputStr(const std::string& label,
                                           const boost::numeric::ublas::matrix<double>& valuesMatrix);
    void outputProgressData(std::string msg);
    void printBee(std::string msg,
                  double &fitness,
                  std::vector<double> &parameters);
    void stopRun(const std::string &elapsedTimeStr,
                 const std::string &fitnessStr);
    bool wasStoppedByUser();

signals:
    /**
     * @brief Signal emitted at the end of a multi-run set of runs
     * @param multiRunSpeciesFilename : name of Multi-Run Species File
     * @param multiRunModelFilename : name of Multi-Run Model File
     */
    void AllSubRunsCompleted(std::string multiRunSpeciesFilename,
                             std::string multiRunModelFilename);
    /**
     * @brief Signal emitted to update the calling program of an error in the Bees algorithm
     * @param errorMsg : string value the error message from the Bees algorithm
     */
    void ErrorFound(std::string errorMsg);
    /**
     * @brief Signal emitted at the start of a multi-run set of runs
     * @param multiRunModelFilename : name of Multi-Run Model File
     * @param totalIndividualRuns : total number of runs
     */
    void InitializeSubRuns(std::string multiRunModelFilename,
                           int totalIndividualRuns);
    /**
     * @brief Signal emitted with the Run has completed
     * @param bestFitness : string value representing the best fitness value
     * @param showDiagnosticsChart : boolean signfying whether the
     * diagnostic 3d chart should be displayed after the run completes
     */
    void RunCompleted(std::string bestFitness,
                      bool showDiagnosticsChart);
    /**
     * @brief Signal emitted when NLopt Estimation sub run of a multi run has completed
     * @param run : current run
     * @param numRuns : total number of runs in multi run
     * @param EstimationAlgorithm : name of estimation algorithm
     * @param MinimizerAlgorithm : name of the minimizer algorithm
     * @param ObjectiveCriterion : name of the objective criterion
     * @param ScalingAlgorithm : name of the scaling algorithm
     * @param multiRunModelFilename : name of file that will contain the model data from all of the multi runs
     * @param fitness : fitness value of current run
     */
    void SubRunCompleted(int run,
                         int numRuns,
                         std::string EstimationAlgorithm,
                         std::string MinimizerAlgorithm,
                         std::string ObjectiveCriterion,
                         std::string ScalingAlgorithm,
                         std::string multiRunModelFilename,
                         double fitness);
    /**
     * @brief Signal emitted when a repetition run is completed
     * @param RunNum : the number of the parent run
     * @param SubRun : the number of the child sub run
     * @param NumSubRuns : number of sub runs in the parent run
     */
    void RepetitionRunCompleted(int RunNum, int SubRun, int NumSubRuns);

public:
    /**
     * @brief Class constructor for the Bees Estimation Algorithm interface class
     */
    Bees_Estimator();
   ~Bees_Estimator();

    /**
     * @brief The main routine that runs the Bees Estimation algorithm
     * @param BeeStruct : data structure containing parameters needed for the Bees algorithm
     * @param RunNumber : the run number
     * @param MultiRunLines : contents of multi-run run file
     * @param TotalIndividualRuns : total of all multi-run runs
     */
    void estimateParameters(nmfStructsQt::ModelDataStruct &BeeStruct,
                            int& RunNumber,
                            std::vector<QString>& MultiRunLines,
                            int& TotalIndividualRuns);
    /**
     * @brief Gets the estimated carrying capacity values per species
     * @param EstCarryingCapacity : vector of carrying capacities per species
     */
    void getEstCarryingCapacities(std::vector<double> &EstCarryingCapacity);
    /**
     * @brief Gets the estimated carrying capacity covariate coefficient values per species
     * @param EstCarryingCapacityCovariateCoeffs : vector of carrying capacities covariate coefficients per species
     */
    void getEstCarryingCapacityCovariateCoeffs(std::vector<double> &EstCarryingCapacityCovariateCoeffs);
    /**
     * @brief Gets the estimated catchability values per species
     * @param EstCatchability : vector of catchability values per species
     */
    void getEstCatchability(std::vector<double> &EstCatchability);
    /**
     * @brief Gets the estimated catchability covariate coefficient values per species
     * @param EstCatchability : vector of catchability covariate coefficient values per species
     */
    void getEstCatchabilityCovariateCoeffs(std::vector<double> &estCatchabilityCovariateCoeffs);
    /**
     * @brief Gets the estimated alpha interaction values per species
     * @param EstInteraction : vector of alpha interaction values per species
     */
    void getEstCompetitionAlpha(boost::numeric::ublas::matrix<double> &EstInteraction);
    /**
     * @brief Gets the estimated beta competition values per species-guild
     * @param EstCompGuilds : vector of beta competition values per species-guild
     */
    void getEstCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &EstCompGuilds);
    /**
     * @brief Gets the estimated beta competition values per guild-guild
     * @param EstCompGuildsGuilds : vector of beta competition values per guild-guild
     */
    void getEstCompetitionBetaGuildsGuilds(boost::numeric::ublas::matrix<double> &EstCompGuildsGuilds);
    /**
     * @brief Gets the estimated beta competition values per species
     * @param EstCompSpecies : vector of beta competition values per species
     */
    void getEstCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &EstCompSpecies);
    /**
     * @brief Gets the estimated exponent values per species
     * @param EstExponent : vector of exponent values per species
     */
    void getEstExponent(std::vector<double> &EstExponent);
    /**
     * @brief Gets the estimated growth rate covariate coefficient values per species
     * @param EstGrowthRateCovariateCoeffs : vector of growth rate covariate coefficient values per species
     */
    void getEstGrowthRateCovariateCoeffs(std::vector<double> &EstGrowthRateCovariateCoeffs);
    /**
     * @brief Gets the estimated growth rate values per species
     * @param EstGrowthRates : vector of growth rate values per species
     */
    void getEstGrowthRates(std::vector<double> &EstGrowthRates);
    /**
     * @brief Gets the estimated handling values per species
     * @param EstHandling : vector of handling values per species
     */
    void getEstHandling(boost::numeric::ublas::matrix<double> &EstHandling);
    /**
     * @brief Gets the estimated initial biomass values per species
     * @param EstInitBiomass : vector of initial biomass values per species
     */
    void getEstInitBiomass(std::vector<double> &EstInitBiomass);
    /**
     * @brief Gets the estimated predation values per species
     * @param EstPredation : vector of predation values per species
     */
    void getEstPredation(boost::numeric::ublas::matrix<double> &EstPredation);
    /**
     * @brief Gets the estimated SurveyQ values per species
     * @param EstSurveyQ : vector of SurveyQ values per species
     */
    void getEstSurveyQ(std::vector<double> &EstSurveyQ);
};


