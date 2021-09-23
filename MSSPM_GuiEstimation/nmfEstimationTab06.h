/**
 * @file nmfEstimationTab06.h
 * @brief GUI definition for the nmfEstimation_Tab6 Estimation Run class
 *
 * This file contains the GUI definition for the Run Estimation Data page. This
 * page allows the user to enter/modify Run settings such as Estimation Model type
 * and its associated parameters.  In addition, the user can also run the Estimation
 * from this page.
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

#include <QComboBox>
#include <QMessageBox>

#include "nmfUtilsQt.h"
#include "nmfStructsQt.h"
#include "EnsembleDialog.h"

#ifndef NMFESTIMATIONTAB6_H
#define NMFESTIMATIONTAB6_H

/**
 * @brief The Run Estimation Settings
 *
 * These widgets allow the user to enter and save the parameters required
 * to run an Estimation.  There's also a text widget which shows a summary
 * to the user of what transpired after the user hits the Run button.
 */
class nmfEstimation_Tab6: public QObject
{
    Q_OBJECT


    nmfDatabase*    m_DatabasePtr;
    int             m_FontSize;
    int             m_IsMonospaced;
    nmfLogger*      m_Logger;
    std::string     m_ProjectDir;
    std::string     m_ProjectName;
    std::string     m_ModelName;
    std::string     m_EstimationDataFile;
    std::string     m_EstimationID;
    std::string     m_EstimationOutputFile;
    std::vector<nmfStructsQt::EstimateRunBox> m_EstimateRunBoxes;
    std::map<QString,QString> m_DetStoTypeMap;
    std::string     m_GrowthForm;
    std::string     m_HarvestForm;
    std::string     m_CompetitionForm;
    std::string     m_PredationForm;
    EnsembleDialog* m_EnsembleDialog;
    std::string     m_EnsembleDefaultFilename;
    std::string     m_EnsembleTimeStampedFilename;
    bool            m_IsMultiRun;
    std::string     m_MultiRunType;

    QWidget*     Estimation_Tab6_Widget;
    QGroupBox*   Estimation_Tab6_ModelAlgorithmsGB;
    QGroupBox*   Estimation_Tab6_Bees_ParametersGB;
    QGroupBox*   Estimation_Tab6_NL_ParametersGB;
    QGroupBox*   Estimation_Tab6_EstParametersGB;
    QComboBox*   Estimation_Tab6_MinimizerAlgorithmCMB;
    QComboBox*   Estimation_Tab6_ObjectiveCriterionCMB;
    QLabel*      Estimation_Tab6_MinimizerAlgorithmLBL;
    QLabel*      Estimation_Tab6_MinimizerDetStoTypeLBL;
    QComboBox*   Estimation_Tab6_MinimizerTypeCMB;
    QTabWidget*  Estimation_Tabs;
    QTextEdit*   Estimation_Tab6_RunTE;
    QPushButton* Estimation_Tab6_RunPB;
    QPushButton* Estimation_Tab6_SavePB;
    QPushButton* Estimation_Tab6_ReloadPB;
    QPushButton* Estimation_Tab6_PrevPB;
    QPushButton* Estimation_Tab6_NextPB;
    QComboBox*   Estimation_Tab6_FontSizeCMB;
    QCheckBox*   Estimation_Tab6_MonoCB;
    QComboBox*   Estimation_Tab6_EstimationAlgorithmCMB;
    QSpinBox*    Estimation_Tab6_Bees_MaxGenerationsSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBestSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumOtherBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NeighborhoodSizeSB;
    QSpinBox*    Estimation_Tab6_Bees_NumberOfRunsSB;
    QLabel*      Estimation_Tab6_ScalingLBL;
    QComboBox*   Estimation_Tab6_ScalingCMB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterValueCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterTimeCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterIterCB;
    QLineEdit*   Estimation_Tab6_NL_StopAfterValueLE;
    QSpinBox*    Estimation_Tab6_NL_StopAfterTimeSB;
    QSpinBox*    Estimation_Tab6_NL_StopAfterIterSB;
    QCheckBox*   Estimation_Tab6_EstimateInitialBiomassCB;
    QCheckBox*   Estimation_Tab6_EstimateGrowthRateCB;
    QCheckBox*   Estimation_Tab6_EstimateCarryingCapacityCB;
    QCheckBox*   Estimation_Tab6_EstimateCatchabilityCB;
    QCheckBox*   Estimation_Tab6_EstimateCompetitionAlphaCB;
    QCheckBox*   Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB;
    QCheckBox*   Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB;
    QCheckBox*   Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB;
    QCheckBox*   Estimation_Tab6_EstimatePredationRhoCB;
    QCheckBox*   Estimation_Tab6_EstimatePredationHandlingCB;
    QCheckBox*   Estimation_Tab6_EstimatePredationExponentCB;
    QCheckBox*   Estimation_Tab6_EstimateSurveyQCB;
    QSpinBox*    Estimation_Tab6_EnsembleTotalRunsSB;
    QLabel*      Estimation_Tab6_EnsembleAveragingAlgorithmLBL;
    QComboBox*   Estimation_Tab6_EnsembleAveragingAlgorithmCMB;
    QGroupBox*   Estimation_Tab6_EnsembleControlsGB;
    QLineEdit*   Estimation_Tab6_EnsembleRunsSetLE;
    QPushButton* Estimation_Tab6_EnsembleAddPB;
    QSpinBox*    Estimation_Tab6_EnsembleRunsSB;
    QPushButton* Estimation_Tab6_EnsembleClearPB;
    QPushButton* Estimation_Tab6_EnsembleViewPB;
    QLabel*      Estimation_Tab6_EnsembleRunsWithSettingsLBL;
    QLabel*      Estimation_Tab6_EnsembleRunsSetLBL;
    QPushButton* Estimation_Tab6_EnsembleSetAllPB;
    QComboBox*   Estimation_Tab6_NL_StopAfterTimeUnitsCMB;
    QPushButton* Estimation_Tab6_EnsembleLoadPB;
    QComboBox*   Estimation_Tab6_EnsembleAverageByCMB;
    QComboBox*   Estimation_Tab6_EnsembleUsingByCMB;
    QSpinBox*    Estimation_Tab6_EnsembleUsingAmountSB;
    QPushButton* Estimation_Tab6_EnsembleUsingPctPB;
    QCheckBox*   Estimation_Tab6_SetDeterministicCB;
    QCheckBox*   Estimation_Tab6_EnsembleSetDeterministicCB;
    QCheckBox*   Estimation_Tab6_BeesSetDeterministicCB;
    QPushButton* Estimation_Tab6_AddToReviewPB;
    QPushButton* Estimation_Tab6_NL_TimeUnitsLockPB;
    QLabel*      Estimation_Tab6_BeesDetStoTypeLBL;
    QLabel*      Estimation_Tab6_BeesSetDeterministicLBL;

    void readSettings();
    bool saveSettingsConfiguration(bool verbose,std::string currentSettingsName);

    void activateCheckBox(QCheckBox* cbox,
                           std::pair<bool,bool> state);
    QList<QCheckBox* > getAllEstimateCheckboxes();
    void initializeDetStoMap();
    void adjustNumberOfParameters();

    bool queryUserIfOkToClearMultiRunFile();
    void runEnsemble();
    bool passRunChecks(std::string& msg);
    void checkAlgorithmIdentifiersForMultiRun(
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling);

public:
    /**
     * @brief nmfEstimation_Tab6 : class constructor for the Run Estimation GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab6(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab6();

    void runEstimation();
    bool addToMultiRunFile(const int& numRunsToAdd,
                           const int& currentNumberOfRuns,
                           const int& totalNumberOfRunsDesired,
                           QString& filename);
    void clearMohnsRhoFile();
    void setBeesDeterministicCB(int state);

    /**
     * @brief Append the passed message to the output text edit widget
     * @param the message to append to the text widget
     */
    void appendOutputTE(QString msg);
    /**
     * @brief Clear the output text edit widget
     */
    void clearOutputTE();
    /**
     * @brief Converts the passed number of seconds to the correct whole number of hours, minutes, or seconds
     * @param seconds : number of seconds to convert to whole number hours or minutes
     * @return the converted value in hours or minutes (or seconds if < 60)
     */
    int convertToAppropriateUnits(int seconds);
    void clearEnsembleFile();
    void enableEnsembleWidgets(bool enable);
    std::string getBeesMaxGenerations();
    std::string getBeesNumBees();
    std::string getBeesNumBestSites();
    std::string getBeesNumEliteSites();
    std::string getBeesNumEliteBees();
    std::string getBeesNumOtherBees();
    std::string getBeesNeighborhoodSize();
    std::string getBeesNumberOfRuns();
    std::string getEnsembleTimeStampedFilename();
    bool loadEnsembleFile(QString ensembleFile,
                          const bool& verbose);
    void setEnsembleRuns(int value);
    void setEnsembleRunsSet(int value);
    bool isAMultiRun();
    bool isSetToDeterministicBees();
    bool isSetToDeterministicNLopt();
    void enableAddToReview(bool enable);
    void enableMultiRunControls(bool enable);
    void enableRunButton(bool enableRun);
    QString getMultiRunColumnData(int col);
    bool isStopAfterValue();
    bool isStopAfterTime();
    bool isStopAfterIter();
    int getMaxGenerations();
    int getNumBees();
    int getNumBestSites();
    int getNumEliteSites();
    int getNumEliteBees();
    int getNumOtherBees();
    int getNeighborhoodSize();
    int getNumSubRuns();
    void setMaxGenerations(int value);
    void setNumBees(int value);
    void setNumBestSites(int value);
    void setNumEliteSites(int value);
    void setNumEliteBees(int value);
    void setNumOtherBees(int value);
    void setNeighborhoodSize(int value);
    void setNumSubRuns(int value);

    /**
     * @brief Gets the current Algorithm selected from the GUI
     * @return Returns the algorithm chosen
     */
    std::string getCurrentAlgorithm();
    /**
     * @brief Gets the current Minimizer routine chosen from the GUI
     * @return Returns the minimizer chosen
     */
    std::string getCurrentMinimizer();
    /**
     * @brief Gets the current Objective Criterion function from the GUI
     * @return Returns the objective criterion function (as a string)
     */
    std::string getCurrentObjectiveCriterion();
    /**
     * @brief Gets the current Scaling functions from the GUI
     * @return Returns the scaling function (as a string)
     */
    std::string getCurrentScaling();
    /**
     * @brief Gets the current time units used for Stop after (time) option
     * @return Units of time
     */
    QString getCurrentTimeUnits();
    /**
     * @brief Gets the list of checked and enabled Estimate run boxes
     * @return Returns list of the Estimate run box structs from Estimation Tab6
     */
    std::vector<nmfStructsQt::EstimateRunBox> getEstimateRunBoxes();
    /**
     * @brief Gets the current Stop after (fcn evals) value
     * @return Returns number of function evaluations after which to stop the run
     */
    int getCurrentStopAfterIter();
    /**
     * @brief Gets the current Stop after fitness value
     * @return The fitness value that when reached will stop the model
     */
    double getCurrentStopAfterValue();
    /**
     * @brief Gets the current Stop after time (seconds)
     * @return The time (in number of seconds) after which to stop the model
     */
    int getCurrentStopAfterTime();
    int getEnsembleUsingAmountValue();
    int getEnsembleNumberOfTotalRuns();
    QString getEnsembleAverageBy();
    QString getEnsembleAveragingAlgorithm();
    QString getEnsembleUsingBy();
    QString createTimeStampedEnsembleFile();

    bool isEstInitialBiomassEnabled();
    bool isEstInitialBiomassChecked();
    bool isEstGrowthRateEnabled();
    bool isEstGrowthRateChecked();
    bool isEstCarryingCapacityEnabled();
    bool isEstCarryingCapacityChecked();
    bool isEstCatchabilityEnabled();
    bool isEstCatchabilityChecked();
    bool isEstCompetitionAlphaEnabled();
    bool isEstCompetitionAlphaChecked();
    bool isEstCompetitionBetaSpeciesEnabled();
    bool isEstCompetitionBetaSpeciesChecked();
    bool isEstCompetitionBetaGuildsEnabled();
    bool isEstCompetitionBetaGuildsChecked();
    bool isEstCompetitionBetaGuildsGuildsEnabled();
    bool isEstCompetitionBetaGuildsGuildsChecked();
    bool isEstPredationRhoEnabled();
    bool isEstPredationRhoChecked();
    bool isEstPredationHandlingEnabled();
    bool isEstPredationHandlingChecked();
    bool isEstPredationExponentEnabled();
    bool isEstPredationExponentChecked();
    bool isEstSurveyQEnabled();
    bool isEstSurveyQChecked();
    bool isEnsembleUsingPct();
    void setRunButtonLabel(QString label);
    void setMohnsRhoFileType(const QString& runType);
    void setMohnsRhoFileHeader();

    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Clears the current message in the output text edit and writes the passed message to it
     * @param font : font to be used for the message
     * @param msg : the content of the message
     */
    void refreshMsg(QFont font, QString msg);
    /**
     * @brief Saves the current application settings to a Qt settings file
     */
    void saveSettings();
    /**
     * @brief Saves application settings but checking with user first
     * @param RunChecks : boolean checking to see if program should check with user prior to saving Model algorithm settings
     * @return Returns true if the user acknowledged the Save
     */
    bool saveSystem(bool RunChecks);
    /**
     * @brief Sets the current units to the passed string
     * @param units : current units to set the time units combo box to
     */
    void setCurrentTimeUnits(QString units);
    void setEnsembleAverageBy(QString averageBy);
    void setEnsembleAveragingAlgorithm(QString aveAlg);
    void setEnsembleUsingBy(QString usingBy);
    void setEnsembleUsingAmountValue(QString usingAmount);
    void setEnsembleUsingPct(bool isUsingPct);
    bool runBeesCheck(QString& errorMsg);

    /**
     * @brief Sets the font for the output text edit widget
     * @param font : the font to use for the output text edit widget
     */
    void setFont(QFont font);
    /**
     * @brief Sets the content for the output text edit widget
     * @param msg : the content to use for the output text edit widget
     */
    void setOutputTE(QString msg);
    void clearWidgets();
    bool getFixedSeedBees();

signals:
    /**
     * @brief Signal sent to check all Estimation tables for completeness
     */
    void CheckAllEstimationTablesAndRun();
    void AddToReview();
    void DimScalarBiomassControls(bool dim);
    void EnableRunButtons(bool state);
    void BeesSetDeterministic(bool state);

    /**
     * @brief Signal sent after the user checks the Mono Font box. It causes
     * the displayed output edit widget to use a monospaced font.
     * @param font : the monospaced font to use in the output edit widget
     */
    void ShowRunMessage(QString font);
    /**
     * @brief Signal sent when the user changes the current Algorithm. Each
     * algorithm has its own set of parameters that must be displayed in the GUI.
     * @param algorithm : the new algorithm selected
     */
    void SetAlgorithm(QString algorithm);
    /**
     * @brief Signal sent to refresh the Forecast Year Range in the Forecast->Setup GUI.
     */
    void UpdateForecastYears();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user selects an Averagin Algorithm for the multi-run feature
     * @param averagingAlgorithm : the selected averaging algorithm
     */
    void callback_AveragingAlgorithmCMB(QString averagingAlgorithm);
    /**
     * @brief Callback invoked when the user clicks the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_ReloadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user toggles the Ensemble controls check box
     * @param isChecked : checked state of Ensemble controls group check box
     */
    void callback_EnsembleControlsGB(bool isChecked);
    /**
     * @brief Callback invoked when the user presses the Set All button. It sets all of the
     * multi-run runs to use the same parameters.
     */
    void callback_EnsembleSetAllPB();
    /**
     * @brief Callback invoked when the user changes the font in the Run Summary tab
     * @param fontSize : the font selected
     */
    void callback_Estimation_Tab6_FontSizeCMB(QString fontSize);
    /**
     * @brief Callback invoked when the user checks the mono font checkbox in the Run Summary tab
     * @param isChecked : check state of the checkbox
     */
    void callback_Estimation_Tab6_MonoCB(int isChecked);
    /**
     * @brief Callback invoked when the user changes the Estimation Algorithm
     * @param algorithm : the estimation algorithm selected
     */
    void callback_EstimationAlgorithmCMB(QString algorithm);
    /**
     * @brief Callback invoked when user changes the minimizer algorithm.
     * @param algorithm : the minimizer algorithm selected
     */
    void callback_MinimizerAlgorithmCMB(QString algorithm);
    /**
     * @brief Callback invoked when the user changes the minimizer type
     * @param type : the type of minimizer (i.e., global or local)
     */
    void callback_MinimizerTypeCMB(QString type);
    /**
     * @brief Callback invoked when user clicks the Load button
     */
    void callback_EnsembleLoadPB();
    /**
     * @brief Callback invoked when the user changes the number of runs desired
     * @param value : number of runs desired
     */
    void callback_EnsembleTotalRunsSB(int value);
    /**
     * @brief Callback invoked when the user changes the number of ensemble runs desired
     * @param value : number of runs desired
     */
    void callback_EnsembleRunsSB(int value);
    /**
     * @brief Callback invoked when the user clicks the Add button to add a set of
     * runs to a multi-run
     */
    void callback_EnsembleAddPB();
    /**
     * @brief Callback invoked when user clicks the Clr button for the Multi-Run
     */
    void callback_EnsembleClearPB();
    /**
     * @brief Callback invoked when the user clicks the Edit button to edit or view a set of
     * runs to a multi-run
     */
    void callback_EnsembleViewPB();
    /**
     * @brief Callback invoked when the user changes the Objective Criterion
     * @param objectiveCriterion : the objective criterion selected
     */
    void callback_ObjectiveCriterionCMB(QString objectiveCriterion);
    /**
     * @brief Callback invoked when user needs to query the states of all the Estimate run check boxes
     * @param unused : unused
     */
    void callback_RefreshEstimateRunBoxes(int unused);
    /**
     * @brief Callback invoked when the user checks the Stop When Reach Value checkbox
     * @param isChecked : boolean signiying the check state
     */
    void callback_StopValCB(int isChecked);
    /**
     * @brief Callback invoked when the user checks the Stop After Time checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterTimeCB(int isChecked);
    /**
     * @brief Callback invoked when user changes the Stop After Time spin box value
     * @param value : new Stop After Time value
     */
    void callback_StopAfterTimeSB(int value);
    /**
     * @brief Callback invoked when user changes the units for the Stop After Time spin box
     * @param units : new units for the time entry
     */
    void callback_StopAfterTimeUnitsCMB(QString units);
    /**
     * @brief Callback invoked when the user checks the Stop After Iteration checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterIterCB(int isChecked);
    /**
     * @brief Callback invoked when the user changes the Stop Ater Interations spin box value
     * @param value : new Stop After Number of iterations value
     */
    void callback_StopAfterIterSB(int value);
    /**
     * @brief Callback invoked when user modifies the Stop After Value value
     * @param value : new Stop After Value value
     */
    void callback_StopAfterValueLE(QString value);
    /**
     * @brief Callback invoked when the user saves the model on the Setup -> Model Setup GUI
     */
    void callback_SaveSettings();
    /**
     * @brief Callback invoked when main routine needs to update Estimate checkboxes
     * @param EstimateRunBoxes : Names and states of Estimate checkboxes to update
     */
    void callback_SetEstimateRunCheckboxes(
            std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes);
    /**
     * @brief Callback invoked when user changes the scaling algorithm
     * @param scalingAlgorithm : new scaling algorithm selected
     */
    void callback_ScalingCMB(QString scalingAlgorithm);

    void callback_EnsembleUsingPctPB();
    void callback_EnsembleUsingAmountCMB(QString value);
    void callback_SetDeterministicCB(int state);
    void callback_EnsembleSetDeterministicCB(int state);
    void callback_AddToReviewPB();
    void callback_EnableSurveyQ(const QString biomassType,
                                const bool enable,
                                const bool checked);
    void callback_EstimateSurveyQCB(int state);
    void callback_TimeUnitsLockPB(bool isChecked);

};

#endif // NMFESTIMATIONTAB6_H
