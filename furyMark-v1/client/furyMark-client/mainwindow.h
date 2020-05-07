#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <document.h>
#include <QSplitter>
//#include <QListWidget>

#include <mylistwidget.h>
#include <generaldata.h>
#include <picturehoster.h>

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
    ~MainWindow();
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
    void popUpList();
    void onBold();
    void onItalic();
    void insertPic();
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
//    void setCurFileTitle(const QString &fileName, int stamp);
    QString getFileName(const QString &fullFileName);
    void loadCloudFile(int stamp, const QString &fileName, const QString &content);
    void loadPicture();

    QPlainTextEdit *editor;
//    QTextBrowser *preview;
//    QListWidget *list;
    MyListWidget *list;
    PictureHoster *hoster;

    QWebEngineView *preview;
    QSplitter *splitter;
    QString curFile; //current file name
    document content;
    int timeStamp;
};
#endif // MAINWINDOW_H
