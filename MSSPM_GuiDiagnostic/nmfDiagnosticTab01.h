#ifndef NMFDIAGNOSTICTAB1_H
#define NMFDIAGNOSTICTAB1_H

#include <tuple>
#include <BeesAlgorithm.h>
#include "NLopt_Estimator.h"

/**
 * @brief Diagnostic Tuple for Percent Variations
 *
 * A tuple that maps Species to growth rate variation, carrying capacity variation,
 * and fitness value.
 */
typedef std::tuple<QString, double, double, double> DiagnosticTuple;

/**
 * @brief Diagnostic Parameters
 *
 * This allows the user to enter Diagnostic parameters that describe the
 * percent variation from the Estimated Value and the number of Diagnostic Points
 * on either side of the Estimated Value.
 */
class nmfDiagnostic_Tab1: public QObject
{
    Q_OBJECT

private:
    int          m_NumPoints;
    int          m_PctVariation;
    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    std::string  m_projectDir;
    QTabWidget*  m_Diagnostic_Tabs;
    QWidget*     m_Diagnostic_Tab1_Widget;
    QComboBox*   m_Diagnostic_Tab1_ParameterCMB;
    QLabel*      m_Diagnostic_Tab1_ParameterLBL;
    QSpinBox*    m_Diagnostic_Tab1_PctVarSB;
    QSpinBox*    m_Diagnostic_Tab1_NumPtsSB;
    QPushButton* m_Diagnostic_Tab1_RunPB;
    Data_Struct  m_theDataStruct;

    void loadEstimatedParameter(const std::string& Algorithm,
                                const std::string& Minimizer,
                                const std::string& ObjectiveCriterion,
                                const std::string& Scaling,
                                const QString& parameterName,
                                std::vector<double>& EstParameter);
    void updateParameterTable(const int&         NumSpeciesOrGuilds,
                              const int&         NumPoints,
                              const std::string& Algorithm,
                              const std::string& Minimizer,
                              const std::string& ObjectiveCriterion,
                              const std::string& Scaling,
                              const std::string& isAggProd,
                              const QString&     ParameterName,
                              std::vector<DiagnosticTuple>& DiagnosticTupleVector);
    void updateParameterTable(const std::string& Algorithm,
                              const std::string& Minimizer,
                              const std::string& ObjectiveCriterion,
                              const std::string& Scaling,
                              const std::string& isAggProd,
                              std::vector<DiagnosticTuple>& DiagnosticTupleVector);
    bool algorithmIdentifiers(std::string &Algorithm,
                              std::string &Minimizer,
                              std::string &ObjectiveCriterion,
                              std::string &Scaling,
                              std::string &CompetitionForm);
    void parameterToTableName(const std::string whichTable,
                              const QString& parameter,
                                    QString& tableName);
    bool isAggProd(std::string Algorithm,
                   std::string Minimizer,
                   std::string ObjectiveCriterion,
                   std::string Scaling);
    double calculateFitness(int     SpeciesOrGuildNum,
                            QString ParameterName,
                            double  ParameterValue);
    double calculateFitness(int     SpeciesOrGuildNum,
                            double  rParameter,
                            double  KParameter);
    void loadGrowthParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            const std::string&   isAggProd,
            std::vector<double>& Parameters);
    void loadHarvestParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            std::vector<double>& Parameters);
    void loadCompetitionParameters(
            const bool&          isAggProd,
            const int&           NumSpecies,
            const int&           theNumGuilds,
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            std::vector<double>& Parameters);
    void loadPredationParameters(
            const int&           NumSpeciesOrGuilds,
            const std::string&   Algorithm,
            const std::string&   Minimizer,
            const std::string&   ObjectCriterion,
            const std::string&   Scaling,
            std::vector<double>& Parameters);
    void readSettings();

public:
    /**
     * @brief nmfDiagnostic_Tab1 : class constructor
     * @param tabs : the tab widget into which this Diagnostic tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfDiagnostic_Tab1(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfDiagnostic_Tab1();

    void clearWidgets();
    void loadWidgets();
    int  getFontSize();
    std::string getTableName(QString paramName);
    int  getLastRunsPctVariation();
    int  getLastRunsNumPoints();
    void getGuildInfo(int& NumGuilds,QStringList& GuildNames);
    void getSpeciesInfo(int& NumSpecies,QStringList& SpeciesNames);
    void saveSettings();
    void setFontSize(int fontSize);
    void setVariation(int fontSize);
    void setNumPoints(int fontSize);
    void setDataStruct(Data_Struct& theBeeStruct);

signals:
    void LoadDataStruct();
    void QueryAlgorithmIdentifiers();
    void ResetOutputWidgetsForAggProd();
    void SetChartType(std::string type, std::string method);

public slots:
    void callback_Diagnostic_Tab1_RunPB();
};

#endif

