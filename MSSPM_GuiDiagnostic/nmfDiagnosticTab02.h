/**
 * @file nmfDiagnosticTab02.h
 * @brief GUI definition for the nmfDiagnostic_Tab2 Retrospective Analysis Diagnostics Data class
 * @date Nov 21, 2019
 * @author Ronald Klasky
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 *
 */
#ifndef NMFDIAGNOSTICTAB2_H
#define NMFDIAGNOSTICTAB2_H

#include <QComboBox>
#include <QHBoxLayout>
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

    nmfStructsQt::ModelDataStruct  m_BeeStruct;
    nmfDatabase* m_DatabasePtr;
    QLabel*      m_Diagnostic_Tab2_MaxYearLBL;
    QLineEdit*   m_Diagnostic_Tab2_MaxYearLE;
    QLabel*      m_Diagnostic_Tab2_MinYearLBL;
    QLineEdit*   m_Diagnostic_Tab2_MinYearLE;
    QSpinBox*    m_Diagnostic_Tab2_NumPeelsSB;
    QComboBox*   m_Diagnostic_Tab2_PeelPositionCMB;
    QLabel*      m_Diagnostic_Tab2_PeelPositionLBL;
    QPushButton* m_Diagnostic_Tab2_RunPB;
    QWidget*     m_Diagnostic_Tab2_Widget;
    QTabWidget*  m_Diagnostic_Tabs;
    nmfLogger*   m_Logger;
    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    std::string  m_ModelName;
    int          m_RunLength;
    bool         m_isMohnsRhoRun;

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
     * @brief Sets the enabled state for the Run button
     * @param state : boolean definin the Run button's enabled state
     */
    void enableRunButton(bool state);
    void setIsMohnsRho(bool state);
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
     * @brief Notifies the user if the run is a Mohns Rho run
     * @return Boolean signifying if the user just started a Mohns Rho run
     */
    bool    isAMohnsRhoRun();
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

    void    setWidgetsDark();
    void    setWidgetsLight();

signals:
    // void ResaveSystem();
    /**
     * @brief Signals to the main routine to run the Mohn's Rho anaylsis with the passed year ranges
     * @param YearRanges : year ranges to run for Mohn's Rho analysis
     */
    void RunDiagnosticEstimation(std::vector<std::pair<int,int> > YearRanges);
    void EnableRunButtons(bool state);
    void CheckMSYBoxes(bool state);


public slots:
    /**
     * @brief Callback for when user changes the number of peel years in the GUI spinbox
     * @param numYearsPeeled : number of peeled years desired
     */
    void callback_NumPeelsSB(int numYearsPeeled);
    /**
     * @brief Callback for when user changes the position of the peeled years
     * @param peelPosition : position of peeled years (beginning or end of time period)
     */
    void callback_PeelPositionCMB(QString peelPosition);
    /**
     * @brief Callback for when the Run button is pressed
     */
    void callback_RunPB();
};

#endif

