#ifndef URLREQUESTINTERCEPTOR_HPP
#define URLREQUESTINTERCEPTOR_HPP

#include <QWebEngineUrlRequestInterceptor>
#include <Core/StreamingProviderStore.hpp>

class UrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    UrlRequestInterceptor(QObject *parent = nullptr);
    UrlRequestInterceptor(const QList<UrlInterceptorLink> &urlInterceptorLinks, const QString &httpAcceptLanguage, QObject *parnet = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

private:
    QList<UrlInterceptorLink> urlInterceptorLinks;
    QString httpAcceptLanguage;
};

#endif // URLREQUESTINTERCEPTOR_HPP
