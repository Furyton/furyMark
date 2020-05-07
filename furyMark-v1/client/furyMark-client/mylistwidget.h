#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QWidget>
#include <QObject>

namespace Ui {
class MyListWidget;
}

class MyListWidget : public QWidget

{
    Q_OBJECT

public:
    explicit MyListWidget(QWidget *parent = nullptr);
    ~MyListWidget();
private slots:

    void on_deleteButton_clicked();

    void on_openButton_clicked();

    void on_pdfButton_clicked();

private:
    Ui::MyListWidget *ui;

    void getList();
signals:
    void openSignal(int, const QString&, const QString&);
};

#endif // MYLISTWIDGET_H
