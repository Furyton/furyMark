#include "mylistwidget.h"
#include "ui_mylistwidget.h"
#include <QDebug>
#include <QtWidgets>
#include <QListWidget>
#include <QPushButton>

MyListWidget::MyListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyListWidget)
{
    ui->setupUi(this);
    setWindowTitle(tr("Operate your cloud files"));

//    ui->listWidget->addItem("item 1");
}

MyListWidget::~MyListWidget()
{
    delete ui;
}

void MyListWidget::getList()
{

}

void MyListWidget::on_deleteButton_clicked()
{
    qDebug() << ui->listWidget->currentRow();
}

void MyListWidget::on_openButton_clicked()
{
    qDebug() << ui->listWidget->currentRow();
    emit openSignal(1, "test.md", "# test ");
}

void MyListWidget::on_pdfButton_clicked()
{
    qDebug() << ui->listWidget->currentRow();
}
