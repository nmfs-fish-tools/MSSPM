/**
 * @file SimulatedBiomassDialog.h
 * @brief GUI definition for the SimulatedBiomassDialog dialog class
 *
 * This file contains the GUI definitions for the SimulatedBiomassDialog dialog class.
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

#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include "nmfSimulatedData.h"


/**
 * @brief SimulatedBiomass Dialog
 *
 * The Dialog contains the preferences for the user to select when creating simulated
 * biomass data.
 */
class SimulatedBiomassDialog : public QDialog
{
    Q_OBJECT

    QLabel*      InfoLBL;
    QLabel*      ErrorLBL1;
    QLabel*      ErrorLBL2;
    QLabel*      ErrorLBL3;
    QPushButton* ContinuePB;
    QPushButton* CancelPB;
    QLineEdit*   ErrorLE;
    QSpinBox*    ErrorSB;

    QVBoxLayout* MainLT;
    QHBoxLayout* BtnLT;
    QHBoxLayout* ErrorLT1;

    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    std::string  m_ModelName;
    nmfDatabase* m_Database;
    nmfLogger*   m_Logger;


public:
    /**
     * @brief SimulatedBiomassDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     * @param projectDir : string representing the directory that the project file is in
     * @param projectName : name of the current project
     * @param modelName : string representing the project settings config file
     * @param database : pointer to database object
     * @param logger : pointer to logger object)
     */
    SimulatedBiomassDialog(QWidget* parent,
                           std::string projectDir,
                           std::string projectName,
                           std::string modelName,
                           nmfDatabase* database,
                           nmfLogger* logger);
    virtual ~SimulatedBiomassDialog() {}
    QString getFilename();
    int getErrorPct();

private Q_SLOTS:
    void callback_ContinuePB();
};

