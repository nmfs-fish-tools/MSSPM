#ifndef NMFFORECASTTAB2_H
#define NMFFORECASTTAB2_H


/**
 * @brief The Forecast Catch Data
 *
 * This allows the user to enter Catch data for the years of the
 * Forecast. Multiplier widgets help the user in calculating Catch
 * data based upon the previous year's Catch data.
 */
class nmfForecast_Tab2: public QObject
{
    Q_OBJECT

private:
    QStandardItemModel* m_smodel;
    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    std::string         m_harvestType;
    std::string         m_ProjectSettingsConfig;
    std::string         m_ProjectDir;

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
    void setHarvestType(std::string harvestType);
    void getAlgorithms(std::string& algorithm,
                       std::string& minimizer,
                       std::string& objectiveCriterion,
                       std::string& scaling);

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

    bool loadWidgets();

signals:
    void RunForecast(std::string ForecastName,
                     bool GenerateBiomass);
public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_SavePB(bool verbose);
    void callback_PrevPB();
    void callback_NextPB();
    void callback_HarvestFormChanged(QString harvestForm);
    void callback_AutoSaveCB(bool checked);
    void callback_MultiplierCB(bool checked);
    void callback_MultiplierChangedDSB(double value);
    void callback_MultiplierChangedCMB(QString type);
    void callback_SelectionChanged(const QItemSelection &sel, const QItemSelection &desel);
};

#endif // NMFFORECASTTAB2_H
