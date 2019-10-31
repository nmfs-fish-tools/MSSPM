#ifndef NMFFORECASTTAB1_H
#define NMFFORECASTTAB1_H


/**
 * @brief Dialog to Load a Forecast
 *
 * The user can load a previously saved Forecast. To delete
 * a Forecast, the user right clicks over a saved Forecast and
 * selects Delete. In order to avoid accidental deletion, this
 * option was made a bit hard to find.
 */
class LoadForecastDlg : public QDialog
{
    Q_OBJECT

private:
    nmfLogger*               m_logger;
    nmfDatabase*             m_databasePtr;
    QString                  m_ForecastName;
    QLineEdit*               m_ForecastNameLE;
    QSpinBox*                m_ForecastRunLengthSB;
    QSpinBox*                m_ForecastNumRunsSB;
    QDialogButtonBox*        m_ButtonBox;
    QListWidget*             m_ForecastsLW;
    std::vector<std::string> m_ForecastNames;

public:
    /**
     * @brief Dialog to select Forecast to Load
     * @param title : title of the dialog
     * @param parent : pointer to the parent widget of this dialog
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param forecastName : name of Forecast
     * @param runLengthSB : Spin Box for the number of Years per Run
     * @param numRunsSB : Spin Box for the number of Runs in the current Forecast
     */
    LoadForecastDlg(const QString&     title,
                          QWidget*     parent,
                          nmfLogger*   logger,
                          nmfDatabase* databasePtr,
                          QLineEdit*   forecastName,
                          QSpinBox*    runLengthSB,
                          QSpinBox*    numRunsSB);
   ~LoadForecastDlg() {}

    void loadWidgets();
    std::string getSelectedItem();

public slots:
    void callback_LoadOk();
    void callback_itemDoubleClicked(QListWidgetItem* item);
    void callback_ContextMenu(QPoint pos);
    void callback_DeleteSelection();
};


/**
 * @brief The Forecast Setup
 *
 * These widgets allow the user to enter Forecast setup information
 * such as Forecast year range, number of runs in Forecast, and
 * whether the Forecast is deterministic or stochastic.
 */
class nmfForecast_Tab1: public QObject
{
    Q_OBJECT

private:
    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    QStandardItemModel* m_smodel;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab1_Widget;
    QPushButton* Forecast_Tab1_SetNamePB;
    QPushButton* Forecast_Tab1_NextPB;
    QPushButton* Forecast_Tab1_LoadPB;
    QPushButton* Forecast_Tab1_SavePB;
    QGroupBox*   Forecast_Tab1_PreviousRunGB;
    QLabel*      Forecast_Tab1_AlgorithmLBL;
    QLabel*      Forecast_Tab1_MinimizerLBL;
    QLabel*      Forecast_Tab1_ObjectiveCriterionLBL;
    QComboBox*   Forecast_Tab1_AlgorithmCMB;
    QComboBox*   Forecast_Tab1_MinimizerCMB;
    QComboBox*   Forecast_Tab1_ObjectiveCriterionCMB;
    QLineEdit*   Forecast_Tab1_StartYearLE;
    QLineEdit*   Forecast_Tab1_EndYearLE;
    QSpinBox*    Forecast_Tab1_RunLengthSB;
    QLineEdit*   Forecast_Tab1_NameLE;
    QSpinBox*    Forecast_Tab1_NumRunsSB;
    QSpinBox*    Forecast_Tab1_DeterministicSB;
    QCheckBox*   Forecast_Tab1_DeterministicCB;

    void loadForecast(std::string forecastToLoad);
    void getAlgorithms(std::string &algorithm,
                       std::string &minimizer,
                       std::string &objectiveCriterion,
                       std::string &scaling);
    void readSettings();

public:
    /**
     * @brief nmfForecast_Tab1 : class constructor
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab1(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab1();

    void        clearWidgets();
    std::string getForecastName();
    int         getSeed();
    int         getStartForecastYear();
    bool        loadWidgets();
    void        saveSettings();

signals:
    void ForecastLoaded(std::string forecastLoaded);
    void ResetOutputWidgetsForAggProd();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_NextPB();
    void callback_SetNamePB();
    void callback_PreviousRunCB(bool state);
    void callback_AlgorithmCMB(QString algorithm);
    void callback_MinimizerCMB(QString minimizer);
    void callback_RunLengthSB(int duration);
    void callback_DeterministicCB(int checked);
    void callback_UpdateForecastYears();
};

#endif // NMFFORECASTTAB1_H
