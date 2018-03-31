#ifndef STREAMINGPROVIDERPARSER_HPP
#define STREAMINGPROVIDERPARSER_HPP

#include <QString>
#include <QStringList>

class StreamingProviderParser
{
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

private:
    QStringList m_providers;

    static const char *extension;
    static const char *search_pattern;

    void makeValidPaths();
    QStringList m_validPaths;

    const QString findHighestPriorityProvider(const QString &provider_name) const;
};

#endif // STREAMINGPROVIDERPARSER_HPP
