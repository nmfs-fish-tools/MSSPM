/**
 * @file nmfMainWindow.h
 * @brief GUI definition for the main nmfMainWindow MSSPM Application class
 *
 * This file contains the Main Window Application class. All of the menu
 * interactions are done through this class.
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

#ifndef NMFMAINWINDOW_H
#define NMFMAINWINDOW_H

#include "nmfDatabase.h"
#include "nmfLogWidget.h"
#include "nmfUtilsStatistics.h"
#include "nmfUtilsStatisticsAveraging.h"
#include "nmfUtilsStatisticsMohnsRho.h"
#include "nmfStructsQt.h"
#include "nmfUtilsQt.h"

#include "nmfChartBar.h"
#include "nmfChartLegend.h"
#include "nmfChartLine.h"
#include "nmfChartLineWithScatter.h"
#include "nmfChartScatter.h"
#include "nmfChartSurface.h"
#include "nmfProgressWidget.h"
#include "ClearOutputDialog.h"
#include "nmfDatabaseConnectDialog.h"
#include "nmfSimulatedData.h"
#include "SimulatedBiomassDialog.h"
#include "nmfMessageWithDontShowDialog.h"

#include "Bees_Estimator.h"
#include "NLopt_Estimator.h"

#include "nmfGrowthForm.h"
#include "nmfCompetitionForm.h"
#include "nmfHarvestForm.h"
#include "nmfPredationForm.h"

#include "nmfSetupTab01.h"
#include "nmfSetupTab02.h"
#include "nmfSetupTab04.h"
#include "nmfSetupTab03.h"

#include "nmfEstimationTab01.h"
#include "nmfEstimationTab02.h"
#include "nmfEstimationTab03.h"
#include "nmfEstimationTab04.h"
#include "nmfEstimationTab05.h"
#include "nmfEstimationTab06.h"
#include "nmfEstimationTab07.h"
#include "nmfEstimationTab08.h"

#include "nmfDiagnosticTab01.h"
#include "nmfDiagnosticTab02.h"

#include "nmfForecastTab01.h"
#include "nmfForecastTab02.h"
#include "nmfForecastTab03.h"
#include "nmfForecastTab04.h"

#include "nmfOutputControls.h"
#include "nmfOutputTableWidgets.h"
#include "nmfViewerWidget.h"
#include "TableNamesDialog.h"

#include <QtDataVisualization>
#include <QDateTime>
#include <QImage>
#include <QOpenGLWidget>
#include <QPixmap>
#include <QUiLoader>
#include <QGraphicsScene>



#include "REMORA_UI.h"


/**
 * @brief Struct to hold initial Species data
 */
struct InitSpeciesDataStruct {
    std::vector<double>      GrowthRate;
    std::vector<double>      GrowthRateCovarCoeff;
    std::vector<double>      GrowthRateMax;
    std::vector<double>      GrowthRateMin;
    std::vector<std::string> GuildName;
    std::vector<double>      InitBiomassMax;
    std::vector<double>      InitBiomassMin;
    std::vector<double>      SpeciesK;
    std::vector<double>      SpeciesKCovarCoeff;
    std::vector<int>         SpeciesKMax;
    std::vector<int>         SpeciesKMin;
    std::vector<std::string> SpeciesName;
    std::vector<int>         SurveyQ;
    std::vector<int>         SurveyQMin;
    std::vector<int>         SurveyQMax;
};

namespace Ui {
    class nmfMainWindow;
}


/**
 * @brief The MSSPM Main Window
 *
 * The MSSPM is a MultiSpecies Surplus Production Model application.  The user builds a desired model
 * and then estimates their desired parameters by using one of the given optimization algorithms. Abundance
 * and Biomass Forecasts, in addition to Diagnostics, may then be displayed in table or chart form.
 *
 */
class nmfMainWindow : public QMainWindow
{

    Q_OBJECT

    /*
     *  10 safe color to use: from colorbrewer2.org
     *  Red to Blue
     */
    QList<QColor> LINE_COLORS =
    {
        QColor(103,  0, 31),
        QColor(178, 24, 43),
        QColor(214, 96, 77),
        QColor(244,165,130),
        QColor(253,219,199),
        QColor(209,229,240),
        QColor(146,197,222),
        QColor( 67,147,195),
        QColor( 33,102,172),
        QColor(  5, 48, 97)
    };


private:
    Ui::nmfMainWindow*                    m_UI;

    QChart*                               m_ChartWidget;
    QChartView*                           m_ChartView2d;
    QWidget*                              m_ChartView3d;
    nmfDatabase*                          m_DatabasePtr;
    nmfStructsQt::ModelDataStruct         m_DataStruct;
    bool                                  m_ShowSignificantDigitsDlg;
    int                                   m_NumSignificantDigits;
    int                                   m_DiagnosticsFontSize;
    int                                   m_DiagnosticsNumPoints;
    int                                   m_DiagnosticsVariation;
    Bees_Estimator*                       m_Estimator_Bees;
    NLopt_Estimator*                      m_Estimator_NLopt;
    int                                   m_ForecastFontSize;
    Q3DSurface*                           m_Graph3D;
    int                                   m_isPressedBeesButton;
    int                                   m_isPressedNLoptButton;
    int                                   m_isPressedGeneticButton;
    int                                   m_isPressedGradientButton;
    bool                                  m_LoadLastProject;
    nmfLogger*                            m_Logger;
    nmfLogWidget*                         m_LogWidget;
    QStringListModel*                     m_ModelSpeciesOrGuild;
    int                                   m_NumLines;
    int                                   m_NumMohnsRhoRanges;
    std::string                           m_Password;
    QChart*                               m_ProgressChartBees;
    QTimer*                               m_ProgressChartTimer;
    QBarSeries*                           m_ProgressSeries;
    QBarSet*                              m_ProgressSet;
    nmfProgressWidget*                    m_ProgressWidget;
    QWidget*                              m_ProgressWidgetContainer;
    std::string                           m_ProjectDir;
    std::string                           m_ProjectDatabase;
    std::string                           m_ProjectName;
    std::string                           m_ModelName;
    int                                   m_RunNumBees;
    int                                   m_RunNumNLopt;
    QString                               m_RunOutputMsg;
    bool                                  m_SaveSettings;
    int                                   m_SeedValue;
    int                                   m_SetupFontSize;
    boost::numeric::ublas::matrix<double> m_SpeciesCalculatedBiomassData;
    QMap<int,QString>                     m_SpeciesTimeMap;
    std::string                           m_Username;
    std::vector<std::pair<int,int> >      m_MohnsRhoRanges;
    bool                                  m_ScreenshotOn;
    std::vector<QPixmap>                  m_Pixmaps;
    int                                   m_NumScreenShot;
    int                                   m_MShotNumRows;
    int                                   m_MShotNumCols;
    nmfViewerWidget*                      m_ViewerWidget;
    bool                                  m_isStartUpOK;
    QTableView*                           m_BiomassAbsTV;
    QTableView*                           m_BiomassRelTV;
    QTableView*                           m_FishingMortalityTV;
    QTableView*                           m_HarvestScaleFactorTV;
    bool                                  m_isRunning;
    QStringList                           m_finalList;
    QStringList                           m_orderedFinalList;
    boost::numeric::ublas::matrix<double> m_biomassMatrix;
    boost::numeric::ublas::matrix<double> m_parameterMatrix;
    int                                   m_NumRuns;
    nmfUtilsStatisticsAveraging*          m_AveragedData;
    nmfUtilsStatisticsMohnsRho*           m_MohnsRhoData;
    boost::numeric::ublas::matrix<double> m_AveBiomass;
    std::vector<boost::numeric::ublas::matrix<double> > m_OutputBiomassEnsemble;
    bool                                  m_IsMultiRun;
    std::string                           m_MultiRunType;
    QDateTime                             m_AppStartTime;
    int                                   m_DBTimeout;

    QBarSeries*              ProgressBarSeries;
    QBarSet*                 ProgressBarSet;
    QGroupBox*               SummaryGB;
    QGroupBox*               DiagnosticSummaryGB;
    QTableView*              InitBiomassTV;
    QTableView*              GrowthRateTV;
    QTableView*              CompetitionAlphaTV;
    QTableView*              CompetitionBetaSTV;
    QTableView*              CompetitionBetaGTV;
    QTableView*              PredationRhoTV;
    QTableView*              PredationHandlingTV;
    QTableView*              PredationExponentTV;
    QTableView*              SurveyQTV;
    QTableView*              CarryingCapacityTV;
    QTableView*              CatchabilityTV;
    QTableView*              BMSYTV;
    QTableView*              MSYTV;
    QTableView*              FMSYTV;
    QTableView*              SummaryTV;
    QTableView*              DiagnosticSummaryTV;
    QTableView*              OutputBiomassTV;
    QTableView*              CovariateCoefficientsTV;
    QVBoxLayout*             VChartLayt;
    QVBoxLayout*             OutputChartMainLayt;
    QWidget*                 NavigatorTreeWidget;
    QTreeWidget*             NavigatorTree;
    QDockWidget*			 MModeDockWidget;
    QDockWidget*			 MModeViewerDockWidget;

    nmfDiagnostic_Tab1*      Diagnostic_Tab1_ptr;
    nmfDiagnostic_Tab2*      Diagnostic_Tab2_ptr;
    nmfEstimation_Tab1*      Estimation_Tab1_ptr;
    nmfEstimation_Tab2*      Estimation_Tab2_ptr;
    nmfEstimation_Tab3*      Estimation_Tab3_ptr;
    nmfEstimation_Tab4*      Estimation_Tab4_ptr;
    nmfEstimation_Tab5*      Estimation_Tab5_ptr;
    nmfEstimation_Tab6*      Estimation_Tab6_ptr;
    nmfEstimation_Tab7*      Estimation_Tab7_ptr;
    nmfEstimation_Tab8*      Estimation_Tab8_ptr;
    nmfForecast_Tab1*        Forecast_Tab1_ptr;
    nmfForecast_Tab2*        Forecast_Tab2_ptr;
    nmfForecast_Tab3*        Forecast_Tab3_ptr;
    nmfForecast_Tab4*        Forecast_Tab4_ptr;
    nmfOutputControls*       Output_Controls_ptr;
    nmfOutputTableWidgets*   Output_TableWidgets_ptr;
    REMORA_UI*               Remora_ptr;
    nmfSetup_Tab1*           Setup_Tab1_ptr;
    nmfSetup_Tab2*           Setup_Tab2_ptr;
    nmfSetup_Tab3*           Setup_Tab3_ptr;
    nmfSetup_Tab4*           Setup_Tab4_ptr;
    QDialog*                 m_PreferencesDlg;
    TableNamesDialog*        m_TableNamesDlg;
    QWidget*                 m_PreferencesWidget;
	QWidget*                 m_TableNamesWidget;
    QTabWidget*              m_EstimatedParametersTW;
    std::map<QString,QTableView*> m_EstimatedParametersMap;
    nmfViewerWidget*         m_MModeViewerWidget;
    QTabWidget*              MModeViewerTBW;
    QTabWidget*              MModeViewerDataSubTab;
    QWidget*                 MModeViewerImageTB;
    QWidget*                 MModeViewerDataTB;
    QWidget*                 MModeViewerTab1;


    void adjustProgressWidget();
    bool areFieldsValid(std::string table,
                        std::vector<std::string> fields);
    bool areFieldsValid(std::string table,
                        std::string system,
                        std::vector<std::string> fields);
    void averageBiomassAndDisplay(QString& fullSpeciesPath);
    void calculateAverageBiomass();
    void checkForPreviousEstimations(bool& thereWasASingleRun,
                                     bool& thereWasAMultiRun);
    void clearOutputData(std::string algorithm,
                         std::string minimizer,
                         std::string objectiveCriterion,
                         std::string scaling);
    bool clearMonteCarloParametersTable(
            std::string& ForecastName,
            std::string  Algorithm,
            std::string  Minimizer,
            std::string  ObjectiveCriterion,
            std::string  Scaling,
            std::string& MonteCarloParametersTable);
    double convertUnitsStringToValue(QString& ScaleStr);
    void displayAverageBiomass();
    int getTabIndex(QTabWidget* tabWidget, QString tabName);
    bool   getOutputBiomassAveraged(boost::numeric::ublas::matrix<double>& AveBiomass);
    int    getNumSignificantDigits();
    int    getNumDistinctRecords(const std::string& field,
                                 const std::string& table);
    QString getFormGrowth();
    QString getFormHarvest();
    QString getFormPredation();
    QString getFormCompetition();
    void   getOutputBiomassEnsemble(
            const int& NumRows,
            const int& NumCols,
            std::vector<boost::numeric::ublas::matrix<double> >& OutputBiomassEnsemble);
    void   initializeNavigatorTree();
    void   initLogo();
    void   initPostGuiConnections();
    void   initializePreferencesDlg();
    void   initializeMMode();
    void   initializeMModeMain();
    void   initializeMModeViewer();
    bool isBeesAlgorithm();
    bool isAMultiRun();
    bool   isSufficientBiomass(int& numInsufficientSpecies);

    void   loadBiomassByGroup(QString& GroupType);
    void   updateOutputBiomassDatabaseTableWithAverageBiomass(boost::numeric::ublas::matrix<double>& AveragedBiomass);
    void   updateOutputBiomassOutputTableWithAveragedBiomass(boost::numeric::ublas::matrix<double>& AveragedBiomass);
    void   setupLogWidget();


    bool   saveSigDigState();
    void   setDatabaseWaitTime();
    void   restoreSigDigState(bool state);
    bool   runningREMORA();

    void   showDockWidgets(bool show);
    QList<QString> getTableNames(bool isExponent);

    double calculateCarryingCapacityForMSY(
            const int& SpeciesNum,
            const std::vector<double>& EstCarryingCapacities,
            const std::vector<double>& EstGrowthRates,
            const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
            const boost::numeric::ublas::matrix<double>& EstPredationRho);
    bool calculateSubRunBiomass(std::vector<double>& EstInitBiomass,
                                std::vector<double>& EstGrowthRates,
                                std::vector<double>& EstGrowthRateCovariateCoeffs,
                                std::vector<double>& EstCarryingCapacities,
                                std::vector<double>& EstCarryingCapacityCovariateCoeffs,
                                std::vector<double>& EstCatchability,
                                std::vector<double>& EstCatchabilityCovariateCoeffs,
                                std::vector<double>& EstExponent,
                                std::vector<double>& EstSurveyQ,
                                std::vector<double>& EstSurveyQCovariateCoeffs,
                                boost::numeric::ublas::matrix<double>& InitBiomassCovariates,
                                boost::numeric::ublas::matrix<double>& GrowthRateCovariates,
                                boost::numeric::ublas::matrix<double>& CarryingCapacityCovariates,
                                boost::numeric::ublas::matrix<double>& CatchabilityRatesCovariates,
                                boost::numeric::ublas::matrix<double>& SurveyQCovariates,
                                boost::numeric::ublas::matrix<double>& PredationRhoCovariate,
                                boost::numeric::ublas::matrix<double>& PredationHandlingCovariate,
                                boost::numeric::ublas::matrix<double>& PredationExponentCovariate,
                                boost::numeric::ublas::matrix<double>& CompetitionBetaAlphaCovariate,
                                boost::numeric::ublas::matrix<double>& CompetitionBetaSpeciesCovariate,
                                boost::numeric::ublas::matrix<double>& CompetitionBetaGuildSpeciesCovariate,
                                boost::numeric::ublas::matrix<double>& CompetitionBetaGuildGuildCovariate,
                                boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
                                boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
                                boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
                                boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
                                boost::numeric::ublas::matrix<double>& EstPredation,
                                boost::numeric::ublas::matrix<double>& EstHandling,
                                boost::numeric::ublas::matrix<double>& calculatedBiomass);
    double calculateMonteCarloValue(const double& uncertainty,
                                    const double& value,
                                    double& randomValue);
    bool calculateMSYValues(
            const bool& isAggProdStr,
            const int& NumLines,
            const int& NumSpeciesOrGuilds,
            const std::string& Algorithm,
            const std::string& Minimizer,
            const std::string& ObjectiveCriterion,
            const std::string& Scaling,
            const QString& TableName,
            QList<double>& BMSYValues,
            QList<double>& MSYValues,
            QList<double>& FMSYValues);
    void calculateSummaryStatistics(QStandardItemModel* smodel,
                                    const bool&         isAggProd,
                                    const std::string&  Algorithm,
                                    const std::string&  Minimizer,
                                    const std::string&  ObjectiveCriterion,
                                    const std::string&  Scaling,
                                    const int&          RunLength,
                                    const int&          NumSpecies,
                                    const bool&         isMohnsRho);
    bool calculateSummaryStatisticsStruct(
            const bool&         isAggProd,
            const std::string&  Algorithm,
            const std::string&  Minimizer,
            const std::string&  ObjectiveCriterion,
            const std::string&  Scaling,
            const int&          RunLength,
            const int&          NumSpeciesOrGuilds,
            const bool&         isMohnsRhoBool,
            const bool&         isAMultiRun,
            boost::numeric::ublas::matrix<double>& EstimatedBiomass,
            StatStruct&         statStruct);
    bool calculateSummaryStatisticsStructMohnsRho(StatStruct& statStruct);
    bool calculateSummaryStatisticsMohnsRhoBiomass(std::vector<double>& mohnsRhoEstimatedBiomass);
    void checkDiagnosticAlgorithmIdentifiersForMultiRun(
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling);
    bool checkForecastAlgorithmIdentifiersForMultiRun(
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling);
    void checkGuildRanges(
            const int& NumGuilds,
            const nmfStructsQt::ModelDataStruct& dataStruct);
    bool checkFields(std::string& table,
                     std::map<std::string, std::vector<std::string> >& dataMap,
                     std::vector<std::string>& fields);
    void checkModelHarvestTypeMatchesForecastButtonLabel(
            const QString& HarvestForm,
            const QString& ForecastButtonLabel);
    bool checkIfTablesAlreadyCreated();
    bool clearOutputBiomassTable(std::string& ForecastName,
                                 std::string  Algorithm,
                                 std::string  Minimizer,
                                 std::string  ObjectiveCriterion,
                                 std::string  Scaling,
                                 std::string& isAggProd,
                                 std::string& BiomassTable);
    void clearOutputTables();
    void closeEvent(QCloseEvent *event);
    void completeApplicationInitialization();
    QString createEstimatedFile();
    std::pair<bool,std::string> dataAdequateForCurrentModel(QStringList estParamNames);
//    bool deleteAllMohnsRho(const std::string& TableName);
//    bool deleteAllOutputMohnsRho();
    /**
     * @brief Forces user to input and save project data.  Until they do so, application
     * functionality is disabled (i.e., grayed out).
     */
    void enableApplicationFeatures(std::string navigatorGroup,
                                   bool enable);
    void enableRunWidgets(bool state);
    QString extractMatrixData(const bool& isEnabled,
                              const bool& isChecked,
                              const bool& isAMultiRun,
                              const QStringList& species,
                              const QString& label,
                              const boost::numeric::ublas::matrix<double>& matrix,
                              const char& format,
                              const int& precision);
    QString extractVectorData(const bool& isEnabled,
                              const bool& isChecked,
                              const bool& isAMultiRun,
                              const QStringList& species,
                              const QString& label,
                              const std::vector<double> &vector,
                              const char& format,
                              const int& precision);
    QTableView* findTableInFocus();
    void getAlgorithmIdentifiers(std::string& algorithm,
                                 std::string& minimizer,
                                 std::string& objectiveCriterion,
                                 std::string& scaling,
                                 std::string& competitionForm);
    QString getCurrentStyle();
    bool getForecastInitialData(
            QString& forecastName,
            int&     numYearsPerRun,
            int&     numRunsPerForecast,
            QString& growthForm,
            QString& harvestForm);
    bool getSystemDataForChart(
            int& StartYear,
            int& RunLength,
            std::string& CompetitionForm,
            std::string& PredationForm);
    bool getDiagnosticsData(
            const int   &NumPoints,
            const int   &NumSpecies,
            std::string &Algorithm,
            std::string &Minimizer,
            std::string &ObjectiveCriterion,
            std::string &Scaling,
            std::string &isAggProd,
            boost::numeric::ublas::matrix<double> &DiagnosticsValue,
            boost::numeric::ublas::matrix<double> &DiagnosticsFitness);
    std::string getFilterButtonsResult();
    bool getFinalObservedBiomass(boost::numeric::ublas::vector<double>& FinalBiomass);
    bool getForecastBiomass(const std::string &ForecastName,
                            const int   &NumSpecies,
                            const int   &RunLength,
                            std::string &Algorithm,
                            std::string &Minimizer,
                            std::string &ObjectiveCriterion,
                            std::string &Scaling,
                            std::vector<boost::numeric::ublas::matrix<double> > &ForecastBiomass);
    bool getGuildData(const int                             &NumGuilds,
                      const int                             &RunLength,
                      const QStringList                     &GuildList,
                      std::map<int,std::vector<int> >       &GuildSpecies,
                      std::vector<int>                      &GuildNum,
                      boost::numeric::ublas::matrix<double> &ObservedBiomassByGuilds);
    bool getHarvestData(const int& NumSpecies,
                        const int& RunLength,
                        const std::string& HarvestType,
                        boost::numeric::ublas::matrix<double>& Harvest);
    bool getInitialObservedBiomass(boost::numeric::ublas::vector<double>& InitBiomass);
    void getInitialYear(int& StartYear,
                        int& RunLength);
    std::string getLegendCode(bool isAveraged,
                              std::string &Algorithm,
                              std::string &Minimizer,
                              std::string &ObjectiveCriterion,
                              std::string &Scaling);
    bool getModelFormData(std::string& GrowthForm,
                          std::string& HarvestForm,
                          std::string& CompetitionForm,
                          std::string& PredationForm,
                          int&         RunLength,
                          int&         InitialYear);

    bool getMultiScenarioBiomass(
            const std::string& ScenarioName,
            int& NumSpecies,
            int& NumYears,
            const QStringList& SortedForecastLabels,
            QStringList& ForecastLabels,
            std::vector<boost::numeric::ublas::matrix<double> >& MultiScenarioBiomass);
    int getNumLines();
    int getNumMohnsRhoMultiRuns();
    boost::numeric::ublas::matrix<double> getObservedBiomassByGroup(
            const int& NumGuilds,
            const int& RunLength,
            const std::string& type);
    bool getOutputInitialBiomass(
            const std::string& ForecastName,
            QList<double> &OutputInitBiomass);
    std::string getObservedBiomassTableName(bool isPreEstimation);
    std::vector<boost::numeric::ublas::matrix<double> >
    getOutputBiomassByGroup(
            const int& RunLength,
            const std::vector<boost::numeric::ublas::matrix<double> >& OutputBiomassSpecies,
            const std::string& type);
    bool getOutputBiomass(const int &m_NumLines,
                          const int &NumSpecies,
                          const int &RunLength,
                          std::vector<std::string> &Algorithms,
                          std::vector<std::string> &Minimizers,
                          std::vector<std::string> &ObjectiveCriteria,
                          std::vector<std::string> &Scalings,
                          std::string              &isAggProd,
                          std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass);
    void getOutputParameterVector(
            const std::string& tableName,
            std::vector<double> &EstParameter);
//    void getOutputCarryingCapacity(std::vector<double> &EstCarryingCapacity, bool isMohnsRho);
    void getOutputCompetition(std::vector<double> &EstCompetition);
//    void getOutputGrowthRate(std::vector<double> &EstGrowthRate, bool isMohnsRho);
    int  getRunLength();
    bool getRunLength(int &RunLength);
    bool getSpeciesInitialData(int& NumSpecies,
                               QStringList& SpeciesList,
                               QList<double>& InitBiomass,
                               QList<double>& GrowthRate,
                               QList<double>& SpeciesK);
    bool getSpeciesWithGuilds(int& NumSpecies,
                              QStringList& SpeciesList,
                              QStringList& GuildList);
    int  getStartYearOffset();
    bool getMSYData(bool isAveraged,
                    const int&     NumLines,
                    const int&     NumGroups,
                    const std::string& Group,
                    QList<double>& BMSYValues,
                    QList<double>& MSYValues,
                    QList<double>& FMSYValues);

    /**
     * @brief Gets the formatted string describing what uncertainty
     * values were used for each Monte Carlo curve for the given species.
     * @param Species : current species whose curve is displayed
     * @param formattedUncertaintyData : list of qstrings describing the uncertainty values used to create each Monte Carlo curve
     */
    void getMonteCarloUncertaintyData(
            const QString& Species,
            QList<QString>& formattedUncertaintyData);
    bool getOutputBiomassEnsembleLineLabels(QList<QString>& lineLabels);
    void initConnections();
    void initGUIs();
    bool isAggProd();
    bool isAtLeastOneFilterPressed();
    /**
     * @brief This method returns a boolean signifying whether or not the an Estimation is currently running?
     * @return true or false
     */
    bool isEstimationRunning();
//    /**
//     * @brief This method returns a boolean signifying whether or not the current run is Mohn's Rho run?
//     * @return true or false
//     */
//    bool isMohnsRho();
    bool isSignificantDigitsEnabled();
    bool isStopped(std::string &runName,
                   std::string &msg1,
                   std::string &msg2,
                   std::string &stopRunFile,
                   std::string &state);
    bool loadCovariateAssignment(std::map<std::string,std::string>& covariateAssignment);
    bool loadCovariateData(std::map<std::string,std::vector<double> >& covariateMap);
    bool loadCovariateRanges(
            std::map<std::string,nmfStructsQt::CovariateStruct>& growthRateCovariateRanges,
            std::map<std::string,nmfStructsQt::CovariateStruct>& carryingCapacityCovariateRanges,
            std::map<std::string,nmfStructsQt::CovariateStruct>& catchabilityCovariateRanges,
            std::map<std::string,nmfStructsQt::CovariateStruct>& surveyQCovariateRanges);
    void loadGuis();
    void loadDatabase();
    bool loadHarvestCatchTables(
            const bool& isAggProd,
            const bool& isMonteCarlo,
            const std::string& ForecastName,
            const std::string& HarvestFormName,
            nmfHarvestForm* harvestFormPtr,
            std::vector<double>& HarvestRandomValues,
            std::vector<double>& HarvestUncertainty,
            std::map<QString,QString>& previousUnits,
            const int& NumSpeciesOrGuilds,
            const int& RunLength,
            boost::numeric::ublas::matrix<double>& Catch);
    bool loadHarvestEffortTables(
            const bool& isAggProd,
            const bool& isMonteCarlo,
            const std::string& ForecastName,
            const std::string& HarvestFormName,
            nmfHarvestForm* harvestFormPtr,
            std::vector<double>& HarvestRandomValues,
            std::vector<double>& HarvestUncertainty,
            std::map<QString,QString>& previousUnits,
            const int& NumSpeciesOrGuilds,
            const int& RunLength,
            boost::numeric::ublas::matrix<double>& Effort);
    bool loadHarvestEffortFitToCatchTables(
            const bool& isAggProd,
            const bool& isMonteCarlo,
            const std::string& ForecastName,
            const std::string& HarvestFormName,
            nmfHarvestForm* harvestFormPtr,
            std::vector<double>& HarvestRandomValues,
            std::vector<double>& HarvestUncertainty,
            std::map<QString,QString>& previousUnits,
            const int& NumSpeciesOrGuilds,
            const int& RunLength,
            boost::numeric::ublas::matrix<double>& Catch,
            boost::numeric::ublas::matrix<double>& Effort);
    bool loadHarvestExploitationTables(
            const bool& isAggProd,
            const bool& isMonteCarlo,
            const std::string& ForecastName,
            const std::string& HarvestFormName,
            nmfHarvestForm* harvestFormPtr,
            std::vector<double>& HarvestRandomValues,
            std::vector<double>& HarvestUncertainty,
            std::map<QString,QString>& previousUnits,
            const int& NumSpeciesOrGuilds,
            const int& RunLength,
            boost::numeric::ublas::matrix<double>& Exploitation);
    bool loadInteraction(int &NumSpecies,
                         std::string InteractionType,
                         std::string InitTable,
                         std::string MinTable,
                         std::string MaxTable,
                         std::vector<double> &MinData,
                         std::vector<double> &MaxData,
                         int &NumInteractionParameters);
    bool loadInteraction(int &NumSpecies,
                         std::string InteractionType,
                         std::string InitTable,
                         std::string MinTable,
                         std::string MaxTable,
                         std::vector<std::vector<double> > &MinData,
                         std::vector<std::vector<double> > &MaxData,
                         int &NumInteractionParameters);
    bool loadInteractionGuilds(int &NumSpecies,
                               int &NumGuilds,
                               std::string InteractionType,
                               std::map<std::string,std::string> &GuildSpeciesMap,
                               std::string InitTable,
                               std::string MinTable,
                               std::string MaxTable,
                               std::vector<std::vector<double> > &MinData,
                               std::vector<std::vector<double> > &MaxData,
                               int &NumInteractionParameters);
    bool loadInteractionGuildsGuilds(int &NumSpecies,
                                     int &NumGuilds,
                                     std::string InteractionType,
                                     std::map<std::string,std::string> &GuildSpeciesMap,
                                     std::string InitTable,
                                     std::string MinTable,
                                     std::string MaxTable,
                                     std::vector<std::vector<double> > &MinData,
                                     std::vector<std::vector<double> > &MaxData,
                                     int &NumInteractionParameters);
    bool loadParameters(nmfStructsQt::ModelDataStruct& dataStruct,
                        const bool& verbose);
    bool loadParametersMisc(nmfStructsQt::ModelDataStruct& dataStruct);
    void loadSummaryStatisticsModel(
            const int& NumSpeciesOrGuilds,
            QStandardItemModel* smodel,
            bool isMohnsRhoBool,
            StatStruct& statStruct);
    void loadVisibleTables(const bool& isAlpha,
                           const bool& isMsProd,
                           const bool& isAggProd,
                           const bool& isRho,
                           const bool& isHandling,
                           QList<QTableView*>& TableViews,
                           std::vector<std::string>& TableNames);
    bool loadUncertaintyData(const bool&          isMonteCarlo,
                             const int&           NumSpecies,
                             const std::string&   ForecastName,
                             std::string          Algorithm,
                             std::string          Minimizer,
                             std::string          ObjectiveCriterion,
                             std::string          Scaling,
                             std::vector<double>& InitBiomassUncertainty,
                             std::vector<double>& GrowthRateUncertainty,
                             std::vector<double>& CarryingCapacityUncertainty,
                             std::vector<double>& PredationUncertainty,
                             std::vector<double>& CompetitionUncertainty,
                             std::vector<double>& BetaSpeciesUncertainty,
                             std::vector<double>& BetaGuildsUncertainty,
                             std::vector<double>& BetaGuildsGuildsUncertainty,
                             std::vector<double>& HandlingUncertainty,
                             std::vector<double>& ExponentUncertainty,
                             std::vector<double>& CatchabilityUncertainty,
                             std::vector<double>& SurveyQUncertainty,
                             std::vector<double>& HarvestUncertainty,
                             std::vector<double>& GrowthRateCovCoeffUncertainty,
                             std::vector<double>& CarryingCapacityCovCoeffUncertainty,
                             std::vector<double>& CatchabilityCovCoeffUncertainty,
                             std::vector<double>& SurveyQCovCoeffUncertainty);
    bool passEstimationChecks(std::vector<QString>& MultiRunLines,
                              int& TotalIndividualRuns);
    void queryUserPreviousDatabase();
    void readSettings(QString name);
    void readSettings();
    void readSettingsGuiOrientation(bool alsoResetPosition);
    void loadAllWidgets();
    void refreshOutputTables();
//    void removeExistingMultiRuns();
    void runBeesAlgorithm(bool showDiagnosticsChart,
                          std::vector<QString>& MultiRunLines,
                          int& TotalIndividualRuns);
    void runNLoptAlgorithm(bool showDiagnosticChart,
                           std::vector<QString>& MultiRunLines,
                           int& TotalIndividualRuns);
    void saveRemoraDataFile(QString filename);
    bool saveScreenshot(QString &outputfile, QPixmap &pm);
    void saveSettings(); //bool loadWidgets=true);
    bool scaleTimeSeriesIfMonteCarlo(const bool& isMonteCarlo,
                                     const std::vector<double>&             Uncertainty,
                                     boost::numeric::ublas::matrix<double>& HarvestMatrix,
                                     std::vector<double>&                   RandomValues);
    void setCurrentOutputTab(QString outputTab);
    void setVisibilityToolbarButtons(bool isVisible);
    void setDefaultDockWidgetsVisibility();
    void setNumLines(int numLines);
    void setDiagnosticLastRunType();
    void setForecastLastRunType();
    void setPage(const QString& section,
                 const int& page);
    void setup2dChart();
    void setup3dChart();
    bool setupIsComplete();
    void setupOutputChartWidgets();
    void setupOutputEstimateParametersWidgets();
    void setupOutputModelFitSummaryWidgets();
    void setupOutputDiagnosticSummaryWidgets();
    void setupOutputScreenShotViewerWidgets();
    void setupOutputViewerWidget();
    void setOutputControlsWidth();
    void setupProgressChart();
    void showChartBiomassVsTime(
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            const int &StartYear,
//            const int &m_NumLines,
            std::vector<std::string> &Algorithms,
            std::vector<std::string> &Minimizers,
            std::vector<std::string> &ObjectiveCriteria,
            std::vector<std::string> &Scalings,
            std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass,
            boost::numeric::ublas::matrix<double> &ObservedBiomass,
            QList<double> &BiomassMSYValues,
            QString &ScaleStr,
            double &ScaleVal,
            const bool& clearChart,
            double &YMin);
    void showChartBiomassVsTimeMultiRunWithScatter(
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            const int &StartYear,
            std::vector<std::string> &Algorithms,
            std::vector<std::string> &Minimizers,
            std::vector<std::string> &ObjectiveCriteria,
            std::vector<std::string> &Scalings,
            std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass,
            boost::numeric::ublas::matrix<double> &ObservedBiomass,
            QList<double> &BiomassMSYValues,
            QString &ScaleStr,
            double &ScaleVal,
            const bool& clearChart,
            double &YMin,
            const QList<QString>& lineLabels);
    void showChartBcVsTimeSelectedSpecies(QList<int> &RowList,
                                          QList<QString> &RowNameList);
//    void showChartBcVsTimeAllSpecies(
//            std::string type,
//            const int &NumSpecies,
//            const QList<int> &Rows,
//            const int &RunLength,
//            std::map<std::string, std::vector<std::string> > &dataMapSpecies,
//            std::map<std::string, std::vector<std::string> > &dataMapCalculatedBiomass,
//            QString &ScaleStr,
//            double &ScaleVal);
//    void showChartCatchVsBc(
//            const int &NumSpecies,
//            const QString &OutputSpecies,
//            const int &SpeciesNum,
//            const int &RunLength,
//            std::map<std::string, std::vector<std::string> > &dataMapCalculatedBiomass,
//            std::map<std::string, std::vector<std::string> > &dataMapCatch,
//            QString &ScaleStr,
//            double &ScaleVal);
    bool isAMohnsRhoMultiRun();
    bool isAMultiOrMohnsRhoRun();
    void showChartTableVsTime(
            const std::string &chartType,
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            const int &StartYear,
            int &m_NumLines,
            boost::numeric::ublas::matrix<double> &Harvest,
            std::vector<boost::numeric::ublas::matrix<double> > &Biomass,  // Catch or Calculated Biomass
            QList<double> &Values,
            QString &ScaleStr,
            double &ScaleVal,
            double &YMinVal,
            double &YMaxVal);
    bool showDiagnosticsChart2d(const QString& ScaleStr,
                                const double&  ScaleVal,
                                const double&  YMinSliderVal);
    void showDiagnosticsFitnessVsParameter(
            const int&         NumPoints,
            std::string        XLabel,
            std::string        YLabel,
            const int&         NumSpecies,
            const QString&     OutputSpecies,
            const int&         SpeciesNum,
            boost::numeric::ublas::matrix<double> &DiagnosticsValue,
            boost::numeric::ublas::matrix<double> &DiagnosticsFitness,
            const double& YMinSliderVal);
    void showBiomassVsTimeForMultipleRuns(
            const std::string& ChartTitle,
            const std::string& XAxisLabel,
            const std::string& YAxisLabel,
            const bool& showHistoricalData,
            const int& StartYear,
            const int& NumObservedYears,
            const int& NumSpecies,
            const QString& OutputSpecies,
            const int& SpeciesNum,
            const int NumYears,
            std::vector<boost::numeric::ublas::matrix<double> >& Biomass,
            QString& ScaleStr,
            double& ScaleVal,
            double& YMinSliderVal,
            double& brightnessFactor,
            bool isMonteCarlo,
            bool isEnsemble,
            bool clearChart,
            QStringList ColumnLabelsForLegend);
    bool showForecastChart(const bool& isAggProd,
                           std::string forecastName,
                           const int&  startYear,
                           QString&    scaleStr,
                           double&     scaleVal,
                           double&     yMinSliderVal,
                           double&     brightnessFactor);
//    void showMohnsRhoBiomassVsTime(const std::string &label,
//                                   const int         &InitialYear,
//                                   const int         &StartYear,
//                                   const int         &NumSpecies,
//                                   const QString     &OutputSpecies,
//                                   const int         &SpeciesNum,
//                                   const int         MaxNumYears,
//                                   std::vector<boost::numeric::ublas::matrix<double> > &Biomass,
//                                   QString           &ScaleStr,
//                                   double            &ScaleVal,
//                                   double            &YMinSliderVal,
//                                   double            &brightnessFactor,
//                                   bool              useDimColor,
//                                   bool              clearChart,
//                                   QStringList       ColumnLabelsForLegend);
    void showMModeViewerDockWidget();
    bool simulateCompetition(const std::string& CompetitionForm,
                             double& competitionValue);
    bool simulateGrowth(const int& Year,
                        const int& Species,
                        const QStringList& SpeciesList,
                        const std::string& GrowthForm,
                        const QList<double>& GrowthRate,
                        const QList<double>& SpeciesK,
                        const boost::numeric::ublas::matrix<double>& SimulatedBiomass,
                        double& growthValue);
    bool simulateHarvest(const int& Year,
                         const int& Species,
                         const std::string& HarvestForm,
                         const boost::numeric::ublas::matrix<double>& HarvestData,
                         double& harvestValue);
    bool simulatePredation(const std::string& PredationForm,
                           double& competitionValue);
    void showOutputChart(bool show);
    bool thereIsOutputBiomass();
    void updateDiagnosticSummaryStatistics();
    bool updateObservedBiomassAndEstSurveyQTable(
            const QStringList& Species,
            const int& RunLength,
            const std::vector<double>& EstSurveyQ,
            std::vector<double>& EstSurveyQCovariateCoeffs);
    bool updateOutputBiomassTable(std::string& ForecastName,
                                  int&         StartYear,
                                  int&         RunLength,
                                  bool&        isMonteCarlo,
                                  int&         RunNum,
                                  std::string  Algorithm,
                                  std::string  Minimizer,
                                  std::string  ObjectiveCriterion,
                                  std::string  Scaling,
                                  std::string& isAggProdStr,
                                  std::string& GrowthForm,
                                  std::string& HarvestForm,
                                  std::string& CompetitionForm,
                                  std::string& PredationForm,
                                  std::string& InitBiomassTable,
                                  std::string& GrowthRateTable,
                                  std::string& GrowthRateCovariateCoeffsTable,
                                  std::string& CarryingCapacityTable,
                                  std::string& CarryingCapacityCovariateCoeffsTable,
                                  std::string& CatchabilityTable,
                                  std::string& CatchabilityCovariateCoeffsTable,
                                  std::string& SurveyQTable,
                                  std::string& OutputSurveyQCovariateCoeffsTable,
                                  std::string& OutputBiomassTable);
    void updateProgressChartAnnotation(double xMin, double xMax, double xInc);
    void updateOutputTables(
        std::string&                                 Algorithm,
        std::string&                                 Minimizer,
        std::string&                                 ObjectiveCriterion,
        std::string&                                 Scaling,
        const int&                                   isCompAggProd,
        const QStringList&                           SpeciesList,
        const QStringList&                           GuildList,
        const std::vector<double>&                   EstInitBiomass,
        const std::vector<double>&                   EstGrowthRates,
        const std::vector<double>&                   EstGrowthRateCovariateCoeffs,
        const std::vector<double>&                   EstCarryingCapacities,
        const std::vector<double>&                   EstCarryingCapacityCovariateCoeffs,
        const std::vector<double>&                   EstCatchability,
        const std::vector<double>&                   EstCatchabilityCovariateCoeffs,
        const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
        const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
        const boost::numeric::ublas::matrix<double>& EstPredation,
        const boost::numeric::ublas::matrix<double>& EstHandling,
        const std::vector<double>&                   EstExponent,
        const std::vector<double>&                   EstSurveyQ,
        const std::vector<double>&                   EstSurveyQCovariateCoeffs);
    void updateModelEquationSummary();
    void updateScreenShotViewer(QString filename);
    bool getSurfaceData(
            boost::numeric::ublas::matrix<double>& rowValues,
            boost::numeric::ublas::matrix<double>& columnValues,
            boost::numeric::ublas::matrix<double>& heightValues,
            int& yMin,
            int& yMax);
    QString getColorName(int line);
    bool selectMinimumSurfacePoint();
    /**
     * @brief Callback invoked to update the text in the title bar of the main window to include
     * the application name, project name, and settings file name
     */
    void updateWindowTitle();
    bool setFirstRowEstimatedBiomass(
            const std::string& ForecastName,
            const int& NumSpeciesOrGuilds,
            const boost::numeric::ublas::vector<double>& InitialBiomass,
            boost::numeric::ublas::matrix<double>& EstInitBiomassCovariates,
            boost::numeric::ublas::matrix<double>& EstimatedBiomassBySpecies);
    void setDefaultDockWidgetsVisibility(
            const QString& actionName,
            QAction* action);
//    void QueryUserForMultiRunFilenames(
//            QString& multiRunSpeciesFilename,
//            QString& multiRunModelFilename);
    void updateBiomassEnsembleTable(
            const int& RunNumber,
            const std::string& Algorithm,
            const std::string& Minimizer,
            const std::string& ObjectiveCriterion,
            const std::string& Scaling,
            const boost::numeric::ublas::matrix<double>& CalculatedBiomass);
    void updateOutputCovariateCoefficientsTable(const QStringList& SpeciesList);
    void updateOutputTableViews(
            const QStringList&                           SpeciesList,
            const QStringList&                           GuildList,
            const std::vector<double>&                   EstInitBiomass,
            const std::vector<double>&                   EstGrowthRates,
            const std::vector<double>&                   EstCarryingCapacities,
            const std::vector<double>&                   EstCatchability,
            const std::vector<double>&                   EstPredationExponent,
            const std::vector<double>&                   EstSurveyQ,
            const boost::numeric::ublas::matrix<double>& EstCompetitionAlpha,
            const boost::numeric::ublas::matrix<double>& EstCompetitionBetaSpecies,
            const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuilds,
            const boost::numeric::ublas::matrix<double>& EstCompetitionBetaGuildsGuilds,
            const boost::numeric::ublas::matrix<double>& EstPredationRho,
            const boost::numeric::ublas::matrix<double>& EstPredationHandling);

    //    bool isThereMohnsRhoData();
    //    bool loadGradientParameters(Gradient_Struct &gradientStruct);
    //    void simulateBiomass(double &growthRate,
    //                         double &carryingCapacity,
    //                         std::vector<std::vector<double> > &SavedAlpha);
    //    void runGeneticAlgorithm(std::string& OutputFile,
    //                             std::string& DataFile,
    //                             std::string& ID);
    //    bool loadParamObj(Parameters* ptr,
    //                      const QList<double> &InitialBiomassList,
    //                      const QList<QList<double> > *CatchMatrix,
    //                      const nmfStructsQt::Data_Struct &dataStruct);
    //    bool loadTimeSeriesObj(TimeSeriesObservations* timeSeriesObj,
    //                           QList<double> &InitialBiomassList,
    //                           QList<QList<double> > *CatchMatrix);
    //    bool loadModelParamObj(ModelFormParameters* modelParamObj);
    //
    //    void UpdateOutputTables_GeneticAlgorithm(int& RunLength,
    //                                             std::string& Algorithm,
    //                                             std::string& Minimizer,
    //                                             std::string& ObjectiveCriterion,
    //                                             std::string& Scaling,
    //                                             Parameters*  paramObj,
    //                                             bool&        isCompetition,
    //                                             bool&        isPredation,
    //                                             bool&        isPredationHandling);
    //  void runGradientAlgorithm(std::string &Algorithm,
    //                            std::string &Minimizer,
    //                            std::string &ObjectiveCriterion,
    //                            std::string &Scaling);


signals:
    void KeyPressed(QKeyEvent* event);
    void MouseMoved(QMouseEvent* event);
    void MouseReleased(QMouseEvent* event);

public:
    /**
     * @brief The Main MSSPM Application Window
     * @param parent : parent widget (defaults to nullptr if not specified)
     */
    explicit nmfMainWindow(QWidget *parent = nullptr);
    ~nmfMainWindow();

    /**
     * @brief Notifies main routine if MainWindow has been started correctly with MySQL active
     * @return true/false signifying the state of the application start
     */
    bool isStartUpOK();
    bool okToRunForecast();
    void saveSummaryModelFitTable(QStandardItemModel *smodel);
    void saveSummaryDiagnosticTable(QStandardItemModel *smodel,
                                    std::string& FieldNames);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief Need to prevent Remora's window from appearing in main window's so have
     * to create custom popupMenu.
     * @return Custom context menu
     *
     */
    QMenu* createPopupMenu();

public slots:
    /**
     * @brief Callback invoked when main receives an AllSubRunsCompleted signal from NLopt_Estimator
     */
    void callback_AllSubRunsCompleted();
    void callback_UpdateSeedValue(int isDeterministic);
    /**
     * @brief Callback invoked when user Runs an Estimation
     */
    void callback_CheckAllEstimationTablesAndRun();
    /**
     * @brief Callback invoked to clear all of the Estimation tables. This happens
     * if the user selects a new Project.
     */
    void callback_ClearEstimationTables();
    /**
     * @brief Callback invoked when user deletes a Model
     */
    void callback_ToDoAfterModelDelete();
    /**
     * @brief Callback invoked when user selects a tab from the Diagnostics tab group
     * @param tab : Diagnostics tab selected
     */
    void callback_DiagnosticsTabChanged(int tab);
    /**
     * @brief Callback invoked to set the state of the toolbar's filter buttons
     * @param state : state to set the toolbar filter buttons to
     */
    void callback_EnableFilterButtons(bool state);
    /**
     * @brief Callback invoked when user selects a tab from the Estimation tab group
     * @param tab : Estimation tab selected
     */
    void callback_EstimationTabChanged(int tab);
    /**
     * @brief Callback invoked when user changes the Output Controls line brightness widget
     * @param brightnessFactor : the new brightness factor value
     */
    void callback_ForecastLineBrightnessChanged(double brightnessFactor);
    /**
     * @brief Callback invoked when user loads a new Forecast so as appropriate GUI widgets are auto filled
     * @param ForecastName : name of Forecast loaded
     */
    void callback_ForecastLoaded(std::string ForecastName);
    /**
     * @brief Callback invoked when user selects a tab from the Forecast tab group
     * @param tab : Forecast tab selected
     */
    void callback_ForecastTabChanged(int tab);
//  void callback_Hovered(bool status, int index, QBarSet *barset);
    /**
     * @brief Initializes class variables for the start of a multi-run run
     * @param multiRunModelFilename : name of Multi-Run Model file
     * @param totalIndividualRuns : total number of runs
     */
    void callback_InitializeSubRuns(
            std::string multiRunModelFilename,
            int totalIndividualRuns);
    /**
     * @brief Callback invoked when user loads a new database
     * @param databaseName : name of database to load
     */
    void callback_LoadDatabase(QString databaseName);
    /**
     * @brief Callback invoked when user needs to load parameters for a Diagnostics run
     */
    void callback_LoadDataStruct();
    /**
     * @brief Callback invoked to set the proper state of the GUI after a new Project has been loaded
     */
    void callback_LoadProject();
    /**
     * @brief Callback invoked when user wants to give the Estimation Tab1 class Species Guild
     * information from Setup Tab3.
     */
    void callback_LoadSpeciesGuild();
    /**
     * @brief Callback invoked when user selects an item from the Navigator list
     */
    void callback_NavigatorSelectionChanged();
//    /**
//     * @brief Callback invoked when application realizes no System file has been set
//     */
//    void callback_NoSystemsSet();
    /**
     * @brief Callback invoked when a Forecast has run
     * @param Type : type of Output Chart desired
     * @param SortedForecastLabelsMap : map of forecast labels used for annotation
     */
    void callback_OutputTypeCMB(
            QString Type,
            std::map<QString,QStringList> SortedForecastLabelsMap);
    /**
     * @brief Callback invoked when a user has saved new project settings
     */
    void callback_ProjectSaved();
    void callback_AddedNewDatabase();
    /**
     * @brief Callback invoked when the progress chart timer times out. In this fashion,
     * the progress chart is updated while another process is running.
     */
    void callback_ReadProgressChartDataFile();
    /**
     * @brief Callback invoked when the progress chart timer times out. In this fashion,
     * the progress chart is updated while another process is running.
     * @param validPointsOnly : boolean signifying whether all points or only valid points should
     * be displayed. Valid points are those points not including the 99999 out of range points.
     * @param clearChart : boolean signifying if chart should be cleared before redrawing
     */
    void callback_ReadProgressChartDataFile(bool validPointsOnly, bool clearChart);
    /**
     * @brief Callback invoked when user needs to reset the Output Controls widgets
     */
    void callback_RefreshOutput();
    /**
     * @brief Callback invoked when user needs to reload the Main application's widgets
     */
    void callback_ReloadWidgets();
    /**
     * @brief Callback invoked when user needs to reset the toolbar's filter buttons
     */
    void callback_ResetFilterButtons();
    /**
     * @brief Callback invoked when user wants to reset the current species in the
     * Output widget to what it was just prior to modifying the Population Parameters
     */
    void callback_RestoreOutputSpecies();
    /**
     * @brief Callback invoked when Estimation run has completed
     * @param outputMsg : output message for Estimation run completion
     * @param showDiagnosticChart :
     */
    void callback_RunCompleted(std::string outputMsg,
                               bool showDiagnosticChart);

    void callback_SubRunCompleted(int run,
                                  int numRuns,
                                  std::string EstimationAlgorithm,
                                  std::string MinimizerAlgorithm,
                                  std::string ObjectiveCriterion,
                                  std::string ScalingAlgorithm,
                                  std::string multiRunModelFilename,
                                  double fitness);
    /**
     * @brief Callback invoked when user runs Estimations as part of a Retrospective Analysis Diagnostics run
     * @param ranges : year ranges for current run
     */
    void callback_RunRetrospectiveAnalysisEstimation(std::vector<std::pair<int,int> > ranges);
    void callback_RunRetrospectiveAnalysisEstimationMultiRun(std::vector<std::pair<int,int> > ranges);
    /**
     * @brief Callback invoked with user runs an Estimation
     * @param showDiagnosticsChart : boolean that when true will show the Diagnostics Chart in the OutputDockWidget
     */
    void callback_RunEstimation(bool showDiagnosticsChart);
    /**
     * @brief Callback invoked when user runs a Forecast
     * @param ForecastName : name of Forecast to run
     * @param GenerateBiomass : boolean used to specify whether or not user wants to
     * update the Output Biomass table with the Forecast data
     */
    void callback_RunForecast(std::string ForecastName, bool GenerateBiomass);
    /**
     * @brief Callback invoked when user wants to save the Qt Settings for the Main application page
     */
    void callback_SaveMainSettings();
    /**
     * @brief Callback invoked when user wants to save the output biomass data when generating a forecast
     * @param ForecastName : name of Forecast that's currently being run
     */
    void callback_SaveForecastOutputBiomassData(std::string ForecastName);
    /**
     * @brief Sets the Forecast Harvest Model pointer in the Forecast Tab2 pane
     */
    void callback_SetForecastHarvestModel();
    /**
     * @brief Callback invoked when user saves a new Model
     */
    void callback_ToDoAfterModelSave();
    /**
     * @brief Callback invoked when user changes the type of Output chart desired
     * @param type : type of Output chart to view
     * @param method : diagnostic method if chart type is Diagnostics
     */
    void callback_SetChartType(std::string type,std::string method);
    /**
     * @brief Callback invoked to toggle between a 2d and 3d chart
     * @param setTo2d : Boolean used to set the chart type to 2d (True) or 3d (False)
     */
    void callback_SetChartView2d(bool setTo2d);
    /**
     * @brief Callback invoked when user changes the Competition form so that the Model Equation is updated
     * @param type : newly selected Competition form
     */
    void callback_Setup_Tab4_CompetitionFormCMB(QString type);
    /**
     * @brief Callback invoked when user changes the Growth form so that the Model Equation is updated
     * @param type : newly selected Growth form
     */
    void callback_Setup_Tab4_GrowthFormCMB(QString type);
    /**
     * @brief Callback invoked when user changes the Harvest form so that the Model Equation is updated
     * @param type : newly selected Harvest form
     */
    void callback_Setup_Tab4_HarvestFormCMB(QString type);
    /**
     * @brief Callback invoked when user selects a new Model Preset so that the Model Equation is updated
     * @param type : newly selected Model Preset
     */
    void callback_Setup_Tab4_ModelPresetsCMB(QString type);
    /**
     * @brief Callback invoked when user changes the Predation form so that the Model Equation is updated
     * @param type : newly selected Predation form
     */
    void callback_Setup_Tab4_PredationFormCMB(QString type);
    /**
     * @brief Callback invoked when user selects a tab on the UI's SetupInputTabWidget
     * @param tab : tab selected by the user
     */
    void callback_SetupTabChanged(int tab);
    /**
     * @brief Callback invoked when user wants to update the Output chart
     * @param outputType : type of chart to display
     * @param outputSpecies : Species whose data to show in the chart
     * @return Boolean describing a successful display (True) or error getting supporting data (False)
     */
    bool callback_ShowChart(QString outputType,
                            QString outputSpecies);
    /**
     * @brief Callback invoked when user selects a Retrospective Analysis chart to view
     */
    void callback_ShowChartMohnsRho();
    /**
     * @brief Callback invoked when user selects a Scenario chart to view
     * @param SortedForecastLabels : list of Forecast labels to show on the plot
     * @return Boolean signifying a succesful data fetch and display or unsuccesful (True/False)
     */
    bool callback_ShowChartMultiScenario(QStringList SortedForecastLabels);
    /**
     * @brief Callback invoked when user modifies the Population Parameters with the modifier slider
     * @return true
     */
    bool callback_ShowDiagnostics();
    /**
     * @brief Callback invoked when user enables the 3d Diagnostics chart functionality
     * @return true
     */
    bool callback_ShowDiagnosticsChart3d();
    /**
     * @brief Callback invoked when user completes an Estimation run and needs to update the Run Summary text box
     * @param fontName : name of font to use in the Run Summary text edit box
     */
    void callback_ShowRunMessage(QString fontName);
    /**
     * @brief Callback invoked when user presses the Select Center Point for the 3d Diagnostics plot
     */
    void callback_SelectCenterSurfacePoint();
    /**
     * @brief Callback invoked when user presses the Select Minimum Point for the 3d Diagnostics plot
     */
    void callback_SelectMinimumSurfacePoint();
    /**
     * @brief Callback invoked to set the Output Scenario name from the Output Controls GUI
     */
    void callback_SetOutputScenarioForecast();
    /**
     * @brief Callback invoked when the user wants to change
     * @param state : checked state of the checkbox
     */
    void callback_PreferencesSigDigCB(int state);
    /**
     * @brief Callback invoked when user changes the application style from the Preferences dialog
     * @param style : style of application (Light or Dark)
     */
    void callback_PreferencesSetStyleSheet(QString style);
    /**
     * @brief Callback invoked when user changes the chart group type
     * @param groupType : type of chart grouping desired: species, guild, system
     * @param isAveraged : true if viewing averaged data, false otherwise
     */
    void callback_ShowChartBy(QString groupType,
                              bool isAveraged);
    /**
     * @brief Callback invoked when user is modifying the Population Parameters and needs to
     * store the current value of the Output widget's species
     */
    void callback_StoreOutputSpecies();
    /**
     * @brief Callback invoked when a Bees Estimation Algorithm sub run has completed
     * @param RunNumber : number of run
     * @param SubRunNum : number of sub run
     * @param NumSubRuns : total number of sub runs
     */
    void callback_RepetitionRunCompleted(
            int RunNumber,
            int SubRunNum,
            int NumSubRuns);
    /**
     * @brief Callback invoked to stop the progress widget's elapsed run timer
     */
    void callback_StopTheTimer();
    /**
     * @brief Callback invoked when user loads a System from the Setup Page 4 GUI
     */
    void callback_ModelLoaded();
//  void callback_UpdateProgressData(int SpeciesNum,int NumParams,QString elapsedTime);
    /**
     * @brief Callback invoked when the run has completed and user wants to update the Run Statistics
     */
    void callback_UpdateSummaryStatistics();
    /**
     * @brief Callback invoked to update the Model Equation in the Setup page 4 summary text box
     */
    void callback_UpdateModelEquationSummary();
    void callback_openCSVFile(QPoint pos);
    void context_Action(bool triggered);


    /**
     * @brief Raises an About MSSPM Dialog
     *
     * Raises an information About dialog that shows the MSSPM version number, all libraries used
     * along with their version number, and links to those libraries' web pages.
     */
    void menu_about();
    /**
     * @brief Clears the selected table cells
     */
    void menu_clear();
    /**
     * @brief Clears all the table cells
     */
    void menu_clearAll();
    /**
     * @brief Clears all output data from the respective tables
     */
    void menu_clearOutputData();
    /**
     * @brief Clears output data from the output tables specified by the user
     */
    void menu_clearSpecificOutputData();
    /**
     * @brief Copies the selected table cells
     */
    void menu_copy();
    /**
     * @brief Creates a simulated Biomass time series with the current model settings
     */
    void menu_createSimulatedBiomass();
    /**
     * @brief Creates all necessary MySQL tables
     */
    void menu_createTables();
    /**
     * @brief Deselects all cells in the table
     */
    void menu_deselectAll();
    /**
     * @brief Exports the current database to a .sql disk file
     */
    void menu_exportDatabase();
	/**
     * @brief Export all databases to individual .sql disk files
     */
    void menu_exportAllDatabases();
    /**
     * @brief Imports a .sql disk file and loads it as the current database
     */
    void menu_importDatabase();
    /**
     * @brief Change the Application layout to the default
     */
    void menu_layoutDefault();
    /**
     * @brief Change the Application layout to one with the Output window torn off and placed to the side
     */
    void menu_layoutOutput();
    /**
     * @brief Open the CSV file that's associated with the current image file displayed in the REMORA viewer
     */
    void menu_openCSVFile();
    /**
     * @brief Pastes the previously copied or cleared table cells
     */
    void menu_paste();
    /**
     * @brief Pastes the selected cell's content to all cells
     */
    void menu_pasteAll();
    /**
     * @brief Raises the Preferences dialog
     */
    void menu_preferences();
    /**
     * @brief Quits the Application
     */
    void menu_quit();
    /**
     * @brief Resets the application cursor to the arrow cursor
     */
    void menu_resetCursor();
    /**
     * @brief Runs the model. Activated from the Run toolbar button or ctrl+r
     */
    void menu_runModel();
    /**
     * @brief Save all data generated by current run and display charts
     */
    void menu_saveAndShowCurrentRun();
    /**
     * @brief Save all data generated by current run and display charts
     * @param showDiagnosticChart : boolean to not change chart type
     */
    void menu_saveAndShowCurrentRun(bool showDiagnosticChart);
    /**
     * @brief Save all data generated by current run
     */
    void menu_saveCurrentRun();
    /**
     * @brief Save application settings
     */
    void menu_saveSettings();
    /**
     * @brief Puts the screen shot functionality in multi shot mode
     */
    void menu_screenMultiShot();
    /**
     * @brief Take a screen shot of the current image displayed
     */
    void menu_screenShot();
    /**
     * @brief Take a screen shot of all of the Species' Biomass plots composited into a single image
     */
    void menu_screenShotAll();
    /**
     * @brief Selects all of the table's cells
     */
    void menu_selectAll();
    /**
     * @brief Sets internal variable to the BE toolbar button's pressed state
     * @param toggle : Describes whether the BE toolbar button has been pressed
     */
    void menu_setBees(bool toggle);
    /**
     * @brief Sets internal variable to the NL toolbar button's pressed state
     * @param toggle : Describes whether the NL toolbar button has been pressed
     */
    void menu_setNLopt(bool toggle);
    /**
     * @brief Show chart depicting all runs (i.e., multiple runs on same chart)
     */
    void menu_showAllSavedRuns();
    /**
     * @brief Show chart from current run
     */
    void menu_showCurrentRun();
    /**
     * @brief Raises a dialog that lists all the MySQL database tables for MSSPM
     */
    void menu_showTableNames();
    /**
     * @brief Interrupt and stop the current run
     */
    void menu_stopRun();
    /**
     * @brief Puts application in What's This mode
     *
     * WhatsThis mode changes the cursor to a question mark with an arrow.  When this cursor
     * is hovered over a widget that has WhatsThis data added to it, that data is displayed.
     * This functionality allows the user to enter longer pop-up information that what can be
     * neatly displayed in a tool tip.
     */
    void menu_whatsThis();
    void menu_toggleManagerMode();
    void menu_toggleManagerModeViewer();
    void menu_toggleSignificantDigits();


    void callback_PreferencesOkPB();
    void callback_ErrorFound(std::string errorMsg);
    void callback_ManagerModeViewerClose(bool state);
    void callback_AddToReview();
    void callback_LoadFromModelReview(nmfStructsQt::ModelReviewStruct modeReview);
    void callback_EnableRunButtons(bool state);
    void callback_StopAllRuns();
    void callback_SummaryLoadDiagnosticPB();
    void callback_SummaryExportDiagnosticPB();
    void callback_SummaryImportDiagnosticPB();
    void callback_SummaryLoadPB();
    void callback_SummaryExportPB();
    void callback_SummaryImportPB();
    void callback_UpdateUncertaintyTable();
    void callback_AMohnsRhoMultiRunCompleted();
//  /**
//   * @brief Copy TestData into OutputGrowthRate
//   */
//  void menu_resetGrowthRate();
//  /**
//   * @brief Copy TestData into OutputGrowthRate
//   */
//  void menu_resetCarryingCapacity();
//    /**
//     * @brief Copy TestCompetition into OutputCompetitionAlpha
//     */
//  void menu_resetCompetition();
//  void menu_clearCompetition();
//  void menu_setGenetic(bool toggle);
//  void menu_setGradient(bool toggle);
//  void menu_generateObservedBiomass(std::string &Type);
//  void menu_generateLinearObservedBiomass();
//  void menu_generateLogisticObservedBiomass();
//  void menu_generateLogisticMultiSpeciesObservedBiomass();


};

#endif // NMFMAINWINDOW_H
