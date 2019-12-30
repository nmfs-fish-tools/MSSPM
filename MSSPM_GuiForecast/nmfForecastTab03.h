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
    QString             m_Alpha;
    QString             m_BetaS;
    QString             m_BetaG;
    std::string         m_CompetitionForm;
    nmfDatabase*        m_DatabasePtr;
    std::map<std::string,std::vector<std::string> > m_FormMap;
    std::string         m_GrowthForm;
    std::string         m_HarvestForm;
    bool                m_IsAggProd;
    nmfLogger*          m_Logger;
    QStringList         m_ParameterNames;
    std::string         m_PredationForm;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QString             m_Rho;
    QStandardItemModel* m_SModel;

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

    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();

signals:
    /**
     * @brief Signal emitted to run a new Forecast
     * @param ForecastName : name of Forecast to run
     * @param GenerateBiomass : boolean specifying whether to generate the biomass data or Monte Carlo simulations
     */
    void RunForecast(std::string ForecastName,
                     bool GenerateBiomass);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user clicks Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user clicks Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user clicks Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user changes an item in the Uncertainty Parameters data table. If
     * the autosave button is checked, the Forecast will update after the user changes an item.
     * @param unusedA : unused parameter
     * @param unusedB : unused parameter
     */
    void callback_ItemChanged(const QModelIndex& unusedA,
                              const QModelIndex& unusedB);
    /**
     * @brief Callback invoked when user right clicks over the Uncertainty Parameters data table.
     * A Popup menu will appear asking the user if they want to clear the selected table items.
     * @param point : point on the GUI at which the user clicked (needed so as to know where to draw the popup GUI)
     */
    void callback_ContextMenu(QPoint point);
    /**
     * @brief Callback invoked when the user selects clear from the right click context menu
     */
    void callback_ClearSelection();
};

#endif // NMFFORECASTTAB3_H
