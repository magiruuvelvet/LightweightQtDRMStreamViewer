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
    this->m_providerStoreDir = baseConfigLocation + '/' + providerLocation;
}

ConfigManager::~ConfigManager()
{
    this->m_dir.clear();
    this->m_webEngineProfiles.clear();
    this->m_providerStoreDir.clear();
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

const QString &ConfigManager::providerStoreDir() const
{
    return this->m_providerStoreDir;
}
