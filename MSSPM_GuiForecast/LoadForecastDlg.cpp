#include "nmfForecastTab01.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

LoadForecastDlg::LoadForecastDlg(const QString& title,
                                 QWidget*       parent,
                                 nmfLogger*     logger,
                                 nmfDatabase*   databasePtr,
                                 const std::string& projectName,
                                 QLineEdit*     forecastNameLE,
                                 QSpinBox*      forecastRunLengthSB,
                                 QSpinBox*      forecastNumRunsSB)
    : QDialog(parent)
{
    QLabel *listLabel = new QLabel("Saved Forecasts");

    m_Logger              = logger;
    m_DatabasePtr         = databasePtr;
    m_ProjectName         = projectName;
    m_ForecastNameLE      = forecastNameLE;
    m_ForecastName        = forecastNameLE->text();
    m_ForecastRunLengthSB = forecastRunLengthSB;
    m_ForecastNumRunsSB   = forecastNumRunsSB;

    m_ForecastsLW = new QListWidget();
    m_ForecastsLW->setContextMenuPolicy(Qt::CustomContextMenu);

    m_ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(listLabel);
    mainLayout->addWidget(m_ForecastsLW);
    mainLayout->addWidget(m_ButtonBox);
    setLayout(mainLayout);
    setWindowTitle(title);

    connect(m_ButtonBox,   SIGNAL(accepted()),    this, SLOT(callback_LoadOk()));
    connect(m_ButtonBox,   SIGNAL(rejected()),    this, SLOT(reject()));
    connect(m_ForecastsLW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,          SLOT(callback_itemDoubleClicked(QListWidgetItem*)));
    connect(m_ForecastsLW, SIGNAL(customContextMenuRequested(QPoint)),
            this,          SLOT(callback_ContextMenu(QPoint)));

    m_ButtonBox->setFocus();
    m_ForecastNameLE->setClearButtonEnabled(true);

    loadWidgets();
}

void
LoadForecastDlg::callback_ContextMenu(QPoint pos)
{
    QMenu contextMenu("", m_ForecastsLW);
    QAction actionDelete("Delete", this);
    actionDelete.setToolTip("Delete selected Forecast");
    actionDelete.setStatusTip("Delete selected Forecast");

    connect(&actionDelete, SIGNAL(triggered()),
            this,          SLOT(callback_DeleteSelection()));

    contextMenu.addAction(&actionDelete);
    contextMenu.exec(m_ForecastsLW->viewport()->mapToGlobal(pos));
}

void
LoadForecastDlg::callback_DeleteSelection()
{
    std::string cmd;
    QString ForecastToDelete = m_ForecastsLW->selectedItems()[0]->text();
    int selectedRow = m_ForecastsLW->currentRow();
    QMessageBox::StandardButton reply;
    std::string errorMsg;
    QString msg  = "\nThis will delete forecast:  " + ForecastToDelete +
                   " and its associated data.\n\nOK to delete?\n";
    std::vector<std::string> ForecastTables = {"ForecastBiomass",
                                               "ForecastBiomassMonteCarlo",
                                               "ForecastHarvestCatch",
                                               "ForecastHarvestEffort",
                                               "ForecastHarvestExploitation",
                                               "ForecastUncertainty",
                                               "Forecasts"};

    reply = QMessageBox::question(this, tr("Delete Forecast"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        for (std::string ForecastTable : ForecastTables) {
            cmd = "DELETE FROM " + ForecastTable +  " WHERE ProjectName = '" +
                  m_ProjectName + "' AND ForecastName = '" +
                  ForecastToDelete.toStdString() + "'";
            errorMsg = m_DatabasePtr->nmfUpdateDatabase(cmd);
            if (nmfUtilsQt::isAnError(errorMsg)) {
                m_Logger->logMsg(nmfConstants::Error,"callback_DeleteSelection: DELETE error: " + errorMsg);
                m_Logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                msg = "\nCouldn't delete records from table: " + QString::fromStdString(ForecastTable);
                QMessageBox::warning(this, "Error", msg, QMessageBox::Ok);
                return;
            }
        }
        loadWidgets();
        // Reset selection
        if (selectedRow < m_ForecastsLW->count()) {
            m_ForecastsLW->setCurrentRow(selectedRow);
        } else {
            m_ForecastsLW->setCurrentRow(m_ForecastsLW->count()-1);
        }
        // Clean any widgets if forecast deleted was current one
        if (ForecastToDelete == m_ForecastNameLE->text()) {
           m_ForecastNameLE->setText("");
           m_ForecastRunLengthSB->setValue(1);
           m_ForecastNumRunsSB->setValue(1);
        }
    }
}

void
LoadForecastDlg::loadWidgets()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string forecastName;
    QListWidgetItem *item;
    QList<QListWidgetItem*> items;

    m_ForecastsLW->clear();

    fields   = {"ForecastName"};
    queryStr = "SELECT ForecastName from Forecasts WHERE ProjectName = '" + m_ProjectName + "'";
    dataMap  = m_DatabasePtr->nmfQueryDatabase(queryStr, fields);
    for (unsigned i=0; i<dataMap["ForecastName"].size(); ++i) {
        forecastName = dataMap["ForecastName"][i];
        item = new QListWidgetItem();
        item->setText(QString::fromStdString(forecastName));
        m_ForecastsLW->addItem(item);
    }

    // Select the currently visible name if it exists in list
    if (! m_ForecastName.isEmpty()) {
        items = m_ForecastsLW->findItems(m_ForecastName,Qt::MatchExactly);
        if (items.count() > 0) {
            m_ForecastsLW->setCurrentItem(items[0]);
        }
    }
}

std::string
LoadForecastDlg::getSelectedItem()
{
   return m_ForecastsLW->selectedItems()[0]->text().toStdString();
}

void
LoadForecastDlg::callback_LoadOk()
{
    if (m_ForecastsLW->selectedItems().size() == 0) {
        QMessageBox::information(this, "NULL Selection",
                                 "\nPlease select a Forecast to load or click Cancel.\n");
        return;
    }
    accept();
}

void
LoadForecastDlg::callback_itemDoubleClicked(QListWidgetItem* item)
{
    callback_LoadOk();
}
