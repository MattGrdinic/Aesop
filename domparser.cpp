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

#include <QDebug>

#include "domparser.h"

DomParser::DomParser(const QString xmlFilePath)
{
    this->xmlFilePath = xmlFilePath;
}

bool DomParser::readFile(const QString &fileName)
{
    domDocument = new QDomDocument("mydocument");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!domDocument->setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    return true;
}

QDomDocument* DomParser::getDomDocument()
{
    return this->domDocument;
}

QMap<QString, QString> DomParser::getAppRecord(const QString &appName)
{
    QMap<QString, QString> map;

    // loop up record based on app name
    QDomElement docElem = this->domDocument->documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            //qDebug() << e.tagName();
            QString t = e.attribute("name");
            if(t == appName){
                // add this elements options to the map
                QDomNode childnode = e.firstChild();
                while(!childnode.isNull()) {
                    QDomElement e = childnode.toElement();
                    //qDebug() << e.tagName();
                    map[e.tagName()] = e.text();
                    childnode = childnode.nextSibling();
                }
            }
        }
        n = n.nextSibling();
    }

    return map;
}

bool DomParser::updateAddAppRecord(const QString &appName, const QMap<QString, QString> &map)
{

    bool addRecord = true;

    // do we have a record for this item?
    QDomElement docElem = this->domDocument->documentElement();

    QDomNode n = this->domDocument->documentElement().firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString t = e.attribute("name");
            if(t == appName){
                this->updateAppRecord(appName, map);
                addRecord = false;
            }
        }
        n = n.nextSibling();
    }

    if(addRecord == true){
        this->addAppRecord(appName, map);
    }

    return true;
}

void DomParser::updateAppRecord(const QString &appName, const QMap<QString, QString> &map)
{
    // iterate over list and update values
    QDomElement root = this->domDocument->documentElement();

    QDomNode n = root.firstChild();

    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString t = e.nodeName();

            if(t == "app" && e.attribute("name") == appName){

                QDomNode childnode = e.firstChild();

                while(!childnode.isNull()) {

                    QDomElement e = childnode.toElement();

                    QString c = e.nodeName();

                    //qDebug() << "Node name to update: " << e.nodeName();
                    //qDebug() << "Value to update" << map[c];

                    // TODO: add new elements on the fly

                    // map name key determins field to update
                    QDomNode t = e.firstChild();
                    if(t.isNull()){ // empty nodes must be created on the fly
                        QDomText newText = domDocument->createTextNode(map[c]);
                        e.appendChild(newText);
                    } else {
                        t.setNodeValue(map[c]);
                    }

                    childnode = childnode.nextSibling();
                }

            }
        }
        n = n.nextSibling();
    }

    // save xml
    QFile file(this->xmlFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream s(&file);

    root.save(s, 1);


}

/**
  <app name="demo-2">
   <port>8080</port>
   <mode>0</mode>
   <executable>program</executable>
   <serversourceroot>/home/matthew/www/form-login</serversourceroot>
  </app>
  */
void DomParser::addAppRecord(const QString &appName, const QMap<QString, QString> &map)
{
    QDomElement root = this->domDocument->documentElement();

    QMapIterator<QString, QString> i(map);

    // create parent
    QDomElement parent = domDocument->createElement("app");
    parent.setAttribute("name", appName);

    while(i.hasNext()){
        i.next();
        QDomNode n = domDocument->createElement(i.key());
        QDomText t = domDocument->createTextNode(i.value());
        n.appendChild(t);
        parent.appendChild(n);
    }

    root.appendChild(parent);

    // save xml
    QFile file(this->xmlFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream s(&file);

    root.save(s, 1);

}

void DomParser::deleteAppRecord(const QString &appName)
{

    QDomElement root = this->domDocument->documentElement();

    QDomNode appNode = root.firstChild();

    while(!appNode.isNull()) {
        QDomElement appElement = appNode.toElement();
        if(!appElement.isNull()) {
            QString appElementName = appElement.nodeName();

            if(appElementName == "app" && appElement.attribute("name") == appName){

                appNode = root.removeChild(appNode);

            }
        }

        appNode = appNode.nextSibling();
    }

    // save xml
    QFile file(this->xmlFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream s(&file);

    root.save(s, 1);

}
