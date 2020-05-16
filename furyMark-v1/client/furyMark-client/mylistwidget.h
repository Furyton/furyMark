#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <generaldata.h>

namespace Ui {
class MyListWidget;
}

class MyListWidget : public QWidget

{
    Q_OBJECT

public:
    explicit MyListWidget(QWidget *parent = nullptr);
    ~MyListWidget();
public:
    void init();
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:

    void on_deleteButton_clicked();

    void on_openButton_clicked();

    void on_pdfButton_clicked();

    void getListRequest();

    void respondDealer();

    void onList(const generalData&);
    void onOpen(const generalData&);
    void onPdf(const generalData&);
//    void onDelete(const generalData&);
private:
    Ui::MyListWidget *ui;
    QTcpSocket *socket;
    QList<int> listContent;
public:
    bool locked;
signals:
    void openSignal(int, const QString&, const QString&);
};

#endif // MYLISTWIDGET_H
