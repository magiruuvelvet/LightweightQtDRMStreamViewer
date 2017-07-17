#include "StreamingProviderStore.hpp"
#include "ConfigManager.hpp"

StreamingProviderStore *StreamingProviderStore::instance()
{
    static StreamingProviderStore *i = new StreamingProviderStore();
    return i;
}

StreamingProviderStore::StreamingProviderStore()
{
    this->m_providerStorePath = Config()->providerStoreDir();
}

StreamingProviderStore::~StreamingProviderStore()
{
    this->m_providers.clear();
    delete this;
}

const QString &StreamingProviderStore::providerStorePath() const
{
    return this->m_providerStorePath;
}

void StreamingProviderStore::addProvider(const Provider &provider)
{
    this->m_providers.append(provider);
}

const Provider &StreamingProviderStore::provider(const QString &id) const
{
    for (auto&& i : this->m_providers)
    {
        if (i.id == id)
        {
            return i;
        }
    }

    // if the provider wasn't found return an empty one
    return this->m_null;
}
