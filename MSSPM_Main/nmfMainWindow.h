#ifndef NMFMAINWINDOW_H
#define NMFMAINWINDOW_H

#include <nlopt.hpp>

#include "nmfDatabase.h"
#include "nmfLogWidget.h"
#include "nmfUtilsStatistics.h"
#include "nmfStructsQt.h"
#include "nmfUtilsQt.h"

#include "nmfChartBar.h"
#include "nmfChartLine.h"
#include "nmfChartLineWithScatter.h"
#include "nmfChartScatter.h"
#include "nmfProgressWidget.h"
#include "ClearOutputDialog.h"
#include "PreferencesDialog.h"
#include "nmfDatabaseConnectDialog.h"
#include "nmfOutputChart3DBarModifier.h"
#include "nmfOutputChart3D.h"

//#include "LogisticMultiSpeciesDialog.h"
//#include "Parameters.h"
//#include "ModelFormParameters.h"
//#include "TimeSeriesObservations.h"

//#include "GA_Estimator.h"
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

#include "nmfDiagnosticTab01.h"
#include "nmfDiagnosticTab02.h"

#include "nmfForecastTab01.h"
#include "nmfForecastTab02.h"
#include "nmfForecastTab03.h"
#include "nmfForecastTab04.h"

#include "nmfOutputControls.h"

//class Gradient_Estimator;

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
    Ui::nmfMainWindow* m_UI;

    QChart*                               m_ChartWidget;
    QChartView*                           m_ChartView2d;
    QWidget*                              m_ChartView3d;
    nmfDatabase*                          m_DatabasePtr;
    Data_Struct                           m_DataStruct;
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
    nmfOutputChart3DBarModifier*          m_Modifier;
    std::string                           m_MohnsRhoLabel;
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
    std::string                           m_ProjectSettingsConfig;
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

    QBarSeries*              ProgressBarSeries;
    QBarSet*                 ProgressBarSet;
    QTableView*              GrowthRateTV;
    QTableView*              CompetitionAlphaTV;
    QTableView*              CompetitionBetaSTV;
    QTableView*              CompetitionBetaGTV;
    QTableView*              PredationTV;
    QTableView*              HandlingTV;
    QTableView*              ExponentTV;
    QTableView*              CarryingCapacityTV;
    QTableView*              BMSYTV;
    QTableView*              MSYTV;
    QTableView*              FMSYTV;
    QTableView*              SummaryTV;
    QTableView*              DiagnosticSummaryTV;
    QTableView*              OutputBiomassTV;
    QTableView*              OutputBiomassMSSPMTV;
    QVBoxLayout*             ProgressMainLayt;
    QVBoxLayout*             VChartLayt;
    QVBoxLayout*             OutputChartMainLayt;
    QSurfaceDataProxy*       SurfaceProxy;
    QSurface3DSeries*        SurfaceSeries;
    QWidget*                 NavigatorTreeWidget;
    QTreeWidget*             NavigatorTree;

    nmfDiagnostic_Tab1*      Diagnostic_Tab1_ptr;
    nmfDiagnostic_Tab2*      Diagnostic_Tab2_ptr;
    nmfEstimation_Tab1*      Estimation_Tab1_ptr;
    nmfEstimation_Tab2*      Estimation_Tab2_ptr;
    nmfEstimation_Tab3*      Estimation_Tab3_ptr;
    nmfEstimation_Tab4*      Estimation_Tab4_ptr;
    nmfEstimation_Tab5*      Estimation_Tab5_ptr;
    nmfEstimation_Tab6*      Estimation_Tab6_ptr;
    nmfForecast_Tab1*        Forecast_Tab1_ptr;
    nmfForecast_Tab2*        Forecast_Tab2_ptr;
    nmfForecast_Tab3*        Forecast_Tab3_ptr;
    nmfForecast_Tab4*        Forecast_Tab4_ptr;
    MSSPM_GuiOutputControls* Output_Controls_ptr;
    nmfSetup_Tab1*           Setup_Tab1_ptr;
    nmfSetup_Tab2*           Setup_Tab2_ptr;
    nmfSetup_Tab3*           Setup_Tab3_ptr;
    nmfSetup_Tab4*           Setup_Tab4_ptr;

//  Gradient_Struct             gradientStruct;
//  int                         RunNumGenetic;
//  int                         RunNumGradient;
//  Gradient_Estimator*         gradient_Estimator;
//  Parameters*                 paramObj;
//  ModelFormParameters*        modelParamObj;
//  nmfSimulation_Tab1*         Simulation_Tab1_ptr;
//  nmfSimulation_Tab2*         Simulation_Tab2_ptr;
//  nmfSimulation_Tab3*         Simulation_Tab3_ptr;
//  nmfSimulation_Tab4*         Simulation_Tab4_ptr;
//  nmfSimulation_Tab5*         Simulation_Tab5_ptr;
//  nmfSimulation_Tab6*         Simulation_Tab6_ptr;
//  LogisticMultiSpeciesDialog* LogisticMultiSpeciesDlg;

    void   clearOutputData(std::string algorithm,
                           std::string minimizer,
                           std::string objectiveCriterion,
                           std::string scaling);
    double convertUnitsStringToValue(QString& ScaleStr);
    int    getNumDistinctRecords(const std::string& field,
                                 const std::string& table);
    void   setupLogWidget();
    void   initializeNavigatorTree();
    void   initLogo();
    void   initPostGuiConnections();
    void   showDockWidgets(bool show);

public:
    /**
     * @brief The Main MSSPM Application Window
     * @param parent : parent widget (defaults to 0 if not specified)
     */
    explicit nmfMainWindow(QWidget *parent = 0);
    ~nmfMainWindow();

    void addDataToSurface();
    void adjustProgressWidget();
    double calculateMonteCarloValue(const double& uncertainty,
                                    const double& value);
    void calculateSummaryStatistics(QStandardItemModel *smodel,
                                    const bool        &isAggProd,
                                    const std::string &Algorithm,
                                    const std::string &Minimizer,
                                    const std::string &ObjectiveCriterion,
                                    const std::string &Scaling,
                                    const int         &RunLength,
                                    const int         &NumSpecies,
                                    const bool        &isMohnsRho);
    bool calculateSummaryStatisticsMohnsRhoBiomass(std::vector<double>& mohnsRhoEstimatedBiomass);
    bool checkIfTablesAlreadyCreated();
    bool clearOutputBiomassTable(std::string& ForecastName,
                                 std::string& Algorithm,
                                 std::string& Minimizer,
                                 std::string& ObjectiveCriterion,
                                 std::string& Scaling,
                                 std::string& isAggProd,
                                 std::string& BiomassTable);
    void clearOutputTables();
    void closeEvent(QCloseEvent *event);
    void completeApplicationInitialization();
    bool deleteAllMohnsRho(const std::string& TableName);
    bool deleteAllOutputMohnsRho();
    QTableView* findTableInFocus();
    void getAlgorithmIdentifiers(std::string& algorithm,
                                 std::string& minimizer,
                                 std::string& objectiveCriterion,
                                 std::string& scaling,
                                 std::string& competitionForm);
    QString getCurrentStyle();
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
    bool getFinalObservedBiomass(QList<double> &FinalBiomass);
    bool getForecastBiomass(const std::string &ForecastName,
                            const int   &NumSpecies,
                            const int   &RunLength,
                            std::string &Algorithm,
                            std::string &Minimizer,
                            std::string &ObjectiveCriterion,
                            std::string &Scaling,
                            std::vector<boost::numeric::ublas::matrix<double> > &ForecastBiomass);
    bool getForecastBiomassMonteCarlo(const std::string& ForecastName,
                                      const int&         NumSpecies,
                                      const int&         RunLength,
                                      const int&         NumRuns,
                                      std::string&       Algorithm,
                                      std::string&       Minimizer,
                                      std::string&       ObjectiveCriterion,
                                      std::string&       Scaling,
                                      std::vector<boost::numeric::ublas::matrix<double> >& ForecastBiomassMonteCarlo);
    bool getGuildData(const int                             &NumGuilds,
                      const int                             &RunLength,
                      const QStringList                     &GuildList,
                      std::map<int,std::vector<int> >       &GuildSpecies,
                      std::vector<int>                      &GuildNum,
                      boost::numeric::ublas::matrix<double> &ObservedBiomassByGuilds);
    bool getGuilds(int &NumGuilds, QStringList &GuildList);
    bool getInitialObservedBiomass(QList<double> &InitBiomass);
    bool getInitialSpeciesData(int &NumSpecies,
                            InitSpeciesDataStruct &InitSpeciesData);

    void getInitialYear(int& InitialYear,
                        int& MaxNumYears);
    std::string getLegendCode(std::string &Algorithm,
                              std::string &Minimizer,
                              std::string &ObjectiveCriterion,
                              std::string &Scaling);
    bool getModelFormData(std::string& GrowthForm,
                          std::string& HarvestForm,
                          std::string& CompetitionForm,
                          std::string& PredationForm,
                          int&         RunLength,
                          int&         InitialYear);
    bool getMohnsRhoBiomass(
            const std::string& ScenarioName,
            int& NumSpecies,
            QStringList& ColumnLabelsForLegend,
            std::vector<boost::numeric::ublas::matrix<double> >& BiomassMohnsRho);
    std::string getMohnsRhoLabel(const int& index);
    void getMohnsRhoLabelsToDelete(const int& NumMohnsRhos,
                                   std::string &mohnsRhoLabelsToDelete);
    bool getMultiScenarioBiomass(
            const std::string& ScenarioName,
            int& NumSpecies,
            int& NumYears,
            const QStringList& SortedForecastLabels,
            QStringList& ForecastLabels,
            std::vector<boost::numeric::ublas::matrix<double> >& MultiScenarioBiomass);
    int getNumLines();

    bool getOutputBiomass(const int &m_NumLines, const int &NumSpecies, const int &RunLength,
                          std::vector<std::string> &Algorithms,
                          std::vector<std::string> &Minimizers,
                          std::vector<std::string> &ObjectiveCriteria,
                          std::vector<std::string> &Scalings,
                          std::string              &isAggProd,
                          std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass);
    void getOutputCarryingCapacity(std::vector<double> &EstCarryingCapacity, bool isMohnsRho);
    void getOutputCompetition(std::vector<double> &EstCompetition);
    void getOutputGrowthRate(std::vector<double> &EstGrowthRate, bool isMohnsRho);
    int  getRunLength();
    bool getRunLength(int &RunLength);
    bool getSpecies(int &NumSpecies, QStringList &SpeciesList);
    void getSpeciesGuildMap(std::map<std::string,std::string>& SpeciesGuildMap);
    int  getStartYearOffset();

    bool getTimeSeriesData(const std::string  MohnsRhoLabel,
                           const std::string  ForecastName,
                           const std::string& TableName,
                           const int&         NumSpecies,
                           const int&         RunLength,
                           boost::numeric::ublas::matrix<double> &Catch);
    bool getTimeSeriesDataByGuild(std::string        ForecastName,
                                  const std::string& TableName,
                                  const int&         NumSpecies,
                                  const int&         RunLength,
                                  boost::numeric::ublas::matrix<double> &Catch);
    void initConnections();
    void initGUIs();
    bool isAggProd();
    bool isAtLeastOneFilterPressed();
    /**
     * @brief This method returns a boolean signifying whether or not the an Estimation is currently running?
     * @return true or false
     */
    bool isEstimationRunning();
    /**
     * @brief This method returns a boolean signifying whether or not the current run is Mohn's Rho run?
     * @return true or false
     */
    bool isMohnsRho();
    bool isStopped(std::string &runName,
                   std::string &msg1,
                   std::string &msg2,
                   std::string &stopRunFile,
                   std::string &state);
    void loadGuis();
    void loadDatabase();
    bool loadInteraction(int &NumSpecies,
                         std::string InteractionType,
                         std::string MinTable,
                         std::string MaxTable,
                         std::vector<double> &MinData,
                         std::vector<double> &MaxData,
                         int &NumInteractionParameters);
    bool loadInteraction(int &NumSpecies,
                         std::string InteractionType,
                         std::string MinTable,
                         std::string MaxTable,
                         std::vector<std::vector<double> > &MinData,
                         std::vector<std::vector<double> > &MaxData,
                         int &NumInteractionParameters);
    bool loadInteractionGuilds(int &NumSpecies,
                               int &NumGuilds,
                               std::string InteractionType,
                               std::map<std::string,std::string> &GuildSpeciesMap,
                               std::string MinTable,
                               std::string MaxTable,
                               std::vector<std::vector<double> > &MinData,
                               std::vector<std::vector<double> > &MaxData,
                               int &NumInteractionParameters);
    bool loadParameters(Data_Struct &m_DataStruct,
                        const bool& verbose);
    void loadVisibleTables(const bool& isAlpha,
                           const bool& isMsProd,
                           const bool& isAggProd,
                           const bool& isRho,
                           const bool& isHandling,
                           QList<QTableView*>& TableViews,
                           QList<QString>& TableNames);
    bool loadUncertaintyData(const bool&          isMonteCarlo,
                             const int&           NumSpecies,
                             const std::string&   ForecastName,
                             const std::string&   Algorithm,
                             const std::string&   Minimizer,
                             const std::string&   ObjectiveCriterion,
                             const std::string&   Scaling,
                             std::vector<double>& GrowthRateUncertainty,
                             std::vector<double>& CarryingCapacityUncertainty,
                             std::vector<double>& PredationUncertainty,
                             std::vector<double>& CompetitionUncertainty,
                             std::vector<double>& BetaSpeciesUncertainty,
                             std::vector<double>& BetaGuildsUncertainty,
                             std::vector<double>& HandlingUncertainty,
                             std::vector<double>& ExponentUncertainty,
                             std::vector<double>& CatchabilityUncertainty,
                             std::vector<double>& HarvestUncertainty);
    bool modifyTable(const std::string& TableName,
                     const QString&     OriginalSystemName,
                     const QString&     MohnsRhoLabel,
                     const int&         MohnsRhoStartYear,
                     const int&         MohnsRhoRunLength,
                     const int&         InitialYear);
    void queryUserPreviousDatabase();
    void readSettings(QString name);
    void readSettings();
    void runBeesAlgorithm();
    void runNextMohnsRhoEstimation();
    void runNLoptAlgorithm();
    bool saveScreenshot(QString &outputfile, QPixmap &pm);
    void saveSettings();
    bool scaleTimeSeries(const std::vector<double>&             Uncertainty,
                         boost::numeric::ublas::matrix<double>& HarvestMatrix);
    void setNumLines(int numLines);
    void setup2dChart();
    void setup3dChart();
    void setupOutputChartWidgets();
    void setupOutputEstimateParametersWidgets();
    void setupOutputModelFitSummaryWidgets();
    void setupOutputDiagnosticSummaryWidgets();
    void setupProgressChart();
    void showChartBcBoVsTime(
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            const int &StartYear,
            const int &m_NumLines,
            std::vector<std::string> &Algorithms,
            std::vector<std::string> &Minimizers,
            std::vector<std::string> &ObjectiveCriteria,
            std::vector<std::string> &Scalings,
            std::vector<boost::numeric::ublas::matrix<double> > &OutputBiomass,
            boost::numeric::ublas::matrix<double> &ObservedBiomass,
            QList<double> &BiomassMSYValues,
            QString &ScaleStr,
            double &ScaleVal,
            double &YMin);
    void showChartBcVsTimeSelectedSpecies(QList<int> &RowList,
                                          QList<QString> &RowNameList);
    void showChartBcVsTimeAllSpecies(
            std::string type,
            const int &NumSpecies,
            const QList<int> &Rows,
            const int &RunLength,
            std::map<std::string, std::vector<std::string> > &dataMapSpecies,
            std::map<std::string, std::vector<std::string> > &dataMapCalculatedBiomass,
            QString &ScaleStr,
            double &ScaleVal);
    void showChartCatchVsBc(
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            std::map<std::string, std::vector<std::string> > &dataMapCalculatedBiomass,
            std::map<std::string, std::vector<std::string> > &dataMapCatch,
            QString &ScaleStr,
            double &ScaleVal);
    void showChartTableVsTime(
            const std::string &label,
            const int &NumSpecies,
            const QString &OutputSpecies,
            const int &SpeciesNum,
            const int &RunLength,
            const int &StartYear,
            int &m_NumLines,
            boost::numeric::ublas::matrix<double> &Catch,  // Catch data
            std::vector<boost::numeric::ublas::matrix<double> > &Biomass,  // Catch or Calculated Biomass
            QList<double> &Values,
            QString &ScaleStr,
            double &ScaleVal,
            double &YMinSliderVal);
    bool showDiagnosticsChart2d(QString& ScaleStr,
                                double&  ScaleVal,
                                double&  YMinSliderVal);
    void showDiagnosticsFitnessVsParameter(
            const int&         NumPoints,
            std::string        XLabel,
            std::string        YLabel,
            const int&         NumSpecies,
            const QString&     OutputSpecies,
            const int&         SpeciesNum,
            boost::numeric::ublas::matrix<double> &DiagnosticsValue,
            boost::numeric::ublas::matrix<double> &DiagnosticsFitness,
            double& YMinSliderVal);
    void showForecastBiomassVsTime(const std::string &label,
                                   const int         &StartYear,
                                   const int         &NumSpecies,
                                   const QString     &OutputSpecies,
                                   const int         &SpeciesNum,
                                   const int         NumYears,
                                   std::vector<boost::numeric::ublas::matrix<double> > &Biomass,
                                   QString           &ScaleStr,
                                   double            &ScaleVal,
                                   double            &YMinSliderVal,
                                   double            &brightnessFactor,
                                   bool              useDimColor,
                                   bool              clearChart,
                                   QStringList       ColumnLabelsForLegend);
    bool showForecastChart(const bool& isAggProd,
                           std::string ForecastName,
                           const int&  StartYear,
                           QString&    ScaleStr,
                           double&     ScaleVal,
                           double&     YMinSliderVal,
                           double      BrightnessFactor);
    void showMohnsRhoBiomassVsTime(const std::string &label,
                                   const int         &InitialYear,
                                   const int         &StartYear,
                                   const int         &NumSpecies,
                                   const QString     &OutputSpecies,
                                   const int         &SpeciesNum,
                                   const int         MaxNumYears,
                                   std::vector<boost::numeric::ublas::matrix<double> > &Biomass,
                                   QString           &ScaleStr,
                                   double            &ScaleVal,
                                   double            &YMinSliderVal,
                                   double            &brightnessFactor,
                                   bool              useDimColor,
                                   bool              clearChart,
                                   QStringList       ColumnLabelsForLegend);
    void updateDiagnosticSummaryStatistics();
    bool updateOutputBiomassTable(std::string& ForecastName,
                                  int&         StartYear,
                                  int&         RunLength,
                                  bool&        isMonteCarlo,
                                  int&         RunNum,
                                  std::string& Algorithm,
                                  std::string& Minimizer,
                                  std::string& ObjectiveCriterion,
                                  std::string& Scaling,
                                  std::string& isAggProdStr,
                                  std::string& GrowthForm,
                                  std::string& HarvestForm,
                                  std::string& CompetitionForm,
                                  std::string& PredationForm,
                                  std::string& GrowthRateTable,
                                  std::string& CarryingCapacityTable,
                                  std::string& CatchabilityTable,
                                  std::string& BiomassTable);
    void updateOutputBiomassTableFromTestValues();
    void updateProgressChartAnnotation(double xMin, double xMax, double xInc);
    void updateOutputTables(
        std::string                    &Algorithm,
        std::string                    &Minimizer,
        std::string                    &ObjectiveCriterion,
        std::string                    &Scaling,
        const int                      &isCompAggProd,
        const QStringList              &SpeciesList,
        const QStringList              &GuildList,
        const QList<double>            &GrowthRateList,
        const QList<double>            &SpeciesKList,
        const QList<double>            &CatchabilityList);
    void updateOutputTables(
        std::string                                 &Algorithm,
        std::string                                 &Minimizer,
        std::string                                 &ObjectiveCriterion,
        std::string                                 &Scaling,
        const int                                   &isCompAggProd,
        const QStringList                           &SpeciesList,
        const QStringList                           &GuildList,
        const std::vector<double>                   &EstGrowthRates,
        const std::vector<double>                   &EstCarryingCapacities,
        const std::vector<double>                   &EstCatchability,
        const boost::numeric::ublas::matrix<double> &EstCompetitionAlpha,
        const boost::numeric::ublas::matrix<double> &EstCompetitionBetaSpecies,
        const boost::numeric::ublas::matrix<double> &EstCompetitionBetaGuilds,
        const boost::numeric::ublas::matrix<double> &EstPredation,
        const boost::numeric::ublas::matrix<double> &EstHandling,
        const std::vector<double>                   &EstExponent);
    void updateTextBoxFormula();

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
    //                      const Data_Struct &dataStruct);
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

protected:
    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void callback_ClearEstimationTables();
    void callback_DiagnosticsTabChanged(int tab);
    void callback_EnableFilterButtons(bool state);
    void callback_EstimationTabChanged(int tab);
    void callback_ForecastLineBrightnessChanged(double brightnessFactor);
    void callback_ForecastLoaded(std::string ForecastName);
    void callback_ForecastTabChanged(int tab);
    void callback_Hovered(bool status, int index, QBarSet *barset);
    void callback_LoadDatabase(QString databaseName);
    void callback_LoadDataStruct();
    void callback_LoadProject();
    void callback_NavigatorSelectionChanged();
    void callback_NoSystemsSet();
    void callback_OutputTypeCMB(QString type,std::map<QString,QStringList> SortedForecastLabelsMap);
    void callback_ReadProgressChartDataFile();
    void callback_RefreshOutput();
    void callback_ReloadWidgets();
    void callback_ResetFilterButtons();
    void callback_RunCompleted(QString outputStr);
    void callback_RunDiagnosticEstimation(std::vector<std::pair<int,int> > ranges);
    void callback_RunEstimation();
    void callback_RunForecast(std::string ForecastName,bool GenerateBiomass);
    void callback_SaveMainSettings();
    void callback_SetChartType(std::string type,std::string method);
    void callback_SetChartView2d(bool state);
    void callback_Setup_Tab4_CompetitionFormCMB(QString type);
    void callback_Setup_Tab4_GrowthFormCMB(QString type);
    void callback_Setup_Tab4_HarvestFormCMB(QString type);
    void callback_Setup_Tab4_ModelPresetsCMB(QString type);
    void callback_Setup_Tab4_PredationFormCMB(QString type);
    void callback_SetupTabChanged(int tab);
    bool callback_ShowChart(QString outputType,QString outputSpecies);
    bool callback_ShowChartMohnsRho();
    bool callback_ShowChartMultiScenario(QStringList SortedForecastLabels);
    bool callback_ShowDiagnosticsChart3d();
    void callback_ShowRunMessage(QString fontName);
    void callback_SelectCenterSurfacePoint();
    void callback_SetOutputScenarioForecast();
    void callback_SetStyleSheet(QString style);
    void callback_SubRunCompleted(int RunNum,int SubRunNum,int NumSubRuns);
    void callback_SystemLoaded();
    void callback_SystemSaved();
    void callback_TestTimer();
    void callback_UpdateProgressData(int SpeciesNum,int NumParams,QString elapsedTime);
    void callback_UpdateSummaryStatistics();
    void callback_UpdateTextBoxFormula();
    void callback_UpdateWindowTitle();
    void callback_YAxisMinValueChanged(int value);

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
     * @brief Save all data generated by current run and display charts
     */
    void menu_saveAndShowCurrentRun();
    /**
     * @brief Save all data generated by current run
     */
    void menu_saveCurrentRun();
    /**
     * @brief Save application settings
     */
    void menu_saveSettings();
    /**
     * @brief Take a screen shot of the current image displayed
     */
    void menu_screenshot();
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
