/** @file nmfForecastTab04.h
 * @brief GUI definition for the nmfForecast_Tab4 Forecast Run class
 *
 * This file contains the GUI definition for the Forecast Run. The widgets
 * defined here allow the user to run a Forecast, add the run to a MultiScenario plot,
 * and to view the run results in a text box.
 */
#ifndef NMFFORECASTTAB4_H
#define NMFFORECASTTAB4_H

#include "MultiScenarioSaveDlg.h"

/**
 * @brief Run Information
 *
 * This allows the user to run the Forecast as well as add the
 * Forecast to a Scenario.
 */
class nmfForecast_Tab4: public QObject
{
    Q_OBJECT

    std::string                   m_CurrentScenario;
    nmfDatabase*                  m_DatabasePtr;
    std::string                   m_EstimationOutputFile;
    std::string                   m_EstimationDataFile;
    std::string                   m_EstimationID;
    int                           m_FontSize;
    nmfLogger*                    m_Logger;
    std::string                   m_ProjectDir;
    std::string                   m_ProjectSettingsConfig;
    MultiScenarioSaveDlg*      m_SaveDlg;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab4_Widget;
    QTextEdit*   Forecast_Tab4_OutputTE;
    QPushButton* Forecast_Tab4_RunPB;
    QPushButton* Forecast_Tab4_SaveToMultiScenarioPB;
    QPushButton* Forecast_Tab4_PrevPB;
    QLineEdit*   Forecast_Tab4_OutputFileLE;
    QLineEdit*   Forecast_Tab4_DataFileLE;
    QLineEdit*   Forecast_Tab4_IDLE;
    QPushButton* Forecast_Tab4_OutputFileBrowsePB;
    QPushButton* Forecast_Tab4_OutputFileCreatePB;
    QPushButton* Forecast_Tab4_DataFileBrowsePB;
    QComboBox*   Forecast_Tab4_FontSizeCMB;
    QLineEdit*   Forecast_Tab1_NameLE;

    void readSettings();

public:
    /**
     * @brief nmfForecast_Tab4 : class constructor for the Run Forecast GUI page
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab4(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab4();

    /**
     * @brief Appends text to the Run Information window
     * @param msg - the text to append to the Run Information window
     */
    void appendOutputTE(QString msg);
    /**
     * @brief Clears the Run Information window
     */
    void clearOutputTE();
    /**
     * @brief Gets the current Forecast name
     * @return Returns the current Forecast name from the 1st Forecast GUI
     */
    std::string getCurrentForecastName();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Saves the current GUI page settings to a Qt settings file
     */
    void saveSettings();
    /**
     * @brief Sets the font size for the Run Information window
     * @param fontSize : the font size selected by the user
     */
    void setFontSize(int fontSize);
    /**
     * @brief Sets the output Scenario name for the Forecast being generated
     * @param scenario : name of the current Forecast's Scenario
     */
    void setOutputScenario(std::string scenario);
    /**
     * @brief Clears the Run Information window and adds the passed text
     * @param msg : text to put into the Run Information window
     */
    void setOutputTE(QString msg);

signals:
    /**
     * @brief Signal emitted after user clicks the Run Forecast button
     * @param forecastName : name of the Forecast to run
     * @param generateBiomass : boolean signifying if a Monte Carlo simulation is to be run
     */
    void RunForecast(std::string forecastName, bool generateBiomass);
    /**
     * @brief Signal emitted when the user updates a Scenario
     */
    void RefreshOutput();
    /**
     * @brief Sets the type of chart and passes the Forecast-Scenario map
     * @param type : type of chart desired
     * @param sortedForecastLabelsMap : map of Forecast-Scenario data (a Scenario consists of one or more Forecasts)
     */
    void SetChartType(QString type,
                      std::map<QString,QStringList> sortedForecastLabelsMap);
    /**
     * @brief Signal emitted when a Scenario is generated. It causes the appropriate Output widgets to be set.
     */
    void UpdateOutputScenarios();
    /**
     * @brief Signal emitted when GUI needs to query the Scenario name from the Output Controls widgets
     */
    void QueryOutputScenario();
    /**
     * @brief Signal emitted to set the Scenario name in the appropriate Output Controls widget
     * @param scenario : name of Scenario to set
     */
    void SetOutputScenarioText(QString scenario);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Run Forecast button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when the user clicks Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user changes the font size
     * @param fontSize : new font size selected
     */
    void callback_FontSizeCMB(QString fontSize);
    /**
     * @brief Callback invoked when the user clicks the Multi-Scenario Forecast button
     */
    void callback_RunMultiScenarioPB();
    /**
     * @brief Callback invoked after the user updates a Scenario
     */
    void callback_RefreshOutput();
};

#endif // NMFFORECASTTAB4_H
