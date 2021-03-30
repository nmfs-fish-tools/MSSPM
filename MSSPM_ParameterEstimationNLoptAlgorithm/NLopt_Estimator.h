/**
 * @file NLopt_Estimator.h
 * @brief Class definition for the NLopt_Estimator API
 *
 * This file contains the class definition for the NLopt_Estimator API. This
 * API acts as an interface class between the Main calling program and the lower-level
 * NLopt 3rd party library.
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

#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfUtilsStatistics.h"
#include "nmfConstantsMSSPM.h"
#include "nmfGrowthForm.h"
#include "nmfHarvestForm.h"
#include "nmfCompetitionForm.h"
#include "nmfPredationForm.h"

#include <QObject>
#include <QString>

#include <exception>
#include <nlopt.hpp>
#include <random>


/**
 * @brief This class acts as an interface class to the NLopt library.
 *
 * NLopt is an open-source library for non-linear optimization.
 *
 */
class  NLopt_Estimator : public QObject
{

    Q_OBJECT

private:
    static nlopt::opt                             m_Optimizer;
    std::vector<double>                    m_InitialCarryingCapacities;
    std::vector<double>                    m_EstCatchability;
    std::vector<double>                    m_EstExponent;
    boost::numeric::ublas::matrix<double>  m_EstAlpha;
    boost::numeric::ublas::matrix<double>  m_EstBetaSpecies;
    boost::numeric::ublas::matrix<double>  m_EstBetaGuilds;
    boost::numeric::ublas::matrix<double>  m_EstBetaGuildsGuilds;
    std::vector<double>                    m_EstCarryingCapacities;
    std::vector<double>                    m_EstInitBiomass;
    std::vector<double>                    m_EstGrowthRates;
    std::vector<double>                    m_EstInitialBiomass;
    boost::numeric::ublas::matrix<double>  m_EstPredation;
    boost::numeric::ublas::matrix<double>  m_EstHandling;
    std::map<std::string,nlopt::algorithm> m_MinimizerToEnum;
    std::vector<double>                    m_Parameters;


    std::string returnCode(int result);
    void stopRun(const std::string &elapsedTimeStr,
                 const std::string &fitnessStr);
    void createOutputStr(const int&         numTotalParameters,
                         const int&         numEstParameters,
                         const int&         numSubRuns,
                         const double&      bestFitness,
                         const double&      fitnessStdDev,
                         const Data_Struct& beeStruct,
                         std::string&       bestFitnessStr);
    std::string convertValues1DToOutputStr(const std::string& label,
                                    const std::vector<double> &Values,
                                    const bool& includeTotal);
    std::string convertValues2DToOutputStr(const std::string& label,
                                    const boost::numeric::ublas::matrix<double> &matrix);
    static void incrementObjectiveFunctionCounter(std::string MSSPMName,
                                           double fitness,
                                           Data_Struct NLoptDataStruct);
//    double  dnorm4(double x, double mu, double sigma, int give_log);

    void loadInitBiomassParameterRanges(
            std::vector<std::pair<double,double> >& parameterRanges,
            const Data_Struct& dataStruct);
    void setStoppingCriteria(Data_Struct&  NLoptStruct);
    void setObjectiveFunction(Data_Struct& NLoptStruct,
                              std::string& MaxOrMin);
    void setParameterBounds(Data_Struct& NLoptStruct,
                            std::vector<std::pair<double,double> >& ParameterRanges,
                            const int& NumEstParameters);
    void reloadNLoptStruct(
            Data_Struct& NLoptStruct,
            const QString& MultiRunLine);


    static double myNaturalLog(double value);
    static double myExp(double value);

signals:
    /**
     * @brief Signal emitted at the end of a multi-run set of runs
     * @param multiRunSpeciesFilename : name of Multi-Run Species File
     * @param multiRunModelFilename : name of Multi-Run Model File
     */
    void AllSubRunsCompleted(std::string multiRunSpeciesFilename,
                             std::string multiRunModelFilename);
    /**
     * @brief Signal emitted at the start of a multi-run set of runs
     * @param multiRunModelFilename : name of Multi-Run Model File
     * @param totalIndividualRuns : total number of runs
     */
    void InitializeSubRuns(std::string multiRunModelFilename,
                           int totalIndividualRuns);
    /**
     * @brief Signal emitted to query user for the Multi Run Species and Model file names
     */
    void QueryUserForMultiRunFilenames();
    /**
     * @brief Signal emitted with NLopt Estimation run has complete
     * @param bestFitness : string representing the best fitness value
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


public:
    /**
     * @brief Class constructor for the NLopt Estimation interface
     */
    NLopt_Estimator();
   ~NLopt_Estimator();

//    /**
//     * @brief Counts the number of run iterations
//     */
//    static int m_NLoptIters;
    /**
     * @brief Counts the number of function evaluations
     */
    static int m_NLoptFcnEvals;

    static int m_NumObjFcnCalls;
//    /**
//     * @brief Counts the number of run iterations by the thousands
//     */
//    static int m_Counter;
    /**
     * @brief Keeps track of the run number
     */
    static int m_RunNum;
    /**
     * @brief The main routine that runs the NLopt Optimizer
     * @param NLoptDataStruct : structure containing all of the parameters needed by NLopt
     * @param RunNum : the number of the run
     * @param MultiRunLines : the contents of the multi-run run file
     * @param TotalIndividualRuns : total of all of the multi-run runs
     */
    void estimateParameters(
            Data_Struct& NLoptDataStruct,
            int&          RunNumber,
            std::vector<QString>& MultiRunLines,
            int& TotalIndividualRuns);
    /**
     * @brief Extracts the estimated parameters from the NLopt Optimizer run
     * @param NLoptDataStruct : input parameters to the NLopt Optimizer
     * @param EstParameters : output estimated parameters from the NLopt Optimizer
     * @param InitBiomass : estimated initial biomass parameters
     * @param GrowthRate : estimated growth rate parameters
     * @param CarryingCapacity : estimated carrying capacity parameters
     * @param CatchabilityRate : estimated catchability parameters
     * @param CompetitionAlpha : estimated competition alpha parameters
     * @param CompetitionBetaSpecies : estimated food competition beta parameters per Species
     * @param CompetitionBetaGuilds : estimated food competition beta parameters per Species-Guild
     * @param CompetitionBetaGuildsGuilds : estimated food competition beta parameters per Guild-Guild
     * @param Predation : estimated predation parameters
     * @param Handling : estimated handling parameters
     * @param Exponent : estimated handling parameters
     */
    static void extractParameters(
            const Data_Struct&                     NLoptDataStruct,
            const double*                          EstParameters,
            std::vector<double>&                   InitBiomass,
            std::vector<double>&                   GrowthRate,
            std::vector<double>&                   CarryingCapacity,
            std::vector<double>&                   CatchabilityRate,
            boost::numeric::ublas::matrix<double>& CompetitionAlpha,
            boost::numeric::ublas::matrix<double>& CompetitionBetaSpecies,
            boost::numeric::ublas::matrix<double>& CompetitionBetaGuilds,
            boost::numeric::ublas::matrix<double>& CompetitionBetaGuildsGuilds,
            boost::numeric::ublas::matrix<double>& Predation,
            boost::numeric::ublas::matrix<double>& Handling,
            std::vector<double>&                   Exponent);
    /**
     * @brief Get the estimated carrying capacity values
     * @param EstCarryingCapacities : the estimated carrying capacity values to return
     */
    void getEstCarryingCapacities(
            std::vector<double>& EstCarryingCapacities);
    /**
     * @brief Get the estimated catchability values
     * @param EstCatchability : the estimated catchability values to return
     */
    void getEstCatchability(
            std::vector<double>& EstCatchability);
    /**
     * @brief Get the estimated food competition alpha values
     * @param EstInteraction : the estimated food competition alpha values to return
     */
    void getEstCompetitionAlpha(
            boost::numeric::ublas::matrix<double>& EstInteraction);
    /**
     * @brief Get the estimated food competition beta values per Species-Guild
     * @param EstCompGuilds : the estimated food competition beta values per Species-Guild to return
     */
    void getEstCompetitionBetaGuilds(
            boost::numeric::ublas::matrix<double>& EstCompGuilds);
    /**
     * @brief Get the estimated food competition beta values per Guild-Guild
     * @param EstCompGuilds : the estimated food competition beta values per Guild-Guild to return
     */
    void getEstCompetitionBetaGuildsGuilds(
            boost::numeric::ublas::matrix<double>& EstCompGuildsGuilds);
    /**
     * @brief Get the estimated food competition beta values per Species
     * @param EstCompSpecies : the estimated food competition beta values per Species to return
     */
    void getEstCompetitionBetaSpecies(
            boost::numeric::ublas::matrix<double>& EstCompSpecies);
    /**
     * @brief Get the estimated exponent values
     * @param EstExponent : the estimated exponent values to return
     */
    void getEstExponent(
            std::vector<double>& EstExponent);
    /**
     * @brief Get the estimated growth rate values
     * @param GrowthRates : the estimated growth rate values to return
     */
    void getEstGrowthRates(
            std::vector<double>& GrowthRates);
    /**
     * @brief Get the estimated handling values
     * @param EstHandling : the estimated handling values to return
     */
    void getEstHandling(
            boost::numeric::ublas::matrix<double>& EstHandling);
    /**
     * @brief Get the estimated initial biomass values
     * @param InitBiomass : the estimated initial biomass values to return
     */
    void getEstInitBiomass(
            std::vector<double>& InitBiomass);
    /**
     * @brief Get the estimated predation values
     * @param EstPredation : the estimated predation values to return
     */
    void getEstPredation(
            boost::numeric::ublas::matrix<double>& EstPredation);
    /**
     * @brief Gets the NLopt version
     * @return Returns a QString which comprises the major, minor, and bugfix version of NLopt
     */
    QString getVersion();
    /**
     * @brief Calculates the objective function fitness value
     * @param n : unused (needed by NLopt library)
     * @param EstParameters : estimated parameter values
     * @param Gradient : unused (needed by NLopt library)
     * @param FunctionData : input parameter estimation data
     * @return
     */
    static double objectiveFunction(
            unsigned      n,
            const double* EstParameters,
            double*       Gradient,
            void*         FunctionData);
    /**
     * @brief Rescales each column of the input matrix with (x - ave)/(max-min)
     * @param Matrix : input matrix to be rescaled
     * @param RescaledMatrix : the output rescaled matrix
     */
    static void rescaleMean(
            const boost::numeric::ublas::matrix<double>& Matrix,
            boost::numeric::ublas::matrix<double>&       RescaledMatrix);
    /**
     * @brief Rescales each column of the input matrix with (x - min)/(max-min)
     * @param Matrix : input matrix to be rescaled
     * @param RescaledMatrix : the output rescaled matrix
     */
    static void rescaleMinMax(
            const boost::numeric::ublas::matrix<double>& Matrix,
            boost::numeric::ublas::matrix<double>&       RescaledMatrix);
    /**
     * @brief Updates the output chart data file with Optimization status. Another
     * process reads this file and updates the progress chart accordingly.
     * @param MSSPMName : name of the run
     * @param NumGens : iteration number counted by thousand
     * @param BestFitness : fitness value
     * @param ObjectiveCriterion : name of objective criterion (Least Squares, Maximum Likelihood, or Model Efficiency)
     * @param NumGensSinceBestFit : unused
     */
    static void writeCurrentLoopFile(
            std::string& MSSPMName,
            int&         NumGens,
            double&      BestFitness,
            std::string& ObjectiveCriterion,
            int&         NumGensSinceBestFit);

public slots:
    /**
     * @brief Callback invoked when the user stops the Estimation run
     */
    void callback_StopTheOptimizer();

};


