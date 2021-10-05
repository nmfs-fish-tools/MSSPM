/**
 * @file EnsembleDialog.h
 * @brief GUI definition for the EnsembleDialog dialog class
 *
 * This file contains the GUI definitions for the EnsembleDialog dialog class.
 * This dialog shows the user the current multi run parameters as have been specified
 * in the GUI.
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


/**
 * @brief Dialog containing the Ensemble run parameter values
 *
 * The Dialog contains the Ensemble/Multi-run run parameters that are saved when the user saves a Multi-run.
 */
class EnsembleDialog : public QDialog
{
    Q_OBJECT

    std::string         m_ProjectDir;
    QStandardItemModel* m_SModel;
    QTableView*         m_ensembleTV;
    QWidget*            m_Parent;

public:
    /**
     * @brief EnsembleDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     */
    EnsembleDialog(QWidget* parent, std::string projectDir);
    virtual ~EnsembleDialog() {}

    /**
     * @brief Clears the current multi-run parameters
     */
    void clear();
    /**
     * @brief Gets the data associated with the passed in column number
     * @param col : column number whose data is to be returned
     * @return Encoded QString of column data
     */
    QString getColumnData(int col);
    /**
     * @brief Loads GUI widgets with data in the passed ensemble filename
     * @param ensembleFile : ensemble filename to load
     * @return True if loaded with no errors, false otherwise
     */
    bool loadWidgets(QString ensembleFile);

public Q_SLOTS:


};

