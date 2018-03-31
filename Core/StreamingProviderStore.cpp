#include "StreamingProviderStore.hpp"
#include "ConfigManager.hpp"

#include <Widgets/BrowserWindow.hpp>

StreamingProviderStore *StreamingProviderStore::instance()
{
    static StreamingProviderStore *i = new StreamingProviderStore();
    return i;
}

StreamingProviderStore::StreamingProviderStore()
{
    this->m_providerStorePaths = Config()->providerStoreDirs();
}

StreamingProviderStore::~StreamingProviderStore()
{
    this->m_providers.clear();
    this->m_providerStorePaths.clear();
    delete this;
}

const QString &StreamingProviderStore::localProviderStorePath() const
{
    return this->m_providerStorePaths.last();
}

const QStringList &StreamingProviderStore::providerStorePaths() const
{
    return this->m_providerStorePaths;
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

bool StreamingProviderStore::contains(const QString &id) const
{
    for (auto&& provider : this->m_providers)
        if (provider.id == id)
            return true;
    return false;
}

void StreamingProviderStore::sort()
{
    std::sort(this->m_providers.begin(), this->m_providers.end(),
              [](const Provider &p1, const Provider &p2){
        return p1.id < p2.id;
    });
}

void StreamingProviderStore::loadProfile(BrowserWindow *w, const Provider &pr)
{
    if (!w)
        return;

    w->providerPath = pr.path;

    pr.titleBarHasPermanentTitle ?
        w->setBaseTitle(pr.titleBarPermanentTitle, true) :
        w->setBaseTitle(pr.name);
    w->setTitleBarVisibility(pr.titleBarVisible);
    w->setTitleBarColor(pr.titleBarColor, pr.titleBarTextColor);
    w->setWindowTitle("Loading...");
    w->setWindowIcon(pr.icon);
    w->setProfile(pr.id);
    w->setScripts(pr.scripts);
    w->setUrlInterceptorEnabled(pr.urlInterceptor, pr.urlInterceptorLinks);
    w->setUrl(pr.url);
}
