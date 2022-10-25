#include "BeesStats.h"

BeesStats::BeesStats(const int &totParameters,
                     const int &numRuns)
{
    m_sumFitness      = 0;
    m_meanFitness     = 0;
    m_totalParameters = totParameters;
    m_numRuns         = numRuns;
    m_sumData.clear();
    m_fitnessData.clear();
    m_stdDev.clear();
    m_meanData.clear();
    std::vector<double> newVec = {};

    for (int i=0; i<m_totalParameters; ++i) {
        m_data.push_back(newVec);
        m_sumData.push_back(0);
    }
}

void
BeesStats::addData(const double& bestFitness,
                   const std::vector<double>& parameters)
{
    double param;

    if ((unsigned)m_totalParameters != parameters.size()) {
        std::cout << "Error (1) BeesStats: Total number of parameters doesn't agree with size of parameters vector passed in." << std::endl;
        return;
    }
    for (int i=0; i<m_totalParameters; ++i) {
        param = parameters[i];
        m_data[i].push_back(param);
        m_sumData[i] += param;
    }
    m_fitnessData.push_back(bestFitness);
    m_sumFitness += bestFitness;
}

void
BeesStats::getMean(double& fitness, std::vector<double>& result)
{
    m_meanData.clear();
    for (int i=0; i<m_totalParameters; ++i) {
        m_meanData.push_back(m_sumData[i]/m_numRuns);
    }
    fitness = m_sumFitness/m_numRuns;
    m_meanFitness = fitness;
    result = m_meanData;
}

void
BeesStats::getStdDev(double& fitnessStdDev,
                     double& totStdDev,
                     std::vector<double>& stdDevParameters)
{
    double diff;
    double stdDev=0;
    double sumSquares=0;

    totStdDev = 0;
    m_stdDev.clear();
    for (int j=0; j<m_totalParameters; ++j) {
        sumSquares = 0;
        for (int i=0; i<m_numRuns; ++i) {
            diff = m_data[j][i] - m_meanData[j];
            sumSquares += (diff * diff);
        }
        stdDev = sqrt(sumSquares/m_numRuns);
        m_stdDev.push_back(stdDev);
        totStdDev += stdDev;
    }
    totStdDev /= m_totalParameters;
    stdDevParameters = m_stdDev;

    // Calculate fitness std dev
    sumSquares = 0;
    for (int i=0; i<m_numRuns; ++i) {
        diff = m_fitnessData[i] - m_meanFitness;
        sumSquares += (diff * diff);
    }
    fitnessStdDev = sqrt(sumSquares/m_numRuns);
}
