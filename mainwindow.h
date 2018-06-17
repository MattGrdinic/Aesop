/****************************************************************************
 **
 ** HPHP GUI
 ** Copyright (C) 2011  Matthew Grdinic
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStringList>

#include <QModelIndex>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QErrorMessage>
#include <QMessageBox>
#include <QGridLayout>
#include <QScrollArea>

#include <QFile>
#include <QTextStream>

#include <QTableWidgetItem>

#include <QProcess>
#include <QWaitCondition>

#include "domparser.h"
#include "serverprocess.h"
#include "utility.h"

//#include <security/pam_appl.h>
//#include <security/pam_modules.h>
//#include <security/pam_ext.h>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QString xmlFilePath;

    QStringList filePathStringList;

    QFileSystemModel *sourceModel;
    QSortFilterProxyModel *proxyModel;

    QProcess *myCompilerProcess;


    QProcess *myServerProcess;
    QMap<int, QProcess*>myServerProcessMap;

    DomParser *domParser; // server settings

    void createConnections();

    // compiler
    void createFileList(QString &);

    void createOutputDirectory(QString &);
    void removeOutputDirectory(QString &);

    // application

    void stopServer(const QString&, const int&);

    bool initPAM(QString);

private slots :

    void slot_pushButtonDocumentationCompiler_clicked();

    void slot_refreshAppList(int);

    void slot_selectFolder();
    void slot_rebuildFileList();
    void slot_folderProcessStdOut(QString, Q_PID);
    void slot_folderProcessFinished(QString, int);

    // code check
    void slot_compileCodeCheck();
    void slot_codeCheckProcessStdOut(QString, Q_PID);
    void slot_codeCheckProcessFinished(QString, int);

    void slot_compileCode();
    void slot_cancelCompile();

    // process slots
    void slot_processStarted();
    void slot_process_readyReadStandardOutput();
    void slot_process_readyReadStandardError();
    void slot_processFinished(int);

    // run code slots
    void slot_startStopApp();

    // server management
    void slot_addAppListRow();

    void slot_appListCellClicked(int, int);

    void slot_serverSelectSourceRootClicked();

    void slot_saveServerSettings();
    void slot_saveHDFFile();

    void slot_deleteApp();

    void slot_serverClearOutput();

    // server slots
    void slot_serverStarted(QString, Q_PID);
    void slot_serverFinished(QString, int);
    void slot_serverStdOut(QString,Q_PID);
    void slot_serverStdErr(QString,Q_PID);

};

#endif // MAINWINDOW_H
