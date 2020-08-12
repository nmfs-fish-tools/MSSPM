#include "MSSPM_GuiManagerMode.h"

MSSPM_GuiManagerMode::MSSPM_GuiManagerMode(
        nmfDatabase* databasePtr,
        nmfLogger* logger,
        std::string& projectSettingsConfig,
        QWidget* MModeWidget)
{
    int startYear;
    int endYear;
    int endForecastYear;
    std::string MainTitle = "Harvest Scale Factor";
    std::string XTitle = "Year";
    std::string YTitle = "Scale Factor";

    m_MovableLineChart = new nmfChartMovableLine(
                MainTitle,XTitle,YTitle);

    // ----------------------------------------
    // Change these lines.....
    m_NumUnusedParameters   = 7;
    m_ForecastName          = "Sample_Schaefer";
    m_HarvestType           = "ForecastCatch";
    m_NumYearsPerRun        = 20;  // remove this later
    m_NumRunsPerForecast    = 10;  // remove this later
    // ----------------------------------------

    m_DatabasePtr           = databasePtr;
    m_Logger                = logger;
    m_ProjectSettingsConfig = projectSettingsConfig;

    MModeYearsPerRunSL      = MModeWidget->findChild<QSlider*    >("MModeYearsPerRunSL");
    MModeRunsPerForecastSL  = MModeWidget->findChild<QSlider*    >("MModeRunsPerForecastSL");
    MModeDetStocSL          = MModeWidget->findChild<QSlider*    >("MModeDetStocSL");
    MModeYearsPerRunLE      = MModeWidget->findChild<QLineEdit*  >("MModeYearsPerRunLE");
    MModeRunsPerForecastLE  = MModeWidget->findChild<QLineEdit*  >("MModeRunsPerForecastLE");
    MModePctMSYLE           = MModeWidget->findChild<QLineEdit*  >("MModePctMSYLE");
    MModeRParamLE           = MModeWidget->findChild<QLineEdit*  >("MModeRParamLE");
    MModeKParamLE           = MModeWidget->findChild<QLineEdit*  >("MModeKParamLE");
    MModeCParamLE           = MModeWidget->findChild<QLineEdit*  >("MModeCParamLE");
    MModePctMSYDL           = MModeWidget->findChild<QDial*      >("MModePctMSYDL");
    MModeRParamDL           = MModeWidget->findChild<QDial*      >("MModeRParamDL");
    MModeKParamDL           = MModeWidget->findChild<QDial*      >("MModeKParamDL");
    MModeCParamDL           = MModeWidget->findChild<QDial*      >("MModeCParamDL");
    MModeSpeciesCMB         = MModeWidget->findChild<QComboBox*  >("MModeSpeciesCMB");
    MModeUpperPlotWidget    = MModeWidget->findChild<QWidget*    >("MModeUpperPlotWidget");
    MModeHarvestChartWidget = MModeWidget->findChild<QWidget*    >("MModeHarvestChartWidget");
    MModeWindowWidget       = MModeWidget->findChild<QWidget*    >("MModeWindowWidget");
    MModeForecastRunPB      = MModeWidget->findChild<QPushButton*>("MModeForecastRunPB");
    MModeShowMSYCB          = MModeWidget->findChild<QCheckBox*  >("MModeShowMSYCB");
    MModeEnablePctMSYCB     = MModeWidget->findChild<QCheckBox*  >("MModeEnablePctMSYCB");

    // RSK - remove this later
    MModeDetStocSL->setValue(1);

    // RSK hardcode to 20 years
    getYearRange(startYear,endYear);
    endForecastYear = endYear + m_NumYearsPerRun;
    m_MovableLineChart->populateChart(MModeHarvestChartWidget,
                                      MModeWindowWidget,
                                      endYear,
                                      endForecastYear);

    //Defaulting the dials to their lowest values
    MModePctMSYDL->setValue(0);
    MModeRParamDL->setValue(0);
    MModeKParamDL->setValue(0);
    MModeCParamDL->setValue(0);
    MModePctMSYLE->setEnabled(false);
    MModePctMSYDL->setEnabled(false);

    //Defaulting the sliders to their lowest values
//  MModeYearsPerRunSL->setValue(1);
    MModeYearsPerRunSL->setValue(m_NumYearsPerRun);
//  MModeRunsPerForecastSL->setValue(1);
    MModeRunsPerForecastSL->setValue(m_NumRunsPerForecast);

    //Setting the paired line edits to the lowest values of their respective widgets
//  MModeYearsPerRunLE->setText(QString::number(1));
    MModeYearsPerRunLE->setText(QString::number(m_NumYearsPerRun));
//  MModeRunsPerForecastLE->setText(QString::number(1));
    MModeRunsPerForecastLE->setText(QString::number(m_NumRunsPerForecast));
    MModePctMSYLE->setText(QString::number(0));
    MModeRParamLE->setText(QString::number(0));
    MModeKParamLE->setText(QString::number(0));
    MModeCParamLE->setText(QString::number(0));

    // Setup chart widgets
    m_ChartWidget = new QChart();
    m_ChartView   = new QChartView(m_ChartWidget);
    QVBoxLayout* vlayt = new QVBoxLayout();
    vlayt->addWidget(m_ChartView);
    MModeUpperPlotWidget->setLayout(vlayt);
    m_ForecastLineChart = new nmfChartLine();
    m_MSYLineChart      = new nmfChartLine();

    setupConnections();

}

MSSPM_GuiManagerMode::~MSSPM_GuiManagerMode()
{

}

void
MSSPM_GuiManagerMode::getYearRange(int& firstYear, int& lastYear)
{
    int StartYear;
    int NumYears;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"StartYear","RunLength"};
    queryStr = "SELECT StartYear,RunLength from Systems where SystemName = '" +
            QString::fromStdString(m_ProjectSettingsConfig).split("__")[0].toStdString() + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["RunLength"].size() != 0) {
        StartYear = std::stoi(dataMap["StartYear"][0]);
        NumYears  = std::stoi(dataMap["RunLength"][0]);
    }
    firstYear = StartYear;
    lastYear  = firstYear + NumYears;
}

void
MSSPM_GuiManagerMode::setupConnections()
{
    std::cout << "Setup Connections begins." << std::endl;

    connect(this,                   SIGNAL(KeyPressed(QKeyEvent*)),
            m_MovableLineChart,     SLOT(callback_keyPressed(QKeyEvent*)));
    connect(this,                   SIGNAL(MouseMoved(QMouseEvent*)),
            m_MovableLineChart,     SLOT(callback_mouseMoved(QMouseEvent*)));

    connect(MModeYearsPerRunSL,     SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_YearsPerRun(int)));
    connect(MModeRunsPerForecastSL, SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_RunsPerFore(int)));
    connect(MModePctMSYDL,          SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_PctMSY(int)));
    connect(MModeRParamDL,          SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_RParam(int)));
    connect(MModeKParamDL,          SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_KParam(int)));
    connect(MModeCParamDL,          SIGNAL(sliderMoved(int)),
            this,                   SLOT(callback_CParam(int)));
    connect(MModeForecastRunPB,     SIGNAL(clicked()),
            this,                   SLOT(callback_RunPB()));
    connect(MModeShowMSYCB,         SIGNAL(toggled(bool)),
            this,                   SLOT(callback_MSYCB(bool)));
    connect(MModeEnablePctMSYCB,    SIGNAL(toggled(bool)),
            this,                   SLOT(callback_EnablePctMSYCB(bool)));
    connect(MModePctMSYDL,          SIGNAL(sliderReleased()),
            this,                   SLOT(callback_PctMSYDL()));
}

void
MSSPM_GuiManagerMode::setData(const QStringList& speciesList)
{
    std::cout << "Set Data works" << std::endl;

    MModeSpeciesCMB->clear();
    MModeSpeciesCMB->addItems(speciesList);
}

void
MSSPM_GuiManagerMode::callback_YearsPerRun(int value)
{
    MModeYearsPerRunLE->setText(QString::number(value));
}

void
MSSPM_GuiManagerMode::callback_RunsPerFore(int value)
{
    MModeRunsPerForecastLE->setText(QString::number(value));
}

void
MSSPM_GuiManagerMode::callback_PctMSY(int value)
{
    MModePctMSYLE->setText(QString::number(value / 100.0));
}

void
MSSPM_GuiManagerMode::callback_RParam(int value)
{
    MModeRParamLE->setText(QString::number(value / 100.0));
}

void
MSSPM_GuiManagerMode::callback_KParam(int value)
{
    MModeKParamLE->setText(QString::number(value / 100.0));
}

void
MSSPM_GuiManagerMode::callback_CParam(int value)
{
    MModeCParamLE->setText(QString::number(value / 100.0));
}

void
MSSPM_GuiManagerMode::callback_RunPB()
{
    callback_RunPB(false);
}

void
MSSPM_GuiManagerMode::callback_RunPB(bool MSYOnly)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_MovableLineChart->calculateYearlyPoints();

    saveUncertaintyParameters();
    saveHarvestData();
    saveOutputBiomassData();
    drawChart(MSYOnly);

    QApplication::restoreOverrideCursor();

}

void
MSSPM_GuiManagerMode::callback_EnablePctMSYCB(bool isChecked)
{
    MModePctMSYLE->setEnabled(isChecked);
    MModePctMSYDL->setEnabled(isChecked);
    callback_RunPB(true);
}


bool
MSSPM_GuiManagerMode::isMSYBoxChecked()
{
    return MModeShowMSYCB->isChecked();
}

bool
MSSPM_GuiManagerMode::isEnablePctMSYBoxChecked()
{
    return MModeEnablePctMSYCB->isChecked();
}


int
MSSPM_GuiManagerMode::getNumYearsPerRun()
{
    return MModeYearsPerRunLE->text().toInt();
}

int
MSSPM_GuiManagerMode::getNumRunsPerForecast()
{
    return MModeRunsPerForecastLE->text().toInt();
}

void
MSSPM_GuiManagerMode::callback_keyPressed(QKeyEvent* event)
{
     emit KeyPressed(event);
}

void
MSSPM_GuiManagerMode::callback_mouseMoved(QMouseEvent* event)
{
    emit MouseMoved(event);
}

void
MSSPM_GuiManagerMode::callback_MSYCB(bool isChecked)
{
    callback_RunPB(true);
}


void
MSSPM_GuiManagerMode::callback_PctMSYDL()
{
    callback_RunPB(true);
}



void
MSSPM_GuiManagerMode::saveUncertaintyParameters()
{
    std::string cmd;
    std::string errorMsg;
    std::vector<std::string> SpeNames;

    for (int i = 0; i < MModeSpeciesCMB->count(); i++)
    {
        SpeNames.push_back(MModeSpeciesCMB->itemText(i).toStdString());
    }

    std::string ForecastName = "Sample_Schaefer";
    std::vector<std::string> fields = {"ForecastName","Algorithm","Minimizer","ObjectiveCriterion","Scaling"};
    std::string queryStr = "SELECT ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling FROM Forecasts where ";
    queryStr += "ForecastName = '" + ForecastName + "'";
    std::map<std::string, std::vector<std::string> > dataMap = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);

    int NumRecords = dataMap["ForecastName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found." << std::endl;
        std::cout << queryStr << std::endl;
        return;
    }

    std::string Algorithm          = dataMap["Algorithm"][0];
    std::string Minimizer          = dataMap["Minimizer"][0];
    std::string ObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    std::string Scaling            = dataMap["Scaling"][0];
    std::string GrowthRate         = MModeRParamLE->text().toStdString();
    std::string CarryingCapacity   = MModeKParamLE->text().toStdString();
    std::string Harvest            = MModeCParamLE->text().toStdString();

    // Clear previous entry in ForecastUncertainty table
    cmd = "DELETE FROM ForecastUncertainty WHERE ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(MModeWindowWidget, "Error",
                             "\nError in Save command.  Couldn't delete all records from ForecastUncertainty table.\n",
                             QMessageBox::Ok);
        return;
    }

    cmd  = "INSERT INTO ForecastUncertainty (" ;
    cmd += "SpeName,ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    cmd += "GrowthRate,CarryingCapacity,Predation,Competition,BetaSpecies,";
    cmd += "BetaGuilds,Handling,Exponent,Catchability,Harvest) VALUES ";
    for (int i = 0; i < SpeNames.size(); ++i) { // Species
            cmd += "('" + SpeNames[i] + "','" + ForecastName + "','" + Algorithm +
                    "','" + Minimizer + "','" + ObjectiveCriterion + "','" + Scaling + "'";
            cmd += "," + GrowthRate;
            cmd += "," + CarryingCapacity;
            for (int i=0;i<m_NumUnusedParameters;++i) {
                cmd += ", 0";
            }
            cmd += "," + Harvest + "),";

            //Ask about the number of columns in the parameter count
            //ERROR: MSSPM_GuiManagerMode::callback_SavePB: Write table error: Column count doesn't match value count at row 1 QMYSQL: Unable to execute query
    }
    cmd = cmd.substr(0,cmd.size()-1);

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(MModeWindowWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return;
    }
}

void
MSSPM_GuiManagerMode::getLastYearsCatchValues(
        int& lastYear,
        std::vector<double>& lastYearsCatchValues)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    lastYearsCatchValues.clear();

    // Get last year's catch data
    fields    = {"Value"};
    queryStr  = "SELECT Value from Catch where SystemName = '" +
                 QString::fromStdString(m_ProjectSettingsConfig).split("__")[0].toStdString() + "'";
    queryStr += " AND Year = " + std::to_string(lastYear-1);
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["Value"].size(); ++i) {
        lastYearsCatchValues.push_back(std::stod(dataMap["Value"][i]));
    }

}


void
MSSPM_GuiManagerMode::saveHarvestData()
{
    int NumYears = 0;
    int startYear;
    int endYear;
    int NumYearsInForecast = getNumYearsPerRun();
    std::string cmd;
    std::string errorMsg;
    double finalValue;
    std::vector<std::string> SpeNames;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    std::vector<double> lastYearsCatchValues;

    bool systemFound = m_DatabasePtr->getAlgorithmIdentifiers(
                0,m_Logger,m_ProjectSettingsConfig,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);
    if (! systemFound) {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::saveHarvestData: No systems found");
        return;
    }

    getYearRange(startYear,endYear);
    NumYears = endYear-startYear+1;

    m_DatabasePtr->getAllSpecies(m_Logger,SpeNames);

    cmd = "DELETE FROM " + m_HarvestType + " WHERE ForecastName = '" +
           m_ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::saveHarvestData: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(0, "Error",
                             "\nError in Save command.  Couldn't delete all records from" +
                             QString::fromStdString(m_HarvestType) + " table.\n",
                             QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        return;
    }


    getLastYearsCatchValues(NumYears,lastYearsCatchValues);


    cmd = "INSERT INTO " + m_HarvestType + " (ForecastName,Algorithm,Minimizer,ObjectiveCriterion,Scaling,SpeName,Year,Value) VALUES ";
    for (int speciesNum=0; speciesNum<SpeNames.size(); ++speciesNum) { // Species

        for (int yearNum=0; yearNum<=NumYearsInForecast; ++yearNum) { // Time

            finalValue = getScaleValueFromPlot(speciesNum,yearNum) * lastYearsCatchValues[speciesNum];

            cmd += "('" + m_ForecastName +
                    "','" + Algorithm + "','" + Minimizer +
                    "','" + ObjectiveCriterion + "','" + Scaling +
                    "','" + SpeNames[speciesNum] + "'," + std::to_string(yearNum) +
                    ", " + std::to_string(finalValue) + "),";
        }
    }
    cmd = cmd.substr(0,cmd.size()-1);
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::saveHarvestData: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QApplication::restoreOverrideCursor();
        return;
    }

    QApplication::restoreOverrideCursor();
}


double
MSSPM_GuiManagerMode::getScaleValueFromPlot(int speciesNum,
                                            int yearNum)
{
    double scaleValue = m_MovableLineChart->getYValue(yearNum);
    if (scaleValue < 0) {
        m_Logger->logMsg(nmfConstants::Error,"MSSPM_GuiManagerMode::getScaleValueFromPlot: Error reading from Scale plot");
        return 0;
    }
    return scaleValue;
}

void
MSSPM_GuiManagerMode::saveOutputBiomassData()
{

    emit SaveOutputBiomassData(m_ForecastName);
}

void
MSSPM_GuiManagerMode::drawChart(bool MSYOnly)
{
    int StartYear;
    int EndYear;
    int StartForecastYear;
    int YMinSliderVal = 0;
    int NumYearsPerRun     = getNumYearsPerRun();
    int NumRunsPerForecast = getNumRunsPerForecast();
    std::string TableName = "Forecasts";
    std::string ChartType = "Line";
    std::string LineStyle = "SolidLine";
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend;
    std::string MainTitle = "Forecast Run";
    std::string XLabel = "Year";
    std::string YLabel = "Biomass (metric tons)";
    std::vector<bool> GridLines(true,true); // Replace with checkbox values
    int Theme = 0; // Replace with checkbox values
    QList<QColor> LineColors;
    std::string lineColorName = "MonteCarloSimulation";
    boost::numeric::ublas::matrix<double> ChartLineData;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::vector<boost::numeric::ublas::matrix<double> > ForecastBiomassMonteCarlo;
    std::vector<std::string> SpeNames;
    int NumSpecies;
    boost::numeric::ublas::matrix<double> ChartBMSYData;
    int line = 0; // RSK change this later
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QList<double> BMSYValues;
    QList<double> MSYValues;
    QList<double> FMSYValues;
    std::string isAggProdStr = "0"; // RSK fix this later
    double pct = 1.0;

    LineColors.append(QColor(nmfConstants::LineColors[0].c_str()));

    m_DatabasePtr->getAllSpecies(m_Logger,SpeNames);
    NumSpecies = SpeNames.size();

    getYearRange(StartYear,EndYear);
    StartForecastYear = EndYear;

    ChartLineData.resize(NumYearsPerRun+1,NumRunsPerForecast);
    ChartLineData.clear();

    if (! m_DatabasePtr->getForecastInfo(
         TableName,m_ForecastName,NumYearsPerRun,StartForecastYear,
         Algorithm,Minimizer,ObjectiveCriterion,Scaling,NumRunsPerForecast)) {
        return;
    }

    if (! m_DatabasePtr->getForecastBiomassMonteCarlo(0,m_Logger,
          m_ForecastName,NumSpecies,NumYearsPerRun,NumRunsPerForecast,
          Algorithm,Minimizer,ObjectiveCriterion,Scaling,
          ForecastBiomassMonteCarlo)) {
        return;
    }

    // Get ChartLineData
    int SpeciesNum  = 0;   // RSK - remove this later
    double ScaleVal = 1.0; // RSK - remove this later
    for (int line=0; line<NumRunsPerForecast; ++line)
    {
        for (int species=0; species<NumSpecies; ++species) {
            if (species == SpeciesNum) {
                for (int time=0; time<=NumYearsPerRun; ++time) {
                    ChartLineData(time,line) = ForecastBiomassMonteCarlo[line](time,species)/ScaleVal;
                }
            }
        }
    }

    if (! MSYOnly) {

        // Draw forecast line(s)
        m_ChartWidget->removeAllSeries();
        m_ForecastLineChart->populateChart(
                    m_ChartWidget,
                    ChartType,
                    LineStyle,
                    nmfConstantsMSSPM::ShowFirstPoint,
                    StartForecastYear,
                    nmfConstantsMSSPM::LabelXAxisAsInts,
                    YMinSliderVal,
                    ChartLineData,
                    RowLabelsForBars,
                    ColumnLabelsForLegend,
                    MainTitle,
                    XLabel,
                    YLabel,
                    GridLines,
                    Theme,
                    LineColors[0],
                lineColorName,
                1.0);
    }

    if (1) {

        // Remove any existing MSY series
        QList<QAbstractSeries*> allSeries = m_ChartWidget->series();
        int NumSeries = allSeries.size();
        if (NumSeries > NumRunsPerForecast) {
            for (int i=NumRunsPerForecast; i<NumSeries;++i) {
                m_ChartWidget->removeSeries(allSeries[i]);
            }
        }

        TableName = "OutputMSYBiomass";
        ChartBMSYData.resize(NumYearsPerRun+1,1);
        ChartBMSYData.clear();

        fields     = {"Algorithm","Minimizer","ObjectiveCriterion","Scaling","isAggProd","SpeName","Value"};
        queryStr   = "SELECT Algorithm,Minimizer,ObjectiveCriterion,Scaling,isAggProd,SpeName,Value FROM " + TableName;
        queryStr  += " WHERE Algorithm = '"        + Algorithm +
                "' AND Minimizer = '"          + Minimizer +
                "' AND ObjectiveCriterion = '" + ObjectiveCriterion +
                "' AND Scaling = '"            + Scaling +
                "' AND isAggProd = "           + isAggProdStr +
                "  ORDER by SpeName";

        dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
        int NumRecords = dataMap["SpeName"].size();
        if (NumRecords == 0) {
            m_Logger->logMsg(nmfConstants::Error, queryStr);
            return;
        }


        for (int ii=0; ii<2; ++ii) {

            BMSYValues.clear();
            MSYValues.clear();
            FMSYValues.clear();
            ChartBMSYData.clear();

            if ((ii == 0) && isMSYBoxChecked()) {
                LineStyle = "DashedLine";
                pct = 1.0;
            } else if ((ii == 1)  && isEnablePctMSYBoxChecked()) {
                LineStyle = "DottedLine";
                pct = MModePctMSYDL->value()/100.0;
            } else {
                continue;
            }

            //        for (int line=0; line<NumRunsPerForecast; ++line) {
            for (int j=0; j<NumSpecies; ++j) {
                if (TableName == "OutputMSYBiomass") {
                    BMSYValues.append(pct*std::stod(dataMap["Value"][j+line*NumSpecies]));
                } else if (TableName == "OutputMSY") {
                    MSYValues.append(pct*std::stod(dataMap["Value"][j+line*NumSpecies]));
                } else if (TableName == "OutputMSYFishing") {
                    FMSYValues.append(pct*std::stod(dataMap["Value"][j+line*NumSpecies]));
                }
            }
            //        }

            // Draw the BMSY line
            for (int i=0; i<NumSpecies; ++i) {
                if (i == SpeciesNum) {
                    for (int j=0; j<=NumYearsPerRun; ++j) {
                        ChartBMSYData(j,0) = BMSYValues[i+line*NumSpecies]/ScaleVal;
                    }
                    break;
                }
            }
            m_MSYLineChart->populateChart(
                        m_ChartWidget,
                        ChartType,
                        LineStyle,
                        nmfConstantsMSSPM::ShowFirstPoint,
                        StartForecastYear,
                        nmfConstantsMSSPM::LabelXAxisAsInts,
                        YMinSliderVal,
                        ChartBMSYData,
                        RowLabelsForBars,
                        ColumnLabelsForLegend,
                        MainTitle,
                        XLabel,
                        YLabel,
                        GridLines,
                        Theme,
                        LineColors[0],
                        lineColorName,
                        1.0);


        }

    }

}
