/** @file LoadDlg.h
 * @brief GUI definition for the Settings File LoadDlg widget
 *
 * This file contains the GUI definitions for the Settings File Load Dialog. This
 * dialog allows the user to load a previously saved Settings configuration file.
 */

#pragma once

class QDialogButtonBox;

/**
 * @brief Defines the behavior and callback functionality for
 * the Load Systems Dialog used in Setup Tab page 4
 */
class LoadDlg : public QDialog
{
    Q_OBJECT

private:
    int               m_NumberOfRuns;
    int               m_RunLength;
    int               m_TimeStep;
    nmfLogger*        m_logger;
    nmfDatabase*      m_databasePtr;
    QDialogButtonBox* m_buttonBox;
    QListWidget*      m_SettingsLW;
    std::string       m_GrowthForm;
    std::string       m_HarvestForm;
    std::string       m_CompetitionForm;
    std::string       m_PredationForm;
    SystemData        m_data;
    std::vector<std::string> m_SettingNames;

    void reloadSystemsList();
    void saveSettings();
    void updateWindowTitle();

public:
    LoadDlg(const QString &title,
                  QWidget *parent,
                  nmfLogger* m_logger,
                  nmfDatabase* m_databasePtr,
            const QString &currentConfig);
   ~LoadDlg() {}

    /**
     * @brief Gets the database data affiliated with the current Model name
     * @param data : the data structure to load with the system data read from the database
     */
    void getSettingData(SystemData &data);

signals:
    /**
     * @brief Signal emitted when user deletes a System name
     */
    void ClearSystemName();

public slots:
    /**
     * @brief Callback invoked when user clicks the OK button
     */
    void callback_LoadOk();
    /**
     * @brief Callback invoked when user double clicks an item from the System names list
     * @param item : item selected by user
     */
    void callback_ItemDoubleClicked(QListWidgetItem* item);
    /**
     * @brief Callback invoked when user right clicks over an existing System name to reveal the popup Delete menu item
     * @param pos : position on the screen where the popup menu will appear
     */
    void callback_ShowContextMenu(const QPoint &pos);
    /**
     * @brief Callback invoked when user selects the Delete item from the popup context menu item
     */
    void callback_DeleteItem();
};

