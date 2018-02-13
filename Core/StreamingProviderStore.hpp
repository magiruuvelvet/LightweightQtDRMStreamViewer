#ifndef STREAMINGPROVIDERSTORE_HPP
#define STREAMINGPROVIDERSTORE_HPP

#include <QString>
#include <QUrl>
#include <QList>
#include <QColor>
#include <QIcon>

class Provider
{
public:
    Provider() {}

    QString    id;

    QString    name;
    QIcon      icon;
    QUrl       url = QUrl(QLatin1String("about:blank"));
    bool       urlInterceptor = true;

    QString    titleBarPermanentTitle;
    QColor     titleBarColor;
    QColor     titleBarTextColor;
    bool       titleBarVisible = false;
    bool       titleBarHasPermanentTitle = false;
};

class StreamingProviderStore
{
public:
    static StreamingProviderStore *instance();
    ~StreamingProviderStore();

    const QString &providerStorePath() const;

    void addProvider(const Provider &provider);
    const QList<Provider> &providers() const
    { return this->m_providers; }
    const Provider &providerAt(int index) const
    { return this->m_providers.at(index); }
    const Provider &provider(const QString &id) const;

private:
    StreamingProviderStore();
    QList<Provider> m_providers;
    QString m_providerStorePath;

    Provider m_null;
};

#endif // STREAMINGPROVIDERSTORE_HPP
