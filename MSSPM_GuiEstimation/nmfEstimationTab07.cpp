#include "nmfEstimationTab07.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

nmfEstimation_Tab7::nmfEstimation_Tab7(QTabWidget*  tabs,
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
    m_EnsembleDefaultFilename     = nmfConstantsMSSPM::FilenameMultiRun;
    m_EnsembleTimeStampedFilename = nmfConstantsMSSPM::FilenameMultiRun;
    m_IsMultiRun = false;
    m_MultiRunType.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab7::nmfEstimation_Tab7");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab07.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("7. Run Estimation"));

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
//  Estimation_Tab6_Bees_NumBestBeesSB         = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumOtherBeesSB");
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
    Estimation_Tab6_MinimizerSetDeterministicCB   = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_MinimizerSetDeterministicCB");
    Estimation_Tab6_EnsembleSetDeterministicCB    = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_EnsembleSetDeterministicCB");
    Estimation_Tab6_BeesSetDeterministicCB        = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_BeesSetDeterministicCB");
    Estimation_Tab6_AddToReviewPB                 = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_AddToReviewPB");
    Estimation_Tab6_NL_TimeUnitsLockPB            = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_NL_TimeUnitsLockPB");
    Estimation_Tab6_BeesDetStoTypeLBL             = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_BeesDetStoTypeLBL");
    Estimation_Tab6_BeesSetDeterministicLBL       = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_BeesSetDeterministicLBL");

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
            this,                                   SLOT(callback_ReloadPB()));
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
            this,                                         SLOT(callback_AveragingAlgorithmCMB(QString)));
    connect(Estimation_Tab6_ScalingCMB,                   SIGNAL(currentTextChanged(QString)),
            this,                                         SLOT(callback_ScalingCMB(QString)));
    connect(Estimation_Tab6_EnsembleControlsGB,           SIGNAL(toggled(bool)),
            this,                                         SLOT(callback_EnsembleControlsGB(bool)));
    connect(Estimation_Tab6_EnsembleSetAllPB,             SIGNAL(clicked()),
            this,                                         SLOT(callback_EnsembleSetAllPB()));
    connect(Estimation_Tab6_EnsembleLoadPB,               SIGNAL(clicked()),
            this,                                         SLOT(callback_EnsembleLoadPB()));
    connect(Estimation_Tab6_NL_StopAfterTimeUnitsCMB,     SIGNAL(currentTextChanged(QString)),
            this,                                         SLOT(callback_StopAfterTimeUnitsCMB(QString)));
    connect(Estimation_Tab6_EnsembleUsingPctPB,           SIGNAL(clicked()),
            this,                                         SLOT(callback_EnsembleUsingPctPB()));
    connect(Estimation_Tab6_EnsembleUsingByCMB,           SIGNAL(currentTextChanged(QString)),
            this,                                         SLOT(callback_EnsembleUsingAmountCMB(QString)));
    connect( Estimation_Tab6_MinimizerSetDeterministicCB, SIGNAL(stateChanged(int)),
             this,                                        SLOT(callback_SetDeterministicCB(int)));
    connect( Estimation_Tab6_EnsembleSetDeterministicCB,  SIGNAL(stateChanged(int)),
             this,                                        SLOT(callback_EnsembleSetDeterministicCB(int)));
    connect(Estimation_Tab6_AddToReviewPB,                SIGNAL(clicked()),
            this,                                         SLOT(callback_AddToReviewPB()));
//    connect(Estimation_Tab6_EstimateSurveyQCB,            SIGNAL(stateChanged(int)),
//            this,                                         SLOT(callback_EstimateSurveyQCB(int)));
    connect(Estimation_Tab6_NL_TimeUnitsLockPB,           SIGNAL(clicked(bool)),
            this,                                         SLOT(callback_TimeUnitsLockPB(bool)));


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


nmfEstimation_Tab7::~nmfEstimation_Tab7()
{
}

void
nmfEstimation_Tab7::checkAlgorithmIdentifiersForMultiRun(
        std::string& Algorithm,
        std::string& Minimizer,
        std::string& ObjectiveCriterion,
        std::string& Scaling)
{
    readSettings();

    if (m_IsMultiRun) {
        Algorithm          = m_MultiRunType;
        Minimizer          = m_MultiRunType;
        ObjectiveCriterion = m_MultiRunType;
        Scaling            = m_MultiRunType;
    }
}


void
nmfEstimation_Tab7::initializeDetStoMap()
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
nmfEstimation_Tab7::enableAddToReview(bool enable)
{
    Estimation_Tab6_AddToReviewPB->setEnabled(enable);
}

bool
nmfEstimation_Tab7::isAMultiRun()
{
    return (Estimation_Tab6_EnsembleControlsGB->isEnabled() &&
            Estimation_Tab6_EnsembleControlsGB->isChecked() &&
            Estimation_Tab6_EnsembleTotalRunsSB->value() > 1);
}

bool
nmfEstimation_Tab7::isStopAfterValue()
{
    return Estimation_Tab6_NL_StopAfterValueCB->isChecked();
}

bool
nmfEstimation_Tab7::isStopAfterTime()
{
    return Estimation_Tab6_NL_StopAfterTimeCB->isChecked();
}

bool
nmfEstimation_Tab7::isStopAfterIter()
{
    return Estimation_Tab6_NL_StopAfterIterCB->isChecked();
}

void
nmfEstimation_Tab7::setFont(QFont font)
{
    Estimation_Tab6_RunTE->setFont(font);
}

void
nmfEstimation_Tab7::setOutputTE(QString msg)
{
    Estimation_Tab6_RunTE->setText(msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}
void
nmfEstimation_Tab7::appendOutputTE(QString msg)
{
    QString currentMsg = Estimation_Tab6_RunTE->document()->toHtml();
    Estimation_Tab6_RunTE->setText(currentMsg+msg);
    callback_Estimation_Tab6_FontSizeCMB(QString::number(m_FontSize));
}

void
nmfEstimation_Tab7::clearOutputTE()
{
    Estimation_Tab6_RunTE->clear();
}

void
nmfEstimation_Tab7::callback_Estimation_Tab6_MonoCB(int isChecked)
{
    QString fontName = (isChecked == Qt::Checked) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    emit ShowRunMessage(fontName);
}

void
nmfEstimation_Tab7::callback_PrevPB()
{
    int prevPage = Estimation_Tabs->currentIndex()-1;
    Estimation_Tabs->setCurrentIndex(prevPage);
}

void
nmfEstimation_Tab7::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}

bool
nmfEstimation_Tab7::isEstInitialBiomassEnabled()
{
    return Estimation_Tab6_EstimateInitialBiomassCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstInitialBiomassChecked()
{
    return Estimation_Tab6_EstimateInitialBiomassCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstGrowthRateEnabled()
{
    return Estimation_Tab6_EstimateGrowthRateCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstGrowthRateChecked()
{
    return Estimation_Tab6_EstimateGrowthRateCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCarryingCapacityEnabled()
{
    return Estimation_Tab6_EstimateCarryingCapacityCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstCarryingCapacityChecked()
{
    return Estimation_Tab6_EstimateCarryingCapacityCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCatchabilityEnabled()
{
    return Estimation_Tab6_EstimateCatchabilityCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstCatchabilityChecked()
{
    return Estimation_Tab6_EstimateCatchabilityCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCompetitionAlphaEnabled()
{
    return Estimation_Tab6_EstimateCompetitionAlphaCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstCompetitionAlphaChecked()
{
    return Estimation_Tab6_EstimateCompetitionAlphaCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstPredationExponentEnabled()
{
    return Estimation_Tab6_EstimatePredationExponentCB->isEnabled();
}

bool
nmfEstimation_Tab7::isEstPredationExponentChecked()
{
    return Estimation_Tab6_EstimatePredationExponentCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstSurveyQEnabled()
{
    return Estimation_Tab6_EstimateSurveyQCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstSurveyQChecked()
{
    return Estimation_Tab6_EstimateSurveyQCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstPredationRhoEnabled()
{
    return Estimation_Tab6_EstimatePredationRhoCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstPredationRhoChecked()
{
    return Estimation_Tab6_EstimatePredationRhoCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstPredationHandlingEnabled()
{
    return Estimation_Tab6_EstimatePredationHandlingCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstPredationHandlingChecked()
{
    return Estimation_Tab6_EstimatePredationHandlingCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCompetitionBetaSpeciesEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isEnabled();
}

bool
nmfEstimation_Tab7::isEstCompetitionBetaSpeciesChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCompetitionBetaGuildsEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstCompetitionBetaGuildsChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->isChecked();
}

bool
nmfEstimation_Tab7::isEstCompetitionBetaGuildsGuildsEnabled()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isEnabled();
}
bool
nmfEstimation_Tab7::isEstCompetitionBetaGuildsGuildsChecked()
{
    return Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->isChecked();
}

bool
nmfEstimation_Tab7::isSetToDeterministicMinimizer()
{
    return Estimation_Tab6_MinimizerSetDeterministicCB->isChecked();
}

bool
nmfEstimation_Tab7::isSetToDeterministicBees()
{
    return Estimation_Tab6_BeesSetDeterministicCB->isChecked();
}

int
nmfEstimation_Tab7::getMaxGenerations()
{
    return Estimation_Tab6_Bees_MaxGenerationsSB->value();
}

int
nmfEstimation_Tab7::getNumBees()
{
    return Estimation_Tab6_Bees_NumBeesSB->value();
}

int
nmfEstimation_Tab7::getNumBestSites()
{
    return Estimation_Tab6_Bees_NumBestSitesSB->value();
}

int
nmfEstimation_Tab7::getNumEliteSites()
{
    return Estimation_Tab6_Bees_NumEliteSitesSB->value();
}

int
nmfEstimation_Tab7::getNumEliteBees()
{
    return Estimation_Tab6_Bees_NumEliteBeesSB->value();
}

int
nmfEstimation_Tab7::getNumOtherBees()
{
    return Estimation_Tab6_Bees_NumOtherBeesSB->value();
}

int
nmfEstimation_Tab7::getNeighborhoodSize()
{
    return Estimation_Tab6_Bees_NeighborhoodSizeSB->value();
}

int
nmfEstimation_Tab7::getNumSubRuns()
{
    return Estimation_Tab6_Bees_NumberOfRunsSB->value();
}

void
nmfEstimation_Tab7::setMaxGenerations(int value)
{
    Estimation_Tab6_Bees_MaxGenerationsSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumBees(int value)
{
    Estimation_Tab6_Bees_NumBeesSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumBestSites(int value)
{
    Estimation_Tab6_Bees_NumBestSitesSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumEliteSites(int value)
{
    Estimation_Tab6_Bees_NumEliteSitesSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumEliteBees(int value)
{
    Estimation_Tab6_Bees_NumEliteBeesSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumOtherBees(int value)
{
    Estimation_Tab6_Bees_NumOtherBeesSB->setValue(value);
}

void
nmfEstimation_Tab7::setNeighborhoodSize(int value)
{
    Estimation_Tab6_Bees_NeighborhoodSizeSB->setValue(value);
}

void
nmfEstimation_Tab7::setNumSubRuns(int value)
{
    Estimation_Tab6_Bees_NumberOfRunsSB->setValue(value);
}

void
nmfEstimation_Tab7::adjustNumberOfParameters()
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
    queryStr = "SELECT ProjectName,ModelName from " +
                nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["ModelName"].size() != 0) { // This means the model name exists so do an update
        cmd  = "UPDATE " + nmfConstantsMSSPM::TableModels + " SET";
        cmd += "   ProjectName = '" + m_ProjectName +
               "', ModelName = '"   + m_ModelName +
               "', NumberOfParameters = " + std::to_string(numberOfParameters) +
               " WHERE ProjectName = '"   + m_ProjectName +
               "' AND ModelName = '"      + m_ModelName + "'";
        errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_Logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::SaveSettingsConfiguration: Write table error: " + errorMsg);
            m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            return;
        }
    }

}

void
nmfEstimation_Tab7::callback_RunPB()
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
nmfEstimation_Tab7::passRunChecks(std::string& msg)
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
nmfEstimation_Tab7::runEnsemble()
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
nmfEstimation_Tab7::runEstimation()
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
nmfEstimation_Tab7::getEnsembleAverageBy()
{
    return Estimation_Tab6_EnsembleAverageByCMB->currentText();
}

QString
nmfEstimation_Tab7::getEnsembleAveragingAlgorithm()
{
    return Estimation_Tab6_EnsembleAveragingAlgorithmCMB->currentText();
}

QString
nmfEstimation_Tab7::createTimeStampedEnsembleFile()
{
    QString ensembleTimeStampedFilename = QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun);
    QString filePath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    QString defaultEnsembleFilename = QDir(filePath).filePath(ensembleTimeStampedFilename);

    if (isAMultiRun()) {
        // Derive the new ensemble file name
        QStringList parts     = QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun).split(".");
        std::string timestamp = m_Logger->getTimestamp(nmfConstants::TimestampWithUnderscore);
        if (parts.size() == 2) {
            ensembleTimeStampedFilename = parts[0]+"_"+QString::fromStdString(timestamp)+"."+parts[1];
        }
        QString timestampedEnsembleFilename = QDir(filePath).filePath(ensembleTimeStampedFilename);
        QFile::copy(defaultEnsembleFilename,timestampedEnsembleFilename);
    } else {
        ensembleTimeStampedFilename.clear();
    }
    m_EnsembleTimeStampedFilename = ensembleTimeStampedFilename.toStdString();

    return ensembleTimeStampedFilename;
}

std::string
nmfEstimation_Tab7::getEnsembleTimeStampedFilename()
{
    return m_EnsembleTimeStampedFilename;
}

std::vector<nmfStructsQt::EstimateRunBox>
nmfEstimation_Tab7::getEstimateRunBoxes()
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
nmfEstimation_Tab7::callback_ReloadPB()
{
    if (loadWidgets()) {
        enableRunButton(true);
        QMessageBox::information(Estimation_Tabs, "Run Settings Load",
                                 "\nRun Settings successfully loaded.\n",
                                 QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab7::callback_SavePB()
{
    createTimeStampedEnsembleFile();

    bool wasSaved = saveSystem(true);
    if (wasSaved) {
        enableRunButton(true);
        enableAddToReview(false);
    }
}

void
nmfEstimation_Tab7::callback_SaveSettings()
{
    saveSystem(false);
}

bool
nmfEstimation_Tab7::runBeesCheck(QString& errorMsg)
{
    bool allChecksOK  = true;
    int numBees       = Estimation_Tab6_Bees_NumBeesSB->value();
    int numBestSites  = Estimation_Tab6_Bees_NumBestSitesSB->value();
    int numEliteSites = Estimation_Tab6_Bees_NumEliteSitesSB->value();
    int numEliteBees  = Estimation_Tab6_Bees_NumEliteBeesSB->value();
    int numOtherBees  = Estimation_Tab6_Bees_NumOtherBeesSB->value();

    errorMsg = "\n";
    if (numBestSites > numBees) {
        errorMsg += "• numBestSites must be less than numBees\n";
        allChecksOK = false;
    }
    if (numEliteSites > numBestSites) {
        errorMsg += "• numEliteSites must be less than numBestSites\n";
        allChecksOK = false;
    }
    if (numEliteBees > numBees) {
        errorMsg += "• numEliteBees must be less than numBees\n";
        allChecksOK = false;
    }
    if (numOtherBees > numBees) {
        errorMsg += "• numOtherBees must be less than numBees\n";
        allChecksOK = false;
    }

    return allChecksOK;
}

bool
nmfEstimation_Tab7::saveSystem(bool RunChecks)
{
    bool ok;
    bool okToSave = true;
    std::string msg;
    std::string ModelName;
    QString errorMsg="";

    readSettings();
    ModelName = m_ModelName;

    if (RunChecks) {
        if (Estimation_Tab6_Bees_ParametersGB->isVisible()) {
            ok = runBeesCheck(errorMsg);
            if (!ok) {
                QMessageBox::warning(Estimation_Tabs, "Error in Bees Parameters",
                                     "\nPlease make sure Bees parameters are all within their appropriate limits. Found the following error(s):\n"+errorMsg,
                                     QMessageBox::Ok);
                return false;
            }
        }
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

    return okToSave;
}

bool
nmfEstimation_Tab7::saveSettingsConfiguration(bool verbose,
                                              std::string currentModelName)
{
    std::string cmd;
    std::string errorMsg;

    cmd  =  "UPDATE " + nmfConstantsMSSPM::TableModels + " SET";
    cmd +=  "   NumberOfRuns = "                    + getBeesNumberOfRuns() +
            ",  TimeStep = 1"                       + // std::to_string(Estimation_Tab6_TimeStepSB->value()) +
            ",  Algorithm = '"                      + getCurrentAlgorithm() +
            "', Minimizer = '"                      + getCurrentMinimizer() +
            "', ObjectiveCriterion = '"             + getCurrentObjectiveCriterion() +
            "', Scaling = '"                        + getCurrentScaling() +
            "', UseFixedSeedBees = "                + std::to_string(isSetToDeterministicBees()) +
            ",  UseFixedSeedMinimizer = "           + std::to_string(isSetToDeterministicMinimizer()) +
            ",  MinimizerType = '"                  + getCurrentMinimizerType() +
            "', BeesMaxGenerations = "              + getBeesMaxGenerations() +
            ",  BeesNumTotal = "                    + getBeesNumBees() +
            ",  BeesNumBestSites = "                + getBeesNumBestSites() +
            ",  BeesNumEliteSites = "               + getBeesNumEliteSites()  +
            ",  BeesNumElite = "                    + getBeesNumEliteBees() +
            ",  BeesNumOther = "                    + getBeesNumOtherBees() +
            ",  BeesNeighborhoodSize = "            + getBeesNeighborhoodSize() +
            ",  BeesNumRepetitions = "              + getBeesNumberOfRuns() +
            ",  NLoptUseStopVal = "                 + std::to_string(isStopAfterValue() ? 1 : 0) +
            ",  NLoptUseStopAfterTime = "           + std::to_string(isStopAfterTime()  ? 1 : 0) +
            ",  NLoptUseStopAfterIter = "           + std::to_string(isStopAfterIter()  ? 1 : 0) +
            ",  NLoptStopVal = "                    + std::to_string(getCurrentStopAfterValue()) +
            ",  NLoptStopAfterTime = "              + std::to_string(getCurrentStopAfterTime()) +
            ",  NLoptStopAfterIter = "              + std::to_string(getCurrentStopAfterIter()) +
            ",  NLoptNumberOfRuns = "               + std::to_string(Estimation_Tab6_EnsembleTotalRunsSB->value()) +
            ",  EstimateInitialBiomass = "          + std::to_string(isEstInitialBiomassChecked()) +
            ",  EstimateGrowthRate = "              + std::to_string(isEstGrowthRateChecked()) +
            ",  EstimateCarryingCapacity = "        + std::to_string(isEstCarryingCapacityChecked()) +
            ",  EstimateCatchability = "            + std::to_string(isEstCatchabilityChecked()) +
            ",  EstimateCompetition = "             + std::to_string(isEstCompetitionAlphaChecked()) +
            ",  EstimateCompetitionSpecies = "      + std::to_string(isEstCompetitionBetaSpeciesChecked()) +
            ",  EstimateCompetitionGuilds = "       + std::to_string(isEstCompetitionBetaGuildsChecked()) +
            ",  EstimateCompetitionGuildsGuilds = " + std::to_string(isEstCompetitionBetaGuildsGuildsChecked()) +
            ",  EstimatePredation = "               + std::to_string(isEstPredationRhoChecked()) +
            ",  EstimatePredationHandling = "       + std::to_string(isEstPredationHandlingChecked()) +
            ",  EstimatePredationExponent = "       + std::to_string(isEstPredationExponentChecked()) +
            ",  EstimateSurveyQ = "                 + std::to_string(isEstSurveyQChecked()) +
            ",  EnsembleIsBoxChecked = "            + std::to_string(isAMultiRun()) +
            ",  EnsembleAverageAlg = '"             + getEnsembleAveragingAlgorithm().toStdString() +
            "', EnsembleAverageBy = '"              + getEnsembleAverageBy().toStdString() +
            "', EnsembleUsingWhat = '"              + getEnsembleUsingBy().toStdString() +
            "', EnsembleUsingValue = "              + std::to_string(getEnsembleUsingAmountValue()) +
            ",  EnsembleIsUsingPct = "              + std::to_string(isEnsembleUsingPct()) +
            ",  EnsembleFile = '"                   + m_EnsembleTimeStampedFilename + // getEnsembleFilename() +
            "'  WHERE ProjectName = '"              + m_ProjectName +
            "'  AND ModelName = '"                  + currentModelName + "'";

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab7::SaveSettingsConfiguration: Write table error: " + errorMsg);
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
nmfEstimation_Tab7::enableRunButton(bool state)
{
    Estimation_Tab6_RunPB->setEnabled(state);

//    if (state) {
        setRunButtonLabel("Run");
//    } else {
//        setRunButtonLabel("Running...");
//    }
}

void
nmfEstimation_Tab7::setRunButtonLabel(QString label)
{
    Estimation_Tab6_RunPB->setText(label);
}


void
nmfEstimation_Tab7::callback_MinimizerAlgorithmCMB(QString algorithm)
{
    QString detStoTypeLabel = m_DetStoTypeMap[algorithm];
    Estimation_Tab6_MinimizerDetStoTypeLBL->setText(detStoTypeLabel);
    Estimation_Tab6_MinimizerSetDeterministicCB->setEnabled(detStoTypeLabel == "(s)");
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_MinimizerTypeCMB(QString type)
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
nmfEstimation_Tab7::callback_EstimationAlgorithmCMB(QString algorithm)
{
    algorithm = Estimation_Tab6_EstimationAlgorithmCMB->currentText();

    bool isBeesAlgorithm  = (algorithm == "Bees Algorithm");
    bool isNLoptAlgorithm = (algorithm == "NLopt Algorithm");
    bool enableMinimizer  = isNLoptAlgorithm;

    if (algorithm.isEmpty()) {
        return;
    }

    Estimation_Tab6_BeesSetDeterministicCB->setEnabled(isBeesAlgorithm);
    Estimation_Tab6_BeesDetStoTypeLBL->setEnabled(isBeesAlgorithm);
    Estimation_Tab6_BeesSetDeterministicLBL->setEnabled(isBeesAlgorithm);
    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();
    Estimation_Tab6_MinimizerAlgorithmCMB->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerAlgorithmLBL->setEnabled(enableMinimizer);    
    Estimation_Tab6_MinimizerDetStoTypeLBL->setEnabled(enableMinimizer);
    Estimation_Tab6_MinimizerSetDeterministicCB->setEnabled(enableMinimizer);
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
nmfEstimation_Tab7::callback_ObjectiveCriterionCMB(QString objCrit)
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
nmfEstimation_Tab7::callback_ScalingCMB(QString scalingAlgorithm)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_AveragingAlgorithmCMB(QString averagingAlgorithm)
{
    //    enableRunButton(false);
}

std::string
nmfEstimation_Tab7::getBeesMaxGenerations()
{
    return std::to_string(Estimation_Tab6_Bees_MaxGenerationsSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumBeesSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumBestSites()
{
    return std::to_string(Estimation_Tab6_Bees_NumBestSitesSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumEliteSites()
{
    return std::to_string(Estimation_Tab6_Bees_NumEliteSitesSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumEliteBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumEliteBeesSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumOtherBees()
{
    return std::to_string(Estimation_Tab6_Bees_NumOtherBeesSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNeighborhoodSize()
{
    return std::to_string(Estimation_Tab6_Bees_NeighborhoodSizeSB->value());
}

std::string
nmfEstimation_Tab7::getBeesNumberOfRuns()
{
    return std::to_string(Estimation_Tab6_Bees_NumberOfRunsSB->value());
}

std::string
nmfEstimation_Tab7::getCurrentAlgorithm()
{
    return Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab7::getCurrentMinimizer()
{
    return Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab7::getCurrentMinimizerType()
{
    return Estimation_Tab6_MinimizerTypeCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab7::getCurrentObjectiveCriterion()
{
    return Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab7::getCurrentScaling()
{
    return Estimation_Tab6_ScalingCMB->currentText().toStdString();
}

double
nmfEstimation_Tab7::getCurrentStopAfterValue()
{
    return Estimation_Tab6_NL_StopAfterValueLE->text().toDouble();
}

void
nmfEstimation_Tab7::setCurrentMinimizerType(QString value)
{
    Estimation_Tab6_MinimizerTypeCMB->setCurrentText(value);
}

void
nmfEstimation_Tab7::setCurrentTimeUnits(QString units)
{
    Estimation_Tab6_NL_StopAfterTimeUnitsCMB->setCurrentText(units);
}

void
nmfEstimation_Tab7::setAlgorithm(QString algorithm)
{
    Estimation_Tab6_EstimationAlgorithmCMB->setCurrentText(algorithm);
}

void
nmfEstimation_Tab7::setMinimizer(QString minimizer)
{
    Estimation_Tab6_MinimizerAlgorithmCMB->setCurrentText(minimizer);
}

void
nmfEstimation_Tab7::setObjectiveCriterion(QString objectiveCriterion)
{
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(objectiveCriterion);
}

void
nmfEstimation_Tab7::setScaling(QString scaling)
{
    Estimation_Tab6_ScalingCMB->setCurrentText(scaling);
}

QString
nmfEstimation_Tab7::getCurrentTimeUnits()
{
    return Estimation_Tab6_NL_StopAfterTimeUnitsCMB->currentText();
}

int
nmfEstimation_Tab7::convertToAppropriateUnits(int seconds)
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
nmfEstimation_Tab7::getCurrentStopAfterTime()
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
nmfEstimation_Tab7::getCurrentStopAfterIter()
{
    return Estimation_Tab6_NL_StopAfterIterSB->value();
}

void
nmfEstimation_Tab7::callback_EnsembleTotalRunsSB(int value)
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
nmfEstimation_Tab7::getEnsembleNumberOfTotalRuns()
{
    return Estimation_Tab6_EnsembleTotalRunsSB->value();
}

QString
nmfEstimation_Tab7::getEnsembleUsingBy()
{
    return Estimation_Tab6_EnsembleUsingByCMB->currentText();
}

int
nmfEstimation_Tab7::getEnsembleUsingAmountValue()
{
    return Estimation_Tab6_EnsembleUsingAmountSB->value();
}

void
nmfEstimation_Tab7::callback_EnsembleRunsSB(int value)
{
    enableRunButton(false);
}


void
nmfEstimation_Tab7::setMohnsRhoFileType(const QString& runType)
{
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));
    QFile file(fullPath);

    if (file.open(QIODevice::Append)) {
        QTextStream stream(&file);
        stream << runType << "\n";
        file.close();
    }
}

void
nmfEstimation_Tab7::setMohnsRhoFileHeader()
{
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));
    QFile file(fullPath);

    QList<QCheckBox* > EstimateCheckBoxes = getAllEstimateCheckboxes();
    int NumEstimatedCheckboxes = EstimateCheckBoxes.size();

    if (file.open(QIODevice::Append)) {
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

bool
nmfEstimation_Tab7::addToMultiRunFile(const int& numRunsToAdd,
                                      const int& currentNumberOfRuns,
                                      const int& totalNumberOfRunsDesired,
                                      QString& fullPath)
{
    QString msg;
    if (fullPath.isEmpty()) { // Means it's not a Mohn's Rho multi-run
        fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
        fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun));
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
        std::string ObjectiveCriterion = getCurrentObjectiveCriterion();
        std::string Algorithm          = getCurrentAlgorithm();
        std::string Minimizer          = getCurrentMinimizer();
        std::string Scaling            = getCurrentScaling();

        QTextStream stream(&file);
        stream << numRunsToAdd;
        stream << "," << QString::fromStdString(ObjectiveCriterion);
        stream << "," << QString::fromStdString(Algorithm);
        stream << "," << QString::fromStdString(Minimizer);
        stream << "," << QString::fromStdString(Scaling);
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
nmfEstimation_Tab7::clearWidgets()
{
    std::cout << "Note: nmfEstimation_Tab7::clearWidgets TBD" << std::endl;
}

//void
//nmfEstimation_Tab7::callback_EstimateSurveyQCB(int state)
//{
//    emit DimScalarBiomassControls(state == Qt::Checked);
//}


//void
//nmfEstimation_Tab7::callback_EnableSurveyQ(const QString biomassType,
//                                           const bool enable,
//                                           const bool checked)
//{
//    Estimation_Tab6_EstimateSurveyQCB->setEnabled(enable);
//    Estimation_Tab6_EstimateSurveyQCB->blockSignals(true);
//    Estimation_Tab6_EstimateSurveyQCB->setChecked(checked);
//    Estimation_Tab6_EstimateSurveyQCB->blockSignals(false);
//    if (biomassType == "Relative") {
//        emit DimScalarBiomassControls(Estimation_Tab6_EstimateSurveyQCB->isChecked());
//    } else {
//        emit DimScalarBiomassControls(true);
//        Estimation_Tab6_EstimateSurveyQCB->setEnabled(false);
//    }
//}

void
nmfEstimation_Tab7::callback_EnsembleAddPB()
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
        m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleDefaultFilename));
    } else {
        QMessageBox::warning(Estimation_Tabs, "Multi-Run/Ensemble Complete",
                             "\nAttempting to add too many runs into the Multi-Run/Ensemble file.\n",
                             QMessageBox::Ok);
    }
    if (tmpSum == totalNumberOfRunsDesired) {
        QMessageBox::information(Estimation_Tabs, "Multi-Run/Ensemble Setup Complete",
                                 "\nAll runs in current Multi-Run/Ensemble have now been set up.\n",
                                 QMessageBox::Ok);
        enableNonEnsembleWidgets(false);
    }
}

QString
nmfEstimation_Tab7::getMultiRunColumnData(int col)
{
    return m_EnsembleDialog->getColumnData(col);
}


void
nmfEstimation_Tab7::callback_EnsembleClearPB()
{
    if (queryUserIfOkToClearMultiRunFile()) {
        clearEnsembleFile();
    }
}

void
nmfEstimation_Tab7::clearEnsembleFile()
{
    Estimation_Tab6_EnsembleRunsSetLE->setText("0");

    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun));
    QFile file(fullPath);
    file.remove();

    m_EnsembleDialog->clear();
    enableNonEnsembleWidgets(true);
    m_EnsembleDefaultFilename = nmfConstantsMSSPM::FilenameMultiRun;
}

void
nmfEstimation_Tab7::clearMohnsRhoFile()
{
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(QString::fromStdString(nmfConstantsMSSPM::FilenameMohnsRhoRun));
    QFile file(fullPath);
    file.remove();
}


void
nmfEstimation_Tab7::callback_EnsembleViewPB()
{
    if (m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleDefaultFilename))) {
        m_EnsembleDialog->show();
    }
}

bool
nmfEstimation_Tab7::queryUserIfOkToClearMultiRunFile()
{
    QString msg = "\nOK to clear existing Multi Run Parameter file?\n";

    QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Clear"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);

    return (reply == QMessageBox::Yes);
}


void
nmfEstimation_Tab7::callback_StopValCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterValueLE->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterTimeCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterTimeSB->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterIterCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterIterSB->setEnabled(isChecked == Qt::Checked);
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterTimeUnitsCMB(QString units)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterTimeSB(int value)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterIterSB(int value)
{
    enableRunButton(false);
}

void
nmfEstimation_Tab7::callback_StopAfterValueLE(QString value)
{
    enableRunButton(false);
}


void
nmfEstimation_Tab7::callback_RefreshEstimateRunBoxes(int unused)
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
nmfEstimation_Tab7::getAllEstimateCheckboxes()
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
nmfEstimation_Tab7::activateCheckBox(QCheckBox* cbox, std::pair<bool,bool> state)
{
    cbox->setEnabled(state.first);
    cbox->setChecked(state.second);
}

void
nmfEstimation_Tab7::callback_SetEstimateRunCheckboxes(
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
nmfEstimation_Tab7::refreshMsg(QFont font, QString msg)
{
    readSettings();

    clearOutputTE();
    Estimation_Tab6_RunTE->setFont(font);
    setOutputTE(msg);
}

void
nmfEstimation_Tab7::callback_Estimation_Tab6_FontSizeCMB(QString theFontSize)
{
    m_FontSize = theFontSize.toInt();

    QTextCursor cursor = Estimation_Tab6_RunTE->textCursor();
    Estimation_Tab6_RunTE->selectAll();
    Estimation_Tab6_RunTE->setFontPointSize(m_FontSize);
    Estimation_Tab6_RunTE->setTextCursor( cursor );
}

void
nmfEstimation_Tab7::callback_EnsembleControlsGB(bool isChecked)
{
    callback_EnsembleTotalRunsSB(Estimation_Tab6_EnsembleTotalRunsSB->value());
    enableNonEnsembleWidgets(! isChecked);

    if (isChecked) {
        loadEnsembleFile(QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun),
                         nmfConstantsMSSPM::VerboseOff);
    }

}


void
nmfEstimation_Tab7::callback_EnsembleUsingAmountCMB(QString value)
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
nmfEstimation_Tab7::setDeterministicBeesCB(bool state)
{
    Estimation_Tab6_BeesSetDeterministicCB->setChecked(state);
}

void
nmfEstimation_Tab7::setDeterministicMinimizerCB(bool state)
{
    Estimation_Tab6_MinimizerSetDeterministicCB->setChecked(state);
}

void
nmfEstimation_Tab7::callback_SetDeterministicCB(int state)
{
    Estimation_Tab6_EnsembleSetDeterministicCB->blockSignals(true);
    Estimation_Tab6_EnsembleSetDeterministicCB->setChecked(state==Qt::Checked);
    Estimation_Tab6_EnsembleSetDeterministicCB->blockSignals(false);
}

void
nmfEstimation_Tab7::callback_EnsembleSetDeterministicCB(int state)
{
    Estimation_Tab6_MinimizerSetDeterministicCB->blockSignals(true);
    Estimation_Tab6_MinimizerSetDeterministicCB->setChecked(state==Qt::Checked);
    Estimation_Tab6_MinimizerSetDeterministicCB->blockSignals(false);
}

void
nmfEstimation_Tab7::callback_AddToReviewPB()
{
    emit AddToReview();
    enableRunButton(true);
}

bool
nmfEstimation_Tab7::isEnsembleUsingPct()
{
    return (Estimation_Tab6_EnsembleUsingPctPB->text() == "%");
}

void
nmfEstimation_Tab7::callback_EnsembleUsingPctPB()
{
    QString label = Estimation_Tab6_EnsembleUsingPctPB->text();
    if (label == "%") {
        Estimation_Tab6_EnsembleUsingPctPB->setText("");
    } else {
        Estimation_Tab6_EnsembleUsingPctPB->setText("%");
    }
}

void
nmfEstimation_Tab7::callback_EnsembleLoadPB()
{
    loadEnsembleFile(QString::fromStdString(nmfConstantsMSSPM::FilenameMultiRun),
                     nmfConstantsMSSPM::VerboseOn);
}


bool
nmfEstimation_Tab7::loadEnsembleFile(QString ensembleFilename,
                                     const bool& verbose)
{
std::cout << "Loading: " << ensembleFilename.toStdString() << std::endl;
    bool retv = true;
    int TotalNumRuns = 0;
    QString msg;
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(ensembleFilename);
    QFile file(fullPath);

    m_EnsembleDefaultFilename = ensembleFilename.toStdString();

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream inStream(&file);
        while(!inStream.atEnd()) {
            QString line = inStream.readLine();
            QStringList fields = line.split(",");
            TotalNumRuns += fields[0].toInt();
        }
        file.close();
        setEnsembleTotalRuns(TotalNumRuns);
        setEnsembleRunsSet(TotalNumRuns);
        enableNonEnsembleWidgets(false);
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
nmfEstimation_Tab7::enableNonEnsembleWidgets(bool enable)
{
    Estimation_Tab6_ModelAlgorithmsGB->setEnabled(enable);
    Estimation_Tab6_NL_ParametersGB->setEnabled(enable);
    Estimation_Tab6_Bees_ParametersGB->setEnabled(enable);
    Estimation_Tab6_EstParametersGB->setEnabled(enable);

    enableRunButton(false);
}

void
nmfEstimation_Tab7::enableEnsembleControls(bool enable)
{
    Estimation_Tab6_EnsembleControlsGB->setChecked(enable);
}

void
nmfEstimation_Tab7::setEnsembleTotalRuns(int value)
{
   Estimation_Tab6_EnsembleTotalRunsSB->setValue(value);
}

void
nmfEstimation_Tab7::setEnsembleRunsSet(int value)
{
   Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(value));
}

void
nmfEstimation_Tab7::setEnsembleAverageBy(QString averageBy)
{
    Estimation_Tab6_EnsembleAverageByCMB->setCurrentText(averageBy);
}

void
nmfEstimation_Tab7::setEnsembleAveragingAlgorithm(QString aveAlg)
{
    Estimation_Tab6_EnsembleAveragingAlgorithmCMB->setCurrentText(aveAlg);
}

void
nmfEstimation_Tab7::setEnsembleUsingBy(QString usingBy)
{
    Estimation_Tab6_EnsembleUsingByCMB->setCurrentText(usingBy);
}

void
nmfEstimation_Tab7::setEnsembleUsingAmountValue(QString usingAmount)
{
    Estimation_Tab6_EnsembleUsingAmountSB->setValue(usingAmount.toInt());
}

void
nmfEstimation_Tab7::setEnsembleUsingPct(bool isUsingPct)
{
    QString text = (isUsingPct) ? "%" : "";
    Estimation_Tab6_EnsembleUsingPctPB->setText(text);
}


void
nmfEstimation_Tab7::callback_EnsembleSetAllPB()
{
    int numRunsToAdd             = Estimation_Tab6_EnsembleTotalRunsSB->value();
    int currentNumberOfRuns      = Estimation_Tab6_EnsembleRunsSetLE->text().toInt();
    int totalNumberOfRunsDesired = numRunsToAdd;
    QString fullPath = "";

    if (addToMultiRunFile(numRunsToAdd,currentNumberOfRuns,totalNumberOfRunsDesired,fullPath)) {
        Estimation_Tab6_EnsembleRunsSetLE->setText(QString::number(Estimation_Tab6_EnsembleTotalRunsSB->value()));
        m_EnsembleDialog->loadWidgets(QString::fromStdString(m_EnsembleDefaultFilename));
        if (numRunsToAdd+currentNumberOfRuns == totalNumberOfRunsDesired) {
            QString msg = "\nSaved Mult-Run Parameter File to:\n\n" + fullPath + "\n";
            QMessageBox::information(Estimation_Tabs, "MultiRun Parameter File Save",
                                     msg,QMessageBox::Ok);
            enableNonEnsembleWidgets(false);
        }
    }
}

bool
nmfEstimation_Tab7::loadWidgets()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string algorithm = getCurrentAlgorithm();
    QString objectiveCriterion;

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab7::loadWidgets()");

    Estimation_Tab6_Bees_ParametersGB->hide();
    Estimation_Tab6_NL_ParametersGB->hide();

    if (algorithm == "Bees Algorithm") {
        Estimation_Tab6_Bees_ParametersGB->show();
        Estimation_Tab6_Bees_ParametersGB->setVisible(true);
        Estimation_Tab6_BeesSetDeterministicCB->setEnabled(true);
        Estimation_Tab6_BeesDetStoTypeLBL->setEnabled(true);
        Estimation_Tab6_BeesSetDeterministicLBL->setEnabled(true);
    } else if (algorithm == "NLopt Algorithm") {
        Estimation_Tab6_NL_ParametersGB->show();
        Estimation_Tab6_NL_ParametersGB->setVisible(true);
        Estimation_Tab6_BeesSetDeterministicCB->setEnabled(false);
        Estimation_Tab6_BeesDetStoTypeLBL->setEnabled(false);
        Estimation_Tab6_BeesSetDeterministicLBL->setEnabled(false);
    }

    readSettings();

    if (m_ModelName.empty())
        return false;

    fields     = {"ProjectName","ModelName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","Algorithm","Minimizer",
                  "UseFixedSeedBees","UseFixedSeedMinimizer","MinimizerType",
                  "ObjectiveCriterion","Scaling","GAGenerations","GAPopulationSize",
                  "GAMutationRate","GAConvergence","BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter","NLoptNumberOfRuns",
                  "EstimateInitialBiomass","EstimateGrowthRate","EstimateCarryingCapacity",
                  "EstimateCatchability","EstimateCompetition","EstimateCompetitionSpecies",
                  "EstimateCompetitionGuilds","EstimateCompetitionGuildsGuilds","EstimatePredation",
                  "EstimatePredationHandling","EstimatePredationExponent","EstimateSurveyQ",
                  "EnsembleIsBoxChecked","EnsembleAverageAlg","EnsembleAverageBy","EnsembleUsingWhat",
                  "EnsembleUsingValue","EnsembleIsUsingPct","EnsembleFile"};
    queryStr   = std::string("SELECT ProjectName,ModelName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,") +
                 "NumberOfRuns,StartYear,RunLength,TimeStep,Algorithm,Minimizer," +
                 "UseFixedSeedBees,UseFixedSeedMinimizer,MinimizerType,ObjectiveCriterion,Scaling," +
                 "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence," +
                 "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions," +
                 "BeesMaxGenerations,BeesNeighborhoodSize," +
                 "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter," +
                 "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter,NLoptNumberOfRuns," +
                 "EstimateInitialBiomass,EstimateGrowthRate,EstimateCarryingCapacity," +
                 "EstimateCatchability,EstimateCompetition,EstimateCompetitionSpecies," +
                 "EstimateCompetitionGuilds,EstimateCompetitionGuildsGuilds,EstimatePredation," +
                 "EstimatePredationHandling,EstimatePredationExponent,EstimateSurveyQ," +
                 "EnsembleIsBoxChecked,EnsembleAverageAlg,EnsembleAverageBy,EnsembleUsingWhat," +
                 "EnsembleUsingValue,EnsembleIsUsingPct,EnsembleFile FROM " +
                  nmfConstantsMSSPM::TableModels +
                 " WHERE ProjectName = '" + m_ProjectName +
                 "' AND ModelName = '"    + m_ModelName   + "'";
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
    Estimation_Tab6_Bees_NumOtherBeesSB->setValue(std::stoi(dataMap["BeesNumOther"][0]));
    Estimation_Tab6_Bees_NumberOfRunsSB->setValue(std::stoi(dataMap["BeesNumRepetitions"][0]));
    Estimation_Tab6_Bees_MaxGenerationsSB->setValue(std::stoi(dataMap["BeesMaxGenerations"][0]));
    Estimation_Tab6_Bees_NeighborhoodSizeSB->setValue(std::stof(dataMap["BeesNeighborhoodSize"][0]));
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(objectiveCriterion);
    Estimation_Tab6_ScalingCMB->setCurrentText(QString::fromStdString(dataMap["Scaling"][0]));
    Estimation_Tab6_NL_StopAfterValueCB->setChecked(dataMap["NLoptUseStopVal"][0] == "1");
    Estimation_Tab6_NL_StopAfterTimeCB->setChecked(dataMap["NLoptUseStopAfterTime"][0] == "1");
    Estimation_Tab6_NL_StopAfterIterCB->setChecked(dataMap["NLoptUseStopAfterIter"][0] == "1");
    Estimation_Tab6_NL_StopAfterValueLE->setText(QString::fromStdString(dataMap["NLoptStopVal"][0]));
//  Estimation_Tab6_NL_StopAfterTimeSB->setValue(convertToAppropriateUnits(std::stoi(dataMap["NLoptStopAfterTime"][0])));
    Estimation_Tab6_NL_StopAfterTimeSB->setValue(std::stoi(dataMap["NLoptStopAfterTime"][0]));
    Estimation_Tab6_NL_StopAfterIterSB->setValue(std::stoi(dataMap["NLoptStopAfterIter"][0]));
//  Estimation_Tab6_EnsembleTotalRunsSB->setValue(std::stoi(dataMap["NLoptNumberOfRuns"][0]));

    Estimation_Tab6_EstimateInitialBiomassCB->setChecked(dataMap["EstimateInitialBiomass"][0] == "1");
    Estimation_Tab6_EstimateGrowthRateCB->setChecked(dataMap["EstimateGrowthRate"][0] == "1");
    Estimation_Tab6_EstimateCarryingCapacityCB->setChecked(dataMap["EstimateCarryingCapacity"][0] == "1");
    Estimation_Tab6_EstimateCatchabilityCB->setChecked(dataMap["EstimateCatchability"][0] == "1");
    Estimation_Tab6_EstimateCompetitionAlphaCB->setChecked(dataMap["EstimateCompetition"][0] == "1");
    Estimation_Tab6_EstimateCompetitionBetaSpeciesSpeciesCB->setChecked(dataMap["EstimateCompetitionSpecies"][0] == "1");
    Estimation_Tab6_EstimateCompetitionBetaGuildSpeciesCB->setChecked(dataMap["EstimateCompetitionGuilds"][0] == "1");
    Estimation_Tab6_EstimateCompetitionBetaGuildGuildCB->setChecked(dataMap["EstimateCompetitionGuildsGuilds"][0] == "1");
    Estimation_Tab6_EstimatePredationRhoCB->setChecked(dataMap["EstimatePredation"][0] == "1");
    Estimation_Tab6_EstimatePredationHandlingCB->setChecked(dataMap["EstimatePredationHandling"][0] == "1");
    Estimation_Tab6_EstimatePredationExponentCB->setChecked(dataMap["EstimatePredationExponent"][0] == "1");
    Estimation_Tab6_EstimateSurveyQCB->setChecked(dataMap["EstimateSurveyQ"][0] == "1");

    Estimation_Tab6_EnsembleAveragingAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["EnsembleAverageAlg"][0]));
    Estimation_Tab6_EnsembleAverageByCMB->setCurrentText(QString::fromStdString(dataMap["EnsembleAverageBy"][0]));
    Estimation_Tab6_EnsembleUsingByCMB->setCurrentText(QString::fromStdString(dataMap["EnsembleUsingWhat"][0]));
    Estimation_Tab6_EnsembleUsingAmountSB->setValue(std::stoi(dataMap["EnsembleUsingValue"][0]));
    if (dataMap["EnsembleIsUsingPct"][0] == "1") {
        Estimation_Tab6_EnsembleUsingPctPB->setText("%");
    }
    // Not necessary since file is loaded during callback_EnsembleControlsGB.
    // And that's done when Model is loaded.
    //loadEnsembleFile(QString::fromStdString(dataMap["EnsembleFile"][0]),false);

    callback_EnsembleTotalRunsSB(std::stoi(dataMap["NLoptNumberOfRuns"][0]));
    callback_EstimationAlgorithmCMB(QString::fromStdString(dataMap["Algorithm"][0]));

    setDeterministicBeesCB(dataMap["UseFixedSeedBees"][0] == "1");
    setDeterministicMinimizerCB(dataMap["UseFixedSeedMinimizer"][0] == "1");
    Estimation_Tab6_MinimizerTypeCMB->setCurrentText(QString::fromStdString(dataMap["MinimizerType"][0]));
    Estimation_Tab6_MinimizerAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Minimizer"][0]));

    callback_ObjectiveCriterionCMB(objectiveCriterion);

    // RSK Hack to set the CMB correctly. Remove after implementing all of the disabled items in the 3 algorithm CMB's.
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(dataMap["ObjectiveCriterion"][0]));

    m_IsMultiRun   = (dataMap["EnsembleIsBoxChecked"][0] == "1");
    m_MultiRunType =  dataMap["EnsembleAverageAlg"][0];
    Estimation_Tab6_EnsembleControlsGB->setChecked(m_IsMultiRun);
    callback_EnsembleControlsGB(m_IsMultiRun);

    saveSettings();

    return true;
}

void
nmfEstimation_Tab7::readSettings()
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

    settings->beginGroup("Runtime");
    m_IsMultiRun    = settings->value("IsMultiRun",false).toBool();
    m_MultiRunType  = settings->value("MultiRunType","").toString().toStdString();
    settings->endGroup();

    delete settings;

    index = Estimation_Tab6_FontSizeCMB->findText(QString::number(m_FontSize));
    Estimation_Tab6_FontSizeCMB->setCurrentIndex(index);
}

void
nmfEstimation_Tab7::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Estimation");
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("FontSize",   Estimation_Tab6_FontSizeCMB->currentText());
    settings->setValue("Monospace",  (int)Estimation_Tab6_MonoCB->isChecked());
    settings->endGroup();

    settings->beginGroup("Runtime");
    settings->setValue("IsMultiRun",   isAMultiRun());
    settings->setValue("MultiRunType", getEnsembleAveragingAlgorithm());
    settings->endGroup();

    delete settings;
}

void
nmfEstimation_Tab7::callback_TimeUnitsLockPB(bool isChecked)
{
    QString iconName = (isChecked) ? ":/icons/locked.png" : ":/icons/unlocked.png";
    QIcon icon(iconName);

    Estimation_Tab6_NL_StopAfterTimeUnitsCMB->setEnabled(!isChecked);
    Estimation_Tab6_NL_TimeUnitsLockPB->setIcon(icon);
}
