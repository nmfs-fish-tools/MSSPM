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
    m_MaxFitness = 999999999;

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
                                   const double* EstParameters,
                                   std::vector<double>& initBiomass,
                                   std::vector<double>& growthRate,
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
                                   std::vector<double>& surveyQCovariateCoeffs)
{
    bool isLogistic     = (NLoptDataStruct.GrowthForm      == "Logistic");
    bool isCatchability = (NLoptDataStruct.HarvestForm     == nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
                          (NLoptDataStruct.HarvestForm     == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString());
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
    growthRateCovariateCoeffs.clear();
    carryingCapacity.clear();
    carryingCapacityCovariateCoeffs.clear();
    catchability.clear();
    catchabilityCovariateCoeffs.clear();
    competitionAlpha.clear();
    competitionBetaSpecies.clear();
    competitionBetaGuilds.clear();
    competitionBetaGuildsGuilds.clear();
    predation.clear();
    handling.clear();
    exponent.clear();
    surveyQ.clear();
    surveyQCovariateCoeffs.clear();

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

    // Always extract growth rate covariates
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        growthRateCovariateCoeffs.emplace_back(EstParameters[offset+i]);
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
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            carryingCapacityCovariateCoeffs.emplace_back(EstParameters[offset+i]);
        }
        offset += NumSpeciesOrGuilds; // 2* because of the Covariate Coeffs
    }

    if (isCatchability) {
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            catchability.emplace_back(EstParameters[offset+i]);
        }
        offset += NumSpeciesOrGuilds;
        for (int i=0; i<NumSpeciesOrGuilds; ++i) {
            catchabilityCovariateCoeffs.emplace_back(EstParameters[offset+i]);
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
    for (int i=0; i<NumSpeciesOrGuilds; ++i) {
        surveyQCovariateCoeffs.emplace_back(EstParameters[offset+i]);
    }
//    offset += NumSpeciesOrGuilds;
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
    nmfStructsQt::ModelDataStruct NLoptDataStruct = *((nmfStructsQt::ModelDataStruct *)dataPtr);
    bool isAggProd            = (NLoptDataStruct.CompetitionForm == "AGG-PROD");
//  bool isCatch              = (NLoptDataStruct.HarvestForm     == nmfConstantsMSSPM::HarvestCatch.toStdString());
    bool isEffort             = (NLoptDataStruct.HarvestForm     == nmfConstantsMSSPM::HarvestEffort.toStdString());
    bool isEffortFitToCatch   = (NLoptDataStruct.HarvestForm     == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString());
    bool isCheckedInitBiomass = nmfUtils::isEstimateParameterChecked(NLoptDataStruct,"");
    double EstBiomassTMinus1;
    double GrowthTerm;
    double HarvestTerm;
    double CompetitionTerm;
    double PredationTerm;
    double systemCarryingCapacity;
    double guildK;
    double fitness = 0;
    double initBiomassCoeff = 0.0; // Not estimated
    //double eps = 0;
    //double logEstBiomassVal = 0;
    int timeMinus1;
    int NumYears       =  NLoptDataStruct.RunLength+1 - m_MohnsRhoOffset;
    int NumSpecies     =  NLoptDataStruct.NumSpecies;
    int NumGuilds      =  NLoptDataStruct.NumGuilds;
    int DefaultFitness = (NLoptDataStruct.ObjectiveCriterion == "Model Efficiency") ? INT_MIN : INT_MAX;
    int guildNum = 0;
    int NumSpeciesOrGuilds;
    double surveyQVal;
    std::string speciesName;
    std::vector<double> initAbsBiomass;
    std::vector<double> growthRate;
    std::vector<double> growthRateCovariateCoeffs;
    std::vector<double> carryingCapacity;
    std::vector<double> carryingCapacityCovariateCoeffs;
    std::vector<double> guildCarryingCapacity;
    std::vector<double> predationExponent;
    std::vector<double> catchability;
    std::vector<double> catchabilityCovariateCoeffs;
    std::vector<double> surveyQ;
    std::vector<double> surveyQCovariateCoeffs;
    boost::numeric::ublas::matrix<double> initBiomassCovariate;
    boost::numeric::ublas::matrix<double> growthRateCovariate;
    boost::numeric::ublas::matrix<double> carryingCapacityCovariate;
    boost::numeric::ublas::matrix<double> catchabilityCovariate;
    boost::numeric::ublas::matrix<double> surveyQCovariate;
    boost::numeric::ublas::matrix<double> predationRhoCovariate;
    boost::numeric::ublas::matrix<double> predationHandlingCovariate;
    boost::numeric::ublas::matrix<double> predationExponentCovariate;
    boost::numeric::ublas::matrix<double> competitionAlphaCovariate;
    boost::numeric::ublas::matrix<double> competitionBetaSpeciesCovariate;
    boost::numeric::ublas::matrix<double> competitionBetaGuildSpeciesCovariate;
    boost::numeric::ublas::matrix<double> competitionBetaGuildGuildCovariate;
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
    boost::numeric::ublas::matrix<double> ObsCatch = NLoptDataStruct.Catch;
    boost::numeric::ublas::matrix<double> ObsCatchRescaled;
    boost::numeric::ublas::matrix<double> EstCatch; //  = qEB, for Fit to Catch calculations
    boost::numeric::ublas::matrix<double> EstCatchRescaled;
    boost::numeric::ublas::matrix<double> Effort       = NLoptDataStruct.Effort;
    boost::numeric::ublas::matrix<double> Exploitation = NLoptDataStruct.Exploitation;
    std::map<int,std::vector<int> > GuildSpecies = NLoptDataStruct.GuildSpecies;
    std::string MSSPMName = "Run " + std::to_string(m_RunNum) + "-1";
    std::string covariateAlgorithmType = NLoptDataStruct.CovariateAlgorithmType;
    std::string ForecastHarvestType = NLoptDataStruct.ForecastHarvestType;

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
    nmfUtils::initialize(ObsCatchRescaled,                    NumYears,           NumSpeciesOrGuilds);
    nmfUtils::initialize(competitionAlpha,                    NumSpeciesOrGuilds, NumSpeciesOrGuilds);
    nmfUtils::initialize(competitionBetaSpecies,              NumSpecies,         NumSpecies);
    nmfUtils::initialize(competitionBetaGuilds,               NumSpeciesOrGuilds, NumGuilds);
    nmfUtils::initialize(competitionBetaGuildsGuilds,         NumGuilds,          NumGuilds);
    nmfUtils::initialize(predationRho,                        NumSpeciesOrGuilds, NumSpeciesOrGuilds);
    nmfUtils::initialize(predationHandling,                   NumSpeciesOrGuilds, NumSpeciesOrGuilds);
    nmfUtils::initialize(EstCatch,                            NumYears,           NumSpeciesOrGuilds);
    nmfUtils::initialize(EstCatchRescaled,                    NumYears,           NumSpeciesOrGuilds);

    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"InitBiomass",                   initBiomassCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"GrowthRate",                    growthRateCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"CarryingCapacity",              carryingCapacityCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"Catchability",                  catchabilityCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"SurveyQ",                       surveyQCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"PredationRho",                  predationRhoCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"PredationHandling",             predationHandlingCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"PredationExponent",             predationExponentCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"CompetitionAlpha",              competitionAlphaCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"CompetitionBetaSpeciesSpecies", competitionBetaSpeciesCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"CompetitionBetaGuildSpecies",   competitionBetaGuildSpeciesCovariate);
    nmfUtilsQt::getCovariates(NLoptDataStruct,NumYears,"CompetitionBetaGuildGuild",     competitionBetaGuildGuildCovariate);

    extractParameters(NLoptDataStruct, EstParameters, initAbsBiomass,
                      growthRate,growthRateCovariateCoeffs,
                      carryingCapacity,carryingCapacityCovariateCoeffs,
                      catchability,catchabilityCovariateCoeffs,
                      competitionAlpha,competitionBetaSpecies,
                      competitionBetaGuilds,competitionBetaGuildsGuilds,
                      predationRho,predationHandling,predationExponent,
                      surveyQ,surveyQCovariateCoeffs);
//std::cout << "extracting cov: " << growthRateCovariateCoeffs[0] << std::endl;

    // Since we may be estimating SurveyQ, need to divide the Observed Biomass by the SurveyQ
    double surveyQTerm;
    for (int species=0; species<NumSpecies; ++species) {
        surveyQVal = surveyQ[species];
        for (int time=0; time<NumYears; ++time) {
            if (ObsBiomassBySpeciesOrGuilds(time,species) != nmfConstantsMSSPM::NoData) {
                surveyQTerm = nmfUtils::applyCovariate(nullptr,covariateAlgorithmType,surveyQVal,
                                                       surveyQCovariateCoeffs[species],
                                                       surveyQCovariate(0,species));
                ObsBiomassBySpeciesOrGuilds(time,species) /= surveyQTerm;
            }
        }
    }

    // If user has selected Effort, set ObsCatch = catchability * Effort * ObsBiomass
    if (isEffortFitToCatch || isEffort) {
        nmfUtils::initialize(ObsCatch,NumYears,NumSpeciesOrGuilds);
        for (int time=0; time<NumYears; ++time) {
            for (int species=0; species<NumSpeciesOrGuilds; ++species) {
                ObsCatch(time,species) = catchability[species]*Effort(time,species)*ObsBiomassBySpeciesOrGuilds(time,species);
            }
        }
    }

    // Set year 0's estimated biomass and catch values
    for (int species=0; species<NumSpeciesOrGuilds; ++species) {
        surveyQTerm = nmfUtils::applyCovariate(nullptr,
                    covariateAlgorithmType,surveyQ[species],
                    surveyQCovariateCoeffs[species],surveyQCovariate(0,species));
        //EstBiomassSpecies(0,species) = NLoptDataStruct.ObservedBiomassBySpecies(0,species)/surveyQTerm;
        // Always use the initial biomass for B(0) and never the estimated year=0 biomass
        EstBiomassSpecies(0,species) = initAbsBiomass[species];
//std::cout << "Comparing: " << NLoptDataStruct.ObservedBiomassBySpecies(0,species)/surveyQTerm
//          << " to " << initAbsBiomass[species] << std::endl;
        if (isEffortFitToCatch) {
            EstCatch(0,species) = catchability[species]*Effort(0,species)*EstBiomassSpecies(0,species);
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

    // RSK - Remember there's only initial guild biomass data
    // Multiply by guild surveyQ data when you have it
    for (int i=0; i<NumGuilds; ++i) {
        EstBiomassGuilds(0,i) = NLoptDataStruct.ObservedBiomassByGuilds(0,i);
    }
    if (NLoptGrowthForm == nullptr) {
        incrementObjectiveFunctionCounter(MSSPMName,-1.0,NLoptDataStruct);
        return -1;
    }

    for (int time=1; time<NumYears; ++time) {
        timeMinus1 = time - 1;
        for (int species=0; species<NumSpeciesOrGuilds; ++species) {
            speciesName = NLoptDataStruct.SpeciesNames[species];

            EstBiomassTMinus1 = EstBiomassSpecies(timeMinus1,species);

            if (isCheckedInitBiomass && timeMinus1 == 0) {
                EstBiomassTMinus1 = nmfUtils::applyCovariate(nullptr,
                            covariateAlgorithmType,EstBiomassTMinus1,
                            initBiomassCoeff,initBiomassCovariate(0,species));
            }

            GrowthTerm      = NLoptGrowthForm->evaluate(covariateAlgorithmType,
                                                        EstBiomassTMinus1,
                                                        growthRate[species],
                                                        growthRateCovariateCoeffs[species],
                                                        growthRateCovariate(timeMinus1,species),
                                                        carryingCapacity[species],
                                                        carryingCapacityCovariateCoeffs[species],
                                                        carryingCapacityCovariate(timeMinus1,species));
            HarvestTerm     = NLoptHarvestForm->evaluate(covariateAlgorithmType,
                                                         timeMinus1,species,EstBiomassTMinus1,
                                                         ObsCatch,Effort,Exploitation,
                                                         catchability[species],
                                                         catchabilityCovariateCoeffs[species],
                                                         catchabilityCovariate(timeMinus1,species));
            CompetitionTerm = NLoptCompetitionForm->evaluate(covariateAlgorithmType,
                                                             timeMinus1,species,EstBiomassTMinus1,
                                                             growthRate,
                                                             growthRateCovariate,
                                                             guildCarryingCapacity[guildNum],
                                                             systemCarryingCapacity,
                                                             EstBiomassSpecies,
                                                             EstBiomassGuilds,
                                                             competitionAlpha,
                                                             competitionAlphaCovariate,
                                                             competitionBetaSpecies,
                                                             competitionBetaSpeciesCovariate,
                                                             competitionBetaGuilds,
                                                             competitionBetaGuildSpeciesCovariate,
                                                             competitionBetaGuildsGuilds,
                                                             competitionBetaGuildGuildCovariate);
            PredationTerm   = NLoptPredationForm->evaluate(covariateAlgorithmType,
                                                           timeMinus1,species,
                                                           EstBiomassSpecies,EstBiomassTMinus1,
                                                           predationRho,
                                                           predationRhoCovariate,
                                                           predationHandling,
                                                           predationHandlingCovariate,
                                                           predationExponent,
                                                           predationExponentCovariate);

            EstBiomassTMinus1 +=  GrowthTerm - HarvestTerm - CompetitionTerm - PredationTerm;
            EstBiomassTMinus1  = (EstBiomassTMinus1 < 0) ? 0 : EstBiomassTMinus1;
            if ((EstBiomassTMinus1 < 0) || (std::isnan(std::fabs(EstBiomassTMinus1)))) {
                incrementObjectiveFunctionCounter(MSSPMName,(double)DefaultFitness,NLoptDataStruct);
                return DefaultFitness;
            }
            EstBiomassSpecies(time,species) = EstBiomassTMinus1;

            // update EstBiomassGuilds for next time step
            for (int i=0; i<NumGuilds; ++i) {
                for (unsigned j=0; j<GuildSpecies[i].size(); ++j) {
                    EstBiomassGuilds(time,i) += EstBiomassSpecies(time,GuildSpecies[i][j]);
                }
            }

            // Calculate EstCatch = qEB if need be
            if (isEffortFitToCatch) {
                EstCatch(time,species) = catchability[species]*Effort(time,species)*EstBiomassSpecies(time,species);
            }

        } // end i
    } // end time

    // Scale the data
    // Rescale the EstCatch
    std::string m_Scaling = NLoptDataStruct.ScalingAlgorithm;
    if (m_Scaling == "Min Max") {
        nmfUtils::rescaleMatrixMinMax(ObsCatch, ObsCatchRescaled);
        nmfUtils::rescaleMatrixMinMax(EstCatch, EstCatchRescaled);
        nmfUtils::rescaleMatrixMinMax(EstBiomassSpecies, EstBiomassRescaled);
        nmfUtils::rescaleMatrixMinMax(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    } else if (m_Scaling == "Mean") {
        nmfUtils::rescaleMatrixMean(ObsCatch, ObsCatchRescaled);
        nmfUtils::rescaleMatrixMean(EstCatch, EstCatchRescaled);
        nmfUtils::rescaleMatrixMean(EstBiomassSpecies, EstBiomassRescaled);
        nmfUtils::rescaleMatrixMean(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    } else {
        nmfUtils::rescaleMatrixMinMax(ObsCatch, ObsCatchRescaled);
        nmfUtils::rescaleMatrixMinMax(EstCatch, EstCatchRescaled);
        nmfUtils::rescaleMatrixMinMax(EstBiomassSpecies, EstBiomassRescaled);
        nmfUtils::rescaleMatrixMinMax(ObsBiomassBySpeciesOrGuilds, ObsBiomassBySpeciesOrGuildsRescaled);
    }

//    for (int species=0; species<NumSpecies; ++species) {
//        surveyQVal = surveyQ[species];
//        for (int time=0; time<NumYears; ++time) {
//            surveyQTerm = nmfUtils::applyCovariate(nullptr,covariateAlgorithmType,surveyQVal,
//                                                   surveyQCovariateCoeffs[species],
//                                                   surveyQCovariate(0,species));
//            EstBiomassRescaled(time,species) /= surveyQTerm;
//        }
//    }


    // Remove first row from all matrices because we don't want to include
    // the initial biomass in the fitness calculations.
    boost::numeric::ublas::matrix<double> ObsCatchTrimmed;
    boost::numeric::ublas::matrix<double> ObsBiomassTrimmed;
    boost::numeric::ublas::matrix<double> EstCatchTrimmed;
    boost::numeric::ublas::matrix<double> EstBiomassTrimmed;

    nmfUtils::removeFirstRow(ObsCatchRescaled,                   ObsCatchTrimmed);
    nmfUtils::removeFirstRow(ObsBiomassBySpeciesOrGuildsRescaled,ObsBiomassTrimmed);
    nmfUtils::removeFirstRow(EstCatchRescaled,                   EstCatchTrimmed);
    nmfUtils::removeFirstRow(EstBiomassRescaled,                 EstBiomassTrimmed);

    // Calculate fitness using the appropriate objective criterion
    if (NLoptDataStruct.ObjectiveCriterion == "Least Squares") {
        fitness =  nmfUtilsStatistics::calculateLeastSquares(
                    isEffortFitToCatch,
                    ObsCatchTrimmed, ObsBiomassTrimmed,
                    EstCatchTrimmed, EstBiomassTrimmed,
                    NLoptDataStruct.FitWeights);
    } else if (NLoptDataStruct.ObjectiveCriterion == "Model Efficiency") {
        // Negate the MEF here since the ranges is from -inf to 1, where 1 is best.  So we negate it,
        // then minimize that, and then negate and plot the resulting value.
        fitness = -nmfUtilsStatistics::calculateModelEfficiency(
                    isEffortFitToCatch,
                    ObsCatchTrimmed, ObsBiomassTrimmed,
                    EstCatchTrimmed, EstBiomassTrimmed,
                    NLoptDataStruct.FitWeights);
    } else if (NLoptDataStruct.ObjectiveCriterion == "Maximum Likelihood") {
        // The maximum likelihood calculations must use the unscaled data or else the results will be incorrect.
        fitness =  nmfUtilsStatistics::calculateMaximumLikelihoodNoRescale(
                    isEffortFitToCatch,
                    ObsCatchTrimmed, ObsBiomassTrimmed,
                    EstCatchTrimmed, EstBiomassTrimmed,
                    NLoptDataStruct.FitWeights);
    }
//if (nUnused == 0) {
//std::cout << "objcri: " << fitness << std::endl;
//}

// Debug code to print out the CarryingCapacity covariates
//if (fitness < m_MaxFitness) {
//    for (int i=0; i<70; ++i) {
//        if (i == 0) {std::cout << std::endl;}
//        if ((i>=40) and (i<= 49)) {
//            std::cout << "parameter:  " << EstParameters[i] <<
//                         ", fitness: " << fitness << std::endl;
//        }
//    }
//    m_MaxFitness = fitness;
//}


    incrementObjectiveFunctionCounter(MSSPMName,fitness,NLoptDataStruct);

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
    //  m_NLoptFcnEvals = m_Optimizer.get_numevals();

    ++m_NumObjFcnCalls;

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
        std::vector<double>& parameterInitialValues,
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
        parameterInitialValues.emplace_back(dataStruct.InitBiomass[species]);
    }
}

void
NLopt_Estimator::loadSurveyQParameterRanges(
        std::vector<double>& parameterInitialValues,
        std::vector<std::pair<double,double> >& parameterRanges,
        const nmfStructsQt::ModelDataStruct& dataStruct)
{
    bool isCheckedSurveyQ= nmfUtils::isEstimateParameterChecked(dataStruct,"SurveyQ");
    std::pair<double,double> aPair;
    std::string speciesName;
    std::map<std::string,nmfStructsQt::CovariateStruct> covariateCoeffMap;
    nmfStructsQt::CovariateStruct covariateStruct;
    double initialValue;

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
        parameterInitialValues.emplace_back(dataStruct.SurveyQ[species]);
    }

    // Always load SurveyQ Covariate Coefficient values
    for (unsigned species=0; species<dataStruct.SurveyQMin.size(); ++species) {
        speciesName       = dataStruct.SpeciesNames[species];
        covariateCoeffMap = dataStruct.SurveyQCovariateCoeff;
        covariateStruct   = covariateCoeffMap[speciesName];
        if (isCheckedSurveyQ) {
            aPair = std::make_pair(covariateStruct.CoeffMinValue,
                                   covariateStruct.CoeffMaxValue);
        } else {
            initialValue = covariateStruct.CoeffValue;
            aPair = std::make_pair(initialValue,initialValue);
        }
        parameterRanges.emplace_back(aPair);
        parameterInitialValues.emplace_back(covariateStruct.CoeffValue);
    }
}

void
NLopt_Estimator::setAdditionalParameters(const nmfStructsQt::ModelDataStruct& dataStruct)
{
   if ((dataStruct.MinimizerAlgorithm == "GN_CRS2_LM") && dataStruct.NLoptUseInitialPopulationSize) {
      m_Optimizer.set_population((unsigned int)dataStruct.NLoptInitialPopulationSize);
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
                                    std::vector<double>& ParameterInitialValues,
                                    std::vector<std::pair<double,double> >& ParameterRanges,
                                    const int& NumEstParameters)
{
    // If all lower bounds = their respective upper bounds, nlopt exits with SUCCESS but the estimated
    // points are not exactly equal to the input parameter points. This may be a bug with NLopt or perhaps
    // there's a tolerance or stop condition that I'm not specifying.  As a work-around, I've found that if I
    // tweak every parameter range by the following eps value, NLopt estimates correctly. RSK 03-24-2021
//  double eps = 1e-10;
    std::vector<double> lowerBounds(NumEstParameters);
    std::vector<double> upperBounds(NumEstParameters);

    // Set parameter bounds for all parameters
    for (int i=0; i<NumEstParameters; ++i) {
        lowerBounds[i] = ParameterRanges[i].first;
        upperBounds[i] = ParameterRanges[i].second;
    }
    m_Optimizer.set_lower_bounds(lowerBounds);
    m_Optimizer.set_upper_bounds(upperBounds);

    // Set starting points for all parameters
    m_Parameters.clear();
    for (int i=0; i<NumEstParameters; ++i) {
        if (lowerBounds[i] == upperBounds[i]) {
            m_Parameters.push_back(lowerBounds[i]);
        } else {
            // Initial value should be the user entered initial value and not always the mid-point
//          m_Parameters.push_back((lowerBounds[i] + upperBounds[i])/2.0);
            m_Parameters.push_back(ParameterInitialValues[i]);
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
    std::vector<double> ParameterInitialValues;
    std::vector<std::pair<double,double> > ParameterRanges;
    QDateTime startTime = nmfUtilsQt::getCurrentTime();

    m_NLoptFcnEvals  = 0;
    m_NumObjFcnCalls = 0;
    m_Quit           = false;
    m_RunNum        += 1;

    // Define forms
    NLoptGrowthForm      = std::make_unique<nmfGrowthForm>     (NLoptStruct.GrowthForm);
    NLoptHarvestForm     = std::make_unique<nmfHarvestForm>    (NLoptStruct.HarvestForm);
    NLoptCompetitionForm = std::make_unique<nmfCompetitionForm>(NLoptStruct.CompetitionForm);
    NLoptPredationForm   = std::make_unique<nmfPredationForm>  (NLoptStruct.PredationForm);

    // Load parameter ranges
    loadInitBiomassParameterRanges(           ParameterInitialValues, ParameterRanges, NLoptStruct);
    NLoptGrowthForm->loadParameterRanges(     ParameterInitialValues, ParameterRanges, NLoptStruct);
    NLoptHarvestForm->loadParameterRanges(    ParameterInitialValues, ParameterRanges, NLoptStruct);
    NLoptCompetitionForm->loadParameterRanges(ParameterInitialValues, ParameterRanges, NLoptStruct);
    NLoptPredationForm->loadParameterRanges(  ParameterInitialValues, ParameterRanges, NLoptStruct);
    loadSurveyQParameterRanges(               ParameterInitialValues, ParameterRanges, NLoptStruct);
    NumEstParameters = ParameterRanges.size();
std::cout << "*** NumEstParam: " << NumEstParameters << std::endl;
//for (int i=0; i< NumEstParameters; ++i) {
// std::cout << "  " << ParameterRanges[i].first << ", " << ParameterRanges[i].second << std::endl;
//}

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
                ParameterInitialValues.clear();

                loadInitBiomassParameterRanges(           ParameterInitialValues, ParameterRanges, NLoptStruct);
                NLoptGrowthForm->loadParameterRanges(     ParameterInitialValues, ParameterRanges, NLoptStruct);
                NLoptHarvestForm->loadParameterRanges(    ParameterInitialValues, ParameterRanges, NLoptStruct);
                NLoptCompetitionForm->loadParameterRanges(ParameterInitialValues, ParameterRanges, NLoptStruct);
                NLoptPredationForm->loadParameterRanges(  ParameterInitialValues, ParameterRanges, NLoptStruct);
                loadSurveyQParameterRanges(               ParameterInitialValues, ParameterRanges, NLoptStruct);
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
                setAdditionalParameters(NLoptStruct);

                // Set Parameter Bounds, Objective Function, and Stopping Criteria
                setSeed(isSetToDeterministic,NLoptStruct.useFixedSeedNLopt);
                setParameterBounds(NLoptStruct,ParameterInitialValues,ParameterRanges,NumEstParameters);
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
//for (int j=10;j<20;++j) {std::cout << "before: " << m_Parameters[j] << std::endl;}
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
//for (int j=10;j<20;++j) {std::cout << "after: " << m_Parameters[j] << std::endl;}
                    extractParameters(NLoptStruct, &m_Parameters[0],
                            m_EstInitBiomass,
                            m_EstGrowthRates,        m_EstGrowthRateCovariateCoeffs,
                            m_EstCarryingCapacities, m_EstCarryingCapacityCovariateCoeffs,
                            m_EstCatchability,       m_EstCatchabilityCovariateCoeffs,
                            m_EstAlpha,
                            m_EstBetaSpecies,
                            m_EstBetaGuilds,
                            m_EstBetaGuildsGuilds,
                            m_EstPredation,
                            m_EstHandling,
                            m_EstExponent,
                            m_EstSurveyQ,            m_EstSurveyQCovariateCoeffs);
//for (int j=0;j<(int)m_Parameters.size();++j) {std::cout << j << ": est param: " << m_Parameters[j] << std::endl;}

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
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Initial Absolute Biomass:    ",  m_EstInitBiomass,  false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Growth Rate:          ",         m_EstGrowthRates,  false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Growth Rate Covariate Coeffs: ", m_EstGrowthRateCovariateCoeffs,  false);

    if (growthForm == "Logistic") {
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Carrying Capacity:  ",                  m_EstCarryingCapacities,             true);
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Carrying Capacity Covariate Coeffs:  ", m_EstCarryingCapacityCovariateCoeffs,false);
    }

    if ((harvestForm == nmfConstantsMSSPM::HarvestEffort.toStdString()) ||
        (harvestForm == nmfConstantsMSSPM::HarvestEffortFitToCatch.toStdString())) {
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Catchability:       ",             m_EstCatchability,false);
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Catchability Covariate Coeffs:  ", m_EstCatchabilityCovariateCoeffs,false);
    }
    if (competitionForm == "NO_K") {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (alpha):",         m_EstAlpha);
    } else if ((competitionForm == "MS-PROD") ||
               (competitionForm == "AGG-PROD"))
    {
        if (competitionForm == "MS-PROD") {
            bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (beta::species):", m_EstBetaSpecies);
        }
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Competition (beta::guilds): ", m_EstBetaSpecies);
    }

    if ((predationForm == "Type I")  ||
        (predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Predation (rho):   ",m_EstPredation);
    }
    if ((predationForm == "Type II") ||
        (predationForm == "Type III"))
    {
        bestFitnessStr += nmfUtils::convertValues2DToOutputStr("Handling:",          m_EstHandling);
    }
    if (predationForm == "Type III")
    {
        bestFitnessStr += "<br>&nbsp;&nbsp;";
        bestFitnessStr += nmfUtils::convertValues1DToOutputStr("Predation Exponent", m_EstExponent,false);
    }
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("SurveyQ:",                  m_EstSurveyQ, false);
    bestFitnessStr += nmfUtils::convertValues1DToOutputStr("SurveyQ Covariate Coeffs:", m_EstSurveyQCovariateCoeffs, false);
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
NLopt_Estimator::getEstGrowthRateCovariateCoeffs(std::vector<double>& estGrowthRateCovariateCoeffs)
{
    estGrowthRateCovariateCoeffs = m_EstGrowthRateCovariateCoeffs;
}

void
NLopt_Estimator::getEstCarryingCapacities(std::vector<double>& estCarryingCapacities)
{
    estCarryingCapacities = m_EstCarryingCapacities;
}

void
NLopt_Estimator::getEstCarryingCapacityCovariateCoeffs(std::vector<double>& estCarryingCapacityCovariateCoeffs)
{
    estCarryingCapacityCovariateCoeffs = m_EstCarryingCapacityCovariateCoeffs;
}

void
NLopt_Estimator::getEstCatchability(std::vector<double> &estCatchability)
{
    estCatchability = m_EstCatchability;
}

void
NLopt_Estimator::getEstCatchabilityCovariateCoeffs(std::vector<double> &estCatchabilityCovariateCoeffs)
{
    estCatchabilityCovariateCoeffs = m_EstCatchabilityCovariateCoeffs;
}

void
NLopt_Estimator::getEstSurveyQ(std::vector<double> &estSurveyQ)
{
    estSurveyQ = m_EstSurveyQ;
}

void
NLopt_Estimator::getEstSurveyQCovariateCoeffs(std::vector<double> &estSurveyQCovariateCoeffs)
{
    estSurveyQCovariateCoeffs = m_EstSurveyQCovariateCoeffs;
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
