#ifndef NMFDIAGNOSTICTAB2_H
#define NMFDIAGNOSTICTAB2_H

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

/**
 * @brief Retrospective Analysis
 *
 * Performs the Mohn's Rho Analysis. Years are "peeled" off one-by-one
 * and each resulting plot is shown on the same chart.
 */
class nmfDiagnostic_Tab2: public QObject
{
    Q_OBJECT

private:

    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    std::string  m_projectDir;
    int          m_RunLength;
    QWidget*     m_Diagnostic_Tab2_Widget;
    QTabWidget*  m_Diagnostic_Tabs;
    QPushButton* m_Diagnostic_Tab2_RunPB;
    QLineEdit*   m_Diagnostic_Tab2_MinYearLE;
    QLineEdit*   m_Diagnostic_Tab2_MaxYearLE;
    QLabel*      m_Diagnostic_Tab2_MinYearLBL;
    QLabel*      m_Diagnostic_Tab2_MaxYearLBL;
    QSpinBox*    m_Diagnostic_Tab2_NumPeelsSB;
    QComboBox*   m_Diagnostic_Tab2_PeelPositionCMB;
    Data_Struct  m_theBeeStruct;

    void readSettings();

public:
    /**
     * @brief nmfDiagnostic_Tab2 : class constructor
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfDiagnostic_Tab2(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfDiagnostic_Tab2();

    void    clearWidgets();
    int     getStartYearLBL();
    int     getEndYearLBL();
    int     getStartYearLE();
    int     getEndYearLE();
    int     getNumPeels();
    QString getPeelPosition();
    bool    getRunLength(int &RunLength);
    void    loadWidgets();
    void    loadWidgets(int NumPeels);
    void    saveSettings();
    void    setStartYearLE(int StartYear);
    void    setEndYearLE(int EndYear);
    void    setStartYearLBL(int StartYear);
    void    setEndYearLBL(int EndYear);

signals:
    void RunDiagnosticEstimation(std::vector<std::pair<int,int> > ranges);
    void ResaveSystem();

public slots:
    void callback_Diagnostic_Tab2_RunPB();
    void callback_Diagnostic_Tab2_NumPeelsSB(int val);
    void callback_Diagnostic_Tab2_PeelPositionCMB(QString position);
};

#endif

