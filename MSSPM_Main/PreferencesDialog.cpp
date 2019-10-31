#include "PreferencesDialog.h"

#include <QStandardItemModel>
#include <QTableWidget>

PreferencesDialog::PreferencesDialog(QWidget *parent,
                                     nmfDatabase* databasePtr) :
    QDialog(parent)
{
    m_databasePtr = databasePtr;

    MainLT   = new QVBoxLayout();
    BtnLT    = new QHBoxLayout();
    StyleLT  = new QHBoxLayout();
    StyleLBL = new QLabel("Application Style:");
    StyleCMB = new QComboBox();
    CancelPB = new QPushButton("Cancel");
    OkPB     = new QPushButton("OK");

    OkPB->setToolTip("Save Preferences and Close");
    OkPB->setStatusTip("Save Preferences and Close");

    StyleLT->addWidget(StyleLBL);
    StyleLT->addWidget(StyleCMB);
    StyleLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Expanding));
    MainLT->addLayout(StyleLT);
    BtnLT->addWidget(CancelPB);
    BtnLT->addWidget(OkPB);
    BtnLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));

    // Load widgets
    StyleCMB->addItem("Light");
    StyleCMB->addItem("Dark");

    MainLT->addLayout(BtnLT);
    this->setLayout(MainLT);

    connect(StyleCMB, SIGNAL(currentIndexChanged(QString)),
            this,         SLOT(callback_StyleCMB(QString)));
    connect(CancelPB, SIGNAL(clicked()), this, SLOT(close()));
    connect(OkPB,     SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle("Preferences");
    loadWidgets();

}

void
PreferencesDialog::loadWidgets()
{
    // Set style
    // This works only because there's only 1 stylesheet.
    // Change logic if/when introduce another stylesheet.
    if (qApp->styleSheet().isEmpty())
        StyleCMB->setCurrentText("Light");
    else
        StyleCMB->setCurrentText("Dark");
}

void
PreferencesDialog::callback_StyleCMB(QString style)
{
    emit SetStyleSheet(style);
}


