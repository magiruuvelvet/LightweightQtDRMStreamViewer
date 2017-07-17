#ifndef STREAMINGPROVIDERPARSER_HPP
#define STREAMINGPROVIDERPARSER_HPP

#include <QString>
#include <QStringList>

class StreamingProviderParser
{
public:
    StreamingProviderParser();
    ~StreamingProviderParser();

    void findAll();
    bool parse(const QString &provider_name) const;

    const QStringList &providers() const
    { return this->m_providers; }

private:
    QStringList m_providers;

    static const char *extension;
    static const char *search_pattern;
};

#endif // STREAMINGPROVIDERPARSER_HPP
