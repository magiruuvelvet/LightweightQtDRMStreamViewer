#include "UrlRequestInterceptor.hpp"

#include <QRegExp>
#include <QDebug>

UrlRequestInterceptor::UrlRequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
}

UrlRequestInterceptor::UrlRequestInterceptor(const QList<UrlInterceptorLink> &urlInterceptorLinks,
                                             QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
    this->urlInterceptorLinks = urlInterceptorLinks;
}

void UrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    for (auto&& url : this->urlInterceptorLinks)
    {
        // too spammy, just show matches
        //qDebug() << "[URL Interceptor] Trying: " << url.pattern;
        if (url.pattern.exactMatch(info.requestUrl().toString()))
        {
            qDebug() << "[URL Interceptor] Match! -> " << url.target;
            info.redirect(url.target);
            return;
        }
    }
}
