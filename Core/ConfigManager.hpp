#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>
#include <QStringList>

class ConfigManager
{
public:
    static ConfigManager *instance();
    ~ConfigManager();

    // Get app configuration directory
    const QString &dir() const;

    // Get web engine profile directory
    const QString &webEngineProfiles() const;

    // Get provider store location
    const QString &localProviderStoreDir() const;
    const QStringList &providerStoreDirs() const;

    // Startup profile to use, if empty display the main UI
    const QString &startupProfile() const { return this->m_startupProfile; }
    QString &startupProfile() { return this->m_startupProfile; }

    // What mode to use to display widgets
    const bool &fullScreenMode() const { return this->m_fullScreenMode; }
    bool &fullScreenMode() { return this->m_fullScreenMode; }

    // Use URL Interceptor to hijack URLs
    const bool &urlInterceptorEnabled() const { return this->m_urlInterceptorEnabled; }
    bool &urlInterceptorEnabled() { return this->m_urlInterceptorEnabled; }

private:
    ConfigManager();

    QString m_dir,
            m_webEngineProfiles;
    QStringList m_providerStoreDirs;

    QString m_startupProfile;
    bool m_fullScreenMode = false;
    bool m_urlInterceptorEnabled = true;
};

inline ConfigManager *Config()
{
    return ConfigManager::instance();
}

#endif // CONFIGMANAGER_HPP
