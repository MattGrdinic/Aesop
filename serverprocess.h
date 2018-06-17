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

#ifndef SERVERPROCESS_H
#define SERVERPROCESS_H

#include "mainwindow.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

class ServerProcess : public QObject
{
    Q_OBJECT
public:
    explicit ServerProcess(QObject *parent = 0);

    QMap<int, QProcess*>myServerProcessMap;

    QProcess *myServerProcess;

    QString stdoutBuffer;
    QString stderrBuffer;

    int startProcess(const QString &, const QStringList &, const QString workingDir);

signals:

    void signal_serverStarted(QString, Q_PID);
    void signal_serverFinished(QString, int);
    void signal_serverStdOut(QString, Q_PID);
    void signal_serverStdErr(QString, Q_PID);

public slots:

    // process slots
    void slot_server_Started();
    void slot_server_Finished(int);
    void slot_server_readyReadStandardOutput();
    void slot_server_readyReadStandardError();
};

#endif // SERVERPROCESS_H
