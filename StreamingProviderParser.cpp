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
#include <QColor>

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

    QDirIterator providers(StreamingProviderStore::instance()->providerStorePath(), QStringList{"*.p"},
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

    QFile file(StreamingProviderStore::instance()->providerStorePath() + '/' + provider_name + ".p");
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
    bool pTitleBarVisible = false;
    QColor pTitleBarColor, pTitleBarTextColor;
    bool hasErrors = false;

    for (auto&& i : props)
    {
        i.startsWith("name:") ? pName = i.mid(5).simplified() : QString();
        i.startsWith("icon:") ? pIcon = i.mid(5).simplified() : QString();
        i.startsWith("url:")  ? pUrl  = i.mid(4).simplified() : QString();

        i.startsWith("titlebar:") ? pTitleBarVisible = (i.mid(9).simplified() == QLatin1String("true") ? true : false) : false;
        i.startsWith("titlebar-color:") ? pTitleBarColor = QColor(i.mid(15).simplified()) : QColor(50, 50, 50, 255);
        i.startsWith("titlebar-text:") ? pTitleBarTextColor = QColor(i.mid(14).simplified()) : QColor(255, 255, 255, 255);
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
    if (pTitleBarVisible && !pTitleBarColor.isValid())
    {
        qDebug() << provider_name << "Field 'titlebar-color' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #323232.";
        pTitleBarColor = QColor(50, 50, 50, 255);
    }
    if (pTitleBarVisible && !pTitleBarTextColor.isValid())
    {
        qDebug() << provider_name << "Field 'titlebar-text' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #ffffff.";
        pTitleBarTextColor = QColor(255, 255, 255, 255);
    }

    if (hasErrors)
    {
        qDebug() << provider_name << "contains errors and was skipped!";
        return false;
    }

    StreamingProviderStore::instance()->addProvider(provider_name, pName, pIcon, pUrl,
                                                    pTitleBarVisible, pTitleBarColor, pTitleBarTextColor);

    pName.clear();
    pIcon.clear();
    pUrl.clear();

    return true;
}
