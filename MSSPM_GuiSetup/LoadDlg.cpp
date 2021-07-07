
#include "nmfSetupTab04.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

LoadDlg::LoadDlg(const QString&     title,
                       QWidget*     parent,
                       nmfLogger*   theLogger,
                       nmfDatabase* theDatabasePtr,
                 const std::string& projectName,
                 const QString&     currentConfig)
    : QDialog(parent)
{
    QLabel *label0 = new QLabel("");
    m_logger      = theLogger;
    m_databasePtr = theDatabasePtr;
    m_SettingsLW  = new QListWidget();
    m_ProjectName = projectName;
    m_SettingNames.clear();

    reloadSystemsList();

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(label0);
    mainLayout->addWidget(m_SettingsLW);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);
    setWindowTitle(title);

    // Add context menu for delete item
    m_SettingsLW->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_SettingsLW,SIGNAL(customContextMenuRequested(QPoint)),
            this,      SLOT(callback_ShowContextMenu(QPoint)));

    connect(m_buttonBox,  SIGNAL(accepted()),    this, SLOT(callback_LoadOk()));
    connect(m_buttonBox,  SIGNAL(rejected()),    this, SLOT(reject()));
    connect(m_SettingsLW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,       SLOT(callback_ItemDoubleClicked(QListWidgetItem*)));

    m_buttonBox->setFocus();

    // Set currentConfig
    QList<QListWidgetItem*> items = m_SettingsLW->findItems(currentConfig,Qt::MatchExactly);
    if (items.size() > 0) {
        m_SettingsLW->setCurrentItem(items[0]);
    }    

}

void
LoadDlg::callback_ShowContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = m_SettingsLW->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Delete", this, SLOT(callback_DeleteItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}


void
LoadDlg::callback_ItemDoubleClicked(QListWidgetItem* item)
{
    callback_LoadOk();
}



void
LoadDlg::reloadSystemsList()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string name;
    QListWidgetItem *item;

    m_SettingsLW->clear();
    m_SettingNames.clear();

    fields     = {"ProjectName","ModelName"};
    queryStr   = "SELECT ProjectName,ModelName FROM Models WHERE ProjectName='" + m_ProjectName + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned int i=0; i<dataMap["ModelName"].size(); ++i) {
        name = dataMap["ModelName"][i];
        m_SettingNames.push_back(name);
        item = new QListWidgetItem();
        item->setText(QString::fromStdString(name));
        m_SettingsLW->addItem(item);
    }
}

void
LoadDlg::callback_LoadOk()
{
    if (m_SettingsLW->selectedItems().size() == 0) {
        QMessageBox::information(this, "NULL Selection",
                                 "\nPlease select a Settings configuration to load or click Cancel.\n");
        return;
    }
    saveSettings();
    accept();
}


void
LoadDlg::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    settings->setValue("Name", m_SettingsLW->currentItem()->text());
    settings->endGroup();

    delete settings;

    updateWindowTitle();
}

void
LoadDlg::updateWindowTitle()
{
    std::string ProjectSettingsConfig;
    std::string winTitle;

    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSSPM::SettingsDirWindows,"MSSPM");

    settings->beginGroup("Settings");
    ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    delete settings;

    winTitle = "MSSPM (" + m_ProjectName + " - " + ProjectSettingsConfig + ")";

    foreach (QWidget *w, qApp->topLevelWidgets()) {
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w)) {
            mainWin->setWindowTitle(QString::fromStdString(winTitle));
        }
    }
}


void
LoadDlg::callback_DeleteItem()
{
    QMessageBox::StandardButton reply;
    QString currentItem = m_SettingsLW->currentItem()->text();
    QString msg = "\nOK to delete all data associated with Model: " + currentItem + " ?";

    reply = QMessageBox::question(this, tr("Delete Model"),
                       tr(msg.toLatin1()),
                       QMessageBox::No|QMessageBox::Yes,
                       QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        m_databasePtr->cleanTables(m_logger,m_ProjectName,currentItem.toStdString());
        reloadSystemsList();
        emit ClearModelName();
        QMessageBox::information(this, "Delete", "\nSuccessfully deleted Model.\n");
    }
}
