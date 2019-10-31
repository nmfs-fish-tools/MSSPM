#ifndef NMFESTIMATIONTAB4_H
#define NMFESTIMATIONTAB4_H


/**
 * @brief Predation Tables
 *
 * These tables allow user to enter and modify minimum and
 * maximum coefficients for Predation Effect (rho), Handling Time (h),
 * and Predation Exponent (b).
 */
class nmfEstimation_Tab4: public QObject
{
    Q_OBJECT

    nmfLogger*               m_logger;
    nmfDatabase*             m_databasePtr;
    std::string              m_predationForm;
    std::string              m_ProjectDir;
    std::string              m_ProjectSettingsConfig;
    std::vector<QTableView*> m_QTableViews1d;
    std::vector<QTableView*> m_QTableViews2d;
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

public:
    /**
     * @brief nmfEstimation_Tab4 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab4(QTabWidget  *tabs,
                       nmfLogger   *logger,
                       nmfDatabase *databasePtr,
                       std::string &projectDir);
    virtual ~nmfEstimation_Tab4();

    bool loadWidgets();
    void clearWidgets();
    void readSettings();
    void getAlgorithm(std::string &Algorithm);
    std::string getForms();
    void getForms(std::string& predationForm,
                  std::string& competitionForm);
    int getNumSpecies();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_PredationFormChanged(QString predationForm);
    void callback_minSplitterMoved(int pos, int index);
    void callback_maxSplitterMoved(int pos, int index);
    void callback_EstimateChecked(int state);
};

#endif // NMFESTIMATIONTAB4_H
