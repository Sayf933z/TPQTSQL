#include "widget.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtDebug>
#include <QtSql/QSqlQuery>
#include <QSqlError>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("jeu");

    db.setUserName("sayf");
    db.setPassword("sayfamine");
    if(db.open())
    {
        qDebug() << "Vous êtes maintenant connecté à " << db.hostName() ;

        QSqlQuery query;

        query.exec("SELECT * FROM `jeu`");
        while ( query.next()) {
            QString id = query.value(0).toString();
            QString Nom = query.value(1).toString();
            QString club = query.value(2).toString();
            qDebug() << Nom << club;
        }
        qDebug() << "Requête ok ! :)" ;
        db.close();
    }
    else
    {
        qDebug() << "La connexion a échouée, désolé" << db.lastError().text();
    }
    w.show();
    return a.exec();
}
