#include "nmfDiagnosticTab02.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

nmfDiagnostic_Tab2::nmfDiagnostic_Tab2(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    // Define class variables
    m_Diagnostic_Tabs = tabs;
    m_Logger          = logger;
    m_DatabasePtr     = databasePtr;
    m_ProjectDir      = projectDir;
    m_isMohnsRhoRunForSingleRun = false;
    m_isMohnsRhoRunForMultiRun  = false;
    m_IsMultiRun      = false;
    m_ProjectName.clear();
    m_ModelName.clear();
    m_MultiRunType.clear();

    // Load ui as a widget from disk
    QFile file(":/forms/Diagnostic/Diagnostic_Tab02.ui");
    file.open(QFile::ReadOnly);
    m_Diagnostic_Tab2_Widget = loader.load(&file,m_Diagnostic_Tabs);
    m_Diagnostic_Tab2_Widget->setObjectName("Diagnostic_Tab2_Widget");
    file.close();

    // Define widgets
    m_Diagnostic_Tab2_MinYearLBL      = m_Diagnostic_Tabs->findChild<QLabel      *>("Diagnostic_Tab2_MinYearLBL");
    m_Diagnostic_Tab2_MaxYearLBL      = m_Diagnostic_Tabs->findChild<QLabel      *>("Diagnostic_Tab2_MaxYearLBL");
    m_Diagnostic_Tab2_MinYearLE       = m_Diagnostic_Tabs->findChild<QLineEdit   *>("Diagnostic_Tab2_MinYearLE");
    m_Diagnostic_Tab2_MaxYearLE       = m_Diagnostic_Tabs->findChild<QLineEdit   *>("Diagnostic_Tab2_MaxYearLE");
    m_Diagnostic_Tab2_RunPB           = m_Diagnostic_Tabs->findChild<QPushButton *>("Diagnostic_Tab2_RunPB");
    m_Diagnostic_Tab2_NumPeelsSB      = m_Diagnostic_Tabs->findChild<QSpinBox    *>("Diagnostic_Tab2_NumPeelsSB");
    m_Diagnostic_Tab2_PeelPositionCMB = m_Diagnostic_Tabs->findChild<QComboBox   *>("Diagnostic_Tab2_PeelPositionCMB");
    m_Diagnostic_Tab2_PeelPositionLBL = m_Diagnostic_Tabs->findChild<QLabel      *>("Diagnostic_Tab2_PeelPositionLBL");

    // Add the loaded widget as the new tabbed page
    m_Diagnostic_Tabs->addTab(m_Diagnostic_Tab2_Widget, tr("2. Retrospective Analysis"));

    // Set default state of widgets
    m_Diagnostic_Tab2_MinYearLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColorLight);
    m_Diagnostic_Tab2_MaxYearLE->setStyleSheet(nmfUtilsQt::ReadOnlyLineEditBgColorLight);
    m_Diagnostic_Tab2_MinYearLE->setReadOnly(true);
    m_Diagnostic_Tab2_MaxYearLE->setReadOnly(true);

    // Hide the Peel position widgets until a user asks for this functionality
    m_Diagnostic_Tab2_PeelPositionCMB->setEnabled(false);
    m_Diagnostic_Tab2_PeelPositionCMB->hide();
    m_Diagnostic_Tab2_PeelPositionLBL->hide();

    // Setup connections
    connect(m_Diagnostic_Tab2_RunPB, SIGNAL(clicked()),
            this,                    SLOT(callback_RunPB()));
    connect(m_Diagnostic_Tab2_NumPeelsSB, SIGNAL(valueChanged(int)),
            this,                    SLOT(callback_NumPeelsSB(int)));
    connect(m_Diagnostic_Tab2_PeelPositionCMB, SIGNAL(currentIndexChanged(QString)),
            this,                    SLOT(callback_PeelPositionCMB(QString)));

    loadWidgets();
}

nmfDiagnostic_Tab2::~nmfDiagnostic_Tab2()
{

}

void
nmfDiagnostic_Tab2::enableRunButton(bool state)
{
    QString label = (state == true) ? "Run" : "Running...";
    m_Diagnostic_Tab2_RunPB->setEnabled(state);
    m_Diagnostic_Tab2_RunPB->setText(label);
}

void
nmfDiagnostic_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    m_ModelName = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("Diagnostics");
    m_Diagnostic_Tab2_NumPeelsSB->setValue(settings->value("YearsPeeled",1).toInt());
    settings->endGroup();

    settings->beginGroup("Runtime");
    m_IsMultiRun    = settings->value("IsMultiRun",false).toBool();
    m_MultiRunType  = settings->value("MultiRunType","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
nmfDiagnostic_Tab2::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Diagnostics");
    settings->setValue("YearsPeeled", getNumPeels());
    settings->endGroup();

    delete settings;
}

void
nmfDiagnostic_Tab2::loadWidgets()
{
    loadWidgets(-1);
}

void
nmfDiagnostic_Tab2::loadWidgets(int NumPeels)
{
    int StartYear=0;
    int RunLength=0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    m_Logger->logMsg(nmfConstants::Normal,"nmfDiagnostic_Tab2::loadWidgets()");

    readSettings();

    clearWidgets();

    fields   = {"StartYear","RunLength"};
    queryStr = "SELECT StartYear,RunLength from " + nmfConstantsMSSPM::TableModels +
               " WHERE ProjectName = '" + m_ProjectName +
               "' AND  ModelName = '"   + m_ModelName   + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["StartYear"].size() != 0) {
        StartYear = std::stoi(dataMap["StartYear"][0]);
        RunLength = std::stoi(dataMap["RunLength"][0]);
        m_RunLength = RunLength;
        setStartYearLE(StartYear);
        setEndYearLE(StartYear+RunLength);
    }

    // Load the labels here...these only change from within loadWidgets().
    setStartYearLBL(StartYear);
    setEndYearLBL(StartYear+RunLength);

    if (NumPeels == -1) {
        m_Diagnostic_Tab2_NumPeelsSB->setMaximum(RunLength);
        m_Diagnostic_Tab2_NumPeelsSB->setMinimum(1);
        callback_NumPeelsSB(m_Diagnostic_Tab2_NumPeelsSB->value()); //1);
    } else {
        callback_NumPeelsSB(NumPeels);
    }

    readSettings();

    m_Logger->logMsg(nmfConstants::Normal,"nmfDiagnostic_Tab2::loadWidgets() end");
}



void
nmfDiagnostic_Tab2::setStartYearLE(int StartYear)
{
    m_Diagnostic_Tab2_MinYearLE->setText(QString::number(StartYear));
}

void
nmfDiagnostic_Tab2::setEndYearLE(int EndYear)
{
    m_Diagnostic_Tab2_MaxYearLE->setText(QString::number(EndYear));
}

void
nmfDiagnostic_Tab2::setStartYearLBL(int StartYear)
{
    m_Diagnostic_Tab2_MinYearLBL->setText(QString::number(StartYear));
}

void
nmfDiagnostic_Tab2::setEndYearLBL(int EndYear)
{
    m_Diagnostic_Tab2_MaxYearLBL->setText(QString::number(EndYear));
}

int
nmfDiagnostic_Tab2::getStartYearLE()
{
    return m_Diagnostic_Tab2_MinYearLE->text().toInt();
}

int
nmfDiagnostic_Tab2::getEndYearLE()
{
    return m_Diagnostic_Tab2_MaxYearLE->text().toInt();
}

int
nmfDiagnostic_Tab2::getStartYearLBL()
{
    return m_Diagnostic_Tab2_MinYearLBL->text().toInt();
}

int
nmfDiagnostic_Tab2::getEndYearLBL()
{
    return m_Diagnostic_Tab2_MaxYearLBL->text().toInt();
}

QString
nmfDiagnostic_Tab2::getPeelPosition()
{
    return m_Diagnostic_Tab2_PeelPositionCMB->currentText();
}

int
nmfDiagnostic_Tab2::getNumPeels()
{
    return m_Diagnostic_Tab2_NumPeelsSB->value();
}

void
nmfDiagnostic_Tab2::clearWidgets()
{
    setStartYearLE(0);
    setEndYearLE(0);
}

bool
nmfDiagnostic_Tab2::isAMohnsRhoRunForSingleRun()
{
    return m_isMohnsRhoRunForSingleRun;
}

void
nmfDiagnostic_Tab2::setMohnsRhoForSingleRun(bool state)
{
    m_isMohnsRhoRunForSingleRun = state;
}

bool
nmfDiagnostic_Tab2::isAMohnsRhoRunForMultiRun()
{
    return m_isMohnsRhoRunForMultiRun;
}

void
nmfDiagnostic_Tab2::setMohnsRhoForMultiRun(bool state)
{
    m_isMohnsRhoRunForMultiRun = state;
}

void
nmfDiagnostic_Tab2::callback_NumPeelsSB(int numPeels)
{
    int RunLength = 0;
    int StartYear = 0;
    if (getPeelPosition() == "From Beginning") {
        setStartYearLE(getStartYearLBL() + numPeels);
    } else if (getPeelPosition() == "From End") {
        // RSK incorrect if doing a MohnsRho run
        m_DatabasePtr->getRunLengthAndStartYear(m_Logger,m_ProjectName,m_ModelName,RunLength,StartYear);
        setEndYearLE(getStartYearLBL()+RunLength-numPeels ); //getEndYearLBL() - numPeels);
    }
    saveSettings();
}

void
nmfDiagnostic_Tab2::callback_PeelPositionCMB(QString position)
{
    int NumPeels = getNumPeels();

    loadWidgets(NumPeels);
    callback_NumPeelsSB(NumPeels);
}


void
nmfDiagnostic_Tab2::callback_RunPB()
{
    QString msg;
    QMessageBox::StandardButton reply;
    int NumPeeledYears = getNumPeels();

    m_Logger->logMsg(nmfConstants::Normal,"");
    m_Logger->logMsg(nmfConstants::Normal,"Start Retrospective");

    readSettings();

    if (m_IsMultiRun) {
QMessageBox::information(m_Diagnostic_Tabs, tr("Feature Unavailable"),
                         tr("\nFeature not yet available.\n\nRetrospective Analysis on a Multi Run is currently in development.\n"),
                         QMessageBox::Ok);
return;
        msg  = "\nWarning: The previous run was a multi-run.\n\nRunning a retrospective analysis on ";
        msg += "a multi-run will cause the multi-run to run multiple times (i.e. one time per peel). ";
        msg += "In this case, the last multi-run would run " + QString::number(NumPeeledYears);
        msg += " times. Also, the multi-run should be deterministic.";
        msg += "\n\nAre you sure you want to continue with this Retrospective Analysis?\n";
        reply = QMessageBox::question(m_Diagnostic_Tabs, tr("Warning"),
                                      tr(msg.toStdString().c_str()),
                                      QMessageBox::Cancel|QMessageBox::Yes,
                                      QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    emit EnableRunButtons(false);

    // Calculate Mohn's Rho
    int StartYear      = getStartYearLE();
    int EndYear        = NumPeeledYears + getEndYearLE();
    std::vector<std::pair<int,int> > ranges;

    // Check for a proper time range
    if ((EndYear < 0) || (StartYear >= EndYear)) {
        msg = "Invalid Year Range for Retrospective. Reload System: Setup -> Model Setup -> Load...";
        m_Logger->logMsg(nmfConstants::Warning,msg.toStdString());
        QMessageBox::warning(m_Diagnostic_Tabs, "Error", "\n"+msg, QMessageBox::Ok);
        emit EnableRunButtons(true);
        return;
    }

    saveSettings();

    // Mohn's Rho = {Σ[(X(t-n,t-n)-X(t-n,t)) / X(t-n,t)]} / x
    // where Σ goes from n=1 to x
    ranges.clear();
    for (int i=1; i<=NumPeeledYears; ++i) {
        ranges.push_back(std::make_pair(StartYear,EndYear-i));
std::cout << "range: " << StartYear << ", " << EndYear-i << std::endl;
    }

    emit CheckMSYBoxes(false);
//    if (m_IsMultiRun) {
//        m_isMohnsRhoRunForMultiRun  = true;
//        m_isMohnsRhoRunForSingleRun = false;
//        emit RunDiagnosticEstimationMultiRun(ranges);
//    } else {
        m_isMohnsRhoRunForMultiRun  = false;
        m_isMohnsRhoRunForSingleRun = true;
        emit RunDiagnosticEstimation(ranges);
//    }
}

void
nmfDiagnostic_Tab2::setWidgetsDark()
{
    nmfUtilsQt::setBackgroundLineEdit(m_Diagnostic_Tab2_MinYearLE,nmfUtilsQt::ReadOnlyLineEditBgColorDark);
    nmfUtilsQt::setBackgroundLineEdit(m_Diagnostic_Tab2_MaxYearLE,nmfUtilsQt::ReadOnlyLineEditBgColorDark);
}

void
nmfDiagnostic_Tab2::setWidgetsLight()
{
    nmfUtilsQt::setBackgroundLineEdit(m_Diagnostic_Tab2_MinYearLE,nmfUtilsQt::ReadOnlyLineEditBgColorLight);
    nmfUtilsQt::setBackgroundLineEdit(m_Diagnostic_Tab2_MaxYearLE,nmfUtilsQt::ReadOnlyLineEditBgColorLight);
}
