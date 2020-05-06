/********************************************************************************
** Form generated from reading UI file 'mylistwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYLISTWIDGET_H
#define UI_MYLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyListWidget
{
public:
    QPushButton *deleteButton;
    QPushButton *openButton;
    QPushButton *pdfButton;
    QListWidget *listWidget;

    void setupUi(QWidget *MyListWidget)
    {
        if (MyListWidget->objectName().isEmpty())
            MyListWidget->setObjectName(QString::fromUtf8("MyListWidget"));
        MyListWidget->resize(371, 430);
        deleteButton = new QPushButton(MyListWidget);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
        deleteButton->setGeometry(QRect(270, 10, 89, 25));
        openButton = new QPushButton(MyListWidget);
        openButton->setObjectName(QString::fromUtf8("openButton"));
        openButton->setGeometry(QRect(270, 50, 89, 25));
        pdfButton = new QPushButton(MyListWidget);
        pdfButton->setObjectName(QString::fromUtf8("pdfButton"));
        pdfButton->setGeometry(QRect(270, 90, 89, 25));
        listWidget = new QListWidget(MyListWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 10, 241, 401));

        retranslateUi(MyListWidget);

        QMetaObject::connectSlotsByName(MyListWidget);
    } // setupUi

    void retranslateUi(QWidget *MyListWidget)
    {
        MyListWidget->setWindowTitle(QCoreApplication::translate("MyListWidget", "Form", nullptr));
        deleteButton->setText(QCoreApplication::translate("MyListWidget", "Delete", nullptr));
        openButton->setText(QCoreApplication::translate("MyListWidget", "Open", nullptr));
        pdfButton->setText(QCoreApplication::translate("MyListWidget", "Get PDF", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyListWidget: public Ui_MyListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYLISTWIDGET_H
