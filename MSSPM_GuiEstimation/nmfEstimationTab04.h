/**
 * @file nmfEstimationTab04.h
 * @brief GUI definition for the nmfEstimation_Tab4 Predation Data class
 *
 * This file contains the GUI definition for the Predation Data page. This
 * page allows the user to enter/modify tables of data that correspond to
 * min/max predation coefficients (predators as columns and prey as rows). The
 * coefficients are Predation Effect, Handling Time, and Predation Exponent.
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
    std::vector<QTableView*> m_TableViewsTypeI;
    std::vector<QTableView*> m_TableViewsTypeII;
    std::vector<QTableView*> m_TableViewsTypeIII;
    std::vector<std::string> m_TableNamesTypeI;
    std::vector<std::string> m_TableNamesTypeII;
    std::vector<std::string> m_TableNamesTypeIII;
    std::vector<std::string> m_TableNames1d;
    std::vector<std::string> m_TableNames2d;
    std::vector<QStandardItemModel*> m_smodels2d;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab4_Widget;
    QTableView*  Estimation_Tab4_PredationTV;
    QTableView*  Estimation_Tab4_PredationMinTV;
    QTableView*  Estimation_Tab4_PredationMaxTV;
    QTableView*  Estimation_Tab4_HandlingTV;
    QTableView*  Estimation_Tab4_HandlingMinTV;
    QTableView*  Estimation_Tab4_HandlingMaxTV;
    QTableView*  Estimation_Tab4_ExponentTV;
    QTableView*  Estimation_Tab4_ExponentMinTV;
    QTableView*  Estimation_Tab4_ExponentMaxTV;
    QSplitter*   Estimation_Tab4_MainSP;
    QSplitter*   Estimation_Tab4_MinSP;
    QSplitter*   Estimation_Tab4_MaxSP;
    QLabel*      Estimation_Tab4_PredationLBL;
    QLabel*      Estimation_Tab4_PredationMinLBL;
    QLabel*      Estimation_Tab4_PredationMaxLBL;
    QLabel*      Estimation_Tab4_HandlingLBL;
    QLabel*      Estimation_Tab4_HandlingMinLBL;
    QLabel*      Estimation_Tab4_HandlingMaxLBL;
    QLabel*      Estimation_Tab4_ExponentLBL;
    QLabel*      Estimation_Tab4_ExponentMinLBL;
    QLabel*      Estimation_Tab4_ExponentMaxLBL;
    QPushButton* Estimation_Tab4_PrevPB;
    QPushButton* Estimation_Tab4_NextPB;
    QPushButton* Estimation_Tab4_LoadPB;
    QPushButton* Estimation_Tab4_SavePB;
    QPushButton* Estimation_Tab4_ImportPB;
    QPushButton* Estimation_Tab4_ExportPB;
    QPushButton* Estimation_Tab4_TransposePB;
    QPushButton* Estimation_Tab4_TransposePB2;
    QSpinBox*    Estimation_Tab4_PredationSB;
    QSpinBox*    Estimation_Tab4_PredationSB2;
    QComboBox*   Estimation_Tab4_MinMaxCMB;
    QComboBox*   Estimation_Tab4_MinMaxCMB2;

    void getForms(std::string& predationForm,
                  std::string& competitionForm);
    int  getNumSpecies();
    std::vector<std::vector<QTableView *> > getGroupsOfTableViews();
    bool isNull();
    bool isTypeI();
    bool isTypeII();
    bool isTypeIII();
    void readSettings();
    void saveCSVFiles(std::vector<std::string>& allTableNames);
    void loadCSVFiles(std::vector<std::string>& allTableNames);
    std::vector<std::string> getAllTableNames();
    std::vector<QTableView*> getAllTableViews();
    void resetSpinBox(const std::pair<int,int>& nonZeroCell,
                      const QStandardItemModel* smodel,
                      const QStandardItemModel* smodel1,
                      const QStandardItemModel* smodel2);
    void saveTables(const bool& isTypeIII,
                    const std::vector<std::string>& SpeNames,
                    const std::vector<QTableView*>& tableViews,
                    const std::vector<std::string>& tableNames);
    void assignTableViewsBasedOnRangeType(const QString& rangeType,
                                          QTableView* possibleTableView1,
                                          QTableView* possibleTableView2,
                                          QTableView*& tableView1,
                                          QTableView*& tableView2);

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
     * @brief Callback invoked when the user clicks the Export button to save a .csv file
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user clicks the Import button to load a .csv file
     */
    void callback_ImportPB();
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
     * @brief Callback invoked when the user changes one of the two range type combo boxes (it just updates the other one)
     * @param rangeType : range type changed to (min/max, min only, or max only)
     */
    void callback_MinMaxCMB(QString rangeType);
    /**
     * @brief Callback invoked when user sets the Competition min/max range percent spin box
     * @param value : integer percent value to make the min/max range difference (i.e., 2 => min is 2% less than the initial values and max is 2% greater than the initial values)
     */
    void callback_PctRangeSB(int value);
    /**
     * @brief Callback invoked when the user clicks the T button. This will cause the data in every Competition matrix to be transposed.
     */
    void callback_TransposePB();

};

#endif // NMFESTIMATIONTAB4_H
