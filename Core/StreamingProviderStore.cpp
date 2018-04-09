#include "StreamingProviderStore.hpp"
#include "ConfigManager.hpp"

#include <Widgets/BrowserWindow.hpp>

Script Script::parse(const QString &script)
{
    const auto scr = script.split(',', QString::KeepEmptyParts);
    if (scr.size() > 1)
    {
        const auto injection_pt = scr.at(1);
        if (QString::compare(injection_pt, "deferred", Qt::CaseInsensitive) == 0 ||
            QString::compare(injection_pt, "deferr", Qt::CaseInsensitive) == 0 ||
            QString::compare(injection_pt, "defer", Qt::CaseInsensitive) == 0)
            return Script{scr.at(0), Script::Deferred};
        else if (QString::compare(injection_pt, "documentready", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "docready", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "ready", Qt::CaseInsensitive) == 0)
            return Script{scr.at(0), Script::DocumentReady};
        else if (QString::compare(injection_pt, "documentcreation", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "doccreation", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "doccreate", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "creation", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "create", Qt::CaseInsensitive) == 0)
            return Script{scr.at(0), Script::DocumentCreation};
        else if (QString::compare(injection_pt, "automatic", Qt::CaseInsensitive) == 0 ||
                 QString::compare(injection_pt, "auto", Qt::CaseInsensitive) == 0 ||
                 injection_pt.isEmpty())
            return Script{scr.at(0), Script::Automatic};
        else
            return Script{scr.at(0), Script::Automatic};
    }
    else
    {
        return Script{scr.at(0), Script::Automatic};
    }
}

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

Provider *StreamingProviderStore::provider_ptr(const QString &id)
{
    for (auto&& i : this->m_providers)
    {
        if (i.id == id)
        {
            return &i;
        }
    }

    return nullptr;
}

Provider *StreamingProviderStore::providerAt_ptr(int index)
{
    if (index > this->m_providers.size())
        return nullptr;
    return &this->m_providers[index];
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

    if (!pr.useragent.isEmpty())
        w->setUserAgent(pr.useragent);

    pr.titleBarHasPermanentTitle ?
        w->setBaseTitle(pr.titleBarPermanentTitle, true) :
        w->setBaseTitle(pr.name);
    w->setTitleBarVisibility(pr.titleBarVisible);
    w->setTitleBarColor(pr.titleBarColor, pr.titleBarTextColor);
    w->setWindowTitle("Loading...");
    w->setWindowIcon(pr.icon.icon);
    w->setProfile(pr.id);
    w->setScripts(pr.scripts);
    w->setUrlInterceptorEnabled(pr.urlInterceptor, pr.urlInterceptorLinks);
    w->setUrl(pr.url);
}

void StreamingProviderStore::resetProfile(BrowserWindow *w)
{
    if (!w)
        return;

    Provider def;

    w->providerPath = Config()->localProviderStoreDir();
    w->restoreUserAgent();
    w->setBaseTitle(qApp->applicationDisplayName());
    w->setTitleBarVisibility(def.titleBarVisible);
    w->setTitleBarColor(def.titleBarColor, def.titleBarTextColor);
    w->setWindowTitle("[empty]");
    w->setWindowIcon(def.icon.icon);
    w->setProfile("Default");
    w->removeScripts();
    w->setUrlInterceptorEnabled(def.urlInterceptor);
    w->setUrl(def.url);
}
