/**
 * @file nmfOutputControls.h
 * @brief GUI definition for the Control widgets class MSSPM_GuiOutputControls in the Output area
 *
 * This file contains the GUI definitions for the Output Control widgets. This
 * class describes the widgets that the user will use to control the selection,
 * behavior, and appearance of the Output plots.
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

#ifndef NMFOUTPUTCONTROLS_H
#define NMFOUTPUTCONTROLS_H

#include <QStringListModel>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QSpinBox>

/**
 * @brief
 * This class contains the widgets and callback functionality for the MSSPM
 * Output charts and controls.
 */
class nmfOutputControls: public QObject
{

    Q_OBJECT

    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectName;
    std::string         m_ModelName;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;
    QHash<QString,int>  m_SpeciesHash;
    QStringListModel*   m_SpeciesOrGuildModel;
    bool                m_IsAveraged;
    int                 m_NumSignificantDigits;
    double              m_MSY_Value;
    double              m_FMSY_Value;
    double              m_BMSY_Value;

    QLabel*      OutputChartTypeLBL;
    QComboBox*   OutputGroupTypeCMB;
    QLabel*      OutputSpeciesLBL;
    QLabel*      OutputSpeListLBL;
    QLabel*      OutputMethodsLBL;
    QLabel*      OutputParametersLBL;
    QLabel*      OutputScenariosLBL;
    QLabel*      OutputAgeListLBL;
    QLabel*      OutputYAxisMinLBL;
    QLabel*      OutputYAxisMaxLBL;
    QLabel*      OutputScaleLBL;
    QLabel*      OutputLineBrightnessLBL;
    QLabel*      OutputShowShadowLBL;
    QLabel*      OutputShowHistoricalDataLBL;
    QSlider*     OutputLineBrightnessSL;
    QComboBox*   OutputSpeciesCMB;
    QComboBox*   OutputParametersCMB;
    QComboBox*   OutputMethodsCMB;
    QComboBox*   OutputScenariosCMB;
    QCheckBox*   OutputParametersZScoreCB;
    QPushButton* OutputParameters2d3dPB;
    QPushButton* OutputParametersCenterPB;
    QPushButton* OutputParametersMinimumPB;
    QComboBox*   OutputChartTypeCMB;
    QSlider*     OutputYAxisMinSL;
    QSpinBox*    OutputYAxisMinSB;
    QSpinBox*    OutputYAxisMaxSB;
    QComboBox*   OutputScaleCMB;
    QListView*   OutputAgeListLV;
    QListView*   OutputSpeListLV;
    QCheckBox*   OutputShowBMSYCB;
    QCheckBox*   OutputShowMSYCB;
    QCheckBox*   OutputShowFMSYCB;
    QLineEdit*   OutputShowBMSYLE;
    QLineEdit*   OutputShowMSYLE;
    QLineEdit*   OutputShowFMSYLE;
    QGroupBox*   ControlsGroupBox;
    QCheckBox*   OutputShowHistoricalDataCB;
    QCheckBox*   OutputShowShadowCB;

    void enableMSYWidgets(bool state);
    bool getSpecies(int& NumSpecies, QStringList& SpeciesList);
    void initConnections();
    void initWidgets();
    bool isAggProd();
    void loadSortedForecastLabels();
    void updateChart();

public:
    /**
     * @brief MSSPM_GuiOutputControls : class constructor for GUI controls in Output dock widget
     * @param controlsGroupBox : group box widget containing the output control widgets
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfOutputControls(QGroupBox*   controlsGroupBox,
                            nmfLogger*   logger,
                            nmfDatabase* databasePtr,
                            std::string& projectDir);
    virtual ~nmfOutputControls();

    /**
     * @brief Clears the Biomass Maximum Sustained Yield line edit widget
     */
    void            clearOutputBMSY();
    /**
     * @brief Clears the Maximum Sustained Yield line edit widget
     */
    void            clearOutputMSY();
    /**
     * @brief Clears the Fishing Mortality Maximum Sustained Yield line edit widget
     */
    void            clearOutputFMSY();
    /**
     * @brief Enables the appropriate widgets once the user has completed a Retrospective Analysis
     */
    void            displayMohnsRho();
    /**
     * @brief Sets the appropriate states of the widgets that allow the user to change brightness of stochastice Forecast runs
     * @param state : enabled state of the widgets (True or False)
     */
    void            enableBrightnessWidgets(bool state);
    /**
     * @brief Returns the number of and the list of unique guilds
     * @param NumGuilds : number of guilds
     * @param GuildList : list of guild names
     * @return Returns true/false based upon if an error was encountered
     */
    bool            getGuilds(int& NumGuilds, QStringList& GuildList);

    int             getNumberSpecies();
    void            setSpeciesNum(int speciesNum);
    void            setAveraged(bool isAveraged);
    bool            displaying3dChart();
    bool            isAveraged();
    void            enableControls(bool state);
    bool            isSetToGuild();
    /**
     * @brief Get the brightness factor set by the Forecast Run Brightness slider widget
     * @return The brightness value desired for the stochastic Forecast plots
     */
    double          getOutputBrightnessFactor();
    /**
     * @brief Get the currently selected Chart type
     * @return The name of the Chart type chosen by the user
     */
    QString         getOutputChartType();
    /**
     * @brief Get the currently selected Diagnostic method
     * @return The name of the Diagnostic method chosen by the user
     */
    QString         getOutputDiagnostics();
    /**
     * @brief Get the currently selected group type
     * @return The name of the group type (i.e., Species, Guild, System)
     */
    QString         getOutputGroupType();
    /**
     * @brief Get the currently selected Diagnostic parameter
     * @return The name of the Diagnostic parameter chosen by the user
     */
    QString         getOutputParameter();
    /**
     * @brief Get the currently selected scale of the y-axis
     * @return The name of the scale (in groups of 000) (i.e., "000" or "000 000")
     */
    QString         getOutputScale();
    /**
     * @brief Get the currently selected Scenario name
     * @return The Scenario (i.e., collection of Forecasts) to view
     */
    QString         getOutputScenario();
    /**
     * @brief Get the currently selected Output Species name
     * @return The name of the Species whose output the user wishes to view
     */
    QString         getOutputSpecies();
    /**
     * @brief  Get the currently selected Output Species index value
     * @return The integer index of the Species whose output the user wishes to view
     */
    int             getOutputSpeciesIndex();
    /**
     * @brief Get the Species list widget (currently disabled) used for other possible chart types
     * @return The widget representing the viewport of the species listview
     */
    QWidget*        getListViewViewport();
    /**
     * @brief Get the list of selected indexes (currently disabled) used for other possible chart types
     * @return The list of selected model indexes
     */
    QModelIndexList getListViewSelectedIndexes();
    /**
     * @brief Get the Y Max slider value which allows the user to change the maximum value that appears on the y-axis.
     * @return The value set by the user on this slider widget
     */
//    int             getYMaxSliderVal();
    /**
     * @brief Get the Y Min slider value which allows the user to change the minimum value that appears on the y-axis.
     * @return The value set by the user on this slider widget
     */
    int             getYMinSliderVal();
    /**
     * @brief Gets the index of a Species from a previously created hash map
     * @param SpeciesName : the name of the species whose index is desired
     * @return The index of the Species passed as an argument
     */
    int             getSpeciesNumFromName(QString SpeciesName);
    /**
     * @brief Informs the user if the BMSY checkbox has been checked
     * @return The state of the BMSY checkbox
     */
    bool            isCheckedOutputBMSY();
    /**
     * @brief Informs the user if the MSY checkbox has been checked
     * @return The state of the MSY checkbox
     */
    bool            isCheckedOutputMSY();
    /**
     * @brief Informs the user if the FMSY checkbox has been checked
     * @return The state of the FMSY checkbox
     */
    bool            isCheckedOutputFMSY();
    /**
     * @brief Informs the user if the MSY checkbox has been enabled
     * @return The state of the MSY checkbox
     */
    bool            isEnabledOutputMSY();
    /**
     * @brief Informs the user if the FMSY checkbox has been enbled
     * @return The state of the FMSY checkbox
     */
    bool            isEnabledOutputFMSY();
    /**
     * @brief Informs the user if the Method combobox is set to Retrospective Analysis
     * @return Boolean signifying if combobox is set to Retrospective Analysis
     */
    bool            isSetToRetrospectiveAnalysis();
    /**
     * @brief Informs the user if the Show Historical Data checkbox is checked
     * @return The state of the Show Historical Data checkbox
     */
    bool            isHistoricalDataShown();
    /**
     * @brief Informs the user if the Show Shadow box is checked
     * @return The state of the Show Shadow box
     */
    bool            isShadowShown();
    /**
     * @brief Informs the user if the ZScore check box has been checked
     * @return The state of the ZScore check box
     */
    bool isSurfaceTypeZScore();
    /**
     * @brief Loads the Species list view widget that's used for specific (but currently disabled) Output chart types
     */
    void            loadSpeciesControlWidget();
    /**
     * @brief Loads all of the Output control widgets
     */
    void            loadWidgets();
    /**
     * @brief Resets the Output Type widget as well as reloading the Scenario widget
     */
    void            refresh();
    /**
     * @brief Reloads the Scenario widget
     */
    void            refreshScenarios();
    /**
     * @brief Saves any Output Controls specific values to the Qt Settings file
     */
    void            saveSettings();
    /**
     * @brief Sets the Species combobox to be the passed species
     * @param species : the species to set the species combo box
     */
    void setCurrentSpecies(QString species);
    /**
     * @brief Sets the Group type to one of: System, Guild:, or Species:
     * @param grouptType : the group type desired
     */
    void setOutputGroupType(QString grouptType);
    /**
     * @brief Sets the current species of the Species combobox widget to the passed species
     * @param species : the species to set the output species combo box to
     */
    void            setOutputSpecies(QString species);
    /**
     * @brief Sets the current index of the Species combobox widget to the passed index value
     * @param index : the index with which to set the Species combobox widget
     */
    void            setOutputSpeciesIndex(int index);
    /**
     * @brief Sets the current Output Type to the passed value
     * @param type : the name of the Output Type to set as the current Output Type
     */
    void            setOutputType(QString type);
    /**
     * @brief Sets the BMSY line edit widget to the passed value
     * @param value : QString value of the BMSY value to display (as read-only) to the user
     */
    void            setTextOutputBMSY(QString value);
    /**
     * @brief Sets the MSY line edit widget to the passed value
     * @param value : QString value of the MSY value to display (as read-only) to the user
     */
    void            setTextOutputMSY(QString text);
    void            resetTextOutputMSY();
    void            resetTextOutputBMSY();
    void            resetTextOutputFMSY();
    void            resetMSYWidgets();
    /**
     * @brief Sets the FMSY line edit widget to the passed value
     * @param value : QString value of the FMSY value to display (as read-only) to the user
     */
    void            setTextOutputFMSY(QString text);
    /**
     * @brief Sets the Forecast Labels map to the passed map. This map maps a Scenario name to a QStringList of Forecast names.
     * @param sortedForecastLabelsMap : map of Forecast name lists to Scenario names
     */
    void            setForecastLabels(std::map<QString,QStringList>& sortedForecastLabelsMap);
    /**
     * @brief Toggles between a 2d and 3d Diagnostics view
     * @param chartType : 2d or 3d
     */
    void            setOutputParameters2d3dPB(QString chartType);
    /**
     * @brief Sets the Output Diagnostics method widget with the passed method
     * @param method : the method to set the Output Diagnostics combobox widget
     */
    void            setOutputDiagnostics(QString method);
    /**
     * @brief Assure appropriate widgets are enabled/disabled when user selects Retrospectve Analysis
     */
    void            setForMohnsRho();
    void setForBiomassVsTime();
    void readSettings();

signals:
    /**
     * @brief Signal emitted to enable the appropriate main menu toolbar filter buttons
     * @param state : state of main menu toolbar filter buttons
     */
    void EnableFilterButtons(bool state);
    /**
     * @brief Signal emitted to notify listeners that the stochastic Forecast line brightnesses have changed
     * @param brightnessFactor : new value for stochastic Forecast line brightnesses
     */
    void ForecastLineBrightnessChanged(double brightnessFactor);
    /**
     * @brief Signal emitted to reset the main menu toolbar's filter buttons
     */
    void ResetFilterButtons();
    /**
     * @brief Signal emitted when the user has pressed the Output controls button signifying they
     * want to reset the 3d surface "current point" button to the center point
     */
    void SelectCenterSurfacePoint();
    /**
     * @brief Signal emitted when the user has pressed the Output controls button signifying they
     * want to reset the 3d surface "current point" button to the minimum point
     */
    void SelectMinimumSurfacePoint();
    /**
     * @brief Signal emitted when the user selects a 2d surface visualization chart type
     * @param isVisible : boolean set to True for 2d chart or False for 3d chart
     */
    void SetChartView2d(bool isVisible);
    /**
     * @brief Sets the 2 vector parameters that will be used in the 3d surface diagnostics comparison chart
     * @param selectedParameters : the 2 selected parameters to be used for the 3d surface plot parameters
     */
    void SetDiagnosticsChart3dParameters(QStringList selectedParameters);
    /**
     * @brief Signal emitted when a Chart is to be shown
     * @param OutputType : the type of Chart to show
     * @param OutputSpecies : the Species on which to base the Chart to be shown
     */
    void ShowChart(QString OutputType,
                   QString OutputSpecies);
    /**
     * @brief Signal emitted when a Chart is to be redrawn after te group type has been changed
     * @param type : the type of group for the current chart (i.e., Species, Guild, System)
     * @param isAveraged : true if viewing averaged data, false otherwise
     */
    void ShowChartBy(QString type,
                     bool isAveraged);
    /**
     * @brief Signal emitted when the user wants to show a MultiScenario chart
     * @param sortedForecastLabels : list of Forecast names to show in the MultiScenario chart
     */
    void ShowChartMultiScenario(QStringList sortedForecastLabels);
    /**
     * @brief Signal emitted when the user wants to show a 3d Diagnostic chart
     */
    void ShowDiagnosticsChart3d();
    /**
     * @brief Signal emitted when the user wants to show a Retrospective Analysis chart
     */
    void ShowChartMohnsRho();

public slots:
    /**
     * @brief Callback invoked when the user selects from the Output Group Type combobox.  The user
     * may select a Species, Guild, or System group type to be displayed in the chart.
     * @param outputGroupType : the type of group to be displayed (i.e., Species, Guild, System)
     */
    void callback_OutputGroupTypeCMB(QString outputGroupType);
    /**
     * @brief Callback invoked when the user selects from the Output Type combo box widget
     * @param outputType : the name of the output chart type selected
     */
    void callback_OutputChartTypeCMB(QString outputType);
    /**
     * @brief Callback invoked when the user selects from the Species combo box widget
     * @param species : the name of the Species the user selected
     */
    void callback_OutputSpeciesCMB(QString species);
    /**
     * @brief Callback invoked when the user selects from the Diagnostic Methods combo box widget
     * @param method : the name of the Diagnostic Method the user selected
     */
    void callback_OutputMethodsCMB(QString method);
    /**
     * @brief Callback invoked when the user selects from the Diagnostic Parameters combo box widget
     * @param parameter : the name of the Diagnostic Parameter the user selected
     */
    void callback_OutputParametersCMB(QString parameter);
    /**
     * @brief Callback invoked when the user selects from the Scenarios combo box widget
     * @param scenario : the name of the Scenario the user selected
     */
    void callback_OutputScenariosCMB(QString scenario);
    /**
     * @brief Callback invoked when the user checks the Parameters checkbox to specify a 2d or 3d Parameter data view.
     * @param state : state of the Parameter checkbox (unchecked - 2d data view, checked - 3d data view)
     */
    void callback_OutputParametersZScoreCB(int state);
    /**
     * @brief Callback invoked when the user checks the BMSY checkbox
     * @param state : state of the BMSY check box
     */
    void callback_OutputBMSYCB(int state);
    /**
     * @brief Callback invoked when the user checks the MSY checkbox
     * @param state : state of the MSY check box
     */
    void callback_OutputMSYCB(int val);
    /**
     * @brief Callback invoked when the user checks the FMSY checkbox
     * @param state : state of the FMSY check box
     */
    void callback_OutputFMSYCB(int val);
    /**
     * @brief Callback invoked when the user checks the Historical Data checkbox
     * @param unused - unused parameter
     */
    void callback_OutputShowHistoricalDataCB(int unused);
    /**
     * @brief Callback invoked when the user checks the Show Shadow check box
     * @param unused - unused parameter
     */
    void callback_OutputShowShadowCB(int unused);
    /**
     * @brief Callback invoked when the user selects from the Scale Factor combo box widget
     * @param scale : the y-axis scale selected, values are strings of 0 triples (i.e., "000" or "000 000")
     */
    void callback_OutputScaleCMB(QString scale);
    /**
     * @brief Callback invoked when the user modifies the Forecast line brightness slider
     * @param value : the value of the Forecast line brightness slider
     */
    void callback_OutputLineBrightnessSL(int value);
    /**
     * @brief Callback invoked when the user modifies the Y-Axis Minimum value slider
     * @param value : the minimum value of the y-axis to set
     */
    void callback_OutputYAxisMinSB(int value);
    /**
     * @brief Callback invoked when the user modifies the Y-Axis Maximum value slider
     * @param value : the maximum value of the y-axis to set
     */
    void callback_OutputYAxisMaxSB(int value);
    /**
     * @brief Callback invoked when the user modifies the Y-Axis Minimum value slider
     * @param value : the minimum value of the y-axis to set
     */
    void callback_OutputYAxisMinSL(int value);
    /**
     * @brief Callback invoked when the user clicks the Parameters button which resets the
     * current point to be the center point on the 3d data view's surface
     */
    void callback_OutputParametersCenterPB();
    /**
     * @brief Callback invoked when the user clicks the Parameters button which resets the
     * current point to be the minimum point on the 3d data view's surface
     */
    void callback_OutputParametersMinimumPB();
    /**
     * @brief Callback invoked to set Control widgets appropriately if model is an
     * AggProd model (i.e., inclusion of Guilds in Control widgets)
     */
    void callback_ResetOutputWidgetsForAggProd();
    /**
     * @brief Callback invoked when the user selects from the Scenarios combo box widget
     */
    void callback_LoadScenariosWidget();
    /**
     * @brief Callback invoked when the user clicks on the 2d/3d chart parameter button.
     */
    void callback_OutputParameters2d3dPB();
    /**
     * @brief Callback invoked when the user selects an Output Scenario from the Forecast -> MultiScenario Forecast button popup
     * @param scenario : Scenario selected by the user
     */
    void callback_SetOutputScenario(QString scenario);
    /**
     * @brief Callback invoked when the user changes the Model structure. Only the
     * parameters that pertain to the appropriate Model parameters should be visible.
     */
    void callback_UpdateDiagnosticParameterChoices();
    /**
     * @brief Callback invoked when the user needs to either check or uncheck all of the MSY boxes
     * @param state : checked state for all of the MSY boxes
     */
    void callback_CheckMSYBoxes(bool state);
};




#endif // NMFOUTPUTCONTROLS_H
