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
    m_TableNamesWidget = m_loader.load(&file,parent);;
    file.close();
    m_TableNamesOkPB = m_TableNamesWidget->findChild<QPushButton*>("TableNamesOkPB");
    m_TableNamesLW   = m_TableNamesWidget->findChild<QListWidget*>("TableNamesLW");
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

    fields    = {"table_name"};
    queryStr  = "SELECT table_name FROM information_schema.tables WHERE ";
    queryStr += "table_schema = '" + ProjectDatabase + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumTables = dataMap["table_name"].size();
    m_TableNamesLW->clear();
    if (NumTables <= 0) {
        m_TableNamesLW->addItem(QString::fromStdString("No tables found in database: " + ProjectDatabase));
    } else {
        m_DatabaseNameLB->setText(QString::fromStdString(ProjectDatabase));
        for (int i=0; i<NumTables; ++i) {
            m_TableNamesLW->addItem(QString::fromStdString(std::to_string(i+1) + ". " + dataMap["table_name"][i]));
        }
    }
}







