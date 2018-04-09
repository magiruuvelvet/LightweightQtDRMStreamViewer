#include "ProviderEditWidget.hpp"

#include <QHeaderView>

ProviderEditWidget::ProviderEditWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setContentsMargins(0,0,0,0);
    this->_layout = new QGridLayout();
    this->_layout->setContentsMargins(0,0,0,0);
    this->_layout->setSpacing(1);

    static const auto stylesheet = "* {color: white; background-color: #444444;}";

    // Notes for others
    // don't make this lambda expressions `static`
    // signals are connected in there, which causes a segfault
    // on repeated execution

    const auto create_label = [](const QString &text) {
        QLabel *l = new QLabel(text);
        l->setStyleSheet("* {color: white;}");
        return l;
    };

    const auto create_lineedit = [&](const QString &placeholder, const QString &objectName) {
        QLineEdit *le = new QLineEdit();
        le->setObjectName(objectName);
        le->setPlaceholderText(placeholder);
        le->setToolTip(placeholder);
        le->setStyleSheet(stylesheet);

        QObject::connect(le, &QLineEdit::textChanged, this, &ProviderEditWidget::string_option_changed);
        return le;
    };

    const auto create_checkbbox = [&](const QString &text, const QString &objectName) {
        QCheckBox *cb = new QCheckBox(text);
        cb->setObjectName(objectName);
        cb->setStyleSheet("* {color: white;} /*QCheckBox::indicator {background:#444444;}*/");

        QObject::connect(cb, &QCheckBox::toggled, this, &ProviderEditWidget::boolean_option_changed);
        return cb;
    };

    const auto create_tablewidget = [&](int rows, int columns,
                                        const QStringList &rowNames,
                                        const QStringList &columNames,
                                        const QString &objectName,
                                        bool hideRows = false, bool hideColums = false) {
        QTableWidget *tw = new QTableWidget(rows, columns);
        tw->setObjectName(objectName);
        tw->setStyleSheet(stylesheet);
        if (hideRows)
            tw->verticalHeader()->hide();
        if (hideColums)
            tw->horizontalHeader()->hide();
        tw->setUserData(0, new TableWidgetUserData(rowNames));
        tw->setUserData(1, new TableWidgetUserData(columNames));
        tw->setVerticalHeaderLabels(static_cast<TableWidgetUserData*>(tw->userData(0))->headerData);
        tw->setHorizontalHeaderLabels(static_cast<TableWidgetUserData*>(tw->userData(1))->headerData);
        for (auto i = 0; i < columns; i++)
            tw->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

        QObject::connect(tw, &QTableWidget::cellChanged, this, &ProviderEditWidget::table_option_changed);
        return tw;
    };

    const auto create_textedit = [&](const QString &objectName) {
        QTextEdit *te = new QTextEdit();
        te->setObjectName(objectName);
        te->setStyleSheet(stylesheet);

        QObject::connect(te, &QTextEdit::textChanged, this, &ProviderEditWidget::textedit_option_changed);
        return te;
    };

    this->_name = create_lineedit("Name", "name");
    this->_icon = create_lineedit("Icon", "icon");
    this->_url = create_lineedit("URL", "url");
    this->_urlInterceptor = create_checkbbox("URL Interceptor", "url_interceptor");
    this->_urlInterceptorLinks = create_tablewidget(0, 2, {}, {"Pattern", "Target URL"}, "url_interceptor_links", true, false);
    this->_scriptsLabel = create_label("Scripts");
    this->_scripts = create_textedit("scripts");
    this->_useragent = create_lineedit("User Agent", "useragent");
    this->_titleBar = create_checkbbox("Show title bar", "show_titlebar");
    this->_permanentTitleBarText = create_checkbbox("Use permanent title bar text", "use_permanent_title");
    this->_titleBarText = create_lineedit("Permanent title bar text", "permanent_title");
    this->_titleBarColors = create_label("Title bar colors");
    this->_titleBarColor = create_lineedit("Title bar color", "titlebar_color");
    this->_titleBarTextColor = create_lineedit("Title bar text color", "titlebar_text_color");

    this->_layout->addWidget(this->_name,                  0, 0, 1, 2);
    this->_layout->addWidget(this->_icon,                  1, 0, 1, 2);
    this->_layout->addWidget(this->_url,                   2, 0, 1, 2);
    this->_layout->addWidget(this->_urlInterceptor,        3, 0, 1, 2);
    this->_layout->addWidget(this->_urlInterceptorLinks,   4, 0, 1, 2);
    this->_layout->addWidget(this->_scriptsLabel,          5, 0, 1, 2);
    this->_layout->addWidget(this->_scripts,               6, 0, 1, 2);
    this->_layout->addWidget(this->_useragent,             7, 0, 1, 2);
    this->_layout->addWidget(this->_titleBar,              8, 0, 1, 1);
    this->_layout->addWidget(this->_permanentTitleBarText, 8, 1, 1, 1);
    this->_layout->addWidget(this->_titleBarText,          9, 0, 1, 2);
    this->_layout->addWidget(this->_titleBarColors,       10, 0, 1, 2);
    this->_layout->addWidget(this->_titleBarColor,        11, 0, 1, 1);
    this->_layout->addWidget(this->_titleBarTextColor,    11, 1, 1, 1);
    this->setLayout(this->_layout);
}

ProviderEditWidget::~ProviderEditWidget()
{
    _name->disconnect();
    delete _name;
    _icon->disconnect();
    delete _icon;
    _url->disconnect();
    delete _url;
    _urlInterceptor->disconnect();
    delete _urlInterceptor;
    _urlInterceptorLinks->disconnect();
    delete _urlInterceptorLinks;
    _urlInterceptorLinkItems.clear();
    delete _scriptsLabel;
    _scripts->disconnect();
    delete _scripts;
    _useragent->disconnect();
    delete _useragent;
    _titleBar->disconnect();
    delete _titleBar;
    _permanentTitleBarText->disconnect();
    delete _permanentTitleBarText;
    _titleBarText->disconnect();
    delete _titleBarText;
    _titleBarColor->disconnect();
    delete _titleBarColor;
    _titleBarTextColor->disconnect();
    delete _titleBarTextColor;
    delete _titleBarColors;

    qDebug() << "ProviderEditWidget destroyed!";
}

void ProviderEditWidget::setProvider(const Provider &provider)
{
    this->provider = provider;
    this->_update();
}

void ProviderEditWidget::_update()
{
    this->_name->setText(provider.name);
    this->_icon->setText(provider.icon.value);
    this->_url->setText(provider.url.toString());
    this->_urlInterceptor->setChecked(provider.urlInterceptor);

    this->_urlInterceptorLinks->clear();
    this->_urlInterceptorLinks->setRowCount(0);
    this->_urlInterceptorLinks->setHorizontalHeaderLabels(
        static_cast<TableWidgetUserData*>(this->_urlInterceptorLinks->userData(1))->headerData);
    this->_urlInterceptorLinkItems.clear();

    for (auto&& i : provider.urlInterceptorLinks)
    {
        const auto row = this->_urlInterceptorLinks->rowCount();
        this->_urlInterceptorLinks->insertRow(row);

        auto pattern = new QTableWidgetItem(i.pattern.pattern());
        this->_urlInterceptorLinkItems.append(pattern);

        auto target = new QTableWidgetItem(i.target.toString());
        this->_urlInterceptorLinkItems.append(target);

        this->_urlInterceptorLinks->setItem(row, 0, pattern);
        this->_urlInterceptorLinks->setItem(row, 1, target);
    }

    this->_scripts->clear();
    for (auto&& i : provider.scripts)
        this->_scripts->append(i);

    this->_useragent->setText(provider.useragent);

    this->_titleBar->setChecked(provider.titleBarVisible);
    this->_permanentTitleBarText->setChecked(provider.titleBarHasPermanentTitle);
    this->_titleBarText->setText(provider.titleBarPermanentTitle);
    this->_titleBarColor->setText(provider.titleBarColor.name(QColor::HexRgb));
    this->_titleBarTextColor->setText(provider.titleBarTextColor.name(QColor::HexRgb));
}

void ProviderEditWidget::string_option_changed(const QString &text)
{
    QObject *sender = this->sender();
    qDebug() << sender->objectName() << text;
}

void ProviderEditWidget::textedit_option_changed()
{
    QObject *sender = this->sender();
    qDebug() << sender->objectName();
}

void ProviderEditWidget::boolean_option_changed(bool checked)
{
    QObject *sender = this->sender();
    qDebug() << sender->objectName() << checked;
}

void ProviderEditWidget::table_option_changed(int row, int column)
{
    QObject *sender = this->sender();
    qDebug() << sender->objectName() << '('<<row<<','<<column<<')';
}
