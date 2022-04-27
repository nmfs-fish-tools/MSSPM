/**
 * @file nmfEstimationTab02.h
 * @brief GUI definition for the nmfEstimation_Tab2 Harvest Data class
 *
 * This file contains the GUI definition for the Harvest Data page. This
 * page allows the user to enter/modify a table of data representing the
 * harvest per Species per year.
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
#ifndef NMFESTIMATIONTAB2_H
#define NMFESTIMATIONTAB2_H

//#include <QSpacer>

/**
 * @brief Harvest Data
 *
 * This GUI class represents the harvest data for the year range and for each
 * Species. The user should save any new or modified harvest data prior to
 * progressing to the next tab.
 */
class nmfEstimation_Tab2: public QObject
{
    Q_OBJECT

    nmfLogger*                        m_Logger;
    nmfDatabase*                      m_DatabasePtr;
    std::string                       m_ProjectDir;
    std::string                       m_ProjectName;
    std::string                       m_ModelName;
    std::string                       m_HarvestType;
    std::vector<std::string>          m_HarvestTables;
    std::vector<QTableView*>          m_TableViews;
    int                               m_NumSignificantDigits;
    QString                           m_PreviousEffortUnits;
    QString                           m_PreviousCatchUnits;

    QWidget*     Estimation_Tab2_Widget;
    QTabWidget*  Estimation_Tabs;
    QTabWidget*  Estimation_Tab2_FitToCatchTB;
    QTableView*  Estimation_Tab2_FTCWeightsTV;
    QTableView*  Estimation_Tab2_CatchTV;
    QTableView*  Estimation_Tab2_FTCCatchTV;
    QTableView*  Estimation_Tab2_EffortTV;
    QTableView*  Estimation_Tab2_FTCEffortTV;
    QPushButton* Estimation_Tab2_PrevPB;
    QPushButton* Estimation_Tab2_NextPB;
    QPushButton* Estimation_Tab2_LoadPB;
    QPushButton* Estimation_Tab2_SavePB;
    QPushButton* Estimation_Tab2_ImportPB;
    QPushButton* Estimation_Tab2_ExportPB;
    QPushButton* Estimation_Tab2_SetAllWeightsPB;
    QPushButton* Estimation_Tab2_SetBiomassWeightsTo1PB;
    QPushButton* Estimation_Tab2_SetBiomassWeightsToHalfPB;
    QPushButton* Estimation_Tab2_SetBiomassWeightsTo0PB;
    QComboBox*   Estimation_Tab2_UnitsEffortCMB;
    QComboBox*   Estimation_Tab2_FTCUnitsEffortCMB;
    QComboBox*   Estimation_Tab2_UnitsCatchCMB;
    QComboBox*   Estimation_Tab2_FTCUnitsCatchCMB;
    QCheckBox*   Estimation_Tab2_ConvertEffortCB;
    QCheckBox*   Estimation_Tab2_FTCConvertEffortCB;
    QCheckBox*   Estimation_Tab2_ConvertCatchCB;
    QCheckBox*   Estimation_Tab2_FTCConvertCatchCB;
    QWidget*     Estimation_Tab2_EffortW;
    QWidget*     Estimation_Tab2_CatchW;

    bool isCatch();
    bool isEffort();
    bool isFitToCatch();
    bool loadCSVFile(const QString& absolutePath,
                     const QString& tableName,
                     QTableView* tableView,
                     const bool& verbose);
    bool loadTable(QTableView* tableView,
                   const std::string& tableName,
                   const bool& verbose);
    void loadUnits(const std::string& tableName);
    bool loadWeights(const bool& verbose);
    void readSettings();
    bool saveCSVFile(std::string& tableName,
                     QTableView* tableView,
                     const bool& verbose);
    bool saveTable(QTableView* tableView,
                   const std::string& tableName);
    bool saveFitWeights(const bool& verbose);
    void setAllBiomassWeights(const double& value);

public:
    /**
     * @brief nmfEstimation_Tab2 : class constructor for the Harvest Parameters GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab2(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab2();

    /**
     * @brief Checks that all tables in this GUI are properly populated
     * @return Boolean value signifying if tables are properly loaded
     */
    bool areTablesOK();
    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Returns the current units set by the user in the combo box
     * @return Current units QString (lbs, kg, mt)
     */
    QString getCurrentUnits();
    /**
     * @brief Gets the currently visible harvest model pointer
     * @return Returns the harvest model pointer
     */
    QStandardItemModel* getHarvestModel();
    /**
     * @brief Gets the Fit to Catch - Catch model pointer
     * (there's a different tableview for effort and the fit to catch effort tables)
     * @return Returns the Fit to Catch - Catch tableview model pointer
     */
    QStandardItemModel* getHarvestModelFTCCatch();
    /**
     * @brief Gets the Fit to Catch - Effort model pointer
     * (there's a different tableview for effort and the fit to catch effort tables)
     * @return Returns the Fit to Catch - Effort tableview model pointer
     */
    QStandardItemModel* getHarvestModelFTCEffort();
    /**
     * @brief If checked, the Catch data table will be automatically be
     * converted as the user changes the Units combo box
     * @return True if convert checkbox is checked, false otherwise
     */
    bool isConvertCatchChecked();
    /**
     * @brief If checked, the Effort data table will be automatically be
     * converted as the user changes the Units combo box
     * @return True if convert checkbox is checked, false otherwise
     */
    bool isConvertEffortChecked();
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
     * @brief Sets the class harvest type variable to the appropriate value
     * @param harvestType : the harvest type selected by the user when defining the model
     */
    void setHarvestType(std::string harvestType);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Export button to save a .csv file
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user modifies the Harvest Form in the Setup -> Model Setup page
     * @param harvestForm : the harvest form selected by the user
     */
    void callback_HarvestFormChanged(QString harvestForm);
    /**
     * @brief Callback invoked when the user clicks the Import button to load a .csv file
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user wants to reload widgets from the main routine without a popup acknowledgement
     */
    void callback_LoadWidgets();
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
     * @brief Callback invoked when the user clicks the Weights Set All button
     */
    void callback_SetAllWeightsPB();
    /**
     * @brief Callback invoked when the user clicks the B=0 button. This sets all of the Biomass weights
     * to 0 (and Catch weights to 1).
     */
    void callback_SetBiomassWeightsTo0PB();
    /**
     * @brief Callback invoked when the user clicks the B=1 button. This sets all of the biomass weights
     * to 1 (and catch weights to 0).
     */
    void callback_SetBiomassWeightsTo1PB();
    /**
     * @brief Callback invoked when the user clicks the B=½ button
     */
    void callback_SetWeightsEqualPB();
    /**
     * @brief Callback invoked when the user changes an individual cell
     * @param topLeftCell : index of top left tableview cell
     * @param bottomRightCell : index of bottom right tableview cell
     */
    void callback_SetWeightsPB(const QModelIndex& topLeftCell,
                               const QModelIndex& bottomRightCell);
    /**
     * @brief Callback invoked when the user changes the Catch Units combo box
     * @param units : new units selected by the user
     */
    void callback_UnitsCatchCMB(QString units);
    /**
     * @brief Callback invoked when the user changes the Effort Units combo box
     * @param units : new units selected by the user
     */
    void callback_UnitsEffortCMB(QString units);
};

#endif // NMFESTIMATIONTAB2_H
