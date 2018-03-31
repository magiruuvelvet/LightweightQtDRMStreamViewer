#ifndef BROWSERWINDOW_HPP
#define BROWSERWINDOW_HPP

#include <QtWebEngine>
#include <QtWebEngineWidgets>
#include <QWebEngineCookieStore>
#include <QNetworkCookie>

#include <QString>
#include <QVector>
#include <QList>
#include <QUrl>

#include <QShortcut>

#include <Gui/BaseWindow.hpp>
#include <Util/UrlRequestInterceptor.hpp>
#include <Core/StreamingProviderStore.hpp>

class BrowserWindow : public BaseWindow
{
    Q_OBJECT
    friend void StreamingProviderStore::loadProfile(BrowserWindow*,const Provider&);

private:
    explicit BrowserWindow(QWidget *parent = nullptr);
public:
    ~BrowserWindow();

    static BrowserWindow *createBrowserWindow(const Provider &profile);

    void show();
    void showNormal();
    void showFullScreen();

    void setWindowTitle(const QString &title);
    void setWindowIcon(const QIcon &icon);
    void setTitleBarVisibility(bool visible);
    void setTitleBarColor(const QColor &color, const QColor &textColor);
    void setBaseTitle(const QString &title, bool permanent = false);
    void setUrl(const QUrl &url);
    void setUrlInterceptorEnabled(bool, const QList<UrlInterceptorLink> &urlInterceptorLinks = QList<UrlInterceptorLink>());
    void setProfile(const QString &id);
    void setScripts(const QList<QString> &scripts);
    void setUserAgent(const QString &ua);

signals:
    void opened();
    void closed();
    void urlChanged(const QUrl &url);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

public slots:
    void toggleFullScreen();
    void forceReload();
    void onLoadProgress(int);
    void onLoadFinished(bool);

private slots:
    void acceptFullScreen(QWebEngineFullScreenRequest);
    void toggleAddressBarVisibility();

private:
    QVBoxLayout *m_layout;
    QLineEdit *emergencyAddressBar;

    QString providerPath;

    QString m_baseTitle;
    bool m_permanentTitle = false;
    QString m_cookieStoreId;
    QString m_engineProfilePath;

    bool m_titleBarVisibility;
    bool m_titleBarVisibilityToggle;

    QWebEngineView *webView;
    QWebEngineCookieStore *m_cookieStore;
    QVector<QNetworkCookie> m_cookies;

    UrlRequestInterceptor *m_interceptor = nullptr;
    bool m_interceptorEnabled = true;

    QWebEngineScript *loadScript(const QString &filename);
    void loadEmbeddedScript(QString &target, const QString &filename, bool compressed = false);
    QString mJs_hideScrollBars;

    QWebEngineScriptCollection *scripts;
    QList<QString> m_scripts;
};

#endif // BROWSERWINDOW_HPP
