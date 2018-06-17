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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // settings and psudo-globals
    QString appPath = QCoreApplication::applicationDirPath();
    this->xmlFilePath = appPath + "/../compiled-web-apps/app-list.xml";

    // create connections
    this->createConnections();

    // create app list
    this->slot_refreshAppList(0);
    ui->tableWidget_appList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // manage app list

    domParser = new DomParser(xmlFilePath);

    if(!domParser->readFile(this->xmlFilePath)){
        QErrorMessage m(this);
        m.showMessage("App settings file not read or contains XML errors.");
        m.exec();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createConnections()
{

    // help
    connect(ui->pushButton_helpCompiler, SIGNAL(clicked()),
            this, SLOT(slot_pushButtonDocumentationCompiler_clicked()));

    connect(ui->pushButton_selectFolder, SIGNAL(clicked()),
            this, SLOT(slot_selectFolder()));

    connect(ui->pushButton_compile, SIGNAL(clicked()),
            this, SLOT(slot_compileCode()));

    connect(ui->pushButton_cancelCompile, SIGNAL(clicked()),
            this, SLOT(slot_cancelCompile()));

    connect(ui->pushButton_compileRebuildFileList, SIGNAL(clicked()),
            this, SLOT(slot_rebuildFileList()));

    // server management
    connect(ui->tableWidget_appList, SIGNAL(cellClicked(int,int)),
            this, SLOT(slot_appListCellClicked(int, int)));

    connect(ui->pushButton_startStopApp, SIGNAL(clicked()),
            this, SLOT(slot_startStopApp()));

    connect(ui->pushButton_serverSelectSourceRoot, SIGNAL(clicked()),
            this, SLOT(slot_serverSelectSourceRootClicked()));

    connect(ui->tabWidget_main, SIGNAL(currentChanged(int)),
            this, SLOT(slot_refreshAppList(int)));


    connect(ui->pushButton_deleteApp, SIGNAL(clicked()),
            this, SLOT(slot_deleteApp()));

    connect(ui->pushButton_serverClearOutput, SIGNAL(clicked()),
            this, SLOT(slot_serverClearOutput()));

    // server options
    connect(ui->pushButton_saveServerSettings, SIGNAL(clicked()),
            this, SLOT(slot_saveServerSettings()));

    connect(ui->pushButton_serverSaveHDFFile, SIGNAL(clicked()),
            this, SLOT(slot_saveHDFFile()));

    // code check
    connect(ui->pushButton_compileCodeCheck, SIGNAL(clicked()),
            this, SLOT(slot_compileCodeCheck()));

}


void MainWindow::slot_pushButtonDocumentationCompiler_clicked()
{
    QString compilerHelpString = "Help Text Not Loaded.";
    QFile instructionsCompiler(":/resources/help/compiler.txt");

    if(instructionsCompiler.open(QIODevice::ReadOnly)){
        compilerHelpString = instructionsCompiler.readAll();
    }

    QString serverHelpString = "Help Text Not Loaded.";
    QFile instructionsServer(":/resources/help/server.txt");

    if(instructionsServer.open(QIODevice::ReadOnly)){
        serverHelpString = instructionsServer.readAll();
    }

    // create
    QTabWidget *helpTabs = new QTabWidget();
    QWidget *helpCompiler = new QWidget();
    QWidget *helpServer = new QWidget();

    // design
    QGridLayout *compilerLayout = new QGridLayout();
    QGridLayout *serverLayout = new QGridLayout();

    QScrollArea *compilerScrollArea = new QScrollArea();
    QScrollArea *serverScrollArea = new QScrollArea();

    QLabel *compilerHelpLabel = new QLabel(compilerHelpString, helpCompiler);
    compilerHelpLabel->setMinimumSize(200, 200);
    QLabel *serverHelpLabel = new QLabel(serverHelpString, helpServer);
    serverHelpLabel->setMinimumSize(200, 200);

    compilerScrollArea->setWidget(compilerHelpLabel);
    serverScrollArea->setWidget(serverHelpLabel);

    compilerLayout->addWidget(compilerScrollArea);
    serverLayout->addWidget(serverScrollArea);

    helpCompiler->setLayout(compilerLayout);
    helpServer->setLayout(serverLayout);

    // insert tab
    helpTabs->addTab(helpCompiler, "Compiler");
    helpTabs->addTab(helpServer, "Server");

    // main layout
    QGridLayout *helpLayout = new QGridLayout();
    helpLayout->addWidget(helpTabs);

    // main init
    QDialog *helpDialog = new QDialog(this);
    helpDialog->setLayout(helpLayout);
    helpDialog->setFixedWidth(590);
    helpDialog->show();
}




// COMPILER


void MainWindow::slot_selectFolder()
{

    // ui
    ui->tabWidget_2->setCurrentIndex(0);

    QString dir = QFileDialog::getExistingDirectory(
            this, tr("Select Directory"),
            "/home",
            QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

    ui->lineEdit_folderToProcess->setText(dir);

    if(dir != "") {
        ui->pushButton_compile->setEnabled(true);
        this->createFileList(dir);
    }
}

void MainWindow::slot_rebuildFileList()
{

    QString path = ui->lineEdit_folderToProcess->text();
    if(ui->lineEdit_folderToProcess->text() != ""){
        this->createFileList(path);
    }
}

// This list must be relative to the base folder, and not contain absolute paths.
void MainWindow::createFileList(QString & path)
{

    // update UI
    ui->tabWidget_2->setCurrentIndex(0);

    // ui
    ui->textEdit_fileList->clear();
    filePathStringList.clear();

    // new program
    QString program = "find";
    QStringList arguments;
    arguments << "." << "-name" << "*.php";

    ServerProcess *p = new ServerProcess(this);

    int pid = p->startProcess(program, arguments, path);

    //qDebug() << "Find PID: " << pid;

    connect(p, SIGNAL(signal_serverFinished(QString,int)),
            this, SLOT(slot_folderProcessFinished(QString,int)));

    connect(p, SIGNAL(signal_serverStdOut(QString,Q_PID)),
            this, SLOT(slot_folderProcessStdOut(QString,Q_PID)));

}

void MainWindow::slot_folderProcessStdOut(QString message, Q_PID pid)
{
    ui->textEdit_fileList->append(message + "\n");
    filePathStringList << message;
}

void MainWindow::slot_folderProcessFinished(QString message, int code)
{
    QFile file(ui->lineEdit_folderToProcess->text() + "/files.list");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream s(&file);
    for (int i = 0; i < filePathStringList.size(); ++i){
        s << filePathStringList.at(i);
    }
}

void MainWindow::slot_compileCodeCheck()
{
    // do we have a path
    if(ui->lineEdit_folderToProcess->text() == ""){
        QErrorMessage m(this);
        m.showMessage("You must set a folder to process first.");
        m.exec();
        return;
    }

    // update UI
    ui->tabWidget_2->setCurrentIndex(2);

    ui->textEdit_compilerCodeCheckOutput->clear();

    // run grep - grep -iR eval\( ./
    // TODO: expand to more cases, (mysqli, ftp, zip, etc)

    QString program = "grep";
    QStringList arguments;
    arguments << "-iR" << "eval\(" << "./";

    QString path = ui->lineEdit_folderToProcess->text();

    ServerProcess *p = new ServerProcess(this);

    int pid = p->startProcess(program, arguments, path);

    //qDebug() << "Run grep: " << pid;

    connect(p, SIGNAL(signal_serverFinished(QString,int)),
            this, SLOT(slot_codeCheckProcessFinished(QString,int)));

    connect(p, SIGNAL(signal_serverStdOut(QString,Q_PID)),
            this, SLOT(slot_codeCheckProcessStdOut(QString,Q_PID)));
}

void MainWindow::slot_codeCheckProcessStdOut(QString message, Q_PID pid)
{
    ui->textEdit_compilerCodeCheckOutput->setText(message + "\n");
}

void MainWindow::slot_codeCheckProcessFinished(QString message, int code)
{
    QString message_out = "\n\n=====CODE CHECK COMPLETE=====\n\n";
    message_out += "If we have any text above the complete message your code will";
    message_out += " most likely compile, but not run, as it uses the unsupported eval() function.";
    message_out += "\n\nIt should be said though that JavaScript also uses eval(), and ";
    message_out += "this report may be showing code that uses an include of raw JavaScript via PHP.";
    message_out += " as WordPress does in some versions.";
    ui->textEdit_compilerCodeCheckOutput->append(message_out);
}



void MainWindow::createOutputDirectory(QString & appPath)
{

    QString outputPath = appPath + "/../compiled-web-apps/" + ui->lineEdit_projectName->text();

    // create processed folder if needed
    if(!QDir(outputPath).exists()){

        QDir outPath(outputPath);
        if(!outPath.mkpath(outputPath)){
            qDebug() << "Cannot create outoput folder.";
        }
    }

}

void MainWindow::removeOutputDirectory(QString & appPath)
{

    Utility *u = new Utility;
    QString outputPath = appPath + "/../compiled-web-apps/" + ui->lineEdit_projectName->text();
    QDir d(outputPath);
    u->removeDirectory(d);

}

void MainWindow::slot_compileCode()
{

    if(ui->lineEdit_projectName->text() == ""){
        QErrorMessage m(this);
        m.showMessage("You must set a Project/Executable name.");
        m.exec();
        return;
    }

    // update UI
    ui->tabWidget_2->setCurrentIndex(1);

    ui->textEdit_compilerOutput->clear();

    // create/gather options

    // app path: e.g. /home/matthew/Documents/hiphop-ui/hiphop-gui-build-desktop"
    QString appPath = QCoreApplication::applicationDirPath();

    // build output directory
    this->removeOutputDirectory(appPath);
    this->createOutputDirectory(appPath);

    QString program = appPath + "/../dev/hiphop-php/src/hphp/hphp";

    // set params
    QString keep_tempdir = "--keep-tempdir=1";
    QString log_level = "--log=" + QString::number(ui->comboBox_logLevel->currentIndex());
    QString input_directory = "--input-dir=" + ui->lineEdit_folderToProcess->text();
    QString program_name = "--program=" + ui->lineEdit_projectName->text();
    QString output_directory = "--output-dir=" +  appPath + "/../compiled-web-apps/"
                               + ui->lineEdit_projectName->text();


    QString target;

    switch(ui->comboBox_compilerTarget->currentIndex()){
    case 0 :
        target = "--target=analyze";
        break;
    case 1 :
        target = "--target=run";
        break;
    case 2 :
        target = "--target=cpp";
        break;
    case 3 :
        target = "--target=php";
        break;
    case 4 :
        target = "--target=set-ext-cpp";
        break;
    case 5 :
        target = "--target=filecache";
        break;
    }

    // advanced options
    QString cluster_count = "--cluster-count=" + ui->lineEdit_compilerClusterCount->text();

    QString force_mode;
    switch(ui->comboBox_compilerForceMode->currentIndex()){
    case 0 :
        force_mode = "--force=0";
        break;
    case 1 :
        force_mode = "--force=1";
        break;
    }

    QString input_list = "--input-list=files.list";

    QStringList arguments;

    arguments << keep_tempdir
              << log_level
              //<< input_directory
              << input_list
              << program_name
              << output_directory
              << target
              << force_mode
              << cluster_count;

    myCompilerProcess = new QProcess(this);

    myCompilerProcess->setWorkingDirectory(ui->lineEdit_folderToProcess->text());

    // we set the vars like so:
    // export CMAKE_PREFIX_PATH=`/bin/pwd`/../
    // export HPHP_HOME=`/bin/pwd`
    // export HPHP_LIB=`/bin/pwd`/bin

    // which for a call to #env shows these values:
    // PWD=/home/matthew/Documents/hiphop-ui/dev/hiphop-php
    // HPHP_HOME=/home/matthew/Documents/hiphop-ui/dev/hiphop-php
    // HPHP_LIB=/home/matthew/Documents/hiphop-ui/dev/hiphop-php/bin
    // CMAKE_PREFIX_PATH=/home/matthew/Documents/hiphop-ui/dev/hiphop-php/../
    // /home/matthew/Documents/hiphop-ui/dev/hiphop-php/../

    // Add/Set environment variables    
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("HPHP_HOME", appPath + "/../dev/hiphop-php");
    env.insert("HPHP_LIB",appPath + "/../dev/hiphop-php/bin");
    // http://www.kdedevelopers.org/node/3800 < docs for CMAKE_PREFIX_PATH
    env.insert("CMAKE_PREFIX_PATH", appPath + "/../dev/");

    // test
    //qDebug() << env.contains("CMAKE_PREFIX_PATH");

    //qDebug() << env.toStringList();

    myCompilerProcess->setProcessEnvironment(env);

    myCompilerProcess->start(program, arguments);

    connect(myCompilerProcess, SIGNAL(started()), this, SLOT(slot_processStarted()));

    connect(myCompilerProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(slot_process_readyReadStandardError()));

    connect(myCompilerProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(slot_process_readyReadStandardOutput()));

    connect(myCompilerProcess, SIGNAL(finished(int)),
            this, SLOT(slot_processFinished(int)));

}

/**
  * TODO: // Not quite, g++ is running and needs to be termed.
  *          This just kills the main process...
  */
void MainWindow::slot_cancelCompile()
{
    myCompilerProcess->kill();
}

void MainWindow::slot_processStarted()
{
    qDebug() << "Compiler Process Started. PID:" << myCompilerProcess->pid();
    // ui
    ui->pushButton_cancelCompile->setEnabled(true);

    QString myString = "Compiler Process Started.";

    ui->textEdit_compilerOutput->insertHtml(
            "<br/><p style=\"color:Purple; font-size:15px;\">" + myString + "</p><br/><br/>");
}

void MainWindow::slot_process_readyReadStandardError()
{
    qDebug() << "Error incoming...";

    QString myString(myCompilerProcess->readAllStandardError());

    ui->textEdit_compilerOutput->insertHtml(
            "<br/><p style=\"color:Red;\">" + myString + "</p><br/><br/>");
}

void MainWindow::slot_process_readyReadStandardOutput()
{

    QString myString(myCompilerProcess->readAllStandardOutput());

    ui->textEdit_compilerOutput->insertHtml(
            "<p style=\"color:Black;\">" + myString + "</p><br/><br/>");
}

void MainWindow::slot_processFinished(int statusCode)
{
    qDebug() << "Compiler Process Finished. Status Code:" << statusCode;

    // ui
    ui->pushButton_cancelCompile->setEnabled(false);

    QString message = "Compiler Process Finished. Status Code: " + QString::number(statusCode);

    ui->textEdit_compilerOutput->insertHtml(
            "<p style=\"color:Purple; font-size:15px;\">" + message + "</p>");

    // if analyze mode, read error file
    if(ui->comboBox_compilerTarget->currentIndex() == 0){

        QString code_error = QCoreApplication::applicationDirPath()
                          + "/../compiled-web-apps/"
                          + ui->lineEdit_appPathName->text()
                          + ui->lineEdit_projectName->text()
                          + "/CodeError.js";

        qDebug() << code_error;

        QFile file(code_error);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        ui->textEdit_compilerOutput->insertHtml(
                "<br/><p style=\"color:Black;\">Compiler Error/Info Report:</p><br/><br/>");
        ui->textEdit_compilerOutput->insertHtml(file.readAll());

    }

    // grab and save hdf file
    QString hdf = "";
    QFile hdfFile(":/resources/hdf/base.hdf");

    if(hdfFile.open(QIODevice::ReadOnly)){
        hdf = hdfFile.readAll();
    }

    QString hdfSavePath = QCoreApplication::applicationDirPath()
                      + "/../compiled-web-apps/"
                      + ui->lineEdit_appPathName->text()
                      + ui->lineEdit_projectName->text()
                      + "/base.hdf";

    QFile f(hdfSavePath);
    f.open(QIODevice::WriteOnly);
    QTextStream s(&f);
    s << hdf;

}












// SERVER



void MainWindow::slot_refreshAppList(int refreshMode)
{

    if(refreshMode == 1){
        // clear existing list
        ui->tableWidget_appList->setRowCount(0);
    }

    QString appPath = QCoreApplication::applicationDirPath();

    QString app_directory = appPath + "/../compiled-web-apps/";

    QStringList apps, filters;

    // populate valid apps list
    QDirIterator i(app_directory, filters, QDir::AllDirs | QDir::NoDotAndDotDot);

    while(i.hasNext()){
        i.next();
        QString fileName = i.fileName();
        if(fileName == "."
                        || fileName == ""
                        || fileName == ".."
                        || fileName == ".DS_Store"){
            continue;
        }
        apps << i.filePath();
    }

    // refresh ui    
    ui->tableWidget_appList->setHorizontalHeaderLabels(
            QStringList() << tr("Application name") << tr("PID"));

    for(int row = 0; row < apps.size(); ++row){
        this->slot_addAppListRow();
        // set text of item
        QString appName = apps.at(row);
        QStringList appNameList = appName.split('/');
        ui->tableWidget_appList->item(row, 0)->setText(appNameList.last());
    }

    ui->tableWidget_appList->resizeColumnsToContents();
    ui->tableWidget_appList->horizontalHeader()->setStretchLastSection(true);

}

void MainWindow::slot_addAppListRow()
{
    int row = ui->tableWidget_appList->rowCount();
    ui->tableWidget_appList->insertRow(row);

    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget_appList->setItem(row, 0, item0);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget_appList->setItem(row, 1, item1);
}


void MainWindow::slot_appListCellClicked(int row, int column)
{
    // process settings for this application
    ui->lineEdit_appPathName->setText(ui->tableWidget_appList->item(row, 0)->text());

    // convinience
    if(ui->lineEdit_serverExecutableName->text() == ""){
        ui->lineEdit_serverExecutableName->setText(ui->tableWidget_appList->item(row, 0)->text());
    }

    if(ui->lineEdit_serverAdminPort->text() == ""){
        ui->lineEdit_serverAdminPort->setText("8088");
    }

    QMap <QString, QString>map =
            domParser->getAppRecord(ui->tableWidget_appList->item(row, 0)->text());

    QMapIterator<QString, QString> i(map);

    // clear all old/existing values

    ui->lineEdit_serverPort->clear();

    ui->lineEdit_serverAdminPort->clear();

    ui->comboBox_serverMode->setCurrentIndex(0);

    ui->lineEdit_serverExecutableName->clear();

    ui->lineEdit_serverSourceRoot->clear();

    ui->lineEdit_serverDefaultDocument->clear();

    ui->comboBox_serverUseHDF->setCurrentIndex(0);


    while(i.hasNext()){

        i.next();

        // update UI
        if(i.key() == "port"){
            ui->lineEdit_serverPort->setText(i.value());
        }
        if(i.key() == "adminport"){
            ui->lineEdit_serverAdminPort->setText(i.value());
        }
        if(i.key() == "mode"){
            ui->comboBox_serverMode->setCurrentIndex(i.value().toInt());
        }
        if(i.key() == "executable"){
            ui->lineEdit_serverExecutableName->setText(i.value());
        }
        if(i.key() == "serversourceroot"){
            ui->lineEdit_serverSourceRoot->setText(i.value());
        }
        if(i.key() == "defaultdocument"){
            ui->lineEdit_serverDefaultDocument->setText(i.value());
        }
        if(i.key() == "usehdf"){
            ui->comboBox_serverUseHDF->setCurrentIndex(i.value().toInt());
        }

    }

    // update button
    QString pid = ui->tableWidget_appList->item(row, 1)->text();
    qDebug() << pid;
    if(pid != "-1" && pid != ""){
        ui->pushButton_startStopApp->setText("Stop Application");
    } else {
        ui->pushButton_startStopApp->setText("Start Application");
    }

    // load hdf file
    QString appName = ui->tableWidget_appList->item(row, 0)->text();

    QString hdf_data = "";
    QString hdf_path = QCoreApplication::applicationDirPath()
                      + "/../compiled-web-apps/"
                      + appName + "/base.hdf";

    QFile hdfFile(hdf_path);

    if(hdfFile.open(QIODevice::ReadOnly)){
        hdf_data = hdfFile.readAll();
    } else { // no HDF file for some reason...

        // grab and save hdf file
        QString hdf = "";
        QFile hdfFile(":/resources/hdf/base.hdf");

        if(hdfFile.open(QIODevice::ReadOnly)){
            hdf = hdfFile.readAll();
        }

        QString hdfSavePath = QCoreApplication::applicationDirPath()
                              + "/../compiled-web-apps/"
                              + appName + "/base.hdf";

        QFile f(hdfSavePath);
        f.open(QIODevice::WriteOnly);
        QTextStream s(&f);
        s << hdf;

        // show dialog
        QErrorMessage m(this);
        QString message = "No HDF File Found. This is just a notice to let you know we'll go ahead and add one to the project.";
        m.showMessage(message);
        m.exec();

        // now read data - do this instead of adding a return which can thwart compiler optimizations.
        hdf_data = hdf;
    }

    ui->textEdit_serverHDFContents->setText(hdf_data);

}

void MainWindow::slot_saveServerSettings()
{

    QMap<QString, QString> map;

    int row = ui->tableWidget_appList->currentRow();

    if(row == -1){
        QErrorMessage m(this);
        m.showMessage("Please select an app from the 'Application List' first.");
        m.exec();
        return;
    }

    QString appTitle = ui->tableWidget_appList->item(row, 0)->text();

    map["port"] = ui->lineEdit_serverPort->text();
    map["adminport"] = ui->lineEdit_serverAdminPort->text();
    map["mode"] = QString::number(ui->comboBox_serverMode->currentIndex());
    map["executable"] = ui->lineEdit_serverExecutableName->text();
    map["serversourceroot"] = ui->lineEdit_serverSourceRoot->text();
    map["defaultdocument"] = ui->lineEdit_serverDefaultDocument->text();
    map["usehdf"] = QString::number(ui->comboBox_serverUseHDF->currentIndex());

    domParser->updateAddAppRecord(appTitle, map);
}

void MainWindow::slot_saveHDFFile()
{
    int row = ui->tableWidget_appList->currentRow();

    if(row == -1){
        QErrorMessage m(this);
        m.showMessage("Please select an app from the 'Application List' first.");
        m.exec();
        return;
    }

    QString appName = ui->tableWidget_appList->item(row, 0)->text();

    QString hdf_data = ui->textEdit_serverHDFContents->toPlainText();
    QString hdf_path = QCoreApplication::applicationDirPath()
                      + "/../compiled-web-apps/"
                      + appName + "/base.hdf";

    QFile hdfFile(hdf_path);
    hdfFile.open(QIODevice::WriteOnly);
    QTextStream s(&hdfFile);
    s << hdf_data;

    // save this items settings as well
    this->slot_saveServerSettings();
}

void MainWindow::slot_serverSelectSourceRootClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Server Source Directory"),
                                                     "/~",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_serverSourceRoot->setText(dir);
}


void MainWindow::slot_startStopApp()
{

    // do we have an app selected?

    int row = ui->tableWidget_appList->currentRow();

    if(row == -1){
        QErrorMessage m(this);
        m.showMessage("Please select an app from the 'Application List' first.");
        m.exec();
        return;
    }

    // is app already running?
    QString appPid = ui->tableWidget_appList->item(row, 1)->text();
    if(appPid != "-1" && appPid != ""){
        this->stopServer(appPid, row);
        return;
    }

    // do we have a port, or using hdf file?
    if(ui->lineEdit_serverPort->text() == "" && ui->comboBox_serverUseHDF->currentIndex() == 0){
        QErrorMessage m(this);
        m.showMessage("You must set a Server Port or enable HDF file processing.");
        m.exec();
        return;
    }

    // do we have an executable name?
    if(ui->lineEdit_serverExecutableName->text() == ""){
        QErrorMessage m(this);
        m.showMessage("You must supply an executable name.");
        m.exec();
        return;
    }

    QString appPath = QCoreApplication::applicationDirPath();

    QString program = appPath
                      + "/../compiled-web-apps/"
                      + ui->lineEdit_appPathName->text()
                      + "/" + ui->lineEdit_serverExecutableName->text();

    // set params - note we do not includes spaces between flag and option.
    QString server_port = "-p" + ui->lineEdit_serverPort->text();

    // note the = after the param name, the -- item(s) needs this.
    QString server_admin_port = "--admin-port=" + ui->lineEdit_serverAdminPort->text();

    QString server_mode = "";

    switch(ui->comboBox_serverMode->currentIndex()){
    case 0 :
        server_mode = "-mserver";
        break;
    case 1 :
        server_mode = "-mdeamon";
        break;
    case 2 :
        server_mode = "-mhphpi";
        break;
    default :
        server_mode = "";
    }

    QString server_sourceRoot = "-vServer.SourceRoot="
                                + ui->lineEdit_serverSourceRoot->text();

    QString server_defaultDocument = "-vServer.DefaultDocument="
                                + ui->lineEdit_serverDefaultDocument->text();

    QString hdf = "-c" + appPath + "/../dev/hiphop-php/bin/mime.hdf";

    QStringList arguments;


    if(ui->comboBox_serverUseHDF->currentIndex() == 1){
        QString hdf_path = appPath
                          + "/../compiled-web-apps/"
                          + ui->lineEdit_appPathName->text()
                          + "/base.hdf";
        hdf = "-c" + hdf_path;

        // we cannot pass port, source root, etc, as they override the hdf file
        arguments << server_mode
                  << hdf;

    } else {

        arguments << server_mode
                  << server_port
                  << server_admin_port
                  << server_sourceRoot
                  << hdf
                  << server_defaultDocument;
    }


    // start server

    if(ui->lineEdit_serverPort->text() == "80"){
        //bool result = this->initPAM(ui->lineEdit_serverExecutableName->text());
    }

    ServerProcess *p = new ServerProcess(this);

    int pid = p->startProcess(program, arguments, "");

    connect(p, SIGNAL(signal_serverStarted(QString,Q_PID)),
            this, SLOT(slot_serverStarted(QString,Q_PID)));

    connect(p, SIGNAL(signal_serverFinished(QString,int)),
            this, SLOT(slot_serverFinished(QString,int)));

    connect(p, SIGNAL(signal_serverStdOut(QString,Q_PID)),
            this, SLOT(slot_serverStdOut(QString,Q_PID)));

    connect(p, SIGNAL(signal_serverStdErr(QString,Q_PID)),
            this, SLOT(slot_serverStdErr(QString,Q_PID)));

    ui->tableWidget_appList->item(row, 1)->setText(QString::number(pid));

    // update button
    if(QString::number(pid) != "-1" && QString::number(pid) != ""){
        ui->pushButton_startStopApp->setText("Stop Application");
    }

}

bool MainWindow::initPAM(QString service)
{
//    qDebug() << "PAM Called";

//    struct pam_conv c;

//    struct pam_handle *pamh;

//    // start pam session

//    int ret = pam_start(service.toStdString().c_str(), "root", &c, &pamh);

//    if (ret != 0)
//        qDebug() << "Couldn't start pam session";

//    switch(ret){
//    case PAM_ABORT :
//        qDebug() << "PAM (pam_start): General Error.";
//        break;
//    case PAM_BUF_ERR :
//        qDebug() << "PAM (pam_start): Memory buffer error.";
//        break;
//    case PAM_SUCCESS :
//        qDebug() << "PAM (pam_start): Transaction was successful created.";
//        break;
//    case PAM_SYSTEM_ERR :
//        qDebug() << "PAM (pam_start): System Error.";
//        break;
//    }

//    // set user tokens

//    ret = pam_set_item(pamh, PAM_USER, "root");
//    ret = pam_set_item(pamh, PAM_RUSER, "matthew");

//    switch(ret){
//    case PAM_BAD_ITEM :
//        qDebug() << "PAM (pam_set_item): The application attempted to set an undefined or inaccessible item.";
//        break;
//    case PAM_BUF_ERR :
//        qDebug() << "PAM (pam_set_item): Memory buffer error.";
//        break;
//    case PAM_SUCCESS :
//        qDebug() << "PAM (pam_set_item): Data was successfully updated.";
//        break;
//    case PAM_SYSTEM_ERR :
//        qDebug() << "PAM (pam_set_item): The pam_handle_t passed as first argument was invalid.";
//        break;
//    }



//    // set user credentials (pam_setcred)

//    pam_setcred(pamh, PAM_ESTABLISH_CRED);

//    switch(ret){
//    case PAM_BUF_ERR :
//        qDebug() << "PAM (pam_setcred): Memory Buffer Error.";
//        break;
//    case PAM_CRED_ERR :
//        qDebug() << "PAM (pam_setcred): Failed to set user credentials.";
//        break;
//    case PAM_CRED_EXPIRED :
//        qDebug() << "PAM (pam_setcred): User credentials are expired.";
//        break;
//    case PAM_CRED_UNAVAIL :
//        qDebug() << "PAM (pam_setcred): Failed to retrieve user credentials.";
//        break;
//    case PAM_SUCCESS :
//        qDebug() << "PAM (pam_setcred): Data was successfully stored";
//        break;
//    case PAM_SYSTEM_ERR :
//        qDebug() << "PAM (pam_setcred): Null pointer submitted as PAM handle";
//        break;
//    case PAM_USER_UNKNOWN :
//        qDebug() << "PAM (pam_setcred): User is not known to an authentication module.";
//        break;
//    }



//    // authenticate user (pam_authenticate)

//    ret = pam_authenticate(pamh, 0);

//    switch(ret){
//    case PAM_ABORT :
//        qDebug() << "PAM: The application should exit immediately after calling pam_end(3) first";
//        break;
//    case PAM_AUTH_ERR :
//        qDebug() << "PAM: The user was not authenticated.";
//        break;
//    case PAM_CRED_INSUFFICIENT :
//        qDebug() << "PAM: Insufficient Credentials To Authenticate User.";
//        break;
//    case PAM_AUTHINFO_UNAVAIL :
//        qDebug() << "PAM: Authentication Modules Not Availible.";
//        break;
//    case PAM_MAXTRIES :
//        qDebug() << "PAM: Authentication Modules Limit Reached.";
//        break;
//    case PAM_SUCCESS :
//        qDebug() << "PAM: Authentication Modules Limit Reached.";
//        break;
//    case PAM_USER_UNKNOWN :
//        qDebug() << "PAM: User unknown to authentication service.";
//        break;
//    }


//    // start (open) user session

//    //pam_open_session

}

void MainWindow::stopServer(const QString &pid, const int &row)
{
    QProcess *p = new QProcess(this);
    p->start("/bin/kill", QStringList() << "-9" << pid);
    ui->pushButton_startStopApp->setText("Start Application");
    ui->tableWidget_appList->item(row, 1)->setText("");
}

void MainWindow::slot_deleteApp()
{

    // do we have an app to remove?
    int row = ui->tableWidget_appList->currentRow();

    QString appTitle = ui->tableWidget_appList->item(row, 0)->text();

    if(row == -1){
        QErrorMessage m(this);
        m.showMessage("Please select an app from the 'Application List' first.");
        m.exec();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Confirm Delete.");
    msgBox.setInformativeText("Are you sure you wish to delete this application?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    switch (ret) {
       case QMessageBox::No:
           return;
     }

    // is app already running?
    QString appPid = ui->tableWidget_appList->item(row, 1)->text();
    if(appPid != "-1" && appPid != ""){
        this->stopServer(appPid, row);
    }

    // remove files
    QString appPath = QCoreApplication::applicationDirPath();

    QString program = appPath
                      + "/../compiled-web-apps/"
                      + ui->lineEdit_appPathName->text();

    QDir d(program);

    Utility *u = new Utility();
    u->removeDirectory(d);

    // remove tableWidget entry
    ui->tableWidget_appList->removeRow(row);


    // remove xml entry

    domParser->deleteAppRecord(appTitle);

}

void MainWindow::slot_serverClearOutput()
{
    ui->textEdit_appOutput->clear();
}

void MainWindow::slot_serverStarted(QString message, Q_PID pid)
{
    QString final_message = "<p style=\"color:Purple;\">Process Started with PID: </p><br/><br/>";

    ui->textEdit_appOutput->insertHtml(message);
}

void MainWindow::slot_serverFinished(QString message, int code)
{
    int row = ui->tableWidget_appList->currentRow();

    ui->tableWidget_appList->item(row, 1)->setText("");

    QString c = QString::number(code).toStdString().c_str();

    message = "Process finished with message: " + message + "Status Code: " + c;

    ui->textEdit_appOutput->insertHtml(message);
}

void MainWindow::slot_serverStdOut(QString message, Q_PID pid)
{
    message = "<p style=\"color:Black;\">" + message + "</p><br/><br/>";

    ui->textEdit_appOutput->insertHtml(message);
}

void MainWindow::slot_serverStdErr(QString message, Q_PID pid)
{

    message = "<p style=\"color:Red;\">" + message + "</p><br/><br/>";

    ui->textEdit_appOutput->insertHtml(message);
}
