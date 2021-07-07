/**
 * @file nmfEstimationTab07.h
 * @brief GUI definition for the nmfEstimation_Tab7 Estimation Run class
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

#ifndef NMFESTIMATIONTAB7_H
#define NMFESTIMATIONTAB7_H

/**
 * @brief Model Review GUI
 *
 * This GUI allows the user to view past model statistics and generate a model review summary.
 */
class nmfEstimation_Tab7: public QObject
{
    Q_OBJECT

    const int COLUMN_NAME  =  0;
    const int COLUMN_NOTES = 13;

    int           m_NumColumns;
    nmfDatabase*  m_DatabasePtr;
    nmfLogger*    m_Logger;
    std::string   m_ProjectDir;
    std::string   m_ProjectName;
    std::string   m_ModelName;
    QStandardItemModel* m_SModel;

    QTabWidget*   Estimation_Tabs;
    QWidget*      Estimation_Tab7_Widget;
    QTableView*   Estimation_Tab7_ModelReviewTV;
    QPushButton*  Estimation_Tab7_LoadPB;
    QPushButton*  Estimation_Tab7_GenerateSummaryPB;
    QPushButton*  Estimation_Tab7_PrevPB;
    QPushButton*  Estimation_Tab7_DeletePB;
    QPushButton*  Estimation_Tab7_ExportPB;
    QPushButton*  Estimation_Tab7_ImportPB;
    QCheckBox*    Estimation_Tab7_ShowHiddenCB;

    QString getModelEquation(QStandardItemModel* smodel,
                             const int& row);
    QString getEstimatedParameters(QStandardItemModel* smodel,
                                   const int& row);
    void loadModel(QStandardItemModel* smodel,
                   const int& row);
    void resizeColumns();
    void adjustColumnsForReadOnly();
    QStandardItem* createNewModelReviewItem(const int& col,
                                            const QString& value);
public:
    /**
     * @brief nmfEstimation_Tab7 : class constructor for the Model Review GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab7(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab7();

    void updateReviewList(const QStringList& rowList);

    /**
     * @brief Saves the current application settings to the Qt settings file
     */
    void saveSettings();
    /**
     * @brief Reads the current application settings from the Qt settings file
     */
    void readSettings();
    bool showHiddenFields();
    void clearWidgets();

signals:
    void LoadFromModelReview(nmfStructsQt::ModelReviewStruct);


public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Delete button
     */
    void callback_DeletePB();
    /**
     * @brief Callback invoked when the user clicks the Generate Model Summary button
     */
    void callback_GenerateSummaryPB();
    void callback_ShowHiddenCB(int state);
    void callback_LoadPB();
    void callback_ImportPB();
    void callback_ExportPB();

};

#endif // NMFESTIMATIONTAB7_H
