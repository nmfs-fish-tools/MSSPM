/**
 * @file nmfEstimationTab05.h
 * @brief GUI definition for the nmfEstimation_Tab5 Observation Data class
 *
 * This file contains the GUI definition for the Observation Data page. This
 * page allows the user to enter/modify a table of data that correspond to
 * Biomass of each Species for each year of data.
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


#ifndef NMFESTIMATIONTAB5_H
#define NMFESTIMATIONTAB5_H

/**
 * @brief Observation Data
 *
 * This table allows the user to enter and modify Observed Biomass
 * data for each Species for every year in the year range.
 */
class nmfEstimation_Tab5: public QObject
{

    Q_OBJECT

    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QStandardItemModel* m_SModelBiomass;
    QStandardItemModel* m_SModelCovariates;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab5_Widget;
    QTableView*  Estimation_Tab5_BiomassTV;
    QTableView*  Estimation_Tab5_CovariatesTV;
    QPushButton* Estimation_Tab5_PrevPB;
    QPushButton* Estimation_Tab5_NextPB;
    QPushButton* Estimation_Tab5_LoadPB;
    QPushButton* Estimation_Tab5_SavePB;
    QGroupBox*   Estimation_Tab5_CovariatesGB;

    void readSettings();

public:
    /**
     * @brief nmfEstimation_Tab5 : class constructor for the Observation Data GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab5(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab5();

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
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @param firstRowOnly : boolean to specify if the user just wants to reload only the first row (since the first row is not modifiable by the user in this tab)
     * @param MohnsRhoLabel : the Mohns Rho name used to identify the data if the run is part of a Retrospective Analysis
     * @return
     */
    bool loadWidgets(QString MohnsRhoLabel);

    bool loadWidgetsFirstRow();


signals:
    /**
     * @brief Signal notifies any other GUI showing similar data to refresh itself
     * @param showPopup : boolean signifying whether the application should pop up a successful reload acknowledgement
     */
    void ReloadSpecies(bool showPopup);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user saves a new model in Setup -> Model Setup
     */
    void callback_UpdateInitialObservedBiomass();
};

#endif // NMFESTIMATIONTAB5_H
