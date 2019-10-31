
#include "nmfEstimationTab01.h"
#include "nmfConstantsMSSPM.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"


nmfEstimation_Tab1::nmfEstimation_Tab1(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_smodel      = nullptr;
    m_ProjectDir  = projectDir;

    m_logger->logMsg(nmfConstants::Normal,"nmfEstimation_Tab1::nmfEstimation_Tab1");

    Estimation_Tabs = tabs;

    // Load ui as a widget from disk
    QFile file(":/forms/Estimation/Estimation_Tab01.ui");
    file.open(QFile::ReadOnly);
    Estimation_Tab1_Widget = loader.load(&file,Estimation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Estimation_Tabs->addTab(Estimation_Tab1_Widget, tr("1. Population Parameters"));

    Estimation_Tab1_PopulationTV = Estimation_Tabs->findChild<QTableView *>("Estimation_Tab1_PopulationTV");
    Estimation_Tab1_NextPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_NextPB");
    Estimation_Tab1_PopulationGB = Estimation_Tabs->findChild<QGroupBox   *>("Estimation_Tab1_PopulationGB");
    Estimation_Tab1_LoadPB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_ReloadPB");
    Estimation_Tab1_SavePB       = Estimation_Tabs->findChild<QPushButton *>("Estimation_Tab1_SavePB");

    //nmfTableView* Estimation_Tab1_PopulationTV = new nmfTableView();
    QFont noBoldFont;
    noBoldFont.setBold(false);
    Estimation_Tab1_PopulationTV->setFont(noBoldFont);

    connect(Estimation_Tab1_NextPB, SIGNAL(clicked()),
            this,                   SLOT(callback_NextPB()));
    connect(Estimation_Tab1_LoadPB, SIGNAL(clicked()),
            this,                   SLOT(callback_LoadPB()));
    connect(Estimation_Tab1_SavePB, SIGNAL(clicked()),
            this,                   SLOT(callback_SavePB()));

    Estimation_Tab1_NextPB->setText("--\u25B7");

} // end constructor


nmfEstimation_Tab1::~nmfEstimation_Tab1()
{

}

void
nmfEstimation_Tab1::clearWidgets()
{
    Estimation_Tab1_PopulationTV->reset();
}

void
nmfEstimation_Tab1::callback_NextPB()
{
    int nextPage = Estimation_Tabs->currentIndex()+1;
    Estimation_Tabs->setCurrentIndex(nextPage);
}


void
nmfEstimation_Tab1::callback_LoadPB()
{
    loadWidgets();
}

void
nmfEstimation_Tab1::callback_SavePB()
{
    std::string cmd;
    std::string errorMsg;
    std::string SpeName;
    std::string InitBiomassMin,InitBiomassMax,SurveyQMin,SurveyQMax;
    std::string GrowthRateMin,GrowthRateMax,SpeciesKMin,SpeciesKMax;
    QModelIndex index;

    for (int i=0; i<m_smodel->rowCount(); ++i) {
        index = m_smodel->index(i,0);
        SpeName = index.data().toString().toStdString();
        index = m_smodel->index(i,1);
        InitBiomassMin = index.data().toString().toStdString();
        index = m_smodel->index(i,2);
        InitBiomassMax = index.data().toString().toStdString();
        index = m_smodel->index(i,3);
        GrowthRateMin = index.data().toString().toStdString();
        index = m_smodel->index(i,4);
        GrowthRateMax = index.data().toString().toStdString();
        index = m_smodel->index(i,5);
        SpeciesKMin = index.data().toString().toStdString();
        index = m_smodel->index(i,6);
        SpeciesKMax = index.data().toString().toStdString();
        index = m_smodel->index(i,7);
        SurveyQMin = index.data().toString().toStdString();
        index = m_smodel->index(i,8);
        SurveyQMax = index.data().toString().toStdString();

        cmd = "UPDATE Species SET InitBiomassMin=" + InitBiomassMin + ", InitBiomassMax=" + InitBiomassMax +
              ", GrowthRateMin=" + GrowthRateMin + ", GrowthRateMax=" + GrowthRateMax +
              ", SpeciesKMin=" + SpeciesKMin + ", SpeciesKMax=" + SpeciesKMax +
              ", SurveyQMin=" + SurveyQMin + ", SurveyQMax=" + SurveyQMax +
              " WHERE SpeName = '" + SpeName + "'";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            m_logger->logMsg(nmfConstants::Error,"nmfEstimation_Tab1 callback_SavePB: Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
            QMessageBox::warning(Estimation_Tabs, "Error",
                                     "\nError in Save command.  Check that all cells are populated.\n",
                                     QMessageBox::Ok);
            return;
        }
    }

    emit ReloadSpecies();

    QMessageBox::information(Estimation_Tabs, "Species Updated",
                             "\nSpecies table has been successfully updated.\n",
                             QMessageBox::Ok);
}


bool
nmfEstimation_Tab1::loadWidgets()
{
std::cout << "nmfEstimation_Tab1::loadWidgets()" << std::endl;
    int j;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItem *item;

    // Load Population tableview
    fields = {"SpeName","GuildName","InitBiomass","InitBiomassMin","InitBiomassMax",
              "GrowthRate","GrowthRateMin","GrowthRateMax","GrowthRateCovarCoeff",
              "SpeciesK","SpeciesKMin","SpeciesKMax","SpeciesKCovarCoeff",
              "SurveyQ","SurveyQMin","SurveyQMax"};
    queryStr   = "SELECT SpeName,GuildName,InitBiomass,InitBiomassMin,InitBiomassMax,";
    queryStr  += "GrowthRate,GrowthRateMin,GrowthRateMax,GrowthRateCovarCoeff,";
    queryStr  += "SpeciesK,SpeciesKMin,SpeciesKMax,SpeciesKCovarCoeff,";
    queryStr  += "SurveyQ,SurveyQMin,SurveyQMax FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    QStringList PopulationFieldList = {"SpeName","InitBiomassMin","InitBiomassMax",
                                       "GrowthRateMin","GrowthRateMax",
                                       "SpeciesKMin","SpeciesKMax",
                                       "SurveyQMin","SurveyQMax"};

    m_smodel = new QStandardItemModel( NumSpecies, PopulationFieldList.size() );
    for (int i=0; i<NumSpecies; ++i) {
        j = 0;
        for (QString field : PopulationFieldList)
        {
            item = new QStandardItem(QString::fromStdString(dataMap[field.toStdString()][i]));
            item->setTextAlignment(Qt::AlignCenter);
            m_smodel->setItem(i, j++, item);
        }
    }
    m_smodel->setHorizontalHeaderLabels(PopulationFieldList);
    Estimation_Tab1_PopulationTV->setModel(m_smodel);
    Estimation_Tab1_PopulationTV->resizeColumnsToContents();

    return true;
}
