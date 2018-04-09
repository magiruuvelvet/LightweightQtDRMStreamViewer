#ifndef PROVIDEREDITWIDGET_HPP
#define PROVIDEREDITWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <Core/StreamingProviderStore.hpp>

class ProviderEditWidget : public QWidget
{
    Q_OBJECT

public:
    ProviderEditWidget(QWidget *parent = nullptr);
    ~ProviderEditWidget();

public slots:
    void setProvider(const Provider &provider);

private:
    Provider provider;

    QGridLayout *_layout;

    QLineEdit *_name;
    QLineEdit *_icon;
    QLineEdit *_url;
    QCheckBox *_urlInterceptor;
    QTableWidget *_urlInterceptorLinks;
        QList<QTableWidgetItem*> _urlInterceptorLinkItems;
    QTextEdit *_scripts;
        QLabel *_scriptsLabel;
    QLineEdit *_useragent;
    QCheckBox *_titleBar;
    QCheckBox *_permanentTitleBarText;
    QLineEdit *_titleBarText;
    QLineEdit *_titleBarColor;
    QLineEdit *_titleBarTextColor;
        QLabel *_titleBarColors;

    void _update();
};

class TableWidgetUserData : public QObjectUserData
{
public:
    TableWidgetUserData(const QStringList &headerData)
    {
        this->headerData = headerData;
    }
    ~TableWidgetUserData() override
    {
        this->headerData.clear();
    }

    QStringList headerData;
};

#endif // PROVIDEREDITWIDGET_HPP
