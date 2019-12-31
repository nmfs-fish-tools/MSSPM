#ifndef NMFSETUPTAB1_H
#define NMFSETUPTAB1_H

/**
 * @brief Defines the behavior and callback functionality for Setup Tab 1.
 *
 * This tab describes the other Setup tabs to the user. There is no user functionality
 * in this tab. It's only purpose is to give the user an introduction to the Setup tabs.
 */
class nmfSetup_Tab1: public QObject
{
    Q_OBJECT

    QTabWidget* Setup_Tabs;
    QWidget*    Setup_Tab1_Widget;
    QTextEdit*  Setup_Tab1_RequirementsTE;
    QLabel*     Setup_Tab1_RequirementsLBL;

public:
    /**
     * @brief nmfSetup_Tab1 : This GUI display some Getting Started text to guide the user through the rest of the Setup process
     * @param setupTab : The tab widget into which the Setup tab will be placed
     */
    nmfSetup_Tab1(QTabWidget* setupTab);
   ~nmfSetup_Tab1() {}
};

#endif // NMFSETUPTAB1_H
