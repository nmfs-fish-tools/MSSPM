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

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QThread>

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
    int                                    m_Seed;
    int                                    m_FixedSeed;
    static nlopt::opt                      m_Optimizer;
    std::vector<double>                    m_InitialCarryingCapacities;
    std::vector<double>                    m_EstCatchability;
    std::vector<double>                    m_EstCatchabilityCovariateCoeffs;
    std::vector<double>                    m_EstExponent;
    std::vector<double>                    m_EstSurveyQ;
    std::vector<double>                    m_EstSurveyQCovariateCoeffs;
    boost::numeric::ublas::matrix<double>  m_EstAlpha;
    boost::numeric::ublas::matrix<double>  m_EstBetaSpecies;
    boost::numeric::ublas::matrix<double>  m_EstBetaGuilds;
    boost::numeric::ublas::matrix<double>  m_EstBetaGuildsGuilds;
    std::vector<double>                    m_EstCarryingCapacities;
    std::vector<double>                    m_EstCarryingCapacityCovariateCoeffs;
    std::vector<double>                    m_EstInitBiomass;
    std::vector<double>                    m_EstGrowthRates;
    std::vector<double>                    m_EstGrowthRateShape;
    std::vector<double>                    m_EstGrowthRateCovariateCoeffs;
    std::vector<double>                    m_EstInitialBiomass;
    std::vector<double>                    m_EstBMSY;
    std::vector<double>                    m_EstMSY;
    std::vector<double>                    m_EstFMSY;
    boost::numeric::ublas::matrix<double>  m_EstPredation;
    boost::numeric::ublas::matrix<double>  m_EstHandling;
    std::map<std::string,nlopt::algorithm> m_MinimizerToEnum;
    std::vector<double>                    m_Parameters;

    std::string completedMsg(
            std::string code);
    void checkAndApplyLogScale(
            const bool& useLogScale,
            std::vector<double>& parameterInitialValues,
            std::vector<std::pair<double,double> >& parameterRanges);
    static void checkAndUnapplyLogScale(
            const bool& usedLogScale,
            std::vector<double>& initBiomass,
            std::vector<double>& growthRate,
            std::vector<double>& growthRateShape,
            std::vector<double>& growthRateCovariateCoeffs,
            std::vector<double>& carryingCapacity,
            std::vector<double>& carryingCapacityCovariateCoeffs,
            std::vector<double>& catchability,
            std::vector<double>& catchabilityCovariateCoeffs,
            boost::numeric::ublas::matrix<double>& competitionAlpha,
            boost::numeric::ublas::matrix<double>& competitionBetaSpecies,
            boost::numeric::ublas::matrix<double>& competitionBetaGuilds,
            boost::numeric::ublas::matrix<double>& competitionBetaGuildsGuilds,
            boost::numeric::ublas::matrix<double>& predation,
            boost::numeric::ublas::matrix<double>& handling,
            std::vector<double>& exponent,
            std::vector<double>& surveyQ,
            std::vector<double>& surveyQCovariateCoeffs);
    void checkEstimatedValuesForBoundaryConditions(const nmfStructsQt::ModelDataStruct& NLoptDataStruct,
                                                   const int& numEstParameters,
                                                   std::vector<double>& parameterInitialValues,
                                                   std::vector<std::pair<double,double> >& parameterRanges,
                                                   std::vector<double>& initBiomass,
                                                   std::vector<double>& growthRate,
                                                   std::vector<double>& growthRateShape,
                                                   std::vector<double>& growthRateCovariateCoeffs,
                                                   std::vector<double>& carryingCapacity,
                                                   std::vector<double>& carryingCapacityCovariateCoeffs,
                                                   std::vector<double>& catchability,
                                                   std::vector<double>& catchabilityCovariateCoeffs,
                                                   boost::numeric::ublas::matrix<double>& competitionAlpha,
                                                   boost::numeric::ublas::matrix<double>& competitionBetaSpecies,
                                                   boost::numeric::ublas::matrix<double>& competitionBetaGuilds,
                                                   boost::numeric::ublas::matrix<double>& competitionBetaGuildsGuilds,
                                                   boost::numeric::ublas::matrix<double>& predation,
                                                   boost::numeric::ublas::matrix<double>& handling,
                                                   std::vector<double>& exponent,
                                                   std::vector<double>& surveyQ,
                                                   std::vector<double>& surveyQCovariateCoeffs,
                                                   std::vector<double>& bmsy,
                                                   std::vector<double>& msy,
                                                   std::vector<double>& fmsy);
    void createOutputStr(
            const int&         numEstParameters,
            const int&         numTotalParameters,
            const int&         numSubRuns,
//          const double&      bestFitness,
//          const double&      fitnessStdDev,
            const nmfStructsQt::ModelDataStruct& beeStruct,
            std::string&       bestFitnessStr);
    void checkParameterRange(
            const std::string& species,
            const double& parameterInitialValue,
            const std::pair<double,double>& parameterRange,
            const double& parameterValue,
            const std::string parameterName,
            const bool& useScientific,
            std::vector<std::string>& boundaryWarnings);
    static void exponentiate(std::vector<double>& vec);
    static void exponentiate(boost::numeric::ublas::matrix<double>& mat);
    void loadInitBiomassParameterRanges(
            std::vector<double>& parameterInitialValues,
            std::vector<std::pair<double,double> >& parameterRanges,
            const nmfStructsQt::ModelDataStruct& dataStruct);
    void loadSurveyQParameterRanges(
            std::vector<double>& parameterInitialValues,
            std::vector<std::pair<double,double> >& parameterRanges,
            const nmfStructsQt::ModelDataStruct& dataStruct);
    void reloadNLoptStruct(
            nmfStructsQt::ModelDataStruct& NLoptStruct,
            const QString& MultiRunLine);
    std::string returnCodeStr(int result);
    void setObjectiveFunction(
            nmfStructsQt::ModelDataStruct& NLoptStruct,
            std::string& MaxOrMin);
    void setParameterBounds(
            int overallRun,
            nmfStructsQt::ModelDataStruct& NLoptStruct,
            std::vector<double>& ParameterInitialValues,
            std::vector<std::pair<double,double> >& ParameterRanges,
            const int& NumEstParameters);
    void setSeed(
            const bool& isSetToDeterministic,
            const bool& useFixedSeed,
            const bool& useUserFixedSeed,
            const int& userFixedSeedVal,
            const bool& incrementFixedSeed);
    void setStoppingCriteria(nmfStructsQt::ModelDataStruct&  NLoptStruct);
    static void incrementObjectiveFunctionCounter(
            std::string MSSPMName,
            double fitness,
            nmfStructsQt::ModelDataStruct NLoptDataStruct);
    static double myExp(double value);
    static double myNaturalLog(double value);

signals:
    /**
     * @brief Signal emitted at the end of a multi-run set of runs
     * @param elapsedTime: elapsed time of the current run or multi-run
     * @param summaryMsg : message to be displayed on the Run Summary page
     * @param msg : message for output string denoting if all sub-runs converged
     */
    void AllSubRunsCompleted(std::string elapsedTime,
                             std::string summaryMsg,
                             QString msg);
    /**
     * @brief Signal emitted when a Mohn's Rho multi run has completed
     */
    void AMohnsRhoMultiRunCompleted();
    /**
     * @brief Signal emitted at the start of a multi-run set of runs
     * @param multiRunModelFilename : name of Multi-Run Model File
     * @param totalIndividualRuns : total number of runs
     */
    void InitializeSubRuns(std::string multiRunModelFilename,
                           int totalIndividualRuns);
    /**
     * @brief Signal emitted when the user wants to log a group of messages
     * @param msgVec : vector of messages to be logged
     */
    void LogMsg(std::vector<std::string> msgVec);
    /**
     * @brief Signal emitted to reset the GUI after NLopt errored out of a run.
     */
    void NLoptFailureStopRunsAndReset();
    /**
     * @brief Signal emitted with NLopt Estimation run has complete
     * @param bestFitness : string representing the best fitness value
     * @param convergence : string indicating if the model run converged or not
     * @param showDiagnosticsChart : boolean signfying whether the
     * diagnostic 3d chart should be displayed after the run completes
     */
    void RunCompleted(std::string bestFitness,
                      std::string convergence,
                      bool showDiagnosticsChart);
    /**
     * @brief Signal emitted notifying the main program the estimation termination message
     * @param returnCode : the numeric code signifying why the estimation stopped
     * @param returnCodeMsg : the message to display to the user as to why the estimation stopped
     */
    void RunCompletedMsg(int returnCode,
                         std::string returnCodeMsg);
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
     * @brief Updates models table with number of non-trivial parameter (i.e., a parameters whose min range value was different from its max range value)
     * @param NumNonTrivialParameters : number of non-trivial parameters
     */
    void UpdateNumberOfParameters(
            int NumNonTrivialParameters);


public:
    /**
     * @brief m_NLoptFcnEvals : counts the number of function evaluations
     */
    static int m_NLoptFcnEvals;
    /**
     * @brief m_NumObjFcnCalls : counts the number of objective function calls
     */
    static int m_NumObjFcnCalls;
    /**
     * @brief Keeps track of the run number
     */
    static int m_RunNum;


    /**
     * @brief Class constructor for the NLopt Estimation interface
     */
    NLopt_Estimator();
   ~NLopt_Estimator();

    /**
     * @brief The main routine that runs the NLopt Optimizer
     * @param NLoptDataStruct : structure containing all of the parameters needed by NLopt
     * @param RunNum : the number of the run
     * @param bools: pair holding isAMultiRun and isSetToDeterministic
     * @param MultiRunLines : the contents of the multi-run run file
     * @param TotalIndividualRuns : total of all of the multi-run runs
     */
    void estimateParameters(
            nmfStructsQt::ModelDataStruct& NLoptDataStruct,
            int& RunNumber,
            std::pair<bool,bool>& bools,
            std::vector<QString>& MultiRunLines,
            int& TotalIndividualRuns);
    /**
     * @brief Extracts the estimated parameters from the NLopt Optimizer run
     * @param NLoptDataStruct : input parameters to the NLopt Optimizer
     * @param EstParameters : output estimated parameters from the NLopt Optimizer
     * @param InitBiomass : estimated initial biomass parameters
     * @param GrowthRate : estimated growth rate parameters
     * @param GrowthRateShape : estimated growth rate shape parameters
     * @param GrowthRateCovariateCoeffs : estimated growth rate covariate coefficient parameters
     * @param CarryingCapacity : estimated carrying capacity parameters
     * @param CarryingCapacityCovariateCoeffs : estimated carrying capacity covariate coefficient parameters
     * @param Catchability : estimated catchability parameters
     * @param CatchabilityCovariateCoeffs : estimated catchability covariate coefficient parameters
     * @param CompetitionAlpha : estimated competition alpha parameters
     * @param CompetitionBetaSpecies : estimated food competition beta parameters per Species
     * @param CompetitionBetaGuilds : estimated food competition beta parameters per Species-Guild
     * @param CompetitionBetaGuildsGuilds : estimated food competition beta parameters per Guild-Guild
     * @param PredationRho : estimated predation rho parameters
     * @param PredationHandling : estimated predation handling coefficient parameters
     * @param PredationExponent : estimated predation exponent parameters
     * @param SurveyQ : estimated SurveyQ parameters
     * @param SurveyQCovariateCoeffs : estimated SurveyQ covariate coefficient parameters
     * @param BMSY : estimated Biomass MSY parameters
     * @param MSY  : estimated MSY parameters
     * @param FMSY : estimated Fishing MSY parameters
     */
    static void extractParameters(
            const nmfStructsQt::ModelDataStruct&   NLoptDataStruct,
            const double*                          EstParameters,
            std::vector<double>&                   InitBiomass,
            std::vector<double>&                   GrowthRate,
            std::vector<double>&                   GrowthRateShape,
            std::vector<double>&                   GrowthRateCovariateCoeffs,
            std::vector<double>&                   CarryingCapacity,
            std::vector<double>&                   CarryingCapacityCovariateCoeffs,
            std::vector<double>&                   Catchability,
            std::vector<double>&                   CatchabilityCovariateCoeffs,
            boost::numeric::ublas::matrix<double>& CompetitionAlpha,
            boost::numeric::ublas::matrix<double>& CompetitionBetaSpecies,
            boost::numeric::ublas::matrix<double>& CompetitionBetaGuilds,
            boost::numeric::ublas::matrix<double>& CompetitionBetaGuildsGuilds,
            boost::numeric::ublas::matrix<double>& PredationRho,
            boost::numeric::ublas::matrix<double>& PredationHandling,
            std::vector<double>&                   PredationExponent,
            std::vector<double>&                   SurveyQ,
            std::vector<double>&                   SurveyQCovariateCoeffs,
            std::vector<double>&                   BMSY,
            std::vector<double>&                   MSY,
            std::vector<double>&                   FMSY);
    /**
     * @brief Get the estimated carrying capacity values
     * @param EstCarryingCapacities : the estimated carrying capacity values to return
     */
    void getEstCarryingCapacities(
            std::vector<double>& EstCarryingCapacities);
    /**
     * @brief Get the estimated carrying capacity covariate coefficient values
     * @param EstCarryingCapacityCovariateCoeffs : the estimated carrying capacity covariate coefficient values to return
     */
    void getEstCarryingCapacityCovariateCoeffs(
            std::vector<double>& EstCarryingCapacityCovariateCoeffs);
    /**
     * @brief Get the estimated catchability values
     * @param EstCatchability : the estimated catchability values to return
     */
    void getEstCatchability(
            std::vector<double>& EstCatchability);
    /**
     * @brief Get the estimated catchability covariate coefficient values
     * @param EstCatchabilityCovariateCoeffs : the estimated catchability covariate coefficient values to return
     */
    void getEstCatchabilityCovariateCoeffs(
            std::vector<double>& EstCatchabilityCovariateCoeffs);
    /**
     * @brief Get the estimated Biomass MSY values
     * @param EstBMSY : the estimated biomass MSY values to return
     */
    void getEstBMSY(
            std::vector<double>& EstBMSY);
    /**
     * @brief Get the estimated MSY values
     * @param EstMSY : the estimated MSY values to return
     */
    void getEstMSY(
            std::vector<double>& EstMSY);
    /**
     * @brief Get the estimated Fishing MSY values
     * @param EstFMSY : the estimated fishing MSY values to return
     */
    void getEstFMSY(
            std::vector<double>& EstFMSY);
    /**
     * @brief Get the estimated Survey Q values
     * @param EstSurveyQ : the estimated Survey Q values to return
     */
    void getEstSurveyQ(
            std::vector<double>& EstSurveyQ);
    /**
     * @brief Get the estimated Survey Q covariate coefficient values
     * @param EstSurveyQCovariateCoeffs : the estimated Survey Q covariate coefficient values to return
     */
    void getEstSurveyQCovariateCoeffs(
            std::vector<double>& EstSurveyQCovariateCoeffs);
    /**
     * @brief Get the estimated food competition alpha values
     * @param estCompAlpha : the estimated food competition alpha values to return
     */
    void getEstCompetitionAlpha(
            boost::numeric::ublas::matrix<double>& estCompAlpha);
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
     * @param EstGrowthRates : the estimated growth rate values to return
     */
    void getEstGrowthRates(
            std::vector<double>& EstGrowthRates);
    /**
     * @brief Get the estimated growth rate covariate coefficients
     * @param EstGrowthRateCovariateCoeffs : the estimated growth rate covariate coefficients to return
     */
    void getEstGrowthRateCovariateCoeffs(
            std::vector<double>& EstGrowthRateCovariateCoeffs);
    /**
     * @brief Get the estimated growth rate shape parameters
     * @param EstGrowthRateShape : the estimated growth rate shape parameters to return
     */
    void getEstGrowthRateShape(
            std::vector<double>& EstGrowthRateShape);
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
     * @brief Initalizes the model form pointers
     * @param NLoptStruct : data structure containing necessary run information
     */
    void initialize(nmfStructsQt::ModelDataStruct &NLoptStruct);
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
     * @brief Sets any additional NLopt parameters as needed by the minimizer algorithm
     * @param NLoptStruct : data structure containing necessary run information
     */
    void setAdditionalParameters(const nmfStructsQt::ModelDataStruct& dataStruct);
    bool stoppedByUser();
    /**
     * @brief Stops the run gracefully and allows the display to use the estimated values as have been estimated so far
     */
    void stopGracefully();
    void stopRun(const std::string &elapsedTimeStr,
                 const std::string &fitnessStr);
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
            const std::string& MSSPMName,
            const int&         NumGens,
            const double&      BestFitness,
            const std::string& ObjectiveCriterion,
            const int&         NumGensSinceBestFit);

public slots:
    /**
     * @brief Callback invoked when the user indicates they wish to stop all Estimation runs (i.e., during a multi-run)
     */
    void callback_StopAllRuns();
};


