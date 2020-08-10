#include "MSSPM_GuiManagerMode.h"
#include "nmfChartMovableLine.h"

MSSPM_GuiManagerMode::MSSPM_GuiManagerMode(nmfDatabase* databasePtr, nmfLogger* logger, QWidget* MModeWidget)
{
    nmfChartMovableLine* movableLineChart = new nmfChartMovableLine();

    m_DatabasePtr = databasePtr;
    m_Logger = logger;

    MModeYearsPerRunHS      = MModeWidget->findChild<QSlider*>("MModeYearsPerRunHS");
    MModeRunsPerForeHS      = MModeWidget->findChild<QSlider*>("MModeRunsPerForeHS");
    MModeYearsPerRunLE      = MModeWidget->findChild<QLineEdit*>("MModeYearsPerRunLE");
    MModeRunsPerForeLE      = MModeWidget->findChild<QLineEdit*>("MModeRunsPerForeLE");
    MModePercMSYLE          = MModeWidget->findChild<QLineEdit*>("MModePercMSYLE");
    MModeRParamLE           = MModeWidget->findChild<QLineEdit*>("MModeRParamLE");
    MModeKParamLE           = MModeWidget->findChild<QLineEdit*>("MModeKParamLE");
    MModeCParamLE           = MModeWidget->findChild<QLineEdit*>("MModeCParamLE");
    MModePercMSYDL          = MModeWidget->findChild<QDial*>("MModePercMSYDL");
    MModeRParamDL           = MModeWidget->findChild<QDial*>("MModeRParamDL");
    MModeKParamDL           = MModeWidget->findChild<QDial*>("MModeKParamDL");
    MModeCParamDL           = MModeWidget->findChild<QDial*>("MModeCParamDL");
    MModeSpeciesCMB         = MModeWidget->findChild<QComboBox*>("MModeSpeciesCMB");
    MModeHarvestChartWidget = MModeWidget->findChild<QWidget*>("MModeHarvestChartWidget");
    MModeWindowWidget       = MModeWidget->findChild<QWidget*>("MModeWindowWidget");
    MModeForecastRunPB      = MModeWidget->findChild<QPushButton*>("MModeForecastRunPB");

    movableLineChart->populateChart(MModeHarvestChartWidget, MModeWindowWidget);

    //Defaulting the dials to their lowest values
    MModePercMSYDL->setValue(0);
    MModeRParamDL->setValue(0);
    MModeKParamDL->setValue(0);
    MModeCParamDL->setValue(0);

    //Defaulting the sliders to their lowest values
    MModeYearsPerRunHS->setValue(1);
    MModeRunsPerForeHS->setValue(1);

    //Setting the paired line edits to the lowest values of their respective widgets
    MModeYearsPerRunLE->setText(QString::number(1));
    MModeRunsPerForeLE->setText(QString::number(1));
    MModePercMSYLE->setText(QString::number(0));
    MModeRParamLE->setText(QString::number(0));
    MModeKParamLE->setText(QString::number(0));
    MModeCParamLE->setText(QString::number(0));


    connect(this,                SIGNAL(KeyPressed(QKeyEvent*)),
            movableLineChart,    SLOT(callback_keyPressed(QKeyEvent*)));
    connect(this,                SIGNAL(MouseMoved(QMouseEvent*)),
            movableLineChart,    SLOT(callback_mouseMoved(QMouseEvent*)));
}

MSSPM_GuiManagerMode::~MSSPM_GuiManagerMode()
{

}

void
MSSPM_GuiManagerMode::setupConnections()
{
    std::cout << "Setup Connections begins." << std::endl;

    connect(MModeYearsPerRunHS, SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_YearsPerRun(int)));
    connect(MModeRunsPerForeHS, SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_RunsPerFore(int)));
    connect(MModePercMSYDL,     SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_PercMSY(int)));
    connect(MModeRParamDL,      SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_RParam(int)));
    connect(MModeKParamDL,      SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_KParam(int)));
    connect(MModeCParamDL,      SIGNAL(sliderMoved(int)),
            this,               SLOT(callback_CParam(int)));
    connect(MModeForecastRunPB, SIGNAL(clicked()),
            this,               SLOT(callback_ForecastRun()));
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
    MModeRunsPerForeLE->setText(QString::number(value));
}

void
MSSPM_GuiManagerMode::callback_PercMSY(int value)
{
    MModePercMSYLE->setText(QString::number(value / 100.0));
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
MSSPM_GuiManagerMode::callback_ForecastRun()
{
    std::cout << "ForecastRun works." << std::endl;
    saveUncertaintyParameters();
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

    std::string Algorithm = dataMap["Algorithm"][0];
    std::string Minimizer = dataMap["Minimizer"][0];
    std::string ObjectiveCriterion = dataMap["ObjectiveCriterion"][0];
    std::string Scaling = dataMap["Scaling"][0];
    std::string GrowthRate = MModeRParamLE->text().toStdString();
    std::string CarryingCapacity = MModeKParamLE->text().toStdString();
    std::string Harvest = MModeCParamLE->text().toStdString();

    // Clear previous entry in ForecastUncertainty table
    cmd = "DELETE FROM ForecastUncertainty WHERE ForecastName = '" + ForecastName + "'";
    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: DELETE error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(MModeWindowWidget, "Error",
                             "\nError in Save command.  Couldn't delete all records from ForecastUncertainty table.\n",
                             QMessageBox::Ok);
        QApplication::setOverrideCursor(Qt::WaitCursor);
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
            cmd += "," + Harvest + "),";

            //Ask about the number of columns in the parameter count
            //ERROR: nmfForecast_Tab3::callback_SavePB: Write table error: Column count doesn't match value count at row 1 QMYSQL: Unable to execute query
    }
    cmd = cmd.substr(0,cmd.size()-1);

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfForecast_Tab3::callback_SavePB: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        QMessageBox::warning(MModeWindowWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        return;
    }
}
