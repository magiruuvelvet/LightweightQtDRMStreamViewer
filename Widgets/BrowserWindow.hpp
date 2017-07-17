#ifndef BROWSERWINDOW_HPP
#define BROWSERWINDOW_HPP

#include <QtWebEngine>
#include <QtWebEngineWidgets>
#include <QWebEngineCookieStore>
#include <QNetworkCookie>

#include <QWidget>
#include <QLayout>
#include <QString>
#include <QVector>
#include <QList>
#include <QUrl>

#include <QShortcut>

#include "Gui/TitleBar.hpp"

class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BrowserWindow(bool titleBarVisible = false, QWidget *parent = nullptr);
    ~BrowserWindow();

    void showNormal();
    void showFullScreen();

    void setWindowTitle(const QString &title);
    void setWindowIcon(const QIcon &icon);
    void setTitleBarVisibility(bool);
    void setTitleBarColor(const QColor &color, const QColor &textColor);
    void setBaseTitle(const QString &title);
    void setUrl(const QUrl &url);
    void setCookieStoreId(const QString &id);

    void reset();

protected:
//    bool eventFilter(QObject *obj, QEvent *e);
//    void enterEvent(QEvent*);
//    void mouseMoveEvent(QMouseEvent*);
//    void leaveEvent(QEvent*);

public slots:
    void toggleFullScreen();
    void hideScrollBars();

private slots:
    void acceptFullScreen(QWebEngineFullScreenRequest);
    void toggleCursorVisibility();
    void showCursor();
    void hideCursor();

private:
    TitleBar *m_titleBar = nullptr;
    void createTitleBar(bool visible = false);
    QVBoxLayout *m_layout;

    QString m_baseTitle;
    QString m_cookieStoreId;

    QString m_engineProfilePath;

    QWebEngineView *webView;
    QWebEngineCookieStore *m_cookieStore;
    QVector<QNetworkCookie> m_cookies;

    bool isMouseInsideWidget = false;
};

#endif // BROWSERWINDOW_HPP
