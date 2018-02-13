#include "StreamingProviderParser.hpp"
#include "StreamingProviderStore.hpp"

#include "ConfigManager.hpp"

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

const char *StreamingProviderParser::extension = ".p";
const char *StreamingProviderParser::search_pattern = "*.p";

StreamingProviderParser::StreamingProviderParser()
{
}

StreamingProviderParser::~StreamingProviderParser()
{
    this->m_providers.clear();
}

void StreamingProviderParser::findAll()
{
    if (StreamingProviderStore::instance()->providerStorePath().isEmpty())
        return;

    QDirIterator providers(StreamingProviderStore::instance()->providerStorePath(),
                           QStringList{StreamingProviderParser::search_pattern},
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
    if (StreamingProviderStore::instance()->providerStorePath().isEmpty() ||
        !this->m_providers.contains(provider_name))
        return false;

    QFile file(StreamingProviderStore::instance()->providerStorePath() + '/' + provider_name + StreamingProviderParser::extension);
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

    Provider provider;
    provider.id = provider_name;
    bool hasErrors = false;

    for (auto&& i : props)
    {
        i.startsWith("name:") ? provider.name = i.mid(5).simplified() : QString();
        i.startsWith("icon:") ? provider.icon = QIcon(StreamingProviderStore::instance()->providerStorePath() + '/' + i.mid(5).simplified()) : QIcon();
        i.startsWith("url:")  ? provider.url  = QUrl(i.mid(4).simplified()) : QUrl();
        i.startsWith("urlInterceptor:") ? (provider.urlInterceptor = i.mid(15).simplified() == "true" ? true : false) : true;

        i.startsWith("titlebar:") ? provider.titleBarVisible = (i.mid(9).simplified() == QLatin1String("true") ? true : false) : false;
        i.startsWith("titlebar-color:") ? provider.titleBarColor = QColor(i.mid(15).simplified()) : QColor(50, 50, 50, 255);
        i.startsWith("titlebar-text-color:") ? provider.titleBarTextColor = QColor(i.mid(20).simplified()) : QColor(255, 255, 255, 255);

        if (i.startsWith("titlebar-text:"))
        {
            provider.titleBarHasPermanentTitle = true;
            provider.titleBarPermanentTitle = i.mid(14).simplified();
        }
    }

    if (provider.name.isEmpty())
    {
        qDebug() << provider_name << "Field 'name' must not be empty.";
        hasErrors = true;
    }
    if (provider.icon.isNull())
    {
        qDebug() << provider_name << "Field 'icon' is empty. Falling back to text name.";
    }
    if (provider.url.isEmpty())
    {
        qDebug() << provider_name << "Field 'url' must not be empty.";
        hasErrors = true;
    }
    if (provider.titleBarVisible && !provider.titleBarColor.isValid())
    {
        qDebug() << provider_name << "Field 'titlebar-color' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #323232.";
        provider.titleBarColor = QColor(50, 50, 50, 255);
    }
    if (provider.titleBarVisible && !provider.titleBarTextColor.isValid())
    {
        qDebug() << provider_name << "Field 'titlebar-text-color' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #ffffff.";
        provider.titleBarTextColor = QColor(255, 255, 255, 255);
    }

    if (hasErrors)
    {
        qDebug() << provider_name << "contains errors and was skipped!";
        return false;
    }

    StreamingProviderStore::instance()->addProvider(provider);

    return true;
}
