#include "nmfOutputControls.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfOutputControls::nmfOutputControls(
        QGroupBox*   controlsGroupBox,
        nmfLogger*   logger,
        nmfDatabase* databasePtr,
        std::string& projectDir)
{
    m_Logger               = logger;
    m_DatabasePtr          = databasePtr;
    m_ProjectDir           = projectDir;
    m_SpeciesOrGuildModel  = new QStringListModel();
    m_IsAveraged           = nmfConstantsMSSPM::IsNotAveraged;
    m_NumSignificantDigits = -1;
    m_ProjectName.clear();
    m_ModelName.clear();
    m_SortedForecastLabelsMap.clear();
    m_MSY_Value            = -1;
    m_FMSY_Value           = -1;
    m_BMSY_Value           = -1;

    OutputLineBrightnessSL   = nullptr;
    OutputParametersZScoreCB = nullptr;
    ControlsGroupBox = controlsGroupBox;

    //readSettings();

    initWidgets();
    initConnections();
    loadWidgets();

    callback_OutputPublishLoadPB();
    //readSettingsPublish();
}

nmfOutputControls::~nmfOutputControls()
{

}

void
nmfOutputControls::enableControls(bool state)
{
    ControlsGroupBox->setEnabled(state);
}

bool
nmfOutputControls::isAveraged()
{
    return m_IsAveraged;
}

bool
nmfOutputControls::isCheckedOutputRescaleXAxis()
{
   return OutputRescaleXAxisCB->isChecked();
}

void
nmfOutputControls::setAveraged(bool isAveraged)
{
    m_IsAveraged = isAveraged;
}

void
nmfOutputControls::initWidgets()
{
    QModelIndex index;
    QVariant vNull(0);
    QString msg;
    QString subtitle = "---------- Format Properties ----------";

    QLabel*      SubtitleLBL      = new QLabel(subtitle);
    QVBoxLayout* controlLayt      = new QVBoxLayout();
    QHBoxLayout* BMSYLayt         = new QHBoxLayout();
    QHBoxLayout* MSYLayt          = new QHBoxLayout();
    QHBoxLayout* FMSYLayt         = new QHBoxLayout();
    QHBoxLayout* ParametersLayt   = new QHBoxLayout();
    QHBoxLayout* ScaleLayt        = new QHBoxLayout();
    QHBoxLayout* RescaleXAxisLayt = new QHBoxLayout();
    QHBoxLayout* HistoricalLayt   = new QHBoxLayout();
    QHBoxLayout* LegendLayt       = new QHBoxLayout();
    QHBoxLayout* GridLinesLayt    = new QHBoxLayout();
    QHBoxLayout* TitleLayt        = new QHBoxLayout();
    QHBoxLayout* FontLayt         = new QHBoxLayout();
    QHBoxLayout* FontSizeLayt     = new QHBoxLayout();
    QHBoxLayout* LineWidthLayt    = new QHBoxLayout();
    QHBoxLayout* LoadPublishLayt  = new QHBoxLayout();
    QHBoxLayout* SubtitleLayt     = new QHBoxLayout();
    QHBoxLayout* ShadowLayt       = new QHBoxLayout();
    QHBoxLayout* MinMaxLayt       = new QHBoxLayout();
    QWidget*     scrollAreaW      = new QWidget();
    QScrollArea* scrollAreaSA     = new QScrollArea();
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
    OutputLineWidthAxesLBL  = new QLabel("Axes:");
    OutputLineWidthDataLBL  = new QLabel("Lines:");
    OutputLineWidthPointLBL = new QLabel("Points:");
    OutputFontLBL           = new QLabel("Font:");
    OutputFontSizesLBL      = new QLabel("Font Sizes:  Labels:");
    OutputFontSizeNumberLBL = new QLabel("Numbers:");
    OutputLineBrightnessLBL->setToolTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessLBL->setStatusTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessSL->setToolTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessSL->setStatusTip("Sets the brightness for the Monte Carlo simulation lines.");
    OutputLineBrightnessLBL->setEnabled(false);
    OutputLineBrightnessSL->setEnabled(false);
    OutputLineWidthAxesSB     = new QSpinBox();
    OutputLineWidthDataSB     = new QSpinBox();
    OutputLineWidthPointSB    = new QSpinBox();
    OutputFontSizeLabelSB     = new QSpinBox();
    OutputFontSizeNumberSB    = new QSpinBox();
    OutputFontCMB             = new QFontComboBox();
    OutputChartTypeCMB        = new QComboBox();
    OutputSpeciesCMB          = new QComboBox();
    OutputParametersCMB       = new QComboBox();
    OutputMethodsCMB          = new QComboBox();
    OutputScenariosCMB        = new QComboBox();
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
    OutputLegendCB       = new QCheckBox("Legend:");
    OutputGridLinesCB    = new QCheckBox("Grid Lines:");
    OutputTitleCB        = new QCheckBox("Title:");
    OutputSpeciesCB      = new QCheckBox("Species:");
    OutputPublishLoadPB  = new QPushButton("Load Publish Settings");
    OutputPublishSavePB  = new QPushButton("Save");
    OutputPublishDefPB   = new QPushButton("Def");
    OutputShowShadowLBL  = new QLabel("3d Surface Shadow: ");
    OutputRescaleXAxisCB  = new QCheckBox();
    OutputRescaleXAxisLBL = new QLabel("Rescale X-Axis: ");
    OutputShowHistoricalDataCB  = new QCheckBox();
    OutputShowHistoricalDataLBL = new QLabel("Historical Data: ");
    OutputParameters2d3dPB = new QPushButton("3d");
    OutputLegendCB->setChecked(true);
    OutputGridLinesCB->setChecked(true);
    OutputTitleCB->setChecked(true);
    OutputSpeciesCB->setChecked(true);
    OutputTitleCB->setLayoutDirection(Qt::RightToLeft);
    OutputSpeciesCB->setLayoutDirection(Qt::RightToLeft);
    OutputLegendCB->setLayoutDirection(Qt::RightToLeft);
    OutputGridLinesCB->setLayoutDirection(Qt::RightToLeft);

    GridLinesLayt->addWidget(OutputSpeciesCB);
    GridLinesLayt->addWidget(OutputTitleCB);
    GridLinesLayt->addWidget(OutputLegendCB);
    GridLinesLayt->addWidget(OutputGridLinesCB);
    GridLinesLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
//    GridLinesLayt->addStretch();
    LineWidthLayt->addWidget(OutputLineWidthAxesLBL);
    LineWidthLayt->addWidget(OutputLineWidthAxesSB);
    LineWidthLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    LineWidthLayt->addWidget(OutputLineWidthDataLBL);
    LineWidthLayt->addWidget(OutputLineWidthDataSB);
    LineWidthLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    LineWidthLayt->addWidget(OutputLineWidthPointLBL);
    LineWidthLayt->addWidget(OutputLineWidthPointSB);
    LineWidthLayt->addStretch();
    FontLayt->addWidget(OutputFontLBL);
    FontLayt->addWidget(OutputFontCMB);
    FontLayt->addStretch();
    FontSizeLayt->addWidget(OutputFontSizesLBL);
    FontSizeLayt->addWidget(OutputFontSizeLabelSB);
    FontSizeLayt->addWidget(OutputFontSizeNumberLBL);
    FontSizeLayt->addWidget(OutputFontSizeNumberSB);
    FontSizeLayt->addStretch();
    SubtitleLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    SubtitleLayt->addWidget(SubtitleLBL);
    SubtitleLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    LoadPublishLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    LoadPublishLayt->addWidget(OutputPublishDefPB);
    LoadPublishLayt->addWidget(OutputPublishLoadPB);
    LoadPublishLayt->addWidget(OutputPublishSavePB);
    LoadPublishLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
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
    controlLayt->addLayout(RescaleXAxisLayt);
    controlLayt->addLayout(HistoricalLayt);
    controlLayt->addLayout(ShadowLayt);

    // ---------- Format Properties ----------
    controlLayt->setAlignment(Qt::AlignLeft);
    controlLayt->addSpacerItem(new QSpacerItem(1,5,QSizePolicy::Fixed,QSizePolicy::Fixed));
    controlLayt->addLayout(SubtitleLayt);
    controlLayt->addLayout(GridLinesLayt);
    controlLayt->addLayout(LineWidthLayt);
    controlLayt->addLayout(FontLayt);
    controlLayt->addLayout(FontSizeLayt);
    OutputLineWidthAxesSB->setMinimum(0);
    OutputLineWidthAxesSB->setMaximum(6);
    OutputLineWidthDataSB->setMinimum(0);
    OutputLineWidthDataSB->setMaximum(42);
    OutputFontSizeLabelSB->setMinimum(9);
    OutputFontSizeLabelSB->setMaximum(32);
    OutputFontSizeNumberSB->setMinimum(7);
    OutputFontSizeNumberSB->setMaximum(32);
    OutputLineWidthPointSB->setMinimum(5);
    OutputLineWidthPointSB->setMaximum(32);
    OutputFontCMB->setFixedWidth(200);
    OutputFontCMB->setWritingSystem(QFontDatabase::Any);
    OutputFontCMB->setFontFilters(QFontComboBox::AllFonts);
    OutputPublishSavePB->setFixedWidth(40);
    OutputPublishDefPB->setFixedWidth(30);
    RescaleXAxisLayt->addWidget(OutputRescaleXAxisLBL);
    RescaleXAxisLayt->addWidget(OutputRescaleXAxisCB);
    RescaleXAxisLayt->addStretch();
    HistoricalLayt->addWidget(OutputShowHistoricalDataLBL);
    HistoricalLayt->addWidget(OutputShowHistoricalDataCB);
    HistoricalLayt->addStretch();
    ShadowLayt->addWidget(OutputShowShadowLBL);
    ShadowLayt->addWidget(OutputShowShadowCB);
    ShadowLayt->addStretch();
    controlLayt->addLayout(LoadPublishLayt);

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
    msg = "<html>\
<strong><center>Entity Type</center></strong><br>\
Plots may show individual Species, Guilds, or the entire System. When viewing \
Guilds or the System, keep in mind that the scale of each Species may not be \
the same.\
</html>";
    OutputGroupTypeCMB->setWhatsThis(msg);

    msg ="<html>\
<strong><center>B MSY</center></strong><br>\
If K=0 (which will happen if the growth rate selected is Linear), \
the equation used for K becomes K(i) = r(i)/alpha(i,i).<br><br>\
If K is not 0, then:<br><br>\
&nbsp;&nbsp;For Species:&nbsp;B MSY = K(i)/2, where r(i) = carrying capacity for that species<br><br>\
&nbsp;&nbsp;For Guilds:&nbsp;&nbsp;B MSY = Σ[K(i)]/2, where Σ is over all species in the selected guild<br><br>\
&nbsp;&nbsp;For System:&nbsp;&nbsp;B MSY = Σ[K(i)]/2, where Σ is over all species\
</html>";
    OutputShowBMSYCB->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYCB->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYCB->setWhatsThis(msg);
    OutputShowBMSYLE->setToolTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setStatusTip("Biomass Maximum Sustained Yield (K/2)");
    OutputShowBMSYLE->setWhatsThis(msg);
    msg ="<html>\
<strong><center>MSY</center></strong><br>\
If K=0 (which will happen if the growth rate selected is Linear), \
the equation used for K becomes K(i) = r(i)/alpha(i,i).<br><br>\
If K is not 0, then:<br><br>\
&nbsp;&nbsp;For Species: MSY = [r(i)K(i)]/4, where r(i) and K(i) are the growth rate and carrying capacity for that species<br><br>\
&nbsp;&nbsp;For Guilds: MSY = Σ[r(i)K(i)]/4, where Σ is over all species in the selected guild<br><br>\
&nbsp;&nbsp;For System: MSY = Σ[r(i)K(i)]/4, where Σ is over all species\
</html>";
    OutputShowMSYCB->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYCB->setStatusTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYCB->setWhatsThis(msg);
    OutputShowMSYLE->setToolTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setStatusTip("Maximum Sustained Yield (rK/4)");
    OutputShowMSYLE->setWhatsThis(msg);
    msg ="<html>\
<strong><center>F MSY</center></strong><br>\
For Species: F MSY = r(i) where r(i) = growth rate for that species<br><br>\
For Guilds: F MSY = guildAverage[r(i)] / NumSpeciesInGuild, where the guildAverage is the average for the selected guild<br><br>\
For System: F MSY = average[r(i)] / NumSpecies, where the average is over all species\
</html>";
    OutputShowFMSYCB->setToolTip(  "Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYCB->setStatusTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYCB->setWhatsThis(msg);
    OutputShowFMSYLE->setToolTip(  "Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYLE->setStatusTip("Fishing Mortality Maximum Sustained Yield (r/2)");
    OutputShowFMSYLE->setWhatsThis(msg);
    OutputYAxisMinLBL->setToolTip(  "Set the min value of the y-axis");
    OutputYAxisMinLBL->setStatusTip("Set the min value of the y-axis");
    OutputYAxisMinSL->setToolTip(  "Set the min value of the y-axis");
    OutputYAxisMinSL->setStatusTip("Set the min value of the y-axis");
    OutputYAxisMaxLBL->setToolTip(  "Set the max value of the y-axis");
    OutputYAxisMaxLBL->setStatusTip("Set the max value of the y-axis");
    OutputYAxisMaxSB->setToolTip(  "Set the max value of the y-axis");
    OutputYAxisMaxSB->setStatusTip("Set the max value of the y-axis");
    OutputScenariosLBL->setToolTip(  "These are the available Forecast Scenarios");
    OutputScenariosLBL->setStatusTip("These are the available Forecast Scenarios");
    OutputScenariosCMB->setToolTip(  "These are the available Multi-Forecast Scenarios");
    OutputScenariosCMB->setStatusTip("These are the available Multi-Forecast Scenarios");
    OutputPublishLoadPB->setToolTip(  "Loads the appropriate format widgets with Publishing values");
    OutputPublishLoadPB->setStatusTip("Loads the appropriate format widgets with Publishing values");
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
    OutputRescaleXAxisCB->setToolTip(   "Applies a \"nice\" scale factor to the x-axis");
    OutputRescaleXAxisCB->setStatusTip( "Applies a \"nice\" scale factor to the x-axis");
    OutputRescaleXAxisLBL->setToolTip(  "Applies a \"nice\" scale factor to the x-axis");
    OutputRescaleXAxisLBL->setStatusTip("Applies a \"nice\" scale factor to the x-axis");
    OutputShowHistoricalDataCB->setToolTip(   "Toggles historical data for a Forecast plot");
    OutputShowHistoricalDataCB->setStatusTip( "Toggles historical data for a Forecast plot");
    OutputShowHistoricalDataLBL->setToolTip(  "Toggles historical data for a Forecast plot");
    OutputShowHistoricalDataLBL->setStatusTip("Toggles historical data for a Forecast plot");
    OutputShowShadowCB->setToolTip(  "Toggles the 3d surface's shadow");
    OutputShowShadowCB->setStatusTip("Toggles the 3d surface's shadow");
    OutputChartTypeLBL->setToolTip(  "The type of chart that will be displayed");
    OutputChartTypeLBL->setStatusTip("The type of chart that will be displayed");
    OutputChartTypeCMB->setToolTip(  "The type of chart that will be displayed");
    OutputChartTypeCMB->setStatusTip("The type of chart that will be displayed");

    OutputSpeciesCB->setToolTip(     "Toggles the plot's Species name as the title");
    OutputSpeciesCB->setStatusTip(   "Toggles the plot's Species name as the title");
    OutputTitleCB->setToolTip(       "Toggles the plot's main title");
    OutputTitleCB->setStatusTip(     "Toggles the plot's main title");
    OutputLegendCB->setToolTip(      "Toggles the plot's legend");
    OutputLegendCB->setStatusTip(    "Toggles the plot's legend");
    OutputGridLinesCB->setToolTip(   "Toggles the plot's grid lines");
    OutputGridLinesCB->setStatusTip( "Toggles the plot's grid lines");
    OutputLineWidthAxesLBL->setToolTip(   "Sets the axis line width");
    OutputLineWidthAxesLBL->setStatusTip( "Sets the axis line width");
    OutputLineWidthAxesSB->setToolTip(    "Sets the axis line width");
    OutputLineWidthAxesSB->setStatusTip(  "Sets the axis line width");
    OutputLineWidthDataLBL->setToolTip(   "Sets the data line width");
    OutputLineWidthDataLBL->setStatusTip( "Sets the data line width");
    OutputLineWidthDataSB->setToolTip(    "Sets the data line width");
    OutputLineWidthDataSB->setStatusTip(  "Sets the data line width");
    OutputLineWidthPointLBL->setToolTip(   "Sets the observed biomass point size");
    OutputLineWidthPointLBL->setStatusTip( "Sets the observed biomass point size");
    OutputLineWidthPointSB->setToolTip(    "Sets the observed biomass point size");
    OutputLineWidthPointSB->setStatusTip(  "Sets the observed biomass point size");
    OutputFontLBL->setToolTip(   "Sets the axis label font");
    OutputFontLBL->setStatusTip( "Sets the axis label font");
    OutputFontCMB->setToolTip(   "Selects the axis label font");
    OutputFontCMB->setStatusTip( "Selects the axis label font");
    OutputFontSizeLabelSB->setToolTip(    "Sets the axis label font size");
    OutputFontSizeLabelSB->setStatusTip(  "Sets the axis label font size");
    OutputFontSizeNumberSB->setToolTip(    "Sets the axis numeric scale font size");
    OutputFontSizeNumberSB->setStatusTip(  "Sets the axis numeric scale font size");
    OutputPublishSavePB->setToolTip(    "Saves the current format settings as the new Publish settings");
    OutputPublishSavePB->setStatusTip(  "Saves the current format settings as the new Publish settings");
    OutputPublishDefPB->setToolTip(     "Loads default values for the format settings");
    OutputPublishDefPB->setStatusTip(   "Loads default values for the format settings");

    msg = "<html><strong><center>Species Name</center></strong><br>\
This toggles on/off the Species name as the main chart title. \
The user may want to see only the Species name when creating a multi-species plot \
(aka. panel plot).</html>";
    OutputSpeciesCB->setWhatsThis(msg);
    msg = "<html><strong><center>Title</center></strong><br>\
This toggles on/off the main chart title. The user may want this off \
if the chart will be used in a publication.</html>";
    OutputTitleCB->setWhatsThis(msg);
    msg = "<html><strong><center>Legend</center></strong><br>\
This toggles on/off the chart legend. The user may want this off \
if the chart will be used in a publication.</html>";
    OutputLegendCB->setWhatsThis(msg);
    msg = "<html><strong><center>Grid Lines</center></strong><br>\
This toggles on/off the chart grid lines. The user may want them turned off \
if the chart will be used in a publication.</html>";
    OutputGridLinesCB->setWhatsThis(msg);
    msg = "<html><strong><center>Axes Line Widths</center></strong><br>\
This sets the width of the x and y axes lines. The color of the lines is fixed to black.</html>";
    OutputLineWidthAxesLBL->setWhatsThis(msg);
    OutputLineWidthAxesSB->setWhatsThis(msg);
    msg = "<html><strong><center>Data Line Widths</center></strong><br>\
This sets the width of the lines depicting all data (i.e., estimations, forecasts.</html>";
    OutputLineWidthDataLBL->setWhatsThis(msg);
    OutputLineWidthDataSB->setWhatsThis(msg);
    msg = "<html><strong><center>Point Widths</center></strong><br>\
This sets the width of the points (i.e., filled circles) representing the observed \
biomass. The color of these points is fixed to blue.</html>";
    OutputLineWidthPointLBL->setWhatsThis(msg);
    OutputLineWidthPointSB->setWhatsThis(msg);
    msg = "<html><strong><center>Font</center></strong><br>\
This sets the font used for the chart titles and axes. The color is fixed to black.</html>";
    OutputFontLBL->setWhatsThis(msg);
    OutputFontCMB->setWhatsThis(msg);
    msg = "<html><strong><center>Font Label Size</center></strong><br>\
This sets the size of the labels used as the x-axis and y_axis titles. The color is fixed to black.</html>";
    OutputFontSizeLabelSB->setWhatsThis(msg);
    msg = "<html><strong><center>Font Number Size</center></strong><br>\
This sets the size of the numbers used for the x-axis and y-axis scale values. The color is fixed to black.</html>";
    OutputFontSizeNumberSB->setWhatsThis(msg);
    OutputFontSizeNumberLBL->setWhatsThis(msg);
    msg = "<html><strong><center>Load Default</center></strong><br>\
This loads the default starting values for the chart publishing widgets and updates \
the current chart. Be sure to click the <strong>Save</strong> button to the right of \
the <strong>Load Publish Settings</strong> button if you want to save these \
default settings as the new publish settings.</html>";
    OutputPublishDefPB->setWhatsThis(msg);
    msg = "<html><strong><center>Load Publish Settings</center></strong><br>\
This loads the last saved publish settings to set the chart attributes to those values \
that will make charts of high enough quality to be acceptable for publication.</html>";
    OutputPublishLoadPB->setWhatsThis(msg);
    msg = "<html><strong><center>Save</center></strong><br>\
This will save the current publish settings (aka. Format Properties) \
so that the next time the user clicks the <strong>Load Publish Settings</strong> button \
the appropriate widgets will be set to these values.</html>";
    OutputPublishSavePB->setWhatsThis(msg);

    msg ="<html>\
<strong><center>Chart Types</center></strong><br>\
The various output chart types are: <br><br>\
<strong>1. Biomass vs Time</strong><br><br> \
This is a plot of calculated (i.e., estimated) biomass vs time.<br><br>\
<strong>2. Biomass & Catch vs Time</strong><br><br> \
This is a plot of observed biomass with catch vs time.<br><br>\
<strong>3. Harvest vs Time</strong><br><br> \
This plot will show either catch vs time or (catchability * effort * calculated biomass) vs time.<br><br>\
<strong>4. Exploitation Rate</strong><br><br> \
This plot will show (catchability * effort) vs time and will typically \
range from 0 to 1 in the y-axis. The guild calculations are: \
(total catch) / (total estBiomass) or if using effort: Σ(catchability*effort*estBiomass)/Σ(estBiomass).<br><br>\
<strong>5. Diagnostics</strong><br><br> \
These plots will show either the parameter estimation diagnostics \
(2d or 3d) or the retrospective analysis (i.e., Mohn's Rho).<br><br>\
<strong>6. Forecasts</strong><br><br>\
This plot will show the Monte Carlo n% variation forecasts overlayed onto the 0% variation forecast.<br><br>\
<strong>7. Multi-Scenario Plots</strong><br><br>\
This plot shows combined user selected forecasts. That is, after the user runs a forecast,<br>\
they can elect to save the forecast into a multi-forecast scenario. In this fashion, they<br>\
can compare several forecasts simultaneously.<br>\
</html>";
    OutputChartTypeLBL->setWhatsThis(msg);
    OutputChartTypeCMB->setWhatsThis(msg);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartBiomass);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartBiomassAndCatch);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartHarvest);
    OutputChartTypeCMB->addItem(nmfConstantsMSSPM::OutputChartExploitation);
    OutputChartTypeCMB->addItem("(Abs) Bc vs Time");
    OutputChartTypeCMB->addItem("(Rel) Bc vs Time");
    OutputChartTypeCMB->addItem("Catch vs Bc");
    OutputChartTypeCMB->addItem("Diagnostics");
    OutputChartTypeCMB->addItem("Forecast");
    OutputChartTypeCMB->addItem("Multi-Scenario Plots");
    OutputChartTypeCMB->setItemData(0, "Calculated and Observed Biomass vs Time for a Species",    Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(0, "Observed Biomass and Catch vs Time for a Species",         Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(1, "Calculated Biomass vs Time for one or more Species",       Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(2, "Catch vs Calculated Biomass for a Species",                Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(8, "Allows the user to view multiple Forecasts simultaneously",Qt::ToolTipRole);
    // Disable 3 items for now....
    for (int i=4; i<7; ++i) {
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
    msg ="<html>\
<strong><center>Center</center></strong><br>\
This button will move the surface marker to the center of the surface. The marker displays \
the coordinates of the point as well as the y-value (i.e., fitness) of the point.\
</html>";
    OutputParametersCenterPB->setWhatsThis(msg);
    OutputParametersMinimumPB->setText("");
    OutputParametersMinimumPB->setIcon(minimumIcon);
    OutputParametersMinimumPB->setFixedWidth(20);
    OutputParametersMinimumPB->setFixedHeight(20);
    OutputParametersMinimumPB->setToolTip("Selects minimum point on surface\n(i.e., the estimated r and K values)");
    OutputParametersMinimumPB->setStatusTip("Selects minimum point on surface\n(i.e., the estimated r and K values)");
    OutputParametersMinimumPB->setEnabled(false);
    msg ="<html>\
<strong><center>Minimum</center></strong><br>\
This button will move the surface marker to the lowest point of the surface. The marker displays \
the coordinates of the point as well as the y-value (i.e., fitness) of the minimum fitness point.\
</html>";
    OutputParametersMinimumPB->setWhatsThis(msg);
    OutputParameters2d3dPB->setFixedWidth(20);
    OutputParameters2d3dPB->setFixedHeight(20);
    OutputParameters2d3dPB->setEnabled(false);
    OutputParameters2d3dPB->setToolTip("Displays a 1-parameter 2d chart or 2-parameter 3d chart.");
    OutputParameters2d3dPB->setStatusTip("Displays a 1-parameter 2d chart or 2-parameter 3d chart.");
    msg ="<html>\
<strong><center>Plot Type Toggle</center></strong><br>\
This button is a toggle that will switch the plot from a 1-parameter 2d line chart to a 2-parameter 3d surface plot. \
The 3d surface plot can be manipulated with the right mouse button.\
</html>";
    OutputParameters2d3dPB->setWhatsThis(msg);

    OutputScenariosLBL->setEnabled(false);
    OutputScenariosCMB->setEnabled(false);
    OutputMethodsCMB->setEnabled(false);
    OutputMethodsLBL->setEnabled(false);
    OutputSpeciesCMB->setToolTip("The species/guild reflected in the current chart");
    OutputSpeciesCMB->setStatusTip("The species/guild reflected in the current chart");
    OutputSpeciesLBL->setToolTip("The species/guild reflected in the current chart");
    OutputSpeciesLBL->setStatusTip("The species/guild reflected in the current chart");
    msg ="<html>\
<strong><center>Species/Guilds</center></strong><br>\
This pulldown lists the species or guilds available to the user in the current project. \
To select species/guilds use the previous pulldown to select entity type.\
</html>";
    OutputSpeciesCMB->setWhatsThis(msg);
    OutputSpeciesLBL->setWhatsThis(msg);
    OutputMethodsCMB->setToolTip(  "Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsCMB->setStatusTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsLBL->setToolTip(  "Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    OutputMethodsLBL->setStatusTip("Allows user to select between viewing Parameter Profiles or a Retrospective Analysis");
    msg ="<html>\
<strong><center>Diagnostic Methods</center></strong><br>\
This pulldown allows the user to choose which type of diagnostic plot is to be viewed. \
Currently, the user may view either Profile Parameters or a Retrospective Analysis.\
</html>";
    OutputMethodsCMB->setWhatsThis(msg);
    OutputMethodsLBL->setWhatsThis(msg);
    OutputParametersCMB->setToolTip("Allows user to select which Parameter to view graphically");
    OutputParametersCMB->setStatusTip("Allows user to select which Parameter to view graphically");
    OutputParametersLBL->setToolTip("Allows user to select which Parameter to view graphically");
    OutputParametersLBL->setStatusTip("Allows user to select which Parameter to view graphically");
    msg ="<html>\
<strong><center>Estimated Parameters</center></strong><br>\
This pulldown allows the user to choose which estimated parameter's diagnostic plots \
they wish to view. Plots may be views as either a 2d line plot or a 3d surface plot.\
</html>";
    OutputParametersCMB->setWhatsThis(msg);
    OutputParametersLBL->setWhatsThis(msg);
    OutputScenariosLBL->setToolTip("Allows user to select which Scenario to view");
    OutputScenariosLBL->setStatusTip("Allows user to select which Scenario to view");
    OutputScenariosCMB->setToolTip("Allows user to select which Scenario to view");
    OutputScenariosCMB->setStatusTip("Allows user to select which Scenario to view");
    msg ="<html>\
<strong><center>Scenarios</center></strong><br>\
This pulldown allows the user to choose which forecast scenario to view. From the \
forecast run page, the user may create scenarios that show multiple forecasts. These \
are named and can be viewed with this pulldown.\
</html>";
    OutputScenariosCMB->setWhatsThis(msg);
    OutputScenariosLBL->setWhatsThis(msg);
    OutputScaleCMB->addItem("Default");
    OutputScaleCMB->addItem("000");
    OutputScaleCMB->addItem("000 000");
    OutputScaleCMB->addItem("000 000 000");
    OutputScaleLBL->setToolTip("Sets the scale of the y-axis");
    OutputScaleLBL->setStatusTip("Sets the scale of the y-axis");
    OutputScaleCMB->setToolTip("Sets the scale of the y-axis");
    OutputScaleCMB->setStatusTip("Sets the scale of the y-axis");
    msg ="<html>\
<strong><center>Scale Factor</center></strong><br>\
This pulldown allows the user to select the y-axis scale of the current plot. \
Note that changing this will change the scale of the plot as well as updating the \
y-axis label.\
</html>";
    OutputScaleCMB->setWhatsThis(msg);
    OutputScaleLBL->setWhatsThis(msg);

    loadSpeciesControlWidget();
    callback_LoadScenariosWidget();

    readSettings();

    OutputShowMSYCB->setEnabled( false);
    OutputShowFMSYCB->setEnabled(false);
    OutputYAxisMaxLBL->setEnabled(false);
    OutputYAxisMinSB->setEnabled(false);
    OutputYAxisMaxSB->setEnabled(false);
    OutputShowShadowCB->setEnabled(false);
    OutputShowShadowCB->setChecked(false);
    OutputShowShadowLBL->setEnabled(false);
    OutputShowHistoricalDataCB->setChecked(false);
    OutputShowHistoricalDataCB->setEnabled(false);
    OutputShowHistoricalDataLBL->setEnabled(false);
    OutputParametersZScoreCB->setEnabled(false);

    ControlsGroupBox->setMinimumHeight(100);
}

bool
nmfOutputControls::isShadowShown()
{
    return OutputShowShadowCB->isChecked();
}

bool
nmfOutputControls::isHistoricalDataShown()
{
    return OutputShowHistoricalDataCB->isChecked();
}

void
nmfOutputControls::setCurrentSpecies(QString species)
{
    OutputSpeciesCMB->setCurrentText(species);
}

void
nmfOutputControls::initConnections()
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
            this,                     SLOT(callback_OutputParametersZScoreCB(int)));
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
            this,                     SLOT(callback_OutputParametersCenterPB()));
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
    connect(OutputRescaleXAxisCB,   SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputRescaleXAxisCB(int)));
    connect(OutputShowHistoricalDataCB, SIGNAL(stateChanged(int)),
            this,                       SLOT(callback_OutputShowHistoricalDataCB(int)));
    connect(OutputSpeciesCB,        SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputSpeciesCB(int)));
    connect(OutputTitleCB,          SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputTitleCB(int)));
    connect(OutputLegendCB,         SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputLegendCB(int)));
    connect(OutputGridLinesCB,      SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_OutputGridLinesCB(int)));
    connect(OutputPublishLoadPB,    SIGNAL(clicked()),
            this,                   SLOT(callback_OutputPublishLoadPB()));
    connect(OutputPublishSavePB,    SIGNAL(clicked()),
            this,                   SLOT(callback_OutputPublishSavePB()));
    connect(OutputPublishDefPB,     SIGNAL(clicked()),
            this,                   SLOT(callback_OutputPublishDefPB()));
    connect(OutputLineWidthAxesSB,  SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputLineWidthAxisSB(int)));
    connect(OutputLineWidthDataSB,  SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputLineWidthDataSB(int)));
    connect(OutputLineWidthPointSB, SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputLineWidthPointSB(int)));
    connect(OutputFontSizeLabelSB,  SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputFontSizeLabelSB(int)));
    connect(OutputFontSizeNumberSB, SIGNAL(valueChanged(int)),
            this,                   SLOT(callback_OutputFontSizeNumberSB(int)));
    connect(OutputFontCMB,          SIGNAL(currentTextChanged(QString)),
            this,                   SLOT(callback_OutputFontCMB(QString)));
}

void
nmfOutputControls::callback_LoadScenariosWidget()
{
    int currentIndex = OutputScenariosCMB->currentIndex();
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields   = {"ScenarioName"};
    queryStr = "SELECT DISTINCT ScenarioName from " +
                nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND ModelName = '"    + m_ModelName   +
               "' ORDER BY ScenarioName";
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
nmfOutputControls::loadWidgets()
{
    readSettings();
    loadSortedForecastLabels();
    loadSpeciesControlWidget();
    callback_LoadScenariosWidget();
}


void
nmfOutputControls::loadSortedForecastLabels()
{
    m_DatabasePtr->createScenarioMap(
                m_ProjectName,
                m_ModelName,
                m_SortedForecastLabelsMap);
}

void
nmfOutputControls::loadSpeciesControlWidget()
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
nmfOutputControls::getSpeciesNumFromName(QString SpeciesName)
{
    return m_SpeciesHash[SpeciesName];
}


bool
nmfOutputControls::getSpecies(int&         NumSpecies,
                                    QStringList& SpeciesList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    SpeciesList.clear();

    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName from " + nmfConstantsMSSPM::TableSpecies + " ORDER BY SpeName";
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
nmfOutputControls::getGuilds(int&         NumGuilds,
                                   QStringList& GuildList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    GuildList.clear();

    fields    = {"GuildName"};
    queryStr  = "SELECT GuildName from " + nmfConstantsMSSPM::TableGuilds + " ORDER BY GuildName";
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
nmfOutputControls::refreshBrightnessSlider()
{
    OutputLineBrightnessSL->setValue(OutputLineBrightnessSL->value());
}

void
nmfOutputControls::callback_OutputLineBrightnessSL(int value)
{
    double brightnessFactor = value/(double)OutputLineBrightnessSL->maximum(); // 0 .. 1

    emit ForecastLineBrightnessChanged(brightnessFactor);
}


int
nmfOutputControls::getYMinSliderVal()
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
nmfOutputControls::callback_OutputYAxisMinSL(int value)
{
    updateChart();
}

void
nmfOutputControls::callback_OutputYAxisMinSB(int value)
{
    updateChart();
}

void
nmfOutputControls::callback_OutputYAxisMaxSB(int value)
{
    updateChart();
}


void
nmfOutputControls::callback_OutputParametersCenterPB()
{
    emit SelectCenterSurfacePoint();
}

void
nmfOutputControls::callback_OutputParametersMinimumPB()
{
    emit SelectMinimumSurfacePoint();
}


void
nmfOutputControls::enableMSYWidgets(bool state)
{
    OutputShowBMSYCB->setEnabled(state);
    OutputShowBMSYLE->setEnabled(state);
    OutputShowMSYCB->setEnabled( state);
    OutputShowMSYLE->setEnabled( state);
    OutputShowFMSYCB->setEnabled(state);
    OutputShowFMSYLE->setEnabled(state);
}

void
nmfOutputControls::enableBrightnessWidgets(bool state)
{
    OutputLineBrightnessLBL->setEnabled(state);
    OutputLineBrightnessSL->setEnabled(state);
}

int
nmfOutputControls::getNumberSpecies()
{
    return OutputSpeciesCMB->count();
}

void
nmfOutputControls::setSpeciesNum(int speciesNum)
{
    OutputSpeciesCMB->setCurrentIndex(speciesNum);
}

bool
nmfOutputControls::isSetToGuild()
{
    return (OutputSpeciesCMB->currentText() != "Species:");
}

bool
nmfOutputControls::isSetToDiagnostics()
{
    return (getOutputChartType() == "Diagnostics");
}

bool
nmfOutputControls::isSetToMultiScenarioPlots()
{
    return (OutputChartTypeCMB->currentText() == "Multi-Scenario Plots");
}

bool
nmfOutputControls::isSetToRetrospectiveAnalysis()
{
    return (OutputMethodsCMB->currentText() == "Retrospective Analysis");
}

void
nmfOutputControls::refreshChart()
{
    int currentIndex = OutputChartTypeCMB->currentIndex();

    // Flip to a different chart type
    if (currentIndex == 0) {
        OutputChartTypeCMB->setCurrentIndex(1);
    } else {
        OutputChartTypeCMB->setCurrentIndex(0);
    }
    // Flip back to original chart type
    OutputChartTypeCMB->setCurrentIndex(currentIndex);
}

void
nmfOutputControls::callback_OutputChartTypeCMB(QString outputType)
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

    // Turn off historical checkbox if not in Forecast
    if (! isForecast) {
        OutputShowHistoricalDataCB->setChecked(false);
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
    OutputShowHistoricalDataLBL->setEnabled(isForecast);
    OutputShowHistoricalDataCB->setEnabled(isForecast);

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
            emitRefreshChart();
            if (! displaying3dChart()) {
                emit SetChartView2d(true);
            }
        } else { // Mohn's Rho
            emit ShowChartMohnsRho();
        }
    } else if (isForecast) {
        // Need to flip historical checkbox
        bool showHistoricalData = OutputShowHistoricalDataCB->isChecked();
        OutputShowHistoricalDataCB->setChecked(false);
        OutputYAxisMinSL->setValue(0);
        callback_ResetOutputWidgetsForAggProd();
        emit SetChartView2d(true);
        emitRefreshChart();
        OutputShowHistoricalDataCB->setChecked(showHistoricalData);
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
nmfOutputControls::callback_OutputGroupTypeCMB(QString outputGroupType)
{
    if (outputGroupType == "Species:" ) {
        loadSpeciesControlWidget();
        emitRefreshChart();
    } else if (outputGroupType == "Guild:") {
        loadSpeciesControlWidget();
        emit ShowChartBy("Guild",m_IsAveraged);
    } else if (outputGroupType == "System") {
        OutputSpeciesCMB->clear();
        emit ShowChartBy("System",m_IsAveraged);
    }
}

void
nmfOutputControls::refresh()
{
    int currentIndex = OutputChartTypeCMB->currentIndex();
//qDebug() << "ci: " << currentIndex;
    try {
//      if (currentIndex == 0) {
//qDebug() << "num: " << OutputChartTypeCMB->count();  // RSK fix possible bug here when choose Type III in GUI
//          OutputChartTypeCMB->setCurrentIndex(1);
//      } else {
//          OutputChartTypeCMB->setCurrentIndex(0);
//      }
      OutputChartTypeCMB->setCurrentIndex(currentIndex);
    } catch (...) {
      return;
    }

    refreshScenarios();
}

void
nmfOutputControls::refreshScenarios()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"ScenarioName"};
    queryStr   = "SELECT DISTINCT ScenarioName FROM " +
                  nmfConstantsMSSPM::TableForecastBiomassMultiScenario +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ScenarioName"].size();

    OutputScenariosCMB->clear();
    for (int i=0; i<NumRecords; ++i) {
        OutputScenariosCMB->addItem(QString::fromStdString(dataMap["ScenarioName"][i]));
    }
}


void
nmfOutputControls::callback_OutputSpeciesCMB(QString outputSpecies)
{
    // Flip historical checkbox so as to avoid overlaying plots
    // RSK -revisit this
    bool isHistoricalDataChecked = isHistoricalDataShown();
    OutputShowHistoricalDataCB->setChecked(false);

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

    OutputShowHistoricalDataCB->setChecked(isHistoricalDataChecked);
}

void
nmfOutputControls::callback_OutputMethodsCMB(QString method)
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
        emitRefreshChart();
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
nmfOutputControls::setForMohnsRho()
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
nmfOutputControls::setForBiomassVsTime()
{
  OutputChartTypeCMB->setCurrentText(nmfConstantsMSSPM::OutputChartBiomass);
}

void
nmfOutputControls::displayMohnsRho()
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
nmfOutputControls::callback_OutputParametersCMB(QString unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputScenariosCMB(QString scenario)
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
nmfOutputControls::callback_SetOutputScenario(QString scenario)
{
    if (scenario.isEmpty()) {
        return;
    }
    OutputScenariosCMB->setCurrentText(scenario);
}

void
nmfOutputControls::callback_OutputParameters2d3dPB()
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
nmfOutputControls::callback_OutputParametersZScoreCB(int state)
{
    emitRefreshChart();
}

bool
nmfOutputControls::displaying3dChart()
{
    return (OutputParameters2d3dPB->text() == "2d");
}

void
nmfOutputControls::callback_OutputShowShadowCB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputShowHistoricalDataCB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputRescaleXAxisCB(int unused)
{
    emitRefreshChart();
}


void
nmfOutputControls::callback_OutputGridLinesCB(int unused)
{
//    OutputLineWidthAxesSB->blockSignals(true);
//    OutputLineWidthAxesSB->setValue(1);
//    OutputLineWidthAxesSB->blockSignals(false);

    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputSpeciesCB(int unused)
{
    // Can't have title box and species both checked at the same time
    OutputTitleCB->blockSignals(true);
    OutputTitleCB->setChecked(false);
    OutputTitleCB->blockSignals(false);

    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputTitleCB(int unused)
{
    // Can't have title box and species both checked at the same time
    OutputSpeciesCB->blockSignals(true);
    OutputSpeciesCB->setChecked(false);
    OutputSpeciesCB->blockSignals(false);

    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputLegendCB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputLineWidthDataSB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputLineWidthPointSB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputLineWidthAxisSB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputFontSizeLabelSB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputFontSizeNumberSB(int unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputFontCMB(QString unused)
{
    emitRefreshChart();
}

void
nmfOutputControls::callback_OutputPublishSavePB()
{
    saveSettingsPublish();
}

void
nmfOutputControls::callback_OutputPublishDefPB()
{
    OutputSpeciesCB->setChecked(true);
    OutputTitleCB->setChecked(false);
    OutputLegendCB->setChecked(false);
    OutputGridLinesCB->setChecked(false);
    OutputLineWidthAxesSB->setValue( nmfConstantsMSSPM::PublishLineWidthAxes);
    OutputLineWidthDataSB->setValue( nmfConstantsMSSPM::PublishLineWidthData);
    OutputLineWidthPointSB->setValue(nmfConstantsMSSPM::PublishLineWidthPoint);
    OutputFontSizeLabelSB->setValue( nmfConstantsMSSPM::PublishFontSizeLabel);
    OutputFontSizeNumberSB->setValue(nmfConstantsMSSPM::PublishFontSizeNumber);
    OutputFontCMB->setCurrentText(   nmfConstantsMSSPM::PublishFont);
}

void
nmfOutputControls::callback_OutputPublishLoadPB()
{
    readSettingsPublish();
    emitRefreshChart();
}

void
nmfOutputControls::emitRefreshChart()
{
    if (isSetToMultiScenarioPlots()) {
        QString scenario = getOutputScenario();
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if (isSetToDiagnostics() && isSetToRetrospectiveAnalysis()) {
        emit ShowChartMohnsRho();
    } else {
        emit ShowChart("","");
    }
}

void
nmfOutputControls::updateChart()
{
    QString outputGroupType = getOutputGroupType();

    if (outputGroupType == "Species") {
        emitRefreshChart();
    } else if (outputGroupType == "Guild") {
        emit ShowChartBy("Guild",m_IsAveraged);
    } else if (outputGroupType == "System") {
        emit ShowChartBy("System",m_IsAveraged);
    }
}

void
nmfOutputControls::callback_OutputBMSYCB(int val)
{
    updateChart();
}

void
nmfOutputControls::callback_OutputMSYCB(int val)
{
    updateChart();
}

void
nmfOutputControls::callback_OutputFMSYCB(int val)
{
    updateChart();
}

void
nmfOutputControls::callback_OutputScaleCMB(QString scale)
{
    if (isSetToMultiScenarioPlots()) {
        QString scenario = getOutputScenario();
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if (isSetToDiagnostics() && isSetToRetrospectiveAnalysis()) {
        emit ShowChartMohnsRho();
    } else {
        updateChart();
    }
}

void
nmfOutputControls::callback_ResetOutputWidgetsForAggProd()
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
nmfOutputControls::isAggProd()
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
nmfOutputControls::clearOutputBMSY()
{
    OutputShowBMSYLE->clear();
}

void
nmfOutputControls::clearOutputMSY()
{
    OutputShowMSYLE->clear();
}

void
nmfOutputControls::clearOutputFMSY()
{
    OutputShowFMSYLE->clear();
}

bool
nmfOutputControls::isSurfaceTypeZScore()
{
    return OutputParametersZScoreCB->isChecked();
}

bool
nmfOutputControls::isCheckedOutputBMSY()
{
    bool isChecked;

    OutputShowBMSYCB->blockSignals(true);
    isChecked = OutputShowBMSYCB->isChecked();
    OutputShowBMSYCB->blockSignals(false);

    return isChecked;
}

bool
nmfOutputControls::isCheckedOutputGridLines()
{
    return OutputGridLinesCB->isChecked();
}

bool
nmfOutputControls::isCheckedOutputLegend()
{
    return OutputLegendCB->isChecked();
}

bool
nmfOutputControls::isCheckedOutputMSY()
{
    return OutputShowMSYCB->isChecked();
}

bool
nmfOutputControls::isCheckedOutputSpecies()
{
    return OutputSpeciesCB->isChecked();
}

bool
nmfOutputControls::isCheckedOutputTitle()
{
    return OutputTitleCB->isChecked();
}

bool
nmfOutputControls::isEnabledOutputMSY()
{
    return OutputShowMSYCB->isEnabled();
}

bool
nmfOutputControls::isCheckedOutputFMSY()
{
    return OutputShowFMSYCB->isChecked();
}


bool
nmfOutputControls::isEnabledOutputFMSY()
{
    return OutputShowFMSYCB->isEnabled();
}

void
nmfOutputControls::setTextOutputBMSY(QString text)
{
    m_BMSY_Value = (! text.isEmpty()) ? text.toDouble() : -1;
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                text.toDouble(),m_NumSignificantDigits,3);

    OutputShowBMSYLE->blockSignals(true);
    OutputShowBMSYLE->setText(valueWithComma);
    OutputShowBMSYLE->blockSignals(false);
}

void
nmfOutputControls::setTextOutputMSY(QString text)
{
    m_MSY_Value = (! text.isEmpty()) ? text.toDouble() : -1;
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                text.toDouble(),m_NumSignificantDigits,3);
    OutputShowMSYLE->setText(valueWithComma);
}

void
nmfOutputControls::setTextOutputFMSY(QString text)
{
    m_FMSY_Value = (! text.isEmpty()) ? text.toDouble() : -1;
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                text.toDouble(),m_NumSignificantDigits,3);
    OutputShowFMSYLE->setText(valueWithComma);
}

void
nmfOutputControls::resetTextOutputBMSY()
{
    if (m_BMSY_Value < 0) {
        return;
    }
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                  m_BMSY_Value,m_NumSignificantDigits,3);

    OutputShowBMSYLE->blockSignals(true);
    OutputShowBMSYLE->setText(valueWithComma);
    OutputShowBMSYLE->blockSignals(false);
}

void
nmfOutputControls::resetTextOutputMSY()
{
    if (m_MSY_Value < 0) {
        return;
    }
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                  m_MSY_Value,m_NumSignificantDigits,3);
    OutputShowMSYLE->setText(valueWithComma);
}

void
nmfOutputControls::resetTextOutputFMSY()
{
    if (m_FMSY_Value < 0) {
        return;
    }
    QLocale locale(QLocale::English);
    QString valueWithComma = nmfUtilsQt::checkAndCalculateWithSignificantDigits(
                                  m_FMSY_Value,m_NumSignificantDigits,3);
    OutputShowFMSYLE->setText(valueWithComma);
}

void
nmfOutputControls::resetMSYWidgets()
{
    readSettings();
    resetTextOutputBMSY();
    resetTextOutputMSY();
    resetTextOutputFMSY();
}

void
nmfOutputControls::setForecastLabels(
        std::map<QString,QStringList>& SortedForecastLabelsMap)
{
    m_SortedForecastLabelsMap = SortedForecastLabelsMap;

}

QString
nmfOutputControls::getOutputChartType()
{
    return OutputChartTypeCMB->currentText();
}

QString
nmfOutputControls::getOutputFont()
{
    return OutputFontCMB->currentText();
}

int
nmfOutputControls::getOutputFontSizeLabel()
{
    return OutputFontSizeLabelSB->value();
}

int
nmfOutputControls::getOutputFontSizeNumber()
{
    return OutputFontSizeNumberSB->value();
}

QString
nmfOutputControls::getOutputGroupType()
{
    return OutputGroupTypeCMB->currentText().replace(":","");
}

int
nmfOutputControls::getOutputLineWidthAxis()
{
    return OutputLineWidthAxesSB->value();
}

int
nmfOutputControls::getOutputLineWidthData()
{
    return OutputLineWidthDataSB->value();
}

int
nmfOutputControls::getOutputLineWidthPoint()
{
    return OutputLineWidthPointSB->value();
}

QString
nmfOutputControls::getOutputSpecies()
{
    return OutputSpeciesCMB->currentText();
}

QString
nmfOutputControls::getOutputDiagnostics()
{
    return OutputMethodsCMB->currentText();
}

int
nmfOutputControls::getOutputSpeciesIndex()
{
    return OutputSpeciesCMB->currentIndex();
}

QString
nmfOutputControls::getOutputScenario()
{
    return OutputScenariosCMB->currentText();
}

void
nmfOutputControls::setOutputParameters2d3dPB(QString chartType)
{
    OutputParameters2d3dPB->setText(chartType);
    callback_OutputParameters2d3dPB();
}

void
nmfOutputControls::setOutputScale(QString scale)
{
    OutputScaleCMB->setCurrentText(scale);
}

void
nmfOutputControls::setOutputSpecies(QString species)
{
    OutputSpeciesCMB->setCurrentText(species);
}

void
nmfOutputControls::setOutputSpeciesIndex(int index)
{
    OutputSpeciesCMB->setCurrentIndex(index);
}

void
nmfOutputControls::setOutputType(QString type)
{
    // This forces an update if the user needs to
    // refresh the same setting.
    try {
       OutputChartTypeCMB->setCurrentIndex(0);
       OutputChartTypeCMB->setCurrentText(type);
    } catch (...) {return;}

    return;
}

void
nmfOutputControls::setOutputGroupType(QString groupType)
{
    OutputGroupTypeCMB->setCurrentText(groupType);
}

void
nmfOutputControls::setOutputDiagnostics(QString method)
{
    // This forces an update if the user needs to
    // refresh the same setting.
    OutputMethodsCMB->setCurrentIndex(0);
    OutputMethodsCMB->setCurrentText(method);
}



QString
nmfOutputControls::getOutputParameter()
{
    return OutputParametersCMB->currentText();
}

QString
nmfOutputControls::getOutputScale()
{
    return OutputScaleCMB->currentText();
}

double
nmfOutputControls::getOutputBrightnessFactor()
{
    return OutputLineBrightnessSL->value()/(double)OutputLineBrightnessSL->maximum();
}

QWidget*
nmfOutputControls::getListViewViewport()
{
    return OutputSpeListLV->viewport();
}

QModelIndexList
nmfOutputControls::getListViewSelectedIndexes()
{
    return OutputSpeListLV->selectionModel()->selectedIndexes();
}

void
nmfOutputControls::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Preferences");
    m_NumSignificantDigits = settings->value("NumSignificantDigits",-1).toInt();
    settings->endGroup();

    settings->beginGroup("Output");
    if (OutputLineBrightnessSL != nullptr) {
        OutputLineBrightnessSL->setValue(settings->value("Brightness",33).toString().toInt());
    }
    setOutputScale(settings->value("Scale","Default").toString());
    OutputSpeciesCB->setChecked(settings->value("SpeciesCB",false).toBool());
    OutputTitleCB->setChecked(settings->value("TitleCB",false).toBool());
    OutputLegendCB->setChecked(settings->value("LegendCB",false).toBool());
    OutputGridLinesCB->setChecked(settings->value("GridLinesCB",false).toBool());
    settings->endGroup();

    delete settings;
}

void
nmfOutputControls::readSettingsPublish()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    OutputSpeciesCB->blockSignals(true);
    OutputTitleCB->blockSignals(true);
    OutputLegendCB->blockSignals(true);
    OutputGridLinesCB->blockSignals(true);
    OutputLineWidthAxesSB->blockSignals(true);
    OutputLineWidthDataSB->blockSignals(true);
    OutputLineWidthPointSB->blockSignals(true);
    OutputFontSizeLabelSB->blockSignals(true);
    OutputFontSizeNumberSB->blockSignals(true);
    OutputFontCMB->blockSignals(true);

    settings->beginGroup("Output");
    OutputSpeciesCB->setChecked(     settings->value("PublishShowSpecies",   false).toBool());
    OutputTitleCB->setChecked(       settings->value("PublishShowTitle",     false).toBool());
    OutputLegendCB->setChecked(      settings->value("PublishShowLegend",    false).toBool());
    OutputGridLinesCB->setChecked(   settings->value("PublishShowGridLines", false).toBool());
    OutputLineWidthAxesSB->setValue( settings->value("PublishLineWidthAxes", nmfConstantsMSSPM::PublishLineWidthAxes).toInt());
    OutputLineWidthDataSB->setValue( settings->value("PublishLineWidthData", nmfConstantsMSSPM::PublishLineWidthData).toInt());
    OutputLineWidthPointSB->setValue(settings->value("PublishLineWidthPoint",nmfConstantsMSSPM::PublishLineWidthPoint).toInt());
    OutputFontSizeLabelSB->setValue( settings->value("PublishFontSizeLabel", nmfConstantsMSSPM::PublishFontSizeLabel).toInt());
    OutputFontSizeNumberSB->setValue(settings->value("PublishFontSizeNumber",nmfConstantsMSSPM::PublishFontSizeNumber).toInt());
    OutputFontCMB->setCurrentText(   settings->value("PublishFont",          nmfConstantsMSSPM::PublishFont).toString());
    settings->endGroup();
    delete settings;

    OutputSpeciesCB->blockSignals(false);
    OutputTitleCB->blockSignals(false);
    OutputLegendCB->blockSignals(false);
    OutputGridLinesCB->blockSignals(false);
    OutputLineWidthAxesSB->blockSignals(false);
    OutputLineWidthDataSB->blockSignals(false);
    OutputLineWidthPointSB->blockSignals(false);
    OutputFontSizeLabelSB->blockSignals(false);
    OutputFontSizeNumberSB->blockSignals(false);
    OutputFontCMB->blockSignals(false);
}

void
nmfOutputControls::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Output");
    settings->setValue("Brightness", OutputLineBrightnessSL->value());
    settings->setValue("Scale",      getOutputScale());
    settings->setValue("SpeciesCB",  isCheckedOutputSpecies());
    settings->setValue("TitleCB",    isCheckedOutputTitle());
    settings->setValue("LegendCB",   isCheckedOutputLegend());
    settings->setValue("GridLinesCB",isCheckedOutputGridLines());
    settings->endGroup();

    delete settings;
}

void
nmfOutputControls::saveSettingsPublish()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Output");
    settings->setValue("PublishShowSpecies",    OutputSpeciesCB->isChecked());
    settings->setValue("PublishShowTitle",      OutputTitleCB->isChecked());
    settings->setValue("PublishShowLegend",     OutputLegendCB->isChecked());
    settings->setValue("PublishShowGridLines",  OutputGridLinesCB->isChecked());
    settings->setValue("PublishLineWidthAxes",  OutputLineWidthAxesSB->value());
    settings->setValue("PublishLineWidthData",  OutputLineWidthDataSB->value());
    settings->setValue("PublishLineWidthPoint", OutputLineWidthPointSB->value());
    settings->setValue("PublishFontSizeLabel",  OutputFontSizeLabelSB->value());
    settings->setValue("PublishFontSizeNumber", OutputFontSizeNumberSB->value());
    settings->setValue("PublishFont",           OutputFontCMB->currentText());
    settings->endGroup();

    delete settings;
}

void
nmfOutputControls::callback_UpdateDiagnosticParameterChoices()
{
    m_DatabasePtr->loadEstimatedVectorParameters(m_Logger,
                                                 m_ProjectName,
                                                 m_ModelName,
                                                 OutputParametersCMB);
}

void
nmfOutputControls::callback_CheckMSYBoxes(bool state)
{
    OutputShowBMSYCB->setChecked(state);
    OutputShowMSYCB->setChecked(state);
    OutputShowFMSYCB->setChecked(state);
}
