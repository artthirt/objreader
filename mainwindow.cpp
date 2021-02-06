#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>

#include "wobjrender.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Render Obj Object");

    QVBoxLayout *vl = new QVBoxLayout(centralWidget());

    mRender = new WObjRender(this);

    vl->addWidget(mRender);

    centralWidget()->setLayout(vl);
}

MainWindow::~MainWindow()
{
    delete ui;
}

