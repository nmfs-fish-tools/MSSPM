#ifndef MSSPM_GUIMANAGERMODE_H
#define MSSPM_GUIMANAGERMODE_H

#include <QDial>

QT_CHARTS_USE_NAMESPACE

/**
 * @brief The MSSPM_GuiManagerMode class
 */
class MSSPM_GuiManagerMode : public QObject
{
    Q_OBJECT

private:
    QSlider*     MModeYearsPerRunHS;
    QSlider*     MModeRunsPerForeHS;
    QLineEdit*   MModeYearsPerRunLE;
    QLineEdit*   MModeRunsPerForeLE;
    QLineEdit*   MModePercMSYLE;
    QLineEdit*   MModeRParamLE;
    QLineEdit*   MModeKParamLE;
    QLineEdit*   MModeCParamLE;
    QDial*       MModePercMSYDL;
    QDial*       MModeRParamDL;
    QDial*       MModeKParamDL;
    QDial*       MModeCParamDL;
    QComboBox*   MModeSpeciesCMB;
    QWidget*     MModeHarvestChartWidget;
    QWidget*     MModeWindowWidget;
    QPushButton* MModeForecastRunPB;

    QChart*      m_MModeHarvestChartWidget;
    QChart*      m_MModeOutputChartWidget;
    nmfDatabase* m_DatabasePtr;
    nmfLogger*   m_Logger;

signals:
    void KeyPressed(QKeyEvent* event);
    void MouseMoved(QMouseEvent* event);

public:
    /**
     * @brief MSSPM_GuiManagerMode
     * @param MModeWidget
     */
    MSSPM_GuiManagerMode(nmfDatabase* databasePtr, nmfLogger* logger, QWidget* MModeWidget);
    ~MSSPM_GuiManagerMode();

    /**
     * @brief setupConnections
     */
    void setupConnections();

    /**
     * @brief setupConnections
     */
    void setData(const QStringList& speciesList);

public Q_SLOTS:
    /**
     * @brief callback_YearsPerRun
     * @param value
     */
    void callback_YearsPerRun(int value);
    /**
     * @brief callback_RunsPerFore
     * @param value
     */
    void callback_RunsPerFore(int value);
    /**
     * @brief callback_PercMSY
     * @param value
     */
    void callback_PercMSY(int value);
    /**
     * @brief callback_RParam
     * @param value
     */
    void callback_RParam(int value);
    /**
     * @brief callback_KParam
     * @param value
     */
    void callback_KParam(int value);
    /**
     * @brief callback_CParam
     * @param value
     */
    void callback_CParam(int value);
    void callback_ForecastRun();
    void callback_keyPressed(QKeyEvent* event);
    void callback_mouseMoved(QMouseEvent* event);

    void saveUncertaintyParameters();
};

#endif // MSSPM_GUIMANAGERMODE_H
