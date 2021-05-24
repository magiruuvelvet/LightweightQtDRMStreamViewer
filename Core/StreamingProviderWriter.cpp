#include "StreamingProviderWriter.hpp"
#include "StreamingProviderParser.hpp"

#include "ConfigManager.hpp"

#include <QFile>
#include <QFileInfo>

StreamingProviderWriter::StatusCode StreamingProviderWriter::write(const Provider &provider)
{
    const QString file = Config()->localProviderStoreDir() + '/' + provider.id + StreamingProviderParser::extension;
    const QFileInfo fileInfo = QFileInfo(file);

    // Override exising file
    if (fileInfo.exists())
    {
        if (fileInfo.isWritable())
        {
            // preserve existing comments and linefeeds
            return write_private(provider, file, true);
        }
        else
        {
            return PERM_ERROR;
        }
    }
    // Create new file
    else
    {
        // write new file: no comments available
        return write_private(provider, file, false);
    }
}

StreamingProviderWriter::StatusCode StreamingProviderWriter::write_private(
    const Provider &provider,
    const QString &file,
    bool preserveCommentsAndLinefeeds)
{
    if (preserveCommentsAndLinefeeds)
    {
        QFile f(file);
        if (f.open(QFile::ReadOnly | QFile::Text))
        {
            const QString data = f.readAll();
            f.close();

            QStringList props = data.split(QRegExp("[\r\n]"), Qt::KeepEmptyParts);

            ///
            /// hacky workaround: remove all url interceptors and scripts when the size(count) changed
            /// append them in the last step (alters position in file)
            ///

            // find all positions of stackable options
            static const auto find_pos_of_all = [&](const QString &startsWith)
            {
                QList<int> pos;
                for (auto i = 0; i < props.size(); i++)
                    if (props.at(i).startsWith(startsWith, Qt::CaseInsensitive))
                        pos.append(i);
                return pos;
            };

            // find all url interceptor and script options
            auto patternPositions = find_pos_of_all("urlInterceptorPattern:");
            auto targetPositions = find_pos_of_all("urlInterceptorTarget:");
            auto scriptPositions = find_pos_of_all("script:");

            bool urlInterceptorsRemoved = false;
            if (patternPositions.size() != provider.urlInterceptorLinks.size())
            {
                urlInterceptorsRemoved = true;

                // remove all interceptors from the file to append the changed ones later
                for (auto&& i : patternPositions)
                    props[i].clear();
                for (auto&& i : targetPositions)
                    props[i].clear();

                // avoid updating
                patternPositions.clear();
                targetPositions.clear();
            }

            bool scriptsRemoved = false;
            if (scriptPositions.size() != provider.scripts.size())
            {
                scriptsRemoved = true;

                // remove all scripts from the file to append the changed ones later
                for (auto&& i : scriptPositions)
                    props[i].clear();

                // avoid updating
                scriptPositions.clear();
            }

            ///
            /// edit existing properties
            ///
            for (auto&& prop : props)
            {
                if (prop.startsWith("name:", Qt::CaseInsensitive))
                    replace_value(prop, provider.name);
                else if (prop.startsWith("icon:", Qt::CaseInsensitive))
                    replace_value(prop, provider.icon.value);
                else if (prop.startsWith("url:", Qt::CaseInsensitive))
                    replace_value(prop, provider.url.toString());
                else if (prop.startsWith("urlInterceptor:", Qt::CaseInsensitive))
                    replace_value(prop, provider.urlInterceptor ? "true" : "false");
                else if (prop.startsWith("user-agent:", Qt::CaseInsensitive))
                    replace_value(prop, provider.useragent);
                else if (prop.startsWith("titlebar:", Qt::CaseInsensitive))
                    replace_value(prop, provider.titleBarVisible ? "true" : "false");
                else if (provider.titleBarHasPermanentTitle &&
                         prop.startsWith("titlebar-text:", Qt::CaseInsensitive))
                    replace_value(prop, provider.titleBarPermanentTitle);
                else if (prop.startsWith("titlebar-color:", Qt::CaseInsensitive))
                    replace_value(prop, provider.titleBarColor.name(QColor::HexRgb));
                else if (prop.startsWith("titlebar-text-color:", Qt::CaseInsensitive))
                    replace_value(prop, provider.titleBarTextColor.name(QColor::HexRgb));
            }

            // update url interceptors
            int internalCounter_Interceptors = 0;
            for (auto i = 0; i < patternPositions.size(); i++)
            {
                replace_value(props[patternPositions.at(i)], provider.urlInterceptorLinks.at(internalCounter_Interceptors).pattern.pattern());
                replace_value(props[targetPositions.at(i)], provider.urlInterceptorLinks.at(internalCounter_Interceptors).target.toString());
                internalCounter_Interceptors++;
            }

            // update scripts
            int internalCounter_Scripts = 0;
            for (auto i = 0; i < scriptPositions.size(); i++)
            {
                replace_value(props[scriptPositions.at(i)], provider.scripts.at(internalCounter_Scripts));
                internalCounter_Scripts++;
            }

            ///
            /// add new properties
            ///
            static const auto contains_option = [&](const QString &option)
            {
                for (auto&& prop : props)
                    if (prop.startsWith(option, Qt::CaseInsensitive))
                        return true;
                return false;
            };

            if (!contains_option("name:"))
                props.append("name:" + provider.name);
            if (!contains_option("icon:"))
                props.append("icon:" + provider.icon.value);
            if (!contains_option("url:"))
                props.append("url:" + provider.url.toString());
            if (provider.urlInterceptor && !contains_option("urlInterceptor:"))
                props.append("urlInterceptor:true");
            for (auto i = internalCounter_Interceptors; i < provider.urlInterceptorLinks.size(); i++)
            {
                props.append("urlInterceptorPattern:" + provider.urlInterceptorLinks.at(i).pattern.pattern());
                props.append("urlInterceptorTarget:" + provider.urlInterceptorLinks.at(i).target.toString());
            }
            for (auto i = internalCounter_Scripts; i < provider.scripts.size(); i++)
                props.append("script:" + provider.scripts.at(i));
            if (!contains_option("user-agent:") && !provider.useragent.isEmpty())
                props.append("user-agent:" + provider.useragent);
            if (!contains_option("titlebar:") && provider.titleBarVisible)
                props.append("titlebar:true");
            if (!contains_option("titlebar-text:") && provider.titleBarHasPermanentTitle)
                props.append("titlebar-text:" + provider.titleBarPermanentTitle);
            if (!contains_option("titlebar-color:") && provider.titleBarVisible)
                props.append("titlebar-color:" + provider.titleBarColor.name(QColor::HexRgb));
            if (!contains_option("titlebar-text-color:") && provider.titleBarVisible)
                props.append("titlebar-text-color:" + provider.titleBarTextColor.name(QColor::HexRgb));


            f.open(QFile::WriteOnly | QFile::Text);
            QTextStream s(&f);
            for (auto i = 0; i < props.size(); i++)
            {
                s << props.at(i);
                // don't append linefeed on last property
                i != props.size() - 1 ? s << '\n' : s << "";
            }
            s.flush();
            f.close();
            return SUCCESS;
        }
        else
        {
            return FILE_ERROR;
        }
    }
    else
    {
        QFile f(file);
        if (f.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream s(&f);
            s << "name:" << provider.name << '\n';
            if (!provider.icon.value.isEmpty())
                s << "icon:" << provider.icon.value << '\n';
            s << "url:" << provider.url.toString() << '\n';
            if (provider.urlInterceptor)
                s << "urlInterceptor:true" << '\n';
            for (auto&& interceptor : provider.urlInterceptorLinks)
            {
                s << "urlInterceptorPattern:" << interceptor.pattern.pattern() << '\n';
                s << "urlInterceptorTarget:" << interceptor.target.toString() << '\n';
            }
            for (auto&& script : provider.scripts)
                s << "script:" << script << '\n';
            if (!provider.useragent.isEmpty())
                s << "user-agent:" << provider.useragent << '\n';
            if (provider.titleBarVisible)
                s << "titlebar:true" << '\n';
            if (provider.titleBarHasPermanentTitle)
                s << "titlebar-text:" << provider.titleBarPermanentTitle << '\n';
            if (provider.titleBarVisible)
            {
                s << "titlebar-color:" << provider.titleBarColor.name(QColor::HexRgb) << '\n';
                s << "titlebar-text-color:" << provider.titleBarTextColor.name(QColor::HexRgb) << '\n';
            }

            s.flush();
            f.close();
            return SUCCESS;
        }
        else
        {
            return FILE_ERROR;
        }
    }
}

void StreamingProviderWriter::replace_value(QString &in, const QString &new_value)
{
    auto key_sep = in.indexOf(':');
    in = in.left(key_sep + 1) + new_value;
}
