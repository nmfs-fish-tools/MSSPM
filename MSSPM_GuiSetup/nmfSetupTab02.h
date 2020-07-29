/**
 * @file nmfSetupTab02.h
 * @brief GUI definition for the Setup Project page class nmfSetup_Tab2
 *
 * This file contains the GUI definitions for the Setup Project page. This
 * page contains the GUI widgets that allow the user to enter and modify
 * meta data regarding the current Model run.
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
 *
 */

#ifndef NMFSETUPTAB2_H
#define NMFSETUPTAB2_H

#include <QGroupBox>
/**
 * @brief The Setup Tab 2 allows the user to enter and modify Project meta data
 *
 * This tab allows the user to defined Project meta data such as name,
 * location, author, and a brief description.
 */
class nmfSetup_Tab2: public QObject
{
    Q_OBJECT

    nmfDatabase*          m_DatabasePtr;
    QString               m_LastProjectDatabase;
    nmfLogger*            m_Logger;
    bool                  m_NewProject;
    bool                  m_OkToCreateTables;
    QProgressDialog*      m_ProgressDlg;
    QString               m_ProjectAuthor;
    QString               m_ProjectDatabase;
    QString               m_ProjectDescription;
    QString               m_ProjectDir;
    QString               m_ProjectName;
    std::set<std::string> m_ValidDatabases;

    QTabWidget*  Setup_Tabs;
    QWidget*     Setup_Tab2_Widget;
    QGroupBox*   Setup_Tab2_ProjectDataGB;
    QComboBox*   Setup_Tab2_ProjectDatabaseCMB;
    QPushButton* Setup_Tab2_ProjectDirBrowsePB;
    QPushButton* Setup_Tab2_DelDatabasePB;
    QPushButton* Setup_Tab2_AddDatabasePB;
    QPushButton* Setup_Tab2_SaveProjectPB;
    QPushButton* Setup_Tab2_NewProjectPB;
    QPushButton* Setup_Tab2_DelProjectPB;
    QPushButton* Setup_Tab2_RefreshDatabasePB;
    QPushButton* Setup_Tab2_BrowseProjectPB;
    QPushButton* Setup_Tab2_SetProjectPB;
    QPushButton* Setup_Tab2_ReloadProjectPB;
    QLineEdit*   Setup_Tab2_ProjectNameLE;
    QLineEdit*   Setup_Tab2_ProjectAuthorLE;
    QLineEdit*   Setup_Tab2_ProjectDescLE;
    QLineEdit*   Setup_Tab2_ProjectDirLE;
    QTextEdit*   SetupOutputTE;

    void    readSettings();


signals:
    void AddedNewDatabase();
    /**
     * @brief Signal emitted to clear the Estimation data tables
     * after user has deleted the current database
     */
    void ClearEstimationTables();
    /**
     * @brief Signal emitted to notify the application that a new Project has been loaded
     */
    void LoadProject();
    /**
     * @brief Signal emitted to notify the application that all the Guilds and Species are to be deleted
     */
    void RemoveGuildsAndSpecies();
    /**
     * @brief Signal emitted to notify the application to save its settings to the Qt Settings file
     */
    void SaveMainSettings();
    /**
     * @brief Signal emitted to notify the application that a Project was just saved and
     * to update its window title (the window title
     * is composed of the Project name from this Setup page and the System name from Setup
     * page 4) and Navigator Tree settings.
     */
    void ProjectSaved();

public:
    /**
     * @brief nmfSetup_Tab2 : class constructor for the Project Setup Page
     * @param tabWidget : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     */
    nmfSetup_Tab2(QTabWidget*  tabWidget,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr);
    virtual ~nmfSetup_Tab2();

    /**
     * @brief Clears the Project meta data text boxes
     */
    void clearProject();
    /**
     * @brief Creates the database tables necessary for MSSPM to function. Each table
     * is created without any data.  If this is called and the tables already exist,
     * no change will take place (i.e., no current database tables will be affected).
     * @param databaseName : Name of database to populate with tables
     */
    void createTables(QString databaseName);
    /**
     * @brief Enables the Project Data group of widgets so user can enter new Project meta data
     */
    void enableProjectData();
    /**
     * @brief Enables any Setup tabs to the right of the Project setup tab
     * @param enable : boolean which determins if the Setup tabs are to be enabled or disabled
     */
    void enableSetupTabs(bool enable);
    /**
     * @brief Reads the Project Author name from the appropriate GUI widget
     * @return The current Project Author name
     */
    QString getProjectAuthor();
    /**
     * @brief Reads the Project Database name from the appropriate GUI widget
     * @return The current Project Database name
     */
    QString getProjectDatabase();
    /**
     * @brief Reads the Project description from the appropriate GUI widget
     * @return The current Project description
     */
    QString getProjectDescription();
    /**
     * @brief Reads the Project Directory name from the appropriate GUI widget
     * @return The current Project Directory name
     */
    QString getProjectDir();
    /**
     * @brief Reads the Project name from the appropriate GUI widget
     * @return The current Project name
     */
    QString getProjectName();
    /**
     * @brief Initializes the current database
     * @param database : database to initialize
     */
    void    initDatabase(QString database);
    /**
     * @brief Checks the project data to see if they're valid
     * @return Boolean describing whether or not the project data are valid
     */
    bool    isProjectDataValid();
    /**
     * @brief Checks the project name to see if it's valid
     * @param projectName : the project name to check for validity
     * @return Boolean describing whether or not the project name is valid
     */
    bool    isProjectNameValid(QString projectName);
    /**
     * @brief Loads all available database names into the appropriate widget for user selection
     * @param databaseNameToSelect : name of database to select as current
     */
    void    loadDatabaseNames(QString databaseNameToSelect);
    /**
     * @brief Loads the currently selected Project
     * @param logger : pointer to the application logger
     * @param projectFilename : name of the Project file to load
     * @return Boolean describing whether or not the project was loaded
     */
    bool    loadProject(nmfLogger *logger, QString projectFilename);
    /**
     * @brief Loads the widgets for this GUI from the appropriate database table(s)
     */
    void    loadWidgets();
    /**
     * @brief Saves this GUI's project data to the Qt Settings file
     */
    void    saveSettings();
    /**
     * @brief Writes the author of the project into the appropriate widget
     * @param author : the author's name
     */
    void setProjectAuthor(QString author);
    /**
     * @brief Sets the current item in the pulldown to the passed database name
     * @param dbName : name of database to set the pulldown to
     */
    void setProjectDatabase(QString dbName);
    /**
     * @brief Writes the description of the project into the appropriate widget
     * @param desc : the description of the project
     */
    void setProjectDescription(QString desc);
    /**
     * @brief Writes the directory for the project into the appropriate widget
     * @param dir : the directory of the project
     */
    void setProjectDirectory(QString dir);
    /**
     * @brief Writes the name of the current project into the appropriate widget
     * @param name : the name of the project
     */
    void setProjectName(QString name);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user adds a new database
     */
    void callback_Setup_Tab2_AddDatabase();
    /**
     * @brief Callback invoked when the user wants to browse for a new Project
     */
    void callback_Setup_Tab2_BrowseProject();
    /**
     * @brief Callback invoked when the user changes the current database
     * @param newDatabaseName : name of new current database
     */
    void callback_Setup_Tab2_DatabaseChanged(QString newDatabaseName);
    /**
     * @brief Callback invoked when the user deletes a Database
     */
    void callback_Setup_Tab2_DelDatabase();
    /**
     * @brief Callback invoked when the user deletes a Project
     */
    void callback_Setup_Tab2_DelProject();
    /**
     * @brief Callback invoked when the user creates a new Project
     */
    void callback_Setup_Tab2_NewProject();
    /**
     * @brief Callback invoked when the user has finished editing the
     * Project Author name
     */
    void callback_Setup_Tab2_ProjectAuthorAdd();
    /**
     * @brief Callback invoked when the user has finished editing the
     * Project Description
     */
    void callback_Setup_Tab2_ProjectDescAdd();
    /**
     * @brief Callback invoked when the user has selected a new
     * Project Directory
     */
    void callback_Setup_Tab2_ProjectDirAdd();
    /**
     * @brief Callback invoked when the user clicks the Browse Directory button
     */
    void callback_Setup_Tab2_ProjectDirBrowsePB();
    /**
     * @brief Callback invoked when the user has finished editing the Project Name
     */
    void callback_Setup_Tab2_ProjectNameAdd();
    /**
     * @brief Callback invoked when the user wants to refresh the database pulldown list
     */
    void callback_Setup_Tab2_RefreshDatabase();
    /**
     * @brief Callback invoked when the user has clicked the Reload Project button
     */
    void callback_Setup_Tab2_ReloadProject();
    /**
     * @brief Callback invoked when the user clicks the Save Project button
     */
    void callback_Setup_Tab2_SaveProject();
    /**
     * @brief Callback invoked when the user presses the Cancel button from the popup
     * progress dialog after invoking the Create Tables functionality
     */
    void callback_progressDlgCancel();
};

#endif // NMFSETUPTAB2_H
