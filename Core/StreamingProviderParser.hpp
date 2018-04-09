#ifndef STREAMINGPROVIDERPARSER_HPP
#define STREAMINGPROVIDERPARSER_HPP

#include <QString>
#include <QStringList>
#include <QColor>

#include "ConfigManager.hpp"

class QIcon;

class StreamingProviderParser
{
    friend class StreamingProviderWriter;

public:
    StreamingProviderParser();
    ~StreamingProviderParser();

    enum StatusCode {
        SUCCESS,
        FILE_ERROR,
        SYNTAX_ERROR,
        FILE_EMPTY,
        ALREADY_IN_LIST
    };

    void findAll();
    StatusCode parse(const QString &provider_name) const;

    const QStringList &providers() const
    { return this->m_providers; }

    static void parseIcon(const QString &input, QString *value, QIcon *icon,
                          const QString &relativePathPrefix = Config()->localProviderStoreDir());

private:
    QStringList m_providers;

    static const char *extension;
    static const char *search_pattern;

    static bool getBoolean(const QString &value);
    static QColor getColor(const QString &value, const QColor &fallback = QColor(0, 0, 0, 0));

    void makeValidPaths();
    QStringList m_validPaths;

    const QString findHighestPriorityProvider(const QString &provider_name) const;
};

#endif // STREAMINGPROVIDERPARSER_HPP
