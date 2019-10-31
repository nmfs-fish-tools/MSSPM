#pragma once

#include "nmfDatabase.h"

#include <QTableWidget>
#include <QComboBox>

/**
 * @brief Clear Dialog
 *
 * This dialog allows the user to clear output data from specific
 * combinations of Algorithm, Minimizer, Objective Criterion, and
 * Scaling Algorithm settings.
 *
 */
class ClearOutputDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;

    QVBoxLayout* MainLAYT;
    QHBoxLayout* BtnLAYT;
    QLabel*      MainLBL;
    QLabel*      AlgorithmLBL;
    QLabel*      MinimizerLBL;
    QLabel*      ObjectiveCriterionLBL;
    QLabel*      ScalingLBL;
    QComboBox*   AlgorithmCMB;
    QComboBox*   MinimizerCMB;
    QComboBox*   ObjectiveCriterionCMB;
    QComboBox*   ScalingCMB;
    QPushButton* CancelPB;
    QPushButton* OkPB;


public:
    /**
     * @brief ClearOutputDialog : class constructor
     * @param parent : the parent widget (e.g., the main window)
     * @param databasePtr : the pointer to the application database
     */
    ClearOutputDialog(QWidget*     parent,
                      nmfDatabase* databasePtr);
    virtual ~ClearOutputDialog() {}

    std::string getAlgorithm();
    std::string getMinimizer();
    std::string getObjectiveCriterion();
    std::string getScaling();
    void        loadWidgets();

private Q_SLOTS:
    void callback_AlgorithmCMB(QString algorithm);
    void callback_MinimizerCMB(QString minimizer);
    void callback_ObjectiveCriterionCMB(QString objectiveCriterion);
};

