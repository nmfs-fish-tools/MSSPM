#ifndef NMFFORECASTTAB4_H
#define NMFFORECASTTAB4_H

#include "MultiScenarioSaveDialog.h"

/**
 * @brief Run Information
 *
 * This allows the user to run the Forecast as well as add the
 * Forecast to a Scenario.
 */
class nmfForecast_Tab4: public QObject
{
    Q_OBJECT

    nmfLogger*               m_logger;
    nmfDatabase*             m_databasePtr;
    int                      m_fontSize;
    std::string              m_currentScenario;
    std::string              m_ProjectDir;
    std::string              m_ProjectSettingsConfig;
    std::string              m_EstimationOutputFile;
    std::string              m_EstimationDataFile;
    std::string              m_EstimationID;
    MultiScenarioSaveDialog* m_SaveDlg;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab4_Widget;
    QTextEdit*   Forecast_Tab4_OutputTE;
    QPushButton* Forecast_Tab4_RunPB;
    QPushButton* Forecast_Tab4_SaveToMultiScenarioPB;
    QPushButton* Forecast_Tab4_PrevPB;
    QLineEdit*   Forecast_Tab4_OutputFileLE;
    QLineEdit*   Forecast_Tab4_DataFileLE;
    QLineEdit*   Forecast_Tab4_IDLE;
    QPushButton* Forecast_Tab4_OutputFileBrowsePB;
    QPushButton* Forecast_Tab4_OutputFileCreatePB;
    QPushButton* Forecast_Tab4_DataFileBrowsePB;
    QComboBox*   Forecast_Tab4_FontSizeCMB;
    QLineEdit*   Forecast_Tab1_NameLE;

public:
    /**
     * @brief nmfForecast_Tab4 : class constructor
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab4(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab4();

    void appendOutputTE(QString msg);
    void clearOutputTE();
    std::string getCurrentForecastName();
    bool loadWidgets();
    void readSettings();
    void saveSettings();
    void setFontSize(int fontSize);
    void setOutputScenario(std::string scenario);
    void setOutputTE(QString msg);

signals:
    void RunForecast(std::string forecastName, bool generateBiomass);
    void RefreshOutput();
    void SetChartType(QString type,std::map<QString,QStringList> sortedForecastLabelsMap);
    void UpdateOutputScenarios();
    void QueryOutputScenario();
    void SetOutputScenarioText(QString scenario);

public Q_SLOTS:
    void callback_RunPB();
    void callback_PrevPB();
    void callback_Forecast_Tab4_FontSizeCMB(QString theFontSize);
    void callback_RunMultiScenarioPB();
    void callback_RefreshOutput();
};

#endif // NMFFORECASTTAB4_H
