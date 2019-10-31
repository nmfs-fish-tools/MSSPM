#ifndef NMFESTIMATIONTAB5_H
#define NMFESTIMATIONTAB5_H

/**
 * @brief Biomass Table
 *
 * This table allows the user to enter and modify Observed Biomass
 * data for each Species for every year in the year range.
 */
class nmfEstimation_Tab5: public QObject
{

    Q_OBJECT

    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    QStandardItemModel* m_smodelBiomass;
    QStandardItemModel* m_smodelCovariates;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab5_Widget;
    QTableView*  Estimation_Tab5_BiomassTV;
    QTableView*  Estimation_Tab5_CovariatesTV;
    QPushButton* Estimation_Tab5_PrevPB;
    QPushButton* Estimation_Tab5_NextPB;
    QPushButton* Estimation_Tab5_LoadPB;
    QPushButton* Estimation_Tab5_SavePB;
    QGroupBox*   Estimation_Tab5_CovariatesGB;

public:
    /**
     * @brief nmfEstimation_Tab5 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab5(QTabWidget*  tabs,
                       nmfLogger*   m_logger,
                       nmfDatabase* m_databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab5();

    void clearWidgets();
    bool loadWidgets();
    bool loadWidgets(QString MohnsRhoLabel);
    void readSettings();

signals:
    void ReloadSpecies();

public Q_SLOTS:
    void callback_NextPB();
    void callback_PrevPB();
    void callback_LoadPB();
    void callback_SavePB();
    void callback_SetAlgorithm(QString algorithm);
    void callback_UpdateInitialObservedBiomass();
};

#endif // NMFESTIMATIONTAB5_H
