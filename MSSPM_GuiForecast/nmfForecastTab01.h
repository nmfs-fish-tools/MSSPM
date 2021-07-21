/**
 * @file nmfForecastTab01.h
 * @brief GUI definition for the nmfForecast_Tab1 Forecast Setup Data class
 *
 * This file contains the GUI definition for the Forecast Setup Data. The widgets
 * defined here allow the user to enter or modify the data needed to set up a Forecast.
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
#ifndef NMFFORECASTTAB1_H
#define NMFFORECASTTAB1_H

#include "LoadForecastDlg.h"


/**
 * @brief Forecast Setup
 *
 * These widgets allow the user to enter Forecast setup information
 * such as Forecast year range, number of runs in Forecast, and
 * whether the Forecast is deterministic or stochastic.
 */
class nmfForecast_Tab1: public QObject
{
    Q_OBJECT

private:
    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectName;
    std::string         m_ModelName;
    QStandardItemModel* m_SModel;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab1_Widget;
    QPushButton* Forecast_Tab1_SetNamePB;
    QPushButton* Forecast_Tab1_NextPB;
    QPushButton* Forecast_Tab1_LoadPB;
    QPushButton* Forecast_Tab1_SavePB;
    QGroupBox*   Forecast_Tab1_PreviousRunGB;
    QLabel*      Forecast_Tab1_AlgorithmLBL;
    QLabel*      Forecast_Tab1_MinimizerLBL;
    QLabel*      Forecast_Tab1_ObjectiveCriterionLBL;
    QComboBox*   Forecast_Tab1_AlgorithmCMB;
    QComboBox*   Forecast_Tab1_MinimizerCMB;
    QComboBox*   Forecast_Tab1_ObjectiveCriterionCMB;
    QLineEdit*   Forecast_Tab1_StartYearLE;
    QLineEdit*   Forecast_Tab1_EndYearLE;
    QSpinBox*    Forecast_Tab1_RunLengthSB;
    QLineEdit*   Forecast_Tab1_NameLE;
    QSpinBox*    Forecast_Tab1_NumRunsSB;
    QSpinBox*    Forecast_Tab1_DeterministicSB;
    QCheckBox*   Forecast_Tab1_DeterministicCB;

    void loadForecast(std::string forecastToLoad);
    void readSettings();

public:
    /**
     * @brief nmfForecast_Tab1 : class constructor for the Forecast Setup GUI page
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab1(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab1();

    /**
     * @brief Clears all GUI widgets
     */
    void        clearWidgets();
    /**
     * @brief Returns the Forecast name the user inputted. This is the name
     * of the Forecast that will be generated.
     * @return Returns the string name of the Forecast the user inputted
     */
    std::string getForecastName();
    /**
     * @brief Returns the seed value associated with the non-deterministic forecast run
     * @return The integer seed value to set if running a non-deterministic forecast
     */
    int         getSeed();
    /**
     * @brief Returns the start year associated with this Forecast
     * @return The integer start year associated with this Forecast
     */
    int         getStartForecastYear();
    /**
     * @brief Returns boolean signifying if the deterministic box has been checked
     * @return Returns True/False describing if deterministic box has been checked
     */
    bool        isDeterministic();
    /**
     * @brief Returns boolean signifying if the found Forecast name is valid
     * @return True/False describing if the entered Forecast name is valid
     */
    bool        isValidForecastName();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool        loadWidgets();
    /**
     * @brief Saves the GUIs settings to a Qt settings file
     */
    void        saveSettings();
    /**
     * @brief Sets the deterministic state of the appropriate checkbox
     * @param isDeterministic : boolean describing deterministic state
     */
    void setDeterministic(bool isDeterministic);

signals:
    /**
     * @brief Signal emitted when a new Forecast has been loaded
     * @param forecastLoaded : the name of the Forecast loaded
     */
    void ForecastLoaded(std::string forecastLoaded);
    /**
     * @brief If the model is an AggProd model, set any output controls accordingly
     */
    void ResetOutputWidgetsForAggProd();
    /**
     * @brief Signal emitted to change the state of the Run button on Forecast Tab 4
     * @param enable : the enable state: true (enabled) or false (disabled)
     */
    void EnableRun(bool enable);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Set button. It
     * loads the Forecast name with the name of the last project
     * settings configuration.
     */
    void callback_SetNamePB();
//  void callback_PreviousRunCB(bool state);
//  void callback_AlgorithmCMB(QString algorithm);
//  void callback_MinimizerCMB(QString minimizer);
    /**
     * @brief Callback invoked when the user changes the Number of
     * Years in Run widget
     * @param numYears : Number of years in current Forecast
     */
    void callback_RunLengthSB(int numYears);
    /**
     * @brief Callback invoked when the user changes the Deterministic
     * checkbox. This checkbox is used to determine if the current
     * Forecast should run deterministically or stochastically.
     * @param checked : The checked state of the checkbox
     */
    void callback_DeterministicCB(int checked);
    /**
     * @brief Callback invoked when the user changes the Year Range
     * from the Setup -> Model Setup page
     */
    void callback_UpdateForecastYears();
};

#endif // NMFFORECASTTAB1_H
