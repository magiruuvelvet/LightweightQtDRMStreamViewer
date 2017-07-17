#ifndef STREAMINGPROVIDERSTORE_HPP
#define STREAMINGPROVIDERSTORE_HPP

#include <QString>
#include <QUrl>
#include <QList>
#include <QColor>

class StreamingProviderStore
{
public:
    ~StreamingProviderStore();
    static StreamingProviderStore *instance();

    void setProviderStorePath(const QString &path);
    const QString &providerStorePath() const;

    struct Provider {
        QString id;
        QString name;
        QString icon;
        QUrl url;
        bool titleBarVisible;
        QColor titleBarColor;
        QColor titleBarTextColor;
    };

    void addProvider(const QString &id, const QString &name, const QString &icon, const QUrl &url,
                     bool titleBarVisible, const QColor &titleBarColor, const QColor &titleBarTextColor);
    const QList<Provider> &providers() const
    { return this->m_providers; }
    const Provider &providerAt(int index) const
    { return this->m_providers.at(index); }
    const Provider &provider(const QString &id) const;

private:
    StreamingProviderStore();
    QList<Provider> m_providers;
    QString m_providerStorePath;
};

#endif // STREAMINGPROVIDERSTORE_HPP
