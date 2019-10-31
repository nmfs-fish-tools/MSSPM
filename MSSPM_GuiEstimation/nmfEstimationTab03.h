#ifndef NMFESTIMATIONTAB3_H
#define NMFESTIMATIONTAB3_H


/**
 * @brief Competition Tables
 *
 * These tables allow user to enter and modify minimum and
 * maximum alpha and beta food Competition data.
 */
class nmfEstimation_Tab3: public QObject
{
    Q_OBJECT

private:
    nmfLogger*                       m_logger;
    nmfDatabase*                     m_databasePtr;
    std::string                      m_ProjectDir;
    std::string                      m_ProjectSettingsConfig;
    QString                          m_competitionForm;
    QStringList                      m_speciesNames;
    QStringList                      m_guildNames;
    std::vector<QStandardItemModel*> m_smodels;
    std::vector<QTableView*>         m_QTableViews;
    std::vector<std::string>         m_alphaTables;
    std::vector<std::string>         m_betaSpeciesTables;
    std::vector<std::string>         m_betaGuildsTables;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab3_Widget;
    QTableView*  Estimation_Tab3_CompetitionAlphaMinTV;
    QTableView*  Estimation_Tab3_CompetitionAlphaMaxTV;
    QTableView*  Estimation_Tab3_CompetitionBetaSpeciesMinTV;
    QTableView*  Estimation_Tab3_CompetitionBetaSpeciesMaxTV;
    QTableView*  Estimation_Tab3_CompetitionBetaGuildsMinTV;
    QTableView*  Estimation_Tab3_CompetitionBetaGuildsMaxTV;
    QLabel*      Estimation_Tab3_CompetitionAlphaMinLBL;
    QLabel*      Estimation_Tab3_CompetitionAlphaMaxLBL;
    QLabel*      Estimation_Tab3_CompetitionBetaSpeciesMinLBL;
    QLabel*      Estimation_Tab3_CompetitionBetaSpeciesMaxLBL;
    QLabel*      Estimation_Tab3_CompetitionBetaGuildsMinLBL;
    QLabel*      Estimation_Tab3_CompetitionBetaGuildsMaxLBL;
    QSplitter*   Estimation_Tab3_CompetitionMinSP;
    QSplitter*   Estimation_Tab3_CompetitionMaxSP;
    QPushButton* Estimation_Tab3_PrevPB;
    QPushButton* Estimation_Tab3_NextPB;
    QPushButton* Estimation_Tab3_SavePB;
    QPushButton* Estimation_Tab3_LoadPB;
    QCheckBox*   Estimation_Tab3_EstimateCB;

    int getNumSpecies();
    int getNumGuilds();

public:
    /**
     * @brief nmfEstimation_Tab3 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab3(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab3();

    bool loadWidgets();
    void clearWidgets();
    void readSettings();
    void getAlgorithm(std::string &Algorithm);

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_minSplitterMoved(int pos, int index);
    void callback_maxSplitterMoved(int pos, int index);
    void callback_CompetitionFormChanged(QString competitionForm);
    void callback_EstimateChecked(int state);
};

#endif // NMFESTIMATIONTAB3_H
