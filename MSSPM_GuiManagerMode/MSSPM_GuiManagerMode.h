#ifndef MSSPM_GUIMANAGERMODE_H
#define MSSPM_GUIMANAGERMODE_H

#include <QCheckBox>
#include <QDial>
#include <QLineSeries>
#include <QVBoxLayout>
#include "nmfChartMovableLine.h"
#include "nmfChartLine.h"
#include <string.h>


//QT_CHARTS_USE_NAMESPACE

/**
 * @brief The MSSPM_GuiManagerMode class
 */
class MSSPM_GuiManagerMode : public QObject
{
    Q_OBJECT

private:
    QSlider*     MModeYearsPerRunSL;
    QSlider*     MModeRunsPerForecastSL;
    QLineEdit*   MModeYearsPerRunLE;
    QLineEdit*   MModeRunsPerForecastLE;
    QLineEdit*   MModeRParamLE;
    QLineEdit*   MModeKParamLE;
    QLineEdit*   MModeCParamLE;
    QDial*       MModeRParamDL;
    QDial*       MModeKParamDL;
    QDial*       MModeCParamDL;
    QComboBox*   MModeSpeciesCMB;
    QWidget*     MModeHarvestChartWidget;
    QWidget*     MModeUpperPlotWidget;
    QWidget*     MModeWindowWidget;
    QPushButton* MModeForecastRunPB;
    QCheckBox*   MModeShowMSYCB;
    QSlider*     MModeDetStocSL;
    QDial*       MModePctMSYDL;
    QLineEdit*   MModePctMSYLE;
    QCheckBox*   MModeEnablePctMSYCB;

    QChart*              m_MModeHarvestChartWidget;
    QChart*              m_MModeOutputChartWidget;
    nmfDatabase*         m_DatabasePtr;
    nmfLogger*           m_Logger;
    std::string          m_ProjectSettingsConfig;
    nmfChartMovableLine* m_MovableLineChart;
    QChart*              m_ChartWidget;
    QChartView*          m_ChartView;
    nmfChartLine*        m_ForecastLineChart;
    nmfChartLine*        m_MSYLineChart;

    int         m_NumUnusedParameters;
    std::string m_ForecastName;
    std::string m_HarvestType;
    int m_NumYearsPerRun;
    int m_NumRunsPerForecast;

    void drawChart(bool MSYOnly);
    void saveHarvestData();
    void saveUncertaintyParameters();
    void saveOutputBiomassData();
    double getScaleValueFromPlot(int speciesNum,
                            int yearNum);
    void getYearRange(int& firstYear, int& lastYear);
    int getNumYearsPerRun();
    int getNumRunsPerForecast();
    bool isMSYBoxChecked();
    bool isEnablePctMSYBoxChecked();

signals:
    void KeyPressed(QKeyEvent* event);
    void MouseMoved(QMouseEvent* event);
    void SaveOutputBiomassData(std::string forecastName);

public:
    /**
     * @brief MSSPM_GuiManagerMode
     * @param MModeWidget
     */
    MSSPM_GuiManagerMode(
            nmfDatabase* databasePtr,
            nmfLogger*   logger,
            std::string& projectSettingsConfig,
            QWidget*     MModeWidget);
    ~MSSPM_GuiManagerMode();



    void getLastYearsCatchValues(
            int& lastYear,
            std::vector<double>& lastYearsCatchValues);

    /**
     * @brief setupConnections
     */
    void setupConnections();

    /**
     * @brief setupConnections
     */
    void setData(const QStringList& speciesList);

public Q_SLOTS:

    void callback_PctMSYDL();
    void callback_EnablePctMSYCB(bool isChecked);
    /**
     * @brief callback_MSYCB
     * @param isChecked
     */
    void callback_MSYCB(bool isChecked);
    /**
     * @brief callback_YearsPerRun
     * @param value
     */
    void callback_YearsPerRun(int value);
    /**
     * @brief callback_RunsPerFore
     * @param value
     */
    void callback_RunsPerFore(int value);
    /**
     * @brief callback_PctMSY
     * @param value
     */
    void callback_PctMSY(int value);
    /**
     * @brief callback_RParam
     * @param value
     */
    void callback_RParam(int value);
    /**
     * @brief callback_KParam
     * @param value
     */
    void callback_KParam(int value);
    /**
     * @brief callback_CParam
     * @param value
     */
    void callback_CParam(int value);
    void callback_RunPB();
    void callback_RunPB(bool MSYOnly);
    void callback_keyPressed(QKeyEvent* event);
    void callback_mouseMoved(QMouseEvent* event);

};

#endif // MSSPM_GUIMANAGERMODE_H
