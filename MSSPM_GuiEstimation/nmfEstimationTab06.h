/**
 * @file nmfEstimationTab06.h
 * @brief GUI definition for the nmfEstimation_Tab6 Estimation Run class
 *
 * This file contains the GUI definition for the Run Estimation Data page. This
 * page allows the user to enter/modify Run settings such as Estimation Model type
 * and its associated parameters.  In addition, the user can also run the Estimation
 * from this page.
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
 */

#include <QComboBox>
#include <QMessageBox>

#include "nmfUtilsQt.h"

#ifndef NMFESTIMATIONTAB6_H
#define NMFESTIMATIONTAB6_H

/**
 * @brief The Run Estimation Settings
 *
 * These widgets allow the user to enter and save the parameters required
 * to run an Estimation.  There's also a text widget which shows a summary
 * to the user of what transpired after the user hits the Run button.
 */
class nmfEstimation_Tab6: public QObject
{
    Q_OBJECT

    nmfDatabase* m_DatabasePtr;
    int          m_FontSize;
    int          m_IsMonospaced;
    nmfLogger*   m_Logger;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    std::string  m_EstimationDataFile;
    std::string  m_EstimationID;
    std::string  m_EstimationOutputFile;

    QGroupBox*   Estimation_Tab6_Bees_ParametersGB;
    QGroupBox*   Estimation_Tab6_NL_ParametersGB;
    QComboBox*   Estimation_Tab6_MinimizerAlgorithmCMB;
    QComboBox*   Estimation_Tab6_ObjectiveCriterionCMB;
    QLabel*      Estimation_Tab6_MinimizerAlgorithmLBL;
    QComboBox*   Estimation_Tab6_MinimizerTypeCMB;
    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab6_Widget;
    QTextEdit*   Estimation_Tab6_RunTE;
    QPushButton* Estimation_Tab6_RunPB;
    QPushButton* Estimation_Tab6_SavePB;
    QPushButton* Estimation_Tab6_ReloadPB;
    QPushButton* Estimation_Tab6_PrevPB;
    QComboBox*   Estimation_Tab6_FontSizeCMB;
    QCheckBox*   Estimation_Tab6_MonoCB;
    QComboBox*   Estimation_Tab6_EstimationAlgorithmCMB;
    QSpinBox*    Estimation_Tab6_NumberOfRunsSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBestSitesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumEliteBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_NumBestBeesSB;
    QSpinBox*    Estimation_Tab6_Bees_MaxGenerationsSB;
    QSpinBox*    Estimation_Tab6_Bees_NeighborhoodSizeSB;
    QLabel*      Estimation_Tab6_ScalingLBL;
    QComboBox*   Estimation_Tab6_ScalingCMB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterValueCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterTimeCB;
    QCheckBox*   Estimation_Tab6_NL_StopAfterIterCB;
    QLineEdit*   Estimation_Tab6_NL_StopAfterValueLE;
    QSpinBox*    Estimation_Tab6_NL_StopAfterTimeSB;
    QSpinBox*    Estimation_Tab6_NL_StopAfterIterSB;

    void readSettings();
    bool saveSettingsConfiguration(bool verbose,std::string currentSettingsName);
    bool isStopAfterValue();
    bool isStopAfterTime();
    bool isStopAfterNumEvals();

public:
    /**
     * @brief nmfEstimation_Tab6 : class constructor for the Run Estimation GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab6(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab6();

    /**
     * @brief Append the passed message to the output text edit widget
     * @param the message to append to the text widget
     */
    void appendOutputTE(QString msg);
    /**
     * @brief Clear the output text edit widget
     */
    void clearOutputTE();
    /**
     * @brief Gets the current Algorithm selected from the GUI
     * @return Returns the algorithm chosen
     */
    std::string getCurrentAlgorithm();
    /**
     * @brief Gets the current Minimizer routine chosen from the GUI
     * @return Returns the minimizer chosen
     */
    std::string getCurrentMinimizer();
    /**
     * @brief Gets the current Objective Criterion function from the GUI
     * @return Returns the objective criterion function (as a string)
     */
    std::string getCurrentObjectiveCriterion();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Clears the current message in the output text edit and writes the passed message to it
     * @param font : font to be used for the message
     * @param msg : the content of the message
     */
    void refreshMsg(QFont font, QString msg);
    /**
     * @brief Saves the current application settings to a Qt settings file
     */
    void saveSettings();
    /**
     * @brief Saves application settings but checking with user first
     * @param RunChecks : boolean checking to see if program should check with user prior to saving Model algorithm settings
     */
    void saveSystem(bool RunChecks);
    /**
     * @brief Sets the font for the output text edit widget
     * @param font : the font to use for the output text edit widget
     */
    void setFont(QFont font);
    /**
     * @brief Sets the content for the output text edit widget
     * @param msg : the content to use for the output text edit widget
     */
    void setOutputTE(QString msg);

signals:
    /**
     * @brief Signal sent to check all Estimation tables for completeness
     */
    void CheckAllEstimationTablesAndRun();
//    /**
//     * @brief Signal notifying that a new Estimation should be run
//     * @param showDiagnosticsChart : boolean signifying that the user wants to show the Diagnostics chart
//     */
//    void RunEstimation(bool showDiagnosticsChart);
    /**
     * @brief Signal sent after the user checks the Mono Font box. It causes
     * the displayed output edit widget to use a monospaced font.
     * @param font : the monospaced font to use in the output edit widget
     */
    void ShowRunMessage(QString font);
    /**
     * @brief Signal sent when the user changes the current Algorithm. Each
     * algorithm has its own set of parameters that must be displayed in the GUI.
     * @param algorithm : the new algorithm selected
     */
    void SetAlgorithm(QString algorithm);
    /**
     * @brief Signal sent to refresh the Forecast Year Range in the Forecast->Setup GUI.
     */
    void UpdateForecastYears();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user changes the font in the Run Summary tab
     * @param fontSize : the font selected
     */
    void callback_Estimation_Tab6_FontSizeCMB(QString fontSize);
    /**
     * @brief Callback invoked when the user checks the mono font checkbox in the Run Summary tab
     * @param isChecked : check state of the checkbox
     */
    void callback_Estimation_Tab6_MonoCB(int isChecked);
    /**
     * @brief Callback invoked when the user changes the Estimation Algorithm
     * @param algorithm : the algorithm selected
     */
    void callback_EstimationAlgorithmCMB(QString algorithm);
    /**
     * @brief Callback invoked when the user changes the minimizer type
     * @param type : the type of minimizer (i.e., global or local)
     */
    void callback_MinimizerTypeCMB(QString type);
    /**
     * @brief Callback invoked when the user changes the Objective Criterion
     * @param objectiveCriterion : the objective criterion selected
     */
    void callback_ObjectiveCriterionCMB(QString objectiveCriterion);
    /**
     * @brief Callback invoked when the user checks the Stop When Reach Value checkbox
     * @param isChecked : boolean signiying the check state
     */
    void callback_StopValCB(int isChecked);
    /**
     * @brief Callback invoked when the user checks the Stop After Time checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterTimeCB(int isChecked);
    /**
     * @brief Callback invoked when the user checks the Stop After Iteration checkbox
     * @param isChecked : boolean signifying the check state
     */
    void callback_StopAfterIterCB(int isChecked);
    /**
     * @brief Callback invoked when the user saves the model on the Setup -> Model Setup GUI
     */
    void callback_SaveSettings();
};

#endif // NMFESTIMATIONTAB6_H
