#include "ScrollingTextDialog.h"

#include <QPushButton>
#include <QTextBrowser>
#include <QUiLoader>
#include <QVBoxLayout>

ScrollingTextDialog::ScrollingTextDialog(QWidget *parent,
                                         const QString& title) :
    QDialog(parent)
{
    // the constructor will initialize all of the widgets and layout
    QUiLoader m_loader;
    QFile file(":/forms/Main/ScrollingTextDlg.ui");
    file.open(QFile::ReadOnly);
    m_Widget = m_loader.load(&file,parent);;
    file.close();
    m_OkPB  = m_Widget->findChild<QPushButton  *>("OkPB");
    m_MsgTB = m_Widget->findChild<QTextBrowser *>("TextBrowser");

    m_Layout = new QVBoxLayout();
    m_Layout->addWidget(m_Widget);
    this->adjustSize();
    this->setMinimumWidth(400);
    this->setMinimumHeight(350);
    this->setLayout(m_Layout);
    this->setWindowTitle(title);

    connect(m_OkPB, SIGNAL(released()),
            this,   SLOT(close()));
}

void
ScrollingTextDialog::loadText(const QString& text)
{
    m_MsgTB->setText(text);
}






