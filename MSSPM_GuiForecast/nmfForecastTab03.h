#ifndef NMFFORECASTTAB3_H
#define NMFFORECASTTAB3_H


/**
 * @brief Uncertainty Parameters
 *
 * This allows the user to enter Uncertainty parameters for the
 * Forecast. These values represent parameter percentage error.
 */
class nmfForecast_Tab3: public QObject
{
    Q_OBJECT

private:
    QStandardItemModel* m_smodel;
    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    bool                m_isAggProd;
    QString             m_alpha;
    QString             m_betaS;
    QString             m_betaG;
    QString             m_rho;
    std::string         m_ProjectSettingsConfig;
    std::string         m_ProjectDir;
    std::string         m_GrowthForm;
    std::string         m_HarvestForm;
    std::string         m_CompetitionForm;
    std::string         m_PredationForm;
    QStringList         m_ParameterNames;
    std::map<std::string,std::vector<std::string> > m_FormMap;

    QTabWidget*  Forecast_Tabs;
    QWidget*     Forecast_Tab3_Widget;
    QTableView*  Forecast_Tab3_UncertaintyTV;
    QPushButton* Forecast_Tab3_PrevPB;
    QPushButton* Forecast_Tab3_NextPB;
    QPushButton* Forecast_Tab3_LoadPB;
    QPushButton* Forecast_Tab3_SavePB;
    QGroupBox*   Forecast_Tab2_HarvestGB;
    QLineEdit*   Forecast_Tab1_NameLE;
    QCheckBox*   Forecast_Tab3_AutoSaveRunCB;

    void adjustColumnVisibility();
    void clearUncertaintyTable(int& NumSpecies,
                               int& NumParameters);
    QString harvestType();
    void readSettings();

public:
    /**
     * @brief nmfForecast_Tab3 : class constructor
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab3(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab3();

    bool loadWidgets();

signals:
    void RunForecast(std::string ForecastName,
                     bool GenerateBiomass);

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_ItemChanged(const QModelIndex& unusedA,
                              const QModelIndex& unusedB);
    void callback_ContextMenu(QPoint point);
    void callback_ClearSelection();
};

#endif // NMFFORECASTTAB3_H
