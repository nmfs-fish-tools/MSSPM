#ifndef NMFSETUPTAB3_H
#define NMFSETUPTAB3_H

/**
 * @brief Defines the behavior and callback functionality for Setup Tab 3.
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
    QCheckBox*    Setup_Tab3_SpeciesCB;
    QCheckBox*    Setup_Tab3_GuildsCB;
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

    void saveGuildData();
    void saveSpeciesData();
    bool guildDataIsSaved();

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

    int  numColumnsSpecies();
    int  numColumnsGuilds();
    void clearWidgets();
    void loadGuilds();
    void loadSpecies();
    void loadWidgets();
    void populateARowGuilds(int row, int ncols);
    void populateARowSpecies(int row, int ncols);
    void removeFromTable(QTableWidgetItem *itemToRemove,
                         QList<QString>& TablesToDeleteFrom);
    void readSettings();
    void saveSettings();
    void setModelName(std::string modelName);

signals:
    void CreateAllDatabaseTables();
    void DeactivateRunButtons();
    void ReloadWidgets();
    void SSVPALoadWidgets(int TabNum);
    void UpdateNavigator(int item);
    void UpdateNavigator(std::string type, int index);

public Q_SLOTS:
    void callback_Setup_Tab3_NumSpecies(int value);
    void callback_Setup_Tab3_NumGuilds(int numGuilds);
    void callback_Setup_Tab3_SpeciesCB(bool state);
    void callback_Setup_Tab3_GuildsCB(bool state);
    void callback_Setup_Tab3_AddSpeciesPB();
    void callback_Setup_Tab3_DelSpeciesPB();
    void callback_Setup_Tab3_ReloadSpeciesPB();
    void callback_Setup_Tab3_SavePB();
    void callback_Setup_Tab3_PrevPB();
    void callback_Setup_Tab3_AddGuildPB();
    void callback_Setup_Tab3_DelGuildPB();
    void callback_Setup_Tab3_ReloadGuildsPB();
    void callback_Setup_Tab3_SpeciesTableChanged(int,int);
    void callback_Setup_Tab3_GuildsTableChanged(int,int);
    void callback_Setup_Tab3_UpdateSpeciesPB();
    void callback_RemoveGuildsAndSpecies();
};

#endif // NMFSETUPTAB4_H
