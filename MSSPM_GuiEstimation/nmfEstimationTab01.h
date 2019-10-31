#ifndef NMFESTIMATIONTAB1_H
#define NMFESTIMATIONTAB1_H

#include "nmfUtilsQt.h"

/**
 * @brief Estimated Parameters
 *
 * This table is reproduced here from the Species table in Setup Tab 3. This is a
 * more succinct way of looking at the per Species estimated parameters and their
 * min/max values.
 */
class nmfEstimation_Tab1: public QObject
{
    Q_OBJECT

    nmfLogger*          m_logger;
    nmfDatabase*        m_databasePtr;
    QStandardItemModel* m_smodel;
    std::string         m_ProjectDir;

    QTabWidget*   Estimation_Tabs;
    QWidget*      Estimation_Tab1_Widget;
//  nmfTableView* Estimation_Tab1_PopulationTV;
    QTableView*   Estimation_Tab1_PopulationTV;
    QPushButton*  Estimation_Tab1_NextPB;
    QGroupBox*    Estimation_Tab1_PopulationGB;
    QPushButton*  Estimation_Tab1_LoadPB;
    QPushButton*  Estimation_Tab1_SavePB;

signals:
    void ReloadSpecies();

public:
    /**
     * @brief nmfEstimation_Tab1 : class constructor
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab1(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab1();

    bool loadWidgets();
    void clearWidgets();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_NextPB();
};

#endif // NMFESTIMATIONTAB1_H
