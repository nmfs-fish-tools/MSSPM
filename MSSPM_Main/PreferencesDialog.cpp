#include "PreferencesDialog.h"

#include <QStandardItemModel>
#include <QTableWidget>

PreferencesDialog::PreferencesDialog(QWidget *parent,
                                     nmfDatabase* databasePtr) :
    QDialog(parent)
{
    m_databasePtr = databasePtr;

    MainLT    = new QVBoxLayout();
    BtnLT     = new QHBoxLayout();
    SigDigLT  = new QHBoxLayout();
    StyleLT   = new QHBoxLayout();
    MShot1LT  = new QHBoxLayout();
    MShot2LT  = new QHBoxLayout();
    MShot3LT  = new QHBoxLayout();
    StyleLBL  = new QLabel("Application Style:");
    StyleCMB  = new QComboBox();
    MShot1LBL = new QLabel("Layout");
    MShot2LBL = new QLabel(" # rows:");
    MShot3LBL = new QLabel(" # columns:");
    MShot1CMB = new QComboBox();
    MShot2SB  = new QSpinBox();
    MShot3SB  = new QSpinBox();
    CancelPB  = new QPushButton("Cancel");
    OkPB      = new QPushButton("OK");
    SigDigCB  = new QCheckBox("Significant digits:");
    SigDigSB  = new QSpinBox();


    OkPB->setToolTip("Save Preferences and Close");
    OkPB->setStatusTip("Save Preferences and Close");

    SigDigLT->addWidget(SigDigCB);
    SigDigLT->addWidget(SigDigSB);
    StyleLT->addWidget(StyleLBL);
    StyleLT->addWidget(StyleCMB);
    StyleLT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Expanding));
    MShot1LT->addWidget(MShot1LBL);
    MShot1LT->addWidget(MShot1CMB);
    MShot2LT->addWidget(MShot2LBL);
    MShot2LT->addWidget(MShot2SB);
    MShot3LT->addWidget(MShot3LBL);
    MShot3LT->addWidget(MShot3SB);
    MainLT->addLayout(SigDigLT);
    MainLT->addLayout(StyleLT);
    MainLT->addLayout(MShot1LT);
    MainLT->addLayout(MShot2LT);
    MainLT->addLayout(MShot3LT);
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


