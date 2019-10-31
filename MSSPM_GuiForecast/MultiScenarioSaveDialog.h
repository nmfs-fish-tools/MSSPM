#pragma once

#include "nmfDatabase.h"
#include "nmfLogger.h"


/**
 * @brief Allows User to Save Multiple Forecasts
 *
 * Users may save multiple Forecasts into a Scenario for comparative
 * analysis.
 */
class MultiScenarioSaveDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;
    nmfLogger*   m_logger;
    std::string  m_scenarioName;
    std::string  m_forecastName;
    std::map<QString,QStringList> m_orderedForecastLabelsMap;

    QVBoxLayout* MainVLT;
    QHBoxLayout* BtnHLT;
    QHBoxLayout* ScenarioHLT;
    QHBoxLayout* ForecastHLT;
    QHBoxLayout* ShowSortHLT;
    QHBoxLayout* ForecastLabelHLT;
    QLabel*      ScenarioNameLBL;
    QLabel*      ForecastLabelLBL;
    QLabel*      ShowSortLBL;
    QComboBox*   ScenarioNameCMB;
    QComboBox*   ForecastLabelCMB;
    QPushButton* DelScenarioPB;
    QPushButton* DelForecastPB;
    QPushButton* NewScenarioPB;
    QPushButton* SetForecastPB;
    QPushButton* CancelPB;
    QPushButton* OkPB;
    QPushButton* SetOrderPB;
    QPushButton* ShowSortPB;
    QPushButton* RenScenarioPB;
    QPushButton* RenForecastPB;

    // RSK - Using a QListWidget here instead of a QListView as the latter
    // had a problem with losing items if they were dropped on top of another item.
    QListWidget* ForecastLabelLW;

    bool getForecastData(
            const std::string& forecastName,
            std::vector<std::string>& Species,
            std::vector<int>& Years,
            boost::numeric::ublas::matrix<double>& ForecastBiomass);
    void loadScenarioMap();
    void renameScenarioName(QString oldScenario,
                            QString newScenario);
    void renameForecastLabel(QString scenario,
                             QString oldForecast,
                             QString newForecast);

public:
    /**
     * @brief MultiScenarioSaveDialog : class constructor
     * @param parent : the tab widget into which this Estimation tab will be placed
     * @param databasePtr : pointer to the application database
     * @param logger : pointer to the application logger
     * @param SortedForecastLabelsMap : map of Forecasts per Scenario
     * @param currentScenario : name of Scenario that will store passed Forecast
     * @param forecastName : name of Forecast to add to Scenario
     */
    MultiScenarioSaveDialog(QTabWidget*  parent,
                            nmfDatabase* databasePtr,
                            nmfLogger*   logger,
                            std::map<QString,QStringList>& SortedForecastLabelsMap,
                            std::string& currentScenario,
                            std::string  forecastName);
    virtual ~MultiScenarioSaveDialog() {}

    std::string getScenarioName();
    std::string getForecastLabel();
    void getOrderedForecastLabels(std::map<QString,
                                  QStringList>& orderedForecastLabelsMap);
    void printMap(QString msg);
    void loadWidgets();

signals:
    void RefreshOutput();

private Q_SLOTS:
    void callback_ScenarioNameCMB(QString algorithm);
    void callback_NewScenarioPB();
    void callback_DelScenarioPB();
    void callback_RenScenarioPB();
    void callback_SetForecastPB();
    void callback_DelForecastPB();
    void callback_RenForecastPB();
    void callback_OkPB();
    void callback_SetOrderPB();
    void callback_ShowSortPB();
};

