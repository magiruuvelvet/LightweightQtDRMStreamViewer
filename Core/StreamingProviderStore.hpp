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

    // convert helper
    //  > filename,injectionPoint[auto]
    operator const QString() const {
        QString ret = this->filename;
        switch (this->injectionPoint)
        {
            case Script::Automatic: break;
            case Script::Deferred: ret.append(",defer"); break;
            case Script::DocumentReady: ret.append(",ready"); break;
            case Script::DocumentCreation: ret.append(",create"); break;
        }
        return ret;
    }

    // parse string and convert to Script struct
    static Script parse(const QString &script);
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
    //QIcon      icon;
    struct {
        QIcon icon;
        QString value;
    } icon;
    QUrl       url = QUrl(QLatin1String("about:blank"));
    bool       urlInterceptor = false;
    QString    useragent;

    QString    titleBarPermanentTitle;
    QColor     titleBarColor = QColor(50, 50, 50, 255);
    QColor     titleBarTextColor = QColor(255, 255, 255, 255);
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
    Provider *provider_ptr(const QString &id);

    bool contains(const QString &id) const;
    inline int count() const { return this->m_providers.size(); }
    void sort();

    static void loadProfile(BrowserWindow *w, const Provider &provider);
    static void resetProfile(BrowserWindow *w);

private:
    StreamingProviderStore();
    QList<Provider> m_providers;
    QStringList m_providerStorePaths;

    Provider m_null;
};

#endif // STREAMINGPROVIDERSTORE_HPP
