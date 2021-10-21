/**
 * @file nmfEstimationTab06.h
 * @brief GUI definition for the nmfEstimation_Tab7 Estimation Run class
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

#ifndef NMFESTIMATIONTAB7_H
#define NMFESTIMATIONTAB7_H

/**
 * @brief The Run Estimation Settings
 *
 * These widgets allow the user to enter and save the parameters required
 * to run an Estimation.  There's also a text widget which shows a summary
 * to the user of what transpired after the user hits the Run button.
 */
class nmfEstimation_Tab7: public QObject
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
    QCheckBox*   Estimation_Tab6_MinimizerSetDeterministicCB;
    QCheckBox*   Estimation_Tab6_EnsembleSetDeterministicCB;
    QCheckBox*   Estimation_Tab6_BeesSetDeterministicCB;
    QPushButton* Estimation_Tab6_AddToReviewPB;
    QPushButton* Estimation_Tab6_NL_TimeUnitsLockPB;
    QLabel*      Estimation_Tab6_BeesDetStoTypeLBL;
    QLabel*      Estimation_Tab6_BeesSetDeterministicLBL;

    void activateCheckBox(QCheckBox* cbox,
                           std::pair<bool,bool> state);
    void adjustNumberOfParameters();
    void checkAlgorithmIdentifiersForMultiRun(
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling);
    QList<QCheckBox* > getAllEstimateCheckboxes();
    void initializeDetStoMap();
    bool passRunChecks(std::string& msg);
    bool queryUserIfOkToClearMultiRunFile();
    void readSettings();
    void runEnsemble();
    bool saveSettingsConfiguration(bool verbose,std::string currentSettingsName);

public:
    /**
     * @brief nmfEstimation_Tab7 : class constructor for the Run Estimation GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab7(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab7();

    /**
     * @brief Adds an item to the Multi-Run file
     * @param numRunsToAdd : number of runs to add
     * @param currentNumberOfRuns : current number of runs already in Multi-Run file
     * @param totalNumberOfRunsDesired : total number of runs that the Multi-Run file will contain
     * @param filename : filename of Multi-Run file; if it's empty, the default Multi-run filename will be used
     * @return true if add was successful, false otherwise
     */
    bool addToMultiRunFile(const int& numRunsToAdd,
                           const int& currentNumberOfRuns,
                           const int& totalNumberOfRunsDesired,
                           QString& filename);
    /**
     * @brief Append the passed message to the output text edit widget
     * @param the message to append to the text widget
     */
    void appendOutputTE(QString msg);
    /**
     * @brief Removes the disk file which populates the Multi-Run popup when the user clicks View
     */
    void clearEnsembleFile();
    /**
     * @brief Removes the disk file which populates the Multi-Run popup, on a Mohn's Rho run, when the user clicks View
     */
    void clearMohnsRhoFile();
    /**
     * @brief Clear the output text edit widget
     */
    void clearOutputTE();
    /**
     * @brief Clears widgets in GUI
     */
    void clearWidgets();
    /**
     * @brief Converts the passed number of seconds to the correct whole number of hours, minutes, or seconds
     * @param seconds : number of seconds to convert to whole number hours or minutes
     * @return the converted value in hours or minutes (or seconds if < 60)
     */
    int convertToAppropriateUnits(int seconds);
    /**
     * @brief Creates a MultiRun filename that has a timestamp embedded in it
     * @return the timestamped filename
     */
    QString createTimeStampedEnsembleFile();
    /**
     * @brief Sets the enable state of the Add button which adds an item to the Multi-Run file
     * @param enable : true/false enable state for the Add button
     */
    void enableAddToReview(bool enable);
    /**
     * @brief Set the enable state on the non-ensemble widgets (i.e., group boxes)
     * @param enable : true/false enable state
     */
    void enableNonEnsembleWidgets(bool enable);
    /**
     * @brief Sets the enable state on the ensemble widgets (i.e., the ensemble/multi-run group box)
     * @param enable : true/false enable state
     */
    void enableEnsembleControls(bool enable);
    /**
     * @brief Sets the enable state of the Run button
     * @param enable : true/false enable state of the Run button
     */
    void enableRunButton(bool enable);
    /**
     * @brief Gets the Bees Algorithm Max Generations widget value
     * @return string representing the Bees Algorithm Max Generations widget value
     */
    std::string getBeesMaxGenerations();
    /**
     * @brief Gets the Bees Algorithm Neighborhood Size widget value
     * @return string representing the Bees Algorithm Neighborhood Size widget value
     */
    std::string getBeesNeighborhoodSize();
    /**
     * @brief Gets the Bees Algorithm Number of Bees widget value
     * @return string representing the Bees Algorithm Number of Bees widget value
     */
    std::string getBeesNumBees();
    /**
     * @brief Gets the Bees Algorithm Number of Best Sites widget value
     * @return string representing the Bees Algorithm Number of Best Sites widget value
     */
    std::string getBeesNumBestSites();
    /**
     * @brief Gets the Bees Algorithm Number of Elite Sites widget value
     * @return string representing the Bees Algorithm Number of Elite Sites widget value
     */
    std::string getBeesNumEliteSites();
    /**
     * @brief Gets the Bees Algorithm Number of Elite Bees widget value
     * @return string representing the Bees Algorithm Number of Elite Bees widget value
     */
    std::string getBeesNumEliteBees();
    /**
     * @brief Gets the Bees Algorithm Number of Runs widget value
     * @return string representing the Bees Algorithm Number of Runs widget value
     */
    std::string getBeesNumberOfRuns();
    /**
     * @brief Gets the Bees Algorithm Number of Other Runs widget value
     * @return string representing the Bees Algorithm Number of Other Runs widget value
     */
    std::string getBeesNumOtherBees();
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
     * @brief Gets the current minimizer type (either local or global)
     * @return string specifying minimizer type
     */
    std::string getCurrentMinimizerType();
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
     * @brief Gets the current Stop after (fcn evals) value
     * @return Returns number of function evaluations after which to stop the run
     */
    int getCurrentStopAfterIter();
    /**
     * @brief Gets the current Stop after time (seconds)
     * @return The time (in number of seconds) after which to stop the model
     */
    int getCurrentStopAfterTime();
    /**
     * @brief Gets the current Stop after fitness value
     * @return The fitness value that when reached will stop the model
     */
    double getCurrentStopAfterValue();
    /**
     * @brief Gets the current time units used for Stop after (time) option
     * @return Units of time
     */
    QString getCurrentTimeUnits();
    /**
     * @brief Gets the ensemble averaging algorithm
     * @return the ensemble averaging algorithm
     */
    QString getEnsembleAveragingAlgorithm();
    /**
     * @brief Gets the ensemble average-by value which describes how to average the ensemble
     * @return the ensemble average-by value
     */
    QString getEnsembleAverageBy();
    /**
     * @brief Gets the total number of runs in the ensemble
     * @return total number of ensemble runs
     */
    int getEnsembleNumberOfTotalRuns();
    /**
     * @brief Gets the ensemble file name with the embedded time stamp
     * @return the timestamped ensemble file name
     */
    std::string getEnsembleTimeStampedFilename();
    /**
     * @brief Gets the ensemble Using Amount value which specifies how many of the individual runs
     * should be used in the averaging calculations
     * @return the Using Amount value
     */
    int getEnsembleUsingAmountValue();
    /**
     * @brief Gets the Using By value which specifies if the averaging algorithm should use
     * All or only the Top Percent of the Using Amount value
     * @return the Using By value
     */
    QString getEnsembleUsingBy();
    /**
     * @brief Gets the Bees Algorithm Max Generations as an integer
     * @return Bees Algorithm Max Generations value
     */
    int getMaxGenerations();
    /**
     * @brief Gets the Multi-Run Column data for the passed in column
     * @param col : column whose Multi-Run data will be returned
     * @return QString representing encoded Multi-Run data for a particular column
     */
    QString getMultiRunColumnData(int col);
    /**
     * @brief Gets the Bees Algorithm Neighborhood Size as an integer
     * @return Bees Algorithm Neighborhood Size value
     */
    int getNeighborhoodSize();
    /**
     * @brief Gets the Bees Algorithm Number of Bees value as an integer
     * @return Bees Algorithm Number of Bees value
     */
    int getNumBees();
    /**
     * @brief Gets the Bees Algorithm Number of Best Sites value as an integer
     * @return Bees Algorithm Number of Best Sites value
     */
    int getNumBestSites();
    /**
     * @brief Gets the Bees Algorithm Number of Elite Sites value as an integer
     * @return Bees Algorithm Number of Elite Sites value
     */
    int getNumEliteSites();
    /**
     * @brief Gets the Bees Algorithm Number of Elite Bees value as an integer
     * @return Bees Algorithm Number of Elite Bees value
     */
    int getNumEliteBees();
    /**
     * @brief Gets the Bees Algorithm Number of Other Bees value as an integer
     * @return Bees Algorithm Number of Other Bees value
     */
    int getNumOtherBees();
    /**
     * @brief Gets the Bees Algorithm Number of Sub Runs value as an integer
     * @return Bees Algorithm Number of Sub Runs value
     */
    int getNumSubRuns();
    /**
     * @brief Gets the list of checked and enabled Estimate run boxes
     * @return Returns list of the Estimate run box structs from Estimation Tab6
     */
    std::vector<nmfStructsQt::EstimateRunBox> getEstimateRunBoxes();
    /**
     * @brief Returns whether or not the Run is a Multi Run
     * @return true if Multi-Run, false otherwise
     */
    bool isAMultiRun();
    /**
     * @brief Returns whether or not the Using Pct button has been pressed
     * @return  true if pressed, false otherwise
     */
    bool isEnsembleUsingPct();
    /**
     * @brief Returns whether or not the Estimated Carrying Capacity checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCarryingCapacityChecked();
    /**
     * @brief Returns whether or not the Estimated Carrying Capacity checkbox has been enabled
     * @return true if enabled, false otherwise
     */
    bool isEstCarryingCapacityEnabled();
    /**
     * @brief Returns whether or not the Estimated Catchability checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCatchabilityChecked();
    /**
     * @brief Returns whether or not the Estimated Catchability checkbox has been enabled
     * @return true if enabled, false otherwise
     */
    bool isEstCatchabilityEnabled();
    /**
     * @brief Returns whether or not the Estimated Competition Alpha checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionAlphaChecked();
    /**
     * @brief Returns whether or not the Estimated Competition Alpha checkbox has been enabled
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionAlphaEnabled();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Guild by Species checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaGuildsChecked();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Guild by Species checkbox has been enabled
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaGuildsEnabled();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Guild by Guild checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaGuildsGuildsChecked();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Guild by Guild checkbox has been enabled
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaGuildsGuildsEnabled();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Species by Species checkbox has been enabled
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaSpeciesEnabled();
    /**
     * @brief Returns whether or not the Estimated Competition Beta Species by Species checkbox has been checked
     * @return true if checked, false otherwise
     */
    bool isEstCompetitionBetaSpeciesChecked();
    /**
     * @brief Returns whether or not the Estimated Growth Rate checkbox has been checked
     * @return
     */
    bool isEstGrowthRateChecked();
    /**
     * @brief Returns whether or not the Estimated Growth Rate checkbox has been enabled
     * @return
     */
    bool isEstGrowthRateEnabled();
    /**
     * @brief Returns whether or not the Estimated Initial Biomass checkbox has been checked
     * @return
     */
    bool isEstInitialBiomassChecked();
    /**
     * @brief Returns whether or not the Estimated Initial Biomass checkbox has been enabled
     * @return
     */
    bool isEstInitialBiomassEnabled();
    /**
     * @brief Returns whether or not the Estimated Predation Rho checkbox has been checked
     * @return
     */
    bool isEstPredationRhoChecked();
    /**
     * @brief Returns whether or not the Estimated Predation Rho checkbox has been enabled
     * @return
     */
    bool isEstPredationRhoEnabled();
    /**
     * @brief Returns whether or not the Estimated Predation Handling checkbox has been checked
     * @return
     */
    bool isEstPredationHandlingChecked();
    /**
     * @brief Returns whether or not the Estimated Predation Handling checkbox has been enabled
     * @return
     */
    bool isEstPredationHandlingEnabled();
    /**
     * @brief Returns whether or not the Estimated Predation Exponent checkbox has been checked
     * @return
     */
    bool isEstPredationExponentChecked();
    /**
     * @brief Returns whether or not the Estimated Predation Exponent checkbox has been enabled
     * @return
     */
    bool isEstPredationExponentEnabled();
    /**
     * @brief Returns whether or not the Estimated Survey Q checkbox has been checked
     * @return
     */
    bool isEstSurveyQChecked();
    /**
     * @brief Returns whether or not the Estimated Survey Q checkbox has been enabled
     * @return
     */
    bool isEstSurveyQEnabled();
    /**
     * @brief Boolean representing if the Bees algorithm should be run deterministically
     * (i.e. with the inherent randomness being repeatable) or stochastically
     * @return true/false representing if the Bees Algorithm Fixed Seed checkbox has been checked
     */
    bool isSetToDeterministicBees();
    /**
     * @brief Boolean representing if the NLopt Minimizer algorithm should be run deterministically
     * (i.e. with the inherent randomness being repeatable) or stochastically
     * @return true/false representing if the NLopt Algorithm Fixed Seed checkbox has been checked
     */
    bool isSetToDeterministicMinimizer();
    /**
     * @brief Boolean representing of the Stop After Iteration option has been checked
     * @return true/false representing the state of the Stop After Iteration checkbox
     */
    bool isStopAfterIter();
    /**
     * @brief Boolean representing of the Stop After Time option has been checked
     * @return true/false representing the state of the Stop After Time checkbox
     */
    bool isStopAfterTime();
    /**
     * @brief Boolean representing of the Stop After Function Value option has been checked
     * @return true/false representing the state of the Stop After Function Value checkbox
     */
    bool isStopAfterValue();
    /**
     * @brief Loads the ensemble disk file into the ensemble table
     * @param ensembleFile : name of ensemble file
     * @param verbose : if true display popup messages, if false don't
     * @return true if load was ok, false otherwise
     */
    bool loadEnsembleFile(QString ensembleFile,
                          const bool& verbose);
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
     * @brief Checks Bees algorithm parameters (certain parameters must be less than other parameters)
     * @param errorMsg : error message from the check
     * @return true if all checks ok, false otherwise
     */
    bool runBeesCheck(QString& errorMsg);
    /**
     * @brief Emits signals to notify the main program to run an Estimation
     */
    void runEstimation();
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
    void setCurrentMinimizerType(QString value);
    /**
     * @brief Sets the Stop After Time units to sec, min, or hour (this option needs to be unlocked first with the adjacent lock/unlock button)
     * @param units : sec, min, or hour
     */
    void setCurrentTimeUnits(QString units);
    /**
     * @brief Sets the state of the Deterministic Bees Algorithm check box
     * @param state : true if checked, false otherwise
     */
    void setDeterministicBeesCB(bool state);
    /**
     * @brief Sets the state of the Deterministic NLopt Minimizer Algorithm check box
     * @param state : true if checked, false otherwise
     */
    void setDeterministicMinimizerCB(bool state);
    /**
     * @brief Sets the Ensemble Average By value
     * @param averageBy : describes how the averaging should be done
     */
    void setEnsembleAverageBy(QString averageBy);
    /**
     * @brief Sets the Ensemble Averaging Algorithm
     * @param aveAlg : the name of the Ensemble Averaging Algorithm to use
     */
    void setEnsembleAveragingAlgorithm(QString aveAlg);
    /**
     * @brief Sets the total number of Ensemble Runs for the Multi-Run
     * @param value : total number of Ensemble Runs to set the appropriate spin box to
     */
    void setEnsembleTotalRuns(int value);
    /**
     * @brief Sets the Ensemble Runs Set line edit
     * @param value : value to put in the Currently Set text box
     */
    void setEnsembleRunsSet(int value);
    /**
     * @brief Sets the Ensemble Using Amount value that determines either how many of the top n runs
     * to use when averaging or what percent of the top runs to use when averaging
     * @param usingAmount : the using amount value
     */
    void setEnsembleUsingAmountValue(QString usingAmount);
    /**
     * @brief Sets the Ensemble Using By item
     * @param usingBy : the Using By item (either Using All or Using Top), the first
     * specifies using all runs in the average and the second specifies using the top n runs or
     * top n percent of the runs in the average (based on other widget values)
     */
    void setEnsembleUsingBy(QString usingBy);
    /**
     * @brief Sets the boolean that specifies the Using Amount Value as either an absolute number
     * of runs to use when averaging or as a percentage of all runs to use when averaging
     * @param isUsingPct : boolean value that if true denotes using a percentage, if false denotes
     * using an absolute value of runs
     */
    void setEnsembleUsingPct(bool isUsingPct);
    /**
     * @brief Sets the font for the output text edit widget
     * @param font : the font to use for the output text edit widget
     */
    void setFont(QFont font);
    /**
     * @brief Sets the maximum number of generations for the Bees estimation algorithm; a generation is
     * when bees are distributed and assigned to sites
     * @param value : value of max number of generations
     */
    void setMaxGenerations(int value);
    /**
     * @brief Writes the header (i.e., names of fields) to the Mohn's Rho file to be used when
     * running a Mohn's Rho multi-run
     */
    void setMohnsRhoFileHeader();
    /**
     * @brief Sets the type of Mohn's Rho file to either a single run or a multi-run
     * @param runType : the type of Mohn's Rho run
     */
    void setMohnsRhoFileType(const QString& runType);
    /**
     * @brief Sets the neighborhood size in the Bees estimation algorithm, it is the length of a site as
     * a percentage of the entire parameter space
     * @param value : percentage size of neighborhood
     */
    void setNeighborhoodSize(int value);
    /**
     * @brief Sets the number of bees exploring the parameter space, in the Bees estimation algorithm, at any given time
     * @param value : number of exploring bees (i.e., points) in the parameter space
     */
    void setNumBees(int value);
    /**
     * @brief Sets the number of best sites in the Bees estimation algorithm; these are the number of sites in the parameter
     * space to explore in more detail
     * @param value : number of best sites
     */
    void setNumBestSites(int value);
    /**
     * @brief Sets the number of elite bees in the Bees estimation algorithm; these are the number of bees that
     * are assigned to further explore an elite site
     * @param value : number of elite bees
     */
    void setNumEliteBees(int value);
    /**
     * @brief Sets the number of elite sites in the Bees estimation algorithm; these are the number of best sites to
     * explore with the number of elite bees
     * @param value : number of elite sites
     */
    void setNumEliteSites(int value);
    /**
     * @brief Sets the number of other bees in the Bees estimation algorithm; these are the number of bees assigned to
     * explore the best sites that aren't elite sites
     * @param value : number of other bees
     */
    void setNumOtherBees(int value);
    /**
     * @brief Sets the number of sub runs in the Bees estimation algorithm
     * @param value : number of Bees sub runs
     */
    void setNumSubRuns(int value);
    /**
     * @brief Sets the content for the output text edit widget
     * @param msg : the content to use for the output text edit widget
     */
    void setOutputTE(QString msg);
    /**
     * @brief Sets the run button label; used for setting to either Run or Running
     * @param label : value of the run button label
     */
    void setRunButtonLabel(QString label);

signals:
    /**
     * @brief Signal emitted when the user clicks the Add to Review button; it adds the
     * current Run meta-data to the Review GUI
     */
    void AddToReview();
    /**
     * @brief Signal to notify the main routine to set the state of the Bees algorithm to
     * deterministic (if true) and stochastic (if false)
     * @param state : state of Bees algorithm
     */
    void BeesSetDeterministic(bool state);
    /**
     * @brief Signal sent to check all Estimation tables for completeness
     */
    void CheckAllEstimationTablesAndRun();
//  void DimScalarBiomassControls(bool dim);
    /**
     * @brief Signal to notify main routine to set the enable state of all of its Run buttons
     * @param state : enable state of application's Run buttons
     */
    void EnableRunButtons(bool state);
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
     * @brief Callback invoked when user clicks the Add to Review button
     */
    void callback_AddToReviewPB();
    /**
     * @brief Callback invoked when the user selects an Averagin Algorithm for the multi-run feature
     * @param averagingAlgorithm : the selected averaging algorithm
     */
    void callback_AveragingAlgorithmCMB(QString averagingAlgorithm);
//    void callback_EnableSurveyQ(const QString biomassType,
//                                const bool enable,
//                                const bool checked);
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
     * @brief Callback invoked when the user clicks the Using Pct buttin
     */
    void callback_EnsembleUsingPctPB();
    /**
     * @brief Callback invoked when the user changes the Using Amount combobox
     * @param value : current Using Amount value
     */
    void callback_EnsembleUsingAmountCMB(QString value);
    /**
     * @brief Callback invoked when the user checks or unchecks the Ensemble Set Deterministic button
     * @param state : if true - deterministic, else if false - stochastic
     */
    void callback_EnsembleSetDeterministicCB(int state);
//  void callback_EstimateSurveyQCB(int state);
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
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user changes the Objective Criterion
     * @param objectiveCriterion : the objective criterion selected
     */
    void callback_ObjectiveCriterionCMB(QString objectiveCriterion);
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when user needs to query the states of all the Estimate run check boxes
     * @param unused : unused
     */
    void callback_RefreshEstimateRunBoxes(int unused);
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_ReloadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user saves the model on the Setup -> Model Setup GUI
     */
    void callback_SaveSettings();
    /**
     * @brief Callback invoked when user changes the scaling algorithm
     * @param scalingAlgorithm : new scaling algorithm selected
     */
    void callback_ScalingCMB(QString scalingAlgorithm);
    /**
     * @brief Callback invoked when the user checks or unchecks the Model Algorithm Set Deterministic button
     * @param state : if true - deterministic, else if false - stochastic
     */
    void callback_SetDeterministicCB(int state);
    /**
     * @brief Callback invoked when main routine needs to update Estimate checkboxes
     * @param EstimateRunBoxes : Names and states of Estimate checkboxes to update
     */
    void callback_SetEstimateRunCheckboxes(
            std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes);
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
     * @brief Callback invoked when the user clicks the lock button for the time units
     * @param isChecked : if true - units are locked, else if false - units may be changed
     */
    void callback_TimeUnitsLockPB(bool isChecked);

};

#endif // NMFESTIMATIONTAB7_H
