#include "EnsembleDialog.h"

#include <QStandardItemModel>
#include <QTableWidget>

EnsembleDialog::EnsembleDialog(QWidget *parent,
                               std::string projectDir) :
    QDialog(parent)
{
    m_ProjectDir = projectDir;
    m_SModel     = nullptr;
    m_Parent     = parent;

    m_ensembleTV = new QTableView();
//  QPushButton*  savePB     = new QPushButton("Save");
//  QPushButton*  refreshPB  = new QPushButton("Refresh");
    QPushButton*  cancelPB   = new QPushButton("Cancel");
    QHBoxLayout*  hLayt      = new QHBoxLayout;
    QVBoxLayout*  vLayt      = new QVBoxLayout;

    hLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hLayt->addWidget(cancelPB);
//  hLayt->addWidget(refreshPB);
//  hLayt->addWidget(savePB);
    hLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    vLayt->addWidget(m_ensembleTV);
    vLayt->addLayout(hLayt);
    setLayout(vLayt);
    setWindowTitle("Multi-Run/Ensemble Run Parameters");
    setMinimumWidth(800);

    connect(cancelPB,  SIGNAL(clicked()), this, SLOT(close()));
//  connect(refreshPB, SIGNAL(clicked()), this, SLOT(callback_refreshPB()));
//  connect(savePB,    SIGNAL(clicked()), this, SLOT(callback_savePB()));
}

bool
EnsembleDialog::loadWidgets(QString ensembleFile)
{
    QString fullPath = QDir(QString::fromStdString(m_ProjectDir)).filePath("outputData");
    fullPath = QDir(fullPath).filePath(ensembleFile);
    QFile file(fullPath);
    QStringList wholeFile = {};
    int numCols = 0;
    int numRows = 0;
    bool retv = true;
    QStandardItem* item;
    QStringList parts;
    QStringList headerList = {};

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream inputFile(&file);
        while (!inputFile.atEnd()) {
            QString line = inputFile.readLine();
            if (numRows == 0) {
                headerList = line.split(",");
                numCols = int(headerList.size());
            }
            wholeFile << line;
            ++numRows;
        }
        file.close();

        m_SModel = new QStandardItemModel(numRows-1,numCols); // -1 for the header row
        m_SModel->setHorizontalHeaderLabels(headerList);
        for (int row=1; row<numRows; ++row) {
            parts = wholeFile[row].split(",");
            for (int col=0; col<numCols; ++col) {
                item = new QStandardItem(parts[col]);
                item->setTextAlignment(Qt::AlignCenter);
                m_SModel->setItem(row-1,col,item); // -1 because of the header
            }
        }
        m_ensembleTV->setModel(m_SModel);
        m_ensembleTV->resizeColumnsToContents();
    }
//    else {
//        QString msg = "\nCouldn't open Ensemble file:\n\n" + fullPath + "\n";
//        QMessageBox::critical(m_Parent, "Error", msg, QMessageBox::Ok);
//        retv = false;
//    }
    return retv;
}

QString
EnsembleDialog::getColumnData(int col)
{
    QModelIndex index = m_SModel->index(0,col);
    QString colData   = index.data().toString();

    for (int row=1; row<m_SModel->rowCount(); ++row) {
        index = m_SModel->index(row,col);
        colData += "||" + index.data().toString();
    }

    return colData;
}

void
EnsembleDialog::clear()
{
    m_SModel = new QStandardItemModel(0,0);
    m_ensembleTV->setModel(m_SModel);
    m_ensembleTV->resizeColumnsToContents();
}

void
EnsembleDialog::callback_refreshPB()
{
//    loadWidgets();
}

void
EnsembleDialog::callback_savePB()
{

}

