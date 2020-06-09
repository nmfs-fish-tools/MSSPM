/**
 * @file MultiScenarioSaveDlg.h
 * @brief GUI definition for the MultiScenarioSaveDlg widget class
 *
 * This file contains the GUI definition for the MultiScenario Save Dialog.
 * It allows the user to assign Forecasts to Scenarios.  Scenarios may then
 * be graphically viewed as a collected of Forecast plots.
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

#pragma once

#include "nmfDatabase.h"
#include "nmfLogger.h"
#include "nmfConstantsMSSPM.h"


/**
 * @brief Dialog to Save Multiple Forecasts
 *
 * Users may save multiple Forecasts into a Scenario for comparative
 * analysis.
 */
class MultiScenarioSaveDlg : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_DatabasePtr;
    std::string  m_ForecastName;
    nmfLogger*   m_Logger;
    std::map<QString,QStringList> m_OrderedForecastLabelsMap;
    std::string  m_ProjectSettingsConfig;
    std::string  m_ScenarioName;

    QVBoxLayout* MainVLT;
    QHBoxLayout* BtnHLT;
    QHBoxLayout* ScenarioHLT;
    QHBoxLayout* ForecastHLT;
    QHBoxLayout* ShowSortHLT;
    QHBoxLayout* ForecastLabelHLT;
    QLabel*      ScenarioNameLBL;
    QLabel*      ForecastLabelLBL;
    QLabel*      ShowSortLBL;
    QComboBox*   ScenarioNameCMB;
    QComboBox*   ForecastLabelCMB;
    QPushButton* DelScenarioPB;
    QPushButton* DelForecastPB;
    QPushButton* NewScenarioPB;
    QPushButton* SetForecastPB;
    QPushButton* CancelPB;
    QPushButton* OkPB;
    QPushButton* SetOrderPB;
    QPushButton* ShowSortPB;
    QPushButton* RenScenarioPB;
    QPushButton* RenForecastPB;

    // RSK - Using a QListWidget here instead of a QListView as the latter
    // had a problem with losing items if they were dropped on top of another item.
    QListWidget* ForecastLabelLW;

    bool getForecastData(
            const std::string& forecastName,
            std::vector<std::string>& Species,
            std::vector<int>& Years,
            boost::numeric::ublas::matrix<double>& ForecastBiomass);
    void loadScenarioMap();
    void printMap(QString msg);
    void renameScenarioName(QString oldScenario,
                            QString newScenario);
    void renameForecastLabel(QString scenario,
                             QString oldForecast,
                             QString newForecast);

public:
    /**
     * @brief MultiScenarioSaveDlg : class constructor for a Scenario dialog that allows the user to assign Forecasts to Scenarios
     * @param parent : the tab widget into which this Estimation tab will be placed
     * @param databasePtr : pointer to the application database
     * @param logger : pointer to the application logger
     * @param sortedForecastLabelsMap : map of Forecasts per Scenario
     * @param currentScenario : name of Scenario that will store passed Forecast
     * @param forecastName : name of Forecast to add to Scenario
     */
    MultiScenarioSaveDlg(QTabWidget*  parent,
                         nmfDatabase* databasePtr,
                         nmfLogger*   logger,
                         std::string& projectSettingsConfig,
                         std::map<QString,QStringList>& sortedForecastLabelsMap,
                         std::string& currentScenario,
                         std::string  forecastName);
    virtual ~MultiScenarioSaveDlg() {}

    /**
     * @brief Gets the current Scenario name from the appropriate GUI widget
     * @return Returns the current Scenario name
     */
    std::string getScenarioName();
    /**
     * @brief Gets the current Forecast label from the appropriate GUI widget. Each
     * Forecast shown in a Scenario plot has its Forecast name as its label.
     * @return Returns the current Forecast label
     */
    std::string getForecastLabel();
    /**
     * @brief Gets the Scenario-Forecast map. Each Scenario consists of one or more Forecasts.
     * @param orderedForecastLabelsMap : The map containing the Forecast-Scenario association
     */
    void getOrderedForecastLabels(std::map<QString,
                                  QStringList>& orderedForecastLabelsMap);
    /**
     * @brief Loads all widgets for this GUI from database tables
     */
    void loadWidgets();

signals:
    /**
     * @brief Signal emitted to refresh the Scenario output once a Scenario has been modified
     */
    void RefreshOutput();

private Q_SLOTS:
    /**
     * @brief Callback invoked when the user selects from the Scenario Name GUI widget
     * @param scenario : current Scenario name
     */
    void callback_ScenarioNameCMB(QString scenario);
    /**
     * @brief Callback invoked when the user clicks the New Scenario button
     */
    void callback_NewScenarioPB();
    /**
     * @brief Callback invoked when the user clicks the Delete Scenario button
     */
    void callback_DelScenarioPB();
    /**
     * @brief Callback invoked when the user clicks the Rename Scenario button
     */
    void callback_RenScenarioPB();
    /**
     * @brief Callback invoked when the user clicks the Set Forecast Label button
     */
    void callback_SetForecastPB();
    /**
     * @brief Callback invoked when the user clicks the Delete Forecast from Scenario button
     */
    void callback_DelForecastPB();
    /**
     * @brief Callback invoked when the user clicks the Rename Current Forecast button
     */
    void callback_RenForecastPB();
    /**
     * @brief Callback invoked when the user clicks the OK to Save Current Forecast to Current Scenario button
     */
    void callback_OkPB();
    /**
     * @brief Callback invoked when the user clicks the Set Order and Close button
     */
    void callback_SetOrderPB();
    /**
     * @brief Callback invoked when the user clicks the Show Sort Order button
     */
    void callback_ShowSortPB();
};

