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

            QStringList props = data.split(QRegExp("[\r\n]"), QString::KeepEmptyParts);
            for (auto&& prop : props)
            {
                if (prop.startsWith("name", Qt::CaseInsensitive))
                    replace_value(prop, provider.name);
                else if (prop.startsWith("icon", Qt::CaseInsensitive))
                    replace_value(prop, provider.icon.value);
                else if (prop.startsWith("url", Qt::CaseInsensitive))
                    replace_value(prop, provider.url.toString());
            }

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
            s << "icon:" << provider.icon.value << '\n';
            s << "url:" << provider.url.toString() << '\n';

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
