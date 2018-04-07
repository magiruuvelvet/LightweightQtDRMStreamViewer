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

            ///
            /// edit existing properties
            ///
            QStringList props = data.split(QRegExp("[\r\n]"), QString::KeepEmptyParts);
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

//            quint32 pos = 0;
//            bool replaced = false;
//            for (auto&& prop : props)
//            {
//                for (auto&& interceptor : provider.urlInterceptorLinks)
//                {
//                    if (prop.startsWith("urlInterceptorPattern:", Qt::CaseInsensitive))
//                    {
//                        replace_value(prop, interceptor.pattern.pattern());
//                        replaced = true;
//                    }
//                }
//                pos++;
//            }
//-----------------------
//            for (auto&& interceptor : provider.urlInterceptorLinks)
//            {
//                if (prop.startsWith("urlInterceptorPattern:", Qt::CaseInsensitive))
//                    replace_value(prop, interceptor.pattern.pattern());
//                else if (prop.startsWith("urlInterceptorTarget:", Qt::CaseInsensitive))
//                    replace_value(prop, interceptor.target.toString());
//            }
//-----------------------
            // for scripts

            ///
            /// add new properties
            ///


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
            {
                s << "script:" << script.filename;
                switch (script.injectionPoint)
                {
                    case Script::Automatic: break;
                    case Script::Deferred: s << ",defer"; break;
                    case Script::DocumentReady: s << ",ready"; break;
                    case Script::DocumentCreation: s << ",create"; break;
                }
                s << '\n';
            }
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
