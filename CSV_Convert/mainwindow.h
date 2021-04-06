#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QPushButton>
#include <QTextStream>

#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QPushButton* convertPB;
    QStringList m_filenames;
    void processFile(const QString& inputFilename,
                     const QString& outputFilename);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void callback_ConvertPB();

};
#endif // MAINWINDOW_H
