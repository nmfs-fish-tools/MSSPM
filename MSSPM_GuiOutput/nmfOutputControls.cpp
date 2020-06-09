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
    m_SpeciesOrGuildModel = new QStringListModel(this);
    m_ProjectSettingsConfig.clear();
    m_SortedForecastLabelsMap.clear();

    OutputLineBrightnessSL = nullptr;
    OutputParametersCB     = nullptr;
    ControlsGroupBox       = controlsGroupBox;

    readSettings();

    initWidgets();
    initConnections();
    loadWidgets();
}

MSSPM_GuiOutputControls::~MSSPM_GuiOutputControls()
{

}

void
MSSPM_GuiOutputControls::initWidgets()
{
    QModelIndex index;
    QVariant vNull(0);

    QVBoxLayout* controlLayt = new QVBoxLayout();
    QHBoxLayout* BMSYLayt    = new QHBoxLayout();
    QHBoxLayout* MSYLayt     = new QHBoxLayout();
    QHBoxLayout* FMSYLayt    = new QHBoxLayout();
    QHBoxLayout* ParametersLayt = new QHBoxLayout();
    QHBoxLayout* ScaleLayt  = new QHBoxLayout();
    QHBoxLayout* ShadowLayt = new QHBoxLayout();
    OutputTypeLBL           = new QLabel("Chart Type:");
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
    OutputTypeCMB        = new QComboBox();
    OutputSpeciesCMB     = new QComboBox();
    OutputParametersCMB  = new QComboBox();
    OutputMethodsCMB     = new QComboBox();
    OutputScenariosCMB   = new QComboBox();
    OutputParametersCB   = new QCheckBox();
    OutputParametersCenterPB  = new QPushButton();
    OutputParametersMinimumPB = new QPushButton();
    OutputScaleCMB       = new QComboBox();
    OutputYAxisMinSL     = new QSlider(Qt::Horizontal);
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
    ParametersLayt->addWidget(OutputParametersCenterPB);
    ParametersLayt->addWidget(OutputParametersMinimumPB);
    ParametersLayt->addWidget(OutputParametersCB);
    controlLayt->addWidget(OutputTypeLBL);
    controlLayt->addWidget(OutputTypeCMB);
    controlLayt->addWidget(OutputSpeciesLBL);
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
    controlLayt->addWidget(OutputSpeListLBL);
    controlLayt->addWidget(OutputSpeListLV);
    controlLayt->addSpacerItem(new QSpacerItem(1,2,QSizePolicy::Fixed,QSizePolicy::Expanding));
    controlLayt->addWidget(OutputLineBrightnessLBL);
    controlLayt->addWidget(OutputLineBrightnessSL);
    controlLayt->addWidget(OutputYAxisMinLBL);
    controlLayt->addWidget(OutputYAxisMinSL);
    controlLayt->addWidget(OutputYAxisMaxLBL);
    controlLayt->addWidget(OutputYAxisMaxSB);
    ScaleLayt->addWidget(OutputScaleLBL);
    ScaleLayt->addWidget(OutputScaleCMB);
    ScaleLayt->addStretch();
    controlLayt->addLayout(ScaleLayt);
    ShadowLayt->addWidget(OutputShowShadowLBL);
    ShadowLayt->addWidget(OutputShowShadowCB);
    ShadowLayt->addStretch();
    controlLayt->addLayout(ShadowLayt);
    ControlsGroupBox->setLayout(controlLayt);
    OutputSpeListLBL->setEnabled(false);
    OutputSpeListLV->setEnabled(false);
    OutputSpeListLV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    OutputShowBMSYCB->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYCB->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowMSYCB->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYCB->setStatusTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setStatusTip("Maximum Sustained Yield (rK/4)");
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
    OutputYAxisMaxSB->setMaximum(99999);
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

    OutputTypeLBL->setToolTip("The type of chart that will be displayed");
    OutputTypeLBL->setStatusTip("The type of chart that will be displayed");
    OutputTypeCMB->setToolTip("The type of chart that will be displayed");
    OutputTypeCMB->setStatusTip("The type of chart that will be displayed");
    OutputTypeCMB->addItem("Bc & Bo vs Time");
    OutputTypeCMB->addItem("Harvest vs Time");
    OutputTypeCMB->addItem("Fishing Mortality vs Time");
    OutputTypeCMB->addItem("(Abs) Bc vs Time");
    OutputTypeCMB->addItem("(Rel) Bc vs Time");
    OutputTypeCMB->addItem("Catch vs Bc");
    OutputTypeCMB->addItem("Diagnostics");
    OutputTypeCMB->addItem("Forecast");
    OutputTypeCMB->addItem("Multi-Scenario Plots");
    OutputTypeCMB->setItemData(0, "Calculated and Observed Biomass vs Time for a Species",    Qt::ToolTipRole);
    OutputTypeCMB->setItemData(1, "Calculated Biomass vs Time for one or more Species",       Qt::ToolTipRole);
    OutputTypeCMB->setItemData(2, "Catch vs Calculated Biomass for a Species",                Qt::ToolTipRole);
    OutputTypeCMB->setItemData(8, "Allows the user to view multiple Forecasts simultaneously",Qt::ToolTipRole);
    // Disable 3 items for now....
    for (int i=3; i<6; ++i) {
        index = OutputTypeCMB->model()->index(i, 0);
        OutputTypeCMB->model()->setData(index, vNull, Qt::UserRole-1);
    }

    QIcon centerIcon(":/icons/center.png");
    QIcon minimumIcon(":/icons/minimum.png");
    OutputMethodsCMB->addItem("Parameter Profiles");
    OutputMethodsCMB->addItem("Retrospective Analysis");
    OutputParametersLBL->setEnabled(false);
    OutputParametersCMB->setEnabled(false);
    OutputParametersCMB->addItem("Growth Rate (r)");
    OutputParametersCMB->addItem("Carrying Capacity (K)");
    OutputParametersCB->setText("");
    OutputParametersCB->setToolTip("Toggles between 2d and 3d Diagnostics View");
    OutputParametersCB->setStatusTip("Toggles between 2d and 3d Diagnostics View");
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
    OutputYAxisMaxSB->setEnabled(false);
    OutputShowShadowCB->setEnabled(false);
    OutputShowShadowCB->setChecked(true);
    OutputShowShadowLBL->setEnabled(false);

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
    connect(OutputTypeCMB,          SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputTypeCMB(QString)));
    connect(OutputSpeciesCMB,       SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputSpeciesCMB(QString)));
    connect(OutputMethodsCMB,       SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputMethodsCMB(QString)));
    connect(OutputParametersCMB,    SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputParametersCMB(QString)));
    connect(OutputScenariosCMB,     SIGNAL(currentIndexChanged(QString)),
            this,                   SLOT(callback_OutputScenariosCMB(QString)));
    connect(OutputParametersCB,     SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputParametersCB(int)));
    connect(OutputShowBMSYCB,       SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputShowBMSYCB(int)));
    connect(OutputShowMSYCB,        SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputMSYCB(int)));
    connect(OutputShowFMSYCB,       SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputShowFMSYCB(int)));
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
    connect(OutputYAxisMaxSB,       SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputYAxisMaxSB(int)));
    connect(OutputShowShadowCB,     SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputShowShadowCB(int)));
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
                ControlsGroupBox,m_Logger,m_ProjectSettingsConfig,
                Algorithm,Minimizer,ObjectiveCriterion,Scaling,
                CompetitionForm,nmfConstantsMSSPM::DontShowPopupError);

    if (! getGuilds(NumGuilds,GuildList)) {
        m_Logger->logMsg(nmfConstants::Warning,"[Warning] MSSPM_GuiOutputControls::loadSpeciesControlWidget: No records found in table Guilds, Name = "+m_ProjectSettingsConfig);
        return;
    }
    if (CompetitionForm == "AGG-PROD") {
       NumSpecies  = NumGuilds;
       SpeciesList = GuildList;
    } else {
        if (! getSpecies(NumSpecies,SpeciesList)) {
            m_Logger->logMsg(nmfConstants::Error,"[Error 2] loadSpeciesControlWidget: No records found in table Species, Name = "+m_ProjectSettingsConfig);
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
}

int
MSSPM_GuiOutputControls::getYMaxSliderVal()
{
    return OutputYAxisMaxSB->value();
}

void
MSSPM_GuiOutputControls::callback_OutputYAxisMinSL(int value)
{
    emit YAxisMinValueChanged(value);
}

void
MSSPM_GuiOutputControls::callback_OutputYAxisMaxSB(int value)
{
    emit YAxisMaxValueChanged(value);
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


void
MSSPM_GuiOutputControls::callback_OutputTypeCMB(QString outputType)
{
    bool isParameterProfiles = (OutputMethodsCMB->currentText() == "Parameter Profiles");
    bool speciesLVFlag;
    bool isForecast      = (outputType == "Forecast");
    bool isDiagnostic    = (outputType == "Diagnostics");
    bool isMultiScenario = (outputType == "Multi-Scenario Plots");
    bool paramIsChecked  = (OutputParametersCB->checkState() == Qt::Checked);
    bool isBcBoVsTime    = (outputType == "Bc & Bo vs Time");
    bool is3dChecked     = OutputParametersCB->isChecked();
    QString scenario     = getOutputScenario();
    QStringList emptyList;

    emptyList.clear();

    enableMSYWidgets(false);

    OutputLineBrightnessLBL->setEnabled(isForecast);
    OutputLineBrightnessSL->setEnabled(isForecast);
    OutputMethodsLBL->setEnabled(isDiagnostic);
    OutputMethodsCMB->setEnabled(isDiagnostic);
    OutputParametersCMB->setEnabled(isDiagnostic && isParameterProfiles);
    OutputParametersCB->setEnabled(isDiagnostic);
    OutputParametersCenterPB->setEnabled(false);
    OutputParametersMinimumPB->setEnabled(false);
    if (paramIsChecked) {
        OutputParametersCMB->setEnabled(false); // this should override the previous statement
        OutputParametersCB->setEnabled(false);
        OutputParametersCenterPB->setEnabled(true);
        OutputParametersMinimumPB->setEnabled(true);
    }
    OutputParametersCB->setEnabled(isDiagnostic);
    OutputParametersLBL->setEnabled(isDiagnostic && isParameterProfiles);
    OutputScenariosLBL->setEnabled(isMultiScenario);
    OutputScenariosCMB->setEnabled(isMultiScenario);

    OutputYAxisMinLBL->setEnabled(isBcBoVsTime);
    OutputYAxisMinSL->setEnabled(isBcBoVsTime);
    OutputYAxisMaxLBL->setEnabled(isDiagnostic);
    OutputYAxisMaxSB->setEnabled(isDiagnostic);
    OutputShowShadowCB->setEnabled(isDiagnostic && is3dChecked);
    OutputShowShadowLBL->setEnabled(isDiagnostic && is3dChecked);

    if (isDiagnostic) {
        if (isParameterProfiles) {
            callback_ResetOutputWidgetsForAggProd();
            emit ShowChart("","");
            if (OutputParametersCB->isChecked()) {
                emit SetChartView2d(false);
            }
        } else { // Mohn's Rho
            emit ShowChartMohnsRho();
        }
    } else if (isForecast) {

        callback_ResetOutputWidgetsForAggProd();
        emit SetChartView2d(true);
        emit ShowChart("","");
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

        // Reset toolbar buttons and only enable them for "Bc & Bo vs Time"
        emit ResetFilterButtons();
        emit EnableFilterButtons(outputType == "Bc & Bo vs Time");

        OutputSpeListLV->clearSelection();
        emit ShowChart(outputType,"");
    }

    speciesLVFlag = ((outputType == "(Abs) Bc vs Time") ||
                     (outputType == "(Rel) Bc vs Time"));

    bool dontShowScaleWidgets;
    dontShowScaleWidgets = (outputType == "Fishing Mortality vs Time") ||
                          ((outputType == "Diagnostics") && isParameterProfiles);

    OutputSpeciesLBL->setEnabled(! speciesLVFlag);
    OutputSpeciesCMB->setEnabled(! speciesLVFlag);
    OutputSpeListLBL->setEnabled(  speciesLVFlag);
    OutputSpeListLV->setEnabled(   speciesLVFlag);
    OutputShowBMSYCB->setEnabled( outputType == "Bc & Bo vs Time");
    OutputShowBMSYLE->setEnabled( outputType == "Bc & Bo vs Time");
    OutputShowMSYCB->setEnabled(  outputType == "Harvest vs Time");
    OutputShowMSYLE->setEnabled(  outputType == "Harvest vs Time");
    OutputShowFMSYCB->setEnabled( outputType == "Fishing Mortality vs Time");
    OutputShowFMSYLE->setEnabled( outputType == "Fishing Mortality vs Time");
    OutputScaleCMB->setEnabled(! dontShowScaleWidgets) ; //   outputType != "Fishing Mortality vs Time");
    OutputScaleLBL->setEnabled(! dontShowScaleWidgets) ; //   (outputType != "Fishing Mortality vs Time") &&

    if (outputType == "Fishing Mortality vs Time")
        OutputScaleCMB->setCurrentIndex(0);

}

void
MSSPM_GuiOutputControls::refresh()
{
    int currentIndex = OutputTypeCMB->currentIndex();

    OutputTypeCMB->setCurrentIndex(0);
    OutputTypeCMB->setCurrentIndex(currentIndex);

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
    QString chartType = getOutputType();
    QString method    = getOutputDiagnostics();
    OutputScaleLBL->setEnabled(true);
    OutputScaleCMB->setEnabled(true);

    if (chartType == "Multi-Scenario Plots") {
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if ((chartType == "Diagnostics") && (method == "Retrospective Analysis")) {
        emit ShowChartMohnsRho();
    } else {
        emit ShowChart("",outputSpecies);
    }

}

void
MSSPM_GuiOutputControls::callback_OutputMethodsCMB(QString method)
{
    OutputParametersCMB->setEnabled(true);
    OutputParametersLBL->setEnabled(true);
    OutputParametersCB->setEnabled(true);
    OutputScaleLBL->setEnabled(true);
    OutputScaleCMB->setEnabled(true);

    if (method == "Parameter Profiles") {
        callback_ResetOutputWidgetsForAggProd();
        emit ShowChart("","");
        if (OutputParametersCB->isChecked()) {
            emit SetChartView2d(false);
        }
        OutputScaleLBL->setEnabled(false);
        OutputScaleCMB->setEnabled(false);
    } else if (method == "Retrospective Analysis") {
        emit ShowChartMohnsRho();
        OutputParametersCB->setChecked(false);
        OutputParametersLBL->setEnabled(false);
        OutputParametersCMB->setEnabled(false);
        OutputParametersCB->setEnabled(false);
    }

}

void
MSSPM_GuiOutputControls::setForMohnsRho()
{
    OutputParametersLBL->setEnabled(false);
    OutputParametersCMB->setEnabled(false);
}

void
MSSPM_GuiOutputControls::displayMohnsRho()
{
    OutputMethodsLBL->setEnabled(true);
    OutputMethodsCMB->setEnabled(true);
    OutputMethodsCMB->blockSignals(true);
    OutputMethodsCMB->setCurrentText("Retrospective Analysis");
    OutputMethodsCMB->blockSignals(false);
    OutputTypeCMB->blockSignals(true);
    OutputTypeCMB->setCurrentText("Diagnostics");
    OutputTypeCMB->blockSignals(false);

    emit ShowChartMohnsRho();
}

void
MSSPM_GuiOutputControls::callback_OutputParametersCMB(QString unused)
{
    emit ShowChart("","");
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
MSSPM_GuiOutputControls::callback_OutputParametersCB(int state)
{
    if (state == Qt::Checked) {
        OutputParametersCMB->setEnabled(false);
        OutputParametersCenterPB->setEnabled(true);
        OutputParametersMinimumPB->setEnabled(true);
        emit ShowDiagnosticsChart3d();
        emit SetChartView2d(false);
        showDataTable(false);
    } else {
        OutputParametersCMB->setEnabled(true);
        OutputParametersCenterPB->setEnabled(false);
        OutputParametersMinimumPB->setEnabled(false);
        emit SetChartView2d(true);
        showDataTable(true);
    }
    OutputShowShadowLBL->setEnabled(state == Qt::Checked);
    OutputShowShadowCB->setEnabled(state == Qt::Checked);
}

void
MSSPM_GuiOutputControls::callback_OutputShowShadowCB(int dummy)
{
    emit ShowChart("","");
}

void
MSSPM_GuiOutputControls::showDataTable(bool showTable)
{

}

void
MSSPM_GuiOutputControls::callback_OutputShowBMSYCB(int val)
{
    emit ShowChart("","");
}

void
MSSPM_GuiOutputControls::callback_OutputMSYCB(int val)
{
    emit ShowChart("","");
}

void
MSSPM_GuiOutputControls::callback_OutputShowFMSYCB(int val)
{
    emit ShowChart("","");
}

void
MSSPM_GuiOutputControls::callback_OutputScaleCMB(QString scale)
{
    QString scenario = getOutputScenario();

    if (getOutputType() == "Multi-Scenario Plots") {
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else {
        emit ShowChart("","");
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
        OutputSpeciesLBL->setText("Species:");
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
                ControlsGroupBox,m_Logger,m_ProjectSettingsConfig,
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
MSSPM_GuiOutputControls::isCheckedOutputFMSY()
{
    return OutputShowFMSYCB->isChecked();
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
MSSPM_GuiOutputControls::getOutputType()
{
    return OutputTypeCMB->currentText();
}

QString
MSSPM_GuiOutputControls::getOutputScenario()
{
    return OutputScenariosCMB->currentText();
}

void
MSSPM_GuiOutputControls::setOutputParametersCB(bool checked)
{
    OutputParametersCB->setChecked(checked);
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
    OutputTypeCMB->setCurrentIndex(0);
    OutputTypeCMB->setCurrentText(type);

    return;
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
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
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
