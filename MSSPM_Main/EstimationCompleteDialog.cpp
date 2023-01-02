#include "EstimationCompleteDialog.h"


EstimationCompleteDialog::EstimationCompleteDialog(QWidget *parent,
                                                   const QString& title) :
    QDialog(parent)
{
    // the constructor will initialize all of the widgets and layout
    QUiLoader m_loader;
    QFile file(":/forms/Main/EstimationCompleteDlg.ui");
    file.open(QFile::ReadOnly);
    m_Widget = m_loader.load(&file,parent);;
    file.close();
    m_OkPB   = m_Widget->findChild<QPushButton  *>("OkPB");
    m_MsgLBL = m_Widget->findChild<QLabel       *>("MessageLBL");

    m_Layout = new QVBoxLayout();
    m_Layout->addWidget(m_Widget);
    this->adjustSize();
    //    this->setMinimumWidth(400);
    //    this->setMinimumHeight(380);
    this->setLayout(m_Layout);
    this->setWindowTitle(title);

    connect(m_OkPB, SIGNAL(released()),
            this,   SLOT(close()));

    this->hide();
}

void
EstimationCompleteDialog::loadPosition(
        const int& x,
        const int& y)
{
    this->setGeometry(x,y,this->width(),this->height());
}

void
EstimationCompleteDialog::loadText(const QString& text)
{
    m_MsgLBL->setText(text);
}






