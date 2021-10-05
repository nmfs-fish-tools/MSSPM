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
    std::string                       m_ProjectName;
    std::string                       m_ModelName;
    std::string                       m_HarvestType;
    std::map<std::string,std::string> m_GroupBoxTitle;
    QStandardItemModel*               m_SModel;
    int                               m_NumSignificantDigits;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab2_Widget;
    QTableView*  Estimation_Tab2_HarvestTV;
    QGroupBox*   Estimation_Tab2_HarvestGB;
    QPushButton* Estimation_Tab2_PrevPB;
    QPushButton* Estimation_Tab2_NextPB;
    QPushButton* Estimation_Tab2_LoadPB;
    QPushButton* Estimation_Tab2_SavePB;
    QPushButton* Estimation_Tab2_ImportPB;
    QPushButton* Estimation_Tab2_ExportPB;
    QLabel*      Estimation_Tab2_HarvestLBL;

    void loadCSVFile(std::string& tableName);
    void readSettings();
    void saveCSVFile(std::string& tableName);

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
     * @brief Sets the class harvest type variable to the appropriate value
     * @param harvestType : the harvest type selected by the user when defining the model
     */
    void setHarvestType(std::string harvestType);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Export button to save a .csv file
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user modifies the Harvest Form in the Setup -> Model Setup page
     * @param harvestForm : the harvest form selected by the user
     */
    void callback_HarvestFormChanged(QString harvestForm);
    /**
     * @brief Callback invoked when the user clicks the Import button to load a .csv file
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user wants to reload widgets from the main routine without a popup acknowledgement
     */
    void callback_LoadWidgets();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
};

#endif // NMFESTIMATIONTAB2_H
