#ifndef NMFESTIMATIONTAB5_H
#define NMFESTIMATIONTAB5_H

/**
 * @brief Observation Data
 *
 * This table allows the user to enter and modify Observed Biomass
 * data for each Species for every year in the year range.
 */
class nmfEstimation_Tab5: public QObject
{

    Q_OBJECT

    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QStandardItemModel* m_SModelBiomass;
    QStandardItemModel* m_SModelCovariates;

    QTabWidget*  Estimation_Tabs;
    QWidget*     Estimation_Tab5_Widget;
    QTableView*  Estimation_Tab5_BiomassTV;
    QTableView*  Estimation_Tab5_CovariatesTV;
    QPushButton* Estimation_Tab5_PrevPB;
    QPushButton* Estimation_Tab5_NextPB;
    QPushButton* Estimation_Tab5_LoadPB;
    QPushButton* Estimation_Tab5_SavePB;
    QGroupBox*   Estimation_Tab5_CovariatesGB;

    void readSettings();

public:
    /**
     * @brief nmfEstimation_Tab5 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab5(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab5();

    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @param MohnsRhoLabel : the Mohns Rho name used to identify the data if the run is part of a Retrospective Analysis
     * @return
     */
    bool loadWidgets(QString MohnsRhoLabel);

signals:
    /**
     * @brief Signal notifies any other GUI showing similar data to refresh itself
     */
    void ReloadSpecies();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user clicks Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user clicks Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user clicks Save button
     */
    void callback_SavePB();
//  void callback_SetAlgorithm(QString algorithm);
    /**
     * @brief Callback invoked when user saves a new model in Setup -> Model Setup
     */
    void callback_UpdateInitialObservedBiomass();
};

#endif // NMFESTIMATIONTAB5_H
