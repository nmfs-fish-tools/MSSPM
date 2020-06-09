/**
 * @file nmfEstimationTab01.h
 * @author Ronald Klasky
 * @date Nov 21, 2019
 * @brief GUI definition for the nmfEstimation_Tab1 Population Parameters Data class
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
#ifndef NMFESTIMATIONTAB1_H
#define NMFESTIMATIONTAB1_H

#include "nmfUtilsQt.h"

/**
 * @brief Estimated Parameters
 *
 * This table is reproduced here from the Species table in Setup Tab 3. This is a
 * more succinct way of looking at the per Species estimated parameters and their
 * min/max values.
 */
class nmfEstimation_Tab1: public QObject
{
    Q_OBJECT

    bool                 m_runFromModifySlider;
    nmfLogger*           m_Logger;
    nmfDatabase*         m_DatabasePtr;
    QStandardItemModel*  m_GuildModel;
    QStandardItemModel*  m_SpeciesModel;
    std::string          m_ProjectDir;
    QModelIndexList      m_selIndexes;
    std::vector<QString> m_originalSpeciesValuesAll;
    std::vector<double>  m_originalValuesSelected;
    int                  m_StartPosSL;
    QString              m_OutputSpecies;

    QTabWidget*   Estimation_Tabs;
    QTabWidget*   Estimation_Tab1_PopulationTabW;
    QWidget*      Estimation_Tab1_Widget;
//  nmfTableView* Estimation_Tab1_PopulationTV;
    QTableView*   Estimation_Tab1_GuildPopulationTV;
    QTableView*   Estimation_Tab1_SpeciesPopulationTV;
    QPushButton*  Estimation_Tab1_NextPB;
    QGroupBox*    Estimation_Tab1_PopulationGB;
    QPushButton*  Estimation_Tab1_LoadPB;
    QPushButton*  Estimation_Tab1_RestorePB;
    QPushButton*  Estimation_Tab1_SavePB;
    QSlider*      Estimation_Tab1_ModifySL;
    QPushButton*  Estimation_Tab1_ModifyRunPB;
    QCheckBox*    Estimation_Tab1_GuildSuppCB;
    QCheckBox*    Estimation_Tab1_SpeciesSuppCB;
    QCheckBox*    Estimation_Tab1_GuildRangeCB;
    QCheckBox*    Estimation_Tab1_SpeciesRangeCB;
    QCheckBox*    Estimation_Tab1_ModifyRunCB;

    bool arePopulationParametersWithinLimits();
    bool checkAndShowEmptyFieldError(bool showPopup);
    bool checkAndShowOutOfRangeError(QString type,
                                     QString name,
                                     QString badParameter,
                                     bool showPopup);
    void getSelectedIndexes();
    bool isChecked(QCheckBox* cb);
    bool loadGuilds();
    bool loadSpecies();
    void resetModifySlider();
    void resetSelection();
    void resetVisibleColumns();
    void showNoColumns(QTableView* tv);
    void showAllColumns(QTableView* tv);
    void showPrimaryColumns(QTableView* tv);
    void showSuppColumns(QTableView* tv,bool show);
    void showRangeColumns(QTableView* tv,bool show);
    bool savePopulationParametersGuilds(bool showPopup);
    bool saveGuildDataPrimary(bool showPopup);
    bool saveGuildDataSupplemental(bool showPopup);
    bool saveGuildDataRange(bool showPopup);
    bool saveGuildDataSupplementalAndRange(bool showPopup);
    bool savePopulationParametersSpecies(bool showPopup);
    bool saveSpeciesDataPrimary(bool showPopup);
    bool saveSpeciesDataSupplemental(bool showPopup);
    bool saveSpeciesDataRange(bool showPopup);
    bool saveSpeciesDataSupplementalAndRange(bool showPopup);
    void setupHelpSpecies();
    void setupHelpGuilds();

signals:
    /**
     * @brief Signal sent to check all Estimation tables for completeness
     */
    void CheckAllEstimationTablesAndRun();
    /**
     * @brief Signal notifying other widgets to reload guilds data
     * @param showPopup : boolean signifying whether the application should pop up a successful reload acknowledgement
     */
    void ReloadGuilds(bool showPopup);
    /**
     * @brief Signal notifying other widgets to reload species data
     * @param showPopup : boolean signifying whether the application should pop up a successful reload acknowledgement
     */
    void ReloadSpecies(bool showPopup);
    /**
     * @brief Signal notifying that a new Diagnostics parameter run should be made
     */
    void RunDiagnostics();
    /**
     * @brief Signal notifying that a new Estimation should be run
     * @param showDiagnosticsChart : boolean signifying that the user wants to show the Diagnostics chart
     */
    void RunEstimation(bool showDiagnosticsChart);
    /**
     * @brief Signal notifying that the Diagnostics parameter output 3d display should be shown
     */
    void ShowDiagnostics();
    void StoreOutputSpecies();
    void RestoreOutputSpecies();

public:
    /**
     * @brief nmfEstimation_Tab1 : class constructor for the Populate Parameters GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab1(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab1();

    /**
     * @brief Checks if Estimation Run that just finished originated from the user
     * moving the Modify Slider and if so, will next call the diagnostic run functionality
     */
    void checkIfRunFromModifySlider();
    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Gets the species stored in the class representing the current output species
     * @return QString denoting the current Output widget species
     */
    QString getOutputSpecies();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Saves the output species so after the Estimation and Diagnostic
     * run, it can be reset correctly.
     * @param species : the species set in the output widget
     */
    void setOutputSpecies(QString species);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks on Estimation 1 sub tab
     * @param subtab : sub tab clicked (Guilds or Species)
     */
    void callback_CurrentTabChanged(int subtab);
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user presses the Run Modify button which
     * performs an Estimation Run and Diagnostics Run as if the user had clicked on
     * the Modify Slider button.
     */
    void callback_ModifyRunPB();
    /**
     * @brief Callback invoked when the user moves the Slider bar
     * @param value : current value of the Modify slider
     */
    void callback_ModifyMovingSL(int value);
    /**
     * @brief Callback invoked when the user releases the Slider bar
     */
    void callback_ModifyReleasedSL();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Restore button which will
     * reload the table data with the data prior to the user modifying the data with the slider.
     */
    void callback_RestorePB();
    /**
     * @brief Callback invoked when the user makes a selection in the tableview
     * @param selection : the selection just made
     * @param deselection : the deselection just made
     */
    void callback_SelectionChangedTV(const QItemSelection& selection,
                                     const QItemSelection& deselection);
    /**
     * @brief Callback invoked when user checks the Guilds Supplemental Data check box. This
     * shows or hides the supplemental data columns in the Guilds table.
     */
    void callback_GuildSuppCB(bool isChecked);
    /**
     * @brief Callback invoked when user checks the Species Supplemental Data check box. This
     * shows or hides the supplemental data columns in the Species table.
     */
    void callback_SpeciesSuppCB(bool isChecked);
    /**
     * @brief Callback invoked when user checks the Guilds Range Data check box. This
     * shows or hides the range data columns in the Guilds table.
     */
    void callback_GuildRangeCB(bool isChecked);
    /**
     * @brief Callback invoked when user checks the Species Range Data check box. This
     * shows or hides the range data columns in the Species table.
     */
    void callback_SpeciesRangeCB(bool isChecked);

};

#endif // NMFESTIMATIONTAB1_H
