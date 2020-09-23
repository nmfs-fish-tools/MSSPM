/**
 * @file MSSPM_GuiManagerMode.h
 * @brief GUI definition for the Control widgets class MSSPM_GuiManagerMode in the REMORA embedded tiik
 *
 * This file contains the GUI definitions for the controls and charts found in the MSSPM_GuiManagerMode
 * class.  This class represents the MSSPM REMORA Management tool and allows managers to quickly and
 * intuitively create, view, and save forecast scenarios.
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

#ifndef MSSPM_GUIMANAGERMODE_H
#define MSSPM_GUIMANAGERMODE_H

#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QFileDialog>
#include <QLineEdit>
#include <QLineSeries>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QVBoxLayout>

#include "nmfChartMovableLine.h"
#include "nmfChartLine.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"

#include <string.h>


/**
 * @brief The main GUI class that contains all of the widget
 * definitions for the REMORA Management tool.
 */
class REMORA : public QObject
{
    Q_OBJECT

private:
    const int     MAX_SCALE_VALUE = 4;

    QCheckBox*    MModeShowMSYCB;
    QCheckBox*    MModePctMSYCB;
    QCheckBox*    MModeYAxisLockCB;
    QComboBox*    MModeSpeciesCMB;
    QComboBox*    MModeForecastPlotTypeCMB;
    QDial*        MModeRParamDL;
    QDial*        MModeKParamDL;
    QDial*        MModeHParamDL;
    QDial*        MModePctMSYDL;
    QLabel*       MModeForecastPlotTypeLB;
    QLabel*       MModeForecastTypeLB;
    QLabel*       MModePlotTypeLB;
    QLabel*       MModePctMSYLBL;
    QLabel*       MModeSpeciesLB;
    QLineEdit*    MModeScenarioNameLE;
    QLineEdit*    MModeYearsPerRunLE;
    QLineEdit*    MModeRunsPerForecastLE;
    QLineEdit*    MModeRParamLE;
    QLineEdit*    MModeKParamLE;
    QLineEdit*    MModeHParamLE;
    QLineEdit*    MModePctMSYLE;
    QPushButton*  MModeForecastRunPB;
    QPushButton*  MModeForecastDelPB;
    QPushButton*  MModeForecastLoadPB;
    QPushButton*  MModeForecastSavePB;
    QPushButton*  MModeHarvestTypePB;
    QPushButton*  MModeMultiPlotTypePB;
    QPushButton*  MModeMaxScaleFactorPB;
    QRadioButton* MModeDeterministicRB;
    QRadioButton* MModeStochasticRB;
    QRadioButton* MModePlotTypeSSRB;
    QRadioButton* MModePlotTypeMSRB;
    QSlider*      MModeYearsPerRunSL;
    QSlider*      MModeRunsPerForecastSL;
    QWidget*      MModeHarvestChartWidget;
    QWidget*      MModeUpperPlotWidget;
    QWidget*      MModeWindowWidget;
    QWidget*      MModeParentChartW;

    bool                  m_ScenarioChanged;
    int                   m_IndexMaxYScaleFactor;
    int                   m_IndexScaleFactorChart1;
    int                   m_IndexScaleFactorChart2;
    int                   m_IndexScaleFactorPoint;
    int                   m_NumRunsPerForecast;
    int                   m_NumUnusedParameters;
    int                   m_NumYearsPerRun;
    double                m_MaxYAxis;
    nmfChartLine*         m_ForecastBiomassLineChart;
    nmfChartLine*         m_ForecastHarvestLineChart;
    nmfChartLine*         m_ForecastLineChartMonteCarlo;
    nmfChartLine*         m_MSYLineChart;
    nmfDatabase*          m_DatabasePtr;
    nmfLogger*            m_Logger;
    std::map<QString,int> m_SpeciesMap;
    std::map<QString, void(REMORA::*)(
            QString arg)> m_FunctionMap;
    std::string           m_ForecastName;
    std::string           m_HarvestType;
    std::string           m_ProjectDir;
    std::string           m_ProjectSettingsConfig;
    std::vector<int>      m_MaxYAxisValues;
    std::vector<nmfChartMovableLine *> m_MovableLineCharts;
    QChart*               m_ChartWidget;
    QChart*               m_MModeHarvestChartWidget;
    QChart*               m_MModeOutputChartWidget;
    QChartView*           m_ChartView;
    QGridLayout*          m_GridLayt;
    QVBoxLayout*          m_GridVLayt;
    QVBoxLayout*          m_VLayt;
    QList<QChart*>        m_Charts;
    QList<QChartView*>    m_Views;
    QString               m_RemoraScenarioDir;
    QWidget*              m_GridParent;
    QWidget*              m_Widget;

    bool couldShowMSYCB();
    void drawMultiSpeciesChart();
    void drawMSYLines();
    void drawMSYLines(
            QChart* chart,
            int& SpeciesNum,
            int& NumSpecies,
            int& NumYearsPerRun,
            int& NumRunsPerForecast,
            int& NoUncertaintyRun,
            int& StartForecastYear,
            int& YMinSliderVal,
            std::string& Algorithm,
            std::string& Minimizer,
            std::string& ObjectiveCriterion,
            std::string& Scaling,
            QStringList& RowLabelsForBars,
            QStringList& ColumnLabelsForLegend,
            QStringList& HoverData,
            std::string& MainTitle,
            std::string& XLabel,
            std::string& YLabel,
            const bool& ShowLegend,
            const double& Pct);
    void drawPlot();
    void drawSingleSpeciesChart();
    void enableWidgets(bool enable);
    QString getCarryingCapacityUncertainty();
    QString getForecastPlotType();
    QString getGrowthUncertainty();
    QString getHarvestType();
    QString getHarvestUncertainty();
    void getLastYearsCatchValues(
            int& lastYear,
            std::vector<double>& lastYearsCatchValues);
    int getMaxYScaleFactor(const int& speciesNum);
    int getNumRunsPerForecast();
    int getNumScaleFactorPoints(const int& speciesNum);
    int getNumSpecies();
    int getNumYearsPerRun();
    double getPctMSYValue();
    QList<QPointF> getScaleFactorPoints(const int& speciesNum);
    QString getScenarioName();
    int getSpeciesNum();
    void getYearRange(int& firstYear, int& lastYear);
    bool isAbsoluteBiomassPlotType();
    bool isFishingMortalityPlotType();
    bool isMultiPlot();
    bool isMultiSpecies();
    bool isMSYBoxChecked();
    bool isPctMSYBoxChecked();
    bool isRelativeBiomassPlotType();
    bool isSingleSpecies();
    bool isYAxisLocked();
    void loadForecastScenario(QString filename);
    void removeAllMSYLines(QString type);
    void removeMSYLines(
            QChart*             chart,
            const QStringList&  MSYTypes);
    void resetControls();
    void resetNumYearsOnScaleFactorCharts();
    void resetScenarioName();
    void resetYearsPerRunOnScaleFactorPlot();
    void saveForecastParameters();
    bool saveForecastScenario(QString filename);
    void saveHarvestData();
    void saveOutputBiomassData();
    void saveUncertaintyParameters();
    void setAScaleFactorPoint(QString arg1);
    void setDeterministic(QString arg1);
    void setForecastPlotType(QString arg1);
    void setHarvestType(QString arg1);
    void setMaxYScaleFactor(QString maxY);
    void setNumRunsPerForecast(QString numRuns);
    void setNumYearsPerRun(QString numYears);
    void setScenarioChanged(bool state);
    void setScenarioName(QString scenarioName);
    void setSingleSpecies(QString arg1);
    void setupConnections();
    void setupMovableLineCharts(const QStringList& SpeciesList);
    void setMultiPlot(QString isChecked);
    void setMSYLineVisible(QString arg1);
    void setNumScaleFactorPoints(QString arg1);
    void setUncertaintyCarryingCapacity(QString arg1);
    void setUncertaintyGrowth(QString arg1);
    void setUncertaintyHarvest(QString arg1);
    void resetXAxis();
    /**
     * @brief Resets the maximum y-axis value of the model plot if the user has
     * checked the lock button in the controls
     */
    void resetYAxis();
    void updateYearlyScaleFactorPoints();

signals:
    /**
     * @brief Signal emitted when the user presses a key over the movable line chart
     * @param event : key event data
     */
    void KeyPressed(QKeyEvent* event);
    /**
     * @brief Signal emitted when the user moves the mouse over the movable line chart
     * @param event : mouse event data
     */
    void MouseMoved(QMouseEvent* event);
    /**
     * @brief Signal emitted when the user releases the mouse over the movable line chart
     * @param event : mouse event data
     */
    void MouseReleased(QMouseEvent* event);
    /**
     * @brief Signal emitted when the user wants to save the output biomass data for the current forecast
     * @param forecastName : the forecast name
     */
    void SaveOutputBiomassData(std::string forecastName);
    /**
     * @brief Signal emitted when the user needs to update the seed value in the main routine when saving the
     * current forecast parameters
     * @param seedValue : 1 or 0 (0 signifies that a random seed value will be used)
     */
    void UpdateSeedValue(int seedValue);

public:

    /**
     * @brief Class definition for REMORA Management Tool
     * @param DatabasePtr : pointer to database
     * @param Logger : pointer to logger
     * @param ProjectDir : project directory
     * @param ProjectSettingsConfig : project settings configuration name
     * @param SpeciesList : list of species
     * @param MModeWidget : parent widget in which to place this class
     */
    REMORA(
            nmfDatabase* DatabasePtr,
            nmfLogger*   Logger,
            std::string& ProjectDir,
            std::string& ProjectSettingsConfig,
            QStringList& SpeciesList,
            QWidget*     MModeWidget);
    ~REMORA();

    /**
     * @brief getScaleValueFromPlot : returns harvest scale value for the passed in species and year
     * @param species : species number from the list of species
     * @param year : year along x-axis of scale factor plot
     * @return Returns scale factor (y-value) corresponding to the passed in values
     */
    double getScaleValueFromPlot(int species, int year);
    /**
     * @brief grabImage : Performs a "smart" screen grab based upon the state of the controls
     * @param pixmap : Returns the pixmap of the captured widget
     */
    void grabImage(QPixmap& pixmap);
    /**
     * @brief Returns true if the Deterministic radio button is pressed
     * @return true if state is deterministic, else false if stochastic
     */
    bool isDeterministic();
    /**
     * @brief Sets REMORA's internal forecast name variable to the passed in forecast name
     * @param forecastName : the name of the current forecast
     */
    void setForecastName(QString forecastName);
    /**
     * @brief Sets REMORA's internal number years per run variable to the passed value and updates the GUI
     * @param numYearsPerRun : number of years per forecast run
     */
    void setForecastNumYearsPerRun(int numYearsPerRun);
    /**
     * @brief Sets REMORA's internal number of runs per forecast to the passed value and updates the GUI
     * @param numRunsPerForecast : number of runs to generate per forecast scenario run
     */
    void setForecastNumRunsPerForecast(int numRunsPerForecast);
    /**
     * @brief Sets REMORA's species pulldown list to the passed list of QStrings
     * @param speciesList : current list of species in model
     */
    void setSpeciesList(const QStringList& speciesList);


public Q_SLOTS:
    /**
     * @brief Callback invoked when the user presses the delete scenario button
     */
    void callback_DelPB();
    /**
     * @brief Callback invoked when the user presses the Deterministic radio button
     * @param pressed : state of the radio button (true/false)
     */
    void callback_DeterministicRB(bool pressed);
    /**
     * @brief Callback invoked when the user selects a new forecast plot type
     * @param type : name of the forecast plot type chosen
     */
    void callback_ForecastPlotTypeCMB(QString type);
    /**
     * @brief Callback invoked when the user presses a keyboard key
     * @param event : key event data structure
     */
    void callback_KeyPressed(QKeyEvent* event);
    /**
     * @brief Callback invoked when the user presses the Load Scenario button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user toggles the maximum scale factor button
     */
    void callback_MaxScaleFactorPB();
    /**
     * @brief Callback invoked when the user moves the mouse
     * @param event : mouse event data
     */
    void callback_MouseMoved(QMouseEvent* event);
    /**
     * @brief Callback invoked when the user releases the mouse
     * @param event : mouse event data
     */
    void callback_MouseReleased(QMouseEvent* event);
    /**
     * @brief Callback invoked when the user checks the MSY checkbox
     * @param isChecked : boolean signifying if the checkbox has been checked
     */
    void callback_MSYCB(bool isChecked);
    /**
     * @brief Callback invoked when the user presses the multi-species plot type
     * toggle button. There are states: 1 and 1+. The former signifies the user wishes
     * to see all plots on the same chart (with no Monte Carlo plots). The latter signifies
     * the user wishes to see all plots on separate smaller charts (with the Monte Carlo plots)
     * and composited into one image.
     */
    void callback_MultiPlotTypePB();
    /**
     * @brief Callback invoked when the user presses the Multi-Species radio button
     * @param pressed : state of the Multi-Species button
     */
    void callback_MultiSpeciesRB(bool pressed);
    /**
     * @brief Callback invoked when the user checks the Percent MSY checkbox. This will enable the
     * Percent MSY dial to the right of this checkbox.
     * @param isChecked : state of checkbox
     */
    void callback_PctMSYCB(bool isChecked);
    /**
     * @brief Callback invoked when the user modifies the Percent MSY dial
     * @param value : Integer value from the Percent MSY dial
     */
    void callback_PctMSYDL(int value);
    /**
     * @brief Callback invoked when the user presses the Run button. Run causes the appropriate
     * forecast scenario biomass data to be generated and saved in the database for each species.
     * A default plot is then displayed using these data. Additional plots can be viewed by selecting
     * another species or by changing the plot type.
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when the user interacts with the Runs per Forecast slider
     * @param value : current value on the Runs per Forecast slider
     */
    void callback_RunsPerForecastSL(int value);
    /**
     * @brief Callback invoked when the user presses the Save Scenario button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user presses the single species radio button
     * @param pressed : state of the single species radio button
     */
    void callback_SingleSpeciesRB(bool pressed);
    /**
     * @brief Callback invoked when the user changes the species combo box
     * @param species : current species shown in the species combo box
     */
    void callback_SpeciesCMB(QString species);
    /**
     * @brief Callback invoked when the user presses the stochastic radio button
     * @param pressed : state of the stochastic radio button
     */
    void callback_StochasticRB(bool pressed);
    /**
     * @brief Callback invoked when the user modifies the Harvest Uncertainty dial
     * @param value : current value of the Harvest Uncertainty dial
     */
    void callback_UncertaintyHarvestParameterDL(int value);
    /**
     * @brief Callback invoked when the user presses the Harvest Uncertainty parameter toggle
     * button
     */
    void callback_UncertaintyHarvestParameterPB();
    /**
     * @brief Callback invoked when the user modifies the Carrying Capacity Uncertainty dial
     * @param value : current value of the Carrying Capacity Uncertainty dial
     */
    void callback_UncertaintyKParameterDL(int value);
    /**
     * @brief Callback invoked when the user modifies the Growth Uncertainty dial
     * @param value : current value of the Growth Uncertainty dial
     */
    void callback_UncertaintyRParameterDL(int value);
    /**
     * @brief Callback invoked when the user checks the Lock Y Axis checkbox. Checking this box
     * forces all subsequent plots to have the same maximum y range value. This is useful if
     * multiple plots will be captured and then flipped through to view differences.
     * @param checked : boolean determining if the box has just been checked (true) or unchecked (false)
     */
    void callback_YAxisLockedCB(bool checked);
    /**
     * @brief Callback invoked when the user releases the Years per Run slider
     * @param value : current value of the Years per Run slider
     */
    void callback_YearsPerRunSL(int value);
};

#endif // MSSPM_GUIMANAGERMODE_H
