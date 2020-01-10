/** @file nmfEstimationTab04.h
 * @brief GUI definition for the nmfEstimation_Tab4 Predation Data class
 *
 * This file contains the GUI definition for the Predation Data page. This
 * page allows the user to enter/modify tables of data that correspond to
 * min/max predation coefficients (predators as columns and prey as rows). The
 * coefficients are Predation Effect, Handling Time, and Predation Exponent.
 */

#ifndef NMFESTIMATIONTAB4_H
#define NMFESTIMATIONTAB4_H


/**
 * @brief Predation Data
 *
 * These tables allow the user to enter and modify minimum and
 * maximum coefficients for Predation Effect (rho), Handling Time (h),
 * and Predation Exponent (b).
 */
class nmfEstimation_Tab4: public QObject
{
    Q_OBJECT

    nmfLogger*               m_Logger;
    nmfDatabase*             m_DatabasePtr;
    std::string              m_PredationForm;
    std::string              m_ProjectDir;
    std::string              m_ProjectSettingsConfig;
    std::vector<QTableView*> m_TableViews1d;
    std::vector<QTableView*> m_TableViews2d;
    std::vector<std::string> m_Tables1d;
    std::vector<std::string> m_Tables2d;
    std::vector<QStandardItemModel*> m_smodels1d;
    std::vector<QStandardItemModel*> m_smodels2d;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab4_Widget;
    QTableView*  Estimation_Tab4_PredationMinTV;
    QTableView*  Estimation_Tab4_PredationMaxTV;
    QTableView*  Estimation_Tab4_HandlingMinTV;
    QTableView*  Estimation_Tab4_HandlingMaxTV;
    QTableView*  Estimation_Tab4_ExponentMinTV;
    QTableView*  Estimation_Tab4_ExponentMaxTV;
    QSplitter*   Estimation_Tab4_PredationMinSP;
    QSplitter*   Estimation_Tab4_PredationMaxSP;
    QLabel*      Estimation_Tab4_PredationMinLBL;
    QLabel*      Estimation_Tab4_PredationMaxLBL;
    QLabel*      Estimation_Tab4_HandlingMinLBL;
    QLabel*      Estimation_Tab4_HandlingMaxLBL;
    QLabel*      Estimation_Tab4_ExponentMinLBL;
    QLabel*      Estimation_Tab4_ExponentMaxLBL;
    QPushButton* Estimation_Tab4_PrevPB;
    QPushButton* Estimation_Tab4_NextPB;
    QPushButton* Estimation_Tab4_LoadPB;
    QPushButton* Estimation_Tab4_SavePB;
    QCheckBox*   Estimation_Tab4_EstimateCB;

    void getForms(std::string& predationForm,
                  std::string& competitionForm);
    int  getNumSpecies();
    void readSettings();

public:
    /**
     * @brief nmfEstimation_Tab4 : class constructor for the Predation Parameters GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab4(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab4();

    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();


public Q_SLOTS:
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
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user updates the Predation Form in the Setup -> Model Setup page
     * @param predationForm : the predation form selected by the user
     */
    void callback_PredationFormChanged(QString predationForm);
    /**
     * @brief Callback invoked when the user changes any of the Minimum Predator Coeficients splitters
     * @param pos : position of the splitter
     * @param index : the index of the splitter changed
     */
    void callback_MinSplitterMoved(int pos, int index);
    /**
     * @brief Callback invoked when the user changes any of the Minimum Competitive Effects splitters
     * @param pos : position of the splitter
     * @param index : the index of the splitter changed
     */
    void callback_MaxSplitterMoved(int pos, int index);
    /**
     * @brief Callback invoked when the user toggles the Estimate checkbox
     * @param state : when set to Checked means that the minimum and maxumum estimated value limits are the same (effectively defining the parameter(s))
     */
    void callback_EstimateChecked(int state);
};

#endif // NMFESTIMATIONTAB4_H
