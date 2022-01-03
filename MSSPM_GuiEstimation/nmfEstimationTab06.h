/**
 * @file nmfEstimationTab06.h
 * @author Ronald Klasky
 * @date Oct 20, 2020
 * @brief GUI definition for the nmfEstimation_Tab6 Environmental Covariates Data class
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
#ifndef NMFESTIMATIONTAB6_H
#define NMFESTIMATIONTAB6_H

#include <nmfStructsQt.h>

/**
 * @brief Covariate Data
 *
 * These widgets allow the user to input custom environmental covariate data and then apply
 * the desired covariates to the desired species-parameter combinations. The user enters
 * the covariate data time series. The user also enters the covariate intial value and
 * ranges so that the covariate coefficients may be estimated if desired.
 *
 */
class nmfEstimation_Tab6: public QObject
{
    Q_OBJECT

private:
    nmfLogger*   m_Logger;
    nmfDatabase* m_DatabasePtr;
    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    std::string  m_ModelName;
    QStandardItemModel* m_smodelC;
    QStandardItemModel* m_smodelSP;
    QStandardItemModel* m_smodelIR;
    std::vector<std::string> m_ParamNames;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab6_Widget;
    QPushButton* Estimation_Tab6_AddPB;
    QPushButton* Estimation_Tab6_ClearPB;
    QPushButton* Estimation_Tab6_DeletePB;
    QPushButton* Estimation_Tab6_RenamePB;
    QPushButton* Estimation_Tab6_ImportPB;
    QPushButton* Estimation_Tab6_ExportPB;
    QTableView*  Estimation_Tab6_CovariateTV;
    QTableView*  Estimation_Tab6_SpeciesParameterTV;
    QTableView*  Estimation_Tab6_InitialValuesTV;
    QPushButton* Estimation_Tab6_SavePB;
    QPushButton* Estimation_Tab6_LoadPB;
    QPushButton* Estimation_Tab6_NextPB;
    QPushButton* Estimation_Tab6_PrevPB;
    QPushButton* Estimation_Tab6_SetEstOffPB;
    QPushButton* Estimation_Tab6_ClearRangesPB;
    QPushButton* Estimation_Tab6_ViewNormalizedPB;
    QSpinBox*    Estimation_Tab6_SpeciesRangeSB;
    QComboBox*   Estimation_Tab6_SpeciesRangeCMB;
    QComboBox*   Estimation_Tab6_MinMaxCMB;
    QComboBox*   Estimation_Tab6_AlgTypeCMB;

    void addCovariateColumn(QString covariateName);
    void calculateCovariateScaleFactor(const int& col,
                                       double& min,
                                       double& max);
    void importTableData(
            const bool& firstLineReadOnly,
            const QString& type,
            const std::string& tableName,
            QTableView* tableView);
    void initializeCovariateTable();
    void initializeSpeciesParameterTable();
    void initializeInitialValuesAndRangesTable();
    bool isViewNormalizeToggled();
    void loadCSVFile(
            const bool& firstLineReadOnly,
            const std::string& tableName,
            QTableView* tableView);
    bool okSaveInitialValuesAndRangesChecks();
    void readSettings();
    bool saveCovariateAlgorithmType();
    bool saveCovariateTable();
    bool saveCovariateAssignmentTable();
    bool saveInitialValuesAndRangesTable();
    void saveCSVFile(
            const QString& type,
            QStandardItemModel* smodel,
            const std::string& tableName);
    void saveTableValuesToCSVFile(
            const QString& type,
            const std::string& tableName,
            QStandardItemModel* smodel);
    void setCovariateAlgorithmType(std::string& algType);

public:
    /**
     * @brief nmfEstimation_Tab6 : class constructor for the Environmental Covariates GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab6(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab6();

    /**
     * @brief Gets the current value for the covariate algorithm type
     * @return String signifying the currently selected covariate algorithm type
     */
    std::string getCovariateAlgorithmType();
    /**
     * @brief Loads the Covariate Algorithm Type into the 1st Covariate tab
     */
    void loadCovariateAlgorithmType();
    /**
     * @brief Loads the (2nd tab) Covariate Assignment Table
     */
    void loadCovariateAssignmentTable();
    /**
     * @brief Loads the (3rd tab) Covariate Initial Values and Ranges table
     */
    void loadCovariateInitialValuesAndRangesTable();
    /**
     * @brief Loads the (1st tab) Covariate table
     */
    void loadCovariateTable();
    /**
     * @brief Loads the initial values
     */
    void loadInitialValuesAndRangesForEditableCells();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    void loadWidgets();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user wants to add a new environmental covariate
     */
    void callback_AddPB();
    /**
     * @brief Callback invoked when the user wants to clear the initial values and range table
     */
    void callback_ClearInitialValuesAndRangePB();
    /**
     * @brief Callback invoked when the user wants to clear all of the covariate assignments
     */
    void callback_ClearPB();
    /**
     * @brief Callback invoked when the user wants to delete the currently selected covariate
     */
    void callback_DeletePB();
    /**
     * @brief Callback invoked when the user wants to export the data from the currently viewed table
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user wants to import a previously exported version of the
     * currently viewed table's data
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user wants to load the currently viewed table with the data
     * from the corresponding database table (i.e., since the last Save)
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked to move to the next page (i.e., GUI)
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked to move to the previous page (i.e., GUI)
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user wants to rename the currently selected Environmental Covariate
     */
    void callback_RenamePB();
    /**
     * @brief Callback invoked when the user wants to save the currently viewed table's data into the
     * corresponding database table
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked to allow only those parameters that have code implemented for Environmental Covariates
     * @param EstimateRunBoxes : vector of parameter boxes that have been checked
     */
    void callback_SetEstimateRunCheckboxes(
            std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes);
    /**
     * @brief Callback invoked when the user wants to effectively disable covariate estimation
     * on specific (or all) Species/Parameter combinations
     */
    void callback_SetEstOffPB();
    /**
     * @brief Callback invoked when the user wants to change the min/max range values manually or by using the spin box
     * @param value : value describes how the user wants to change the min/max range values
     */
    void callback_SpeciesRangeCMB(QString value);
    /**
     * @brief Callback invoked as the user changes the Spin Box values that represent the percent change that should
     * be made to either (or both) of the min/max range values
     * @param pct : percent change made to min/max range values
     */
    void callback_SpeciesRangeSB(int pct);
    /**
     * @brief Callback invoked when the user presses the view normalized covariate data toggle button
     * @param checked : describes if the button is pressed in (true) or pressed out (false)
     */
    void callback_ViewNormalizedPB(bool checked);

};

#endif
