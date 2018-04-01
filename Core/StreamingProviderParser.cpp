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
    this->makeValidPaths();
}

StreamingProviderParser::~StreamingProviderParser()
{
    this->m_providers.clear();
}

void StreamingProviderParser::findAll()
{
    for (auto&& path : this->m_validPaths)
    {
        QStringList currentPaths;

        QDirIterator providers(path,
                               QStringList{StreamingProviderParser::search_pattern},
                               QDir::Files | QDir::Readable | QDir::NoDotAndDotDot,
                               QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
        while (providers.hasNext())
        {
            (void) providers.next();

            const auto provider_file = providers.fileInfo().absoluteFilePath();
            currentPaths.append(provider_file);
        }

        currentPaths.sort();
        this->m_providers.append(currentPaths);
    }
}

StreamingProviderParser::StatusCode StreamingProviderParser::parse(const QString &provider_name) const
{
    const auto provider_file = this->findHighestPriorityProvider(provider_name);
    if (provider_file.isEmpty())
        return FILE_ERROR;

    if (StreamingProviderStore::instance()->contains(QFileInfo(provider_name).baseName()))
        return ALREADY_IN_LIST;

    const auto provider_path = QFileInfo(provider_file).path();

    QFile file(provider_file);
    QString data;
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        data = file.readAll();
        file.close();
    }
    else
    {
        qDebug() << provider_file << "Unable to open file for reading!";
        return FILE_ERROR;
    }

    if (data.isEmpty())
    {
        qDebug() << provider_file << "File is empty!";
        return FILE_EMPTY;
    }

    QStringList props = data.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    data.clear();

    Provider provider;
    provider.id = QFileInfo(provider_file).baseName();
    provider.path = provider_path;
    qDebug() << "<<<" << provider.id << provider.path;
    bool hasErrors = false;

    for (auto&& i : props)
    {
        if (i.startsWith('#'))
            continue;

        i.startsWith("name:") ? provider.name = i.mid(5).simplified() : QString();
        if (i.startsWith("icon:"))
        {
            const auto icon = i.mid(5).simplified();

            if (!icon.isEmpty())
            {
                if (QFileInfo(icon).isAbsolute())
                    provider.icon = QIcon(icon);
                else
                    //provider.icon = QIcon(StreamingProviderStore::instance()->providerStorePath() + '/' + icon);
                    provider.icon = QIcon(provider_path + '/' + icon);
            }
        }

        i.startsWith("url:")  ? provider.url  = QUrl(i.mid(4).simplified()) : QUrl();
        i.startsWith("urlInterceptor:") ? (provider.urlInterceptor = i.mid(15).simplified() == "true" ? true : false) : true;

        i.startsWith("user-agent:") ? provider.useragent = i.mid(11).simplified() : QString();

        i.startsWith("titlebar:") ? provider.titleBarVisible = (i.mid(9).simplified() == QLatin1String("true") ? true : false) : false;
        i.startsWith("titlebar-color:") ? provider.titleBarColor = QColor(i.mid(15).simplified()) : QColor(50, 50, 50, 255);
        i.startsWith("titlebar-text-color:") ? provider.titleBarTextColor = QColor(i.mid(20).simplified()) : QColor(255, 255, 255, 255);

        if (i.startsWith("titlebar-text:"))
        {
            provider.titleBarHasPermanentTitle = true;
            provider.titleBarPermanentTitle = i.mid(14).simplified();
        }

        if (i.startsWith("urlInterceptorPattern:"))
        {
            const auto pattern = i.mid(22).simplified();
            provider.urlInterceptorLinks.append(UrlInterceptorLink{QRegExp(pattern), QUrl()});
            qDebug() << provider_file << "Found URL Interceptor Pattern:" << pattern;
        }
        if (i.startsWith("urlInterceptorTarget:"))
        {
            const auto target = i.mid(21).simplified();
            if (provider.urlInterceptorLinks.size() != 0 &&
                !provider.urlInterceptorLinks.last().pattern.isEmpty() &&
                provider.urlInterceptorLinks.last().target.isEmpty())
            {
                provider.urlInterceptorLinks.last().target = QUrl(target);
                qDebug() << provider_file << "Added new target for pattern:" << target;
            }
            else
            {
                qDebug() << provider_file << "Missing pattern or target for last pattern!";
            }
        }

        if (i.startsWith("script:"))
        {
            const auto script = i.mid(7).simplified();
            const auto scr = script.split(',', QString::SkipEmptyParts);

            static const auto contains_script = [&](const QList<Script> &scripts, const QString &filename) {
                for (auto&& scr : scripts)
                    if (scr.filename == filename)
                        return true;
                return false;
            };

            if (!contains_script(provider.scripts, scr.at(0)))
            {
                if (scr.size() > 1)
                {
                    const auto injection_pt = scr.at(1);
                    if (QString::compare(injection_pt, "deferred", Qt::CaseInsensitive) == 0 ||
                        QString::compare(injection_pt, "deferr", Qt::CaseInsensitive) == 0 ||
                        QString::compare(injection_pt, "defer", Qt::CaseInsensitive) == 0)
                        provider.scripts.append(Script{scr.at(0), Script::Deferred});
                    else if (QString::compare(injection_pt, "documentready", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "docready", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "ready", Qt::CaseInsensitive) == 0)
                        provider.scripts.append(Script{scr.at(0), Script::DocumentReady});
                    else if (QString::compare(injection_pt, "documentcreation", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "doccreation", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "doccreate", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "creation", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "create", Qt::CaseInsensitive) == 0)
                        provider.scripts.append(Script{scr.at(0), Script::DocumentCreation});
                    else if (QString::compare(injection_pt, "automatic", Qt::CaseInsensitive) == 0 ||
                             QString::compare(injection_pt, "auto", Qt::CaseInsensitive) == 0 ||
                             injection_pt.isEmpty())
                        provider.scripts.append(Script{scr.at(0), Script::Automatic});
                    else
                    {
                        qDebug() << provider_file << "Warning: Unknown script option:" << injection_pt
                                 << "| Default to Automatic";
                        provider.scripts.append(Script{scr.at(0), Script::Automatic});
                    }

                    qDebug() << provider_file << "Loaded script" << scr.at(0) << "with mode" << scr.at(1);
                }
                else
                {
                    provider.scripts.append(Script{scr.at(0), Script::Automatic});
                    qDebug() << provider_file << "Loaded script" << scr.at(0) << "with mode" << Script::Automatic;
                }
            }
            else
                qDebug() << provider_file << "Warning: Duplicate 'script' skipped ->" << script;
        }
    }

    if (provider.name.isEmpty())
    {
        qDebug() << provider_file << "Field 'name' must not be empty.";
        hasErrors = true;
    }
    if (provider.icon.isNull())
    {
        qDebug() << provider_file << "Field 'icon' is empty. Falling back to text name.";
    }
    if (provider.url.isEmpty())
    {
        qDebug() << provider_file << "Field 'url' must not be empty.";
        hasErrors = true;
    }
    if (provider.titleBarVisible && !provider.titleBarColor.isValid())
    {
        qDebug() << provider_file << "Field 'titlebar-color' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #323232.";
        provider.titleBarColor = QColor(50, 50, 50, 255);
    }
    if (provider.titleBarVisible && !provider.titleBarTextColor.isValid())
    {
        qDebug() << provider_file << "Field 'titlebar-text-color' is not a valid color. See the Qt doc on how to set this field correctly."
                                  << "Falling back to the default color which is #ffffff.";
        provider.titleBarTextColor = QColor(255, 255, 255, 255);
    }

    if (!provider.urlInterceptorLinks.isEmpty())
    {
        for (auto&& url : provider.urlInterceptorLinks)
        {
            if (url.pattern.isEmpty() || url.target.isEmpty())
            {
                qDebug() << provider_file << "URL Interceptor list is invalid. Please fix the issue.";
                hasErrors = true;
            }
        }
    }

    if (hasErrors)
    {
        qDebug() << provider_file << "contains errors and was skipped!";
        return SYNTAX_ERROR;
    }

    StreamingProviderStore::instance()->addProvider(provider);

    return SUCCESS;
}

void StreamingProviderParser::makeValidPaths()
{
    for (auto&& path : Config()->providerStoreDirs())
    {
        QFileInfo finfo(path);
        if (finfo.exists() && finfo.isReadable())
        {
            this->m_validPaths.append(path);
        }
    }
}

const QString StreamingProviderParser::findHighestPriorityProvider(const QString &provider_name) const
{
    QString provider;
    for (auto&& path : this->m_providers)
        if (path.endsWith(QFileInfo(provider_name).baseName() + StreamingProviderParser::extension))
            provider = path;
    return provider;
}
