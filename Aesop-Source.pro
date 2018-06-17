#-------------------------------------------------
#
# Project created by QtCreator 2011-03-07T13:14:43
#
#-------------------------------------------------

QT       += core gui xml

TARGET = Aesop
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    domparser.cpp \
    serverprocess.cpp \
    utility.cpp

HEADERS  += mainwindow.h \
    domparser.h \
    serverprocess.h \
    utility.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

INCLUDEPATH += \
    #/usr/share/pam

#LIBS += -L/usr/lib -lpam \
