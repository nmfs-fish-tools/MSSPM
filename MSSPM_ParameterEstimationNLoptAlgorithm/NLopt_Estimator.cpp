#include "NLopt_Estimator.h"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vector>

bool m_Quit;
//int NLopt_Estimator::m_NLoptIters   = 0;
int NLopt_Estimator::m_NLoptFcnEvals  = 0;
int NLopt_Estimator::m_NumObjFcnCalls = 0;
int NLopt_Estimator::m_RunNum         = 0;
nlopt::opt NLopt_Estimator::m_Optimizer;

std::unique_ptr<nmfGrowthForm>      NLoptGrowthForm;
std::unique_ptr<nmfHarvestForm>     NLoptHarvestForm;
std::unique_ptr<nmfCompetitionForm> NLoptCompetitionForm;
std::unique_ptr<nmfPredationForm>   NLoptPredationForm;


NLopt_Estimator::NLopt_Estimator()
{
    m_Quit = false;
    m_Seed = 0;
    m_MinimizerToEnum.clear();
    m_MohnsRhoOffset = 0;

    // Load Minimizer Name Map with global algorithms
    m_MinimizerToEnum["GN_ORIG_DIRECT_L"] = nlopt::GN_ORIG_DIRECT_L;
    m_MinimizerToEnum["GN_DIRECT_L"]      = nlopt::GN_DIRECT_L;
    m_MinimizerToEnum["GN_DIRECT_L_RAND"] = nlopt::GN_DIRECT_L_RAND;
    m_MinimizerToEnum["GN_CRS2_LM"]       = nlopt::GN_CRS2_LM;
    m_MinimizerToEnum["GD_StoGO"]         = nlopt::GD_STOGO;

    // Load Minimizer Name Map with local algorithms
    m_MinimizerToEnum["LN_COBYLA"]        = nlopt::LN_COBYLA;
    m_MinimizerToEnum["LN_BOBYQA"]        = nlopt::LN_BOBYQA;
    m_MinimizerToEnum["LN_PRAXIS"]        = nlopt::LN_PRAXIS;
    m_MinimizerToEnum["LN_NELDERMEAD"]    = nlopt::LN_NELDERMEAD;
    m_MinimizerToEnum["LN_SBPLX"]         = nlopt::LN_SBPLX;
    m_MinimizerToEnum["LD_MMA"]           = nlopt::LD_MMA;
    m_MinimizerToEnum["LD_SLSQP"]         = nlopt::LD_SLSQP;
    m_MinimizerToEnum["LD_LBFGS"]         = nlopt::LD_LBFGS;

}

NLopt_Estimator::~NLopt_Estimator()
{
}

QString
NLopt_Estimator::getVersion()
{
    int major;
    int minor;
    int bugfix;

    nlopt::version(major,minor,bugfix);

    return (QString::number(major) + "." +
            QString::number(minor) + "." +
            QString::number(bugfix));
}

void
NLopt_Estimator::extractParameters(const nmfStructsQt::ModelDataStruct& NLoptDataStruct,
                                   const double *EstParameters,
                                   std::vector<double>& initBiomass,
                                   std::vector<double>& growthRate,
                                   std::vector<double>& carryingCapacity,
                                   std::vector<double>& catchabilityRate,
                                   boost::numeric::ublas::matrix<double>& competitionAlpha,
                                   boost::numeric::ublas::matrix<double>& competitionBetaSpecies,
                                   boost::numeric::ublas::matrix<double>& competitionBetaGuilds,
                                   boost::numeric::ublas::matrix<double>& competitionBetaGuildsGuilds,
                                   boost::numeric::ublas::matrix<double>& predation,
                                   boost::numeric::ublas::matrix<double>& handling,
                                   std::vector<double>& exponent,
                                   std::vector<double>& surveyQ)
{
    bool isLogistic     = (NLoptDataStruct.GrowthForm      == "Logistic");
    bool isCatchability = (NLoptDataStruct.HarvestForm     == "Effort (qE)");
    bool isAlpha        = (NLoptDataStruct.CompetitionForm == "NO_K");
    bool isMSPROD       = (NLoptDataStruct.CompetitionForm == "MS-PROD");
    bool isAGGPROD      = (NLoptDataStruct.CompetitionForm == "AGG-PROD");
    bool isRho          = (NLoptDataStruct.PredationForm   == "Type I") ||
                          (NLoptDataStruct.PredationForm   == "Type II") ||
                          (NLoptDataStruct.PredationForm   == "Type III");
    bool isHandling     = (NLoptDataStruct.PredationForm   == "Type II") ||
                          (NLoptDataStruct.PredationForm   == "Type III");
    bool isExponent     = (NLoptDataStruct.PredationForm   == "Type III");
    int m;
    int offset = 0;
    int NumSpecies = NLoptDataStruct.NumSpecies;
    int NumGuilds  = NLoptDataStruct.NumGuilds;
    int NumSpeciesOrGuilds = (isAGGPROD) ? NumGuilds : NumSpecies;
    int MatrixSize = NumSpeciesOrGuilds*NumSpeciesOrGuilds;

    initBiomass.clear();
    growthRate.clear();
    carryingCapacity.clear();
    catchabilityRate.clear();
    competitionAlpha.clear();
    competitionBetaSpecies.clear();
    competitionBetaGuilds.clear();
    competitionBetaGuildsGuilds.clear();
    predation.clear();
    handling.clear();
    exponent.clear();
    surveyQ.clear();

    // Must do this so that if a subsequent run does not have any of these,
    // they will be, not only cleared, but resized to 0,0.
    nmfUtils::initialize(competitionAlpha,0,0);
    nmfUtils::initialize(competitionBetaSpecies,0,0);
    nmfUtils::initialize(competitionBetaGuilds,0,0);
    nmfUtils::initialize(competitionBetaGuildsGuilds,0,0);
    nmfUtils::initialize(predation,0,0);
    nmfUtils::initialize(handling,0,0);

    // Always extract init biomass
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        initBiomass.emplace_back(EstParameters[offset+i]);
    }
    offset += NumSpeciesOrGuilds;

    // Always extract growth rate
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        growthRate.emplace_back(EstParameters[offset+i]);
    }
    offset += NumSpeciesOrGuilds;

    // Load the carrying capacity vector
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        if (isLogistic) {
            carryingCapacity.emplace_back(EstParameters[offset+i]);
        } else {
            carryingCapacity.emplace_back(0);
        }
    }
    if (isLogistic) {
        offset += NumSpeciesOrGuilds;
    }

    if (isCatchability) {
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            catchabilityRate.emplace_back(EstParameters[offset+i]);
        }
        offset += NumSpeciesOrGuilds;
    }

    if (isAlpha) {
        m = 0;
        nmfUtils::initialize(competitionAlpha,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                competitionAlpha(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += MatrixSize;
    }

    if (isMSPROD) {
        m = 0;
        nmfUtils::initialize(competitionBetaSpecies,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                competitionBetaSpecies(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += MatrixSize;
        m = 0;
        nmfUtils::initialize(competitionBetaGuilds, NumSpeciesOrGuilds,NumGuilds);
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                competitionBetaGuilds(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += NumSpeciesOrGuilds*NumGuilds;
    }

    if (isAGGPROD) {
        m = 0;
        nmfUtils::initialize(competitionBetaGuildsGuilds, NumGuilds,NumGuilds);
        for (int i=0; i<NumGuilds; ++i) {
            for (int j=0; j<NumGuilds; ++j) {
                competitionBetaGuildsGuilds(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += NumGuilds*NumGuilds;
    }


    if (isRho) {
        m = 0;
        nmfUtils::initialize(predation,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                predation(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += MatrixSize;
    }

    if (isHandling) {
        m = 0;
        nmfUtils::initialize(handling,NumSpeciesOrGuilds,NumSpeciesOrGuilds);
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            for (int j=0; j<NumSpeciesOrGuilds; ++j) {
                handling(i,j) = EstParameters[offset + (m++)];
            }
        }
        offset += MatrixSize;
    }

    if (isExponent) {
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            exponent.emplace_back(EstParameters[offset+i]);
        }
        offset += NumSpeciesOrGuilds;
    }

    // Survey Q
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        surveyQ.emplace_back(EstParameters[offset+i]);
    }
    offset += NumSpeciesOrGuilds;
}

double
NLopt_Estimator::myNaturalLog(double value)
{
    if (value == 0) {
        return 0;
    } else {
        return std::log(value);
    }
}
double
NLopt_Estimator::myExp(double value)
{
    if (value == 0) {
        return 0;
    } else {
        return std::exp(value);
    }
}

double
NLopt_Estimator::objectiveFunction(unsigned      nUnused,
                                   const double* EstParameters,
                                   double*       gradientUnused,
                                   void*         dataPtr)
{
    const int DefaultFitness = 99999;
    nmfStructsQt::ModelDataStruct NLoptDataStruct = *((nmfStructsQt::ModelDataStruct *)dataPtr);
    bool isAggProd = (NLoptDataStruct.CompetitionForm == "AGG-PROD");
    double EstBiomassVal;
    double GrowthTerm;
    double HarvestTerm;
    double CompetitionTerm;
    double PredationTerm;
    double systemCarryingCapacity;
    double guildK;
    double fitness=0;
    int timeMinus1;
    int NumYears   = NLoptDataStruct.RunLength+1 - m_MohnsRhoOffset;
    int NumSpecies = NLoptDataStruct.NumSpecies;    
    int NumGuilds  = NLoptDataStruct.NumGuilds;
    int guildNum = 0;
    int NumSpeciesOrGuilds;
    double surveyQVal;
    std::vector<double> initBiomass;
    std::vector<double> growthRate;
    std::vector<double> carryingCapacity;
    std::vector<double> guildCarryingCapacity;
    std::vector<double> predationExponent;
    std::vector<double> catchabilityRate;
    std::vector<double> surveyQ;
    boost::numeric::ublas::matrix<double> EstBiomassSpecies;
    boost::numeric::ublas::matrix<double> EstBiomassGuilds;
    boost::numeric::ublas::matrix<double> EstBiomassRescaled;
    boost::numeric::ublas::matrix<double> ObsBiomassBySpeciesOrGuildsRescaled;
    boost::numeric::ublas::matrix<double> competitionAlpha;
    boost::numeric::ublas::matrix<double> competitionBetaSpecies;
    boost::numeric::ublas::matrix<double> competitionBetaGuilds;
    boost::numeric::ublas::matrix<double> competitionBetaGuildsGuilds;
    boost::numeric::ublas::matrix<double> predationRho;
    boost::numeric::ublas::matrix<double> predationHandling;
    boost::numeric::ublas::matrix<double> ObsBiomassBySpeciesOrGuilds;
    boost::numeric::ublas::matrix<double> Catch        = NLoptDataStruct.Catch;
    boost::numeric::ublas::matrix<double> Effort       = NLoptDataStruct.Effort;
    boost::numeric::ublas::matrix<double> Exploitation = NLoptDataStruct.Exploitation;
    std::map<int,std::vector<int> > GuildSpecies = NLoptDataStruct.GuildSpecies;
    std::string MSSPMName = "Run " + std::to_string(m_RunNum) + "-1";
//std::cout << "NLopt_Estimator::objectiveFunction - start" << std::endl;

    if (m_Quit) {
//      throw nlopt::forced_stop();
        nlopt::forced_stop();
        throw "user stopped";
    }

    if (isAggProd) {
        NumSpeciesOrGuilds = NumGuilds;
        ObsBiomassBySpeciesOrGuilds = NLoptDataStruct.ObservedBiomassByGuilds;
    } else {
        NumSpeciesOrGuilds = NumSpecies;
        ObsBiomassBySpeciesOrGuilds = NLoptDataStruct.ObservedBiomassBySpecies;
    }

    //std::cout << "\n\nSetting size of competitionAlpha: " <<  NumSpeciesOrGuilds << "x" <<  NumSpeciesOrGuilds << std::endl;
    nmfUtils::initialize(EstBiomassSpecies,                   NumYears,           NumSpeciesOrGuilds);
    nmfUtils::initialize(EstBiomassGuilds,                    NumYears,           NumGuilds);
    nmfUtils::initialize(EstBiomassRescaled,                  NumYears,           NumSpeciesOrGuilds);
    nmfUtils::initialize(ObsBiomassBySpeciesOrGuildsRescaled, NumYears,           NumSpeciesOrGuilds);
    nmfUtils::initialize(competitionAlpha,                    NumSpeciesOrGuilds, NumSpeciesOrGuilds);
    nmfUtils::initialize(competitionBetaSpecies,              NumSpecies,         NumSpecies);
    nmfUtils::initialize(competitionBetaGuilds,               NumSpeciesOrGuilds, NumGuilds);
    nmfUtils::initialize(competitionBetaGuildsGuilds,         NumGuilds,          NumGuilds);
    nmfUtils::initialize(predationRho,                        NumSpeciesOrGuilds, NumSpeciesOrGuilds);
    nmfUtils::initialize(predationHandling,                   NumSpeciesOrGuilds, NumSpeciesOrGuilds);

    // RSK - how does this initBiomass work with SurveyQ
    extractParameters(NLoptDataStruct, EstParameters, initBiomass,
                      growthRate,carryingCapacity,catchabilityRate,
                      competitionAlpha,competitionBetaSpecies,
                      competitionBetaGuilds,competitionBetaGuildsGuilds,
                      predationRho,predationHandling,predationExponent,surveyQ);

    // Since we may be estimating SurveyQ, need to divide the Observed Biomass by the SurveyQ
    for (int species=0; species<int(ObsBiomassBySpeciesOrGuilds.size2()); ++species) {
        surveyQVal = surveyQ[species];
        for (int time=0; time<int(ObsBiomassBySpeciesOrGuilds.size1()); ++time) {
            ObsBiomassBySpeciesOrGuilds(time,species) /= surveyQVal;
        }
    }

    // Calculate carrying capacity for all guilds
    systemCarryingCapacity = 0;
    for (int i=0; i<NumGuilds; ++i) {
        guildK = 0;
        for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
            guildK += carryingCapacity[GuildSpecies[i][j]];

            systemCarryingCapacity += guildK;
        }
        guildCarryingCapacity.push_back(guildK);
    }

    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        EstBiomassSpecies(0,i) = NLoptDataStruct.ObservedBiomassBySpecies(0,i)/surveyQ[i];
//      EstBiomassSpecies(0,i) = ObsBiomassBySpeciesOrGuilds(0,i);
    }

    // RSK - Remember there's only initial guild biomass data
    // Multiply by guild surveyQ data when you have it
    for (int i=0; i<NumGuilds; ++i) {
        EstBiomassGuilds(0,i)  = NLoptDataStruct.ObservedBiomassByGuilds(0,i);
    }

    if (NLoptGrowthForm == nullptr) {
        incrementObjectiveFunctionCounter(MSSPMName,-1.0,NLoptDataStruct);
        return -1;
    }

    bool isCheckedInitBiomass = nmfUtils::isEstimateParameterChecked(NLoptDataStruct,"");

    for (int time=1; time<NumYears; ++time) {

        timeMinus1 = time - 1;
        for (int species=0; species<NumSpeciesOrGuilds; ++species) {

            if (isCheckedInitBiomass) { // if estimating the initial biomass
                if (timeMinus1 == 0) {
//                EstBiomassVal = initBiomass[species];
                  EstBiomassVal = EstBiomassSpecies(0,species);
                } else {
                    EstBiomassVal = EstBiomassSpecies(timeMinus1,species);
                }
            } else {
                EstBiomassVal = EstBiomassSpecies(timeMinus1,species);
            }

            GrowthTerm      = NLoptGrowthForm->evaluate(species,EstBiomassVal,
                                                        growthRate,carryingCapacity);
            HarvestTerm     = NLoptHarvestForm->evaluate(timeMinus1,species,
                                                         Catch,Effort,Exploitation,
                                                         EstBiomassVal,catchabilityRate);
            CompetitionTerm = NLoptCompetitionForm->evaluate(
                                   timeMinus1,species,EstBiomassVal,
                                   systemCarryingCapacity,
                                   growthRate,
                                   guildCarryingCapacity[guildNum],
                                   competitionAlpha,
                                   competitionBetaSpecies,
                                   competitionBetaGuilds,
                                   competitionBetaGuildsGuilds,
                                   EstBiomassSpecies,
                                   EstBiomassGuilds);
            PredationTerm   = NLoptPredationForm->evaluate(
                                   timeMinus1,species,
                                   predationRho,predationHandling,predationExponent,
                                   EstBiomassSpecies,EstBiomassVal);

            EstBiomassVal  += GrowthTerm - HarvestTerm - CompetitionTerm - PredationTerm;
            if (EstBiomassVal < 0) { // test code only
                EstBiomassVal = 0;
            }

            if ((EstBiomassVal < 0) || (std::isnan(std::fabs(EstBiomassVal)))) {
                incrementObjectiveFunctionCounter(MSSPMName,(double)DefaultFitness,NLoptDataStruct);
                return DefaultFitness;
            }

            EstBiomassSpecies(time,species) = EstBiomassVal;

            // update EstBiomassGuilds for next time step
            for (int i=0; i<NumGuilds; ++i) {
                for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                    EstBiomassGuilds(time,i) += EstBiomassSpecies(time,GuildSpecies[i][j]);
                }
            }
        } // end i
    } // end time

    // Scale the data
    std::string m_Scaling = NLoptDataStruct.ScalingAlgorithm;
    if (m_Scaling == "Min Max") {
        rescaleMinMax(EstBiomassSpecies, EstBiomassRescaled);
        rescaleMinMax(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    } else if (m_Scaling == "Mean") {
        rescaleMean(EstBiomassSpecies, EstBiomassRescaled);
        rescaleMean(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    } else {
//      std::cout << "Error: No Scaling Algorithm detected. Defaulting to Min Max." << std::endl;
        rescaleMinMax(EstBiomassSpecies, EstBiomassRescaled);
        rescaleMinMax(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    }

    // temp test RSK - take this logstuff out possibly
//for (int ii=0; ii<int(ObsBiomassBySpeciesOrGuildsRescaled.size1()); ++ii) {
//    for (int jj=0; jj<int(ObsBiomassBySpeciesOrGuildsRescaled.size2()); ++jj) {
//        ObsBiomassBySpeciesOrGuildsRescaled(ii,jj) = myNaturalLog(ObsBiomassBySpeciesOrGuildsRescaled(ii,jj));
//    }
//}
    // Calculate fitness using the appropriate objective criterion
    if (NLoptDataStruct.ObjectiveCriterion == "Least Squares") {

        fitness =  nmfUtilsStatistics::calculateSumOfSquares(
                    EstBiomassRescaled,
                    ObsBiomassBySpeciesOrGuildsRescaled);
//std::cout << "fitness1: " << fitness << std::endl; // OK
    } else if (NLoptDataStruct.ObjectiveCriterion == "Model Efficiency") {

        // Negate the MEF here since the ranges is from -inf to 1, where 1 is best.  So we negate it,
        // then minimize that, and then negate and plot the resulting value.
        fitness = -nmfUtilsStatistics::calculateModelEfficiency(
                    EstBiomassRescaled,
                    ObsBiomassBySpeciesOrGuildsRescaled);
    } else if (NLoptDataStruct.ObjectiveCriterion == "Maximum Likelihood") {
        // The maximum likelihood calculations must use the unscaled data or else the
        // results will be incorrect.

        fitness =  nmfUtilsStatistics::calculateMaximumLikelihoodNoRescale(
                    EstBiomassSpecies,
                    ObsBiomassBySpeciesOrGuilds);

        //std::cout << "fitness2: " << fitness << std::endl; // Not OK
     }

    incrementObjectiveFunctionCounter(MSSPMName,fitness,NLoptDataStruct);

    //std::cout << "NLopt_Estimator::objectiveFunction - end" << std::endl;
//std::cout << "fitness: " << fitness << std::endl;
    return fitness;
}


void
NLopt_Estimator::incrementObjectiveFunctionCounter(std::string MSSPMName,
                                                   double fitness,
                                                   nmfStructsQt::ModelDataStruct NLoptDataStruct)
{
    int unused = -1;

    // Update progress output file
    // RSK - comment out for now, some algorithms yield 0 evals while they're calculating
//    m_NLoptFcnEvals = m_Optimizer.get_numevals();

    ++m_NumObjFcnCalls;
//std::cout << "m_NumObjFcnCalls,fitness: " << m_NumObjFcnCalls << "," << fitness << std::endl;
    if (m_NumObjFcnCalls%1000 == 0) {

        writeCurrentLoopFile(MSSPMName,
                             m_NumObjFcnCalls,
                             fitness,
                             NLoptDataStruct.ObjectiveCriterion,
                             unused);
    }

}

void
NLopt_Estimator::writeCurrentLoopFile(const std::string &MSSPMName,
                                      const int         &NumGens,
                                      const double      &BestFitness,
                                      const std::string &ObjectiveCriterion,
                                      const int         &NumGensSinceBestFit)
{
    double adjustedBestFitness; // May need negating if ObjCrit is Model Efficiency
    std::ofstream outputFile(nmfConstantsMSSPM::MSSPMProgressChartFile,
                             std::ios::out|std::ios::app);

    adjustedBestFitness = BestFitness;
    //
    // Model Efficiency is to be maximized instead of minimized.  The
    // best value is 1. Since the code is set up to minimize for Least
    // Squares, I just negated the fitness and ran the minimization code.
    // Now, I just need to negate the fitness again so the plot will
    // show the fitness approaching +1.
    //
    if (ObjectiveCriterion == "Model Efficiency") {
        adjustedBestFitness = -adjustedBestFitness;
    }

    outputFile << MSSPMName   << ", "
               << NumGens     << ", "
               << adjustedBestFitness << ", "
               << NumGensSinceBestFit << std::endl;

    outputFile.close();
}

void
NLopt_Estimator::loadInitBiomassParameterRanges(
        std::vector<std::pair<double,double> >& parameterRanges,
        const nmfStructsQt::ModelDataStruct& dataStruct)
{
    bool isCheckedInitBiomass = nmfUtils::isEstimateParameterChecked(dataStruct,"InitBiomass");
    std::pair<double,double> aPair;

    // Always load initial biomass values
    for (unsigned species=0; species<dataStruct.InitBiomassMin.size(); ++species) {
        if (isCheckedInitBiomass) {
            aPair = std::make_pair(dataStruct.InitBiomassMin[species],
                                   dataStruct.InitBiomassMax[species]);
        } else {
            aPair = std::make_pair(dataStruct.InitBiomass[species],
                                   dataStruct.InitBiomass[species]);
        }
        parameterRanges.emplace_back(aPair);
    }
}

void
NLopt_Estimator::loadSurveyQParameterRanges(
        std::vector<std::pair<double,double> >& parameterRanges,
        const nmfStructsQt::ModelDataStruct& dataStruct)
{
    bool isCheckedSurveyQ= nmfUtils::isEstimateParameterChecked(dataStruct,"SurveyQ");
    std::pair<double,double> aPair;

    // Always load SurveyQ values
    for (unsigned species=0; species<dataStruct.SurveyQMin.size(); ++species) {
        if (isCheckedSurveyQ) {
            aPair = std::make_pair(dataStruct.SurveyQMin[species],
                                   dataStruct.SurveyQMax[species]);
        } else {
            aPair = std::make_pair(dataStruct.SurveyQ[species],
                                   dataStruct.SurveyQ[species]);
        }
        parameterRanges.emplace_back(aPair);
    }
}

void
NLopt_Estimator::setStoppingCriteria(nmfStructsQt::ModelDataStruct &NLoptStruct)
{
    if (NLoptStruct.NLoptUseStopVal) {
        std::cout << "Setting stop fitness value: " << NLoptStruct.NLoptStopVal << std::endl;
        m_Optimizer.set_stopval(NLoptStruct.NLoptStopVal);
    }
    if (NLoptStruct.NLoptUseStopAfterTime) {
        std::cout << "Setting max run time: " << NLoptStruct.NLoptStopAfterTime << std::endl;
        m_Optimizer.set_maxtime(NLoptStruct.NLoptStopAfterTime);
    }
    if (NLoptStruct.NLoptUseStopAfterIter) {
        std::cout << "Setting max num function evaluations: " << NLoptStruct.NLoptStopAfterIter << std::endl;
        m_Optimizer.set_maxeval(NLoptStruct.NLoptStopAfterIter);
    }

}


void
NLopt_Estimator::setObjectiveFunction(nmfStructsQt::ModelDataStruct& NLoptStruct,
                                      std::string& MaxOrMin)
{
    if (NLoptStruct.ObjectiveCriterion == "Least Squares") {
        MaxOrMin = "minimum";
        m_Optimizer.set_min_objective(objectiveFunction, &NLoptStruct);
    } else if (NLoptStruct.ObjectiveCriterion == "Maximum Likelihood") {
        MaxOrMin = "minimum";
        m_Optimizer.set_min_objective(objectiveFunction, &NLoptStruct);
    } else if (NLoptStruct.ObjectiveCriterion == "Model Efficiency") {
        MaxOrMin = "maximum";
        m_Optimizer.set_max_objective(objectiveFunction, &NLoptStruct);
    }
}


void
NLopt_Estimator::setSeed(const bool& isSetToDeterministic,
                         const bool& useFixedSeed)
{
    if (useFixedSeed) {
        nlopt::srand(1);
    } else {
        if (isSetToDeterministic) {
            nlopt::srand(++m_Seed);
        } else {
            nlopt::srand_time();
        }
    }
}


void
NLopt_Estimator::setParameterBounds(nmfStructsQt::ModelDataStruct& NLoptStruct,
                                    std::vector<std::pair<double,double> >& ParameterRanges,
                                    const int& NumEstParameters)
{
    // If all lower bounds = their respective upper bounds, nlopt exits with SUCCESS but the estimated
    // points are not exactly equal to the input parameter points. This may be a bug with NLopt or perhaps
    // there's a tolerance or stop condition that I'm not specifying.  As a work-around, I've found that if I
    // tweak every parameter range by the following eps value, NLopt estimates correctly. RSK 03-24-2021
    double eps = 1e-10;
    double lowerVal;
    double upperVal;
    std::vector<double> lowerBounds(NumEstParameters);
    std::vector<double> upperBounds(NumEstParameters);

    // Set parameter bounds for all parameters
    for (int i=0; i<NumEstParameters; ++i) {
        lowerVal = ParameterRanges[i].first;
        upperVal = ParameterRanges[i].second;
        lowerBounds[i] = lowerVal;
        upperBounds[i] = upperVal;
    }
    m_Optimizer.set_lower_bounds(lowerBounds);
    m_Optimizer.set_upper_bounds(upperBounds);

    // Set starting points for all parameters
    m_Parameters.clear();
    for (int i=0; i<NumEstParameters; ++i) {
        if (lowerBounds[i] == upperBounds[i]) {
            m_Parameters.push_back(lowerBounds[i]);
        } else {
            m_Parameters.push_back(lowerBounds[i] + (upperBounds[i]-lowerBounds[i])/2.0);
        }
    }
    NLoptStruct.Parameters = m_Parameters;
}

void
NLopt_Estimator::initialize(nmfStructsQt::ModelDataStruct &NLoptStruct)
{
    NLoptGrowthForm      = std::make_unique<nmfGrowthForm>(     NLoptStruct.GrowthForm);
    NLoptHarvestForm     = std::make_unique<nmfHarvestForm>(    NLoptStruct.HarvestForm);
    NLoptCompetitionForm = std::make_unique<nmfCompetitionForm>(NLoptStruct.CompetitionForm);
    NLoptPredationForm   = std::make_unique<nmfPredationForm>(  NLoptStruct.PredationForm);
}


void
NLopt_Estimator::estimateParameters(nmfStructsQt::ModelDataStruct &NLoptStruct,
                                    int& RunNumber,
                                    std::pair<bool,bool>& bools,
                                    std::vector<QString>& MultiRunLines,
                                    int& TotalIndividualRuns)
{

    bool isAMultiRun = bools.first;
    bool isSetToDeterministic = bools.second;
    bool foundOneNLoptRun = false;
    int NumEstParameters;
    int NumMultiRuns = 1;
    int NumSubRuns = 0;
    double fitnessStdDev = 0;
    std::string bestFitnessStr = "TBD";
    std::string MaxOrMin;
    std::vector<std::pair<double,double> > ParameterRanges;
    QDateTime startTime = nmfUtilsQt::getCurrentTime();

    m_NLoptFcnEvals  = 0;
    m_NumObjFcnCalls = 0;
    m_Quit           = false;
    m_RunNum        += 1;

    NumSubRuns  =  NLoptStruct.BeesNumRepetitions; // RSK fix this

    // Define forms
    NLoptGrowthForm      = std::make_unique<nmfGrowthForm>(     NLoptStruct.GrowthForm);
    NLoptHarvestForm     = std::make_unique<nmfHarvestForm>(    NLoptStruct.HarvestForm);
    NLoptCompetitionForm = std::make_unique<nmfCompetitionForm>(NLoptStruct.CompetitionForm);
    NLoptPredationForm   = std::make_unique<nmfPredationForm>(  NLoptStruct.PredationForm);

    // Load parameter ranges
    loadInitBiomassParameterRanges(           ParameterRanges, NLoptStruct);
    NLoptGrowthForm->loadParameterRanges(     ParameterRanges, NLoptStruct);
    NLoptHarvestForm->loadParameterRanges(    ParameterRanges, NLoptStruct);
    NLoptCompetitionForm->loadParameterRanges(ParameterRanges, NLoptStruct);
    NLoptPredationForm->loadParameterRanges(  ParameterRanges, NLoptStruct);
    loadSurveyQParameterRanges(               ParameterRanges, NLoptStruct);
    NumEstParameters = ParameterRanges.size();
std::cout << "*** NumEstParam: " << NumEstParameters << std::endl;
for (int i=0; i< NumEstParameters; ++i) {
 std::cout << "  " <<    ParameterRanges[i].first << ", " << ParameterRanges[i].second << std::endl;
}

        if (isAMultiRun) {
            NumMultiRuns = MultiRunLines.size();
        }

        for (int multiRun=0; multiRun<NumMultiRuns; ++multiRun) {
            NumSubRuns = 1;
            if (isAMultiRun) {
                nmfUtilsQt::reloadDataStruct(NLoptStruct,MultiRunLines[multiRun]);
                NumSubRuns = NLoptStruct.NLoptNumberOfRuns;
                NLoptGrowthForm->setType(     NLoptStruct.GrowthForm);
                NLoptHarvestForm->setType(    NLoptStruct.HarvestForm);
                NLoptCompetitionForm->setType(NLoptStruct.CompetitionForm);
                NLoptPredationForm->setType(  NLoptStruct.PredationForm);
                ParameterRanges.clear();

                loadInitBiomassParameterRanges(           ParameterRanges, NLoptStruct);
                NLoptGrowthForm->loadParameterRanges(     ParameterRanges, NLoptStruct);
                NLoptHarvestForm->loadParameterRanges(    ParameterRanges, NLoptStruct);
                NLoptCompetitionForm->loadParameterRanges(ParameterRanges, NLoptStruct);
                NLoptPredationForm->loadParameterRanges(  ParameterRanges, NLoptStruct);
                loadSurveyQParameterRanges(               ParameterRanges, NLoptStruct);
                NumEstParameters = ParameterRanges.size();
            }

            // This must follow the reloadNLoptStruct call
            if (NLoptStruct.EstimationAlgorithm != "NLopt Algorithm") {
                continue; // skip over rest of for statement and continue with next increment
            }
            m_Seed = 0;
            foundOneNLoptRun = true;
            for (int run=0; run<NumSubRuns; ++run) {
                m_Quit = false;

                if (NLoptStruct.isMohnsRho) {
                    m_MohnsRhoOffset = run;
                }

                // Initialize the optimizer with the appropriate algorithm
                m_Optimizer = nlopt::opt(m_MinimizerToEnum[NLoptStruct.MinimizerAlgorithm],NumEstParameters);

                // Set Parameter Bounds, Objective Function, and Stopping Criteria
                setSeed(isSetToDeterministic,NLoptStruct.useFixedSeedNLopt);
                setParameterBounds(NLoptStruct,ParameterRanges,NumEstParameters);
                setObjectiveFunction(NLoptStruct,MaxOrMin);
                setStoppingCriteria(NLoptStruct);

                // Run the Optimizer using the previously defined objective function
                nlopt::result result;
                try {
                    double fitness=0;
                    try {
                        // ******************************************************
                        // *
                        std::cout << "====> Running Optimizer <====" << std::endl;
                        result = m_Optimizer.optimize(m_Parameters, fitness);
                        std::cout << "Optimizer return code: " << returnCode(result) << std::endl;
                        // *
                        // ******************************************************
                    } catch (const std::exception& e) {
                        std::cout << "Exception thrown: " << e.what() << std::endl;
                        return;
                    } catch (...) {
                        std::cout << "Error: Unknown error from NLopt_Estimator::estimateParameters m_Optimizer.optimize()" << std::endl;
                        return;
                    }

                    extractParameters(NLoptStruct, &m_Parameters[0],
                            m_EstInitBiomass,
                            m_EstGrowthRates,  m_EstCarryingCapacities,
                            m_EstCatchability, m_EstAlpha,
                            m_EstBetaSpecies,  m_EstBetaGuilds, m_EstBetaGuildsGuilds,
                            m_EstPredation,    m_EstHandling,   m_EstExponent,  m_EstSurveyQ);

                    createOutputStr(m_Parameters.size(),
                                    NLoptStruct.TotalNumberParameters,
                                    NumSubRuns,
                                    fitness,fitnessStdDev,NLoptStruct,bestFitnessStr);
                    if (isAMultiRun) { // && (!isAMohnsRhoMultiRun)) {
                        // RSK -remove this and replace with logic writing est parameters to file
                        // (Having to use this with a delay is pretty ad hoc.)
                        emit SubRunCompleted(RunNumber++,
                                             TotalIndividualRuns,
                                             NLoptStruct.EstimationAlgorithm,
                                             NLoptStruct.MinimizerAlgorithm,
                                             NLoptStruct.ObjectiveCriterion,
                                             NLoptStruct.ScalingAlgorithm,
                                             NLoptStruct.MultiRunModelFilename,
                                             fitness);
                        QThread::msleep((unsigned long)(500));
                    } else {
                        emit RunCompleted(bestFitnessStr,NLoptStruct.showDiagnosticChart);
                    }

                    if (stoppedByUser()) {
                        return;
                    }
                }
                catch (nlopt::forced_stop &e) {
                    std::cout << "User terminated application: " << e.what() << std::endl;
                    return;
                }
                catch (std::exception &e) {
                    std::cout << "NLopt_Estimator::estimateParameters nlopt failed: " << e.what() << std::endl;
                    return;
                }
                catch (...) {
                    std::cout << "NLopt_Estimator::estimateParameters stopped" << std::endl;
                    return;
                }
            } // end of sub run loop

        }

//        if (isAMohnsRhoMultiRun) {
//            emit AMohnsRhoMultiRunCompleted();
//
          if (isAMultiRun && foundOneNLoptRun) {
            emit AllSubRunsCompleted();
          }

//    }

    std::string elapsedTimeStr = "Elapsed runtime: " + nmfUtilsQt::elapsedTime(startTime);
std::cout << elapsedTimeStr << std::endl;

    stopRun(elapsedTimeStr,bestFitnessStr);

}

bool
NLopt_Estimator::stoppedByUser()
{
    bool retv = false;
    std::string runName = "";
    std::string stopRunFile = nmfConstantsMSSPM::MSSPMStopRunFile;
    std::string state = "";
    std::string msg1,msg2;

    retv = nmfUtils::isStopped(runName,msg1,msg2,stopRunFile,state);

    m_Quit = true;

    return retv;
}


void
NLopt_Estimator::callback_StopAllRuns()
{
std::cout << "Stopping all runs" << std::endl;
   m_Quit = true;
   nlopt::forced_stop();
}


void
NLopt_Estimator::callback_StopTheOptimizer()
{
   m_Quit = true;
}

void
NLopt_Estimator::createOutputStr(
        const int&         numEstParameters,
        const int&         numTotalParameters,
        const int&         numSubRuns,
        const double&      bestFitness,
        const double&      fitnessStdDev,
        const nmfStructsQt::ModelDataStruct& NLoptStruct,
        std::string&       bestFitnessStr)
{
    std::string growthForm      = NLoptStruct.GrowthForm;
    std::string harvestForm     = NLoptStruct.HarvestForm;
    std::string competitionForm = NLoptStruct.CompetitionForm;
    std::string predationForm   = NLoptStruct.PredationForm;

    std::cout << "Est'd Parameters: " << numEstParameters << std::endl;
    std::cout << "Total Parameters: " << numTotalParameters << std::endl;
    std::cout << "Fitness std dev: "  << fitnessStdDev << std::endl;

    // Write to Stop file
    bestFitnessStr  = "Est'd Parameters:&nbsp;"     + std::to_string(numEstParameters);
    bestFitnessStr += "<br>Total Parameters:&nbsp;" + std::to_string(numTotalParameters);

    bestFitnessStr += "<br><br>Number of Runs:&nbsp;&nbsp;&nbsp;" + std::to_string(numSubRuns);
    bestFitnessStr += "<br>Best Fitness (SSE) value of all runs:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString::number(bestFitness,'f',2).toStdString();
    bestFitnessStr += "<br>Std dev of Best Fitness values from all runs:&nbsp;&nbsp;" + QString::number(fitnessStdDev,'f',2).toStdString();
    bestFitnessStr += "<br><br><strong>Estimated Parameters:</strong><br>";
    bestFitnessStr += convertValues1DToOutputStr("Initial Absolute Biomass:    ",m_EstInitBiomass,false);
    bestFitnessStr += convertValues1DToOutputStr("Growth Rate:          ",       m_EstGrowthRates,  false);
    if (growthForm == "Logistic") {
    bestFitnessStr += convertValues1DToOutputStr("Carrying Capacity:  ",         m_EstCarryingCapacities,true);
    }

    if (harvestForm == "Effort (qE)") {
    bestFitnessStr += convertValues1DToOutputStr("Catchability:       ",         m_EstCatchability,false);
    }
    if (competitionForm == "NO_K") {
    bestFitnessStr += convertValues2DToOutputStr("Competition (alpha):",         m_EstAlpha);
    } else if ((competitionForm == "MS-PROD") ||
               (competitionForm == "AGG-PROD"))
    {
        if (competitionForm == "MS-PROD") {
    bestFitnessStr += convertValues2DToOutputStr("Competition (beta::species):", m_EstBetaSpecies);
        }
    bestFitnessStr += convertValues2DToOutputStr("Competition (beta::guilds): ", m_EstBetaSpecies);
    }

    if ((predationForm == "Type I")  ||
        (predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
    bestFitnessStr += convertValues2DToOutputStr("Predation (rho):   ",m_EstPredation);
    }
    if ((predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
    bestFitnessStr += convertValues2DToOutputStr("Handling:          ",m_EstHandling);
    }
    if (predationForm == "Type III")
    {
    bestFitnessStr += "<br>&nbsp;&nbsp;";
    bestFitnessStr += convertValues1DToOutputStr("Predation Exponent", m_EstExponent,false);
    }
    bestFitnessStr += convertValues1DToOutputStr("SurveyQ:          ", m_EstSurveyQ, false);

}


std::string
NLopt_Estimator::convertValues1DToOutputStr(const std::string& label,
                                            const std::vector<double> &Values,
                                            const bool& includeTotal)
{
    double val;
    double totalVal = 0;
    std::string bestFitnessStr = "";

    bestFitnessStr += "<br>"+label;
    bestFitnessStr += "<table>";
    bestFitnessStr += "<tr>";
    for (unsigned i=0; i<Values.size(); ++i) {
        val = Values[i];
        bestFitnessStr += "<td> "+nmfUtils::convertToScientificNotation(val) + "</td>";
        totalVal += val;
    }
    bestFitnessStr += "</tr>";
    bestFitnessStr += "</table>";
    if (includeTotal) {
        bestFitnessStr += "<br>Total " + label + "<br>" +
                nmfUtils::convertToScientificNotation(totalVal);
    }

    return bestFitnessStr;
}


std::string
NLopt_Estimator::convertValues2DToOutputStr(const std::string& label,
                                            const boost::numeric::ublas::matrix<double> &matrix)
{
    std::string bestFitnessStr = "";

    bestFitnessStr += "<br>"+label;
    bestFitnessStr += "<table>";

    for (unsigned i=0; i<matrix.size1(); ++i) {
        bestFitnessStr += "<tr>";
        for (unsigned j=0; j<matrix.size2(); ++j) {
            bestFitnessStr += "<td> " + nmfUtils::convertToScientificNotation(matrix(i,j)) + "</td>";
        }
        bestFitnessStr += "</tr>";
    }

    bestFitnessStr += "</table>";

    return bestFitnessStr;
}


void
NLopt_Estimator::stopRun(const std::string &elapsedTimeStr,
                         const std::string &fitnessStr)
{
std::cout << "StopRun..." << std::endl;
    std::ofstream outputFile(nmfConstantsMSSPM::MSSPMStopRunFile);
    outputFile << "Stop" << std::endl;         // cmd
    outputFile << "" << std::endl;             // run name
    outputFile << elapsedTimeStr << std::endl; // elapsed time
    outputFile << fitnessStr << std::endl;     // fitness

    outputFile.close();
std::cout << "Run Stopped" << std::endl;
}

void
NLopt_Estimator::getEstInitBiomass(std::vector<double>& estInitBiomass)
{
    estInitBiomass = m_EstInitBiomass;
}

void
NLopt_Estimator::getEstGrowthRates(std::vector<double>& estGrowthRates)
{
    estGrowthRates = m_EstGrowthRates;
}

void
NLopt_Estimator::getEstCarryingCapacities(std::vector<double>& estCarryingCapacities)
{
    estCarryingCapacities = m_EstCarryingCapacities;
}

void
NLopt_Estimator::getEstCatchability(std::vector<double> &estCatchability)
{
    estCatchability = m_EstCatchability;
}

void
NLopt_Estimator::getEstSurveyQ(std::vector<double> &estSurveyQ)
{
    estSurveyQ = m_EstSurveyQ;
}

void
NLopt_Estimator::getEstCompetitionAlpha(boost::numeric::ublas::matrix<double> &estCompAlpha)
{
    estCompAlpha = m_EstAlpha;
}

void
NLopt_Estimator::getEstCompetitionBetaSpecies(boost::numeric::ublas::matrix<double> &estCompSpecies)
{
    estCompSpecies = m_EstBetaSpecies;
}

void
NLopt_Estimator::getEstCompetitionBetaGuilds(boost::numeric::ublas::matrix<double> &estCompGuilds)
{
    estCompGuilds = m_EstBetaGuilds;
}

void
NLopt_Estimator::getEstCompetitionBetaGuildsGuilds(boost::numeric::ublas::matrix<double> &estCompGuildsGuilds)
{
    estCompGuildsGuilds = m_EstBetaGuildsGuilds;
}

void
NLopt_Estimator::getEstPredation(boost::numeric::ublas::matrix<double> &estPredation)
{
    estPredation = m_EstPredation;
}

void
NLopt_Estimator::getEstHandling(boost::numeric::ublas::matrix<double> &estHandling)
{
    estHandling = m_EstHandling;
}

void
NLopt_Estimator::getEstExponent(std::vector<double> &estExponent)
{
    estExponent = m_EstExponent;
}

std::string
NLopt_Estimator::returnCode(int result)
{
    std::string retv;

    switch (result) {
        case 1:
            retv = "NLOPT_SUCCESS";
            break;
        case 2:
            retv = "NLOPT_STOPVAL_REACHED";
            break;
        case 3:
            retv = "NLOPT_FTOL_REACHED";
            break;
        case 4:
            retv = "NLOPT_XTOL_REACHED";
            break;
        case 5:
            retv = "NLOPT_MAXEVAL_REACHED";
            break;
        case 6:
            retv = "NLOPT_MAXTIME_REACHED";
            break;
        case -1:
            retv = "NLOPT_FAILURE";
            break;
        case -2:
            retv = "NLOPT_INVALID_ARGS";
            break;
        case -3:
            retv = "NLOPT_OUT_OF_MEMORY";
            break;
        case -4:
            retv = "NLOPT_ROUNDOFF_LIMITED";
            break;
        case -5:
            retv = "NLOPT_FORCED_STOP";
            break;
    }
    return retv;
}


void
NLopt_Estimator::rescaleMinMax(const boost::numeric::ublas::matrix<double> &matrix,
                                     boost::numeric::ublas::matrix<double> &rescaledMatrix)
{
    int numYears   = rescaledMatrix.size1();
    int numSpecies = rescaledMatrix.size2();
    double den;
    double minVal;
    double maxVal;
    std::vector<double> minValues(numSpecies,0);
    std::vector<double> maxValues(numSpecies,0);
    std::vector<double> tmp(numYears,0);

    // Find min,max values for each column of matrix
    for (int species=0; species<numSpecies; ++species) {
        for (int time=0; time<numYears; ++time) {
            tmp[time] = matrix(time,species);
        }
        std::sort(tmp.begin(),tmp.end());
        minValues[species] = tmp.front();
        maxValues[species] = tmp.back();
    }

    // Rescale each column of the matrix with (x - min)/(max-min) formula.
    for (int species=0; species<numSpecies; ++species) {
        minVal = minValues[species];
        maxVal = maxValues[species];
        den    = maxVal - minVal;
        for (int time=0; time<numYears; ++time) {
            rescaledMatrix(time,species) = (matrix(time,species) - minVal) / den;  // min max normalization
        }
    }

}


void
NLopt_Estimator::rescaleMean(const boost::numeric::ublas::matrix<double> &matrix,
                                   boost::numeric::ublas::matrix<double> &rescaledMatrix)
{
    int numYears   = matrix.size1();
    int numSpecies = matrix.size2();
    double den;
    double minVal;
    double maxVal;
    double avgVal;
    std::vector<double> minValues(numSpecies,0);
    std::vector<double> maxValues(numSpecies,0);
    std::vector<double> avgValues(numSpecies,0);
    std::vector<double> tmp(numYears,0);

    // Find min,max values for each column of matrix
    for (int species=0; species<numSpecies; ++species) {
        avgVal = 0;
        for (int time=0; time<numYears; ++time) {
            tmp[time] = matrix(time,species);
            avgVal += tmp[time];
        }
        avgVal /= numYears;
        std::sort(tmp.begin(),tmp.end());
        minValues[species] = tmp.front();
        maxValues[species] = tmp.back();
        avgValues[species] = avgVal;
    }

    // Rescale each column of the matrix with (x - ave)/(max-min) formula.
    for (int species=0; species<numSpecies; ++species) {
        minVal = minValues[species];
        maxVal = maxValues[species];
        den    = maxVal - minVal;
        avgVal = avgValues[species];
        for (int time=0; time<numYears; ++time) {
            rescaledMatrix(time,species) = (matrix(time,species) - avgVal) / den; // mean normalization
        }
    }
}
