#include "StreamingProviderParser.hpp"
#include "StreamingProviderStore.hpp"

#include <QApplication>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QByteArray>
#include <QRegExp>

#include <QDebug>

StreamingProviderParser::StreamingProviderParser()
{
    const QString location = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                           + '/'
                           + qApp->applicationName();
    qDebug() << "Using configuration directory" << location;

    QDir config_dir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    if (!config_dir.mkpath(qApp->applicationName() + '/' + "providers"))
    {
        qDebug() << "Unable to create the configuration path!";
        return;
    }

    this->m_configPath = location;
    StreamingProviderStore::instance()->setProviderStorePath(this->m_configPath + '/' + "providers");
}

StreamingProviderParser::~StreamingProviderParser()
{
    this->m_configPath.clear();
    this->m_providers.clear();
}

void StreamingProviderParser::findAll()
{
    if (this->m_configPath.isEmpty())
        return;

    QDirIterator providers(this->m_configPath + '/' + "providers", QStringList{"*.p"},
                           QDir::Files | QDir::Readable | QDir::NoDotAndDotDot,
                           QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
    while (providers.hasNext())
    {
        (void) providers.next();
        this->m_providers.append(providers.fileInfo().baseName());
    }

    this->m_providers.sort();
}

bool StreamingProviderParser::parse(const QString &provider_name) const
{
    if (this->m_configPath.isEmpty() || !this->m_providers.contains(provider_name))
        return false;

    QFile file(this->m_configPath + '/' + "providers" + '/' + provider_name + ".p");
    QString data;
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        data = file.readAll();
        file.close();
    }
    else
    {
        qDebug() << provider_name << "Unable to open file for reading!";
        return false;
    }

    if (data.isEmpty())
    {
        qDebug() << provider_name << "File is empty!";
        return false;
    }

    QStringList props = data.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    data.clear();

    QString pName, pIcon, pUrl;
    bool hasErrors = false;

    for (auto&& i : props)
    {
        i.startsWith("name:") ? pName = i.mid(5).simplified() : QString();
        i.startsWith("icon:") ? pIcon = i.mid(5).simplified() : QString();
        i.startsWith("url:")  ? pUrl  = i.mid(4).simplified() : QString();
    }

    if (pName.isEmpty())
    {
        qDebug() << provider_name << "Field 'name' must not be empty.";
        hasErrors = true;
    }
    if (pIcon.isEmpty())
    {
        qDebug() << provider_name << "Field 'icon' is empty. Falling back to text name.";
    }
    if (pUrl.isEmpty())
    {
        qDebug() << provider_name << "Field 'url' must not be empty.";
        hasErrors = true;
    }

    if (hasErrors)
    {
        qDebug() << provider_name << "contains errors and was skipped!";
        return false;
    }

    StreamingProviderStore::instance()->addProvider(provider_name, pName, pIcon, pUrl);

    pName.clear();
    pIcon.clear();
    pUrl.clear();

    return true;
}
