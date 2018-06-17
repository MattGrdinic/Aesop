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

#ifndef DOMPARSER_H
#define DOMPARSER_H

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QMap>
#include <QString>

class DomParser
{

public:
    DomParser(const QString xmlFilePath);

    QString xmlFilePath;

    QDomDocument *domDocument;

    bool readFile(const QString &fileName);

    QDomDocument* getDomDocument();

    QMap<QString, QString>getAppRecord(const QString &appName);

    bool updateAddAppRecord(const QString &appName, const QMap<QString, QString> &map);

    void updateAppRecord(const QString &appName, const QMap<QString, QString> &map);

    void addAppRecord(const QString &appName, const QMap<QString, QString> &map);

    void deleteAppRecord(const QString &appName);

private :


};

#endif // DOMPARSER_H
