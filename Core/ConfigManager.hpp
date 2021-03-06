#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>
#include <QStringList>
#include <QRect>

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

    // Gui: MainWindow position and size
    void setMainWindowGeometry(const QRect &rect);
    const QRect &mainWindowGeometry() const;

    // Gui: ConfigWindow position and size
    void setConfigWindowGeometry(const QRect &rect);
    const QRect &configWindowGeometry() const;

private:
    ConfigManager();

    QString m_dir,
            m_webEngineProfiles;
    QStringList m_providerStoreDirs;

    QString m_startupProfile;
    bool m_fullScreenMode = false;
    bool m_urlInterceptorEnabled = true;

    QRect m_mainWindowGeometry = QRect(0, 0, 0, 0);
    QRect m_configWindowGeometry = QRect(0, 0, 0, 0);

private:
    QString m_uiConfigFile;
    bool readUiConfig();
    bool writeUiConfig();
};

inline ConfigManager *Config()
{
    return ConfigManager::instance();
}

#endif // CONFIGMANAGER_HPP
