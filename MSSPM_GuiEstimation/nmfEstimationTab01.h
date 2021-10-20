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

#include <QComboBox>
#include <QSpinBox>
#include <set>

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
    int                  m_NumSignificantDigits;
    nmfLogger*           m_Logger;
    nmfDatabase*         m_DatabasePtr;
    QStandardItemModel*  m_GuildModel;
    QStandardItemModel*  m_SpeciesModel;
    std::string          m_ProjectDir;
    std::string          m_ProjectName;
    std::string          m_ModelName;
    QModelIndexList      m_selIndexes;
    QList<QString>       m_originalSpeciesValuesAll;
    std::vector<double>  m_originalValuesSelected;
    int                  m_StartPosSL;
    QString              m_OutputSpecies;
    QList<QString>       m_SpeciesGuild;

    QTabWidget*   Estimation_Tabs;
    QTabWidget*   Estimation_Tab1_PopulationTabW;
    QWidget*      Estimation_Tab1_Widget;
//  nmfTableView* Estimation_Tab1_PopulationTV;
    QTableView*   Estimation_Tab1_GuildPopulationTV;
    QTableView*   Estimation_Tab1_SpeciesPopulationTV;
    QPushButton*  Estimation_Tab1_NextPB;
    QGroupBox*    Estimation_Tab1_PopulationGB;
    QPushButton*  Estimation_Tab1_LoadPB;
    QPushButton*  Estimation_Tab1_ImportPB;
    QPushButton*  Estimation_Tab1_ExportPB;
    QPushButton*  Estimation_Tab1_RestorePB;
    QPushButton*  Estimation_Tab1_SavePB;
    QSlider*      Estimation_Tab1_ModifySL;
    QPushButton*  Estimation_Tab1_ModifyRunPB;
    QCheckBox*    Estimation_Tab1_GuildSuppCB;
    QCheckBox*    Estimation_Tab1_SpeciesSuppCB;
    QCheckBox*    Estimation_Tab1_GuildRangeCB;
    QCheckBox*    Estimation_Tab1_SpeciesRangeCB;
    QComboBox*    Estimation_Tab1_SpeciesRangeCMB;
    QSpinBox*     Estimation_Tab1_SpeciesRangeSB;
    QCheckBox*    Estimation_Tab1_ModifyRunCB;
    QComboBox*    Estimation_Tab1_MinMaxCMB;

    bool arePopulationParametersWithinLimits();
    bool checkAndShowEmptyFieldError(bool showPopup,
                                     const std::string& location);
    bool checkAndShowOutOfRangeError(QString type,
                                     QString name,
                                     QString badParameter,
                                     bool showPopup);
    bool getCSVFileName(QString& tableName);
    QModelIndexList getSelectedVisibleCells();
    void getSelectedIndexes();
    void importGuildData(const QString& tableName,
                         bool updateSetup);
    void importSpeciesData(const QString& tableName,
                           bool updateSetup);
    bool isChecked(QCheckBox* cb);
    bool isInitBiomassLessThanSpeciesKMin();
    bool loadGuilds();
    bool loadSpecies();
    bool onGuildTab();
    void readSettings();
    void reselectVisibleCells(QModelIndexList indexes);
    void resetModifySlider();
    void resetSelection();
    void resetVisibleColumns();
    bool saveGuildDataPrimary(bool showPopup);
    bool saveGuildDataSupplemental(bool showPopup);
    bool saveGuildDataRange(bool showPopup);
    bool saveGuildDataSupplementalAndRange(bool showPopup);
    void saveGuildsCSVFile(QString& tableName,
                           QList<QString>& GuildName,
                           QList<QString>& GrowthRate,
                           QList<QString>& GuildK);
    bool savePopulationParameterGuildK();
    bool savePopulationParametersGuilds(bool showPopup);
    bool savePopulationParametersSpecies(bool showPopup);
    bool savePopulationParameterSpeciesK();
    void saveSpeciesCSVFile(QString& tableName,
                            QList<QString>& SpeciesName,
                            QList<QString>& SpeciesGuild,
                            QList<QString>& SpeciesInitialBiomass,
                            QList<QString>& SpeciesGrowthRate,
                            QList<QString>& SpeciesK);
    bool saveSpeciesDataPrimary(bool showPopup);
    bool saveSpeciesDataSupplemental(bool showPopup);
    bool saveSpeciesDataRange(bool showPopup);
    bool saveSpeciesDataSupplementalAndRange(bool showPopup);
    void setupHelpSpecies();
    void setupHelpGuilds();
    void showNoColumns(QTableView* tv);
    void showAllColumns(QTableView* tv);
    void showPrimaryColumns(QTableView* tv);
    void showSuppColumns(QTableView* tv,bool show);
    void showRangeColumns(QTableView* tv,bool show);
    bool surveyQValid(bool showPopup);
    void updateBiomassAbsoluteTable();

signals:
    /**
     * @brief Signal sent to check all Estimation tables for completeness
     */
    void CheckAllEstimationTablesAndRun();
    /**
     * @brief Signal sent to notify Setup Tab3 to load its Guild/Species table
     */
    void LoadSetup();
    /**
     * @brief Queries main program to load local list with Species Guild names
     */
    void LoadSpeciesGuild();
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
     * @brief Signal notifying main application to update all of its GUIs.
     */
    void ReloadWidgets();
    /**
     * @brief Signal causes the recently saved Output Species names to be written back into the appropriate Output Controls widget
     */
    void RestoreOutputSpecies();
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
    /**
     * @brief Signal causes the Species names in the Output Species widget to be temporarily saved
     */
    void StoreOutputSpecies();
    /**
     * @brief Signal sent so the Guild Setup tab will be updated with the appropriate Guild fields
     * @param GuildNames : list of guilds names
     * @param GuildGrowthRate : list of guild growth rates
     * @param GuildK : list of guild carrying capacities
     */
    void UpdateGuildSetupData(
            QList<QString> GuildNames,
            QList<QString> GuildGrowthRate,
            QList<QString> GuildK);
    /**
     * @brief Signal sent so the Species Setup tab will be updated with the appropriate Species fields
     * @param SpeciesNames : list of species names
     * @param SpeciesGuilds : list of guilds that each species is in
     * @param SpeciesInitBiomass : list of species initial biomass values
     * @param SpeciesGrowthRate : list of species growth rates
     * @param SpeciesK : list of species carrying capacities
     */
    void UpdateSpeciesSetupData(
            QList<QString> SpeciesNames,
            QList<QString> SpeciesGuilds,
            QList<QString> SpeciesInitBiomass,
            QList<QString> SpeciesGrowthRate,
            QList<QString> SpeciesK);

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
    /**
     * @brief Sets the SpeciesGuild list with the current combobox values from the Species Setup widget
     * @param SpeciesGuildList : list of Species Guild values
     */
    void setSpeciesGuild(QList<QString> SpeciesGuildList);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks on Estimation 1 sub tab
     * @param subtab : sub tab clicked (Guilds or Species)
     */
    void callback_CurrentTabChanged(int subtab);
    /**
     * @brief Callback invoked when the user clicks the Export button to save data to a .csv file
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when user checks the Guilds Range Data check box. This
     * shows or hides the range data columns in the Guilds table.
     */
    void callback_GuildRangeCB(bool isChecked);
    /**
     * @brief Callback invoked when user checks the Guilds Supplemental Data check box. This
     * shows or hides the supplemental data columns in the Guilds table.
     */
    void callback_GuildSuppCB(bool isChecked);
    /**
     * @brief Callback invoked when the user clicks the Import button to load data from a .csv file
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user clicked the Import button from the Setup GUIs - Species tab
     */
    void callback_ImportSpecies();
    /**
     * @brief Callback invoked when the user clicked the Import button from the Setup GUIs - Guild tab
     */
    void callback_ImportGuild();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Load button and they don't want
     * the Estimation Tab1 Guild/Species table to reload
     */
    void callback_LoadPBNoEmit();
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
     * @brief Callback invoked when the user saves the Guilds from
     * the Guilds Setup tab
     * @param GuildName : list of guild names
     * @param GrowthRate : list of guild growth rates
     * @param GuildK : list of guild carrying capacities
     */
    void callback_SaveGuildsCSVFile(QList<QString> GuildName,
                                    QList<QString> GrowthRate,
                                    QList<QString> GuildK);
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user saves the Species from
     * the Species Setup tab
     * @param SpeciesName : list of species names
     * @param SpeciesGuild : list of guilds that each species is a member of
     * @param SpeciesInitialBiomass : list of initial biomass per species
     * @param SpeciesGrowthRate : list of growth rate per species
     * @param SpeciesK : list of carrying capacities per species
     */
    void callback_SaveSpeciesCSVFile(QList<QString> SpeciesName,
                                     QList<QString> SpeciesGuild,
                                     QList<QString> SpeciesInitialBiomass,
                                     QList<QString> SpeciesGrowthRate,
                                     QList<QString> SpeciesK);
    /**
     * @brief Callback invoked when the user makes a selection in the tableview
     * @param selection : the selection just made
     * @param deselection : the deselection just made
     */
    void callback_SelectionChangedTV(const QItemSelection& selection,
                                     const QItemSelection& deselection);
    /**
     * @brief Callback invoked when user checks the Species Range Data check box. This
     * shows or hides the range data columns in the Species table.
     */
    void callback_SpeciesRangeCB(bool isChecked);
    /**
     * @brief Callback invoked when user changes the range type combo box
     * @param value : value of combobox setting (i.e., User Defined or Percentage)
     */
    void callback_SpeciesRangeCMB(QString value);
    /**
     * @brief Callback invoked when user changes the Species range percent combo box
     * @param pct - percentage value of how the min/max values should be set around each parameter
     */
    void callback_SpeciesRangeSB(int pct);
    /**
     * @brief Callback invoked when user checks the Species Supplemental Data check box. This
     * shows or hides the supplemental data columns in the Species table.
     */
    void callback_SpeciesSuppCB(bool isChecked);

};

#endif // NMFESTIMATIONTAB1_H
