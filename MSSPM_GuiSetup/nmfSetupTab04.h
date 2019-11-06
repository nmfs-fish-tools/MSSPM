#ifndef NMFSETUPTAB4_H
#define NMFSETUPTAB4_H


class QDialogButtonBox;


/**
 * @brief Defines the behavior and callback functionality for the Load Systems Dialog.
 */
class LoadDlg : public QDialog
{
    Q_OBJECT

private:
    int               m_NumberOfRuns;
    int               m_RunLength;
    int               m_TimeStep;
    nmfLogger*        m_logger;
    nmfDatabase*      m_databasePtr;
    QDialogButtonBox* m_buttonBox;
    QListWidget*      m_SettingsLW;
    std::string       m_GrowthForm;
    std::string       m_HarvestForm;
    std::string       m_CompetitionForm;
    std::string       m_PredationForm;
    SystemData        m_data;
    std::vector<std::string> m_SettingNames;

public:
    LoadDlg(const QString &title,
                  QWidget *parent,
                  nmfLogger* m_logger,
                  nmfDatabase* m_databasePtr,
            const QString &currentConfig);
   ~LoadDlg() {}

    void reloadSystemsList();
    void saveSettings();
    void getSettingData(SystemData &m_data);
    void updateWindowTitle();

signals:
    void ClearSystemName();

public slots:
    void callback_LoadOk();
    void callback_ItemDoubleClicked(QListWidgetItem* item);
    void callback_ShowContextMenu(const QPoint &pos);
    void callback_DeleteItem();
};

/**
 * @brief Defines the behavior and callback functionality for Setup Tab 4.
 *
 * This tab allows the user to define a System which contains data related to the
 * current model such as run length, year range, and model form.
 */
class nmfSetup_Tab4: public QObject
{
    Q_OBJECT

    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    QDialog      m_loadDlg;
    QPushButton* m_cancelBtn;
    QPushButton* m_okBtn;
    std::vector<std::string> m_ModelPresetNames;
    std::map<std::string,std::vector<std::string> > m_ModelPresets;

    QTabWidget*  Setup_Tabs;
    QLabel*      Setup_Tab4_ModelPresetsLBL;
    QPushButton* Setup_Tab4_GrowthHighlightPB;
    QPushButton* Setup_Tab4_HarvestHighlightPB;
    QPushButton* Setup_Tab4_PredationHighlightPB;
    QPushButton* Setup_Tab4_CompetitionHighlightPB;
    QPushButton* Setup_Tab4_CalcPB;
    QComboBox*   Setup_Tab4_FontSizeCMB;
    QComboBox*   Setup_Tab4_ModelPresetsCMB;
    QComboBox*   Setup_Tab4_GrowthFormCMB;
    QComboBox*   Setup_Tab4_PredationFormCMB;
    QComboBox*   Setup_Tab4_HarvestFormCMB;
    QComboBox*   Setup_Tab4_CompetitionFormCMB;
    QTextEdit*   SetupOutputTE;
    QLineEdit*   Setup_Tab4_SystemNameLE;
    QLineEdit*   Setup_Tab4_SystemCarryingCapacityLE;
    QTextEdit*   Setup_Tab4_ModelEquationTE;
    QWidget*     Setup_Tab4_Widget;
    QPushButton* Setup_Tab4_SavePB;
    QPushButton* Setup_Tab4_LoadPB;
    QPushButton* Setup_Tab4_DelPB;
    QPushButton* Setup_Tab4_NextPB;
    QPushButton* Setup_Tab4_PrevPB;
    QPushButton* Setup_Tab4_SystemNamePB;
    QSpinBox*    Setup_Tab4_NumberOfRunsSB;
    QSpinBox*    Setup_Tab4_StartYearSB;
    QLineEdit*   Setup_Tab4_EndYearLE;
    QSpinBox*    Setup_Tab4_RunLengthSB;

    double calculateSystemCarryingCapacity();
    void   getHighlightColors(QString& growthHighlightColor,
                            QString& harvestHighlightColor,
                            QString& competitionHighlightColor,
                            QString& predationHighlightColor,
                            QString& growthColorName,
                            QString& harvestColorName,
                            QString& competitionColorName,
                            QString& predationColorName);
    bool   systemFileExists(QString SystemName);
    bool   saveSettingsConfiguration(bool verbose,
                                     std::string CurrentSettingsName);

public:
    LoadDlg*   loadDialog;
    QComboBox* modelPresetsCMB();
    QComboBox* growthFormCMB();
    QComboBox* predationFormCMB();
    QComboBox* harvestFormCMB();
    QComboBox* competitionFormCMB();

    /**
     * @brief nmfSetup_Tab4 : class constructor
     * @param tabs : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfSetup_Tab4(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab4();

    void clearWidgets();
    void deleteSystem(QString systemToDelete);
    void drawEquation(QString label, QString eqn, QString Key);
    int  getFontSize();
    int  getRunLength();
    bool isGrowthFormHighlighted();
    bool isHarvestFormHighlighted();
    bool isPredationFormHighlighted();
    bool isCompetitionFormHighlighted();
    bool isAggProd();
    bool isTypeIII();
    void loadSpeciesTable();
    void loadWidgets();
    void populateARowSpecies(int row, int ncols);
    void populateARowGuilds(int row, int ncols);
    void readSettings();
    void reloadSystemName();
    void renameSpecies(std::string newSpeName, std::string existingSpeName);
    void saveSettings();
    void saveSystem(bool RunChecks);
    void setModelName(std::string modelName);
    void setStartYear(int StartYear);
    void setRunLength(int RunLength);
    void setFontSize(int fontSize);
    void setHighlightColors();
    void setSystemName(QString systemName);
    bool speciesFieldsOK(int NumSpecies);
    void uncheckHighlightButtons();
    void updateOutputWidget();
    void updateSpeciesTable(int NumSpecies);

signals:
    void CompetitionFormChanged(QString competitionForm);
    void CreateAllDatabaseTables();
    void DeactivateRunButtons();
    void HarvestFormChanged(QString harvestForm);
    void PredationFormChanged(QString predationForm);
    void RedrawEquation();
    void ReloadWidgets();
    void SystemSaved();
    void SystemLoaded();
    void SetAlgorithm(QString algorithm);
    void SaveMainSettings();
    void SaveEstimationRunSettings();
    void SSVPALoadWidgets(int TabNum);
    void UpdateNavigator(int item);
    void UpdateNavigator(std::string type, int index);
    void UpdateInitialForecastYear();
    void UpdateInitialObservedBiomass();

public Q_SLOTS:
    void callback_Setup_Tab4_SavePB();
    void callback_Setup_Tab4_DelPB();
    void callback_Setup_Tab4_LoadPB();
    void callback_Setup_Tab4_NextPB();
    void callback_Setup_Tab4_PrevPB();
    void callback_Setup_Tab4_ModelPresetsCMB(QString val);
    void callback_Setup_Tab4_GrowthFormCMB(QString name);
    void callback_Setup_Tab4_PredationFormCMB(QString name);
    void callback_Setup_Tab4_HarvestFormCMB(QString name);
    void callback_Setup_Tab4_CompetitionFormCMB(QString name);
    void callback_Setup_Tab4_FontSizeCMB(QString theFontSize);
    void callback_GrowthHighlightPB();
    void callback_HarvestHighlightPB();
    void callback_PredationHighlightPB();
    void callback_CompetitionHighlightPB();
    void callback_ReloadWidgets();
    void callback_UpdateEndYear(int value);
    void callback_SystemNamePB();
    void callback_ClearSystemName();
    void callback_CalcPB();
    void loadSystem();
    void callback_SaveSystem();
};

#endif // NMFSETUPTAB3_H
