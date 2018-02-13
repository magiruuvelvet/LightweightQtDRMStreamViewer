#include "UrlRequestInterceptor.hpp"

#include <QRegExp>
#include <QDebug>

UrlRequestInterceptor::UrlRequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
}

void UrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
//    if (info.requestUrl().toString().contains("magiruuvelvet.gdn")) {
//        qDebug() << "hijacked url";
//        info.redirect(QUrl("http://localhost"));
//    }

    // Netflix-1080p
    //   ( https://github.com/truedread/netflix-1080p )
    // URLs to hijack:
    //  -> "*://assets.nflxext.com/*/ffe/player/html/*",
    //  -> "*://www.assets.nflxext.com/*/ffe/player/html/*"
    // https://github.com/magiruuvelvet/netflix-1080p/raw/master/cadmium-playercore-5.0008.572.011.js
    static const QRegExp netflix1080p_pattern(".*\:\/\/assets\.nflxext\.com\/.*\/ffe\/player\/html\/.*|"
                                              ".*\:\/\/www\.assets\.nflxext\.com\/.*\/ffe\/player\/html\/.*");

    if (netflix1080p_pattern.exactMatch(info.requestUrl().toString()))
    {
        qDebug() << "Netflix Player detected! Injecting Netflix 1080p Unlocker...";
        info.redirect(QUrl("https://github.com/magiruuvelvet/netflix-1080p/raw/master/cadmium-playercore-5.0008.572.011.js"));
    }
}
