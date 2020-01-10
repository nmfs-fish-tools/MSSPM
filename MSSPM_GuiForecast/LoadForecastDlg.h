/** @file LoadForecastDlg.h
 * @brief GUI definition for the LoadForecastDlg widget class
 *
 * This file is the dialog that appears when a user wishes to select a Forecast
 * to load.
 */

#pragma once

/**
 * @brief Dialog to Load a Forecast
 *
 * The user can load a previously saved Forecast. To delete
 * a Forecast, the user right clicks over a saved Forecast and
 * selects Delete.
 */
class LoadForecastDlg : public QDialog
{
    Q_OBJECT

private:
    QDialogButtonBox*        m_ButtonBox;
    nmfDatabase*             m_DatabasePtr;
    QString                  m_ForecastName;
    QLineEdit*               m_ForecastNameLE;
    QSpinBox*                m_ForecastRunLengthSB;
    QSpinBox*                m_ForecastNumRunsSB;
    QListWidget*             m_ForecastsLW;
    std::vector<std::string> m_ForecastNames;
    nmfLogger*               m_Logger;

public:
    /**
     * @brief Dialog to select Forecast to Load
     * @param title : title of the dialog
     * @param parent : pointer to the parent widget of this dialog
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param forecastName : name of Forecast
     * @param runLengthSB : Spin Box for the number of Years per Run
     * @param numRunsSB : Spin Box for the number of Runs in the current Forecast
     */
    LoadForecastDlg(const QString&     title,
                          QWidget*     parent,
                          nmfLogger*   logger,
                          nmfDatabase* databasePtr,
                          QLineEdit*   forecastName,
                          QSpinBox*    runLengthSB,
                          QSpinBox*    numRunsSB);
   ~LoadForecastDlg() {}

    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return
     */
    void loadWidgets();
    /**
     * @brief Gets the selected item the user has selected from the Load Forecast button
     * @return Returns the selected Forecast
     */
    std::string getSelectedItem();

public slots:
    void callback_LoadOk();
    void callback_itemDoubleClicked(QListWidgetItem* item);
    void callback_ContextMenu(QPoint pos);
    void callback_DeleteSelection();
};

