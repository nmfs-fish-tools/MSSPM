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
    std::string         m_ProjectSettingsConfig;
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
     * @brief nmfForecast_Tab1 : class constructor
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
     * @brief Returns the forecast name the user inputted. This is the name of the forecast that will be generated.
     * @return Returns the string name of the forecast the user inputted
     */
    std::string getForecastName();
    /**
     * @brief Returns the seed value associated with the non-deterministic forecast run
     * @return The integer seed value to set if running a non-deterministic forecast
     */
    int         getSeed();
    /**
     * @brief Returns the start year associated with this forecast
     * @return The integer start year associated with this forecast
     */
    int         getStartForecastYear();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool        loadWidgets();
    /**
     * @brief Saves the GUIs settings to a Qt settings file
     */
    void        saveSettings();

signals:
    /**
     * @brief Signal emitted when a new forecast has been loaded
     * @param forecastLoaded : the name of the forecast loaded
     */
    void ForecastLoaded(std::string forecastLoaded);
    /**
     * @brief If the model is an AggProd model, set any output controls accordingly
     */
    void ResetOutputWidgetsForAggProd();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user clicks Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user clicks Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user clicks the Set button. It loads the forecast name with the name of the last project settings configuration.
     */
    void callback_SetNamePB();
//  void callback_PreviousRunCB(bool state);
//  void callback_AlgorithmCMB(QString algorithm);
//  void callback_MinimizerCMB(QString minimizer);
    /**
     * @brief Callback invoked when user changes the Number of Years in Run widget
     * @param numYears : Number of years in current forecast
     */
    void callback_RunLengthSB(int numYears);
    /**
     * @brief Callback invoked when user changes the deterministic checkbox. This checkbox is used to determine if the current forecast should run deterministically or stochastically.
     * @param checked : The checked state of the checkbox
     */
    void callback_DeterministicCB(int checked);
    /**
     * @brief Callback invoked when user changes the year range from the Setup -> Model Setup page
     */
    void callback_UpdateForecastYears();
};

#endif // NMFFORECASTTAB1_H
