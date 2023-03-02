#include "ChartSaveDialog.h"

ChartSaveDialog::ChartSaveDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vlayout       = new QVBoxLayout();
    QHBoxLayout *h1layout      = new QHBoxLayout();
    QHBoxLayout *h2layout      = new QHBoxLayout();
    QHBoxLayout *hButtonLayout = new QHBoxLayout();
    QLabel *label              = new QLabel("Chart image file name:");
    QPushButton *cancelPB      = new QPushButton("Cancel");
    QPushButton *okPB          = new QPushButton("OK");
    m_filenameLE               = new QLineEdit();
    m_fileTypeCMB              = new QComboBox();
    m_fileResCMB               = new QComboBox();
    m_filePubPB                = new QPushButton("pub");
    m_createPdfCB              = new QCheckBox("Include pdf file for publishing: ");
    m_qualitySB                = new QSpinBox();

    m_filenameLE->setToolTip(   "Enter filename for screen grab of chart.");
    m_filenameLE->setStatusTip( "Enter filename for screen grab of chart.");
    m_fileTypeCMB->setToolTip(  "Type of image file (N.B. tiff is uncompressed)");
    m_fileTypeCMB->setStatusTip("Type of image file (N.B. tiff is uncompressed)");
    m_fileTypeCMB->addItems({"jpg","png","tiff"});
    m_fileResCMB->setToolTip(  "Resolution of image file (dpi)");
    m_fileResCMB->setStatusTip("Resolution of image file (dpi)");
    m_fileResCMB->addItems({"100","300","600","1000"});
    m_filePubPB->setToolTip(  "Click to set image attributes for publication");
    m_filePubPB->setStatusTip("Click to set image attributes for publication");
    m_createPdfCB->setToolTip(  "Check this box if you want to create a pdf of the captured image");
    m_createPdfCB->setStatusTip("Check this box if you want to create a pdf of the captured image");
    m_qualitySB->setToolTip(  "Image quality: -1 (default), 0 (lowest quality/smallest file), 100 (highest quality/largest file)");
    m_qualitySB->setStatusTip("Image quality: -1 (default), 0 (lowest quality/smallest file), 100 (highest quality/largest file)");

    okPB->setDefault(true);
    m_filePubPB->setFixedWidth(30);
    m_createPdfCB->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    m_qualitySB->setMaximum(100);
    m_qualitySB->setMinimum(-1);
    m_qualitySB->setValue(-1);

    h1layout->addWidget(m_filenameLE);
    h1layout->addWidget(m_fileTypeCMB);
    h1layout->addWidget(m_fileResCMB);
    h1layout->addWidget(m_qualitySB);
    h1layout->addWidget(m_filePubPB);
    h2layout->addWidget(m_createPdfCB);
    h2layout->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hButtonLayout->addWidget(cancelPB);
    hButtonLayout->addWidget(okPB);
    vlayout->addWidget(label);
    vlayout->addLayout(h1layout);
    vlayout->addLayout(h2layout);
    vlayout->addLayout(hButtonLayout);
    this->setLayout(vlayout);

    connect(cancelPB,     SIGNAL(pressed()), this, SLOT(close()));
    connect(okPB,         SIGNAL(pressed()), this, SLOT(accept()));
    connect(m_filePubPB,  SIGNAL(clicked()), this, SLOT(callback_filePubPB()));
    connect(m_fileTypeCMB,SIGNAL(currentTextChanged(const QString&)),
            this,         SLOT(callback_fileTypeCMB(const QString&)));
}

bool
ChartSaveDialog::createPdf() {
    return m_createPdfCB->isChecked();
}

QString
ChartSaveDialog::getFilename() {
    QString retv = "";
    QFileInfo fileInfo(m_filenameLE->text());
    QString basename = fileInfo.completeBaseName();
    if (basename.isEmpty()) {
        return retv;
    }

    QString suffix = fileInfo.completeSuffix();
    if ((suffix != "jpg") && (suffix != "png") && (!suffix.isEmpty())) {
        basename += "." + suffix;
    }
    return basename + "." + m_fileTypeCMB->currentText();
}

QString
ChartSaveDialog::getFilenamePdf() {
    QFileInfo fileInfo(m_filenameLE->text());
    QString basename = fileInfo.completeBaseName();

    return (basename.isEmpty()) ? "" : basename+".pdf";
}

int
ChartSaveDialog::getQuality() {
    return m_qualitySB->value();
}

int
ChartSaveDialog::getResolution() {
    return m_fileResCMB->currentText().toInt();
}


void
ChartSaveDialog::callback_filePubPB()
{
    m_fileTypeCMB->setCurrentText("tiff");
    m_fileResCMB->setCurrentText("600");
    m_createPdfCB->setChecked(true);
}

void
ChartSaveDialog::callback_fileTypeCMB(const QString& fileType)
{
    m_qualitySB->setEnabled(((fileType == "jpg") || (fileType == "png")));
}


