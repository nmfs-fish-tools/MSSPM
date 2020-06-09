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
    m_ProjectSettingsConfig.clear();

    m_Logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab6::nmfEstimation_Tab6");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab06.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab6_Widget = loader.load(&file, Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab6_Widget, tr("6. Run Estimation"));

    Estimation_Tab6_Bees_ParametersGB       = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_Bees_ParametersGB");
    Estimation_Tab6_NL_ParametersGB         = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab6_NL_ParametersGB");
    Estimation_Tab6_EstimationAlgorithmCMB  = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_EstimationAlgorithmCMB");
    Estimation_Tab6_ObjectiveCriterionCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ObjectiveCriterionCMB");
    Estimation_Tab6_MinimizerAlgorithmCMB   = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinimizerAlgorithmCMB");
    Estimation_Tab6_MinimizerAlgorithmLBL   = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_MinimizerAlgorithmLBL");
    Estimation_Tab6_MinimizerTypeCMB        = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_MinimizerTypeCMB");
    Estimation_Tab6_RunTE                   = Estimation_Tabs->findChild<QTextEdit   *>("Estimation_Tab6_RunTE");
    Estimation_Tab6_RunPB                   = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_RunPB");
    Estimation_Tab6_ReloadPB                = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_ReloadPB");
    Estimation_Tab6_SavePB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_SavePB");
    Estimation_Tab6_PrevPB                  = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab6_PrevPB");
    Estimation_Tab6_FontSizeCMB             = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_FontSizeCMB");
    Estimation_Tab6_MonoCB                  = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_MonoCB");
    Estimation_Tab6_NumberOfRunsSB          = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NumberOfRunsSB");
    Estimation_Tab6_Bees_NumBeesSB          = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBeesSB");
    Estimation_Tab6_Bees_NumEliteSitesSB    = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteSitesSB");
    Estimation_Tab6_Bees_NumBestSitesSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumBestSitesSB");
    Estimation_Tab6_Bees_NumEliteBeesSB     = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumEliteBeesSB");
    Estimation_Tab6_Bees_NumBestBeesSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NumOtherBeesSB");
    Estimation_Tab6_Bees_MaxGenerationsSB   = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_MaxGenerationsSB");
    Estimation_Tab6_Bees_NeighborhoodSizeSB = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_Bees_NeighborhoodSizeSB");
    Estimation_Tab6_ScalingLBL              = Estimation_Tabs->findChild<QLabel      *>("Estimation_Tab6_ScalingLBL");
    Estimation_Tab6_ScalingCMB              = Estimation_Tabs->findChild<QComboBox   *>("Estimation_Tab6_ScalingCMB");
    Estimation_Tab6_NL_StopAfterValueCB     = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterValueCB");
    Estimation_Tab6_NL_StopAfterTimeCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterTimeCB");
    Estimation_Tab6_NL_StopAfterIterCB      = Estimation_Tabs->findChild<QCheckBox   *>("Estimation_Tab6_NL_StopAfterIterCB");
    Estimation_Tab6_NL_StopAfterValueLE     = Estimation_Tabs->findChild<QLineEdit   *>("Estimation_Tab6_NL_StopAfterValueLE");
    Estimation_Tab6_NL_StopAfterTimeSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterTimeSB");
    Estimation_Tab6_NL_StopAfterIterSB      = Estimation_Tabs->findChild<QSpinBox    *>("Estimation_Tab6_NL_StopAfterIterSB");

    // Update tool tip
    BeesMsg  = "Stochastic search algorithm based on the behavior of honey bees.";
    NLoptMsg = "Open-Source NonLinear Optimization package";

    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 0, BeesMsg,        Qt::ToolTipRole);
    Estimation_Tab6_EstimationAlgorithmCMB->setItemData( 1, NLoptMsg,       Qt::ToolTipRole);

    QFont font = Estimation_Tab6_RunTE->font();
    font.setBold(false);
    Estimation_Tab6_RunTE->setFont(font);
    Estimation_Tab6_PrevPB->setText("\u25C1--");

    connect(Estimation_Tab6_PrevPB,                 SIGNAL(clicked()),
            this,                                   SLOT(callback_PrevPB()));
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
    connect(Estimation_Tab6_MinimizerTypeCMB,       SIGNAL(currentTextChanged(QString)),
            this,                                   SLOT(callback_MinimizerTypeCMB(QString)));

    readSettings();

    // Initialize font
    Estimation_Tab6_MonoCB->blockSignals(true);
    Estimation_Tab6_MonoCB->setChecked(m_IsMonospaced);
    Estimation_Tab6_MonoCB->blockSignals(false);
    QString fontName = (m_IsMonospaced) ? "Courier" : Estimation_Tab6_RunPB->font().family();
    QFont defaultFont(fontName,11,QFont::Medium,false);
    setFont(defaultFont);

    callback_EstimationAlgorithmCMB("Bees Algorithm");
    Estimation_Tab6_RunPB->setEnabled(true);

    callback_MinimizerTypeCMB(Estimation_Tab6_MinimizerTypeCMB->currentText());

    m_Logger->logMsg(nmfConstants::Normal,"nmfMSPRODTab6::nmfMSPRODTab6 Complete");
}


nmfEstimation_Tab6::~nmfEstimation_Tab6()
{
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
nmfEstimation_Tab6::isStopAfterNumEvals()
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
nmfEstimation_Tab6::callback_RunPB()
{
    QString msg;

    if (isStopAfterValue() ||
        isStopAfterTime()  ||
        isStopAfterNumEvals())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Rerun all data checks, in case user changes number of Species or Guilds and
        // then tries to re-run.
        m_Logger->logMsg(nmfConstants::Normal,"");
        m_Logger->logMsg(nmfConstants::Normal,"Start Estimation");

        emit CheckAllEstimationTablesAndRun();
    } else {
        msg = "\nPlease select at least one Stop parameter.\n";
        QMessageBox::warning(Estimation_Tabs, "Error", msg, QMessageBox::Ok);
    }
}

void
nmfEstimation_Tab6::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab6::callback_SavePB()
{
    saveSystem(true);
    Estimation_Tab6_RunPB->setEnabled(true);
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
    std::string SystemName;

    readSettings();
    SystemName = m_ProjectSettingsConfig;

    if (RunChecks) {
        msg = "\nOK to save current settings as: " + SystemName + " ?";
        QMessageBox::StandardButton reply = QMessageBox::question(Estimation_Tabs, tr("Save"),
                                                                  tr(msg.c_str()),
                                                                  QMessageBox::No|QMessageBox::Yes,
                                                                  QMessageBox::Yes);
        okToSave = (reply == QMessageBox::Yes);
    }
    if (okToSave) {
        m_ProjectSettingsConfig = SystemName;
        saveSettingsConfiguration(RunChecks,SystemName);
        readSettings();
    }

    emit UpdateForecastYears();
}

bool
nmfEstimation_Tab6::saveSettingsConfiguration(bool verbose,
                                              std::string CurrentSettingsName)
{
    std::string cmd;
    std::string errorMsg;

    cmd  = "UPDATE Systems SET";
    cmd += "   NumberOfRuns = "          + std::to_string(Estimation_Tab6_NumberOfRunsSB->value()) +
           ",  TimeStep = 1"             + // std::to_string(Estimation_Tab6_TimeStepSB->value()) +
           ",  Algorithm = '"            + Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString() +
           "', Minimizer = '"            + Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString() +
           "', ObjectiveCriterion = '"   + Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString() +
           "', Scaling = '"              + Estimation_Tab6_ScalingCMB->currentText().toStdString() +
           "', BeesNumTotal = "          + std::to_string(Estimation_Tab6_Bees_NumBeesSB->value()) +
           ",  BeesNumElite = "          + std::to_string(Estimation_Tab6_Bees_NumEliteSitesSB->value()) +
           ",  BeesNumOther = "          + std::to_string(Estimation_Tab6_Bees_NumBestSitesSB->value()) +
           ",  BeesNumEliteSites = "     + std::to_string(Estimation_Tab6_Bees_NumEliteBeesSB->value())  +
           ",  BeesNumBestSites = "      + std::to_string(Estimation_Tab6_Bees_NumBestBeesSB->value()) +
           ",  BeesNumRepetitions = "    + std::to_string(Estimation_Tab6_NumberOfRunsSB->value()) +
           ",  BeesMaxGenerations = "    + std::to_string(Estimation_Tab6_Bees_MaxGenerationsSB->value()) +
           ",  BeesNeighborhoodSize = "  + std::to_string(Estimation_Tab6_Bees_NeighborhoodSizeSB->value()) +
           ",  NLoptUseStopVal = "       + std::to_string(Estimation_Tab6_NL_StopAfterValueCB->isChecked() ? 1 : 0) +
           ",  NLoptUseStopAfterTime = " + std::to_string(Estimation_Tab6_NL_StopAfterTimeCB->isChecked() ? 1 : 0) +
           ",  NLoptUseStopAfterIter = " + std::to_string(Estimation_Tab6_NL_StopAfterIterCB->isChecked() ? 1 : 0) +
           ",  NLoptStopVal = "          + Estimation_Tab6_NL_StopAfterValueLE->text().toStdString() +
           ",  NLoptStopAfterTime = "    + std::to_string(Estimation_Tab6_NL_StopAfterTimeSB->value()) +
           ",  NLoptStopAfterIter = "    + std::to_string(Estimation_Tab6_NL_StopAfterIterSB->value()) +
           "   WHERE SystemName = '"     + CurrentSettingsName + "'";

    errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        m_Logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab6::SaveSettingsConfiguration: Write table error: " + errorMsg);
        m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
        return false;
    }

    if (verbose) {
        QMessageBox::information(Estimation_Tabs, "Settings Updated",
                                 "\nSettings in Systems table have been successfully updated.\n");
    }
    saveSettings();

    return true;
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

    // Disable Run button until user Saves new model
    Estimation_Tab6_RunPB->setEnabled(false);

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
}

std::string
nmfEstimation_Tab6::getCurrentAlgorithm()
{
    return Estimation_Tab6_EstimationAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentMinimizer()
{
    return Estimation_Tab6_MinimizerAlgorithmCMB->currentText().toStdString();
}

std::string
nmfEstimation_Tab6::getCurrentObjectiveCriterion()
{
    return Estimation_Tab6_ObjectiveCriterionCMB->currentText().toStdString();
}

void
nmfEstimation_Tab6::callback_StopValCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterValueLE->setEnabled(isChecked == Qt::Checked);
}

void
nmfEstimation_Tab6::callback_StopAfterTimeCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterTimeSB->setEnabled(isChecked == Qt::Checked);
}

void
nmfEstimation_Tab6::callback_StopAfterIterCB(int isChecked)
{
    Estimation_Tab6_NL_StopAfterIterSB->setEnabled(isChecked == Qt::Checked);
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

    if (m_ProjectSettingsConfig.empty())
        return false;

    fields     = {"SystemName","CarryingCapacity","GrowthForm","PredationForm","HarvestForm","WithinGuildCompetitionForm",
                  "NumberOfRuns","StartYear","RunLength","TimeStep","Algorithm","Minimizer",
                  "ObjectiveCriterion","Scaling","GAGenerations","GAPopulationSize",
                  "GAMutationRate","GAConvergence","BeesNumTotal","BeesNumElite","BeesNumOther",
                  "BeesNumEliteSites","BeesNumBestSites","BeesNumRepetitions",
                  "BeesMaxGenerations","BeesNeighborhoodSize",
                  "NLoptUseStopVal","NLoptUseStopAfterTime","NLoptUseStopAfterIter",
                  "NLoptStopVal","NLoptStopAfterTime","NLoptStopAfterIter"};
    queryStr   = "SELECT SystemName,CarryingCapacity,GrowthForm,PredationForm,HarvestForm,WithinGuildCompetitionForm,";
    queryStr  += "NumberOfRuns,StartYear,RunLength,TimeStep,Algorithm,Minimizer,ObjectiveCriterion,Scaling,";
    queryStr  += "GAGenerations,GAPopulationSize,GAMutationRate,GAConvergence,";
    queryStr  += "BeesNumTotal,BeesNumElite,BeesNumOther,BeesNumEliteSites,BeesNumBestSites,BeesNumRepetitions,";
    queryStr  += "BeesMaxGenerations,BeesNeighborhoodSize,";
    queryStr  += "NLoptUseStopVal,NLoptUseStopAfterTime,NLoptUseStopAfterIter,";
    queryStr  += "NLoptStopVal,NLoptStopAfterTime,NLoptStopAfterIter ";
    queryStr  += "FROM Systems where SystemName = '";
    queryStr  += m_ProjectSettingsConfig + "'";

    dataMap    = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        std::cout << "Error: No records found in Systems" << std::endl;
        return false;
    }

    objectiveCriterion = QString::fromStdString(dataMap["ObjectiveCriterion"][0]);
    Estimation_Tab6_NumberOfRunsSB->setValue(std::stoi(dataMap["NumberOfRuns"][0]));
    Estimation_Tab6_EstimationAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Algorithm"][0]));
    Estimation_Tab6_Bees_NumBeesSB->setValue(std::stoi(dataMap["BeesNumTotal"][0]));
    Estimation_Tab6_Bees_NumEliteSitesSB->setValue(std::stoi(dataMap["BeesNumEliteSites"][0]));
    Estimation_Tab6_Bees_NumBestSitesSB->setValue(std::stoi(dataMap["BeesNumBestSites"][0]));
    Estimation_Tab6_Bees_NumEliteBeesSB->setValue(std::stoi(dataMap["BeesNumElite"][0]));
    Estimation_Tab6_Bees_NumBestBeesSB->setValue(std::stoi(dataMap["BeesNumOther"][0]));
    Estimation_Tab6_NumberOfRunsSB->setValue(std::stoi(dataMap["BeesNumRepetitions"][0]));
    Estimation_Tab6_Bees_MaxGenerationsSB->setValue(std::stoi(dataMap["BeesMaxGenerations"][0]));
    Estimation_Tab6_Bees_NeighborhoodSizeSB->setValue(std::stof(dataMap["BeesNeighborhoodSize"][0]));
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(objectiveCriterion);
    Estimation_Tab6_ScalingCMB->setCurrentText(QString::fromStdString(dataMap["Scaling"][0]));
    Estimation_Tab6_NL_StopAfterValueCB->setChecked(dataMap["NLoptUseStopVal"][0] == "1");
    Estimation_Tab6_NL_StopAfterTimeCB->setChecked(dataMap["NLoptUseStopAfterTime"][0] == "1");
    Estimation_Tab6_NL_StopAfterIterCB->setChecked(dataMap["NLoptUseStopAfterIter"][0] == "1");
    Estimation_Tab6_NL_StopAfterValueLE->setText(QString::fromStdString(dataMap["NLoptStopVal"][0]));
    Estimation_Tab6_NL_StopAfterTimeSB->setValue(std::stoi(dataMap["NLoptStopAfterTime"][0]));
    Estimation_Tab6_NL_StopAfterIterSB->setValue(std::stoi(dataMap["NLoptStopAfterIter"][0]));

    callback_EstimationAlgorithmCMB(QString::fromStdString(dataMap["Algorithm"][0]));
    Estimation_Tab6_MinimizerAlgorithmCMB->setCurrentText(QString::fromStdString(dataMap["Minimizer"][0]));
    callback_ObjectiveCriterionCMB(objectiveCriterion);

    // RSK Hack to set the CMB correctly. Remove after implementing all of the disabled items in the 3 algorithm CMB's.
    Estimation_Tab6_ObjectiveCriterionCMB->setCurrentText(QString::fromStdString(dataMap["ObjectiveCriterion"][0]));

    // Enable Run button
    Estimation_Tab6_RunPB->setEnabled(true);

    return true;
}

void
nmfEstimation_Tab6::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");
    int index;

    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Estimation");
    m_EstimationOutputFile = settings->value("OutputFile","").toString().toStdString();
    m_EstimationDataFile   = settings->value("DataFile","").toString().toStdString();
    m_EstimationID         = settings->value("ID","").toString().toStdString();
    m_FontSize           = settings->value("FontSize",9).toString().toInt();
    m_IsMonospaced       = settings->value("Monospace",0).toString().toInt();
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
