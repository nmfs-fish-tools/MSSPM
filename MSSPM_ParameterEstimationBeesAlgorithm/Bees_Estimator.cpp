

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

    std::cout << msg << " ";
    std::cout << fitness << " : ";
    for (int i=0; i<numParameters; ++i) {
        std::cout << " " << parameters[i];
    }
    std::cout << std::endl;
}

void
Bees_Estimator::estimateParameters(Data_Struct &beeStruct,
                                   int& RunNumber,
                                   std::vector<QString>& MultiRunLines,
                                   int& TotalIndividualRuns)
{
    bool ok=false;
    bool isAggProd   = (beeStruct.CompetitionForm == "AGG-PROD");
    bool isAMultiRun = (beeStruct.NLoptNumberOfRuns > 1);
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
    double totStdDev;
    double bestFitness;
    double fitnessStdDev   = 0;
    double MeanFitness     = 0;
    double lastBestFitness = 99999;
    std::string msg;
    std::string errorMsg;
    std::string bestFitnessStr;
    std::vector<double> lastBestParameters;
    std::chrono::_V2::system_clock::time_point startTime = nmfUtils::startTimer();
    std::chrono::_V2::system_clock::time_point startTimeSpecies;
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
    m_EstCarryingCapacities.clear();
    m_EstAlpha.clear();
    m_EstPredation.clear();
    m_EstHandling.clear();
    m_EstCatchability.clear();
    m_EstExponent.clear();
    m_EstBetaSpecies.clear();
    m_EstBetaGuilds.clear();
    nmfUtils::initialize(m_EstAlpha,      NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstPredation,  NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstHandling,   NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstBetaSpecies,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
    nmfUtils::initialize(m_EstBetaGuilds, NumSpeciesOrGuilds,NumGuilds);

    startTimeSpecies = nmfUtils::startTimer();

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

        for (int run=0; run<NumSubRuns; ++run) {

            for (int subRunNum=1; subRunNum<=NumRepetitions; ++subRunNum)
            {
std::cout << "subRunNum: " << subRunNum << std::endl;
                // Initialize main class ptr
                beesAlg   = std::make_unique<BeesAlgorithm>(beeStruct,nmfConstantsMSSPM::VerboseOn);
                beesStats = std::make_unique<BeesStats>(beeStruct.TotalNumberParameters,NumRepetitions);
                beesAlg->initializeParameterRangesAndPatchSizes();
                errorMsg.clear();
                ok = beesAlg->estimateParameters(
                            bestFitness,EstParameters,
                            RunNumber,subRunNum,errorMsg);
std::cout << "  ok = " << ok << std::endl;
                if (! errorMsg.empty()) {
                    ok = false;
                    emit ErrorFound(errorMsg);
                    break;
                }
                if (ok) {
                    msg = "Run " + std::to_string(subRunNum);
                    printBee(msg,bestFitness,EstParameters);
                    beesStats->addData(bestFitness,EstParameters);
                    if (bestFitness < lastBestFitness) {
                        lastBestFitness = bestFitness;
                        lastBestParameters = EstParameters;
                    }
                    emit RepetitionRunCompleted(RunNumber,subRunNum,NumRepetitions);
                }
                // Added a delay to give Qt enough time to finish drawing this run's curve.
                std::this_thread::sleep_for(std::chrono::microseconds(usecDelay));

                // Break out if user has stopped the run
                if (wasStoppedByUser()) {
                    std::cout << "Bees_Estimator StoppedByUser" << std::endl;
                    ok = false;
                    break;
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
                                                 m_EstCarryingCapacities,
                                                 m_EstSystemCarryingCapacity);
                beesAlg->extractHarvestParameters(EstParameters,startPos,m_EstCatchability);
                beesAlg->extractCompetitionParameters(EstParameters,startPos,
                                                      m_EstAlpha,
                                                      m_EstBetaSpecies,
                                                      m_EstBetaGuilds);

                beesAlg->extractPredationParameters(EstParameters,startPos,m_EstPredation);
                beesAlg->extractHandlingParameters(EstParameters,startPos,m_EstHandling);
                beesAlg->extractExponentParameters(EstParameters,startPos,m_EstExponent);
                numEstParameters = beesAlg->calculateActualNumEstParameters();
                numTotalParameters = EstParameters.size();
                createOutputStr(numTotalParameters,numEstParameters,NumRepetitions,
                                bestFitness,fitnessStdDev,beeStruct,bestFitnessStr);
                emit RunCompleted(bestFitnessStr,beeStruct.showDiagnosticChart);

            }

        } // end for run

        if (isAMultiRun) {
            emit SubRunCompleted(RunNumber++,
                                 TotalIndividualRuns,
                                 beeStruct.EstimationAlgorithm,
                                 beeStruct.MinimizerAlgorithm,
                                 beeStruct.ObjectiveCriterion,
                                 beeStruct.ScalingAlgorithm,
                                 beeStruct.MultiRunModelFilename,
                                 bestFitness);
        } else {
            emit RunCompleted(bestFitnessStr,beeStruct.showDiagnosticChart);
        }

    } // end for multiRun

    if (isAMultiRun) {
        emit AllSubRunsCompleted(beeStruct.MultiRunSpeciesFilename,
                                 beeStruct.MultiRunModelFilename);
    }

    std::string elapsedTimeStr = "Elapsed runtime: " + nmfUtils::elapsedTime(startTime);
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

    return (cmd == "StoppedByUser");
}

void
Bees_Estimator::createOutputStr(const int&         numTotalParameters,
                                const int&         numEstParameters,
                                const int&         numSubRuns,
                                const double&      bestFitness,
                                const double&      fitnessStdDev,
                                const Data_Struct& beeStruct,
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
        bestFitnessStr += convertValues1DToOutputStr("Carrying Capacity:",m_InitialCarryingCapacities,true);
    }
    bestFitnessStr += "<br><br><strong>Estimated Parameters:</strong>";
    bestFitnessStr += convertValues1DToOutputStr("Initial Biomass:    ",m_EstInitBiomass,false);
    bestFitnessStr += convertValues1DToOutputStr("Growth Rate:        ",m_EstGrowthRates,false);
    if (growthForm == "Logistic") {
        bestFitnessStr += convertValues1DToOutputStr("Carrying Capacity:  ",m_EstCarryingCapacities,true);
    }

    if (harvestForm == "Effort (qE)") {
        bestFitnessStr += convertValues1DToOutputStr("Catchability:       ",m_EstCatchability,false);
    }

    if (competitionForm == "NO_K") {
        bestFitnessStr += convertValues2DToOutputStr("Competition (alpha):",m_EstAlpha);
    } else if ((competitionForm == "MS-PROD") ||
               (competitionForm == "AGG-PROD"))
    {
        if (competitionForm == "MS-PROD") {
            bestFitnessStr += convertValues2DToOutputStr("Competition (beta::species):",m_EstBetaSpecies);
        }
        bestFitnessStr += convertValues2DToOutputStr("Competition (beta::guilds): ",m_EstBetaSpecies);
    }

    if ((predationForm == "Type I")  ||
            (predationForm == "Type II") ||
            (predationForm == "Type III"))
    {
        bestFitnessStr += convertValues2DToOutputStr("Predation (rho):",m_EstPredation);
    }
    if ((predationForm == "Type II") ||
            (predationForm == "Type III"))
    {
        bestFitnessStr += convertValues2DToOutputStr("Handling:       ",m_EstHandling);
    }
    if (predationForm == "Type III")
    {
        bestFitnessStr += "<br>&nbsp;&nbsp;";
        bestFitnessStr += convertValues1DToOutputStr("Predation Exponent:",m_EstExponent,false);
//        for (unsigned i=0; i<m_EstExponent.size(); ++i) {
//            if (i == 0) {
//                bestFitnessStr += "&nbsp;&nbsp;Predation Exponent: ";
//            }
//            bestFitnessStr += nmfUtils::to_string_scientificNotation(m_EstExponent[i]) + " ";
//        }
    }
}

std::string
Bees_Estimator::convertValues1DToOutputStr(const std::string& label,
                                           const std::vector<double> &values,
                                           const bool& includeTotal)
{
    double val;
    double totalVal = 0;
    std::string bestFitnessStr = "";

    bestFitnessStr += "<br>&nbsp;&nbsp;" + label + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    for (unsigned i=0; i<values.size(); ++i) {
        val = values[i];
        bestFitnessStr += nmfUtils::convertToScientificNotation(val) + "&nbsp;&nbsp;";
        totalVal += val;
    }
    if (includeTotal) {
        bestFitnessStr += "<br>&nbsp;&nbsp;Total " + label + "&nbsp;&nbsp;" +
                nmfUtils::convertToScientificNotation(totalVal);
    }

    return bestFitnessStr;
}


std::string
Bees_Estimator::convertValues2DToOutputStr(const std::string& label,
                                           const boost::numeric::ublas::matrix<double> &matrix)
{
    std::string bestFitnessStr = "";

    for (unsigned i=0; i<matrix.size1(); ++i) {
        bestFitnessStr += "<br>&nbsp;&nbsp;";
        for (unsigned j=0; j<matrix.size2(); ++j) {
            if ((i == 0) && (j == 0)) {
                bestFitnessStr += "&nbsp;&nbsp;" + label + "<br>&nbsp;&nbsp;";
            }
            bestFitnessStr += "&nbsp;&nbsp;&nbsp;" + nmfUtils::convertToScientificNotation(matrix(i,j));
        }
    }

    return bestFitnessStr;
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
Bees_Estimator::getEstimatedInitBiomass(std::vector<double> &estInitBiomass)
{
    estInitBiomass = m_EstInitBiomass;
}

void
Bees_Estimator::getEstimatedGrowthRates(std::vector<double> &estGrowthRates)
{
    estGrowthRates = m_EstGrowthRates;
}

void
Bees_Estimator::getEstimatedCarryingCapacities(std::vector<double> &estCarryingCapacities)
{
    estCarryingCapacities = m_EstCarryingCapacities;
}

void
Bees_Estimator::getEstimatedCatchability(std::vector<double> &estCatchability)
{
    estCatchability = m_EstCatchability;
}

void
Bees_Estimator::getEstimatedCompetitionAlpha(boost::numeric::ublas::matrix<double> &estInteraction)
{
    estInteraction = m_EstAlpha;
}

void
Bees_Estimator::getEstimatedCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &estCompSpecies)
{
    estCompSpecies = m_EstBetaSpecies;
}

void
Bees_Estimator::getEstimatedCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &estCompGuilds)
{
    estCompGuilds = m_EstBetaGuilds;
}

void
Bees_Estimator::getEstimatedPredation(boost::numeric::ublas::matrix<double> &estPredation)
{
    estPredation = m_EstPredation;
}

void
Bees_Estimator::getEstimatedHandling(boost::numeric::ublas::matrix<double> &estHandling)
{
    estHandling = m_EstHandling;
}

void
Bees_Estimator::getEstimatedExponent(std::vector<double> &estExponent)
{
    estExponent = m_EstExponent;
}
