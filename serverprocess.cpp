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

#include "serverprocess.h"
#include "mainwindow.h"

ServerProcess::ServerProcess(QObject *parent) :
        QObject(parent)
{
}

int ServerProcess::startProcess(const QString & program, const QStringList & args, const QString workingDir = "")
{
    myServerProcess = new QProcess(this);

    if(workingDir != ""){
        myServerProcess->setWorkingDirectory(workingDir);
    }

    QStringList l;
    l << "-mserver" << "-p8080";

    myServerProcess->start(program, args);

    connect(myServerProcess, SIGNAL(started()),
            this, SLOT(slot_server_Started()));

    connect(myServerProcess, SIGNAL(finished(int)),
            this, SLOT(slot_server_Finished(int)));

    connect(myServerProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(slot_server_readyReadStandardOutput()));

    connect(myServerProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(slot_server_readyReadStandardError()));

    if (!myServerProcess->waitForStarted(10000))
             return -1;

    return myServerProcess->pid();

}

// slots
void ServerProcess::slot_server_Started()
{

    QString myString = "";

    emit signal_serverStarted(myString, myServerProcess->pid());

    emit signal_serverStdOut(myString, myServerProcess->pid());
}

void ServerProcess::slot_server_Finished(int statusCode)
{
    QString myString(myServerProcess->readAllStandardError());

    emit signal_serverFinished(myString, statusCode);

}

void ServerProcess::slot_server_readyReadStandardOutput()
{
    QString myString(myServerProcess->readAllStandardOutput());

    emit signal_serverStdOut(myString, myServerProcess->pid());
}

void ServerProcess::slot_server_readyReadStandardError()
{
    QString myString(myServerProcess->readAllStandardError());

    emit signal_serverStdErr(myString, myServerProcess->pid());
}
