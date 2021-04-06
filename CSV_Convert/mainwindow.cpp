#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_filenames.clear();
    // Load July data
    for (int i=1; i<=31; ++i) {
        m_filenames << "/home/rklasky/workspaceQtCreator/NetLogo/data/nesdisVHNchlaWeekly_2020-07-" + QString("%1").arg(i,2,10,QChar('0')) + ".csv";
    }
    // Load August data
    for (int i=1; i<=31; ++i) {
        m_filenames << "/home/rklasky/workspaceQtCreator/NetLogo/data/nesdisVHNchlaWeekly_2020-08-" + QString("%1").arg(i,2,10,QChar('0')) + ".csv";
    }

    QPushButton* convertPB = ui->ConvertPB;

    connect(convertPB, SIGNAL(clicked()),
            this,      SLOT(callback_ConvertPB()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::callback_ConvertPB()
{
    QString outputFilename;

    // Calculate NetLogo formatted csv file from the ERDDAP NESDIS csv file
    for (QString inputFilename : m_filenames) {
        outputFilename = inputFilename;
        outputFilename = outputFilename.replace("nesdisVHNchlaWeekly","NetLogo");
        processFile(inputFilename,outputFilename);
    }
}

void
MainWindow::processFile(const QString& inputFilename,
                        const QString& outputFilename)
{
    int numLonCells = 80; // Number of cells in NetLogo gaming area
    int numLatCells = 70;
    double lat;
    double lon;
    // Hardcoded for corners of the rotated George's Bank map I'm using in NetLogo
    double latN =  44.79375; // 44.134913;
    double latS =  39.01875; // 40.010787;
    double lonE = -65.64375000000001; // -64.759660;
    double lonW = -70.96875000000001; // -71.235352;
    double latInc = std::abs((latN-latS)/numLatCells);
    double lonInc = std::abs((lonW-lonE)/numLonCells);
    double value;
    QString valueStr;
    QString allLines;
    QStringList lineList;
    QStringList parts;
    double newValue = 0;
    int i=0;
    int xindex,yindex;
    // RSK - make this an array of vectors and then make a dup array to hold
    // the averaged values since there can be more than one value in a NetLogo cell
    int data[numLonCells][numLatCells];
    double transparencyScaleValue = 90.0;

    for (int i=0; i<numLonCells; ++i) {
        for (int j=0; j<numLatCells; ++j) {
            data[i][j] = 0;
        }
    }

    QFile fileIn(inputFilename);
    if (fileIn.open(QIODevice::ReadOnly)) {
        allLines = fileIn.readAll().trimmed();
        lineList = allLines.split('\n');
        for (QString line : lineList) {
            parts = line.split(','); // lat = 2, lon = 3, value = 4
            lat = parts[2].toDouble();
            lon = parts[3].toDouble();
            valueStr = parts[4];
            value = (valueStr == "NaN") ? 0 : valueStr.toDouble();
            if ((lat <= latN) && (lat >= latS) && (lon >= lonW) && (lon <= lonE)) {
                // Found data point inside map
                xindex =  int((latN-lat)/latInc);
                yindex =  int((std::abs(lonW)-std::abs(lon))/lonInc);
                xindex = (xindex >= numLonCells) ? numLonCells-1 : xindex;
                yindex = (yindex >= numLatCells) ? numLatCells-1 : yindex;
                newValue = int(transparencyScaleValue*value);
                data[xindex][yindex] = (newValue > 255) ? 255 : newValue;
//std::cout << xindex << "," << yindex << ": " << value << std::endl;
            }
        }
    }

    // Write out data to file
    QFile fileOut(outputFilename);
    if (fileOut.open(QIODevice::WriteOnly)) {
        QTextStream stream(&fileOut);
        for (int i=0; i<numLatCells; ++i) {
            stream << data[i][0];
            for (int j=1; j<numLonCells; ++j) {
                stream << "," << data[i][j];
            }
            stream << "\n";
        }
        fileOut.close();
    }

}
