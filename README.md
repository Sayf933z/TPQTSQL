# Gestion de Joueurs avec Qt et MySQL

Ce projet est une application Qt permettant d'interagir avec une base de données MySQL sous Linux. Il fournit une interface graphique pour consulter, modifier et synchroniser des données liées à des joueurs de football.

---

## Fonctionnalités

### 1. **Connexion à une Base de Données**
- La base de données MySQL `jeu` est utilisée pour stocker des informations sur les joueurs :
  - **Colonnes** : `Nom`, `Club`, `Note`
  - La connexion est configurée dans le code via `QSqlDatabase`.

### 2. **Affichage et Modification**
- Une interface graphique avec un tableau (`QTableWidget`) permet de :
  - Afficher les joueurs, leurs clubs et leurs notes.
  - Modifier les notes directement dans le tableau.

### 3. **Synchronisation avec la Base**
- Les modifications des notes dans l'interface sont automatiquement sauvegardées dans la base via des requêtes SQL.

---

## Prérequis

### Outils nécessaires
- **Linux**
- **Qt Creator** (ou un environnement de développement supportant Qt)
- **MySQL** installé et configuré.

### Installation de Qt
Pour installer Qt sous Linux :
```bash
sudo apt update
sudo apt install qtcreator qt5-default libqt5sql5-mysql
```
### Création de la base de données
-  Connectez vous à MySQL :
  ```bash
  msqyl -u root -p
  ```
  -  Créez la base jeu et la table :
   ```sql
CREATE DATABASE jeu;
USE jeu;
CREATE TABLE jeu (
    id INT PRIMARY KEY AUTO_INCREMENT,
    Nom VARCHAR(255),
    Club VARCHAR(255),
    Note INT
);
``` 

 - Ajoutez des données initiales :
      ```sql
      INSERT INTO jeu (Nom, Club, Note) VALUES
      ('Ronaldo', 'Madrid', 90),
      ('Grizou', 'Barca', 85);
       ```
## Structure du Code

### Connexion à la Base de Données

Dans `main.cpp`, la connexion est initialisée avec `QSqlDatabase` :

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
db.setDatabaseName("jeu");
db.setUserName("sayf");
db.setPassword("sayfamine");
if (db.open()) {
    qDebug() << "Vous êtes maintenant connecté à " << db.hostName();

    QSqlQuery query;
    query.exec("SELECT * FROM `jeu`");
    while (query.next()) {
        QString id = query.value(0).toString();
        QString Nom = query.value(1).toString();
        QString club = query.value(2).toString();
        qDebug() << Nom << club;
    }
    db.close();
} else {
    qDebug() << "La connexion a échoué :" << db.lastError().text();
}
  ```
La méthode `addDatabase("QMYSQL")` configure le pilote pour une base de données MySQL. Les identifiants de connexion (`DatabaseName`, `UserName`, `Password`) sont définis pour établir la connexion.

Voici un aperçu de la requête SQL exécutée pour récupérer toutes les données de la table `jeu` :

```cpp
if (db.open()) {
    qDebug() << "Vous êtes maintenant connecté à " << db.hostName();

    QSqlQuery query;
    query.exec("SELECT * FROM `jeu`");
    while (query.next()) {
        QString id = query.value(0).toString();
        QString Nom = query.value(1).toString();
        QString club = query.value(2).toString();
        qDebug() << Nom << club;
    }


    db.close();
} else {
    qDebug() << "La connexion a échoué :" << db.lastError().text();
}
```
### Affichage Graphique des Données

Dans `widget.cpp`, un tableau (`QTableWidget`) est configuré pour afficher les données récupérées depuis la base de données MySQL :

```cpp
tableWidget = new QTableWidget(this);
tableWidget->setRowCount(0);
tableWidget->setColumnCount(3);
tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Club" << "Note");
```

- **`setRowCount(0)`** : Initialise le tableau sans lignes.
- **`setColumnCount(3)`** : Définit trois colonnes : `Nom`, `Club`, et `Note`.
- **`setHorizontalHeaderLabels`** : Ajoute des en-têtes de colonnes, à savoir : `Nom`, `Club`, et `Note`.

Les données récupérées de la base de données sont ensuite insérées ligne par ligne dans le tableau graphique :

```cpp
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
```
- **`insertRow(row)`** : Ajoute une nouvelle ligne dans le tableau pour chaque joueur.
- **`setItem(row, column, new QTableWidgetItem(...))`** : Insère les données de chaque joueur (nom, club, note) dans les cellules appropriées du tableau.

Ainsi, à chaque itération, le tableau est rempli avec les données des joueurs récupérées depuis la base de données MySQL.

---

### Mise à Jour des Données

Les données du tableau peuvent être modifiées directement par l'utilisateur. Lorsqu'une cellule est modifiée, le signal `itemChanged` est déclenché, et une méthode est appelée pour mettre à jour la base de données avec la nouvelle valeur.

Voici le code pour connecter le signal `itemChanged` à la fonction qui met à jour la base de données :

```cpp
connect(tableWidget, &QTableWidget::itemChanged, this, &Widget::updateNoteInDatabase);
```
### Mise à Jour des Données (suite)

La fonction `updateNoteInDatabase` est ensuite définie pour traiter les modifications apportées à la table, en particulier les notes des joueurs :

```cpp
void Widget::updateNoteInDatabase(QTableWidgetItem* item) {
    // Si la colonne modifiée n'est pas la colonne des notes, on ne fait rien.
    if (item->column() != 2) return;

    int row = item->row();
    bool ok;
    int newNote = item->text().toInt(&ok);

    if (!ok) {
        qDebug() << "La note n'est pas valide";
        return;
    }

    QString Nom = tableWidget->item(row, 0)->text();

    // Connexion à la base de données
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
            qDebug() << "Note mise à jour pour" << Nom << ": " << newNote;
        } else {
            qDebug() << "Erreur lors de la mise à jour :" << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "Erreur de connexion à la base de données :" << db.lastError().text();
    }
}
```
### Explication du Code

1. **Vérification de la colonne modifiée**  
   La fonction `updateNoteInDatabase` commence par vérifier si la colonne modifiée est bien celle des notes (colonne avec l'index 2). Si la colonne modifiée n'est pas celle des notes, la fonction ne fait rien et retourne immédiatement. Cela permet d'éviter des modifications inutiles dans d'autres colonnes, comme le nom ou le club des joueurs.

   ```cpp
   if (item->column() != 2) return;
   
    ```
2. **Conversion de la valeur modifiée**  
   La valeur modifiée dans la cellule est récupérée sous forme de texte. Elle est ensuite convertie en entier en utilisant la méthode `toInt()` de Qt. La conversion retourne un booléen (`ok`) qui indique si la conversion a réussi. Si la conversion échoue (par exemple si l'utilisateur entre un texte non numérique), un message d'erreur est affiché et la fonction retourne sans effectuer d'autres actions.

   ```cpp
   bool ok;
   int newNote = item->text().toInt(&ok);
   if (!ok) {
       qDebug() << "La note n'est pas valide";
       return;
   }
   ```
3. **Récupération du nom du joueur**  
   Le nom du joueur correspondant à la ligne modifiée est récupéré à partir de la première colonne (Nom) de la table graphique. Cette valeur est utilisée dans la requête SQL pour identifier de manière unique l'enregistrement à mettre à jour.

   ```cpp
   QString Nom = tableWidget->item(row, 0)->text();
   ```
   - item(row, 0) : Accède à la cellule dans la colonne 0 (le nom du joueur) pour la ligne modifiée.
   - Cette étape est essentielle pour relier la modification graphique au bon enregistrement dans la base de données.
     
4. **Connexion à la base de données**
   Une nouvelle connexion à la base de données est établie pour effectuer la mise à jour. Les paramètres de connexion, comme le nom de l'utilisateur et le mot de passe, sont configurés de la même manière que lors de l'affichage initial.

Si la connexion échoue, un message d'erreur est affiché et la fonction retourne immédiatement.

  ```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
db.setDatabaseName("jeu");
db.setUserName("sayf");
db.setPassword("sayfamine");

if (!db.open()) {
    qDebug() << "Erreur de connexion à la base de données :" << db.lastError().text();
    return;
}
  ```
- addDatabase("QMYSQL") : Spécifie l'utilisation du pilote MySQL pour Qt.
- db.open() : Tente d'établir une connexion à la base.
- Si la connexion échoue, un message indiquant l'erreur rencontrée (db.lastError().text()) est affiché dans la console.

5. **Mise  à jour dans la base de données**
   Une requête préparée est utilisée pour sécuriser et exécuter la mise à jour. Les valeurs sont liées à des paramètres nommés (:note et :Nom) pour éviter toute injection SQL.

     ```cpp
     QSqlQuery query;
    query.prepare("UPDATE `jeu` SET `Note` = :note WHERE `Nom` = :Nom");
    query.bindValue(":note", newNote);
    query.bindValue(":Nom", Nom);

    if (query.exec()) {
    qDebug() << "Note mise à jour pour" << Nom << ": " << newNote;
    } else {
    qDebug() << "Erreur lors de la mise à jour :" << query.lastError().text();
    }
      ```
- prepare() : Prépare une requête SQL avec des paramètres nommés pour plus de sécurité.
- bindValue() : Associe les valeurs des variables C++ (comme newNote et Nom) aux paramètres SQL.
- exec() : Exécute la requête préparée. Si elle échoue, un message d'erreur est affiché.
   
6. **Fermeture de la connexion**
   Pour éviter des fuites de ressources ou des problèmes de surcharge, la connexion à la base de données est fermée une fois la mise à jour terminée.

   ```cpp
   db.close();
   ```
   Qt gère automatiquement la libération des ressources associées à la base de données après l'appel de cette méthode.

   **Conclusion**
   La fonction updateNoteInDatabase intègre une série de vérifications et de traitements pour garantir que les modifications apportées à l'interface graphique sont correctement synchronisées avec la base de données MySQL. Cette architecture modulaire et sécurisée permet d'ajouter facilement des fonctionnalités supplémentaires, comme :
   
   - Validation avancée des données (par exemple, vérifier que les notes restent dans une plage valide, comme 0-100).
   - Gestion des erreurs plus conviviale, par exemple, afficher une boîte de dialogue si une mise à jour échoue.
  
     Ce processus assure une interaction fluide entre l'interface utilisateur et la base de données tout en maintenant l'intégrité des données.
     
     Voici une capture vidéo qui montre l'intéraction entre le code et la BDD : [Capture vidéo du 2025-01-07 14-24-19.webm](https://github.com/user-attachments/assets/e8090791-7545-4096-8f82-afcaef0aa626)
