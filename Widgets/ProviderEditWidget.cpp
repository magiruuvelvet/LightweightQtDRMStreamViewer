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

    static const auto create_label = [](const QString &text) {
        QLabel *l = new QLabel(text);
        l->setStyleSheet("* {color: white;}");
        return l;
    };

    static const auto create_lineedit = [](const QString &placeholder) {
        QLineEdit *le = new QLineEdit();
        le->setPlaceholderText(placeholder);
        le->setToolTip(placeholder);
        le->setStyleSheet(stylesheet);
        return le;
    };

    static const auto create_checkbbox = [](const QString &text) {
        QCheckBox *cb = new QCheckBox(text);
        cb->setStyleSheet("* {color: white;} /*QCheckBox::indicator {background:#444444;}*/");
        return cb;
    };

    static const auto create_tablewidget = [](int rows, int columns) {
        QTableWidget *tw = new QTableWidget(rows, columns);
        tw->setStyleSheet(stylesheet);
        tw->verticalHeader()->hide();
        // FIXME: improve this, see also bottom in _update()
        tw->setHorizontalHeaderLabels({"Pattern", "Target URL"});
        for (auto i = 0; i < columns; i++)
            tw->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        return tw;
    };

    static const auto create_textedit = []{
        QTextEdit *te = new QTextEdit();
        te->setStyleSheet(stylesheet);
        return te;
    };

    this->_name = create_lineedit("Name");
    this->_icon = create_lineedit("Icon");
    this->_url = create_lineedit("URL");
    this->_urlInterceptor = create_checkbbox("URL Interceptor");
    this->_urlInterceptorLinks = create_tablewidget(0, 2);
    this->_scriptsLabel = create_label("Scripts");
    this->_scripts = create_textedit();
    this->_useragent = create_lineedit("User Agent");
    this->_titleBar = create_checkbbox("Show title bar");
    this->_permanentTitleBarText = create_checkbbox("Use permanent title bar text");
    this->_titleBarText = create_lineedit("Permanent title bar text");
    this->_titleBarColors = create_label("Title bar colors");
    this->_titleBarColor = create_lineedit("Title bar color");
    this->_titleBarTextColor = create_lineedit("Title bar text color");

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
    delete _name;
    delete _icon;
    delete _url;
    delete _urlInterceptor;
    delete _urlInterceptorLinks;
    _urlInterceptorLinkItems.clear();
    delete _scriptsLabel;
    delete _scripts;
    delete _useragent;
    delete _titleBar;
    delete _permanentTitleBarText;
    delete _titleBarText;
    delete _titleBarColor;
    delete _titleBarTextColor;
    delete _titleBarColors;
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
    this->_urlInterceptorLinks->setHorizontalHeaderLabels({"Pattern", "Target URL"});
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
