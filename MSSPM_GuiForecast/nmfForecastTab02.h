/**
 * @file nmfForecastTab02.h
 * @brief GUI definition for the nmfForecast_Tab2 Forecast Harvest Data class
 *
 * This file contains the GUI definition for the Harvest Data. The widgets
 * defined here allow the user to enter or modify the data needed for Harvest data
 * for a Forecast. The user may also edit the data by using the Multiplier check box
 * and by specifying either a Constant or Variable Multiplier value. User can set the
 * state of the GUI to auto run a Forecast after a Save.
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
#ifndef NMFFORECASTTAB2_H
#define NMFFORECASTTAB2_H


/**
 * @brief Forecast Harvest Data
 *
 * This allows the user to enter Harvest data for the years of the
 * Forecast. Multiplier widgets help the user in calculating Harvest
 * data based upon the previous year's Harvest data.
 */
class nmfForecast_Tab2: public QObject
{
    Q_OBJECT

private:
    nmfDatabase*        m_DatabasePtr;
    std::string         m_HarvestTableName;
    nmfLogger*          m_Logger;
    std::string         m_ModelName;
    std::string         m_ProjectDir;
    std::string         m_ProjectName;
    int                 m_NumSignificantDigits;
    QString             m_PreviousUnits;
    QStandardItemModel* m_EstimationHarvestModel;
    QStandardItemModel* m_EstimationHarvestModelCatchFTC;
    QStandardItemModel* m_EstimationHarvestModelEffortFTC;

    QTabWidget*     Forecast_Tabs;
    QWidget*        Forecast_Tab2_Widget;
    QTableView*     Forecast_Tab2_HarvestTV;
    QGroupBox*      Forecast_Tab2_HarvestGB;
    QPushButton*    Forecast_Tab2_PrevPB;
    QPushButton*    Forecast_Tab2_NextPB;
    QPushButton*    Forecast_Tab2_LoadPB;
    QPushButton*    Forecast_Tab2_SavePB;
    QPushButton*    Forecast_Tab2_SetHarvestTypePB;
    QPushButton*    Forecast_Tab2_SetDefaultHarvestPB;
    QLineEdit*      Forecast_Tab1_NameLE;
    QCheckBox*      Forecast_Tab2_MultiplierCB;
    QComboBox*      Forecast_Tab2_MultiplierCMB;
    QDoubleSpinBox* Forecast_Tab2_MultiplierDSB;
    QCheckBox*      Forecast_Tab2_AutoSaveCB;
    QComboBox*      Forecast_Tab2_UnitsCMB;
    QCheckBox*      Forecast_Tab2_ConvertCB;

    bool loadTable();
    void loadUnits(const std::string& tableName);
    bool loadWidgets(const bool& readHarvestTypeFromTable);
    bool restoreData(int minRow, int minCol, int maxRow, int maxCol);
    void readSettings();
    void saveHarvestData(bool verbose);
    void setForecastHarvestLabel(const QString& harvestType);
    bool setHarvestTypeFromModel();
//  void setHarvestTableName(std::string harvestType);
    void updateUnitsTable(const std::string& harvestTable);

public:
    /**
     * @brief nmfForecast_Tab2 : class constructor for the Forecast Harvest Data GUI page
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab2(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab2();

    /**
     * @brief Notifies caller that data have been entered for this widget
     * @return Boolean where true signifies data have been entered and false that data have not been entered
     */
    bool areDataComplete();
    /**
     * @brief Clears all GUI widgets
     */
    void clearWidgets();
    /**
     * @brief Sets the enable-ness of the Forecast Harvest button and its label
     * @param enable : boolean describing if button should be enabled
     * @param buttonLabelToSet : what the button label should be
     */
    void enableHarvestTypeButton(
            const bool& enable,
            const QString& buttonLabelToSet);
    /**
     * @brief Returns the current units set by the user in the combo box
     * @return Current units QString (lbs, kg, mt)
     */
    QString getCurrentUnits();
    /**
     * @brief Returns the type of harvest as displayed in the harvest type pushbutton
     * @return The currently selected type of harvest data. Either "Catch:" or "Effort:"
     */
    QString getHarvestType();
    /**
     * @brief If checked, the data table will be automatically
     * converted as the user changes the Units combo box
     * @return True if convert checkbox is checked, false otherwise
     */
    bool isConvertChecked();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Sets the Units combo box
     * @param currentUnits : value with which to set the current units combo box
     */
    void setCurrentUnits(QString currentUnits);
    /**
     * @brief Sets the class variable representing the Estimation Harvest model table
     * @param Harvest_smodel : the currently set Estimation Harvest model table
     * @param CatchFTC_smodel : the Fit to Catch - Catch Estimation Harvest model table
     * @param EffortFTC_smodel : the Fit to Catch - Effort Estimation Harvest model table
     */
    void setForecastEstimationHarvestModels(
            QStandardItemModel* Harvest_smodel,
            QStandardItemModel* CatchFTC_smodel,
            QStandardItemModel* EffortFTC_smodel);
signals:
    /**
     * @brief Signal to notify the main routine to run the passed forecast
     * @param ForecastName : name of forecast to run
     * @param GenerateBiomass : boolean to specify whether or not to generate biomass data
     */
    void RunForecast(std::string ForecastName,
                     bool GenerateBiomass);
    /**
     * @brief Gets the Estimation Harvest Model and sets m_EstimationHarvestModel to it
     */
    void GetEstimationHarvestModel();
    /**
     * @brief Signal emitted to notify the main routine that it needs to reload the
     * Uncertainty Table in Forecast tab3. This is due to the fact that if the user
     * changes the forecast harvest type from Catch to Effort, the Catchability (q)
     * parameter must be exposed in the Uncertainty Parameters table.
     */
    void UpdateUncertaintyTable();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user checks the Auto Save checkbox. This checkbox saves the Harvest data after each edit and then automatically runs a Forecast.
     * @param checked : Boolean signifying the checked state of the checkbox
     */
    void callback_AutoSaveCB(bool checked);
    /**
     * @brief Callback invoked when the user clicks the Default Set button which will copy the last row of the Harvest Estimation table
     * to every row of the Forecast Harvest table.
     */
    void callback_SetDefaultHarvestPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user checks the Multiplier widget. This checkbox enables the user
     * to modify selected table rows either by a constant multiplier or by a multiplier and the previous row's value.
     * @param checked : checked state of the Multiplier widget
     */
    void callback_MultiplierCB(bool checked);
    /**
     * @brief Callback invoked when the user changes the type of Multiplier
     * @param type : type of Multiplier selected (current values are Constant and Variable)
     */
    void callback_MultiplierChangedCMB(QString type);
    /**
     * @brief Callback invoked when the user changes the Multiplier value
     * @param value : value of new harvest data multiplier
     */
    void callback_MultiplierChangedDSB(double value);
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user changes the Harvest table selection.  As the user changes the
     * selection, the multiplier type and value are used to update the selection.
     * @param sel : the newly selected items
     * @param desel : the newly deselected items
     */
    void callback_SelectionChanged(const QItemSelection &sel,
                                   const QItemSelection &desel);
    /**
     * @brief Callback invoked when the user clicks the Set Harvest Type button
     */
    void callback_SetHarvestTypePB();
    /**
     * @brief Callback invoked when the user changes the Units combo box
     * @param units : new units selected by the user
     */
    void callback_UnitsCMB(QString units);

};

#endif // NMFFORECASTTAB2_H
