#include "ConfigManager.hpp"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>

#include <QDebug>

ConfigManager *ConfigManager::instance()
{
    static ConfigManager *i = new ConfigManager();
    return i;
}

ConfigManager::ConfigManager()
{
    // Setup Configuration Directory
    const QString baseConfigLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    const QString appConfigLocation = baseConfigLocation
                           + '/'
                           + qApp->applicationName();
    const QString providerLocation = qApp->applicationName() + '/' + "providers";
    qDebug() << "Using configuration directory" << appConfigLocation;

    QDir config_dir(baseConfigLocation);
    if (!config_dir.mkpath(providerLocation))
    {
        qDebug() << "Unable to create the configuration path!";
        return;
    }

    this->m_dir = appConfigLocation;
    this->m_webEngineProfiles = appConfigLocation + '/' + "WebEngineProfiles";
    this->m_providerStoreDirs.append(baseConfigLocation + '/' + providerLocation);

#ifdef Q_OS_UNIX
    // *NIX: add system paths (usually /usr/share)
    const auto paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    for (auto&& path : paths)
        if (path.startsWith("/usr"))
            this->m_providerStoreDirs.insert(0, path + '/' + "providers");
#endif

#ifdef Q_OS_WIN32
    // Windows: add exe directory
    this->m_providerStoreDirs.insert(0, QApplication::applicationDirPath() + '/' + "providers");
#endif
}

ConfigManager::~ConfigManager()
{
    this->m_dir.clear();
    this->m_webEngineProfiles.clear();
    this->m_providerStoreDirs.clear();
    this->m_startupProfile.clear();
    delete this;
}

const QString &ConfigManager::dir() const
{
    return this->m_dir;
}

const QString &ConfigManager::webEngineProfiles() const
{
    return this->m_webEngineProfiles;
}

const QString &ConfigManager::localProviderStoreDir() const
{
    return this->m_providerStoreDirs.last();
}

const QStringList &ConfigManager::providerStoreDirs() const
{
    return this->m_providerStoreDirs;
}
