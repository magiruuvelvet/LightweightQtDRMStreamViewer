#ifndef STREAMINGPROVIDERSTORE_HPP
#define STREAMINGPROVIDERSTORE_HPP

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QList>
#include <QColor>
#include <QIcon>

#include <QWebEngineScript>

class BrowserWindow;

struct UrlInterceptorLink
{
    QRegExp pattern;
    QUrl target;
};

struct Script
{
    QString filename;
    enum InjectionPoint {
        Deferred,
        DocumentReady,
        DocumentCreation,
        Automatic // @run-at from userscript; default
    } injectionPoint;
};

#include <QDebug>
inline QDebug operator<< (QDebug d, const Script::InjectionPoint &injection_pt)
{
    d << ([&]{
        switch (injection_pt)
        {
            case Script::Deferred: return "defer";
            case Script::DocumentReady: return "ready";
            case Script::DocumentCreation: return "create";
            case Script::Automatic: return "auto";
        }
    })();
    return d;
}
inline QDebug operator<< (QDebug d, const Script &script) {
    d << '['
      << script.filename.toUtf8().constData() << ','
      << script.injectionPoint
      << ']';
    return d;
}

struct Provider
{
    QString    id;
    QString    path;

    QString    name;
    QIcon      icon;
    QUrl       url = QUrl(QLatin1String("about:blank"));
    bool       urlInterceptor = true;
    QString    useragent;

    QString    titleBarPermanentTitle;
    QColor     titleBarColor;
    QColor     titleBarTextColor;
    bool       titleBarVisible = false;
    bool       titleBarHasPermanentTitle = false;

    QList<UrlInterceptorLink> urlInterceptorLinks;
    QList<Script> scripts;
};

class StreamingProviderStore
{
public:
    static StreamingProviderStore *instance();
    ~StreamingProviderStore();

    const QString &localProviderStorePath() const;
    const QStringList &providerStorePaths() const;

    void addProvider(const Provider &provider);
    const QList<Provider> &providers() const
    { return this->m_providers; }
    const Provider &providerAt(int index) const
    { return this->m_providers.at(index); }
    const Provider &provider(const QString &id) const;

    bool contains(const QString &id) const;
    void sort();

    static void loadProfile(BrowserWindow *w, const Provider &provider);

private:
    StreamingProviderStore();
    QList<Provider> m_providers;
    QStringList m_providerStorePaths;

    Provider m_null;
};

#endif // STREAMINGPROVIDERSTORE_HPP
