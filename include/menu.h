#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QFrame>
#include <QStackedWidget>

class Menu : public QDialog
{
    Q_OBJECT
public:
    Menu(QWidget *parent, Qt::WindowFlags f);

private slots:
    void graphicsMenu();
    void exitGraphicsDiscard();
    void exitGraphicsApply();

private:
    void setGraphicsMenu();

    QStackedWidget *m_stacked_widget;
};

#endif // MENU_H
