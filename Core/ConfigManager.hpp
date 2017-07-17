#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>

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
    const QString &providerStoreDir() const;

    // Startup profile to use, if empty display the main UI
    const QString &startupProfile() const { return this->m_startupProfile; }
    QString &startupProfile() { return this->m_startupProfile; }

    // What mode to use to display widgets
    const bool &fullScreenMode() const { return this->m_fullScreenMode; }
    bool &fullScreenMode() { return this->m_fullScreenMode; }

private:
    ConfigManager();

    QString m_dir,
            m_webEngineProfiles,
            m_providerStoreDir;

    QString m_startupProfile;
    bool m_fullScreenMode = false;
};

inline ConfigManager *Config()
{
    return ConfigManager::instance();
}

#endif // CONFIGMANAGER_HPP
