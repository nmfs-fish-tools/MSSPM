/**
 * @file nmfForecastTab03.h
 * @brief GUI definition for the nmfForecast_Tab3 Forecast Uncertainty Parameter Data class
 *
 * This file contains the GUI definition for the Uncertainty Parameter Data. The widgets
 * defined here allow the user to enter or modify the data needed for Uncertainty Parameter values
 * for a Forecast. User can set the state of the GUI to auto run a Forecast after a Save.
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
#ifndef NMFFORECASTTAB3_H
#define NMFFORECASTTAB3_H


/**
 * @brief Uncertainty Parameters
 *
 * This allows the user to enter Uncertainty parameters for the
 * Forecast. These values represent parameter percentage error.
 */
class nmfForecast_Tab3: public QObject
{
    Q_OBJECT

private:
    QString             m_Alpha;
    QString             m_BetaS;
    QString             m_BetaG;
    std::string         m_CompetitionForm;
    nmfDatabase*        m_DatabasePtr;
    std::map<std::string,std::vector<std::string> > m_FormMap;
    std::string         m_GrowthForm;
    std::string         m_HarvestForm;
    bool                m_IsAggProd;
    nmfLogger*          m_Logger;
    QStringList         m_ParameterNames;
    std::string         m_PredationForm;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QString             m_Rho;
    QStandardItemModel* m_SModel;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab3_Widget;
    QTableView*  Forecast_Tab3_UncertaintyTV;
    QPushButton* Forecast_Tab3_PrevPB;
    QPushButton* Forecast_Tab3_NextPB;
    QPushButton* Forecast_Tab3_LoadPB;
    QPushButton* Forecast_Tab3_SavePB;
    QGroupBox*   Forecast_Tab2_HarvestGB;
    QLineEdit*   Forecast_Tab1_NameLE;
    QCheckBox*   Forecast_Tab3_AutoSaveRunCB;

    void adjustColumnVisibility();
    void clearUncertaintyTable(int& NumSpecies,
                               int& NumParameters);
    QString harvestType();
    void readSettings();

public:
    /**
     * @brief nmfForecast_Tab3 : class constructor for the Uncertainty Parameters GUI page
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab3(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab3();

    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();

signals:
    /**
     * @brief Signal emitted to run a new Forecast
     * @param ForecastName : name of Forecast to run
     * @param GenerateBiomass : boolean specifying whether to generate the biomass data or Monte Carlo simulations
     */
    void RunForecast(std::string ForecastName,
                     bool GenerateBiomass);

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
     * @brief Callback invoked when the user changes an item in the Uncertainty Parameters data table. If
     * the Auto Save button is checked, the Forecast will update after the user changes an item.
     * @param unusedA : unused parameter
     * @param unusedB : unused parameter
     */
    void callback_ItemChanged(const QModelIndex& unusedA,
                              const QModelIndex& unusedB);
    /**
     * @brief Callback invoked when user right clicks over the Uncertainty Parameters data table.
     * A Popup menu will appear asking the user if they want to clear the selected table items.
     * @param point : point on the GUI at which the user clicked (needed so as to know where to draw the popup GUI)
     */
    void callback_ContextMenu(QPoint point);
    /**
     * @brief Callback invoked when the user selects Clear from the right click context menu
     */
    void callback_ClearSelection();
};

#endif // NMFFORECASTTAB3_H
