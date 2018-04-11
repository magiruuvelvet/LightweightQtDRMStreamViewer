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
#include <QPushButton>

#include <Core/StreamingProviderStore.hpp>

class ProviderEditWidget : public QWidget
{
    Q_OBJECT

    friend class ConfigWindow;
    friend class FieldIdUserData;
    friend class ButtonIdUserData;

public:
    ProviderEditWidget(QWidget *parent = nullptr);
    ~ProviderEditWidget();

signals:
    void providersUpdated();

public slots:
    void setProvider(Provider *provider);

private:
    Provider provider;
    Provider *provider_ptr = nullptr;

    bool first_start = true;
    bool is_updating = false;
    bool provider_renamed = false;

    QGridLayout *_layout;

    QLineEdit *_id;
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

    QPushButton *m_btnAddProvider;
    QPushButton *m_btnAddUrlInterceptor;
    QPushButton *m_btnRemUrlInterceptor;
    QWidget *btnWrapper;

    enum FieldId {
        ID,
        NAME,
        ICON,
        URL,
        URL_INTERCEPTOR,
        URL_INTERCEPTOR_LINKS,
        SCRIPTS,
        USERAGENT,
        SHOW_TITLEBAR,
        USE_PERMANENT_TITLE,
        PERMANENT_TITLE,
        TITLEBAR_COLOR,
        TITLEBAR_TEXT_COLOR,
    };
    enum ButtonId {
        ADD_PROVIDER,
        ADD_URL_INTERCEPTOR,
        REM_URL_INTERCEPTOR
    };

    void _update();
    void _save();

private slots:
    void string_option_changed(const QString &text);
    void textedit_option_changed();
    void boolean_option_changed(bool);
    void table_option_changed(int row, int column);

    void button_clicked();
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

class FieldIdUserData : public QObjectUserData
{
public:
    FieldIdUserData(const ProviderEditWidget::FieldId &id)
    {
        this->id = id;
    }

    ProviderEditWidget::FieldId id;
};

class ButtonIdUserData : public QObjectUserData
{
public:
    ButtonIdUserData(const ProviderEditWidget::ButtonId &id)
    {
        this->id = id;
    }

    ProviderEditWidget::ButtonId id;
};

#endif // PROVIDEREDITWIDGET_HPP
