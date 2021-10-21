/**
 * @file nmfCovariateTab01.h
 * @brief GUI definition for the nmfCovariate_Tab_01 Data class
 * @date Oct 20, 2021
 * @author Ronald Klasky
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
#ifndef MSSPM_GUICOVARIATE2_H
#define MSSPM_GUICOVARIATE2_H

#include "nmfConstantsMSSPM.h"

class nmfCovariate_Tab2: public QObject
{
    Q_OBJECT

private:

public:
    nmfCovariate_Tab2(QTabWidget*  tabs,
                      nmfLogger*   logger,
                      nmfDatabase* databasePtr,
                      std::string& projectDir);
    virtual ~nmfCovariate_Tab2() {}

};

#endif // MSSPM_GUICOVARIATE2_H
