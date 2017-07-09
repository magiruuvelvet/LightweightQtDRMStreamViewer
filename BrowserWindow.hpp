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

class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow();

    void setWindowTitle(const QString &title);
    void setBaseTitle(const QString &title);
    void setUrl(const QUrl &url);
    void setCookieStoreId(const QString &id);

    void reset();

public slots:
    void toggleFullScreen();
    void hideScrollBars();

private slots:
    void acceptFullScreen(QWebEngineFullScreenRequest);

private:
    QVBoxLayout *m_layout;

    QString m_baseTitle;
    QString m_cookieStoreId;

    QString m_engineProfilePath;

    QWebEngineView *webView;
    QWebEngineCookieStore *m_cookieStore;
    QVector<QNetworkCookie> m_cookies;
};

#endif // BROWSERWINDOW_HPP
