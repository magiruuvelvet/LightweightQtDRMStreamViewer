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

void StreamingProviderStore::loadProfile(BrowserWindow *w, const Provider &pr)
{
    if (!w)
        return;

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

    //Config()->fullScreenMode() ? w->showFullScreen() : w->showNormal();
}
