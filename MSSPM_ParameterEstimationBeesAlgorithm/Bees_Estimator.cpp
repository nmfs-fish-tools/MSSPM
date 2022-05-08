

#include "Bees_Estimator.h"


Bees_Estimator::Bees_Estimator() {
}


Bees_Estimator::~Bees_Estimator() {

}

void
Bees_Estimator::printBee(std::string          msg,
                         double&              fitness,
                         std::vector<double>& parameters)
{
    int numParameters = parameters.size();

    std::cout << msg << ", ";
    std::cout << "Fitness: " << fitness << ": ";
    for (int i=0; i<numParameters; ++i) {
        std::cout << " " << parameters[i];
    }
    std::cout << std::endl;
}

void
Bees_Estimator::estimateParameters(nmfStructsQt::ModelDataStruct &beeStruct,
                                   int& RunNumber,
                                   std::vector<QString>& MultiRunLines,
                                   int& TotalIndividualRuns)
{
    bool foundOneBeesRun = false;
    bool ok=false;
    bool isAggProd   = (beeStruct.CompetitionForm == "AGG-PROD");
    bool isAMultiRun = (beeStruct.NLoptNumberOfRuns > 1) && (MultiRunLines.size() > 0);
    int startPos = 0;
    int NumSpecies = beeStruct.NumSpecies;
    int NumGuilds  = beeStruct.NumGuilds;
    int NumSpeciesOrGuilds = (isAggProd) ? NumGuilds : NumSpecies;
    int NumRepetitions = beeStruct.BeesNumRepetitions;
    int NumMultiRuns = 1;
    int numTotalParameters;
    int numEstParameters; // The parameters that don't have their min range equal to their max range.
    int usecDelay = 300000;
    int NumSubRuns;
//  int TotalIndividualRuns = 0;
    long seed;
    double totStdDev;
    double bestFitness;
    double fitnessStdDev   = 0;
    double MeanFitness     = 0;
    double lastBestFitness = 99999;
    std::string msg;
    std::string errorMsg;
    std::string bestFitnessStr;
    std::vector<double> lastBestParameters;
    QDateTime startTime = nmfUtilsQt::getCurrentTime();
    std::vector<double> EstParameters;
    std::vector<double> MeanEstParameters;
    std::vector<double> stdDevParameters;
    std::unique_ptr<BeesAlgorithm> beesAlg;
    std::unique_ptr<BeesStats>     beesStats;
//  std::vector<QString> MultiRunLines;

    m_InitialCarryingCapacities.clear();
    m_EstSystemCarryingCapacity = 0;
    m_EstInitBiomass.clear();
    m_EstGrowthRates.clear();
    m_EstGrowthRateCovariateCoeffs.clear();
    m_EstCarryingCapacities.clear();
    m_EstCarryingCapacityCovariateCoeffs.clear();
    m_EstCatchabilityCovariateCoeffs.clear();
    m_EstAlpha.clear();
    m_EstPredation.clear();
    m_EstHandling.clear();
    m_EstCatchability.clear();
    m_EstSurveyQ.clear();
    m_EstSurveyQCovariateCoeffs.clear();
    m_EstExponent.clear();
    m_EstBetaSpecies.clear();
    m_EstBetaGuilds.clear();
    m_EstBetaGuildsGuilds.clear();
    nmfUtils::initialize(m_EstAlpha,      NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstPredation,  NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstHandling,   NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstBetaSpecies,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstBetaGuilds, NumSpeciesOrGuilds,NumGuilds);
    nmfUtils::initialize(m_EstBetaGuildsGuilds, NumGuilds,   NumGuilds);

//    startTimeSpecies = nmfUtils::startTimer();
std::cout << "Bees: num estimate boxes: " << beeStruct.EstimateRunBoxes.size() << std::endl;
std::cout << "Bees: isAMultiRun: " << isAMultiRun << std::endl;
    if (isAMultiRun) {
        NumMultiRuns = int(MultiRunLines.size());
    }

//    emit InitializeSubRuns(beeStruct.MultiRunModelFilename,TotalIndividualRuns);

    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        m_InitialCarryingCapacities.push_back(beeStruct.CarryingCapacity[i]);
    }

    for (int multiRun=0; multiRun<NumMultiRuns; ++multiRun) {

        NumSubRuns = 1;
        if (isAMultiRun) {
            nmfUtilsQt::reloadDataStruct(beeStruct,MultiRunLines[multiRun]);
            NumSubRuns = beeStruct.NLoptNumberOfRuns;
        }

std::cout << "multiRun: " << multiRun << ", EstimationAlgorithm: " << beeStruct.EstimationAlgorithm << std::endl;
        if (beeStruct.EstimationAlgorithm != "Bees Algorithm") { // This must follow the reloadNLoptStruct call
std::cout << "Skipping: " <<  MultiRunLines[multiRun].toStdString() << std::endl;
            continue; // skip over rest of for statement and continue with next increment
        }

        foundOneBeesRun = true;
        for (int run=0; run<NumSubRuns; ++run) {

            for (int subRunNum=1; subRunNum<=NumRepetitions; ++subRunNum)
            {
std::cout << "subRunNum: " << subRunNum << std::endl;
                // Initialize main class ptr
                beesAlg   = std::make_unique<BeesAlgorithm>(beeStruct,nmfConstantsMSSPM::VerboseOn);
                seed      = (beeStruct.useFixedSeedBees) ? run*NumSubRuns+subRunNum : -1;
                beesAlg->setSeed(seed);
                beesStats = std::make_unique<BeesStats>(beeStruct.TotalNumberParameters,NumRepetitions);
                beesAlg->initializeParameterRangesAndPatchSizes(beeStruct);

                errorMsg.clear();
                ok = beesAlg->estimateParameters(
                            bestFitness,EstParameters,
                            RunNumber,subRunNum,errorMsg);
std::cout << "Bees: num est parameters: " << EstParameters.size() << std::endl;

                if (! errorMsg.empty()) {
                    ok = false;
                    emit ErrorFound(errorMsg);
                    break;
                }
                if (ok) {
                    msg = "Run: " + std::to_string(subRunNum);
                    printBee(msg,bestFitness,EstParameters);
                    beesStats->addData(bestFitness,EstParameters);
                    if (bestFitness < lastBestFitness) {
                        lastBestFitness = bestFitness;
                        lastBestParameters = EstParameters;
                    }
                    emit RepetitionRunCompleted(RunNumber,subRunNum,NumRepetitions);
                }
                // Added a delay to give Qt enough time to finish drawing this run's curve.
                QThread::usleep((unsigned long)(usecDelay));

                // Break out if user has stopped the run
                if (wasStoppedByUser()) {
                    std::cout << "=== === ===> Bees_Estimator StoppedByUser" << std::endl;
                    ok = false;
//                  break;
                    return;
                }
            }

            if (ok) {
                // Use the last best data and get some statistics
                bestFitness   = lastBestFitness;
                EstParameters = lastBestParameters;
                beesStats->getMean(MeanFitness,MeanEstParameters);
                beesStats->getStdDev(fitnessStdDev,totStdDev,stdDevParameters);

                // Extract the parameters and place them into their respective data structures.
                beesAlg->extractInitBiomass(EstParameters,startPos,
                                            m_EstInitBiomass);
                beesAlg->extractGrowthParameters(EstParameters,startPos,
                                                 m_EstGrowthRates,
                                                 m_EstGrowthRateCovariateCoeffs,
                                                 m_EstCarryingCapacities,
                                                 m_EstCarryingCapacityCovariateCoeffs,
                                                 m_EstSystemCarryingCapacity);
                beesAlg->extractHarvestParameters(EstParameters,startPos,
                                                  m_EstCatchability,
                                                  m_EstCatchabilityCovariateCoeffs);
                beesAlg->extractCompetitionParameters(EstParameters,startPos,
                                                      m_EstAlpha,
                                                      m_EstBetaSpecies,
                                                      m_EstBetaGuilds,
                                                      m_EstBetaGuildsGuilds);
                beesAlg->extractPredationParameters(EstParameters,startPos,m_EstPredation);
                beesAlg->extractHandlingParameters(EstParameters,startPos,m_EstHandling);
                beesAlg->extractExponentParameters(EstParameters,startPos,m_EstExponent);
                beesAlg->extractSurveyQParameters(EstParameters,startPos,
                                                  m_EstSurveyQ,m_EstSurveyQCovariateCoeffs);
                numEstParameters   = beesAlg->calculateActualNumEstParameters();
                numTotalParameters = EstParameters.size();
                createOutputStr(numEstParameters,numTotalParameters,NumRepetitions,
                                bestFitness,fitnessStdDev,beeStruct,bestFitnessStr);

                emit RunCompleted(bestFitnessStr,beeStruct.showDiagnosticChart);

            }

        } // end for run

        if (isAMultiRun) {
std::cout << "2 Emitting SubRunCompleted" << std::endl;
            emit SubRunCompleted(RunNumber++,
                                 TotalIndividualRuns,
                                 beeStruct.EstimationAlgorithm,
                                 beeStruct.MinimizerAlgorithm,
                                 beeStruct.ObjectiveCriterion,
                                 beeStruct.ScalingAlgorithm,
                                 beeStruct.MultiRunModelFilename,
                                 bestFitness);
        } else {
std::cout << "2 Emitting RunCompleted" << std::endl;
//            emit RunCompleted(bestFitnessStr,beeStruct.showDiagnosticChart);
        }

    } // end for multiRun

    if (isAMultiRun && foundOneBeesRun) {
std::cout << "3 Emitting AllSubRunsCompleted" << std::endl;
        emit AllSubRunsCompleted(beeStruct.MultiRunSpeciesFilename,
                                 beeStruct.MultiRunModelFilename);
    }

    std::string elapsedTimeStr = "Elapsed runtime: " + nmfUtilsQt::elapsedTime(startTime);
    std::cout << elapsedTimeStr << std::endl;

    stopRun(elapsedTimeStr,bestFitnessStr);
}


bool
Bees_Estimator::wasStoppedByUser()
{
    std::string cmd;
    std::ifstream inputFile(nmfConstantsMSSPM::MSSPMStopRunFile);
    if (inputFile) {
        std::getline(inputFile,cmd);
    }
    inputFile.close();

    return (QString::fromStdString(cmd).contains("Stop")); // cmd == "StoppedByUser");
}

void
Bees_Estimator::createOutputStr(
        const int&         numEstParameters,
        const int&         numTotalParameters,
        const int&         numSubRuns,
        const double&      bestFitness,
        const double&      fitnessStdDev,
        const nmfStructsQt::ModelDataStruct& beeStruct,
        std::string&       bestFitnessStr)
{
    std::string growthForm      = beeStruct.GrowthForm;
    std::string harvestForm     = beeStruct.HarvestForm;
    std::string competitionForm = beeStruct.CompetitionForm;
    std::string predationForm   = beeStruct.PredationForm;

    std::cout << "Est'd Parameters: " << numEstParameters << std::endl;
    std::cout << "Total Parameters: " << numTotalParameters << std::endl;
    std::cout << "Fitness std dev: "  << fitnessStdDev << std::endl;

    // Write to Stop file
    bestFitnessStr  = "Est'd Parameters:&nbsp;"     + std::to_string(numEstParameters);
    bestFitnessStr += "<br>Total Parameters:&nbsp;" + std::to_string(numTotalParameters);

    bestFitnessStr += "<br><br>Number of Runs:&nbsp;&nbsp;&nbsp;" + std::to_string(numSubRuns);
    bestFitnessStr += "<br>Best Fitness (SSE) value of all runs:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + std::to_string(bestFitness);
    bestFitnessStr += "<br>Std dev of Best Fitness values from all runs:&nbsp;&nbsp;" + std::to_string(fitnessStdDev);

    if (growthForm == "Logistic") {
        bestFitnessStr += "<br><br>Initial Parameters:";
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Carrying Capacity:",m_InitialCarryingCapacities,true);
    }
    bestFitnessStr += "<br><br><strong>Estimated Parameters:<br></strong>";
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Initial Absolute Biomass:    ", m_EstInitBiomass,false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Growth Rate:        ",          m_EstGrowthRates,false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Growth Rate Covariate Coeffs: ",m_EstGrowthRateCovariateCoeffs,false);
    if (growthForm == "Logistic") {
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Carrying Capacity:  ",                 m_EstCarryingCapacities,true);
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Carrying Capacity Covariate Coeffs:  ",m_EstCarryingCapacityCovariateCoeffs,true);
    }

    if (harvestForm == "Effort (qE)") {
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Catchability:       ",                 m_EstCatchability,false);
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Catchability Covariate Coeffs:       ",m_EstCatchabilityCovariateCoeffs,false);
    }

    if (competitionForm == "NO_K") {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (alpha):",m_EstAlpha);
    } else if ((competitionForm == "MS-PROD") ||
               (competitionForm == "AGG-PROD"))
    {
        if (competitionForm == "MS-PROD") {
            bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (beta::species):",m_EstBetaSpecies);
        }
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (beta::guilds): ",m_EstBetaSpecies);
    }

    if ((predationForm == "Type I")  ||
        (predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Predation (rho):",m_EstPredation);
    }
    if ((predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Handling:       ",m_EstHandling);
    }
    if (predationForm == "Type III")
    {
        bestFitnessStr += "<br>&nbsp;&nbsp;";
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Predation Exponent:",m_EstExponent,false);
//        for (unsigned i=0; i<m_EstExponent.size(); ++i) {
//            if (i == 0) {
//                bestFitnessStr += "&nbsp;&nbsp;Predation Exponent: ";
//            }
//            bestFitnessStr += nmfUtils::to_string_scientificNotation(m_EstExponent[i]) + " ";
//        }
    }
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Survey Q:    ",m_EstSurveyQ,false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Survey Q Covariate Coeffs:  ",m_EstSurveyQCovariateCoeffs,false);

}

void
Bees_Estimator::stopRun(const std::string &elapsedTimeStr,
                        const std::string &fitnessStr)
{
    std::ofstream outputFile(nmfConstantsMSSPM::MSSPMStopRunFile);
    outputFile << "Stop" << std::endl;         // cmd
    outputFile << "" << std::endl;             // run name
    outputFile << elapsedTimeStr << std::endl; // elapsed time
    outputFile << fitnessStr << std::endl;
    outputFile.close();
}

void
Bees_Estimator::getEstInitBiomass(std::vector<double> &estInitBiomass)
{
    estInitBiomass = m_EstInitBiomass;
}

void
Bees_Estimator::getEstGrowthRates(std::vector<double> &estGrowthRates)
{
    estGrowthRates = m_EstGrowthRates;
}

void
Bees_Estimator::getEstGrowthRateCovariateCoeffs(std::vector<double> &estGrowthRateCovariateCoeffs)
{
    estGrowthRateCovariateCoeffs = m_EstGrowthRateCovariateCoeffs;
}

void
Bees_Estimator::getEstCarryingCapacities(std::vector<double> &estCarryingCapacities)
{
    estCarryingCapacities = m_EstCarryingCapacities;
}

void
Bees_Estimator::getEstCarryingCapacityCovariateCoeffs(std::vector<double> &estCarryingCapacityCovariateCoeffs)
{
    estCarryingCapacityCovariateCoeffs = m_EstCarryingCapacityCovariateCoeffs;
}

void
Bees_Estimator::getEstCatchabilityCovariateCoeffs(std::vector<double> &estCatchabilityCovariateCoeffs)
{
    estCatchabilityCovariateCoeffs = m_EstCatchabilityCovariateCoeffs;
}

void
Bees_Estimator::getEstCatchability(std::vector<double> &estCatchability)
{
    estCatchability = m_EstCatchability;
}

void
Bees_Estimator::getEstSurveyQ(std::vector<double> &estSurveyQ)
{
    estSurveyQ = m_EstSurveyQ;
}

void
Bees_Estimator::getEstSurveyQCovariateCoeffs(std::vector<double> &estSurveyQCovariateCoeffs)
{
    estSurveyQCovariateCoeffs = m_EstSurveyQCovariateCoeffs;
}

void
Bees_Estimator::getEstCompetitionAlpha(boost::numeric::ublas::matrix<double> &estInteraction)
{
    estInteraction = m_EstAlpha;
}

void
Bees_Estimator::getEstCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &estCompSpecies)
{
    estCompSpecies = m_EstBetaSpecies;
}

void
Bees_Estimator::getEstCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &estCompGuilds)
{
    estCompGuilds = m_EstBetaGuilds;
}

void
Bees_Estimator::getEstCompetitionBetaGuildsGuilds(boost::numeric::ublas::matrix<double> &estCompGuildsGuilds)
{
    estCompGuildsGuilds = m_EstBetaGuildsGuilds;
}

void
Bees_Estimator::getEstPredation(boost::numeric::ublas::matrix<double> &estPredation)
{
    estPredation = m_EstPredation;
}

void
Bees_Estimator::getEstHandling(boost::numeric::ublas::matrix<double> &estHandling)
{
    estHandling = m_EstHandling;
}

void
Bees_Estimator::getEstExponent(std::vector<double> &estExponent)
{
    estExponent = m_EstExponent;
}
