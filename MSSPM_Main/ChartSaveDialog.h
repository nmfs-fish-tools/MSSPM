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

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

/**
 * @brief This class displays a QDialog and allows the user to do a screen
 * grab of the displayed chart. The user may save the file to either a jpg
 * or png file format.
 */
class ChartSaveDialog : public QDialog
{
    Q_OBJECT

    QLineEdit*   m_filenameLE;
    QComboBox*   m_fileTypeCMB;
    QComboBox*   m_fileResCMB;
    QPushButton* m_filePubPB;
    QCheckBox*   m_createPdfCB;
    QSpinBox*    m_qualitySB;

public:

    /**
     * @brief Class  constructor, builds the dialog allowing the user to save the current chart image
     * @param parent : the parent widget (typically the main window)
     */
    ChartSaveDialog(QWidget* parent);
    virtual ~ChartSaveDialog() {}

    /**
     * @brief Gets the value of the check box determining if a pdf file should also be created
     * @return true/false representing whether a pdf file should be created
     */
    bool createPdf();
    /**
     * @brief Gets the filename for the chart image
     * @return The filename entered by the user
     */
    QString getFilename();
    /**
     * @brief Gets the pdf filename for the chart image
     * @return The pdf filename derived from the filenamed entered by the user
     */
    QString getFilenamePdf();
    /**
     * @brief Returns the image quality set in the spin box
     * @return The image quality: -1 default, 0 minimum (smallest file size), 100 max (largest file size)
     */
    int getQuality();
    /**
     * @brief Returns the desired image resolution in dpi
     * @return The dpi image resolution
     */
    int getResolution();

private Q_SLOTS:
    /**
     * @brief Callback invoked when the user pressed the Pub button to set image attributes for publication
     */
    void callback_filePubPB();
    /**
     * @brief Callback invoked when the user changes the file type combo box
     * @param fileType : name of new file type user has set the combo box
     */
    void callback_fileTypeCMB(const QString& fileType);

};

