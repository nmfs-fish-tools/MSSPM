/**
 * @file nmfDiagnosticTab01.h
 * @brief GUI definition for the nmfDiagnostic_Tab1 Parameter Profiles Diagnostics Data class
 * @date Nov 21, 2019
 * @author Ronald Klasky
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
 *
 */
#ifndef NMFDIAGNOSTICTAB1_H
#define NMFDIAGNOSTICTAB1_H

#include <tuple>
#include <BeesAlgorithm.h>
#include "NLopt_Estimator.h"
#include "nmfConstantsMSSPM.h"

/**
 * @brief Diagnostic Tuple for Percent Variations
 *
 * A tuple that maps Species to growth rate variation, carrying capacity variation,
 * and fitness value.
 */
typedef std::tuple<QString, double, double, double> DiagnosticTuple;

/**
 * @brief Diagnostic Parameters
 *
 * This allows the user to enter Diagnostic parameters that describe the
 * percent variation from the Estimated Value and the number of Diagnostic Points
 * on either side of the Estimated Value.
 */
class nmfDiagnostic_Tab1: public QObject
{
    Q_OBJECT

private:
    nmfDatabase* m_DatabasePtr;
    nmfStructsQt::ModelDataStruct  m_DataStruct;
    QTabWidget*  m_Diagnostic_Tabs;
    QWidget*     m_Diagnostic_Tab1_Widget;
//  QComboBox*   m_Diagnostic_Tab1_ParameterCMB;
//  QLabel*      m_Diagnostic_Tab1_ParameterLBL;
    QComboBox*   Diagnostic_Tab1_SurfaceParameter1CMB;
    QComboBox*   Diagnostic_Tab1_SurfaceParameter2CMB;
    QSpinBox*    m_Diagnostic_Tab1_PctVarSB;
    QSpinBox*    m_Diagnostic_Tab1_NumPtsSB;
    QPushButton* m_Diagnostic_Tab1_RunPB;
    QRadioButton* m_Diagnostic_Tab1_UseLastSingleRunRB;
    QRadioButton* m_Diagnostic_Tab1_UseLastMultiRunRB;
    nmfLogger*   m_Logger;
    int          m_IsMultiRun;
    int          m_NumPoints;
    int          m_PctVariation;
    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    std::string  m_ModelName;
    std::string  m_MultiRunType;
    bool         m_UsingHPCFiles;
    std::map<std::string,std::string> m_OutputTableName;
    std::map<std::string,std::string> m_DiagnosticTableName;

    double calculateFitness(const int& SpeciesOrGuildNum,
                            const std::vector<std::pair<QString,double> >& ParameterData);
    void checkAlgorithmIdentifiersForMultiRun(
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling);
    bool isAggProd(std::string Algorithm,
                   std::string Minimizer,
                   std::string ObjectiveCriterion,
                   std::string Scaling);
    void loadCompetitionParameters(
            const bool&          isAggProd,
            const int&           NumSpecies,
            const int&           theNumGuilds,
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            std::vector<double>& Parameters);
    /**
     * @brief Reads the appropriate table and loads the passed in parameter
     * @param algorithm : name of estimation algorithm
     * @param minimizer : name of estimation algorithm minimizer function
     * @param objectiveCriterion : name of estimation algorithm objective criterion
     * @param scaling : name of estimation algorithm scaling function
     * @param parameterName : name of parameter whose estimation data are requested
     * @param estParameter : the estimation data to be returned to calling function
     * @return Boolean signifying if estimated parameters were found and loaded into vector
     */
    bool loadEstimatedParameter(const std::string&   algorithm,
                                const std::string&   minimizer,
                                const std::string&   objectiveCriterion,
                                const std::string&   scaling,
                                const QString&       parameterName,
                                std::vector<double>& estParameter);
    void loadEstimatedParameterZScore(const std::string&   algorithm,
                                      const std::string&   minimizer,
                                      const std::string&   objectiveCriterion,
                                      const std::string&   scaling,
                                      const QString&       parameterName,
                                      std::vector<double>& estParameter);
    void loadGrowthParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            const std::string&   isAggProd,
            std::vector<double>& Parameters,
            const int&           CurrentOffset,
            int&                 GrowthRateOffset,
            int&                 GrowthRateShapeOffset,
            int&                 GrowthRateCovarOffset,
            int&                 CarryingCapacityOffset,
            int&                 CarryingCapacityCovarOffset);
    void loadHarvestParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            std::vector<double>& Parameters,
            const int&           CurrentOffset,
            int&                 CatchabilityOffset,
            int&                 CatchabilityCovarOffset);
    void loadOutputParameters(
            const std::string&   TableName,
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectiveCriterion,
            const std::string&   Scaling,
            const std::string&   isAggProd,
            std::vector<double>& Parameters);
    void loadPredationParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            const std::string&   isAggProdStr,
            std::vector<double>& Parameters);
    void parameterToTableName(const std::string  whichTable,
                              const QString&     parameter,
                                    std::string& tableName);
    /**
     * @brief Reads the program settings file and sets appropriate class variables
     */
    void readSettings();
    void updateParameterTable(const int&         NumSpeciesOrGuilds,
                              const int&         NumPoints,
                              const std::string& Algorithm,
                              const std::string& Minimizer,
                              const std::string& ObjectiveCriterion,
                              const std::string& Scaling,
                              const std::string& isAggProd,
                              const QString&     ParameterName,
                              std::vector<DiagnosticTuple>& DiagnosticTupleVector);
    void updateParameterTable(const std::string& Algorithm,
                              const std::string& Minimizer,
                              const std::string& ObjectiveCriterion,
                              const std::string& Scaling,
                              const std::string& isAggProd,
                              const std::string& SurfaceType,
                              std::vector<DiagnosticTuple>& DiagnosticTupleVector);

public:
    /**
     * @brief nmfDiagnostic_Tab1 : class constructor
     * @param tabs : the tab widget into which this Diagnostic tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfDiagnostic_Tab1(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfDiagnostic_Tab1();

    /**
     * @brief Sets whether the Run button is enabled
     * @param state : boolean variable for the Run button enabled state
     */
    void enableRunButton(bool state);
    /**
     * @brief Gets the table name that corresponds to the passed parameter name
     * @param paramName : estimation parameter name
     * @return : Returns the name of the database table that corresponds to the passed in parameter name
     */
    std::string getTableName(QString paramName);
    /**
     * @brief Gets the previous run's variation parameter that was stored in settings (not in a database table)
     * @return The previously saved variation parameter
     */
    int getLastRunsPctVariation();
    /**
     * @brief Gets the previous run's number of diagnostic points that was stored in settings (not in a database table)
     * @return The previously saved number of diagnostic points
     */
    int getLastRunsNumPoints();
    /**
     * @brief Gets the first parameter to be used for the 3d parameter surface
     * @return the parameter name
     */
    QString getParameter1Name();
    /**
     * @brief Gets the second parameter to be used for the 3d parameter surface
     * @return the parameter name
     */
    QString getParameter2Name();
    /**
     * @brief Gets data for the current Guilds in the model
     * @param numGuilds : number of guilds found in the model
     * @param guildNames : names of the guilds found in the model
     */
    void getGuildInfo(int& numGuilds,
                      QStringList& guildNames);
    /**
     * @brief Gets data for the current Species in the model
     * @param numSpecies : number of species found in the model
     * @param speciesNames : names of the species found in the model
     */
    void getSpeciesInfo(int& numSpecies,
                        QStringList& speciesNames);
    /**
     * @brief Returns true if the Use Last Run was set to Multi Run
     * @return true if Use Last Run was checked, false otherwise
     */
    bool isSetLastRunMultiDiagnostics();
    /**
     * @brief Returns true if the Use Last Single Run radio button was checked
     * @return checked state of Use Last Single Run button
     */
    bool isSingleRunRBChecked();
    /**
     * @brief Returns true if the Use Last Single Run radio button was enabled
     * @return enabled state of Use Last Single Run button
     */
    bool isSingleRunRBEnabled();
    /**
     * @brief Returns true if the Use Last Multi Run radio button was checked
     * @return checked state of Use Last Multi Run button
     */
    bool isMultiRunRBChecked();
    /**
     * @brief Returns true if the Use Last Multi Run radio button was enabled
     * @return enabled state of Use Last Multi Run button
     */
    bool isMultiRunRBEnabled();
    /**
     * @brief Load widgets for this GUI panel
     */
    void loadWidgets();
    /**
     * @brief Save program settings
     */
    void saveSettings();
    /**
     * @brief Sets the class data structure variable
     * @param theDataStruct : the data structure containing the estimated parameter variables
     */
    void setDataStruct(nmfStructsQt::ModelDataStruct& theDataStruct);
    /**
     * @brief Sets the state of the class HPC flag so that the appropriate algorithms will be used to read the output estimated parameter tables
     * @param flag : state of the HPC flag, true if user just loaded an hpc run, false otherwise
     */
    void setHPCFlag(bool flag);
    /**
     * @brief Sets the value for number of points for the Number of Diagnostics Points GUI widget
     * @param numPoints : the number of points used in the GUI slider
     */
    void setNumPoints(int numPoints);
    /**
     * @brief Sets the enabled state of the Use Last Single Run radio button
     * @param isEnabled : true if enabled, false if disabled
     */
    void setSingleRunRBEnabled(bool isEnabled);
    /**
     * @brief Sets the enabled and checked state of the Use Last Single Run radio button
     * @param isEnabled : true if enabled, false if disabled
     * @param isChecked : true if checked, false if checked
     */
    void setSingleRunRBState(bool isEnabled, bool isChecked);
    /**
     * @brief Sets the enabled state of the Use Last Multi Run radio button
     * @param isEnabled : true if enabled, false if disabled
     */
    void setMultiRunRBEnabled(bool isEnabled);
    /**
     * @brief Sets the enabled and checked state of the Use Last Multi Run radio button
     * @param isEnabled : true if enabled, false if disabled
     * @param isChecked : true if checked, false if checked
     */
    void setMultiRunRBState(bool isEnabled, bool isChecked);
    /**
     * @brief Sets the value for the % Variation GUI widget
     * @param variation : the variation value used in the GUI slider
     */
    void setVariation(int variation);

signals:
    /**
     * @brief Signal emitted to set all of the output MSY checkboxes to the passed state
     * @param state : state to set Output Controls MSY boxes
     */
    void CheckMSYBoxes(bool state);
    /**
     * @brief Signal emitted to notify calling routine to set all Run buttons to the passed in state
     * @param state : state to set Run buttons (true=enabled, false=disabled)
     */
    void EnableRunButtons(bool state);
    /**
     * @brief Signal for loading the estimation algorithm's data structure
     */
    void LoadDataStruct();
    /**
     * @brief Signal that resets Output Control widgets for AggProd setting
     */
    void ResetOutputWidgetsForAggProd();
    /**
     * @brief Signal that sets Output GUI widgets based upon chart type
     * @param type : type of chart selected
     * @param method : type of diagnostic desired
     */
    void SetChartType(std::string type, std::string method);
    /**
     * @brief Signals the main application to set the HPC flag in this class
     */
    void SetDiagnosticHPCFlag();
    /**
     * @brief Signals to the main app that user is not running a retrospective analysis
     * @param state : true if running mohn's rho, false otherwise
     */
    void SetRetrospectiveAnalysis(bool state);

public slots:
    /**
     * @brief Callback for when the Run button is pressed
     */
    void callback_RunPB();
    /**
     * @brief Callback loads the parameter check boxes after the user clicks Save on the Model Setup Tab 4
     */
    void callback_UpdateDiagnosticParameterChoices();
    /**
     * @brief Callback invoked when the user clicks on the Use Last Multi Run radio button
     */
    void callback_UseLastMultiRunRB();
    /**
     * @brief Callback invoked when the user clicks on the Use Last Single Run radio button
     */
    void callback_UseLastSingleRunRB();
};

#endif

