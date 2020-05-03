#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWebEnginePage>

class previewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit previewPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // PREVIEWPAGE_H
