#include "nmfEstimationTab06.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab6::nmfEstimation_Tab6(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;
    QString   NLoptMsg;
    QString   BeesMsg;

    m_Logger       = logger;
    m_DatabasePtr  = databasePtr;
    m_FontSize     = 9;
    m_IsMonospaced = false;
    m_ProjectDir   = projectDir;
    m_ModelName.clear();
    m_ProjectName.clear();
    m_GrowthForm      = "";
    m_HarvestForm     = "";
    m_CompetitionForm = "";
    m_PredationForm   = "";
    m_EnsembleDialog  = new EnsembleDialog(tabs,m_ProjectDir);
    m_EnsembleDialog->hide();
    m_EnsembleFilename = nmfConstantsMSSPM::MultiRunFilename;

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Run Estimation"));

    Estimation_Tab6_Bees_ParametersGB          = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_Bees_ParametersGB");
    Estimation_Tab6_NL_ParametersGB            = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_NL_ParametersGB");
    Estimation_Tab6_EstParametersGB            = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_EstParametersGB");
    Estimation_Tab6_ModelAlgorithmsGB          = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_ModelAlgorithmsGB");
    Estimation_Tab6_EstimationAlgorithmCMB     = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EstimationAlgorithmCMB");
    Estimation_Tab6_ObjectiveCriterionCMB      = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ObjectiveCriterionCMB");
    Estimation_Tab6_MinimizerAlgorithmCMB      = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinimizerAlgorithmCMB");
    Estimation_Tab6_MinimizerAlgorithmLBL      = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_MinimizerAlgorithmLBL");
    Estimation_Tab6_MinimizerDetStoTypeLBL     = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_MinimizerDetStoTypeLBL");
    Estimation_Tab6_MinimizerTypeCMB           = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinimizerTypeCMB");
    Estimation_Tab6_RunTE                      = Estimation_Tabs->findChild<QTextEdit   *>("Estimation_Tab6_RunTE");
    Estimation_Tab6_RunPB                      = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_RunPB");
    Estimation_Tab6_ReloadPB                   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ReloadPB");
    Estimation_Tab6_SavePB                     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_SavePB");
    Estimation_Tab6_PrevPB                     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_PrevPB");
    Estimation_Tab6_NextPB                     = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_NextPB");
    Estimation_Tab6_FontSizeCMB                = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_FontSizeCMB");
    Estimation_Tab6_MonoCB                     = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_MonoCB");
    Estimation_Tab6_Bees_NumberOfRunsSB        = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumberOfRunsSB");
    Estimation_Tab6_Bees_NumBeesSB             = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBeesSB");
    Estimation_Tab6_Bees_NumOtherBeesSB        = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumOtherBeesSB");
    Estimation_Tab6_Bees_NumEliteSitesSB       = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteSitesSB");
    Estimation_Tab6_Bees_NumBestSitesSB        = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBestSitesSB");
    Estimation_Tab6_Bees_NumEliteBeesSB        = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteBeesSB");
    Estimation_Tab6_Bees_NumBestBeesSB         = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumOtherBeesSB");
    Estimation_Tab6_Bees_MaxGenerationsSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_MaxGenerationsSB");
    Estimation_Tab6_Bees_NeighborhoodSizeSB    = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NeighborhoodSizeSB");
    Estimation_Tab6_ScalingLBL                 = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_ScalingLBL");
    Estimation_Tab6_ScalingCMB                 = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ScalingCMB");
    Estimation_Tab6_NL_StopAfterValueCB        = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterValueCB");
    Estimation_Tab6_NL_StopAfterTimeCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterTimeCB");
    Estimation_Tab6_NL_StopAfterIterCB         = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterIterCB");
    Estimation_Tab6_NL_StopAfterValueLE        = Estimation_Tabs->findChild<QLineEdit   *>("Estimation_Tab6_NL_StopAfterValueLE");
    Estimation_Tab6_NL_StopAfterTimeSB         = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterTimeSB");
    Estimation_Tab6_NL_StopAfterIterSB         = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterIterSB");
    Estimation_Tab6_NL_StopAfterTimeUnitsCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_NL_StopAfterTimeUnitsCMB");
    Estimation_Tab6_EstimateInitialBiomassCB   = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateInitialBiomassCB");
    Estimation_Tab6_EstimateGrowthRateCB       = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateGrowthRateCB");
    Estimation_Tab6_EstimateCarryingCapacityCB = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCarryingCapacityCB");
    Estimation_Tab6_EstimateCatchabilityCB     = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCatchabilityCB");
    Estimation_Tab6_EstimateCompetitionAlphaCB = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCompetitionAlphaCB");
    Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB");
    Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB   = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB");
    Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB     = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB");
    Estimation_Tab6_EstimatePredationRhoCB                  = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimatePredationRhoCB");
    Estimation_Tab6_EstimatePredationHandlingCB             = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimatePredationHandlingCB");
    Estimation_Tab6_EstimatePredationExponentCB             = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimatePredationExponentCB");
    Estimation_Tab6_EstimateSurveyQCB                       = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EstimateSurveyQCB");
    Estimation_Tab6_EnsembleTotalRunsSB           = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_EnsembleTotalRunsSB");
    Estimation_Tab6_EnsembleAveragingAlgorithmLBL = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_EnsembleAveragingAlgorithmLBL");
    Estimation_Tab6_EnsembleAveragingAlgorithmCMB = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EnsembleAveragingAlgorithmCMB");
    Estimation_Tab6_EnsembleControlsGB            = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_EnsembleControlsGB");
    Estimation_Tab6_EnsembleRunsSetLE             = Estimation_Tabs->findChild<QLineEdit   *>("Estimation_Tab6_EnsembleRunsSetLE");
    Estimation_Tab6_EnsembleAddPB                 = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleAddPB");
    Estimation_Tab6_EnsembleRunsSB                = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_EnsembleRunsSB");
    Estimation_Tab6_EnsembleClearPB               = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleClearPB");
    Estimation_Tab6_EnsembleViewPB                = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleViewPB");
    Estimation_Tab6_EnsembleRunsWithSettingsLBL   = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_EnsembleRunsWithSettingsLBL");
    Estimation_Tab6_EnsembleRunsSetLBL            = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_EnsembleRunsSetLBL");
    Estimation_Tab6_EnsembleSetAllPB              = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleSetAllPB");
    Estimation_Tab6_EnsembleLoadPB                = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleLoadPB");
    Estimation_Tab6_EnsembleAverageByCMB          = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EnsembleAverageByCMB");
    Estimation_Tab6_EnsembleUsingByCMB            = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EnsembleUsingByCMB");
    Estimation_Tab6_EnsembleUsingAmountSB         = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_EnsembleUsingAmountSB");
    Estimation_Tab6_EnsembleUsingPctPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_EnsembleUsingPctPB");
    Estimation_Tab6_SetDeterministicCB            = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_SetDeterministicCB");
    Estimation_Tab6_EnsembleSetDeterministicCB    = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EnsembleSetDeterministicCB");
    Estimation_Tab6_AddToReviewPB                 = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_AddToReviewPB");
    Estimation_Tab6_NL_TimeUnitsLockPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_NL_TimeUnitsLockPB");

    Estimation_Tab6_AddToReviewPB->setEnabled(false);

    // Update tool tip
    BeesMsg  = "Stochastic search algorithm based on the behavior of honey bees.";
    NLoptMsg = "Open-Source NonLinear Optimization package";

    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 0, BeesMsg,        Qt::ToolTipRole);
    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 1, NLoptMsg,       Qt::ToolTipRole);

    QFont font = Estimation_Tab6_RunTE->font();
    font.setBold(false);
    Estimation_Tab6_RunTE->setFont(font);
    Estimation_Tab6_PrevPB->setText("\u25C1--");
    Estimation_Tab6_NextPB->setText("--\u25B7");

    connect(Estimation_Tab6_PrevPB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_PrevPB()));
    connect(Estimation_Tab6_NextPB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_NextPB()));
    connect(Estimation_Tab6_RunPB,                  SIGNAL(clicked()),
            this,                                   SLOT(callback_RunPB()));
    connect(Estimation_Tab6_SavePB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_SavePB()));
    connect(Estimation_Tab6_ReloadPB,               SIGNAL(clicked()),
            this,                                   SLOT(callback_LoadPB()));
    connect(Estimation_Tab6_FontSizeCMB,            SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_Estimation_Tab6_FontSizeCMB(QString)));
    connect(Estimation_Tab6_MonoCB,                 SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_Estimation_Tab6_MonoCB(int)));
    connect(Estimation_Tab6_EstimationAlgorithmCMB, SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_EstimationAlgorithmCMB(QString)));
    connect(Estimation_Tab6_ObjectiveCriterionCMB,  SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_ObjectiveCriterionCMB(QString)));
    connect(Estimation_Tab6_NL_StopAfterValueCB,    SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopValCB(int)));
    connect(Estimation_Tab6_NL_StopAfterTimeCB,     SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopAfterTimeCB(int)));
    connect(Estimation_Tab6_NL_StopAfterIterCB,     SIGNAL(stateChanged(int)),
            this,                                   SLOT(callback_StopAfterIterCB(int)));
    connect(Estimation_Tab6_MinimizerAlgorithmCMB,  SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_MinimizerAlgorithmCMB(QString)));
    connect(Estimation_Tab6_MinimizerTypeCMB,       SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_MinimizerTypeCMB(QString)));
    connect(Estimation_Tab6_EnsembleTotalRunsSB,    SIGNAL(valueChanged(int)),
            this,                                   SLOT(callback_EnsembleTotalRunsSB(int)));
    connect(Estimation_Tab6_EnsembleRunsSB,         SIGNAL(valueChanged(int)),
            this,                                   SLOT(callback_EnsembleRunsSB(int)));
    connect(Estimation_Tab6_EnsembleAddPB,          SIGNAL(clicked()),
            this,                                   SLOT(callback_EnsembleAddPB()));
    connect(Estimation_Tab6_EnsembleClearPB,        SIGNAL(clicked()),
            this,                                   SLOT(callback_EnsembleClearPB()));
    connect(Estimation_Tab6_EnsembleViewPB,         SIGNAL(clicked()),
            this,                                   SLOT(callback_EnsembleViewPB()));
    connect(Estimation_Tab6_NL_StopAfterTimeSB,     SIGNAL(valueChanged(int)),
            this,                                   SLOT(callback_StopAfterTimeSB(int)));
    connect(Estimation_Tab6_NL_StopAfterIterSB,     SIGNAL(valueChanged(int)),
            this,                                   SLOT(callback_StopAfterIterSB(int)));
    connect(Estimation_Tab6_NL_StopAfterValueLE,    SIGNAL(textChanged(QString)),
            this,                                   SLOT(callback_StopAfterValueLE(QString)));
    connect(Estimation_Tab6_EnsembleAveragingAlgorithmCMB, SIGNAL(currentTextChanged(QString)),
            this,                                     SLOT(callback_AveragingAlgorithmCMB(QString)));
    connect(Estimation_Tab6_ScalingCMB,               SIGNAL(currentTextChanged(QString)),
            this,                                     SLOT(callback_ScalingCMB(QString)));
    connect(Estimation_Tab6_EnsembleControlsGB,       SIGNAL(toggled(bool)),
            this,                                     SLOT(callback_EnsembleControlsGB(bool)));
    connect(Estimation_Tab6_EnsembleSetAllPB,         SIGNAL(clicked()),
            this,                                     SLOT(callback_EnsembleSetAllPB()));
    connect(Estimation_Tab6_EnsembleLoadPB,           SIGNAL(clicked()),
            this,                                     SLOT(callback_EnsembleLoadPB()));
    connect(Estimation_Tab6_NL_StopAfterTimeUnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                                     SLOT(callback_StopAfterTimeUnitsCMB(QString)));
    connect(Estimation_Tab6_EnsembleUsingPctPB,       SIGNAL(clicked()),
            this,                                     SLOT(callback_EnsembleUsingPctPB()));
    connect(Estimation_Tab6_EnsembleUsingByCMB,       SIGNAL(currentTextChanged(QString)),
            this,                                     SLOT(callback_EnsembleUsingAmountCMB(QString)));
    connect( Estimation_Tab6_SetDeterministicCB,      SIGNAL(stateChanged(int)),
             this,                                    SLOT(callback_SetDeterministicCB(int)));
    connect( Estimation_Tab6_EnsembleSetDeterministicCB, SIGNAL(stateChanged(int)),
             this,                                       SLOT(callback_EnsembleSetDeterministicCB(int)));            
    connect(Estimation_Tab6_AddToReviewPB,           SIGNAL(clicked()),
            this,                                    SLOT(callback_AddToReviewPB()));
    connect(Estimation_Tab6_EstimateSurveyQCB,       SIGNAL(stateChanged(int)),
            this,                                    SLOT(callback_EstimateSurveyQCB(int)));
    connect(Estimation_Tab6_NL_TimeUnitsLockPB,      SIGNAL(clicked(bool)),
            this,                                    SLOT(callback_TimeUnitsLockPB(bool)));


    // Wire up signals/slots for the Estimate Run checkboxes
    for (QCheckBox* cbox : getAllEstimateCheckboxes()) {
        connect(cbox, SIGNAL(stateChanged(int)),
                this, SLOT(callback_RefreshEstimateRunBoxes(int)));
    }

    readSettings();

    // Initialize font
    Estimation_Tab6_MonoCB->blockSignals(true);
    Estimation_Tab6_MonoCB->setChecked(m_IsMonospaced);
    Estimation_Tab6_MonoCB->blockSignals(false);
    QString fontName = (m_IsMonospaced) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    QFont defaultFont(fontName,11,QFont::Medium,false);
    setFont(defaultFont);

    // Initialize minimize function label map
    initializeDetStoMap();

    // Initialize lock button
    QIcon lockedIcon(":/icons/locked.png");
    Estimation_Tab6_NL_TimeUnitsLockPB->setIcon(lockedIcon);
    Estimation_Tab6_NL_StopAfterTimeUnitsCMB->setEnabled(false);
    Estimation_Tab6_NL_TimeUnitsLockPB->setChecked(true);

    callback_EstimationAlgorithmCMB("Bees Algorithm");
    enableRunButton(true);

    callback_MinimizerTypeCMB(Estimation_Tab6_MinimizerTypeCMB->currentText());
    callback_MinimizerAlgorithmCMB(Estimation_Tab6_MinimizerAlgorithmCMB->currentText());

}


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{
}

void
nmfEstimation_Tab6::initializeDetStoMap()
{
    // Global optimizations
    m_DetStoTypeMap["GN_ORIG_DIRECT_L"] = "(d)";
    m_DetStoTypeMap["GN_DIRECT_L"]      = "(d)";
    m_DetStoTypeMap["GN_DIRECT_L_RAND"] = "(s)";
    m_DetStoTypeMap["GN_CRS2_LM"]       = "(s)";
    m_DetStoTypeMap["GD_StoGO"]         = "(d)";

    // Local optimizations
    m_DetStoTypeMap["LN_COBYLA"]        = "(d)";
    m_DetStoTypeMap["LN_NELDERMEAD"]    = "(d)";
    m_DetStoTypeMap["LN_SBPLX"]         = "(d)";
    m_DetStoTypeMap["LD_LBFGS"]         = "(d)";
    m_DetStoTypeMap["LD_MMA"]           = "(s)";
}

void
nmfEstimation_Tab6::enableAddToReview(bool enable)
{
    Estimation_Tab6_AddToReviewPB->setEnabled(enable);
}

bool
nmfEstimation_Tab6::isAMultiRun()
{
    return (Estimation_Tab6_EnsembleControlsGB->isEnabled() &&
            Estimation_Tab6_EnsembleControlsGB->isChecked() &&
            Estimation_Tab6_EnsembleTotalRunsSB->value() > 1);
}

bool
nmfEstimation_Tab6::isStopAfterValue()
{
    return Estimation_Tab6_NL_StopAfterValueCB->isChecked();
}

bool
nmfEstimation_Tab6::isStopAfterTime()
{
    return Estimation_Tab6_NL_StopAfterTimeCB->isChecked();
}

bool
nmfEstimation_Tab6::isStopAfterIter()
{
    return Estimation_Tab6_NL_StopAfterIterCB->isChecked();
}

void
nmfEstimation_Tab6::setFont(QFont font)
{
    Estimation_Tab6_RunTE->setFont(font);
}

void
nmfEstimation_Tab6::setOutputTE(QString msg)
{
    Estimation_Tab6_RunTE->setText(msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}
void
nmfEstimation_Tab6::appendOutputTE(QString msg)
{
    QString currentMsg = Estimation_Tab6_RunTE->document()->toHtml();
    Estimation_Tab6_RunTE->setText(currentMsg+msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}

void
nmfEstimation_Tab6::clearOutputTE()
{
    Estimation_Tab6_RunTE->clear();
}

void
nmfEstimation_Tab6::callback_Estimation_Tab6_MonoCB(int isChecked)
{
    QString fontName = (isChecked == Qt::Checked) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    emit ShowRunMessage(fontName);
}

void
nmfEstimation_Tab6::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab6::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

bool
nmfEstimation_Tab6::isEstInitialBiomassEnabled()
{
    return Estimation_Tab6_EstimateInitialBiomassCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstInitialBiomassChecked()
{
    return Estimation_Tab6_EstimateInitialBiomassCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstGrowthRateEnabled()
{
    return Estimation_Tab6_EstimateGrowthRateCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstGrowthRateChecked()
{
    return Estimation_Tab6_EstimateGrowthRateCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCarryingCapacityEnabled()
{
    return Estimation_Tab6_EstimateCarryingCapacityCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstCarryingCapacityChecked()
{
    return Estimation_Tab6_EstimateCarryingCapacityCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCatchabilityEnabled()
{
    return Estimation_Tab6_EstimateCatchabilityCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstCatchabilityChecked()
{
    return Estimation_Tab6_EstimateCatchabilityCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCompetitionAlphaEnabled()
{
    return Estimation_Tab6_EstimateCompetitionAlphaCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstCompetitionAlphaChecked()
{
    return Estimation_Tab6_EstimateCompetitionAlphaCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstPredationExponentEnabled()
{
    return Estimation_Tab6_EstimatePredationExponentCB->isEnabled();
}

bool
nmfEstimation_Tab6::isEstPredationExponentChecked()
{
    return Estimation_Tab6_EstimatePredationExponentCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstSurveyQEnabled()
{
    return Estimation_Tab6_EstimateSurveyQCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstSurveyQChecked()
{
    return Estimation_Tab6_EstimateSurveyQCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstPredationRhoEnabled()
{
    return Estimation_Tab6_EstimatePredationRhoCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstPredationRhoChecked()
{
    return Estimation_Tab6_EstimatePredationRhoCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstPredationHandlingEnabled()
{
    return Estimation_Tab6_EstimatePredationHandlingCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstPredationHandlingChecked()
{
    return Estimation_Tab6_EstimatePredationHandlingCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCompetitionBetaSpeciesEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isEnabled();
}

bool
nmfEstimation_Tab6::isEstCompetitionBetaSpeciesChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCompetitionBetaGuildsEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstCompetitionBetaGuildsChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isChecked();
}

bool
nmfEstimation_Tab6::isEstCompetitionBetaGuildsGuildsEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isEnabled();
}
bool
nmfEstimation_Tab6::isEstCompetitionBetaGuildsGuildsChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isChecked();
}

bool
nmfEstimation_Tab6::isSetToDeterministic()
{
    return Estimation_Tab6_SetDeterministicCB->isChecked();
}

void
nmfEstimation_Tab6::adjustNumberOfParameters()
{
    int numberOfParameters = 0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string cmd;
    std::string errorMsg;

    if (Estimation_Tab6_EstimateInitialBiomassCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateInitialBiomassCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateGrowthRateCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateGrowthRateCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCarryingCapacityCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCarryingCapacityCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCatchabilityCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCatchabilityCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimatePredationHandlingCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimatePredationHandlingCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCompetitionAlphaCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCompetitionAlphaCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimatePredationRhoCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimatePredationRhoCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimatePredationExponentCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimatePredationExponentCB->isChecked()) ? 1 : 0;
    }
    if (Estimation_Tab6_EstimateSurveyQCB->isEnabled()) {
        numberOfParameters += (Estimation_Tab6_EstimateSurveyQCB->isChecked()) ? 1 : 0;
    }

    // Update current ModelName in Models table
    fields   = {"ProjectName","ModelName"};
    queryStr = "SELECT ProjectName,ModelName from Models WHERE ProjectName = '" + m_ProjectName +
               "' AND  ModelName = '" + m_ModelName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ModelName"].size() != 0) { // This means the model name exists so do an update
        cmd  = "UPDATE Models SET";
        cmd += "   ProjectName = '" + m_ProjectName +
               "', ModelName = '" + m_ModelName +
               "', NumberOfParameters = " + std::to_string(numberOfParameters) +
               " WHERE ProjectName = '" + m_ProjectName + "' AND ModelName = '" + m_ModelName + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::SaveSettingsConfiguration: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

}

void
nmfEstimation_Tab6::callback_RunPB()
{
    if (Estimation_Tab6_EnsembleControlsGB->isEnabled() &&
        Estimation_Tab6_EnsembleControlsGB->isChecked() &&
        Estimation_Tab6_EnsembleTotalRunsSB->value() > 1) {
        runEnsemble();
    } else {
        runEstimation();
    }
}

bool
nmfEstimation_Tab6::passRunChecks(std::string& msg)
{
    int MIN_NUMBER_RUNS_TO_AVERAGE = 2;
    int topAmount    = getEnsembleUsingAmountValue();
    int numTotalRuns = getEnsembleNumberOfTotalRuns();
    bool isUsingAll  = (getEnsembleUsingBy() == "using All");

    if (isUsingAll) {
        return true;
    }

    // Check #1
    if (isEnsembleUsingPct()) {
        if (numTotalRuns*(topAmount/100.0) < MIN_NUMBER_RUNS_TO_AVERAGE) {
            msg = "The \"using Top\" number of runs % specified must evaluate to be at least " +
                   std::to_string(MIN_NUMBER_RUNS_TO_AVERAGE) + " runs.";
            return false;
        }
    }

    // Check #2
    if (!isEnsembleUsingPct() && (topAmount > numTotalRuns))
    {
        msg = "The \"using Top\" number of runs specified must be less than the Number of Total Runs given.";
        return false;
    }

    return true;
}

void
nmfEstimation_Tab6::runEnsemble()
{
    std::string msg;

    if (! passRunChecks(msg)) {
        m_Logger->logMsg(nmfConstants::Error,msg);
        QMessageBox::critical(Estimation_Tabs, "Error", QString::fromStdString("\n"+msg+"\n"), QMessageBox::Ok);
        return;
    }

    msg = "\nOK to run the current MultiRun/Ensemble?\n";
    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Run Ensemble"),
                                                              tr(msg.c_str()), QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        runEstimation();
    } else {
        return;
    }
}

void
nmfEstimation_Tab6::runEstimation()
{
    QString msg;

    // Adjust the NumberOfParameters in case user has unchecked any
//    adjustNumberOfParameters();

    if (isStopAfterValue() || isStopAfterTime() || isStopAfterIter())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Rerun all data checks, in case user changes number of Species or Guilds and
        // then tries to re-run.
        m_Logger->logMsg(nmfConstants::Normal,"");
        m_Logger->logMsg(nmfConstants::Normal,"Start Estimation");

        // Disable all run buttons while a run is in progress
        emit EnableRunButtons(false);
        setRunButtonLabel("Running...");
        emit CheckAllEstimationTablesAndRun();
    } else {
        msg = "\nPlease select at least one Stop parameter.\n";
        QMessageBox::warning(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
    }
    QApplication::restoreOverrideCursor();
}

QString
nmfEstimation_Tab6::getEnsembleAverageBy()
{
    return Estimation_Tab6_EnsembleAverageByCMB->currentText();
}

QString
nmfEstimation_Tab6::getEnsembleAveragingAlgorithm()
{
    return Estimation_Tab6_EnsembleAveragingAlgorithmCMB->currentText();
}

QString
nmfEstimation_Tab6::createEnsembleFile()
{
    QString ensembleFilename = QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename);
    QString filePath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString fullCurrentEnsembleFilename = QDir(filePath).filePath(ensembleFilename);

    if (isAMultiRun()) {
        // Derive the new ensemble file name
        QStringList parts     = QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename).split(".");
        std::string timestamp = m_Logger->getTimestamp(nmfConstants::TimestampWithUnderscore);
        if (parts.size() == 2) {
            ensembleFilename = parts[0]+"_"+QString::fromStdString(timestamp)+"."+parts[1];
        }
        QString fullNewEnsembleFilename = QDir(filePath).filePath(ensembleFilename);
        QFile::copy(fullCurrentEnsembleFilename,fullNewEnsembleFilename);
    } else {
        ensembleFilename.clear();
    }
    m_EnsembleFilename = ensembleFilename.toStdString();

    return ensembleFilename;
}

std::string
nmfEstimation_Tab6::getEnsembleFilename()
{
    return m_EnsembleFilename;
}


std::vector<nmfStructsQt::EstimateRunBox>
nmfEstimation_Tab6::getEstimateRunBoxes()
{
    nmfStructsQt::EstimateRunBox runBox;
    int i=0;

    m_EstimateRunBoxes.clear();

    QList<QCheckBox*> allEstimateRunBoxes = getAllEstimateCheckboxes();
    for (QCheckBox* cbox : allEstimateRunBoxes) {
        if (cbox->isChecked()) {
            runBox.parameter = nmfConstantsMSSPM::EstimateCheckboxNames[i];
            runBox.state     = std::make_pair(true,true);
            m_EstimateRunBoxes.push_back(runBox);
        }
        ++i;
    }

    return m_EstimateRunBoxes;
}

void
nmfEstimation_Tab6::callback_LoadPB()
{
    if (loadWidgets()) {
        QMessageBox::information(Estimation_Tabs, "Run Settings Load",
                                 "\nRun Settings successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab6::callback_SavePB()
{
    saveSystem(true);
    enableRunButton(true);
}

void
nmfEstimation_Tab6::callback_SaveSettings()
{
    saveSystem(false);
}

void
nmfEstimation_Tab6::saveSystem(bool RunChecks)
{
    bool okToSave = true;
    std::string msg;
    std::string ModelName;

    readSettings();
    ModelName = m_ModelName;

    if (RunChecks) {
        msg = "\nOK to save current settings as: " + ModelName + " ?";
        QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Save"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes,
                                                                  QMessageBox::Yes);
        okToSave = (reply == QMessageBox::Yes);
    }
    if (okToSave) {
        m_ModelName = ModelName;
        saveSettingsConfiguration(RunChecks,ModelName);
        readSettings();
    }

    emit UpdateForecastYears();
}

bool
nmfEstimation_Tab6::saveSettingsConfiguration(bool verbose,
                                              std::string currentModelName)
{
    std::string cmd;
    std::string errorMsg;

    cmd  =  "UPDATE Models SET";
    cmd +=  "   NumberOfRuns = "          + getBeesNumberOfRuns() +
            ",  TimeStep = 1"             + // std::to_string(Estimation_Tab6_TimeStepSB->value()) +
            ",  Algorithm = '"            + getCurrentAlgorithm() +
            "', Minimizer = '"            + getCurrentMinimizer() +
            "', ObjectiveCriterion = '"   + getCurrentObjectiveCriterion() +
            "', Scaling = '"              + getCurrentScaling() +
            "', BeesMaxGenerations = "    + getBeesMaxGenerations() +
            ",  BeesNumTotal = "          + getBeesNumBees() +
            ",  BeesNumBestSites = "      + getBeesNumBestSites() +
            ",  BeesNumEliteSites = "     + getBeesNumEliteSites()  +
            ",  BeesNumElite = "          + getBeesNumEliteBees() +
            ",  BeesNumOther = "          + getBeesNumOtherBees() +
            ",  BeesNeighborhoodSize = "  + getBeesNeighborhoodSize() +
            ",  BeesNumRepetitions = "    + getBeesNumberOfRuns() +
            ",  NLoptUseStopVal = "       + std::to_string(isStopAfterValue() ? 1 : 0) +
            ",  NLoptUseStopAfterTime = " + std::to_string(isStopAfterTime()  ? 1 : 0) +
            ",  NLoptUseStopAfterIter = " + std::to_string(isStopAfterIter()  ? 1 : 0) +
            ",  NLoptStopVal = "          + std::to_string(getCurrentStopAfterValue()) +
            ",  NLoptStopAfterTime = "    + std::to_string(getCurrentStopAfterTime()) +
            ",  NLoptStopAfterIter = "    + std::to_string(getCurrentStopAfterIter()) +
            ",  NLoptNumberOfRuns = "     + std::to_string(Estimation_Tab6_EnsembleTotalRunsSB->value()) +
            "   WHERE ProjectName = '"    + m_ProjectName +
            "'  AND ModelName = '"        + currentModelName + "'";

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::SaveSettingsConfiguration: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, "Settings Updated",
                                 "\nSettings in Models table have been successfully updated.\n");
    }
    saveSettings();

    return true;
}

void
nmfEstimation_Tab6::enableRunButton(bool state)
{
    Estimation_Tab6_RunPB->setEnabled(state);
    setRunButtonLabel("Run");
}

void
nmfEstimation_Tab6::setRunButtonLabel(QString label)
{
    Estimation_Tab6_RunPB->setText(label);
}


void
nmfEstimation_Tab6::callback_MinimizerAlgorithmCMB(QString algorithm)
{
    Estimation_Tab6_MinimizerDetStoTypeLBL->setText(m_DetStoTypeMap[algorithm]);
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_MinimizerTypeCMB(QString type)
{
    QString msg;
    Estimation_Tab6_MinimizerAlgorithmCMB->clear();
    if (type.toLower() == "global") {
        msg = "<html>\
<strong><center>Global Minimizer Algorithms</center></strong>\
<br>\
MSSPM includes several minimization (i.e., optimization) algorithms from the NLopt C++ library. \
The algorithm name is formatted {G,L}{N,D}_xxxx where:<br><br>\
\"G/L\" corresponds to a global/local optimization algorithm<br>\
\"N/D\" corresponds to a derivative-free/gradient-based optimization algorithm<br>\
<br>\
The available global algorithms are as follows:<br>\
<br>\
<strong>[1] GN_ORIG_DIRECT_L - Original DIviding RECTangles Algorithm</strong><br>\
<br>\
Ref: J. M. Gablonsky and C. T. Kelley, \"A locally-biased form of the DIRECT algorithm\", J. Global Optimization, \
vol. 21 (1), p. 27-37 (2001)<br>\
<br>\
This is a deterministic-search algorithm based on systematic division \
of the search domain into smaller and smaller hyper-rectangles. The Gablonsky version makes the algorithm \
\"more biased towards local search\" so that it is more efficient for functions without too many \
local minima. This algorithm, based on the original Fortran code, has a number of hard-coded limitations \
(i.e., the number of function evaluations). There is some support for arbitrary nonlinear inequality \
constraints.<br>\
<br>\
<strong>[2] GN_DIRECT_L - Locally Biased DIviding RECTangles Algorithm</strong><br>\
<br>\
Ref: J. M. Gablonsky and C. T. Kelley, \"A locally-biased form of the DIRECT \
algorithm\", J. Global Optimization, vol. 21 (1), p. 27-37 (2001)<br>\
<br>\
This is a deterministic-search algorithm based on systematic division of the search domain into smaller and smaller \
hyper-rectangles. The Gablonsky version makes the algorithm \"more biased towards local search\" \
so that it is more efficient for functions without too many local minima.<br>\
<br>\
<strong>[3] GN_DIRECT_L_RAND - Locally Biased DIviding RECTangles Algorithm with Randomization</strong><br>\
<br>\
Ref: J. M. Gablonsky and C. T. Kelley, \"A locally-biased form of the DIRECT algorithm\", \
J. Global Optimization, vol. 21 (1), p. 27-37 (2001)<br>\
<br>\
This is a deterministic-search algorithm based \
on systematic division of the search domain into smaller and smaller hyper-rectangles. The Gablonsky \
version makes the algorithm \"more biased towards local search\" so that it is more efficient \
for functions without too many local minima. This is a slightly randomized variant of GN_DIRECT_L \
which uses some randomization to help decide which dimension to halve next in the case of near-ties.<br>\
<br>\
<strong>[4] GN_CRS2_LM - Controlled Random Search with Local Mutation</strong><br>\
<br>\
Ref: P. Kaelo and M. M. Ali, \"Some variants of the controlled random search algorithm for global optimization\", \
J. Optim. Theory Appl. 130 (2), 253-264 (2006)<br>\
<br>\
This is sometimes compared to a genetic algorithm as it starts with a random population \
of points and randomly evolves these points by heuristic rules. \
The \"evolution\" somewhat resembles a randomized Nelder-Mead algorithm.<br>\
<br>\
<strong>[5] GD_StoGO - Stochastic Global Optimization</strong><br>\
<br>\
Ref: K. Madsen, S. Zertchaninov, and A. Zilinskas, \"Global Optimization using Branch-and-Bound,\" \
unpublished (1998).<br>\
<br>\
This is a global optimization algorithm that works by systematically dividing the \
search space (which must be bound-constrained) into smaller hyper-rectangles via a \
branch-and-bound technique, and searching them by a gradient-based local-search \
algorithm (a BFGS variant), optionally including some randomness.\
</html>";
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_ORIG_DIRECT_L");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_DIRECT_L");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_DIRECT_L_RAND");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GN_CRS2_LM");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("GD_StoGO");

        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(0,
            "Global, Non-Derivative Dividing Rectangles Algorithm with Hard-Coded Limitations", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(1,
            "Global, Non-Derivative Dividing Rectangles Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(2,
            "Global, Non-Derivative Dividing Rectangles Algorithm with Randomization", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(3,
            "Global, Non-Derivative Controlled Random Search with Local Mutation Algorithm with Evolution", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(4,
            "Global, Gradient-Based Stochastic Search followed by a Local Gradient-based Algorithm", Qt::ToolTipRole);
    } else if (type.toLower() == "local") {
        msg ="<html>\
<strong><center>Local Minimizer Algorithms</center></strong><br>\
These are local NLopt optimization functions available to the user in MSSPM.<br>\
<br>\
<strong>[1] LN_COBYLA - Constrained Optimization BY Linear Approximations</strong><br>\
<br>\
Ref: M.J.D. Powell, \"A direct search optimization method that models \
the objective and constraint functions by linear interpolation,\" in \
Advances in Optimization and Numerical Analysis, (Kluwer Academic: Dordrecht, 1994), p. 51-67<br>\
<br>\
It constructs successive linear approximations of the objective function \
and constraints via a simplex of n+1 points (in n dimensions), and \
optimizes these approximations in a trust region at each step.<br>\
<br>\
<strong>[2] LN_NELDERMEAD - Nelder-Mead Simplex</strong><br>\
<br>\
Ref: J. A. Nelder and R. Mead, \"A simplex method for function minimization,\" \
The Computer Journal 7, p. 308-313 (1965)<br>\
<br>\
An implementation of almost the original Nelder-Mead simplex algorithm. N.B. It\
does fail to converge at all for some functions and examples may be constructed in \
which it converges to a point that is not a local minimum.<br>\
<br>\
<strong>[3] LN_SBPLX - Subplex Nelder-Mead Variant Using Sequence of Subspaces</strong><br>\
<br>\
Ref: T. Rowan, \"Functional Stability Analysis of Numerical Algorithms\", \
Ph.D. thesis, Department of Computer Sciences, University of Texas at Austin, 1990<br>\
<br>\
Subplex (a variant of Nelder-Mead using a sequence of subspaces) \
is claimed to be much more efficient and robust than the original Nelder-Mead, \
while retaining the latter's facility with discontinuous objectives. Implementation \
exists fo explicit support for bound constraints (via the method in [M. J. \
Box, \"A new method of constrained optimization and a comparison with other methods,\" \
Computer J. 8 (1), 42-52 (1965)] as \
described above). This seems to be a big improvement in the case where the optimum \
lies against one of the constraints. <br>\
<br>\
<strong>[4] LD_LBFGS - Low-storage BFGS (Broyden-Fletcher-Goldfarb-Shanno)</strong><br>\
<br>\
Ref: J. Nocedal, \"Updating quasi-Newton matrices with limited storage,\" Math. Comput. \
35, 773-782 (1980)<br>\
<br>\
This algorithm is based on a Fortran implementation of the low-storage BFGS algorithm \
written by Prof. Ladislav Luksan.<br>\
<br>\
<strong>[5] LD_MMA - Method of Moving Asymptotes</strong><br>\
<br>\
Ref: Krister Svanberg, \"A class of globally convergent optimization methods \
based on conservative convex separable approximations,\" SIAM J. Optim. 12 (2), \
p. 555-573 (2002)<br>\
<br>\
This is a globally convergent algorithm which does not mean it converges to the \
global optimum; it means that it is guaranteed to converge to some local minimum \
from any feasible starting point. At each point x, MMA forms a local approximation \
using the gradient of f and the constraint functions, plus a quadratic \"penalty\" \
term to make the approximations \"conservative\" (upper bounds for the exact functions).\
</html>";
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("LN_COBYLA");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("LN_NELDERMEAD");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("LN_SBPLX");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("LD_LBFGS");
        Estimation_Tab6_MinimizerAlgorithmCMB->addItem("LD_MMA");

        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(0,
            "Local, Non-Derivative Constrained Optimization BY Linear Approximations Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(1,
            "Local, Non-Derivative Nelder-Mead Simplex Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(2,
            "Local, Non-Derivative Nelder-Mead Variant Using a Sequence of Subspaces Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(3,
            "Local, Gradient-Based Low-Storage BFGS (Broyden-Fletcher-Goldfarb-Shanno) Algorithm", Qt::ToolTipRole);
        Estimation_Tab6_MinimizerAlgorithmCMB->setItemData(4,
            "Local, Gradient-Based Method of Moving Asymptotes Algorithm", Qt::ToolTipRole);

    }
    Estimation_Tab6_MinimizerAlgorithmCMB->setWhatsThis(msg);
    Estimation_Tab6_MinimizerAlgorithmLBL->setWhatsThis(msg);
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_EstimationAlgorithmCMB(QString algorithm)
{
    algorithm = Estimation_Tab6_EstimationAlgorithmCMB->currentText();

    bool isBeesAlgorithm  = (algorithm == "Bees Algorithm");
    bool isNLoptAlgorithm = (algorithm == "NLopt Algorithm");
    bool enableMinimizer  = isNLoptAlgorithm;

    if (algorithm.isEmpty()) {
        return;
    }


    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();
    Estimation_Tab6_MinimizerAlgorithmCMB->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerAlgorithmLBL->setEnabled(enableMinimizer);    
    Estimation_Tab6_MinimizerDetStoTypeLBL->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerTypeCMB->setEnabled(enableMinimizer);

    // Enable all ObjectiveCriterion
    for (int i=0; i<Estimation_Tab6_ObjectiveCriterionCMB->count(); ++i)
        Estimation_Tab6_ObjectiveCriterionCMB->setItemData(i, 33, Qt::UserRole - 1 ); // Hack to enable all qcombobox items temporarily

    if (isBeesAlgorithm) {
        Estimation_Tab6_Bees_ParametersGB->show();
//      Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText("Least Squares"); // override what was in the table
    }  else if (isNLoptAlgorithm) {
        Estimation_Tab6_NL_ParametersGB->show();
//      Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText("Least Squares"); // override what was in the table
    }

    enableRunButton(false);

    emit SetAlgorithm(algorithm);
}

void
nmfEstimation_Tab6::callback_ObjectiveCriterionCMB(QString objCrit)
{
    bool isMaximumLikelihood = (objCrit == "Maximum Likelihood");

    Estimation_Tab6_ScalingCMB->clear();
    Estimation_Tab6_ScalingLBL->setEnabled(! isMaximumLikelihood);
    Estimation_Tab6_ScalingCMB->setEnabled(! isMaximumLikelihood);
    if (! isMaximumLikelihood) {
        Estimation_Tab6_ScalingCMB->addItem("Mean");
        Estimation_Tab6_ScalingCMB->addItem("Min Max");
    }
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_ScalingCMB(QString scalingAlgorithm)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_AveragingAlgorithmCMB(QString averagingAlgorithm)
{
    //    enableRunButton(false);
}

std::string
nmfEstimation_Tab6::getBeesMaxGenerations()
{
    return std::to_string(Estimation_Tab6_Bees_MaxGenerationsSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumBeesSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumBestSites()
{
    return std::to_string(Estimation_Tab6_Bees_NumBestSitesSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumEliteSites()
{
    return std::to_string(Estimation_Tab6_Bees_NumEliteSitesSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumEliteBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumEliteBeesSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumOtherBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumOtherBeesSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNeighborhoodSize()
{
    return std::to_string(Estimation_Tab6_Bees_NeighborhoodSizeSB->value());
}

std::string
nmfEstimation_Tab6::getBeesNumberOfRuns()
{
    return std::to_string(Estimation_Tab6_Bees_NumberOfRunsSB->value());
}

std::string
nmfEstimation_Tab6::getCurrentAlgorithm()
{
    return Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentMinimizer()
{
    if (Estimation_Tab6_MinimizerAlgorithmCMB->isEnabled()) {
        return Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString();
    } else {
        return "";
    }
}

std::string
nmfEstimation_Tab6::getCurrentObjectiveCriterion()
{
    return Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentScaling()
{
    if (Estimation_Tab6_ScalingCMB->isEnabled()) {
        return Estimation_Tab6_ScalingCMB->currentText().toStdString();
    } else {
        return "";
    }
}

double
nmfEstimation_Tab6::getCurrentStopAfterValue()
{
    return Estimation_Tab6_NL_StopAfterValueLE->text().toDouble();
}


void
nmfEstimation_Tab6::setCurrentTimeUnits(QString units)
{
    Estimation_Tab6_NL_StopAfterTimeUnitsCMB->setCurrentText(units);
}


QString
nmfEstimation_Tab6::getCurrentTimeUnits()
{
    return Estimation_Tab6_NL_StopAfterTimeUnitsCMB->currentText();
}

int
nmfEstimation_Tab6::convertToAppropriateUnits(int seconds)
{
    int retv = seconds;

    if (seconds % 3600 == 0) {
        retv = seconds/3600;
        setCurrentTimeUnits("hour");
    } else if (seconds % 60 == 0) {
        retv = seconds/60;
        setCurrentTimeUnits("min");
    }
    return retv;
}


int
nmfEstimation_Tab6::getCurrentStopAfterTime()
{
    int timeInSeconds = Estimation_Tab6_NL_StopAfterTimeSB->value();
    QString timeUnits = getCurrentTimeUnits();

    if (timeUnits == "min") {
        timeInSeconds *= 60;
    } else if (timeUnits == "hour") {
        timeInSeconds *= 3600;
    }

    return timeInSeconds;
}

int
nmfEstimation_Tab6::getCurrentStopAfterIter()
{
    return Estimation_Tab6_NL_StopAfterIterSB->value();
}

void
nmfEstimation_Tab6::callback_EnsembleTotalRunsSB(int value)
{
    if (! Estimation_Tab6_EnsembleControlsGB->isChecked()) {
        return;
    }

    bool isMultiRun = (value > 1);

    Estimation_Tab6_EnsembleAveragingAlgorithmLBL->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleAveragingAlgorithmCMB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleRunsSetLE->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleRunsSB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleAddPB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleViewPB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleClearPB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleRunsWithSettingsLBL->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleRunsSetLBL->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleSetAllPB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleAverageByCMB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleUsingByCMB->setEnabled(isMultiRun);
    Estimation_Tab6_EnsembleSetDeterministicCB->setEnabled(isMultiRun);
    bool showUsingWidgets = (getEnsembleUsingBy() == "using Top:" && isMultiRun);
    Estimation_Tab6_EnsembleUsingAmountSB->setEnabled(showUsingWidgets);
    Estimation_Tab6_EnsembleUsingPctPB->setEnabled(showUsingWidgets);
    if (! isMultiRun) {
        Estimation_Tab6_EnsembleRunsSetLE->setText("0");
        Estimation_Tab6_EnsembleRunsSB->setValue(1);
    }
    Estimation_Tab6_EstParametersGB->setEnabled(isMultiRun);
    Estimation_Tab6_NL_ParametersGB->setEnabled(isMultiRun);
    Estimation_Tab6_ModelAlgorithmsGB->setEnabled(isMultiRun);

    enableRunButton(false);
}

int
nmfEstimation_Tab6::getEnsembleNumberOfTotalRuns()
{
    return Estimation_Tab6_EnsembleTotalRunsSB->value();
}

QString
nmfEstimation_Tab6::getEnsembleUsingBy()
{
    return Estimation_Tab6_EnsembleUsingByCMB->currentText();
}

int
nmfEstimation_Tab6::getEnsembleUsingAmountValue()
{
    return Estimation_Tab6_EnsembleUsingAmountSB->value();
}

void
nmfEstimation_Tab6::callback_EnsembleRunsSB(int value)
{
    enableRunButton(false);
}

bool
nmfEstimation_Tab6::addToMultiRunFile(const int& numRunsToAdd,
                                      const int& currentNumberOfRuns,
                                      const int& totalNumberOfRunsDesired,
                                      QString& fullPath)
{
    QString msg;
    if (fullPath.isEmpty()) { // Means it's not a Mohn's Rho multi-run
        fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
        fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename));
    }
    QFile file(fullPath);

    if (numRunsToAdd+currentNumberOfRuns > totalNumberOfRunsDesired) {
        QMessageBox::warning(Estimation_Tabs, "Multi-Run/Ensemble Complete",
                             "\nMulti-Run/Ensemble runs have already been set. Click Clear if you want to clear the current runs in order to restart the Ensemble run building.\n",
                             QMessageBox::Ok);
        return false;
    }

    QList<QCheckBox* > EstimateCheckBoxes;
    EstimateCheckBoxes = getAllEstimateCheckboxes();
    int NumEstimatedCheckboxes = EstimateCheckBoxes.size();

//    if ((currentNumberOfRuns == 0) && (file.exists())) {
//        if (! queryUserIfOkToClearMultiRunFile()) {
//            Estimation_Tab6_EnsembleRunsSetLE->setText("0");
//            return;
//        }
//        Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(numRunsToAdd));
//    }

    if (! file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << "Num Runs,Objective Criterion,Algorithm,Minimizer,Scaling";
            stream << ",Bees Max Generations,Bees Total Num,Bees Num Best Sites,Bees Num Elite Sites";
            stream << ",Bees Num Elite,Bees Num Other,Bees Neighborhood Size(%),Bees Num of SubRuns";
            stream << ",NL Stop Condition,NL Checked,Value";
            stream << ",NL Stop Condition,NL Checked,NL Num Seconds";
            stream << ",NL Stop Condition,NL Checked,NL Num Evals";
            for (int i=0; i<NumEstimatedCheckboxes; ++i) {
                stream << ",Est Parameter,Enabled,Checked";
            }
            stream << "\n";
            file.close();
        }
    }

    if (file.open(QIODevice::Append)) {
        QTextStream stream(&file);

        stream << numRunsToAdd;
        stream << "," << QString::fromStdString(getCurrentObjectiveCriterion());
        stream << "," << QString::fromStdString(getCurrentAlgorithm());
        stream << "," << QString::fromStdString(getCurrentMinimizer());
        stream << "," << QString::fromStdString(getCurrentScaling());
        stream << "," << QString::fromStdString(getBeesMaxGenerations());
        stream << "," << QString::fromStdString(getBeesNumBees());
        stream << "," << QString::fromStdString(getBeesNumBestSites());
        stream << "," << QString::fromStdString(getBeesNumEliteSites());
        stream << "," << QString::fromStdString(getBeesNumEliteBees());
        stream << "," << QString::fromStdString(getBeesNumOtherBees());
        stream << "," << QString::fromStdString(getBeesNeighborhoodSize());
        stream << "," << QString::fromStdString(getBeesNumberOfRuns());
        stream << ",StopAfterValue," << isStopAfterValue() << "," << getCurrentStopAfterValue();
        stream << ",StopAfterTime,"  << isStopAfterTime()  << "," << getCurrentStopAfterTime();
        stream << ",StopAfterIter,"  << isStopAfterIter()  << "," << getCurrentStopAfterIter();
        for (int i=0; i<NumEstimatedCheckboxes; ++i) {
            stream << "," << QString::fromStdString(nmfConstantsMSSPM::EstimateCheckboxNames[i])
                   << "," << EstimateCheckBoxes[i]->isEnabled()
                   << "," << EstimateCheckBoxes[i]->isChecked();
        }

        stream << "\n";

        file.close();
    }

    return true;

}

void
nmfEstimation_Tab6::clearWidgets()
{
    std::cout << "Note: nmfEstimation_Tab6::clearWidgets TBD" << std::endl;
}

void
nmfEstimation_Tab6::callback_EstimateSurveyQCB(int state)
{
    emit DimScalarBiomassControls(state == Qt::Checked);

}

void
nmfEstimation_Tab6::callback_EnableSurveyQ(const QString biomassType,
                                           const bool enable,
                                           const bool checked)
{
    Estimation_Tab6_EstimateSurveyQCB->setEnabled(enable);
    Estimation_Tab6_EstimateSurveyQCB->blockSignals(true);
    Estimation_Tab6_EstimateSurveyQCB->setChecked(checked);
    Estimation_Tab6_EstimateSurveyQCB->blockSignals(false);
    if (biomassType == "Relative") {
        emit DimScalarBiomassControls(Estimation_Tab6_EstimateSurveyQCB->isChecked());
    } else {
        emit DimScalarBiomassControls(true);
        Estimation_Tab6_EstimateSurveyQCB->setEnabled(false);
    }
}

void
nmfEstimation_Tab6::callback_EnsembleAddPB()
{
    int numRunsToAdd             = Estimation_Tab6_EnsembleRunsSB->value();
    int currentNumberOfRuns      = Estimation_Tab6_EnsembleRunsSetLE->text().toInt();
    int totalNumberOfRunsDesired = Estimation_Tab6_EnsembleTotalRunsSB->value();
    int tmpSum                   = numRunsToAdd+currentNumberOfRuns;
    QString fullPath = "";

    enableRunButton(false);

    if (tmpSum <= totalNumberOfRunsDesired) {
        Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(tmpSum));
        addToMultiRunFile(numRunsToAdd,currentNumberOfRuns,totalNumberOfRunsDesired,fullPath);
        m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleFilename));
    } else {
        QMessageBox::warning(Estimation_Tabs, "Multi-Run/Ensemble Complete",
                             "\nAttempting to add too many runs into the Multi-Run/Ensemble file.\n",
                             QMessageBox::Ok);
    }
    if (tmpSum == totalNumberOfRunsDesired) {
        QMessageBox::information(Estimation_Tabs, "Multi-Run/Ensemble Setup Complete",
                                 "\nAll runs in current Multi-Run/Ensemble have now been set up.\n",
                                 QMessageBox::Ok);
        enableEnsembleWidgets(false);
    }
}

QString
nmfEstimation_Tab6::getMultiRunColumnData(int col)
{
    return m_EnsembleDialog->getColumnData(col);
}


void
nmfEstimation_Tab6::callback_EnsembleClearPB()
{
    if (queryUserIfOkToClearMultiRunFile()) {
        clearEnsembleFile();
    }
}

void
nmfEstimation_Tab6::clearEnsembleFile()
{
    Estimation_Tab6_EnsembleRunsSetLE->setText("0");

    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename));
    QFile file(fullPath);
    file.remove();

    m_EnsembleDialog->clear();
    enableEnsembleWidgets(true);
    m_EnsembleFilename = nmfConstantsMSSPM::MultiRunFilename;
}

void
nmfEstimation_Tab6::clearMohnsRhoFile()
{
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::MohnsRhoRunFilename));
    QFile file(fullPath);
    file.remove();
}

void
nmfEstimation_Tab6::callback_EnsembleViewPB()
{
    if (m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleFilename))) {
        m_EnsembleDialog->show();
    }
}

bool
nmfEstimation_Tab6::queryUserIfOkToClearMultiRunFile()
{
    QString msg = "\nOK to clear existing Multi Run Parameter file?\n";

    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Clear"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);

    return (reply == QMessageBox::Yes);
}


void
nmfEstimation_Tab6::callback_StopValCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterValueLE->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterTimeCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterTimeSB->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterIterCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterIterSB->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterTimeUnitsCMB(QString units)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterTimeSB(int value)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterIterSB(int value)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab6::callback_StopAfterValueLE(QString value)
{
    enableRunButton(false);
}


void
nmfEstimation_Tab6::callback_RefreshEstimateRunBoxes(int unused)
{
    nmfStructsQt::EstimateRunBox runBox;
    m_EstimateRunBoxes.clear();
    QList<QCheckBox*> allEstimateRunBoxes = getAllEstimateCheckboxes();
    int i=0;
    for (QCheckBox* cbox : allEstimateRunBoxes) {
        if (cbox->isChecked()) {
            runBox.parameter = nmfConstantsMSSPM::EstimateCheckboxNames[i];
            runBox.state     = std::make_pair(true,true);
            m_EstimateRunBoxes.push_back(runBox);
        }
        ++i;
    }
}

QList<QCheckBox* >
nmfEstimation_Tab6::getAllEstimateCheckboxes()
{
    QList<QCheckBox*> AllCheckboxes = {
        Estimation_Tab6_EstimateInitialBiomassCB,
        Estimation_Tab6_EstimateGrowthRateCB,
        Estimation_Tab6_EstimateCarryingCapacityCB,
        Estimation_Tab6_EstimateCatchabilityCB,
        Estimation_Tab6_EstimatePredationHandlingCB,
        Estimation_Tab6_EstimateCompetitionAlphaCB,
        Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB,
        Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB,
        Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB,
        Estimation_Tab6_EstimatePredationRhoCB,
        Estimation_Tab6_EstimatePredationExponentCB,
        Estimation_Tab6_EstimateSurveyQCB
    };

    return AllCheckboxes;
}

void
nmfEstimation_Tab6::activateCheckBox(QCheckBox* cbox, std::pair<bool,bool> state)
{
    cbox->setEnabled(state.first);
    cbox->setChecked(state.second);
}

void
nmfEstimation_Tab6::callback_SetEstimateRunCheckboxes(
        std::vector<nmfStructsQt::EstimateRunBox> EstimateRunBoxes)
{
    m_EstimateRunBoxes = EstimateRunBoxes;

    QList<QCheckBox*> allEstimateRunBoxes = getAllEstimateCheckboxes();
    for (QCheckBox* cbox : allEstimateRunBoxes) {
        activateCheckBox(cbox,std::make_pair(false,false));
    }

    for (nmfStructsQt::EstimateRunBox runBox : EstimateRunBoxes) {
        if (runBox.parameter == "InitBiomass") {
            activateCheckBox(Estimation_Tab6_EstimateInitialBiomassCB,runBox.state);
        } else if (runBox.parameter == "GrowthRate") {
            activateCheckBox(Estimation_Tab6_EstimateGrowthRateCB,runBox.state);
        } else if (runBox.parameter == "CarryingCapacity") {
            activateCheckBox(Estimation_Tab6_EstimateCarryingCapacityCB,runBox.state);
        } else if (runBox.parameter == "Catchability") {
            activateCheckBox(Estimation_Tab6_EstimateCatchabilityCB,runBox.state);
        } else if (runBox.parameter == "CompetitionAlpha") {
            activateCheckBox(Estimation_Tab6_EstimateCompetitionAlphaCB,runBox.state);
        } else if (runBox.parameter == "CompetitionBetaSpeciesSpecies") {
            activateCheckBox(Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB,runBox.state);
        } else if (runBox.parameter == "CompetitionBetaGuildSpecies") {
            activateCheckBox(Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB,runBox.state);
        } else if (runBox.parameter == "CompetitionBetaGuildGuild") {
            activateCheckBox(Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB,runBox.state);
        } else if (runBox.parameter == "PredationRho") {
            activateCheckBox(Estimation_Tab6_EstimatePredationRhoCB,runBox.state);
        } else if (runBox.parameter == "PredationExponent") {
            activateCheckBox(Estimation_Tab6_EstimatePredationExponentCB,runBox.state);
        } else if (runBox.parameter == "PredationHandling") {
            activateCheckBox(Estimation_Tab6_EstimatePredationHandlingCB,runBox.state);
        } else if (runBox.parameter == "SurveyQ") {
            activateCheckBox(Estimation_Tab6_EstimateSurveyQCB,runBox.state);
        }
    }
}

void
nmfEstimation_Tab6::refreshMsg(QFont font, QString msg)
{
    readSettings();

    clearOutputTE();
    Estimation_Tab6_RunTE->setFont(font);
    setOutputTE(msg);
}

void
nmfEstimation_Tab6::callback_Estimation_Tab6_FontSizeCMB(QString theFontSize)
{
    m_FontSize = theFontSize.toInt();

    QTextCursor cursor = Estimation_Tab6_RunTE->textCursor();
    Estimation_Tab6_RunTE->selectAll();
    Estimation_Tab6_RunTE->setFontPointSize(m_FontSize);
    Estimation_Tab6_RunTE->setTextCursor( cursor );
}

void
nmfEstimation_Tab6::callback_EnsembleControlsGB(bool isChecked)
{
    callback_EnsembleTotalRunsSB(Estimation_Tab6_EnsembleTotalRunsSB->value());
    enableEnsembleWidgets(! isChecked);

    if (isChecked) {
        loadEnsembleFile(QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename),
                         nmfConstantsMSSPM::VerboseOff);
    }
}


void
nmfEstimation_Tab6::callback_EnsembleUsingAmountCMB(QString value)
{
    if (value == "using All") {
        Estimation_Tab6_EnsembleUsingAmountSB->setEnabled(false);
        Estimation_Tab6_EnsembleUsingPctPB->setEnabled(false);
    } else if (value == "using Top:") {
        Estimation_Tab6_EnsembleUsingAmountSB->setEnabled(true);
        Estimation_Tab6_EnsembleUsingPctPB->setEnabled(true);
    }
}

void
nmfEstimation_Tab6::callback_SetDeterministicCB(int state)
{
    Estimation_Tab6_EnsembleSetDeterministicCB->blockSignals(true);
    Estimation_Tab6_EnsembleSetDeterministicCB->setChecked(state==Qt::Checked);
    Estimation_Tab6_EnsembleSetDeterministicCB->blockSignals(false);
}

void
nmfEstimation_Tab6::callback_EnsembleSetDeterministicCB(int state)
{
    Estimation_Tab6_SetDeterministicCB->blockSignals(true);
    Estimation_Tab6_SetDeterministicCB->setChecked(state==Qt::Checked);
    Estimation_Tab6_SetDeterministicCB->blockSignals(false);
}

void
nmfEstimation_Tab6::callback_AddToReviewPB()
{
    emit AddToReview();
}

bool
nmfEstimation_Tab6::isEnsembleUsingPct()
{
    return (Estimation_Tab6_EnsembleUsingPctPB->text() == "%");
}

void
nmfEstimation_Tab6::callback_EnsembleUsingPctPB()
{
    QString label = Estimation_Tab6_EnsembleUsingPctPB->text();
    if (label == "%") {
        Estimation_Tab6_EnsembleUsingPctPB->setText("");
    } else {
        Estimation_Tab6_EnsembleUsingPctPB->setText("%");
    }
}

void
nmfEstimation_Tab6::callback_EnsembleLoadPB()
{
    loadEnsembleFile(QString::fromStdString(nmfConstantsMSSPM::MultiRunFilename),
                     nmfConstantsMSSPM::VerboseOn);
}


bool
nmfEstimation_Tab6::loadEnsembleFile(QString ensembleFilename,
                                     const bool& verbose)
{
std::cout << "Loading: " << ensembleFilename.toStdString() << std::endl;
    bool retv = true;
    int TotalNumRuns = 0;
    QString msg;
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(ensembleFilename);
    QFile file(fullPath);

    m_EnsembleFilename = ensembleFilename.toStdString();

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream inStream(&file);
        while(!inStream.atEnd()) {
            QString line = inStream.readLine();
            QStringList fields = line.split(",");
            TotalNumRuns += fields[0].toInt();
        }
        file.close();
        setEnsembleRuns(TotalNumRuns);
        setEnsembleRunsSet(TotalNumRuns);
        saveSystem(false);
        enableEnsembleWidgets(false);
        m_EnsembleDialog->loadWidgets(ensembleFilename);
    } else {
        if (verbose) {
            QMessageBox::warning(Estimation_Tabs, "Warning",
                                 "\nNo previous Multi-Run/Ensemble file found to load.\n",
                                 QMessageBox::Ok);
        }
        retv = false;
    }
    return retv;
}

void
nmfEstimation_Tab6::enableEnsembleWidgets(bool enable)
{
    enableRunButton(! enable);
    Estimation_Tab6_NL_ParametersGB->setEnabled(enable);
    Estimation_Tab6_Bees_ParametersGB->setEnabled(enable);
    Estimation_Tab6_EstParametersGB->setEnabled(enable);
    Estimation_Tab6_ModelAlgorithmsGB->setEnabled(enable);
}

void
nmfEstimation_Tab6::enableMultiRunControls(bool enable)
{
    Estimation_Tab6_EnsembleControlsGB->setChecked(enable);
}

void
nmfEstimation_Tab6::setEnsembleRuns(int value)
{
   Estimation_Tab6_EnsembleTotalRunsSB->setValue(value);
}

void
nmfEstimation_Tab6::setEnsembleRunsSet(int value)
{
   Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(value));
}

void
nmfEstimation_Tab6::setEnsembleAverageBy(QString averageBy)
{
    Estimation_Tab6_EnsembleAverageByCMB->setCurrentText(averageBy);
}

void
nmfEstimation_Tab6::setEnsembleAveragingAlgorithm(QString aveAlg)
{
    Estimation_Tab6_EnsembleAveragingAlgorithmCMB->setCurrentText(aveAlg);
}

void
nmfEstimation_Tab6::setEnsembleUsingBy(QString usingBy)
{
    Estimation_Tab6_EnsembleUsingByCMB->setCurrentText(usingBy);
}

void
nmfEstimation_Tab6::setEnsembleUsingAmountValue(QString usingAmount)
{
    Estimation_Tab6_EnsembleUsingAmountSB->setValue(usingAmount.toInt());
}

void
nmfEstimation_Tab6::setEnsembleUsingPct(bool isUsingPct)
{
    QString text = (isUsingPct) ? "%" : "";
    Estimation_Tab6_EnsembleUsingPctPB->setText(text);
}


void
nmfEstimation_Tab6::callback_EnsembleSetAllPB()
{
    int numRunsToAdd             = Estimation_Tab6_EnsembleTotalRunsSB->value();
    int currentNumberOfRuns      = Estimation_Tab6_EnsembleRunsSetLE->text().toInt();
    int totalNumberOfRunsDesired = numRunsToAdd;
    QString fullPath = "";

    if (addToMultiRunFile(numRunsToAdd,currentNumberOfRuns,totalNumberOfRunsDesired,fullPath)) {
        Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(Estimation_Tab6_EnsembleTotalRunsSB->value()));
        m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleFilename));
        if (numRunsToAdd+currentNumberOfRuns == totalNumberOfRunsDesired) {
            QString msg = "\nSaved Mult-Run Parameter File to:\n\n" + fullPath + "\n";
            QMessageBox::information(Estimation_Tabs, "MultiRun Parameter File Save",
                                     msg,QMessageBox::Ok);
            enableEnsembleWidgets(false);
        }
    }
}

bool
nmfEstimation_Tab6::loadWidgets()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string algorithm = getCurrentAlgorithm();
    QString objectiveCriterion;

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::loadWidgets()");

    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();

    if (algorithm == "Bees Algorithm") {
        Estimation_Tab6_Bees_ParametersGB->show();
        Estimation_Tab6_Bees_ParametersGB->setVisible(true);
    } else if (algorithm == "NLopt Algorithm") {
        Estimation_Tab6_NL_ParametersGB->show();
        Estimation_Tab6_NL_ParametersGB->setVisible(true);
    }

    readSettings();

    if (m_ModelName.empty())
        return false;

    fields     = {"ProjectName","ModelName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","Algorithm","Minimizer",
                  "ObjectiveCriterion","Scaling","GAGenerations","GAPopulationSize",
                  "GAMutationRate","GAConvergence","BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter","NLoptNumberOfRuns"};
    queryStr   = "SELECT ProjectName,ModelName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter,NLoptNumberOfRuns ";
    queryStr  += "FROM Models WHERE ProjectName = '" + m_ProjectName + "' AND ModelName = '";
    queryStr  += m_ModelName + "'";
    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["ModelName"].size();
    if (NumRecords == 0) {
        m_Logger->logMsg(nmfConstants::Warning,"No records found in Models");
        return false;
    }

    m_GrowthForm      = dataMap["GrowthForm"][0];
    m_HarvestForm     = dataMap["HarvestForm"][0];
    m_CompetitionForm = dataMap["WithinGuildCompetitionForm"][0];
    m_PredationForm   = dataMap["PredationForm"][0];

    objectiveCriterion = QString::fromStdString(dataMap["ObjectiveCriterion"][0]);
    Estimation_Tab6_Bees_NumberOfRunsSB->setValue(std::stoi(dataMap["NumberOfRuns"][0]));
    Estimation_Tab6_EstimationAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Algorithm"][0]));
    Estimation_Tab6_Bees_NumBeesSB->setValue(std::stoi(dataMap["BeesNumTotal"][0]));
    Estimation_Tab6_Bees_NumEliteSitesSB->setValue(std::stoi(dataMap["BeesNumEliteSites"][0]));
    Estimation_Tab6_Bees_NumBestSitesSB->setValue(std::stoi(dataMap["BeesNumBestSites"][0]));
    Estimation_Tab6_Bees_NumEliteBeesSB->setValue(std::stoi(dataMap["BeesNumElite"][0]));
    Estimation_Tab6_Bees_NumBestBeesSB->setValue(std::stoi(dataMap["BeesNumOther"][0]));
    Estimation_Tab6_Bees_NumberOfRunsSB->setValue(std::stoi(dataMap["BeesNumRepetitions"][0]));
    Estimation_Tab6_Bees_MaxGenerationsSB->setValue(std::stoi(dataMap["BeesMaxGenerations"][0]));
    Estimation_Tab6_Bees_NeighborhoodSizeSB->setValue(std::stof(dataMap["BeesNeighborhoodSize"][0]));
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(objectiveCriterion);
    Estimation_Tab6_ScalingCMB->setCurrentText(QString::fromStdString(dataMap["Scaling"][0]));
    Estimation_Tab6_NL_StopAfterValueCB->setChecked(dataMap["NLoptUseStopVal"][0] == "1");
    Estimation_Tab6_NL_StopAfterTimeCB->setChecked(dataMap["NLoptUseStopAfterTime"][0] == "1");
    Estimation_Tab6_NL_StopAfterIterCB->setChecked(dataMap["NLoptUseStopAfterIter"][0] == "1");
    Estimation_Tab6_NL_StopAfterValueLE->setText(QString::fromStdString(dataMap["NLoptStopVal"][0]));
    Estimation_Tab6_NL_StopAfterTimeSB->setValue(convertToAppropriateUnits(std::stoi(dataMap["NLoptStopAfterTime"][0])));
    Estimation_Tab6_NL_StopAfterIterSB->setValue(std::stoi(dataMap["NLoptStopAfterIter"][0]));
//  Estimation_Tab6_EnsembleTotalRunsSB->setValue(std::stoi(dataMap["NLoptNumberOfRuns"][0]));

    callback_EnsembleTotalRunsSB(std::stoi(dataMap["NLoptNumberOfRuns"][0]));
    callback_EstimationAlgorithmCMB(QString::fromStdString(dataMap["Algorithm"][0]));

    Estimation_Tab6_MinimizerAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Minimizer"][0]));

    callback_ObjectiveCriterionCMB(objectiveCriterion);

    // RSK Hack to set the CMB correctly. Remove after implementing all of the disabled items in the 3 algorithm CMB's.
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(dataMap["ObjectiveCriterion"][0]));

    enableRunButton(true);

    return true;
}

void
nmfEstimation_Tab6::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");
    int index;

    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Estimation");
    m_EstimationOutputFile = settings->value("OutputFile","").toString().toStdString();
    m_EstimationDataFile   = settings->value("DataFile","").toString().toStdString();
    m_EstimationID         = settings->value("ID","").toString().toStdString();
    m_FontSize             = settings->value("FontSize",9).toString().toInt();
    m_IsMonospaced         = settings->value("Monospace",0).toString().toInt();
    settings->endGroup();

    delete settings;

    index = Estimation_Tab6_FontSizeCMB->findText(QString::number(m_FontSize));
    Estimation_Tab6_FontSizeCMB->setCurrentIndex(index);
}

void
nmfEstimation_Tab6::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("Monospace",  (int)Estimation_Tab6_MonoCB->isChecked());
    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab6::callback_TimeUnitsLockPB(bool isChecked)
{
    QString iconName = (isChecked) ? ":/icons/locked.png" : ":/icons/unlocked.png";
    QIcon icon(iconName);

    Estimation_Tab6_NL_StopAfterTimeUnitsCMB->setEnabled(!isChecked);
    Estimation_Tab6_NL_TimeUnitsLockPB->setIcon(icon);
}
