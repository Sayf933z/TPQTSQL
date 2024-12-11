#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>  // Inclure pour utiliser QTableWidget

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    // Déclaration du QTableWidget pour afficher les résultats
    QTableWidget *tableWidget;
private slots:
    void updateNoteInDatabase(QTableWidgetItem* item);

};

#endif // WIDGET_H
