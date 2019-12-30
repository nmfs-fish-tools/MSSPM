#ifndef NMFFORECASTTAB2_H
#define NMFFORECASTTAB2_H


/**
 * @brief Forecast Harvest Data
 *
 * This allows the user to enter Harvest data for the years of the
 * Forecast. Multiplier widgets help the user in calculating Harvest
 * data based upon the previous year's Harvest data.
 */
class nmfForecast_Tab2: public QObject
{
    Q_OBJECT

private:
    nmfDatabase*        m_DatabasePtr;
    std::string         m_HarvestType;
    nmfLogger*          m_Logger;
    std::string         m_ProjectSettingsConfig;
    std::string         m_ProjectDir;
    QStandardItemModel* m_SModel;

    QTabWidget*     Forecast_Tabs;
    QWidget*        Forecast_Tab2_Widget;
    QTableView*     Forecast_Tab2_HarvestTV;
    QGroupBox*      Forecast_Tab2_HarvestGB;
    QPushButton*    Forecast_Tab2_PrevPB;
    QPushButton*    Forecast_Tab2_NextPB;
    QPushButton*    Forecast_Tab2_LoadPB;
    QPushButton*    Forecast_Tab2_SavePB;
    QLineEdit*      Forecast_Tab1_NameLE;
    QCheckBox*      Forecast_Tab2_MultiplierCB;
    QComboBox*      Forecast_Tab2_MultiplierCMB;
    QDoubleSpinBox* Forecast_Tab2_MultiplierDSB;
    QCheckBox*      Forecast_Tab2_AutoSaveCB;

    bool restoreData(int minRow, int minCol, int maxRow, int maxCol);
    void readSettings();
    void saveHarvestData(bool verbose);
    void setHarvestType(std::string harvestType);


public:
    /**
     * @brief nmfForecast_Tab2 : class constructor
     * @param tabs : the tab widget into which this Forecast tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfForecast_Tab2(QTabWidget*  tabs,
                     nmfLogger*   logger,
                     nmfDatabase* databasePtr,
                     std::string& projectDir);
    virtual ~nmfForecast_Tab2();

    /**
     * @brief Clears all GUI widgets
     */
    void clearWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();

signals:
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
//  void callback_HarvestFormChanged(QString harvestForm);
    /**
     * @brief Callback invoked when user check the auto save checkbox. This checkbox saves the Harvest data after each edit and then automatically runs a Forecast.
     * @param checked : Boolean signifying the checked state of the checkbox
     */
    void callback_AutoSaveCB(bool checked);
    /**
     * @brief Callback invoked when user checks the Multiplier widget. This checkbox enables the user
     * to modify selected table rows either by a constant multiplier or by a multiplier and the previous row's value.
     * @param checked : checked state of the Multiplier widget
     */
    void callback_MultiplierCB(bool checked);
    /**
     * @brief Callback invoked when user changes the Multiplier value
     * @param value : value of new harvest data multiplier
     */
    void callback_MultiplierChangedDSB(double value);
    /**
     * @brief Callback invoked when user changes the type of Multiplier
     * @param type : type of Multiplier selected (current values are Constant and Variable)
     */
    void callback_MultiplierChangedCMB(QString type);
    /**
     * @brief Callback invoked when user changes the Harvest table selection.  As the user changes the
     * selection, the multiplier type and value are used to update the selection.
     * @param sel : the newly selected items
     * @param desel : the newly deselected items
     */
    void callback_SelectionChanged(const QItemSelection &sel, const QItemSelection &desel);
};

#endif // NMFFORECASTTAB2_H
