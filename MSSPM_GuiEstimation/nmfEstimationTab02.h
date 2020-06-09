/**
 * @file nmfEstimationTab02.h
 * @brief GUI definition for the nmfEstimation_Tab2 Harvest Data class
 *
 * This file contains the GUI definition for the Harvest Data page. This
 * page allows the user to enter/modify a table of data representing the
 * harvest per Species per year.
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
#ifndef NMFESTIMATIONTAB2_H
#define NMFESTIMATIONTAB2_H

/**
 * @brief Harvest Data
 *
 * This GUI class represents the harvest data for the year range and for each
 * Species. The user should save any new or modified harvest data prior to
 * progressing to the next tab.
 */
class nmfEstimation_Tab2: public QObject
{
    Q_OBJECT

    nmfLogger*                        m_Logger;
    nmfDatabase*                      m_DatabasePtr;
    std::string                       m_ProjectDir;
    std::string                       m_ProjectSettingsConfig;
    std::string                       m_HarvestType;
    std::map<std::string,std::string> m_GroupBoxTitle;
    QStandardItemModel*               m_SModel;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab2_Widget;
    QTableView*  Estimation_Tab2_CatchTV;
    QGroupBox*   Estimation_Tab2_CatchGB;
    QPushButton* Estimation_Tab2_PrevPB;
    QPushButton* Estimation_Tab2_NextPB;
    QPushButton* Estimation_Tab2_LoadPB;
    QPushButton* Estimation_Tab2_SavePB;

    void readSettings();

public:
    /**
     * @brief nmfEstimation_Tab2 : class constructor for the Harvest Parameters GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab2(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab2();

    /**
     * @brief Checks that all tables in this GUI are properly populated
     * @return Boolean value signifying if tables are properly loaded
     */
    bool areTablesOK();
    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @param MohnsRhoLabel : Optional Mohns Rho label used when running a Restrospective Analysis
     * @return
     */
    bool loadWidgets(QString MohnsRhoLabel);

    /**
     * @brief Sets the class harvest type variable to the appropriate value
     * @param harvestType : the harvest type selected by the user when defining the model
     */
    void setHarvestType(std::string harvestType);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user modifies the Harvest Form in the Setup -> Model Setup page
     * @param harvestForm : the harvest form selected by the user
     */
    void callback_HarvestFormChanged(QString harvestForm);

};

#endif // NMFESTIMATIONTAB2_H
