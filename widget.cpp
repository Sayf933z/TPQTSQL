#include "widget.h"
#include "ui_widget.h"
#include <QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTableWidget>
#include <QtDebug>
#include <QSqlError>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Initialiser le QTableWidget
    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(0);  // Commencer avec 0 ligne
    tableWidget->setColumnCount(3);  // Trois colonnes (Nom, Club, Note)
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Club" << "Note");

    // Activer l'édition des cellules
    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    // Connexion à la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("jeu");
    db.setUserName("sayf");
    db.setPassword("sayfamine");

    if (db.open())
    {
        qDebug() << "Connecté à la base de données.";

        // Créer une requête SQL pour récupérer les données
        QSqlQuery query("SELECT * FROM `jeu`");

        // Remplir le QTableWidget avec les données de la base
        int row = 0;
        while (query.next()) {
            QString nom = query.value(1).toString();
            QString club = query.value(2).toString();
            int note = query.value(3).toInt();

            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(nom));
            tableWidget->setItem(row, 1, new QTableWidgetItem(club));
            tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(note)));
            row++;
        }

        // Redimensionner et positionner le tableau
        tableWidget->resize(400, 300);
        tableWidget->move(50, 50);

        // Connecter le signal 'itemChanged' à la méthode 'updateNoteInDatabase'
        connect(tableWidget, &QTableWidget::itemChanged, this, &Widget::updateNoteInDatabase);

        db.close();
    }
    else
    {
        qDebug() << "Échec de la connexion à la base de données:" << db.lastError().text();
    }
}

void Widget::updateNoteInDatabase(QTableWidgetItem* item)
{
    // Vérifier si la colonne modifiée est la colonne "Note" (colonne 2)
    if (item->column() != 2) return;

    // Récupérer la ligne et la nouvelle valeur de la note
    int row = item->row();
    bool ok;
    int newNote = item->text().toInt(&ok);

    if (!ok) {
        qDebug() << "La note n'est pas un entier valide";
        return;
    }

    // Récupérer le nom du joueur (colonne 0)
    QString Nom = tableWidget->item(row, 0)->text();

    // Créer une connexion à la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("jeu");
    db.setUserName("sayf");
    db.setPassword("sayfamine");

    if (db.open()) {
        QSqlQuery query;
        query.prepare("UPDATE `jeu` SET `Note` = :note WHERE `Nom` = :Nom");
        query.bindValue(":note", newNote);
        query.bindValue(":Nom", Nom);

        if (query.exec()) {
            qDebug() << "Nouvelle note pour" << Nom << "qui passe à:" << newNote;
        } else {
            qDebug() << "Erreur lors de la mise à jour de la note:" << query.lastError().text();
        }

        db.close();
    } else {
        qDebug() << "Échec de la connexion pour la mise à jour de la note.";
    }
}

Widget::~Widget()
{
    delete ui;
}

