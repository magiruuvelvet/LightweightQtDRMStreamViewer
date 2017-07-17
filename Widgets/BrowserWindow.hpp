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
    explicit BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow();

    void show();
    void showNormal();
    void showFullScreen();

    void setWindowTitle(const QString &title);
    void setWindowIcon(const QIcon &icon);
    void setTitleBarVisibility(bool visible);
    void setTitleBarColor(const QColor &color, const QColor &textColor);
    void setBaseTitle(const QString &title, bool permanent = false);
    void setUrl(const QUrl &url);
    void setProfile(const QString &id);

    void reset();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
//    void enterEvent(QEvent*);
//    void mouseMoveEvent(QMouseEvent*);
//    void leaveEvent(QEvent*);

public slots:
    void toggleFullScreen();
    void forceReload();
    void onLoadProgress(int);
    void onLoadFinished(bool);

private slots:
    void acceptFullScreen(QWebEngineFullScreenRequest);
    void toggleCursorVisibility();
    void showCursor();
    void hideCursor();

private:
    QVBoxLayout *m_layout;

    void createTitleBar();
    TitleBar *m_titleBar = nullptr;
    bool m_titleBarVisibility = false;

    QString m_baseTitle;
    bool m_permanentTitle = false;
    QString m_cookieStoreId;
    QString m_engineProfilePath;

    QWebEngineView *webView;
    QWebEngineCookieStore *m_cookieStore;
    QVector<QNetworkCookie> m_cookies;

    void loadEmbeddedScript(QString &target, const QString &filename);
    QString mJs_hideScrollBars;
};

#endif // BROWSERWINDOW_HPP
