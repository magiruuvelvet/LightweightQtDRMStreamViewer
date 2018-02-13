#ifndef URLREQUESTINTERCEPTOR_HPP
#define URLREQUESTINTERCEPTOR_HPP

#include <QWebEngineUrlRequestInterceptor>

class UrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    UrlRequestInterceptor(QObject *parent = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};

#endif // URLREQUESTINTERCEPTOR_HPP
