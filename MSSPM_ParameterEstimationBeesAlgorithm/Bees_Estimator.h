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
#include "nmfRandom.h"

#include "BeesAlgorithm.h"
#include "BeesStats.h"

#include <QFile>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <chrono>
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
    std::vector<double>                   m_EstGrowthRates;
    std::vector<double>                   m_EstCarryingCapacities;
//  std::vector<double>                   m_EstExploitationRates;
    std::vector<double>                   m_EstCatchability;
    std::vector<double>                   m_EstExponent;
    boost::numeric::ublas::matrix<double> m_EstAlpha;
    boost::numeric::ublas::matrix<double> m_EstBetaSpecies;
    boost::numeric::ublas::matrix<double> m_EstBetaGuilds;
    boost::numeric::ublas::matrix<double> m_EstPredation;
    boost::numeric::ublas::matrix<double> m_EstHandling;

    void createOutputStr(const int&         numTotalParameters,
                         const int&         numEstParameters,
                         const int&         numSubRuns,
                         const double&      bestFitness,
                         const double&      fitnessStdDev,
                         const Data_Struct& beeStruct,
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
    void RunCompleted(QString outputStr);
    void SubRunCompleted(int RunNum, int SubRun, int NumSubRuns);
    void UpdateProgressData(int NumSpecies, int NumParams, QString elapsedTime);

public:
    Bees_Estimator();
   ~Bees_Estimator();

    void estimateParameters(Data_Struct &siStruct,int RunNum);
    void getEstimatedGrowthRates(std::vector<double> &EstGrowthRates);
    void getEstimatedCarryingCapacities(std::vector<double> &EstCarryingCapacity);
    void getEstimatedCatchability(std::vector<double> &EstCatchability);
    void getEstimatedCompetitionAlpha(boost::numeric::ublas::matrix<double> &EstInteraction);
    void getEstimatedCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &theEstCompSpecies);
    void getEstimatedCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &theEstCompGuilds);
    void getEstimatedPredation(boost::numeric::ublas::matrix<double> &EstPredation);
    void getEstimatedHandling(boost::numeric::ublas::matrix<double> &EstHandling);
    void getEstimatedExponent(std::vector<double> &EstExponent);

};


