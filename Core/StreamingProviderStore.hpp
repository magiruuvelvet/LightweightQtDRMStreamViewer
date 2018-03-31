#ifndef STREAMINGPROVIDERSTORE_HPP
#define STREAMINGPROVIDERSTORE_HPP

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QList>
#include <QColor>
#include <QIcon>

class BrowserWindow;

struct UrlInterceptorLink
{
    QRegExp pattern;
    QUrl target;
};

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
    QList<QString> scripts;
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
