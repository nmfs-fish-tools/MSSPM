/**
 * @file nmfSetupTab03.h
 * @brief GUI definition for the Setup Species page class nmfSetup_Tab3
 *
 * This file contains the GUI definitions for the Setup Species page. This
 * page contains the GUI widgets that allow the user to enter and modify
 * Species and Guild data. Guild data should be entered first as it
 * automatically populates the Guild pulldown field widget in the Species data.
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

#ifndef NMFSETUPTAB3_H
#define NMFSETUPTAB3_H

#include <set>

/**
 * @brief The Setup Tab 3 allows the user to enter and modify Species data
 *
 * This tab allows the user to define Species and Guild data. The Guild data
 * must be defined first since the user needs to specify an existing guild for
 * each species.
 */
class nmfSetup_Tab3: public QObject
{
    Q_OBJECT

    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QStandardItemModel* m_smodelSpecies;
    QStandardItemModel* m_smodelOtherPredSpecies;
    QStringList         m_colLabelsSpecies;
    QStringList         m_colLabelsGuilds;
    std::vector<std::string> m_ModelPresetNames;
    std::map<std::string,std::vector<std::string> > m_ModelPresets;

    QTabWidget*   Setup_Tabs;
    QTabWidget*   Setup_Tab3_GuildsSpeciesTabW;
    QComboBox*    Setup_Tab3_ModelTypeCMB;
    QComboBox*    Setup_Tab3_RandomnessCMB;
    QComboBox*    Setup_Tab3_ModelNameCMB;
    QComboBox*    Setup_Tab3_GrowthFormCMB;
    QComboBox*    Setup_Tab3_PredationFormCMB;
    QComboBox*    Setup_Tab3_HarvestFormCMB;
    QComboBox*    Setup_Tab3_CompetitionFormCMB;
    QTextEdit*    SetupOutputTE;
    QWidget*      Setup_Tab3_Widget;
    QPushButton*  Setup_Tab3_CreateTemplatesPB;
    QSpinBox*     Setup_Tab3_NumSpeciesSB;
    QSpinBox*     Setup_Tab3_NumGuildsSB;
    QTableWidget* Setup_Tab3_SpeciesTW;
    QTableWidget* Setup_Tab3_GuildsTW;
    QLineEdit*    Setup_Tab2_ProjectDirLE;
    QLineEdit*    Setup_Tab2_FirstYearLE;
    QLineEdit*    Setup_Tab2_LastYearLE;
    QLineEdit*    Setup_Tab2_NumSeasonsLE;
    QLabel*       Setup_Tab3_NumSpeciesLBL;
    QLabel*       Setup_Tab3_NumGuildsLBL;
    QPushButton*  Setup_Tab3_AddSpeciesPB;
    QPushButton*  Setup_Tab3_DelSpeciesPB;
    QPushButton*  Setup_Tab3_AddGuildPB;
    QPushButton*  Setup_Tab3_DelGuildPB;
    QPushButton*  Setup_Tab3_UpdateSpeciesPB;
    QPushButton*  Setup_Tab3_SavePB;
    QPushButton*  Setup_Tab3_LoadPB;
    QPushButton*  Setup_Tab3_PrevPB;
    QPushButton*  Setup_Tab3_ReloadSpeciesPB;
    QPushButton*  Setup_Tab3_ReloadGuildsPB;

    void clearSpeciesWidgets();
    void clearGuildWidgets();
    bool guildDataIsSaved();
    void loadGuilds();
    void loadSpecies();
    int  numColumnsSpecies();
    int  numColumnsGuilds();
    void populateARowGuilds(int row, int ncols);
    void populateARowSpecies(int row, int ncols);
    void pruneTablesForGuilds(std::vector<std::string>& Guilds);
    void pruneTablesForSpecies(std::vector<std::string>& Species);
    void readSettings();
    void removeFromTables(const QString& type,
                          const QTableWidgetItem *itemToRemove,
                          const std::vector<std::string>& TablesToDeleteFrom);
    void saveGuildData();
    void saveSettings();
    void saveSpeciesData();
    void setupHelp();
    void executeDelete(std::string cmd);

public:
    /**
     * @brief nmfSetup_Tab3 : class constructor
     * @param tabs : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfSetup_Tab3(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab3();

    /**
     * @brief Loads all of the widgets for this Setup page
     */
    void loadWidgets();

signals:
    /**
     * @brief Signal emitted after user saves Species. This is
     * necessary since the input Estimation tables may need to
     * be modified with new Species.
     */
    void ReloadWidgets();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user modifies the Number of Species Spin Box widget
     * @param value : current value of Spin Box widget
     */
    void callback_Setup_Tab3_NumSpecies(int value);
    /**
     * @brief Callback invoked when user modifies the Number of Guilds Spin Box widget
     * @param value : current value of Spin Box widget
     */
    void callback_Setup_Tab3_NumGuilds(int numGuilds);
    /**
     * @brief Callback invoked when user clicks the Add Species button
     */
    void callback_Setup_Tab3_AddSpeciesPB();
    /**
     * @brief Callback invoked when user clicks the Delete Species button
     */
    void callback_Setup_Tab3_DelSpeciesPB();
    /**
     * @brief Callback invoked when user clicks the Reload Guilds button
     */
    void callback_Setup_Tab3_ReloadGuildsPB();
    /**
     * @brief Callback invoked when user emits a signal to Reload the Guilds
     * @param showPopup : boolean specifying whether or not a popup should be
     * shown to the user acknowledging a successful Guilds reload
     */
    void callback_Setup_Tab3_ReloadGuildsPB(bool showPopup);
    /**
     * @brief Callback invoked when user clicks the Reload Species button
     */
    void callback_Setup_Tab3_ReloadSpeciesPB();
    /**
     * @brief Callback invoked when user emits a signal to Reload the Species
     * @param showPopup : boolean specifying whether or not a popup should be
     * shown to the user acknowledging a successful Species reload
     */
    void callback_Setup_Tab3_ReloadSpeciesPB(bool showPopup);
    /**
     * @brief Callback invoked when user clicks the Save Species button
     */
    void callback_Setup_Tab3_SavePB();
    /**
     * @brief Callback invoked when user clicks the Previous Page button
     */
    void callback_Setup_Tab3_PrevPB();
    /**
     * @brief Callback invoked when user clicks the Add Guilds button
     */
    void callback_Setup_Tab3_AddGuildPB();
    /**
     * @brief Callback invoked when user clicks the Delete Guilds button
     */
    void callback_Setup_Tab3_DelGuildPB();
    /**
     * @brief Callback invoked when user modifies the Species table
     */
    void callback_Setup_Tab3_SpeciesTableChanged(int,int);
    /**
     * @brief Callback invoked when user modifies the Guilds table
     */
    void callback_Setup_Tab3_GuildsTableChanged(int,int);
    /**
     * @brief Callback invoked when user clicks the Guilds page button to refresh
     * the Species page. This is necessary as Guilds are listed in the Guild column
     * on the Species page.
     */
    void callback_Setup_Tab3_UpdateSpeciesPB();

};

#endif // NMFSETUPTAB4_H
