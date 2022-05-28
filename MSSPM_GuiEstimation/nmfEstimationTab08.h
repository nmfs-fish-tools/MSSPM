/**
 * @file nmfEstimationTab07.h
 * @brief GUI definition for the nmfEstimation_Tab8 Estimation Run class
 *
 * This file contains the GUI definition for the Model Review Estimation Data page.
 * This page allows the user to view past model statistics and settings. The user may
 * also generate summary reports from this page.
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

#include <QComboBox>
#include <QDesktopServices>
#include <QMessageBox>

#include "nmfConstantsMSSPM.h"
#include "nmfGrowthForm.h"
#include "nmfHarvestForm.h"
#include "nmfCompetitionForm.h"
#include "nmfPredationForm.h"
#include "nmfStructsQt.h"

#ifndef NMFESTIMATIONTAB8_H
#define NMFESTIMATIONTAB8_H

/**
 * @brief Model Review GUI
 *
 * This GUI allows the user to view past model statistics and generate a model review summary.
 */
class nmfEstimation_Tab8: public QObject
{
    Q_OBJECT

    const int COLUMN_NAME  =  0;
    const int COLUMN_NOTES = 14;

    int           m_NumColumns;
    nmfDatabase*  m_DatabasePtr;
    nmfLogger*    m_Logger;
    std::string   m_ProjectDir;
    std::string   m_ProjectName;
    std::string   m_ModelName;
    QStandardItemModel* m_SModel;
    int                 m_NumSignificantDigits;
    std::vector<std::string> m_ModelReviewFields = {
        "ModelName","rSquared","SSResiduals","AIC",
        "GrowthForm","HarvestForm","CompetitonForm","PredationForm","numRuns",
        "ObjectiveCriterion","EstimationAlgorithm","Minimizer","Scaling","DatabaseSnapshot","Notes",
        "isDeterministicBees","maxGenerations","numBees","numBestSites","numEliteSites",
        "numEliteBees","numOtherBees","neighborhoodSize","numSubRuns","isDeterministicNLopt",
        "isStopAfterValue","StopAfterValue","isStopAfterTime","StopAfterTime","isStopAfterIter",
        "StopAfterIter","isEstInitBiomassEnabled","isEstInitBiomassChecked",
        "isEstGrowthRateEnabled","isEstGrowthRateChecked","isEstCarryingCapacityEnabled",
        "isEstCarryingCapacityChecked","isEstCatchabilityEnabled","isEstCatchabilityChecked",
        "isEstCompAlphaEnabled","isEstCompAlphaChecked","isEstCompBetaSpeciesEnabled",
        "isEstCompBetaSpeciesChecked","isEstCompBetaGuildsEnabled","isEstCompBetaGuildsChecked",
        "isEstCompBetaGuildsGuildsEnabled","isEstCompBetaGuildsGuildsChecked","isEstPredRhoEnabled",
        "isEstPredRhoChecked","isEstPredHandlingEnabled","isEstPredHandlingChecked",
        "isEstPredExponentEnabled","isEstPredExponentChecked","isEstSurveyQEnabled",
        "isEstSurveyQChecked","isAMultiRun","AveAlg","HowToAverage","UsingWhat","UsingNumber","isUsingPct",
        "EnsembleFile","EstimatedParametersFile"};

    QTabWidget*   Estimation_Tabs;
    QWidget*      Estimation_Tab8_Widget;
    QTableView*   Estimation_Tab8_ModelReviewTV;
    QPushButton*  Estimation_Tab8_LoadModelPB;
    QPushButton*  Estimation_Tab8_LoadPB;
    QPushButton*  Estimation_Tab8_SavePB;
    QPushButton*  Estimation_Tab8_GenerateSummaryPB;
    QPushButton*  Estimation_Tab8_PrevPB;
    QPushButton*  Estimation_Tab8_DeletePB;
    QPushButton*  Estimation_Tab8_ExportPB;
    QPushButton*  Estimation_Tab8_ImportPB;
    QCheckBox*    Estimation_Tab8_ShowHiddenCB;

    void adjustColumnsForReadOnly();
    QStandardItem* createNewModelReviewItem(const int& col,
                                            const QString& value);
    QString getEstimatedParameters(QStandardItemModel* smodel,
                                   const int& row);
    QString getModelEquation(QStandardItemModel* smodel,
                             const int& row);
    void loadModel(QStandardItemModel* smodel,
                   const int& row);
    void resizeColumns();
    bool saveModelReviewTable();

public:
    /**
     * @brief nmfEstimation_Tab8 : class constructor for the Model Review GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab8(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab8();

    /**
     * @brief Clears the widgets of the Model Review table
     */
    void clearWidgets();
    /**
     * @brief Snapshots the data and returns the saved database filename
     * @return The name of the database snapshot file
     */
    QString generateDatabaseSnapshot();
    /**
     * @brief This method is used to reload the Model Review table. If the user changes their
     * significant digits preference, this table needs to be redrawn so as to accurately reflect that
     * preference. This method does an export and then an import (using a temp file) to accomplish this.
     */
    void loadWidgets();
    /**
     * @brief This method loads the Model Review table with out any significant digits logic.
     */
    void loadWidgetsNoSignificantDigits();
    /**
     * @brief Reads the current application settings from the Qt settings file
     */
    void readSettings();
    /**
     * @brief Saves the current application settings to the Qt settings file
     */
    void saveSettings();
    /**
     * @brief showHiddenFields
     * @return
     */
    bool showHiddenFields();
    //  void updateReviewList(const QStringList& rowList);
    /**
     * @brief This method updates the Model Review Table with the passed data
     * @param rowList : list of all of the row data with which to update the Model Review table
     */
    void updateModelReviewTable(const QStringList& rowList);

signals:
    /**
     * @brief Signal emitted when the user loads a row from the Model Review
     */
    void LoadFromModelReview(nmfStructsQt::ModelReviewStruct);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Delete button
     */
    void callback_DeletePB();
    /**
     * @brief Callback invoked when the user clicks the Export to CSV file button
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user clicks the Generate Model Summary button
     */
    void callback_GenerateSummaryPB();
    /**
     * @brief Callback invoked when the user clicks the Import from CSV file button
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user clicks the Load button after selecting a desired row
     */
    void callback_LoadModelPB();
    /**
     * @brief Callback invoked when the user clicks the Load button to reload the Model Review table
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Save (to database) button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user checks the show hidden columns checkbox
     * @param state : state of the checkbox (Checked or Unchecked)
     */
    void callback_ShowHiddenCB(int state);

};

#endif // NMFESTIMATIONTAB8_H
