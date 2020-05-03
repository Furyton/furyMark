#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QDebug>
#include <QWebEngineView>
#include <previewpage.h>
#include <QWebChannel>
#include <document.h>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
    , editor(new QPlainTextEdit)
    , preview(new QWebEngineView)
{
//    ui->setupUi(this);
    readSettings();

    splitter = new QSplitter(this);
    setCentralWidget(splitter);

    splitter->addWidget(editor);

    previewPage *page = new previewPage(this);
    preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &content);
    page->setWebChannel(channel);

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

//MainWindow::~MainWindow()
//{
//    delete ui;
//}

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
//    QFileDialog dialog(this);
//    dialog.setWindowModality(Qt::WindowModal);
//    dialog.setAcceptMode(QFileDialog::AcceptSave);
//    if(dialog.exec() != QDialog::Accepted) {
//        return false;
//    }
//    return saveFile(dialog.selectedFiles().first());

    QString path = QFileDialog::getSaveFileName(this,
        tr("Save MarkDown File"), "", tr("MarkDown(*.md, *.markdown)"));
    if (path.isEmpty())
        return false;
    return saveFile(path);
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
#endif

    //show preview
    QAction *showPreview = new QAction(tr("Pre&view"), this);
    QKeySequence *s = new QKeySequence(tr("Ctrl+B"));
    showPreview->setShortcut(*s);
    showPreview->setStatusTip(tr("Show the preview page"));
    connect(showPreview, &QAction::triggered, this, &MainWindow::switchPreview);
    editMenu->addAction(showPreview);
    editToolBar->addAction(showPreview);

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(editor, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(editor, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif


    //modified signal
    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentModified);


#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

}

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

/*
 * saveFile(QString)
 *
 * save file named filename to the local return if it succeeds
 * it shows a dialog indicate the errormessage if failed
*/

bool MainWindow::saveFile(const QString &fileName)
{
    QString errorMessage;

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
