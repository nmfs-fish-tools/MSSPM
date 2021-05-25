/**
 * @file nmfEstimationTab05.h
 * @brief GUI definition for the nmfEstimation_Tab5 Observation Data class
 *
 * This file contains the GUI definition for the Observation Data page. This
 * page allows the user to enter/modify a table of data that correspond to
 * Biomass of each Species for each year of data.
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 */


#ifndef NMFESTIMATIONTAB5_H
#define NMFESTIMATIONTAB5_H

/**
 * @brief Observation Data
 *
 * This table allows the user to enter and modify Observed Biomass
 * data for each Species for every year in the year range.
 */
class nmfEstimation_Tab5: public QObject
{

    Q_OBJECT

    nmfDatabase*        m_DatabasePtr;
    nmfLogger*          m_Logger;
    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    QStandardItemModel* m_SModelAbsoluteBiomass;
    QStandardItemModel* m_SModelRelativeBiomass;
    QStandardItemModel* m_SModelCovariates;
    QStandardItemModel* m_SModelScalars;

    QTabWidget*   Estimation_Tabs;
    QWidget*      Estimation_Tab5_Widget;
    QTableView*   Estimation_Tab5_AbsoluteBiomassTV;
    QTableView*   Estimation_Tab5_RelativeBiomassTV;
    QTableView*   Estimation_Tab5_Rel2AbsScalarTV;
    QTableView*   Estimation_Tab5_CovariatesTV;
    QPushButton*  Estimation_Tab5_PrevPB;
    QPushButton*  Estimation_Tab5_NextPB;
    QPushButton*  Estimation_Tab5_LoadPB;
    QPushButton*  Estimation_Tab5_SavePB;
    QPushButton*  Estimation_Tab5_ImportPB;
    QPushButton*  Estimation_Tab5_ExportPB;
    QPushButton*  Estimation_Tab5_CalcBiomassPB;
    QRadioButton* Estimation_Tab5_AbsoluteBiomassRB;
    QRadioButton* Estimation_Tab5_RelativeBiomassRB;
    QGroupBox*    Estimation_Tab5_CovariatesGB;
    QCheckBox*    Estimation_Tab5_EstimateSurveyQCB;

    void importAbsoluteBiomass();
    void importRelativeBiomass();
    void importScalarValues();
    void importTableData(const bool& firstLineReadOnly,
                         const QString& type,
                         const std::string& tableName,
                         QTableView* tableView);
    bool isAbsoluteBiomassChecked();
    void loadAbsoluteBiomass(const int& RunLength,
                             const int& StartYear,
                             const int& NumSpecies,
                             const QString& SystemName,
                             const QString& MohnsRhoLabel,
                             const std::vector<std::string>& SpeciesNames,
                             const QStringList& SpeciesList,
                             QStringList& VerticalList);
    void loadCovariates(const int& RunLength,
                        const QStringList& VerticalList);
    void loadRelativeBiomass(const int& RunLength,
                             const int& StartYear,
                             const int& NumSpecies,
                             const QString& SystemName,
                             const QString& MohnsRhoLabel,
                             const std::vector<std::string>& SpeciesNames,
                             const QStringList& SpeciesList,
                             QStringList& VerticalList);
    void loadScalars(const int& RunLength,
                     const int& StartYear,
                     const int& NumSpecies,
                     const QString& SystemName,
                     const QString& MohnsRhoLabel,
                     const std::vector<std::string>& SpeciesNames,
                     const QStringList& SpeciesList,
                     QStringList& VerticalList);
    void loadTableValuesFromDatabase(const int& RunLength,
                                     const int& StartYear,
                                     const int& NumSpecies,
                                     const QString& SystemName,
                                     const QString& MohnsRhoLabel,
                                     const std::vector<std::string>& SpeciesNames,
                                     const QStringList& SpeciesList,
                                     QStringList& VerticalList,
                                     const std::string& tableName,
                                     QStandardItemModel* smodel,
                                     QTableView* tableView);
    void loadCSVFile(const bool& firstLineReadOnly,
                     const std::string& tableName,
                     QTableView* tableView);
    void matchTableColumnWidths();
    void readSettings();
    void saveCSVFile(const QString& type,
                     QStandardItemModel* smodel,
                     const std::string& tableName);
    void saveAbsoluteBiomass();
    void saveRelativeBiomass();
    void saveScalarValues();
    void saveTableValuesToCSVFile(const QString& type,
                                  const std::string& tableName,
                                  QStandardItemModel* smodel);
    void saveTableValuesToDatabase(const std::string& tableName,
                                   QStandardItemModel* smodel);

public:
    /**
     * @brief nmfEstimation_Tab5 : class constructor for the Observation Data GUI page
     * @param tabs : the tab widget into which this Estimation tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     * @param projectDir : the project directory
     */
    nmfEstimation_Tab5(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfEstimation_Tab5();

    /**
     * @brief Checks that all tables in this GUI are properly populated
     * @return Boolean value signifying if tables are properly loaded
     */
    bool areTablesOK();
    /**
     * @brief Clears the GUI's widgets
     */
    void clearWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @return Returns true if all data were loaded successfully
     */
    bool loadWidgets();
    /**
     * @brief Loads all widgets for this GUI from database tables
     * @param firstRowOnly : boolean to specify if the user just wants to reload only the first row (since the first row is not modifiable by the user in this tab)
     * @param MohnsRhoLabel : the Mohns Rho name used to identify the data if the run is part of a Retrospective Analysis
     * @return
     */
    bool loadWidgets(QString MohnsRhoLabel);

    bool loadWidgetsFirstRow();
    void enableEstimateSurveyQCB(bool enable);
    bool isTableValueOK(QString value);
    bool useRelativeBiomass();

signals:
    /**
     * @brief Signal notifies any other GUI showing similar data to refresh itself
     * @param showPopup : boolean signifying whether the application should pop up a successful reload acknowledgement
     */
    void ReloadSpecies(bool showPopup);
    void EnableSurveyQ(const QString biomassType,
                       const bool enable,
                       const bool checked);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user clicks the Absolute Biomass radio button
     */
    void callback_AbsoluteBiomassRB();
    /**
     * @brief Callback invoked when the user clicks the (calc)ulate Relative Biomass to Absolute Biomass button
     */
    void callback_CalcBiomassPB();
    /**
     * @brief Callback invoked when the user clicks the Export button to save a .csv file
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user clicks the Import button to load a .csv file
     */
    void callback_ImportPB();
    /**
     * @brief Callback invoked when the user clicks the Load button to load the corresponding
     * database table
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user wants to reload widgets from the main routine without a popup acknowledgement
     */
    void callback_LoadWidgets();
    /**
     * @brief Callback invoked when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user clicks the Relative Biomass radio button
     */
    void callback_RelativeBiomassRB();
    /**
     * @brief Callback invoked when user scrolls the Relative Biomass table to sync it with the Scalar table
     * @param value : value to scroll the table
     */
    void callback_RelativeBiomassTVScrolled(int value);
    /**
     * @brief Callback invoked when user scrolls the Scalar table to sync it with the Relative Biomass table
     * @param value : value to scroll the table
     */
    void callback_Rel2AbsScalarTVScrolled(int value);
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user saves a new model in Setup -> Model Setup
     */
    void callback_UpdateInitialObservedBiomass();
    void callback_DimScalarBiomassControls(bool dim);
    void callback_EstimateSurveyQCB(int state);

};

#endif // NMFESTIMATIONTAB5_H
