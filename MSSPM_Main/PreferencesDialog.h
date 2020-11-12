/**
 * @file PreferencesDialog.h
 * @brief GUI definition for the PreferencesDialog dialog class
 *
 * This file contains the GUI definitions for the PreferencesDialog dialog class.
 * The application preferences allow the user to customize various aspects
 * of the user interface.
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


/**
 * @brief Preferences Dialog
 *
 * The Dialog contains the application preferences such as if the window color
 * scheme is light (the default) or dark.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;

    QVBoxLayout* MainLT;
    QHBoxLayout* BtnLT;
    QHBoxLayout* StyleLT;
    QHBoxLayout* MShot1LT;
    QHBoxLayout* MShot2LT;
    QHBoxLayout* MShot3LT;
    QLabel*      StyleLBL;
    QComboBox*   StyleCMB;
    QPushButton* CancelPB;
    QPushButton* OkPB;
    QLabel*      MShot1LBL;
    QLabel*      MShot2LBL;
    QLabel*      MShot3LBL;
    QComboBox*   MShot1CMB;
    QSpinBox*    MShot2SB;
    QSpinBox*    MShot3SB;

signals:
    void SetStyleSheet(QString style);

public:
    /**
     * @brief PreferencesDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     * @param databasePtr : the pointer to the application database
     */
    PreferencesDialog(QWidget*     parent,
                      nmfDatabase* databasePtr);
    virtual ~PreferencesDialog() {}

    void loadWidgets();

private Q_SLOTS:
    void callback_StyleCMB(QString algorithm);
};

