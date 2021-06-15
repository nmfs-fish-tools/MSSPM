/**
 * @file TableNamesDialog.h
 * @brief GUI definition for the TableNamesDialog dialog class
 *
 * This file contains the GUI definitions for the TableNamesDialog dialog class.
 * This GUI lists the names of the tables in the current database.
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
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief TableNamesDialog Dialog
 *
 * The Dialog contains the name of database, and tables within a list widgets.
 *
 */
class TableNamesDialog : public QDialog
{
    Q_OBJECT

    std::string                           m_ProjectDatabase;
    nmfDatabase*                          m_databasePtr;
    QWidget*                              m_TableNamesWidget;
    QPushButton*                          m_TableNamesOkPB;
    QListWidget*                          m_TableNamesLW;
    QLabel*                               m_DatabaseNameLB;
    QVBoxLayout*                          m_layout;

public:
    /**
     * @brief TableNamesDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     * @param databasePtr : the pointer to the application database
     * @param projectDatabase : the name of the database.
     */
    TableNamesDialog(QWidget*     parent,
                     nmfDatabase* databasePtr,
                     std::string  projectDatabase);
    virtual ~TableNamesDialog() {}
    /**
     * @brief loadTableNames : This function is called whenever a user tries to view the list of tables in the current database.
     */
    void loadTableNames();


private Q_SLOTS:

};

