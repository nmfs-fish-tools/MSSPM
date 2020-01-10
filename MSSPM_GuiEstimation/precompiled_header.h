/** @file precompiled_header.h
 * @brief Include files for project classes
 *
 * This file contains the project, system, Qt, and Boost include files
 * needed for the .h files in this project.  Inculding them here allows them to
 * be precompiled by the compiler and hence represent a savings in compile time.
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

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QModelIndex>
#include <QObject>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpacerItem>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStringList>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>
#include <QTextStream>
#include <QToolTip>
#include <QUiLoader>
#include <QValidator>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

