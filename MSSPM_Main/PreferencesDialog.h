#pragma once

#include "nmfDatabase.h"

#include <QTableWidget>
#include <QComboBox>

/**
 * @brief Preferences Dialog
 *
 * The Dialog contains the application preferences such as if the window color
 * scheme is light (the default) or dark.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;

    QVBoxLayout* MainLT;
    QHBoxLayout* BtnLT;
    QHBoxLayout* StyleLT;
    QLabel*      StyleLBL;
    QComboBox*   StyleCMB;
    QPushButton* CancelPB;
    QPushButton* OkPB;

signals:
    void SetStyleSheet(QString style);

public:
    /**
     * @brief PreferencesDialog : class constructor
     * @param parent : the parent widget (typically the main window)
     * @param databasePtr : the pointer to the application database
     */
    PreferencesDialog(QWidget*     parent,
                      nmfDatabase* databasePtr);
    virtual ~PreferencesDialog() {}

    void loadWidgets();

private Q_SLOTS:
    void callback_StyleCMB(QString algorithm);
};

