#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QDebug>
#include <QWebEngineView>
#include <previewpage.h>
#include <QWebChannel>
#include <document.h>
#include <QKeySequence>
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
    , editor(new QPlainTextEdit)
    , preview(new QWebEngineView)
{
//    ui->setupUi(this);
    initRecentFileActions();
    loadRecentFileActions();
    dragMode = false;

    list = new MyListWidget();
    hoster = new PictureHoster();
    qtimer = new QTimer(this);

    readSettings();

    editor->setAcceptDrops(false);
    preview->setAcceptDrops(false);
    this->setAcceptDrops(true);

    splitter = new QSplitter(this);
    setCentralWidget(splitter);

    splitter->addWidget(editor);

    previewPage *page = new previewPage(this);
    preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &content);
    channel->registerObject(QStringLiteral("main"), this);
    page->setWebChannel(channel);

    QFile file(":/3rdparty/markdown.css");
    file.open(QIODevice::ReadOnly | QFile::Text);
    QTextStream in(&file);
//    qDebug() << in.readAll();
    css = in.readAll();

    preview->setUrl(QUrl("qrc:/index.html"));

    splitter->addWidget(preview);

//    splitter->
    splitter->setHandleWidth(10);
    splitter->setStretchFactor(0, 7);
    splitter->setStretchFactor(1, 3);

//    preview->hide();
//    preview->setParent(this);

    buildWidgetActions();
    createStatusBar();

//    preview->hide();

    setCurFileTitle(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
    delete list;
    delete hoster;
}

/*
 * closeEvent
 *
 * prevent user from closing window if the file hasn't been saved.
*/

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(saveDialog()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/*
 * createFile()
 *
 * create a new file in the workspace(not in local)
 *
*/

void MainWindow::createFile()
{
    if(saveDialog()) {
        timeStamp = -1;
        editor->clear();
        setCurFileTitle(QString());
    }
}

/*
 * open()
 *
 * open a new file by selecting from a fileDialog
*/

void MainWindow::open()
{
    if(saveDialog()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                         tr("Open MarkDown File"), "", tr("MarkDown(*.md)"));
//        qDebug() << fileName;
        if(!fileName.isEmpty()) {
            loadLoacalFile(fileName);
        }
    }
}

/*
 * save()
 *
 * save cur file, and save as... if this is a newly created file.
*/

bool MainWindow::save()
{
    if(curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

/*
 * saveAs()
 *
 * save this file as another new file.
*/

bool MainWindow::saveAs()
{
    QFileDialog dialog(this, tr("Save Markdown File"));
    dialog.setNameFilter("*.md");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(dialog.exec() != QDialog::Accepted) {
        return false;
    }
    return saveFile(dialog.selectedFiles().first());

//    QString path = QFileDialog::getSaveFileName(this,
//        tr("Save MarkDown File"), "", tr("MarkDown(*.md)"));
//    if (path.isEmpty())
//        return false;
//    return saveFile(path);
}

/*
 * a slot response to modifying signal.
 * add '*' after window title if modified.
*/

void MainWindow::documentModified()
{
    setWindowModified(editor->document()->isModified());
//    qDebug() << editor->document()->toMarkdown();
//    preview->setMarkdown(editor->document()->toMarkdown());
//    content = editor->document()->toPlainText();
    content.setText(editor->document()->toPlainText());
}

/*
 * create all the actions and buttons here
 * it maybe a little messy
*/


void MainWindow::buildWidgetActions()
{

    //create file operator
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QToolBar *fileToolBar = addToolBar(tr("File"));
    QAction *newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file..."));
    connect(newAct, &QAction::triggered, this, &MainWindow::createFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    //open file operator
    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    QMenu *recentFiles = new QMenu(tr("Recent Files"));
    for (int i = 0; i < MaxRecentFiles; ++i)
        recentFiles ->addAction(recentFileActs[i]);

    fileMenu->addMenu(recentFiles);


    fileMenu->addSeparator();

    //save file operator
    QAction *saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);


    //save as operator
    QAction *saveAsAct = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    //exit operator
    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    //edit
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD

    //cut
    QAction *cutAct = new QAction(tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, editor, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    //copy
    QAction *copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, editor, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);


    //paste
    QAction *pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, editor, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);


    //bold
    QAction *boldAct = new QAction(tr("Bold"), this);
    QKeySequence *boldKeyShortCut = new QKeySequence(tr("Ctrl+B"));
    boldAct->setShortcut(*boldKeyShortCut);
    boldAct->setStatusTip(tr("Emphasize the selected text."));
    connect(boldAct, &QAction::triggered, this, &MainWindow::onBold);
    editMenu->addAction(boldAct);
    editToolBar->addAction(boldAct);

    //italic
    QAction *italicAct = new QAction(tr("Italic"), this);
    QKeySequence *italicKeyShortCut = new QKeySequence(tr("Ctrl+I"));
    italicAct->setShortcut(*italicKeyShortCut);
    italicAct->setStatusTip(tr("Make the selected text italic style."));
    connect(italicAct, &QAction::triggered, this, &MainWindow::onItalic);
    editMenu->addAction(italicAct);
    editToolBar->addAction(italicAct);

    //Picture
    QAction *picAct = new QAction(tr("Picture"), this);
    QKeySequence *picShortCut = new QKeySequence(tr("Ctrl+Shift+P"));
    picAct->setShortcut(*picShortCut);
    picAct->setStatusTip(tr("Add a pic to your md file, you can choose to post the img online or not"));
    connect(picAct, &QAction::triggered, this, &MainWindow::insertPic);
    editMenu->addAction(picAct);
    editToolBar->addAction(picAct);

    QAction *insert = new QAction(tr("Fomula"), this);
    QKeySequence *insertShortCut = new QKeySequence(tr("Ctrl+Shift+F"));
    insert->setShortcut(*insertShortCut);
    insert->setStatusTip(tr("using ZhiHu platform to get the pic of your fomula, and add it into your file"));
    connect(insert, &QAction::triggered, this, &MainWindow::onInsert);
    editMenu->addAction(insert);
    editToolBar->addAction(insert);

    QAction *html = new QAction(tr("html"), this);
    connect(html, &QAction::triggered, this, [=](){
        QFileDialog dialog(this, tr("Save Html File"));
        dialog.setNameFilter("*.html");
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        if(dialog.exec() != QDialog::Accepted) {
            return;
        }
        saveFile(dialog.selectedFiles().first());
        QSaveFile file(dialog.selectedFiles().first());
        if(file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            QString HTML = "<!doctype html><html><meta charset=\"utf-8\"><head><title>" + curFile + "</title></head>\
                    <body>" + myHtml + "</body></html>";
            out << HTML;
            file.commit();
        }
    });
    editToolBar->addAction(html);

//    editToolBar->setVisible();

#endif

    QMenu *view = menuBar()->addMenu(tr("&View"));
    QMenu *cloud = menuBar()->addMenu(tr("Cloud"));

    //show preview
    QAction *showPreview = new QAction(tr("Pre&view"), this);
    QKeySequence *s = new QKeySequence(tr("Ctrl+P"));
    showPreview->setShortcut(*s);
    showPreview->setStatusTip(tr("Show the preview page"));
    connect(showPreview, &QAction::triggered, this, &MainWindow::switchPreview);
    view->addAction(showPreview);
//    editToolBar->addAction(showPreview);

    QAction *cloudFileopt = new QAction(tr("Clo&udFile"), this);
    QKeySequence *cloudShortCut = new QKeySequence(tr("Ctrl+Shift+C"));
    cloudFileopt->setShortcut(*cloudShortCut);
    cloudFileopt->setStatusTip(tr("Operate the cloud files"));
    connect(cloudFileopt, &QAction::triggered, this, &MainWindow::popUpList);
    cloud->addAction(cloudFileopt);

    QAction *uploadAct = new QAction(tr("Upl&oad"), this);
    QKeySequence *uploadKey = new QKeySequence(tr("Ctrl+Shift+U"));
    uploadAct->setShortcut(*uploadKey);
    uploadAct->setStatusTip(tr("Upload this file to the server"));
    connect(uploadAct, &QAction::triggered, this, &MainWindow::upload);
    cloud->addAction(uploadAct);

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(editor, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(editor, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif



    QMenu* options = menuBar()->addMenu(tr("settings"));
    QAction *timer = new QAction(tr("auto save in 10s"), this);
    timer->setStatusTip(tr("check this option if you want your file be saved every 10s"));
    timer->setCheckable(true);
    connect(timer, &QAction::triggered, this, [=](){
        qDebug() << timer->isChecked();
        if(timer->isChecked()){
            qtimer->start(10000);
        } else {
            qtimer->stop();
        }
    });
    connect(qtimer, &QTimer::timeout, this, [=](){
        if(curFile.isEmpty()) return;
        if(!editor -> document() -> isModified()) return;
        if(!save()) {
            qtimer->stop();
            timer->setChecked(false);
        }
     });

    options->addAction(timer);

    QAction *dragSetting = new QAction(tr("set drag"), this);
    dragSetting->setCheckable(true);
    dragSetting->setStatusTip(tr("check this option so that when you drag a pic into this window, it will excute pic post mode instead of using the local url"));
    connect(dragSetting, &QAction::triggered, this, [=](){
        qDebug() << dragSetting->isChecked();
        if(dragSetting->isChecked()) {
            dragMode = true;
        } else {
            dragMode = false;
        }
    });

    options->addAction(dragSetting);

    //modified signal
    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentModified);


#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    connect(list, &MyListWidget::openSignal, this, &MainWindow::loadCloudFile);
    connect(hoster, &PictureHoster::urlSignal, [=](const QString & url){
       editor->appendPlainText(url);
    });
}
void MainWindow::onInsert()
{
    bool ok;
    QString fomula = QInputDialog::getText(this, "Typing Fomula", "fomula", QLineEdit::Normal, QString("Type here"), &ok);
//    QUrl url("https://www.zhihu.com/equation?tex=" + fomula);
    if(ok)
        editor->appendPlainText("![fomula](https://www.zhihu.com/equation?tex=" + QUrl::toPercentEncoding(fomula) + ") ");
}
void MainWindow::insertPic()
{
    const QMessageBox::StandardButton ret
        = QMessageBox::information(this, tr("FuryMark"),
                               tr("Your are inserting a picture.\n"
                                  "Do you want to add it via picture post which we recommend?\n"
                                  "It will take some time.\n"
                                  "notice: picture larger than 5MB is not allowed."),
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    switch (ret) {
        case QMessageBox::Yes:
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
            return hoster->sendPost();
        case QMessageBox::No:
            loadPicture();
            return;
        default:
            break;
    }

}
void MainWindow::loadPicture()
{
    QString strFilePath = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("Select a picture"), "./",
                                                       tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm *.jpeg);;All files (*.*)"));
    editor->appendPlainText("![" + getFileName(strFilePath) + "](file:" + strFilePath + ")");
}

/*
 * Action responsed to preview button
 * close or show the preview page
*/

void MainWindow::switchPreview()
{
    static bool visible = true;
    if(visible) {
        preview->setParent(this);
        visible = false;
    } else {
        splitter->addWidget(preview);
        visible = true;
    }
}

void MainWindow::onBold()
{
    QTextCursor currentTextCursor = editor->textCursor();

    if(!currentTextCursor.hasSelection()) {
        currentTextCursor.insertText("**" + tr("Type here") + "**");
        currentTextCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
        currentTextCursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 2);
        editor->setTextCursor(currentTextCursor);
    } else {
        QString text = currentTextCursor.selectedText();
        if(text.length() >= 4 && text.startsWith("**") && text.endsWith("**")) {
            text.remove(0, 2); text.remove(text.length() - 2, 2);
            currentTextCursor.insertText(text);
        } else {
            currentTextCursor.insertText("**" + currentTextCursor.selectedText() + "**");
        }
    }
}

void MainWindow::onItalic()
{
    QTextCursor currentTextCursor = editor->textCursor();

    if(!currentTextCursor.hasSelection()) {
        currentTextCursor.insertText(" *" + tr("Type here") + "* ");
        currentTextCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
        currentTextCursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 2);
        editor->setTextCursor(currentTextCursor);
    } else {
        QString text = currentTextCursor.selectedText();
        if(text.length() >= 4 && text.startsWith(" *") && text.endsWith("* ")) {
            text.remove(0, 2); text.remove(text.length() - 2, 2);
            currentTextCursor.insertText(text);
        } else {
            currentTextCursor.insertText(" *" + currentTextCursor.selectedText() + "* ");
        }
    }
}

void MainWindow::upload()
{
    if(!save()) return;

    qDebug() << "uploading...";
    socket = new QTcpSocket(this);

    QString ip = "127.0.0.1";
    int port = 8806;
    socket->connectToHost(ip, port);

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    if(!socket->waitForConnected(10000)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Server", "connecting to server failed.");
//        this->close();
        return;
    }
//    QGuiApplication::restoreOverrideCursor();

    generalData data;

    data.type = 0;
    data.timeStamp = timeStamp;
    data.fileName = getFileName(curFile);
    data.content = editor->toPlainText();

    qDebug() << data.fileName <<' ' <<data.timeStamp;

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

    int sendRespond = socket->write(buffer);

    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
        return;
    }

//    QString sendData = "Hello world from upload function";
//    int sendRespond = socket->write(sendData.toUtf8());
//    if(sendRespond == -1) {
//        QGuiApplication::restoreOverrideCursor();
//        QMessageBox::information(this, "server", "sending data error");
//        return;
//    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(responseDealer()));
}

void MainWindow::responseDealer()
{
    QGuiApplication::restoreOverrideCursor();

    QByteArray buffer;
    buffer = socket->readAll();

    if(buffer.length() > 0) {
        generalData rec;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        stream >> rec;

        if(rec.type == 8) {
            timeStamp = rec.timeStamp;
            qDebug() << "received id " << timeStamp;
            statusBar() -> showMessage(QString("Uploaded, get file id:%1").arg(timeStamp), 2000);
        } else if(rec.type == 9) {
            QMessageBox::information(this, "server", "something wrong with server :(");
        }
    } else {
        QMessageBox::information(this, "server", "receive data error.");
    }
    socket->disconnectFromHost();
}
/*
 * save mainWindow's size settings while closing
 * and read Settings at the beginning
*/

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}
void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", splitter->saveState());
}



void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

/*
 * saveDialog
 *
 * when user is going to close cur file but not save it,
 * this dialog will ask user what to do.
 *
*/

bool MainWindow::saveDialog()
{
    if(!editor -> document() -> isModified()) return true;

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("FuryMark"),
                               tr("This document isn't saved yet\n"
                                  "Do you want to save it?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }

    return true;
}

/*
 * loadLocalFile(QString)
 *
 * open the file named fileName and load all the content to the editor.
*/

void MainWindow::loadLoacalFile(const QString &fileName)
{
    timeStamp = -1;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    editor -> setPlainText(in.readAll());

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurFileTitle(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::loadCloudFile(int stamp, const QString &fileName, const QString &content)
{
    if(saveDialog()) {
        timeStamp = stamp;
        editor->clear();

        setCurFileTitle(QString());
        setWindowFilePath(fileName);
        editor->appendPlainText(content);
//        qDebug() << editor -> document() -> isModified();
    }
}

/*
 * saveFile(QString)
 *
 * save file named filename to the local return if it succeeds
 * it shows a dialog indicate the errormessage if failed
*/

bool MainWindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    qDebug() << fileName;
    if(QFileInfo(fileName).suffix().toLower() != "md") return false;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QSaveFile file(fileName);
    if(file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << editor -> toPlainText();

        if(!file.commit()) {
            errorMessage = tr("Cannot write file %1: \n %2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 :\n %2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }

    QGuiApplication::restoreOverrideCursor();

    if(!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("FuryMark"), errorMessage);
        return false;
    }

    setCurFileTitle(fileName);

    statusBar() -> showMessage(tr("File saved"), 2000);
    return true;
}

/*
 * setCurFileTitle(const QString)
 * set file's name as window's title
 * and "untitled.md" if the file is newly created.
*/
void MainWindow::setCurFileTitle(const QString &fileName) {
    curFile = fileName;

    editor->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if(curFile.isEmpty()) {
        shownName = "untitled.md";
    } else {
        QSettings settings;
        QStringList files = settings.value("recentFileList").toStringList();
        files.removeAll(fileName);
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();

        settings.setValue("recentFileList", files);

        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
            if (mainWin)
                mainWin->loadRecentFileActions();
        }
    }
//    qDebug() << fileName;
//    qDebug() << shownName;
    setWindowFilePath(shownName);
}

/*
 * getFileName(const QString)
 *
 * get file's name.
*/

QString MainWindow::getFileName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/*
 * commitData(QSessiongManager)
 *
 * work when user is closing the app
 *
 * prevent user from closing the window if the file
 * hasn't been saved.
*/

void MainWindow::commitData(QSessionManager &manager)
{
    if(manager.allowsInteraction()) {
        if(!saveDialog()) {
            manager.cancel();
        }
    } else {
        if(editor->document()->isModified()) {
            save();
        }
    }
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}
void MainWindow::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    static int imgCount = 0;

    foreach(QUrl url, urls) {
        QString file_name = url.toLocalFile();
//        editor->appendPlainText(QFileInfo(file_name).suffix());
        QString format = QFileInfo(file_name).suffix().toLower();
        if(format == "jpeg" || format == "png" || format == "jpg") {

            imgCount++;
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
            if(dragMode){

                this->hoster->sendPost(file_name);
            } else {
                QGuiApplication::restoreOverrideCursor();
                editor->appendPlainText("![" + QFileInfo(file_name).fileName() + ":index " + QString("%1").arg(imgCount) + "](file:" + file_name + ")");
            }
//            editor->appendPlainText("![" + QFileInfo(file_name).fileName() + ":index " + QString("%1").arg(imgCount) + "](file:" + file_name + ")");
        } else if(format == "md") {
            if(saveDialog())
                loadLoacalFile(file_name);
        }

//        QFileInfo(file_name).suffix()
    }
}
void MainWindow::popUpList()
{
    list->setWindowModality(Qt::ApplicationModal);
    const QRect availableGeometry = screen()->availableGeometry();
//    list->setGeometry()
//    resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
    list->move((availableGeometry.width() - list->width()) / 2,
         (availableGeometry.height() - list->height()) / 2);

    list->show();
    list->init();
}
void MainWindow::initRecentFileActions()
{
    for(int i = 0; i < MaxRecentFiles; i++) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
}
void MainWindow::loadRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = files[i];
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        if(saveDialog())
            loadLoacalFile(action->data().toString());
    }
}
