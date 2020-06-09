/**
 * @file nmfSetupTab01.h
 * @brief GUI definition for the Setup Getting Started page class nmfSetup_Tab1
 *
 * This file contains the GUI definitions for the Setup Getting Started page. This
 * page displays text that describes the setup process to the user.
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

#ifndef NMFSETUPTAB1_H
#define NMFSETUPTAB1_H

/**
 * @brief The Setup Tab 1 displays Setup help information for the user
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
