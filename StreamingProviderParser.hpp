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
    QString m_configPath;
    QStringList m_providers;
};

#endif // STREAMINGPROVIDERPARSER_HPP
