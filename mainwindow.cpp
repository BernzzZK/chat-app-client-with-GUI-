#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Database connection failed");
        return;
    } else {
        QMessageBox::information(this, "Success", "Database connection successful");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
