/**
 * @file ClearOutputDialog.h
 * @brief GUI definition for the Algorithm-Specific ClearOutputDialog widget
 *
 * This file contains the GUI definitions for the Algorithm-Specific Clear
 * Output Dialog. This dialog allows the user to clear output data
 * obtained from a specific combination of Model Estimation algorithms.
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

#include <QTableWidget>
#include <QComboBox>

/**
 * @brief Clear Dialog
 *
 * This dialog allows the user to clear output data from specific
 * combinations of Algorithm, Minimizer, Objective Criterion, and
 * Scaling Algorithm settings.
 *
 */
class ClearOutputDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;

    QVBoxLayout* MainLAYT;
    QHBoxLayout* BtnLAYT;
    QLabel*      MainLBL;
    QLabel*      AlgorithmLBL;
    QLabel*      MinimizerLBL;
    QLabel*      ObjectiveCriterionLBL;
    QLabel*      ScalingLBL;
    QComboBox*   AlgorithmCMB;
    QComboBox*   MinimizerCMB;
    QComboBox*   ObjectiveCriterionCMB;
    QComboBox*   ScalingCMB;
    QPushButton* CancelPB;
    QPushButton* OkPB;

    void loadWidgets();

public:
    /**
     * @brief ClearOutputDialog : class constructor
     * @param parent : the parent widget (e.g., the main window)
     * @param databasePtr : the pointer to the application database
     */
    ClearOutputDialog(QWidget*     parent,
                      nmfDatabase* databasePtr);
    virtual ~ClearOutputDialog() {}

    /**
     * @brief Gets the selected algorithm name from the GUI
     * @return Algorithm selected by the user
     */
    std::string getAlgorithm();
    /**
     * @brief Gets the selected minimizer name from the GUI
     * @return Minimizer selected by the user
     */
    std::string getMinimizer();
    /**
     * @brief Gets the selected objective criterion name from the GUI
     * @return Objective Criterion selected by the user
     */
    std::string getObjectiveCriterion();
    /**
     * @brief Gets the selected scaling name from the GUI
     * @return Scaling selected by the user
     */
    std::string getScaling();

private Q_SLOTS:
    /**
     * @brief Callback invoked when the user selects an item from the Algorithm combo box
     * @param algorithm : algorithm selected by the user
     */
    void callback_AlgorithmCMB(QString algorithm);
    /**
     * @brief Callback invoked when the user selects an item from the Minimizer combo box
     * @param minimizer : minimizer selected by the user
     */
    void callback_MinimizerCMB(QString minimizer);
    /**
     * @brief Callback invoked when the user selects an item from the Objective Criterion combo box
     * @param objectiveCriterion : objective criterion selected by the user
     */
    void callback_ObjectiveCriterionCMB(QString objectiveCriterion);
};

