#ifndef STREAMINGPROVIDERWRITER_HPP
#define STREAMINGPROVIDERWRITER_HPP

#include "StreamingProviderStore.hpp"

#include <QString>
#include <QStringList>
#include <QColor>

class StreamingProviderWriter
{
    StreamingProviderWriter() {}

public:

    enum StatusCode {
        SUCCESS,         // no error
        PERM_ERROR,      // permission related error
        FILE_ERROR       // file i/o error
    };

    static StatusCode write(const Provider &provider);

private:
    static StatusCode write_private(const Provider &provider, const QString &file, bool preserveCommentsAndLinefeeds);
    static void replace_value(QString &in, const QString &new_value);
};

#endif // STREAMINGPROVIDERWRITER_HPP
