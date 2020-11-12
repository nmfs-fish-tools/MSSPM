/**
 * @file precompiled_header.h
 * @brief Include files for project classes
 *
 * This file contains the project, system, Qt, and Boost include files
 * needed for the .h files in this project.  Inculding them here allows them to
 * be precompiled by the compiler and hence represent a savings in compile time.
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

#include "nmfDatabase.h"
#include "nmfLogger.h"
#include "nmfUtils.h"

#include <string>
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cmath>
#include <cassert>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstdlib>


#include <Q3DBars>
#include <QAbstract3DGraph>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QComboBox>
#include <QDir>
#include <QFont>
#include <QFrame>
#include <QFuture>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListView>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPointer>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QShortcut>
#include <QSpacerItem>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUiLoader>
#include <QValidator>
#include <QValueAxis>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>
#include <QWhatsThis>

#include <QtConcurrent>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/version.hpp>
