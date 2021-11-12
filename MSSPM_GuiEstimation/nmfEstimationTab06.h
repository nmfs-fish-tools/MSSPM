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

    void addCovariateColumn(QString covariateName);
    void importTableData(
            const bool& firstLineReadOnly,
            const QString& type,
            const std::string& tableName,
            QTableView* tableView);
    void initializeCovariateTable();
    void initializeSpeciesParameterTable();
    void initializeInitialValuesAndRangesTable();
    void loadCSVFile(
            const bool& firstLineReadOnly,
            const std::string& tableName,
            QTableView* tableView);
    bool okSaveInitialValuesAndRangesChecks();
    void readSettings();
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
     * @brief Clears the GUI's widgets
     */
//    void clearWidgets();
    void loadCovariateTable();
    void loadCovariateAssignmentTable();
    void loadCovariateInitialValuesAndRangesTable();
    void loadInitialValuesAndRangesForEditableCells();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    void loadWidgets();
    void calculateCovariateScaleFactor(const int& col,
                                       double& min,
                                       double& max);

public Q_SLOTS:
    void callback_AddPB();
    void callback_ClearPB();
    void callback_DeletePB();
    void callback_ExportPB();
    void callback_ImportPB();
    void callback_LoadPB();
    void callback_NextPB();
    void callback_PrevPB();
    void callback_RenamePB();
    void callback_SavePB();
    void callback_SetEstimateRunCheckboxes(
            std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes);

};

#endif
