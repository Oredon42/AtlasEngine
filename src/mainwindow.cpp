#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QThread>

#include "include/mainwindow.h"

MainWindow::MainWindow(const std::string &path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AtlasEngine)
{
    move(0, 0);
    ui->setupUi(this);
    ui->openGLWidget->setPath(path);
}

MainWindow::~MainWindow()
{
    delete ui;
}
