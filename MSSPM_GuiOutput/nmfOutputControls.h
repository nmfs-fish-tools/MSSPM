/** @file nmfOutputControls.h
 * @brief GUI definition for the Control widgets class MSSPM_GuiOutputControls in the Output area
 *
 * This file contains the GUI definitions for the Output Control widgets. This
 * class describes the widgets that the user will use to control the selection,
 * behavior, and appearance of the Output plots.
 */

#ifndef MSSPM_GUIOUTPUTCONTROLS_H
#define MSSPM_GUIOUTPUTCONTROLS_H

#include <QStringListModel>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>

/**
 * @brief
 * This class contains the widgets and callback functionality for the MSSPM
 * Output charts and controls.
 */
class MSSPM_GuiOutputControls: public QObject
{

    Q_OBJECT

    nmfDatabase*       m_DatabasePtr;
    nmfLogger*         m_Logger;
    std::string        m_ProjectDir;
    std::string        m_ProjectSettingsConfig;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;
    QHash<QString,int> m_SpeciesHash;
    QStringListModel*  m_SpeciesOrGuildModel;

    QLabel*      OutputTypeLBL;
    QLabel*      OutputSpeciesLBL;
    QLabel*      OutputSpeListLBL;
    QLabel*      OutputMethodsLBL;
    QLabel*      OutputParametersLBL;
    QLabel*      OutputScenariosLBL;
    QLabel*      OutputAgeListLBL;
    QLabel*      OutputYAxisMinLBL;
    QLabel*      OutputScaleLBL;
    QLabel*      OutputLineBrightnessLBL;
    QSlider*     OutputLineBrightnessSL;
    QComboBox*   OutputSpeciesCMB;
    QComboBox*   OutputParametersCMB;
    QComboBox*   OutputMethodsCMB;
    QComboBox*   OutputScenariosCMB;
    QCheckBox*   OutputParametersCB;
    QPushButton* OutputParametersPB;
    QComboBox*   OutputTypeCMB;
    QSlider*     OutputYAxisMinSL;
    QComboBox*   OutputScaleCMB;
    QListView*   OutputAgeListLV;
    QListView*   OutputSpeListLV;
    QCheckBox*   OutputShowMSYCB;
    QCheckBox*   OutputShowFMSYCB;
    QCheckBox*   OutputShowBMSYCB;
    QLineEdit*   OutputShowBMSYLE;
    QLineEdit*   OutputShowMSYLE;
    QLineEdit*   OutputShowFMSYLE;
    QGroupBox*   ControlsGroupBox;

    void initConnections();
    void initWidgets();
    void enableMSYWidgets(bool state);
    bool getSpecies(int& NumSpecies, QStringList& SpeciesList);
    bool getGuilds( int& NumGuilds,  QStringList& GuildList);
    bool isAggProd();
    void loadSortedForecastLabels();
    void readSettings();

public:
    /**
     * @brief MSSPM_GuiOutputControls : class constructor for GUI controls in Output dock widget
     * @param controlsGroupBox : group box widget containing the output control widgets
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    MSSPM_GuiOutputControls(QGroupBox*   controlsGroupBox,
                            nmfLogger*   logger,
                            nmfDatabase* databasePtr,
                            std::string& projectDir);
    virtual ~MSSPM_GuiOutputControls();

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
     * @param state : state of the widgets (True or False)
     */
    void            enableBrightnessWidgets(bool state);
    /**
     * @brief Get the brightness factor set by the Forecast Run Brightness slider widget
     * @return The brightness value desired for the stochastic Forecast plots
     */
    double          getOutputBrightnessFactor();
    /**
     * @brief Get the currently selected Diagnostic method
     * @return The name of the Diagnostic method chosen by the user
     */
    QString         getOutputDiagnostics();
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
     * @brief Get the currently selected Output Chart type
     * @return The name of the chart type user wishes to view
     */
    QString         getOutputType();
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
     * @param checked : the state of the checkbox (checked or not)
     */
    void            setOutputParametersCB(bool checked);
    /**
     * @brief Sets the Output Diagnostics method widget with the passed method
     * @param method : the method to set the Output Diagnostics combobox widget
     */
    void            setOutputDiagnostics(QString method);
    /**
     * @brief Assure appropriate widgets are enabled/disabled when user selects Retrospectve Analysis
     */
    void            setForMohnsRho();

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
     * @brief Signal emiited when the user has pressed the Output controls button signifying they want to reset the 3d surface "current point" button
     */
    void SelectCenterSurfacePoint();
    /**
     * @brief Signal emiited when the user selects a 2d surface visualization chart type
     * @param isVisible : boolean set to True for 2d chart or False for 3d chart
     */
    void SetChartView2d(bool isVisible);
    /**
     * @brief Signal emitted when a Chart is to be shown
     * @param OutputType : the type of Chart to show
     * @param OutputSpecies : the Species on which to base the Chart to be shown
     */
    void ShowChart(QString OutputType, QString OutputSpecies);
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
    /**
     * @brief Signal emitted when the user has changed the Y Axis Minumun Value slider widget
     */
    void YAxisMinValueChanged(int value);

public slots:
    /**
     * @brief Callback invoked when the user selects from the Output Type combo box widget
     * @param outputType : the name of the output chart type selected
     */
    void callback_OutputTypeCMB(QString outputType);
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
    void callback_OutputParametersCB(int state);
    /**
     * @brief Callback invoked when the user checks the BMSY checkbox
     * @param state : state of the BMSY check box
     */
    void callback_OutputShowBMSYCB(int state);
    /**
     * @brief Callback invoked when the user checks the MSY checkbox
     * @param state : state of the MSY check box
     */
    void callback_OutputMSYCB(int val);
    /**
     * @brief Callback invoked when the user checks the FMSY checkbox
     * @param state : state of the FMSY check box
     */
    void callback_OutputShowFMSYCB(int val);
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
    void callback_OutputYAxisMinSL(int value);
    /**
     * @brief Callback invoked when the user clicks the Parameters button which resets the
     * current point to be the center point on the 3d data view's surface
     */
    void callback_OutputParametersPB();
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
     * @brief Callback invoked when the user selects an Output Scenario from the Forecast -> MultiScenario Forecast button popup
     * @param scenario : Scenario selected by the user
     */
    void callback_SetOutputScenario(QString scenario);
};




#endif // MSSPM_GUIOUTPUTCONTROLS_H
