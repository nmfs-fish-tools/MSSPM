#ifndef NMFESTIMATIONTAB6_H
#define NMFESTIMATIONTAB6_H

/**
 * @brief The Run Estimation Settings
 *
 * These widgets allow the user to enter and save the parameters required
 * to run an Estimation.  There's also a text widget which shows a summary
 * to the user of what transpired after the user hits the Run button.
 */
class nmfEstimation_Tab6: public QObject
{
    Q_OBJECT

    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    int          m_fontSize;
    int          m_isMonospaced;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    std::string  m_EstimationOutputFile;
    std::string  m_EstimationDataFile;
    std::string  m_EstimationID;

    QGroupBox*   Estimation_Tab6_Bees_ParametersGB;
    QGroupBox*   Estimation_Tab6_NL_ParametersGB;
    QComboBox*   Estimation_Tab6_MinimizerAlgorithmCMB;
    QComboBox*   Estimation_Tab6_ObjectiveCriterionCMB;
    QLabel*      Estimation_Tab6_MinimizerAlgorithmLBL;
    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab6_Widget;
    QTextEdit*   Estimation_Tab6_RunTE;
    QPushButton* Estimation_Tab6_RunPB;
    QPushButton* Estimation_Tab6_SavePB;
    QPushButton* Estimation_Tab6_ReloadPB;
    QPushButton* Estimation_Tab6_PrevPB;
    QComboBox*   Estimation_Tab6_FontSizeCMB;
    QCheckBox*   Estimation_Tab6_MonoCB;
    QComboBox*   Estimation_Tab6_EstimationAlgorithmCMB;
    QSpinBox*    Estimation_Tab6_NumberOfRunsSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBestSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBestBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_MaxGenerationsSB;
    QSpinBox*    Estimation_Tab6_Bees_NeighborhoodSizeSB;
    QComboBox*   Estimation_Tab6_ScalingCMB;
    QCheckBox*   Estimation_Tab6_NL_StopValCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterTimeCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterIterCB;
    QLineEdit*   Estimation_Tab6_NL_StopValLE;
    QSpinBox*    Estimation_Tab6_NL_StopAfterTimeSB;
    QSpinBox*    Estimation_Tab6_NL_StopAfterIterSB;

public:
    /**
     * @brief nmfEstimation_Tab6 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab6(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab6();

    void appendOutputTE(QString msg);
    void clearOutputTE();
    QString     getDataPath();
    std::string getCurrentAlgorithm();
    std::string getCurrentMinimizer();
    std::string getCurrentObjectiveCriterion();
    bool loadWidgets();
    void readSettings();
    void refreshMsg(QFont font, QString msg);
    void saveSettings();
    bool saveSettingsConfiguration(bool verbose,std::string currentSettingsName);
    void saveSystem(bool RunChecks);
    void setFont(QFont font);
    void setOutputTE(QString msg);

signals:
    void RunEstimation();
    void ShowRunMessage(QString msg);
    void SetAlgorithm(QString algorithm);
    void UpdateForecastYears();

public Q_SLOTS:
    void callback_RunPB();
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_Estimation_Tab6_FontSizeCMB(QString fontSize);
    void callback_Estimation_Tab6_MonoCB(int isChecked);
    void callback_Estimation_Tab6_EstimationAlgorithmCMB(QString algorithm);
    void callback_StopValCB(int isChecked);
    void callback_StopAfterTimeCB(int isChecked);
    void callback_StopAfterIterCB(int isChecked);
    void callback_SaveSettings();
};

#endif // NMFESTIMATIONTAB6_H
