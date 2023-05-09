/**
 * @file nmfOutputTableWidgets.h
 * @brief GUI definition for the Control widgets class MSSPM_GuiOutputControls in the Output area
 *
 * This file contains the GUI definitions for the Output Control widgets. This
 * class describes the widgets that the user will use to control the selection,
 * behavior, and appearance of the Output plots.
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

#ifndef NMFOUTPUTTABLEWIDGETS_H
#define NMFOUTPUTTABLEWIDGETS_H

#include <QStringListModel>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QSpinBox>

/**
 * @brief
 * This class contains the widgets and callback functionality for the MSSPM
 * Output charts and controls.
 */
class nmfOutputTableWidgets: public QObject
{

    Q_OBJECT

    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectName;
    std::string         m_ModelName;
    QWidget*            m_Parent;
//    QAction*            m_SigDigActn;
    int                 m_NumSignificantDigits;
    QString             m_LastFileLoaded; // if empty, last loaded was from db, else from an import
    QString             m_LastLabel;
    QTableView*         m_LastTableView;
    QStringList         m_LastStatisticNames;

    void readSettings();
    void saveSettings();

public:
    /**
     * @brief nmfOutputTableWidgets : class constructor for GUI controls in Output dock widget
     * @param parent: the parent QWidget
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfOutputTableWidgets(QWidget*     parent,
                          nmfLogger*   logger,
                          nmfDatabase* databasePtr,
                          std::string& projectDir);
    virtual ~nmfOutputTableWidgets() {}

    void loadSummaryTable(QTableView* tableView,
                          const QString& label,
                          const QStringList& EstParamNames,
                          const std::string& Algorithm,
                          const std::string& Minimizer,
                          const std::string& ObjectiveCriterion,
                          const std::string& Scaling,
                          const QStringList& statisticNames);
    void exportSummaryTable(QTableView* tableView,
                            const QString& label,
                            const std::string& defaultFilenameCSV);
    void importSummaryTable(QTableView* tableView,
                            const QString& label,
                            const std::string& defaultFilenameCSV);
    void loadWidgets();
    void reloadLast(
            const std::string& Algorithm,
            const std::string& Minimizer,
            const std::string& ObjectiveCriterion,
            const std::string& Scaling,
            const QStringList& EstParamNames);
    void setLastVariables(QTableView* tableView,
                          const QString& label,
                          const QStringList& statisticNames);

signals:
    void ToggleSignificantDigits();

public slots:


};




#endif // NMFOUTPUTTABLEWIDGETS_H
