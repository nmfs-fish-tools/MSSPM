#include "TableNamesDialog.h"
#include "nmfDatabase.h"
#include <QStandardItemModel>
#include <QTableWidget>
#include <QUiLoader>


TableNamesDialog::TableNamesDialog(QWidget *parent,
                                   nmfDatabase* databasePtr) :
    QDialog(parent)
{
    // the constructor will initialize all of the widgets and layout
    m_databasePtr = databasePtr;
    QUiLoader m_loader;
    QFile file(":/forms/Main/TableNamesDlg.ui");
    file.open(QFile::ReadOnly);
    m_TableNamesWidget = m_loader.load(&file,parent);
    file.close();
    m_TableNamesOkPB = m_TableNamesWidget->findChild<QPushButton*>("TableNamesOkPB");
    m_TableNamesTV   = m_TableNamesWidget->findChild<QTableView *>("TableNamesTV");
    m_DatabaseNameLB = m_TableNamesWidget->findChild<QLabel*>("DatabaseNameLB");

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_TableNamesWidget);
    this->adjustSize();
    this->setMinimumWidth(400);
    this->setMinimumHeight(300);
    this->setLayout(m_layout);
    this->setWindowTitle("Table Names");

    connect(m_TableNamesOkPB, SIGNAL(released()),
            this,             SLOT(close()));
}

// This function will be called whenever a user tries to view the tables in the current database.
void
TableNamesDialog::loadTableNames(const std::string& ProjectDatabase)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    int NumTables=0;
    QStandardItem* item1;
    QStandardItem* item2;
    QStringList HeaderList = {"Name","Size (MB)"};

    fields    = {"Name","Size (MB)"};
    queryStr  = "SELECT TABLE_NAME AS `Name`, ROUND((DATA_LENGTH + INDEX_LENGTH) / 1024 / 1024, 2) AS `Size (MB)` FROM information_schema.TABLES WHERE TABLE_SCHEMA = \"" + ProjectDatabase + "\"";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumTables = dataMap["Name"].size();

    QString label = "<strong>Database:</strong> " + QString::fromStdString(ProjectDatabase) + ", ";
    label += "<strong>Num tables:</strong> " + QString::number(NumTables);
    m_DatabaseNameLB->setText(label);
    QStandardItemModel* smodel = new QStandardItemModel(NumTables,2); // 2 columns: name and size
    for (int i=0; i<NumTables; ++i) {
        item1 = new QStandardItem(QString::fromStdString(dataMap["Name"][i]));
        item2 = new QStandardItem(QString::fromStdString(dataMap["Size (MB)" ][i]));
        item2->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(i,0,item1);
        smodel->setItem(i,1,item2);
    }

    smodel->setHorizontalHeaderLabels(HeaderList);
    m_TableNamesTV->setModel(smodel);
    m_TableNamesTV->setSortingEnabled(true);
    m_TableNamesTV->sortByColumn(0,Qt::SortOrder::AscendingOrder);
    m_TableNamesTV->resizeColumnsToContents();
}







