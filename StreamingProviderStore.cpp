#include "StreamingProviderStore.hpp"

StreamingProviderStore::StreamingProviderStore()
{

}

StreamingProviderStore::~StreamingProviderStore()
{
    this->m_providers.clear();
    delete this;
}

StreamingProviderStore *StreamingProviderStore::instance()
{
    static StreamingProviderStore *m_instance = new StreamingProviderStore();
    return m_instance;
}

void StreamingProviderStore::setProviderStorePath(const QString &path)
{
    this->m_providerStorePath = path;
}

const QString &StreamingProviderStore::providerStorePath() const
{
    return this->m_providerStorePath;
}

void StreamingProviderStore::addProvider(const QString &id, const QString &name, const QString &icon, const QUrl &provider,
                                         bool titleBarVisible, const QColor &titleBarColor, const QColor &titleBarTextColor)
{
    this->m_providers.append(Provider{id, name, icon, provider,
                                      titleBarVisible, titleBarColor, titleBarTextColor});
}

const StreamingProviderStore::Provider &StreamingProviderStore::provider(const QString &id) const
{
    static const Provider null{QString(), QString(), QString(), QUrl(),
                               false, QColor(), QColor()};

    for (auto&& i : this->m_providers)
    {
        if (i.id == id)
        {
            return i;
        }
    }

    return null;
}
