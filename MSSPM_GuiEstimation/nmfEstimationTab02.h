#ifndef NMFESTIMATIONTAB2_H
#define NMFESTIMATIONTAB2_H

/**
 * @brief Catch data
 *
 * This table represents the Catch data for the year range and for each
 * Species. The user should save newly modified Catch data prior to
 * progressing to the next tab.
 */
class nmfEstimation_Tab2: public QObject
{
    Q_OBJECT

    nmfLogger*                        m_logger;
    nmfDatabase*                      m_databasePtr;
    std::string                       m_ProjectDir;
    std::string                       m_ProjectSettingsConfig;
    std::string                       m_harvestType;
    std::map<std::string,std::string> m_GroupBoxTitle;
    QStandardItemModel*               m_smodel;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab2_Widget;
    QTableView*  Estimation_Tab2_CatchTV;
    QGroupBox*   Estimation_Tab2_CatchGB;
    QPushButton* Estimation_Tab2_PrevPB;
    QPushButton* Estimation_Tab2_NextPB;
    QPushButton* Estimation_Tab2_LoadPB;
    QPushButton* Estimation_Tab2_SavePB;

public:
    /**
     * @brief nmfEstimation_Tab2 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab2(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab2();

    bool loadWidgets();
    bool loadWidgets(QString MohnsRhoLabel);
    void clearWidgets();
    void readSettings();
    void setHarvestType(std::string harvestType);

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_HarvestFormChanged(QString harvestForm);

};

#endif // NMFESTIMATIONTAB2_H
