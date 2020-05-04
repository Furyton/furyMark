#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <document.h>
#include <QSplitter>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QTextBrowser;
class QWebEngineView;

//namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
//    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;

private slots:
    void createFile();
    void open();
    bool save();
    bool saveAs();
    void documentModified();
    void switchPreview();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
//    Ui::MainWindow *ui;
    void readSettings();
    void writeSettings();
    void buildWidgetActions();
    void createStatusBar();
    bool saveDialog();
    void loadLoacalFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurFileTitle(const QString &fileName);
    QString getFileName(const QString &fullFileName);

    QPlainTextEdit *editor;
//    QTextBrowser *preview;
    QWebEngineView *preview;
    QSplitter *splitter;
    QString curFile; //current file name
    document content;
    int timeStamp;
};
#endif // MAINWINDOW_H
