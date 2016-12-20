#include "menu.h"
#include <QtWidgets>
#include <QApplication>

Menu::Menu(QWidget * parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    connect(this, SIGNAL(rejected()), this->parent(), SLOT(unPause()));

    m_stacked_widget = new QStackedWidget;

    QPushButton *resume_button = new QPushButton(tr("Resume"));
    QPushButton *graphics_button = new QPushButton(tr("Graphics"));
    QPushButton *quit_button = new QPushButton(tr("Quit"));

    connect(resume_button, SIGNAL(clicked()), this->parent(), SLOT(unPause()));
    connect(resume_button, &QAbstractButton::clicked, this, &QWidget::close);
    connect(graphics_button, SIGNAL(clicked()), this, SLOT(graphicsMenu()));
    connect(quit_button, SIGNAL(clicked()), qApp, SLOT(quit()));


    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(resume_button);
    vertical_layout->addWidget(graphics_button);
    vertical_layout->addWidget(quit_button);
    QWidget *menu_widget = new QWidget;
    menu_widget->setLayout(vertical_layout);

    m_stacked_widget->addWidget(menu_widget);

    setGraphicsMenu();

    QHBoxLayout *horizontal_layout = new QHBoxLayout;
    horizontal_layout->addWidget(m_stacked_widget);

    setLayout(horizontal_layout);
}

void Menu::setGraphicsMenu()
{
    QPushButton *apply_button = new QPushButton(tr("Apply"));
    QPushButton *discard_button = new QPushButton(tr("Discard"));

    connect(apply_button, SIGNAL(clicked()), this, SLOT(exitGraphicsApply()));
    connect(discard_button, SIGNAL(clicked()), this, SLOT(exitGraphicsDiscard()));

    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(apply_button);
    vertical_layout->addWidget(discard_button);
    QWidget *graphics_widget = new QWidget;
    graphics_widget->setLayout(vertical_layout);

    m_stacked_widget->insertWidget(1, graphics_widget);
}

void Menu::graphicsMenu()
{
    m_stacked_widget->setCurrentIndex(1);
}

void Menu::exitGraphicsDiscard()
{
    m_stacked_widget->setCurrentIndex(0);
}

void Menu::exitGraphicsApply()
{
    m_stacked_widget->setCurrentIndex(0);
}
