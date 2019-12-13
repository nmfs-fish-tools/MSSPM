#ifndef MSSPM_GUIOUTPUTCONTROLS_H
#define MSSPM_GUIOUTPUTCONTROLS_H

#include <QStringListModel>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>


class MSSPM_GuiOutputControls: public QObject
{

    Q_OBJECT

    nmfLogger*         m_logger;
    nmfDatabase*       m_databasePtr;
    std::string        m_ProjectDir;
    std::string        m_ProjectSettingsConfig;
    QStringListModel*  m_SpeciesOrGuildModel;
    QHash<QString,int> m_SpeciesHash;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;

    QLabel*      OutputTypeLBL;
    QLabel*      OutputSpeciesLBL;
    QLabel*      OutputSpeListLBL;
    QLabel*      OutputMethodsLBL;
    QLabel*      OutputParametersLBL;
    QLabel*      OutputScenariosLBL;
    QLabel*      OutputAgeListLBL;
    QLabel*      OutputYAxisMinLBL;
    QLabel*      OutputScaleLBL;
    QLabel*      OutputLineBrightnessLBL;
    QSlider*     OutputLineBrightnessSL;
    QComboBox*   OutputSpeciesCMB;
    QComboBox*   OutputParametersCMB;
    QComboBox*   OutputMethodsCMB;
    QComboBox*   OutputScenariosCMB;
    QCheckBox*   OutputParametersCB;
    QPushButton* OutputParametersPB;
    QComboBox*   OutputTypeCMB;
    QSlider*     OutputYAxisMinSL;
    QComboBox*   OutputScaleCMB;
    QListView*   OutputAgeListLV;
    QListView*   OutputSpeListLV;
    QCheckBox*   OutputShowMSYCB;
    QCheckBox*   OutputShowFMSYCB;
    QCheckBox*   OutputShowBMSYCB;
    QLineEdit*   OutputShowBMSYLE;
    QLineEdit*   OutputShowMSYLE;
    QLineEdit*   OutputShowFMSYLE;
    QGroupBox*   ControlsGroupBox;

    void initConnections();
    void initWidgets();
    bool getSpecies(int& NumSpecies, QStringList& SpeciesList);
    bool getGuilds( int& NumGuilds,  QStringList& GuildList);
    bool isAggProd();
    void readSettings();

public:
    /**
     * @brief MSSPM_GuiOutputControls : class constructor
     * @param controlsGroupBox : group box widget containing the output control widgets
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    MSSPM_GuiOutputControls(QGroupBox*   controlsGroupBox,
                            nmfLogger*   logger,
                            nmfDatabase* databasePtr,
                            std::string& projectDir);
    virtual ~MSSPM_GuiOutputControls();

    void            clearOutputBMSY();
    void            clearOutputMSY();
    void            clearOutputFMSY();
    void            displayMohnsRho();
    void            enableMSYWidgets(bool state);
    void            enableBrightnessWidgets(bool state);
    QString         getOutputParameter();
    QString         getOutputScale();
    QString         getOutputType();
    QString         getOutputScenario();
    QString         getOutputSpecies();
    QString         getOutputMethod();
    int             getOutputSpeciesIndex();
    QWidget*        getListViewViewport();
    QModelIndexList getListViewSelectedIndexes();
    double          getOutputBrightnessFactor();
    int             getYMinSliderVal();
    QString         getOutputDiagnostics();
    int             getSpeciesNumFromName(QString SpeciesName);
    bool            isCheckedOutputBMSY();
    bool            isCheckedOutputMSY();
    bool            isCheckedOutputFMSY();
    void            loadSpeciesControlWidget();
    void            loadWidgets();
    void            loadSortedForecastLabels();
    void            refresh();
    void            refreshScenarios();
    void            saveSettings();
    void            setOutputSpeciesIndex(int index);
    void            setOutputType(QString type);
    void            setTextOutputBMSY(QString text);
    void            setTextOutputMSY(QString text);
    void            setTextOutputFMSY(QString text);
    void            setForecastLabels(std::map<QString,QStringList>& SortedForecastLabelsMap);
    void            setOutputParametersCB(bool checked);
    void            setOutputDiagnostics(QString method);
    void            setForMohnsRho();

signals:
    void EnableFilterButtons(bool state);
    void ForecastLineBrightnessChanged(double brightnessFactor);
    void NoSystemsSet();
    void ResetFilterButtons();
    void SelectCenterSurfacePoint();
    void SetChartView2d(bool isVisible);
    void ShowChart(QString OutputType,QString OutputSpecies);
    void ShowChartMultiScenario(QStringList SortedForecastLabels);
    void ShowDiagnosticsChart3d();
    void ShowChartMohnsRho();
    void UpdateSummaryStatistics();
    void YAxisMinValueChanged(int value);

public slots:
    void callback_OutputTypeCMB(QString outputType);
    void callback_OutputSpeciesCMB(QString outputSpecies);
    void callback_OutputMethodsCMB(QString method);
    void callback_OutputParametersCMB(QString parameter);
    void callback_OutputScenariosCMB(QString scenario);
    void callback_OutputParametersCB(int state);
    void callback_OutputShowBMSYCB(int val);
    void callback_OutputMSYCB(int val);
    void callback_OutputShowFMSYCB(int val);
    void callback_OutputScaleCMB(QString scale);
    void callback_OutputLineBrightnessSL(int value);
    void callback_OutputYAxisMinSL(int value);
    void callback_OutputParametersPB();
    void callback_ResetOutputWidgetsForAggProd();
    void callback_loadScenariosWidget();
    void callback_SetOutputScenario(QString scenario);
};




#endif // MSSPM_GUIOUTPUTCONTROLS_H
