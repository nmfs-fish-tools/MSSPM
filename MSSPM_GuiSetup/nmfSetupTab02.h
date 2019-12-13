#ifndef NMFSETUPTAB2_H
#define NMFSETUPTAB2_H

/**
 * @brief Defines the behavior and callback functionality for Setup Tab 2.
 *
 * This tab allows the user to defined Project meta data such as name,
 * location, author, and a brief description.
 */
class nmfSetup_Tab2: public QObject
{
    Q_OBJECT

    nmfLogger*            m_logger;
    nmfDatabase*          m_databasePtr;
    QProgressDialog*      m_progressDlg;
    bool                  m_NewProject;
    bool                  m_okToCreateTables;
    QString               m_ProjectDir;
    QString               m_LastProjectDatabase;
    QString               m_ProjectDatabase;
    QString               m_ProjectName;
    QString               m_ProjectAuthor;
    QString               m_ProjectDescription;
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
    QPushButton* Setup_Tab2_BrowseProjectPB;
    QPushButton* Setup_Tab2_SetProjectPB;
    QPushButton* Setup_Tab2_ReloadProjectPB;
    QLineEdit*   Setup_Tab2_ProjectNameLE;
    QLineEdit*   Setup_Tab2_ProjectAuthorLE;
    QLineEdit*   Setup_Tab2_ProjectDescLE;
    QLineEdit*   Setup_Tab2_ProjectDirLE;
    QTextEdit*   SetupOutputTE;

signals:
    void LoadDatabase(QString database);
    void LoadProject();
    void WaitCursor();
    void SaveMainSettings();
    void UpdateWindowTitle();
    void RemoveGuildsAndSpecies();
    void ClearEstimationTables();
    void ReloadWidgets();

public:
    /**
     * @brief nmfSetup_Tab2 : class constructor
     * @param tabs : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param databasePtr : pointer to the application database
     */
    nmfSetup_Tab2(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr);
    virtual ~nmfSetup_Tab2();

    bool    isValidProjectName(QString projectName);
    bool    validProjectData();
    void    createTables(QString databaseName);
    void    initDatabase(QString database);
    void    loadDatabaseNames(QString NameToSelect);
    void    loadProject(nmfLogger *logger, QString fileName);
    void    loadWidgets();
    void    readSettings();
    void    saveSettings();
    QString getProjectName();
    QString getProjectDatabase();
    QString getProjectDir();
    QString getProjectAuthor();
    QString getProjectDescription();

public Q_SLOTS:
    void callback_Setup_Tab2_ProjectDirBrowsePB();
    void callback_Setup_Tab2_AddDatabase();
    void callback_Setup_Tab2_DelDatabase();
    void callback_Setup_Tab2_ProjectDirAdd();
    void callback_Setup_Tab2_ProjectNameAdd();
    void callback_Setup_Tab2_ProjectAuthorAdd();
    void callback_Setup_Tab2_ProjectDescAdd();
    void callback_Setup_Tab2_DatabaseChanged(QString dbName);
    void callback_Setup_Tab2_SaveProject();
    void callback_Setup_Tab2_NewProject();
    void callback_Setup_Tab2_BrowseProject();
    void callback_Setup_Tab2_DelProject();
    void callback_Setup_Tab2_ReloadProject();
    void callback_progressDlgCancel();
};

#endif // NMFSETUPTAB2_H
