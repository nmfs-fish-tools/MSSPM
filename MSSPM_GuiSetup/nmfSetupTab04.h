/**
 * @file nmfSetupTab04.h
 * @brief GUI definition for the Setup Model page class nmfSetup_Tab4
 *
 * This file contains the GUI definitions for the Setup Model page. This
 * page contains the GUI widgets that allow the user to enter and modify
 * data corresponding to general data (i.e., year range). In addition,
 * the user constructs their Model by selecting types from each of the 4 forms:
 * Growth, Harvest, Competition, and Predation. The user can then inspect the resulting formula
 * created in the Model Equation window and highlight each form for clarity.
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

#ifndef NMFSETUPTAB4_H
#define NMFSETUPTAB4_H

#include "LoadDlg.h"

#include "nmfStructsQt.h"


/**
 * @brief The Setup Tab 4 allows the user to enter and modify general Model Setup data
 *
 * This tab allows the user to define a Model which contains data related to the
 * current model such as run length, year range, and model form.
 */
class nmfSetup_Tab4: public QObject
{
    Q_OBJECT

    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    std::string  m_ModelName;
    bool         m_isDark;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    QDialog      m_loadDlg;
    QPushButton* m_cancelBtn;
    QPushButton* m_okBtn;
    std::vector<std::string> m_ModelPresetNames;
    std::map<std::string,std::vector<std::string> > m_ModelPresets;
    LoadDlg*     m_LoadDialog;
    QStringList  m_EstimatedParameters;

    QTabWidget*  Setup_Tabs;
    QLabel*      Setup_Tab4_ModelPresetsLBL;
    QPushButton* Setup_Tab4_GrowthHighlightPB;
    QPushButton* Setup_Tab4_HarvestHighlightPB;
    QPushButton* Setup_Tab4_PredationHighlightPB;
    QPushButton* Setup_Tab4_CompetitionHighlightPB;
    QPushButton* Setup_Tab4_CalcPB;
    QComboBox*   Setup_Tab4_FontSizeCMB;
    QComboBox*   Setup_Tab4_ModelPresetsCMB;
    QComboBox*   Setup_Tab4_GrowthFormCMB;
    QComboBox*   Setup_Tab4_PredationFormCMB;
    QComboBox*   Setup_Tab4_HarvestFormCMB;
    QComboBox*   Setup_Tab4_CompetitionFormCMB;
    QComboBox*   Setup_Tab4_ObsBiomassTypeCMB;
    QTextEdit*   SetupOutputTE;
    QLineEdit*   Setup_Tab4_ModelNameLE;
    QLineEdit*   Setup_Tab4_SystemCarryingCapacityLE;
    QTextEdit*   Setup_Tab4_ModelEquationTE;
    QWidget*     Setup_Tab4_Widget;
    QPushButton* Setup_Tab4_SavePB;
    QPushButton* Setup_Tab4_LoadPB;
    QPushButton* Setup_Tab4_DelPB;
    QPushButton* Setup_Tab4_NextPB;
    QPushButton* Setup_Tab4_PrevPB;
    QPushButton* Setup_Tab4_NewModelPB;
    QSpinBox*    Setup_Tab4_NumberOfRunsSB;
    QSpinBox*    Setup_Tab4_StartYearSB;
    QLineEdit*   Setup_Tab4_EndYearLE;
    QSpinBox*    Setup_Tab4_RunLengthSB;

    double calculateSystemCarryingCapacity();
    void   getHighlightColors(QString& growthHighlightColor,
                            QString& harvestHighlightColor,
                            QString& competitionHighlightColor,
                            QString& predationHighlightColor,
                            QString& growthColorName,
                            QString& harvestColorName,
                            QString& competitionColorName,
                            QString& predationColorName);
    bool   modelExists(QString ModelName);
    bool   saveModelData(bool verbose,
                                     std::string CurrentSettingsName);

    void clearWidgets();
    /**
     * @brief Populates new model data with data from other models in the project
     * @return True for success, false otherwise
     */
    bool populateNewModel();
    void readSettings();
    void setEstimatedParameterNames();
    void saveSettings();
    void setModelPreset(std::string modelName);
    void updateOutputWidget();
    bool loadDuplicateData(
            const std::string& tableName,
            const QStringList& SpeciesList,
            const boost::numeric::ublas::matrix<double>& data);
public:
    /**
     * @brief nmfSetup_Tab4 : class constructor
     * @param tabs : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfSetup_Tab4(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab4();

    /**
     * @brief Deletes the passed model
     * @param modelToDelete : name of Model to delete
     */
    void deleteModel(QString modelToDelete);
    /**
     * @brief Writes the current Model Equation and variable descriptions into the Model
     * Equation text box
     * @param label : Type of model (i.e., Single Species or Multi Species)
     * @param eqn : The equation comprised of the selected forms
     * @param key : The description of all of the variables comprising the current Model Equation
     */
    void drawEquation(QString label, QString eqn, QString key);
    /**
     * @brief Gets the QString list of estimated parameter names
     * @return QStringList of estimated parameter names
     */
    QStringList getEstimatedParameterNames();
    /**
     * @brief Gets the font size for the Model Equation text box
     * @return The font size for the Model Equation text box
     */
    int  getFontSize();
    /**
     * @brief Gets the Competition Form GUI combo box
     * @return The Competition Form GUI combo box widget
     */
    QComboBox* getCompetitionFormCMB();
    /**
     * @brief Returns the contents of the Equation text edit box
     * @return The model equation and variable definitions
     */
    QString getEquation();
    /**
     * @brief Gets the Growth Form GUI combo box
     * @return The Growth Form GUI combo box widget
     */
    QComboBox* getGrowthFormCMB();
    /**
     * @brief Gets the Harvest Form GUI combo box
     * @return The Harvest Form GUI combo box widget
     */
    QComboBox* getHarvestFormCMB();
    /**
     * @brief Gets the Model Presets GUI combo box
     * @return The Model Presets GUI combo box widget
     */
    QComboBox* getModelPresetsCMB();
    /**
     * @brief Gets the Predation Form GUI combo box
     * @return The Predation Form GUI combo box widget
     */
    QComboBox* getPredationFormCMB();
    /**
     * @brief Gets the RunLength-1 value from the GUI
     * @return The number of years in the current Run
     */
    int  getRunLength();
    /**
     * @brief Returns the name of the current Model
     * @return The current Model name
     */
    QString getModelName();
    /**
     * @brief Returns the observed biomass type to be entered by the user
     * @return  The observed biomass type: Absolute or Relative
     */
    QString getObsBiomassType();
    /**
     * @brief Returns whether or not Growth Form is highlighted
     * @return True if Growth Form is highlighted, else False
     */
    bool isGrowthFormHighlighted();
    /**
     * @brief Returns whether or not Harvest Form is highlighted
     * @return True if Harvest Form is highlighted, else False
     */
    bool isHarvestFormHighlighted();
    /**
     * @brief Returns whether or not Predation Form is highlighted
     * @return True if Predation Form is highlighted, else False
     */
    bool isPredationFormHighlighted();
    /**
     * @brief Returns whether or not Competition Form is highlighted
     * @return True if Competition Form is highlighted, else False
     */
    bool isCompetitionFormHighlighted();
    /**
     * @brief Returns whether or not current Predation Form is Agg Prod
     * @return True if Predation Form is Agg Prod else False
     */
    bool isAggProd();
    /**
     * @brief Returns whether or not current Predation Form is Type III
     * @return True if Predation Form is Type III else False
     */
    bool isTypeIII();
    /**
     * @brief Loads the model whose name has been saved in the Settings
     */
    void loadModel();
    /**
     * @brief Load all widgets for this Setup GUI page
     */
    void loadWidgets();
    /**
     * @brief Reloads Model settings for current Model
     */
    void reloadModelName();
    /**
     * @brief Saves Model parameters
     * @param RunChecks : boolean value used to determine if Model parameter checks should be run
     */
    void saveModel(bool RunChecks);
    /**
     * @brief Sets the value of the Start Year to the passed value
     * @param StartYear : new Start Year for all application data
     */
    void setStartYear(int StartYear);
    /**
     * @brief Sets the length of the current Run in years (including first and last years)
     * @param RunLength : number of years in current Run (including first and last years)
     */
    void setRunLength(int RunLength);
    /**
     * @brief Sets the Model Equation font size to the passed value
     * @param fontSize : new font size for the Model Equation
     */
    void setFontSize(int fontSize);
    /**
     * @brief Sets the button highlight colors to their appropriate values
     */
    void setHighlightColors();
    /**
     * @brief sets class m_isDark variable
     */
    void setIsDark(QString style);
    /**
     * @brief set boolean static datamember m_IsDark accordingly based on the QString style
     */
    void setLineEditColors();
    /**
     * @brief Sets the Model name to the passed argument
     * @param modelName : the new Model name
     */
    void setModelName(QString modelName);
    /**
     * @brief Unchecks the formula highlight buttons
     */
    void uncheckHighlightButtons();

signals:
    /**
     * @brief Signal emitted when the user changes the Competition form (needed by the Estimation Tab 3 page)
     * @param competitionForm : the current Competition Form name
     */
    void CompetitionFormChanged(QString competitionForm);
    /**
     * @brief Signal emitted when the user changes the Predation form (needed by the Estimation Tab 4 page)
     * @param predationForm : the current Predation Form name
     */
    void PredationFormChanged(QString predationForm);
    /**
     * @brief Signal emitted when the user needs to redraw the equation in the Model Equation text box
     */
    void RedrawEquation();
    /**
     * @brief Signal emitted when the user needs to reload the current widgets
     */
    void ReloadWidgets();
    /**
     * @brief Signal emitted when the user needs to save the Estimation Run settings
     */
    void SaveEstimationRunSettings();
    /**
     * @brief Signal emitted when the user needs to save the Main Application settings
     */
    void SaveMainSettings();
    /**
     * @brief Signal emitted so that the Estimate checkboxes in the Estimation Tab6 will be updated
     * @param EstimateRunBoxes : list of Estimate checkbox names and states to update
     */
    void SetEstimateRunCheckboxes(std::vector<nmfStructsQt::EstimateRunBox>  EstimateRunBoxes);
    /**
     * @brief Signal emitted after the user deletes an existing Model from the database
     */
    void ModelDeleted();
    /**
     * @brief Signal emitted after the user loads an existing Model from the database
     */
    void ModelLoaded();
    /**
     * @brief Signal emitted after used saves a new Model
     */
    void ModelSaved();
    /**
     * @brief Signal emitted to set the observed biomass type in Estimation Tab 5: Observation Data
     */
    void ObservedBiomassType(QString obsBiomassType);
    /**
     * @brief Signal emitted to update the initial Forecast year
     */
    void UpdateInitialForecastYear();
    /**
     * @brief Signal emitted to update the initial Observed Biomass values
     * @param obsBiomassType : type of observed biomass: Absolute or Relative
     */
    void UpdateInitialObservedBiomass(QString obsBiomassType);
    void UpdateDiagnosticParameterChoices();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user wants to re-calculate the total
     * System Carrying Capacity (K) value
     */
    void callback_CalcPB();
    /**
     * @brief Callback invoked when the user needs to clear the Model name
     */
    void callback_ClearModelName();
    /**
     * @brief Callback invoked when the user selects a Competition Form
     * @param name : name of the Competition Form selected
     */
    void callback_CompetitionFormCMB(QString name);
    /**
     * @brief Callback invoked when the user clicks the Competition Form highlight button
     */
    void callback_CompetitionHighlightPB();
    /**
     * @brief Callback invoked when the user clicks the Delete Model button
     */
    void callback_DelPB();
    /**
     * @brief Callback invoked when the user changes the Model Equation text box font size
     * @param fontSize : new font size chosen by the user
     */
    void callback_FontSizeCMB(QString fontSize);
    /**
     * @brief Callback invoked when the user selects a Growth Form
     * @param name : name of the Growth Form selected
     */
    void callback_GrowthFormCMB(QString name);
    /**
     * @brief Callback invoked when the user clicks the Growth Form highlight button
     */
    void callback_GrowthHighlightPB();
    /**
     * @brief Callback invoked when the user selects a Harvest Form
     * @param name : name of the Harvest Form selected
     */
    void callback_HarvestFormCMB(QString name);
    /**
     * @brief Callback invoked when the user clicks the Harvest Form highlight button
     */
    void callback_HarvestHighlightPB();
    /**
     * @brief Callback invoked when the user clicks the Load Settings button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user selects a Preset from the Model
     * Presets combo box
     * @param preset : name of Model Preset selected by the user
     */
    void callback_ModelPresetsCMB(QString preset);
    /**
     * @brief Callback invoked when the user clicks the New Model button
     */
    void callback_NewModelPB();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user selects the type of observed biomass to be used in the current model
     * @param obsBiomassType : The type of observed biomass data. Either Absolute or Relative.
     */
    void callback_ObsBiomassTypeCMB(QString obsBiomassType);
    /**
     * @brief Callback invoked when the user selects a Predation Form
     * @param name : name of the Predation Form selected
     */
    void callback_PredationFormCMB(QString name);
    /**
     * @brief Callback invoked when the user clicks the Predation Form highlight button
     */
    void callback_PredationHighlightPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked to update the End Year edit widget
     * @param value : value of new end year
     */
    void callback_UpdateEndYear(int value);
};

#endif // NMFSETUPTAB3_H
