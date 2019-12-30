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

    nmfDatabase* m_DatabasePtr;
    int          m_FontSize;
    int          m_IsMonospaced;
    nmfLogger*   m_Logger;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    std::string  m_EstimationDataFile;
    std::string  m_EstimationID;
    std::string  m_EstimationOutputFile;

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

    void readSettings();
    bool saveSettingsConfiguration(bool verbose,std::string currentSettingsName);

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

    /**
     * @brief Append the passed message to the output text edit widget
     * @param the message to append to the text widget
     */
    void appendOutputTE(QString msg);
    /**
     * @brief Clear the output text edit widget
     */
    void clearOutputTE();
    /**
     * @brief Gets the algorithm chosen from the GUI
     * @return Returns the algorithm chosen
     */
    std::string getCurrentAlgorithm();
    /**
     * @brief Gets the Minimizer chosen from the GUI
     * @return Returns the minimizer chosen
     */
    std::string getCurrentMinimizer();
    /**
     * @brief Gets the Objective Criterion function from the GUI
     * @return Returns the objective criterion function (as a string)
     */
    std::string getCurrentObjectiveCriterion();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Clears the current message in the output text edit and writes the passed message to it
     * @param font : font to be used for the message
     * @param msg : the content of the message
     */
    void refreshMsg(QFont font, QString msg);
    /**
     * @brief Saves the current application settings to a Qt settings file
     */
    void saveSettings();
    /**
     * @brief Saves application settings but checking with user first
     * @param RunChecks : boolean checking to see if program should check with user prior to saving Model algorithm settings
     */
    void saveSystem(bool RunChecks);
    /**
     * @brief Sets the font for the output text edit widget
     * @param font : the font to use for the output text edit widget
     */
    void setFont(QFont font);
    /**
     * @brief Sets the content for the output text edit widget
     * @param msg : the content to use for the output text edit widget
     */
    void setOutputTE(QString msg);

signals:
    /**
     * @brief Signal sent notifying that user wants to run an estimation
     */
    void RunEstimation();
    /**
     * @brief Signal is sent after user checks the mono font box. It casues the displayed output edit widget to use a monospaced font.
     * @param font : the monospaced font to use in the output edit widget
     */
    void ShowRunMessage(QString font);
    /**
     * @brief Signal sent when the user changes the current Algorithm. Each algorithm has its own set of parameters that must be displayed in the GUI.
     * @param algorithm : the new algorithm selected
     */
    void SetAlgorithm(QString algorithm);
    /**
     * @brief Signal sent to refresh the forecast year range in the Forecast->Setup GUI.
     */
    void UpdateForecastYears();

public Q_SLOTS:
    /**
     * @brief Callback invoked with the user clicks the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when user clicks Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user clicks Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user clicks Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user changes the font in the Run Summary tab
     * @param fontSize : the font selected
     */
    void callback_Estimation_Tab6_FontSizeCMB(QString fontSize);
    /**
     * @brief Callback invoked when user checks the mono font checkbox in the Run Summary tab
     * @param isChecked : check state of the checkbox
     */
    void callback_Estimation_Tab6_MonoCB(int isChecked);
    /**
     * @brief Callback invoked when user changes the Estimation Algorithm
     * @param algorithm : the algorithm selected
     */
    void callback_Estimation_Tab6_EstimationAlgorithmCMB(QString algorithm);
    /**
     * @brief Callback invoked when user checks the Stop When Reach Value checkbox
     * @param isChecked : boolean signiying the check state
     */
    void callback_StopValCB(int isChecked);
    /**
     * @brief Callback invoked when user checks the Stop After Time checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterTimeCB(int isChecked);
    /**
     * @brief Callback invoked when user checks the Stop After Iteraction checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterIterCB(int isChecked);
    /**
     * @brief Callback invoked when the user saves the model on the Setup -> Model Setup GUI
     */
    void callback_SaveSettings();
};

#endif // NMFESTIMATIONTAB6_H
