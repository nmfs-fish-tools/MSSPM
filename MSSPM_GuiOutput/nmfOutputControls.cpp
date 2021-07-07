#include "nmfOutputControls.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

MSSPM_GuiOutputControls::MSSPM_GuiOutputControls(
        QGroupBox*   controlsGroupBox,
        nmfLogger*   logger,
        nmfDatabase* databasePtr,
        std::string& projectDir)
{
    m_Logger              = logger;
    m_DatabasePtr         = databasePtr;
    m_ProjectDir          = projectDir;
    m_SpeciesOrGuildModel = new QStringListModel();
    m_IsAveraged          = nmfConstantsMSSPM::IsNotAveraged;
    m_ProjectName.clear();
    m_ModelName.clear();
    m_SortedForecastLabelsMap.clear();

    OutputLineBrightnessSL   = nullptr;
    OutputParametersZScoreCB = nullptr;
    ControlsGroupBox = controlsGroupBox;

    readSettings();

    initWidgets();
    initConnections();
    loadWidgets();
}

MSSPM_GuiOutputControls::~MSSPM_GuiOutputControls()
{

}

void
MSSPM_GuiOutputControls::disableControls()
{
    ControlsGroupBox->setEnabled(false);
}

void
MSSPM_GuiOutputControls::enableControls()
{
    ControlsGroupBox->setEnabled(true);
}

bool
MSSPM_GuiOutputControls::isAveraged()
{
    return m_IsAveraged;
}

void
MSSPM_GuiOutputControls::setAveraged(bool isAveraged)
{
    m_IsAveraged = isAveraged;
}

void
MSSPM_GuiOutputControls::initWidgets()
{
    QModelIndex index;
    QVariant vNull(0);
    QString msg;

    QVBoxLayout* controlLayt = new QVBoxLayout();
    QHBoxLayout* BMSYLayt    = new QHBoxLayout();
    QHBoxLayout* MSYLayt     = new QHBoxLayout();
    QHBoxLayout* FMSYLayt    = new QHBoxLayout();
    QHBoxLayout* ParametersLayt = new QHBoxLayout();
    QHBoxLayout* ScaleLayt    = new QHBoxLayout();
    QHBoxLayout* ShadowLayt   = new QHBoxLayout();
    QHBoxLayout* MinMaxLayt   = new QHBoxLayout();
    QWidget*     scrollAreaW  = new QWidget();
    QScrollArea* scrollAreaSA = new QScrollArea();
    QVBoxLayout* controlScrollableLayt = new QVBoxLayout;
    OutputChartTypeLBL      = new QLabel("Chart Type:");
    OutputGroupTypeCMB      = new QComboBox();
    OutputGroupTypeCMB->addItems({"Species:","Guild:","System"});
    OutputSpeciesLBL        = new QLabel("Species:");
    OutputSpeListLBL        = new QLabel("Species:");
    OutputMethodsLBL        = new QLabel("Methods:");
    OutputParametersLBL     = new QLabel("Parameters:");
    OutputScenariosLBL      = new QLabel("Scenarios:");
    OutputYAxisMinLBL       = new QLabel("YAxis Min:");
    OutputYAxisMaxLBL       = new QLabel("YAxis Max:");
    OutputScaleLBL          = new QLabel("Scale Factor:");
    OutputLineBrightnessLBL = new QLabel("Forecast Run Brightness:");
    OutputLineBrightnessSL  = new QSlider(Qt::Horizontal);
    OutputLineBrightnessLBL->setToolTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessLBL->setStatusTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessSL->setToolTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessSL->setStatusTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessLBL->setEnabled(false);
    OutputLineBrightnessSL->setEnabled(false);
    OutputChartTypeCMB   = new QComboBox();
    OutputSpeciesCMB     = new QComboBox();
    OutputParametersCMB  = new QComboBox();
    OutputMethodsCMB     = new QComboBox();
    OutputScenariosCMB   = new QComboBox();
    OutputParametersZScoreCB  = new QCheckBox();
    OutputParametersCenterPB  = new QPushButton();
    OutputParametersMinimumPB = new QPushButton();
    OutputScaleCMB       = new QComboBox();
    OutputYAxisMinSL     = new QSlider(Qt::Horizontal);
    OutputYAxisMinSB     = new QSpinBox();
    OutputYAxisMaxSB     = new QSpinBox();
    OutputSpeListLV      = new QListView();
    OutputShowBMSYCB     = new QCheckBox("B MSY:");
    OutputShowBMSYLE     = new QLineEdit();
    OutputShowMSYCB      = new QCheckBox("MSY:");
    OutputShowMSYLE      = new QLineEdit();
    OutputShowFMSYCB     = new QCheckBox("F MSY:");
    OutputShowFMSYLE     = new QLineEdit();
    OutputShowShadowCB   = new QCheckBox();
    OutputShowShadowLBL  = new QLabel("3d Surface Shadow: ");
    OutputParameters2d3dPB = new QPushButton("3d");
    OutputShowBMSYCB->setMinimumWidth(120);
    OutputShowMSYCB->setMinimumWidth(120);
    OutputShowFMSYCB->setMinimumWidth(120);
    BMSYLayt->addWidget(OutputShowBMSYCB);
    BMSYLayt->addWidget(OutputShowBMSYLE);
    MSYLayt->addWidget(OutputShowMSYCB);
    MSYLayt->addWidget(OutputShowMSYLE);
    FMSYLayt->addWidget(OutputShowFMSYCB);
    FMSYLayt->addWidget(OutputShowFMSYLE);
    OutputParametersCMB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ParametersLayt->addWidget(OutputParametersCMB);
    ParametersLayt->addWidget(OutputParameters2d3dPB);
    ParametersLayt->addWidget(OutputParametersZScoreCB);
    ParametersLayt->addWidget(OutputParametersCenterPB);
    ParametersLayt->addWidget(OutputParametersMinimumPB);
//  MinMaxLayt->addWidget(OutputYAxisMinSB);
    MinMaxLayt->addWidget(OutputYAxisMaxSB);
    controlLayt->addWidget(OutputChartTypeLBL);
    controlLayt->addWidget(OutputChartTypeCMB);
    controlLayt->addWidget(OutputGroupTypeCMB);
    controlLayt->addWidget(OutputSpeciesCMB);
    controlLayt->addWidget(OutputMethodsLBL);
    controlLayt->addWidget(OutputMethodsCMB);
    controlLayt->addWidget(OutputParametersLBL);
    controlLayt->addLayout(ParametersLayt);
    controlLayt->addWidget(OutputScenariosLBL);
    controlLayt->addWidget(OutputScenariosCMB);
    controlLayt->addLayout(BMSYLayt);
    controlLayt->addLayout(MSYLayt);
    controlLayt->addLayout(FMSYLayt);
//  controlLayt->addWidget(OutputSpeListLBL);
//  controlLayt->addWidget(OutputSpeListLV);
    controlLayt->addSpacerItem(new QSpacerItem(1,2,QSizePolicy::Fixed,QSizePolicy::Expanding));
    controlLayt->addWidget(OutputLineBrightnessLBL);
    controlLayt->addWidget(OutputLineBrightnessSL);
    controlLayt->addWidget(OutputYAxisMinLBL);
    controlLayt->addWidget(OutputYAxisMinSL);
    controlLayt->addWidget(OutputYAxisMaxLBL);
    controlLayt->addLayout(MinMaxLayt);
    ScaleLayt->addWidget(OutputScaleLBL);
    ScaleLayt->addWidget(OutputScaleCMB);
    ScaleLayt->addStretch();
    controlLayt->addLayout(ScaleLayt);
    ShadowLayt->addWidget(OutputShowShadowLBL);
    ShadowLayt->addWidget(OutputShowShadowCB);
    ShadowLayt->addStretch();
    controlLayt->addLayout(ShadowLayt);
    scrollAreaSA->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollAreaSA->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollAreaW->setLayout(controlLayt);
    scrollAreaSA->setWidget(scrollAreaW);
    controlScrollableLayt->addWidget(scrollAreaSA);
    scrollAreaSA->setObjectName("ScrollAreaSA");
    ControlsGroupBox->setLayout(controlScrollableLayt);
    scrollAreaSA->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    scrollAreaSA->setMinimumWidth(315);
    scrollAreaSA->setMaximumWidth(315);
    ControlsGroupBox->setMinimumWidth(350);
    ControlsGroupBox->setMaximumWidth(350);

    OutputSpeListLBL->setEnabled(false);
    OutputSpeListLV->setEnabled(false);
    OutputSpeListLV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    OutputGroupTypeCMB->setToolTip("Type of Entity to plot (Species, Guild, or entire System)");
    OutputGroupTypeCMB->setStatusTip("Type of Entity to plot (Species, Guild, or entire System)");
    OutputShowBMSYCB->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYCB->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    msg ="<html>\
<strong><center>B MSY</center></strong><br>\
If K=0 (which will happen if the growth rate selected is Linear), \
the equation used for K becomes K(i) = r(i)/alpha(i,i).\
</html>";
    OutputShowBMSYCB->setWhatsThis(msg);
    OutputShowBMSYLE->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setWhatsThis(msg);
    msg ="<html>\
<strong><center>MSY</center></strong><br>\
If K=0 (which will happen if the growth rate selected is Linear), \
the equation used for K becomes K(i) = r(i)/alpha(i,i).\
</html>";
    OutputShowMSYCB->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYCB->setStatusTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYCB->setWhatsThis(msg);
    OutputShowMSYLE->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setStatusTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setWhatsThis(msg);
    OutputShowFMSYCB->setToolTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYCB->setStatusTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYLE->setToolTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYLE->setStatusTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputYAxisMinLBL->setToolTip("Set the min value of the y-axis");
    OutputYAxisMinLBL->setStatusTip("Set the min value of the y-axis");
    OutputYAxisMinSL->setToolTip("Set the min value of the y-axis");
    OutputYAxisMinSL->setStatusTip("Set the min value of the y-axis");
    OutputYAxisMaxLBL->setToolTip("Set the max value of the y-axis");
    OutputYAxisMaxLBL->setStatusTip("Set the max value of the y-axis");
    OutputYAxisMaxSB->setToolTip("Set the max value of the y-axis");
    OutputYAxisMaxSB->setStatusTip("Set the max value of the y-axis");
    OutputScenariosLBL->setToolTip("These are the available Forecast Scenarios");
    OutputScenariosLBL->setStatusTip("These are the available Forecast Scenarios");
    OutputScenariosCMB->setToolTip("These are the available Multi-Forecast Scenarios");
    OutputScenariosCMB->setStatusTip("These are the available Multi-Forecast Scenarios");
    OutputYAxisMinSL->setMaximum(100);
    OutputYAxisMinSL->setValue(OutputYAxisMinSL->maximum());
    OutputYAxisMinSB->setMaximum(999999);
    OutputYAxisMinSB->setValue(0);
    OutputYAxisMaxSB->setMaximum(999999);
    OutputYAxisMaxSB->setValue(OutputYAxisMaxSB->maximum());
    OutputShowBMSYLE->setReadOnly(true);
    OutputShowMSYLE->setReadOnly(true);
    OutputShowFMSYLE->setReadOnly(true);
    OutputShowMSYLE->setEnabled(false);
    OutputShowFMSYLE->setEnabled(false);
    OutputShowBMSYLE->setAlignment(Qt::AlignRight);
    OutputShowMSYLE->setAlignment(Qt::AlignRight);
    OutputShowFMSYLE->setAlignment(Qt::AlignRight);
    OutputShowShadowLBL->setToolTip("Toggles the 3d surface's shadow");
    OutputShowShadowLBL->setStatusTip("Toggles the 3d surface's shadow");
    OutputShowShadowCB->setToolTip("Toggles the 3d surface's shadow");
    OutputShowShadowCB->setStatusTip("Toggles the 3d surface's shadow");

    OutputChartTypeLBL->setToolTip("The type of chart that will be displayed");
    OutputChartTypeLBL->setStatusTip("The type of chart that will be displayed");
    OutputChartTypeCMB->setToolTip("The type of chart that will be displayed");
    OutputChartTypeCMB->setStatusTip("The type of chart that will be displayed");
    msg ="<html>\
<strong><center>Chart Types</center></strong><br>\
The various output chart types are: <br><br>\
<strong>1. Biomass vs Time</strong><br><br> \
This is a plot of calculated (i.e., estimated) biomass vs time. <br><br>\
<strong>2. Harvest vs Time</strong><br><br> \
This plot will show either catch vs time or (catchability * effort * calculated biomass) vs time.<br><br>\
<strong>3. Exploitation Rate</strong><br><br> \
This plot will show (catchability * effort) vs time and will typically \
range from 0 to 1 in the y-axis.<br><br>\
<strong>4. Diagnostics</strong><br><br> \
These plots will show either the parameter estimation diagnostics \
(2d or 3d) or the retrospective analysis (i.e., Mohn's Rho).<br><br>\
<strong>5. Forecasts</strong><br><br>\
This plot will show the Monte Carlo n% variation forecasts overlayed onto the 0% variation forecast.<br><br>\
<strong>6. Multi-Scenario Plots</strong><br><br>\
This plot shows combined user selected forecasts. That is, after the user runs a forecast,<br>\
they can elect to save the forecast into a multi-forecast scenario. In this fashion, they<br>\
can compare several forecasts simultaneously.<br><br>\
</html>";
    OutputChartTypeLBL->setWhatsThis(msg);
    OutputChartTypeCMB->setWhatsThis(msg);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartBiomass);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartHarvest);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartExploitation);
    OutputChartTypeCMB->addItem("(Abs) Bc vs Time");
    OutputChartTypeCMB->addItem("(Rel) Bc vs Time");
    OutputChartTypeCMB->addItem("Catch vs Bc");
    OutputChartTypeCMB->addItem("Diagnostics");
    OutputChartTypeCMB->addItem("Forecast");
    OutputChartTypeCMB->addItem("Multi-Scenario Plots");
    OutputChartTypeCMB->setItemData(0, "Calculated and Observed Biomass vs Time for a Species",    Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(1, "Calculated Biomass vs Time for one or more Species",       Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(2, "Catch vs Calculated Biomass for a Species",                Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(8, "Allows the user to view multiple Forecasts simultaneously",Qt::ToolTipRole);
    // Disable 3 items for now....
    for (int i=3; i<6; ++i) {
        index = OutputChartTypeCMB->model()->index(i, 0);
        OutputChartTypeCMB->model()->setData(index, vNull, Qt::UserRole-1);
    }

    QIcon centerIcon(":/icons/center.png");
    QIcon minimumIcon(":/icons/minimum.png");
    OutputMethodsCMB->addItem("Parameter Profiles");
    OutputMethodsCMB->addItem("Retrospective Analysis");
    OutputParametersLBL->setEnabled(false);
    OutputParametersCMB->setEnabled(false);
    OutputParametersCMB->addItems(nmfConstantsMSSPM::VectorParameterNames);
    OutputParametersZScoreCB->setText("");
    OutputParametersZScoreCB->setToolTip("Toggles between an absolute fitness scale and a zscore fitness scale");
    OutputParametersZScoreCB->setStatusTip("Toggles between an absolute fitness scale and a zscore fitness scale");
    msg ="<html>\
<strong><center>ZScore</center></strong><br>\
The ZScore value is calculated by the following formula: \
<br><center>ZScore = (x - μ)/σ, where</center> \
<br>x = a particular fitness value, \
<br>μ = estimated parameter, and \
<br>σ = sqrt[Σ(x-μ)²/N] where N=number of estimations \
</html>";
    OutputParametersZScoreCB->setWhatsThis(msg);
    OutputParametersCenterPB->setText("");
    OutputParametersCenterPB->setIcon(centerIcon);
    OutputParametersCenterPB->setFixedWidth(20);
    OutputParametersCenterPB->setFixedHeight(20);
    OutputParametersCenterPB->setToolTip("Selects center point on surface\n(i.e., the estimated r and K values)");
    OutputParametersCenterPB->setStatusTip("Selects center point on surface\n(i.e., the estimated r and K values)");
    OutputParametersCenterPB->setEnabled(false);
    OutputParametersMinimumPB->setText("");
    OutputParametersMinimumPB->setIcon(minimumIcon);
    OutputParametersMinimumPB->setFixedWidth(20);
    OutputParametersMinimumPB->setFixedHeight(20);
    OutputParametersMinimumPB->setToolTip("Selects minimum point on surface\n(i.e., the estimated r and K values)");
    OutputParametersMinimumPB->setStatusTip("Selects minimum point on surface\n(i.e., the estimated r and K values)");
    OutputParametersMinimumPB->setEnabled(false);
    OutputParameters2d3dPB->setFixedWidth(20);
    OutputParameters2d3dPB->setFixedHeight(20);
    OutputParameters2d3dPB->setEnabled(false);
    OutputParameters2d3dPB->setToolTip("Displays a 1-parameter 2d chart or 2-parameter 3d chart.");
    OutputParameters2d3dPB->setStatusTip("Displays a 1-parameter 2d chart or 2-parameter 3d chart.");
    OutputScenariosLBL->setEnabled(false);
    OutputScenariosCMB->setEnabled(false);
    OutputMethodsCMB->setEnabled(false);
    OutputMethodsLBL->setEnabled(false);
    OutputSpeciesCMB->setToolTip("The species reflected in the current chart");
    OutputSpeciesCMB->setStatusTip("The species reflected in the current chart");
    OutputSpeciesLBL->setToolTip("The species reflected in the current chart");
    OutputSpeciesLBL->setStatusTip("The species reflected in the current chart");
    OutputMethodsCMB->setToolTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsCMB->setStatusTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsLBL->setToolTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsLBL->setStatusTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputParametersCMB->setToolTip("Allows user to select which Parameter to view graphically");
    OutputParametersCMB->setStatusTip("Allows user to select which Parameter to view graphically");
    OutputParametersLBL->setToolTip("Allows user to select which Parameter to view graphically");
    OutputParametersLBL->setStatusTip("Allows user to select which Parameter to view graphically");
    OutputScenariosLBL->setToolTip("Allows user to select which Scenario to view");
    OutputScenariosLBL->setStatusTip("Allows user to select which Scenario to view");
    OutputScenariosCMB->setToolTip("Allows user to select which Scenario to view");
    OutputScenariosCMB->setStatusTip("Allows user to select which Scenario to view");
    OutputScaleCMB->addItem("Default");
    OutputScaleCMB->addItem("000");
    OutputScaleCMB->addItem("000 000");
    OutputScaleCMB->addItem("000 000 000");
    OutputScaleLBL->setToolTip("Sets the scale of the y-axis");
    OutputScaleLBL->setStatusTip("Sets the scale of the y-axis");
    OutputScaleCMB->setToolTip("Sets the scale of the y-axis");
    OutputScaleCMB->setStatusTip("Sets the scale of the y-axis");

    loadSpeciesControlWidget();
    callback_LoadScenariosWidget();

    readSettings();

    OutputShowMSYCB->setEnabled( false);
    OutputShowFMSYCB->setEnabled(false);
    OutputYAxisMaxLBL->setEnabled(false);
    OutputYAxisMinSB->setEnabled(false);
    OutputYAxisMaxSB->setEnabled(false);
    OutputShowShadowCB->setEnabled(false);
    OutputShowShadowCB->setChecked(true);
    OutputShowShadowLBL->setEnabled(false);
    OutputParametersZScoreCB->setEnabled(false);

    ControlsGroupBox->setMinimumHeight(100);
}

bool
MSSPM_GuiOutputControls::isShadowShown()
{
    return OutputShowShadowCB->isChecked();
}

void
MSSPM_GuiOutputControls::setCurrentSpecies(QString species)
{
    OutputSpeciesCMB->setCurrentText(species);
}

void
MSSPM_GuiOutputControls::initConnections()
{
    connect(OutputChartTypeCMB,     SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputChartTypeCMB(QString)));
    connect(OutputGroupTypeCMB,     SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputGroupTypeCMB(QString)));
    connect(OutputSpeciesCMB,       SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputSpeciesCMB(QString)));
    connect(OutputMethodsCMB,       SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputMethodsCMB(QString)));
    connect(OutputParametersCMB,    SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputParametersCMB(QString)));
    connect(OutputScenariosCMB,     SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputScenariosCMB(QString)));
    connect(OutputParametersZScoreCB, SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputParametersZScoreCB(int)));
    connect(OutputShowBMSYCB,       SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputBMSYCB(int)));
    connect(OutputShowMSYCB,        SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputMSYCB(int)));
    connect(OutputShowFMSYCB,       SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputFMSYCB(int)));
    connect(OutputScaleCMB,         SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputScaleCMB(QString)));
    connect(OutputLineBrightnessSL, SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputLineBrightnessSL(int)));
    connect(OutputParametersCenterPB, SIGNAL(clicked()),
            this,                   SLOT(callback_OutputParametersCenterPB()));
    connect(OutputParametersMinimumPB,SIGNAL(clicked()),
            this,                   SLOT(callback_OutputParametersMinimumPB()));
    connect(OutputYAxisMinSL,       SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputYAxisMinSL(int)));
    connect(OutputYAxisMinSB,       SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputYAxisMinSB(int)));
    connect(OutputYAxisMaxSB,       SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputYAxisMaxSB(int)));
    connect(OutputShowShadowCB,     SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputShowShadowCB(int)));
    connect(OutputParameters2d3dPB, SIGNAL(clicked()),
            this,                   SLOT(callback_OutputParameters2d3dPB()));
}

void
MSSPM_GuiOutputControls::callback_LoadScenariosWidget()
{
    int currentIndex = OutputScenariosCMB->currentIndex();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"ScenarioName"};
    queryStr = "SELECT DISTINCT ScenarioName from ForecastBiomassMultiScenario ORDER BY ScenarioName";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);

    OutputScenariosCMB->blockSignals(true);
    OutputScenariosCMB->clear();
    for (unsigned scenario=0; scenario<dataMap["ScenarioName"].size(); ++scenario) {
       OutputScenariosCMB->addItem(QString::fromStdString(dataMap["ScenarioName"][scenario]));
    }
    OutputScenariosCMB->blockSignals(false);

    // Set the combobox to where it was prior to reloading.
    if ((currentIndex >= 0) &&
        (currentIndex < OutputScenariosCMB->count()))
    {
        OutputScenariosCMB->setCurrentIndex(currentIndex);
    }
}


void
MSSPM_GuiOutputControls::loadWidgets()
{
    loadSortedForecastLabels();
    loadSpeciesControlWidget();
    callback_LoadScenariosWidget();

}


void
MSSPM_GuiOutputControls::loadSortedForecastLabels()
{
    m_DatabasePtr->createScenarioMap(m_SortedForecastLabelsMap);
}

void
MSSPM_GuiOutputControls::loadSpeciesControlWidget()
{
    int NumSpecies;
    int NumGuilds;
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;
    QStringList SpeciesList;
    QStringList GuildList;

    readSettings();

    m_DatabasePtr->getAlgorithmIdentifiers(
                ControlsGroupBox,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (! getGuilds(NumGuilds,GuildList)) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] MSSPM_GuiOutputControls::loadSpeciesControlWidget: No records found in table Guilds, Name = "+m_ModelName);
        return;
    }
    if ((getOutputGroupType() == "Guild") || (CompetitionForm == "AGG-PROD")) {
       NumSpecies  = NumGuilds;
       SpeciesList = GuildList;
    } else {
        if (! getSpecies(NumSpecies,SpeciesList)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] loadSpeciesControlWidget: No records found in table Species, Name = "+m_ModelName);
            return;
        }
    }

    // Load Species controls combo box
    OutputSpeciesCMB->blockSignals(true);
    OutputSpeListLV->blockSignals(true);

    OutputSpeciesCMB->clear();
    m_SpeciesOrGuildModel->setStringList(QStringList({}));
    m_SpeciesHash.clear();
    for (int species=0; species<NumSpecies; ++species) {
        m_SpeciesHash[SpeciesList[species]] = species;
    }
    // Note: using same model for 2 widgets.  Now if we change the model,
    // we won't have to edit the widgets.
    m_SpeciesOrGuildModel->setStringList(SpeciesList);

    OutputSpeListLV->setModel(m_SpeciesOrGuildModel);

    for (QString Species : SpeciesList) {
        OutputSpeciesCMB->addItem(Species);
    }

    OutputSpeciesCMB->blockSignals(false);
    OutputSpeListLV->blockSignals(false);
}

int
MSSPM_GuiOutputControls::getSpeciesNumFromName(QString SpeciesName)
{
    return m_SpeciesHash[SpeciesName];
}


bool
MSSPM_GuiOutputControls::getSpecies(int&         NumSpecies,
                                    QStringList& SpeciesList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    SpeciesList.clear();

    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName from Species ORDER BY SpeName";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] MSSPM_GuiOutputControls::getSpecies: No species found in table Species");
        return false;
    }

    for (int species=0; species<NumSpecies; ++species) {
        SpeciesList << QString::fromStdString(dataMap["SpeName"][species]);
    }

    return true;
}

bool
MSSPM_GuiOutputControls::getGuilds(int&         NumGuilds,
                                   QStringList& GuildList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    GuildList.clear();

    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from Guilds ORDER BY GuildName";
    dataMap   = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumGuilds = dataMap["GuildName"].size();
    if (NumGuilds == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] MSSPM_GuiOutputControls::getGuilds: No guilds found in table Guilds");
        return false;
    }

    for (int guild=0; guild<NumGuilds; ++guild) {
        GuildList << QString::fromStdString(dataMap["GuildName"][guild]);
    }

    return true;
}


void
MSSPM_GuiOutputControls::callback_OutputLineBrightnessSL(int value)
{
    double brightnessFactor = value/(double)OutputLineBrightnessSL->maximum(); // 0 .. 1

    emit ForecastLineBrightnessChanged(brightnessFactor);
}


int
MSSPM_GuiOutputControls::getYMinSliderVal()
{
    return OutputYAxisMinSL->value();
//    return OutputYAxisMinSB->value();
}

// RSK - unused function
//int
//MSSPM_GuiOutputControls::getYMaxSliderVal()
//{
//    return OutputYAxisMaxSB->value();
//}

void
MSSPM_GuiOutputControls::callback_OutputYAxisMinSL(int value)
{
    updateChart();
}

void
MSSPM_GuiOutputControls::callback_OutputYAxisMinSB(int value)
{
    updateChart();
}

void
MSSPM_GuiOutputControls::callback_OutputYAxisMaxSB(int value)
{
    updateChart();
}


void
MSSPM_GuiOutputControls::callback_OutputParametersCenterPB()
{
    emit SelectCenterSurfacePoint();
}

void
MSSPM_GuiOutputControls::callback_OutputParametersMinimumPB()
{
    emit SelectMinimumSurfacePoint();
}


void
MSSPM_GuiOutputControls::enableMSYWidgets(bool state)
{
    OutputShowBMSYCB->setEnabled(state);
    OutputShowBMSYLE->setEnabled(state);
    OutputShowMSYCB->setEnabled( state);
    OutputShowMSYLE->setEnabled( state);
    OutputShowFMSYCB->setEnabled(state);
    OutputShowFMSYLE->setEnabled(state);
}

void
MSSPM_GuiOutputControls::enableBrightnessWidgets(bool state)
{
    OutputLineBrightnessLBL->setEnabled(state);
    OutputLineBrightnessSL->setEnabled(state);
}

int
MSSPM_GuiOutputControls::getNumberSpecies()
{
    return OutputSpeciesCMB->count();
}

void
MSSPM_GuiOutputControls::setSpeciesNum(int speciesNum)
{
    OutputSpeciesCMB->setCurrentIndex(speciesNum);
}


bool
MSSPM_GuiOutputControls::isSetToRetrospectiveAnalysis()
{
    return OutputMethodsCMB->currentText() == "Retrospective Analysis";
}

void
MSSPM_GuiOutputControls::callback_OutputChartTypeCMB(QString outputType)
{
    bool isParameterProfiles = (OutputMethodsCMB->currentText() == "Parameter Profiles");
    bool speciesLVFlag;
    bool dontShowScaleWidgets;
    bool isBiomassVsTime = (outputType == nmfConstantsMSSPM::OutputChartBiomass);
    bool isHarvestVsTime = (outputType == nmfConstantsMSSPM::OutputChartHarvest);
    bool isFishingMortalityVsTime = (outputType == nmfConstantsMSSPM::OutputChartExploitation);
    bool isForecast      = (outputType == "Forecast");
    bool isDiagnostic    = (outputType == "Diagnostics");
    bool isMultiScenario = (outputType == "Multi-Scenario Plots");
    bool is3dSurfaceDisplayed = displaying3dChart();
    QString scenario     = getOutputScenario();
    QStringList emptyList;

    // For any chart type other than Biomass, Harvest, or Fishing Mortality
    // reset the group type back to Species
    if (! isBiomassVsTime && ! isHarvestVsTime && ! isFishingMortalityVsTime) {
        OutputGroupTypeCMB->setCurrentIndex(0);
        OutputGroupTypeCMB->setEnabled(false);
    } else {
        OutputGroupTypeCMB->setEnabled(true);
    }

    emptyList.clear();

    enableMSYWidgets(false);

    OutputLineBrightnessLBL->setEnabled(isForecast);
    OutputLineBrightnessSL->setEnabled(isForecast);
    OutputMethodsLBL->setEnabled(isDiagnostic);
    OutputMethodsCMB->setEnabled(isDiagnostic);
    OutputParametersCMB->setEnabled(isDiagnostic && isParameterProfiles);
    OutputParametersCenterPB->setEnabled(false);
    OutputParametersMinimumPB->setEnabled(false);
    OutputParameters2d3dPB->setEnabled(isDiagnostic);
    if (is3dSurfaceDisplayed) {
        OutputParametersCMB->setEnabled(false); // this should override the previous statement
        OutputParametersCenterPB->setEnabled(true);
        OutputParametersMinimumPB->setEnabled(true);
        OutputParameters2d3dPB->setEnabled(true);
    }
    OutputParametersLBL->setEnabled(isDiagnostic && isParameterProfiles);
    OutputScenariosLBL->setEnabled(isMultiScenario);
    OutputScenariosCMB->setEnabled(isMultiScenario);

    OutputYAxisMinLBL->setEnabled(isBiomassVsTime || isHarvestVsTime);
    OutputYAxisMinSL->setEnabled( isBiomassVsTime || isHarvestVsTime);
    OutputYAxisMaxLBL->setEnabled(isDiagnostic);
    OutputYAxisMinSB->setEnabled(isDiagnostic);
//  OutputYAxisMaxSB->setEnabled(isDiagnostic);
    OutputShowShadowCB->setEnabled(isDiagnostic && is3dSurfaceDisplayed);
    OutputShowShadowLBL->setEnabled(isDiagnostic && is3dSurfaceDisplayed);

    speciesLVFlag = ((outputType == "(Abs) Bc vs Time") ||
                     (outputType == "(Rel) Bc vs Time"));
    dontShowScaleWidgets = (outputType == nmfConstantsMSSPM::OutputChartExploitation) ||
                          ((outputType == "Diagnostics") && isParameterProfiles);
    OutputSpeciesLBL->setEnabled(! speciesLVFlag);
    OutputSpeciesCMB->setEnabled(! speciesLVFlag);
    OutputSpeListLBL->setEnabled(  speciesLVFlag);
    OutputSpeListLV->setEnabled(   speciesLVFlag);
    OutputShowBMSYCB->setEnabled( outputType == nmfConstantsMSSPM::OutputChartBiomass);
    OutputShowBMSYLE->setEnabled( outputType == nmfConstantsMSSPM::OutputChartBiomass);
    OutputShowMSYCB->setEnabled(  outputType == nmfConstantsMSSPM::OutputChartHarvest);
    OutputShowMSYLE->setEnabled(  outputType == nmfConstantsMSSPM::OutputChartHarvest);
    OutputShowFMSYCB->setEnabled( outputType == nmfConstantsMSSPM::OutputChartExploitation);
    OutputShowFMSYLE->setEnabled( outputType == nmfConstantsMSSPM::OutputChartExploitation);
    OutputScaleCMB->setEnabled(! dontShowScaleWidgets) ;
    OutputScaleLBL->setEnabled(! dontShowScaleWidgets) ;
    if (outputType == nmfConstantsMSSPM::OutputChartExploitation) {
        OutputScaleCMB->setCurrentIndex(0);
        OutputScaleCMB->setEnabled(false);
        OutputScaleLBL->setEnabled(false);
    }

    if (isDiagnostic) {
        if (isParameterProfiles) {
            callback_ResetOutputWidgetsForAggProd();
            emit ShowChart("",""); //,m_IsAveraged);
            if (! displaying3dChart()) {
                emit SetChartView2d(true);
            }
        } else { // Mohn's Rho
            emit ShowChartMohnsRho();
        }
    } else if (isForecast) {
        callback_ResetOutputWidgetsForAggProd();
        emit SetChartView2d(true);
        emit ShowChart("",""); //,m_IsAveraged);
    } else if (isMultiScenario) {
        callback_ResetOutputWidgetsForAggProd();
        emit SetChartView2d(true);
        if (scenario.isEmpty()) {
            emit ShowChartMultiScenario(emptyList);
        } else {
            emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
        }
    } else {
        emit SetChartView2d(true);
        emit ResetFilterButtons(); // Reset toolbar buttons and only enable them for nmfConstantsMSSPM::OutputChartBiomass
        emit EnableFilterButtons(outputType == nmfConstantsMSSPM::OutputChartBiomass);
        OutputSpeListLV->clearSelection();
        updateChart();
    }

}

void
MSSPM_GuiOutputControls::callback_OutputGroupTypeCMB(QString outputGroupType)
{
    if (outputGroupType == "Species:" ) {
        loadSpeciesControlWidget();
        emit ShowChart("",""); //,m_IsAveraged);
    } else if (outputGroupType == "Guild:") {
        loadSpeciesControlWidget();
        emit ShowChartBy("Guild",m_IsAveraged);
    } else if (outputGroupType == "System") {
        OutputSpeciesCMB->clear();
        emit ShowChartBy("System",m_IsAveraged);
    }
}

void
MSSPM_GuiOutputControls::refresh()
{
    int currentIndex = OutputChartTypeCMB->currentIndex();

    OutputChartTypeCMB->setCurrentIndex(0);
    OutputChartTypeCMB->setCurrentIndex(currentIndex);

    refreshScenarios();
}

void
MSSPM_GuiOutputControls::refreshScenarios()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"ScenarioName"};
    queryStr   = "SELECT DISTINCT ScenarioName FROM ForecastBiomassMultiScenario";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ScenarioName"].size();

    OutputScenariosCMB->clear();
    for (int i=0; i<NumRecords; ++i) {
        OutputScenariosCMB->addItem(QString::fromStdString(dataMap["ScenarioName"][i]));
    }
}


void
MSSPM_GuiOutputControls::callback_OutputSpeciesCMB(QString outputSpecies)
{
    QString scenario  = getOutputScenario();
    QString chartType = getOutputChartType();
    QString method    = getOutputDiagnostics();
    if (getOutputChartType() != nmfConstantsMSSPM::OutputChartExploitation) {
        OutputScaleLBL->setEnabled(true);
        OutputScaleCMB->setEnabled(true);
    }

    if (chartType == "Multi-Scenario Plots") {
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if ((chartType == "Diagnostics") && (method == "Retrospective Analysis")) {
        emit ShowChartMohnsRho();
    } else {
        updateChart();
    }

}

void
MSSPM_GuiOutputControls::callback_OutputMethodsCMB(QString method)
{
    OutputParametersCMB->setEnabled(true);
    OutputParametersLBL->setEnabled(true);
    OutputParameters2d3dPB->setEnabled(true);
    OutputParametersZScoreCB->setEnabled(displaying3dChart());
    OutputScaleLBL->setEnabled(true);
    OutputScaleCMB->setEnabled(true);

    if (method == "Parameter Profiles") {
        callback_ResetOutputWidgetsForAggProd();
        emit SetChartView2d(true);
        emit ShowChart("","");
        OutputScaleLBL->setEnabled(false);
        OutputScaleCMB->setEnabled(false);
    } else if (method == "Retrospective Analysis") {
        emit ShowChartMohnsRho();
        OutputParameters2d3dPB->setText("3d");
        OutputParameters2d3dPB->setEnabled(false);
        OutputParametersLBL->setEnabled(false);
        OutputParametersCMB->setEnabled(false);
        OutputParametersZScoreCB->setChecked(false);
        OutputParametersZScoreCB->setEnabled(false);
        OutputParametersMinimumPB->setEnabled(false);
        OutputParametersCenterPB->setEnabled(false);
    }
}

void
MSSPM_GuiOutputControls::setForMohnsRho()
{
    OutputChartTypeCMB->setCurrentText("Diagnostics");
    OutputMethodsCMB->setCurrentText("Retrospective Analysis");
    OutputParametersLBL->setEnabled(false);
    OutputParametersCMB->setEnabled(false);
    OutputParametersZScoreCB->setEnabled(false);
    OutputParameters2d3dPB->setEnabled(false);
    OutputParametersMinimumPB->setEnabled(false);
    OutputParametersCenterPB->setEnabled(false);
}

void
MSSPM_GuiOutputControls::setForBiomassVsTime()
{
  OutputChartTypeCMB->setCurrentText(nmfConstantsMSSPM::OutputChartBiomass);
}

void
MSSPM_GuiOutputControls::displayMohnsRho()
{
    OutputMethodsLBL->setEnabled(true);
    OutputMethodsCMB->setEnabled(true);
    OutputMethodsCMB->blockSignals(true);
    OutputMethodsCMB->setCurrentText("Retrospective Analysis");
    OutputMethodsCMB->blockSignals(false);
    OutputChartTypeCMB->blockSignals(true);
    OutputChartTypeCMB->setCurrentText("Diagnostics");
    OutputChartTypeCMB->blockSignals(false);

    emit ShowChartMohnsRho();
}

void
MSSPM_GuiOutputControls::callback_OutputParametersCMB(QString unused)
{
    emit ShowChart("",""); //,m_IsAveraged);
}

void
MSSPM_GuiOutputControls::callback_OutputScenariosCMB(QString scenario)
{
    std::string msg;

    if (scenario.isEmpty()) {
        return;
    }

    if (m_SortedForecastLabelsMap[scenario].empty()) {
        msg = "Warning: Map has a key (" + scenario.toStdString() + ") but no associated data";
        m_Logger->logMsg(nmfConstants::Warning,msg);
        return;
    }
    emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
}

void
MSSPM_GuiOutputControls::callback_SetOutputScenario(QString scenario)
{
    if (scenario.isEmpty()) {
        return;
    }
    OutputScenariosCMB->setCurrentText(scenario);
}

void
MSSPM_GuiOutputControls::callback_OutputParameters2d3dPB()
{
    QString currentText = OutputParameters2d3dPB->text();
    bool stateIs3d      = (currentText == "3d");

    if (stateIs3d) {
        OutputParameters2d3dPB->setText("2d");
        OutputParametersCMB->setEnabled(false);
        OutputParametersCenterPB->setEnabled(true);
        OutputParametersMinimumPB->setEnabled(true);
        OutputParametersZScoreCB->setEnabled(true);
        emit ShowDiagnosticsChart3d();
        emit SetChartView2d(false);
    } else {
        OutputParameters2d3dPB->setText("3d");
        OutputParametersCMB->setEnabled(true);
        OutputParametersCenterPB->setEnabled(false);
        OutputParametersMinimumPB->setEnabled(false);
        OutputParametersZScoreCB->setEnabled(false);
        emit SetChartView2d(true);
    }
    OutputShowShadowLBL->setEnabled(stateIs3d);
    OutputShowShadowCB->setEnabled(stateIs3d);
}

void
MSSPM_GuiOutputControls::callback_OutputParametersZScoreCB(int state)
{
    emit ShowChart("","");
}

bool
MSSPM_GuiOutputControls::displaying3dChart()
{
    return (OutputParameters2d3dPB->text() == "2d");
}

void
MSSPM_GuiOutputControls::callback_OutputShowShadowCB(int dummy)
{
    emit ShowChart("",""); //,m_IsAveraged);
}

void
MSSPM_GuiOutputControls::updateChart()
{
    QString outputGroupType = getOutputGroupType();

    if (outputGroupType == "Species") {
        emit ShowChart("","");
    } else if (outputGroupType == "Guild") {
        emit ShowChartBy("Guild",m_IsAveraged);
    } else if (outputGroupType == "System") {
        emit ShowChartBy("System",m_IsAveraged);
    }
}

void
MSSPM_GuiOutputControls::callback_OutputBMSYCB(int val)
{
    updateChart();
}

void
MSSPM_GuiOutputControls::callback_OutputMSYCB(int val)
{
    updateChart();
}

void
MSSPM_GuiOutputControls::callback_OutputFMSYCB(int val)
{
    updateChart();
}

void
MSSPM_GuiOutputControls::callback_OutputScaleCMB(QString scale)
{
    QString scenario = getOutputScenario();

    if (getOutputChartType() == "Multi-Scenario Plots") {
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if (isSetToRetrospectiveAnalysis()) {
        emit ShowChartMohnsRho();
    } else {
        updateChart();
    }
}

void
MSSPM_GuiOutputControls::callback_ResetOutputWidgetsForAggProd()
{
    int NumSpecies;
    int NumGuilds;
    QStringList SpeciesList;
    QStringList GuildList;
    QStringList SpeciesOrGuildList;

    if (! getSpecies(NumSpecies,SpeciesList)) {
        return;
    }
    if (! getGuilds(NumGuilds,GuildList)) {
        return;
    }

    if (isAggProd()) {
        OutputSpeciesLBL->setText("Guilds:");
        SpeciesOrGuildList = GuildList;
    } else {
        OutputSpeciesLBL->setText("Species");
        SpeciesOrGuildList = SpeciesList;
    }

    OutputSpeciesCMB->blockSignals(true);
    OutputSpeciesCMB->clear();
    for (QString Species : SpeciesOrGuildList) {
        OutputSpeciesCMB->addItem(Species);
    }
    OutputSpeciesCMB->blockSignals(false);
}

bool
MSSPM_GuiOutputControls::isAggProd()
{
    std::string Algorithm;
    std::string Minimizer;
    std::string ObjectiveCriterion;
    std::string Scaling;
    std::string CompetitionForm;

    m_DatabasePtr->getAlgorithmIdentifiers(
                ControlsGroupBox,m_Logger,m_ProjectName,m_ModelName,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    return (CompetitionForm == "AGG-PROD");
}

void
MSSPM_GuiOutputControls::clearOutputBMSY()
{
    OutputShowBMSYLE->clear();
}

void
MSSPM_GuiOutputControls::clearOutputMSY()
{
    OutputShowMSYLE->clear();
}

void
MSSPM_GuiOutputControls::clearOutputFMSY()
{
    OutputShowFMSYLE->clear();
}

bool
MSSPM_GuiOutputControls::isSurfaceTypeZScore()
{
    return OutputParametersZScoreCB->isChecked();
}

bool
MSSPM_GuiOutputControls::isCheckedOutputBMSY()
{
    bool isChecked;

    OutputShowBMSYCB->blockSignals(true);
    isChecked = OutputShowBMSYCB->isChecked();
    OutputShowBMSYCB->blockSignals(false);

    return isChecked;
}

bool
MSSPM_GuiOutputControls::isCheckedOutputMSY()
{
    return OutputShowMSYCB->isChecked();
}

bool
MSSPM_GuiOutputControls::isEnabledOutputMSY()
{
    return OutputShowMSYCB->isEnabled();
}

bool
MSSPM_GuiOutputControls::isCheckedOutputFMSY()
{
    return OutputShowFMSYCB->isChecked();
}


bool
MSSPM_GuiOutputControls::isEnabledOutputFMSY()
{
    return OutputShowFMSYCB->isEnabled();
}

void
MSSPM_GuiOutputControls::setTextOutputBMSY(QString text)
{
    OutputShowBMSYLE->blockSignals(true);
    OutputShowBMSYLE->setText(text);
    OutputShowBMSYLE->blockSignals(false);
}

void
MSSPM_GuiOutputControls::setTextOutputMSY(QString text)
{
    OutputShowMSYLE->setText(text);
}

void
MSSPM_GuiOutputControls::setTextOutputFMSY(QString text)
{
    OutputShowFMSYLE->setText(text);
}

void
MSSPM_GuiOutputControls::setForecastLabels(
        std::map<QString,QStringList>& SortedForecastLabelsMap)
{
    m_SortedForecastLabelsMap = SortedForecastLabelsMap;

}

QString
MSSPM_GuiOutputControls::getOutputChartType()
{
    return OutputChartTypeCMB->currentText();
}

QString
MSSPM_GuiOutputControls::getOutputGroupType()
{

    return OutputGroupTypeCMB->currentText().replace(":","");
}

QString
MSSPM_GuiOutputControls::getOutputSpecies()
{
    return OutputSpeciesCMB->currentText();
}

QString
MSSPM_GuiOutputControls::getOutputDiagnostics()
{
    return OutputMethodsCMB->currentText();
}

int
MSSPM_GuiOutputControls::getOutputSpeciesIndex()
{
    return OutputSpeciesCMB->currentIndex();
}

QString
MSSPM_GuiOutputControls::getOutputScenario()
{
    return OutputScenariosCMB->currentText();
}

void
MSSPM_GuiOutputControls::setOutputParameters2d3dPB(QString chartType)
{
    OutputParameters2d3dPB->setText(chartType);
    callback_OutputParameters2d3dPB();
}

void
MSSPM_GuiOutputControls::setOutputSpecies(QString species)
{
    OutputSpeciesCMB->setCurrentText(species);
}

void
MSSPM_GuiOutputControls::setOutputSpeciesIndex(int index)
{
    OutputSpeciesCMB->setCurrentIndex(index);
}

void
MSSPM_GuiOutputControls::setOutputType(QString type)
{
    // This forces an update if the user needs to
    // refresh the same setting.
    OutputChartTypeCMB->setCurrentIndex(0);
    OutputChartTypeCMB->setCurrentText(type);

    return;
}

void
MSSPM_GuiOutputControls::setOutputGroupType(QString groupType)
{
    OutputGroupTypeCMB->setCurrentText(groupType);
}

void
MSSPM_GuiOutputControls::setOutputDiagnostics(QString method)
{
    // This forces an update if the user needs to
    // refresh the same setting.
    OutputMethodsCMB->setCurrentIndex(0);
    OutputMethodsCMB->setCurrentText(method);
}



QString
MSSPM_GuiOutputControls::getOutputParameter()
{
    return OutputParametersCMB->currentText();
}

QString
MSSPM_GuiOutputControls::getOutputScale()
{
    return OutputScaleCMB->currentText();
}

double
MSSPM_GuiOutputControls::getOutputBrightnessFactor()
{
    return OutputLineBrightnessSL->value()/(double)OutputLineBrightnessSL->maximum();
}

QWidget*
MSSPM_GuiOutputControls::getListViewViewport()
{
    return OutputSpeListLV->viewport();
}

QModelIndexList
MSSPM_GuiOutputControls::getListViewSelectedIndexes()
{
    return OutputSpeListLV->selectionModel()->selectedIndexes();
}

void
MSSPM_GuiOutputControls::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Output");
    if (OutputLineBrightnessSL != nullptr) {
        OutputLineBrightnessSL->setValue(settings->value("Brightness",33).toString().toInt());
    }
    settings->endGroup();

    delete settings;
}

void
MSSPM_GuiOutputControls::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Output");
    settings->setValue("Brightness", OutputLineBrightnessSL->value());
    settings->endGroup();

    delete settings;
}

void
MSSPM_GuiOutputControls::callback_UpdateDiagnosticParameterChoices()
{
    m_DatabasePtr->loadEstimatedVectorParameters(m_Logger,
                                                 m_ProjectName,
                                                 m_ModelName,
                                                 OutputParametersCMB);
}
