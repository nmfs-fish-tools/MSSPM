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

    nmfLogger*          m_Logger;
    nmfDatabase*        m_DatabasePtr;
    QStandardItemModel* m_SModel;
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
    /**
     * @brief Signal notifying other widgets to reload species data
     */
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

    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();

    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();


public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user clicks the Next Page button
     */
    void callback_NextPB();
};

#endif // NMFESTIMATIONTAB1_H
