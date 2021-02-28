#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QFileDialog>

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

    mTimer.start(200);
    connect(&mTimer, &QTimer::timeout, this, [=](){
        ui->statusbar->showMessage(QString::number(mRender->progress() * 100) + "%");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString fn = QFileDialog::getOpenFileName(nullptr, "Open File *.obj", "", "*.obj");
    if(!fn.isEmpty())
        mRender->loadObjFile(fn);
}
