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

    Data_Struct  m_BeeStruct;
    nmfDatabase* m_DatabasePtr;
    QLabel*      m_Diagnostic_Tab2_MaxYearLBL;
    QLineEdit*   m_Diagnostic_Tab2_MaxYearLE;
    QLabel*      m_Diagnostic_Tab2_MinYearLBL;
    QLineEdit*   m_Diagnostic_Tab2_MinYearLE;
    QSpinBox*    m_Diagnostic_Tab2_NumPeelsSB;
    QComboBox*   m_Diagnostic_Tab2_PeelPositionCMB;
    QPushButton* m_Diagnostic_Tab2_RunPB;
    QWidget*     m_Diagnostic_Tab2_Widget;
    QTabWidget*  m_Diagnostic_Tabs;
    nmfLogger*   m_Logger;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    int          m_RunLength;

    void clearWidgets();
    int  getEndYearLE();
    void readSettings();
    void saveSettings();
    void setEndYearLBL(int EndYear);
    void setStartYearLBL(int StartYear);
    void setEndYearLE(int EndYear);

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

    /**
     * @brief Gets the last year in the year range
     * @return The last year in the year range
     */
    int     getEndYearLBL();
    /**
     * @brief Gets the number of years peeled off of the year range
     * @return The number of peeled years
     */
    int     getNumPeels();
    /**
     * @brief Gets the peel position in the year range
     * @return The peel position (from the end or beginning)
     */
    QString getPeelPosition();
    /**
     * @brief Gets the first year in the year range
     * @return The first year in the year range
     */
    int     getStartYearLBL();
    /**
     * @brief Gets first year of range
     * @return The first year of range
     */
    int     getStartYearLE();
    /**
     * @brief Loads the GUI widgets without any peels.  It calls the overloaded loadWidgets with a -1.
     */
    void    loadWidgets();
    /**
     * @brief Loads the GUI widgets accounting for number of peels user has selected
     * @param NumPeels : number of peels to show in year range
     */
    void    loadWidgets(int NumPeels);
    /**
     * @brief Sets the start year value accounting for number of peels and peel position
     * @param StartYear : First year of range
     */
    void    setStartYearLE(int StartYear);

signals:
    // void ResaveSystem();
    /**
     * @brief Signals to the main routine to run the Mohn's Rho anaylsis with the passed year ranges
     * @param YearRanges : year ranges to run for Mohn's Rho analysis
     */
    void RunDiagnosticEstimation(std::vector<std::pair<int,int> > YearRanges);

public slots:
    /**
     * @brief Callback for when user changes the number of peel years in the GUI spinbox
     * @param numYearsPeeled : number of peeled years desired
     */
    void callback_Diagnostic_Tab2_NumPeelsSB(int numYearsPeeled);
    /**
     * @brief Callback for when user changes the position of the peeled years
     * @param peelPosition : position of peeled years (beginning or end of time period)
     */
    void callback_Diagnostic_Tab2_PeelPositionCMB(QString peelPosition);
    /**
     * @brief Callback for when the Run button is pressed
     */
    void callback_Diagnostic_Tab2_RunPB();
};

#endif

