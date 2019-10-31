#pragma once

#include "nmfUtils.h"
#include "nmfConstantsMSSPM.h"
#include "nmfGrowthForm.h"
#include "nmfHarvestForm.h"
#include "nmfCompetitionForm.h"
#include "nmfPredationForm.h"

#include <QFile>
#include <QString>
#include <exception>

#include <nlopt.hpp>

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
    nlopt::opt                             m_Optimizer;
    std::vector<double>                    m_InitialCarryingCapacities;
    std::vector<double>                    m_EstCatchability;
    std::vector<double>                    m_EstExponent;
    boost::numeric::ublas::matrix<double>  m_EstAlpha;
    boost::numeric::ublas::matrix<double>  m_EstBetaSpecies;
    boost::numeric::ublas::matrix<double>  m_EstBetaGuilds;
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

signals:
    void RunCompleted(QString outputStr);
    void SubRunCompleted(int RunNum, int SubRun, int NumSubRuns);
    void UpdateProgressData(int NumSpecies, int NumParams, QString elapsedTime);

public:
    NLopt_Estimator();
   ~NLopt_Estimator();

    static int          m_nloptIters;
    static int          m_counter;
    static int          m_RunNum;
    std::vector<double> m_EstGrowthRates;
    std::vector<double> m_EstCarryingCapacities;

    static double calculateSumOfSquares(const boost::numeric::ublas::matrix<double>& EstBiomass,
                                        const boost::numeric::ublas::matrix<double>& ObsBiomass);
    static double calculateModelEfficiency(const boost::numeric::ublas::matrix<double>& EstBiomass,
                                           const boost::numeric::ublas::matrix<double>& ObsBiomass);
    void estimateParameters(Data_Struct &nloptStruct,
                            int RunNum);
    static double extractParameters(const Data_Struct& NLoptDataStruct,
                                    const double *EstParameters,
                                    std::vector<double>& growthRate,
                                    std::vector<double>& carryingCapacity,
                                    std::vector<double>& catchabilityRate,
                                    boost::numeric::ublas::matrix<double>& competitionAlpha,
                                    boost::numeric::ublas::matrix<double>& competitionBetaSpecies,
                                    boost::numeric::ublas::matrix<double>& competitionBetaGuilds,
                                    boost::numeric::ublas::matrix<double>& predation,
                                    boost::numeric::ublas::matrix<double>& handling,
                                    std::vector<double>& exponent);
    void getEstGrowthRates(std::vector<double>& GrowthRates);
    void getEstCarryingCapacities(std::vector<double>& CarryingCapacities);
    void getEstCatchability(std::vector<double> &theEstCatchability);
    void getEstCompetitionAlpha(boost::numeric::ublas::matrix<double> &theEstInteraction);
    void getEstCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &theEstCompSpecies);
    void getEstCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &theEstCompGuilds);
    void getEstPredation(boost::numeric::ublas::matrix<double> &theEstPredation);
    void getEstHandling(boost::numeric::ublas::matrix<double> &theEstHandling);
    void getEstExponent(std::vector<double> &theEstExponent);
    static double objectiveFunction(unsigned n,
                                    const double *x,
                                    double *gradient,
                                    void *functionData);
    static void rescaleMean(const boost::numeric::ublas::matrix<double> &matrix,
                            boost::numeric::ublas::matrix<double> &rescaledMatrix);
    static void rescaleMinMax(const boost::numeric::ublas::matrix<double> &matrix,
                              boost::numeric::ublas::matrix<double> &rescaledMatrix);

    static void writeCurrentLoopFile(std::string &MSSPMName,
                                     int         &NumGens,
                                     double      &BestFitness,
                                     std::string &ObjectiveCriterion,
                                     int         &NumGensSinceBestFit);

public slots:
    void callback_StopTheOptimizer();

};


