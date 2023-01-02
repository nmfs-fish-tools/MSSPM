/**
 * @file ScrollingTextDialog.h
 * @brief GUI definition for a generic dialog with just a scrolling text box in it
 *
 * This file contains the GUI definitions for the ScrollingTextDialog dialog class.
 * This GUI shows a text box of html text. Handy if want to show text with anchors/links and
 * other html tags.
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

#include <QPushButton>
#include <QLabel>
#include <QUiLoader>
#include <QVBoxLayout>

/**
 * @brief Dialog that displays scrollable text
 *
 * The Dialog allows the user to display an estimation complete message and have the
 * QDialog be position somewhere other than the center of the main window.
 *
 */
class EstimationCompleteDialog : public QDialog
{
    Q_OBJECT

    QWidget*      m_Widget;
    QPushButton*  m_OkPB;
    QLabel*       m_MsgLBL;
    QVBoxLayout*  m_Layout;

public:
    /**
     * @brief ScrollingTextDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     * @param title : the dialog window title
     */
    EstimationCompleteDialog(QWidget* parent,
                             const QString& title);
    virtual ~EstimationCompleteDialog() {}

    /**
     * @brief loadText : loads text into the scrollable area
     * @param text : html text message to display
     */
    void loadText(const QString& text);
    /**
     * @brief Sets the position of the dialog
     * @param x : x position of the dialog
     * @param y : y position of the dialog
     */
    void loadPosition(
            const int& x,
            const int& y);
};

