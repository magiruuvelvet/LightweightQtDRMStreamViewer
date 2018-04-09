#include "ProviderEditWidget.hpp"

#include <QHeaderView>

#include <QFile>
#include <QFileInfo>

#include <Gui/ProviderButton.hpp>
#include <Core/StreamingProviderParser.hpp>
#include <Core/StreamingProviderWriter.hpp>

#include "ConfigWindow.hpp"

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

    this->_id = create_lineedit("ID (Filename)", "id");
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

    this->_layout->addWidget(this->_id,                    0, 0, 1, 2);
    this->_layout->addWidget(this->_name,                  1, 0, 1, 2);
    this->_layout->addWidget(this->_icon,                  2, 0, 1, 2);
    this->_layout->addWidget(this->_url,                   3, 0, 1, 2);
    this->_layout->addWidget(this->_urlInterceptor,        4, 0, 1, 2);
    this->_layout->addWidget(this->_urlInterceptorLinks,   5, 0, 1, 2);
    this->_layout->addWidget(this->_scriptsLabel,          6, 0, 1, 2);
    this->_layout->addWidget(this->_scripts,               7, 0, 1, 2);
    this->_layout->addWidget(this->_useragent,             8, 0, 1, 2);
    this->_layout->addWidget(this->_titleBar,              9, 0, 1, 1);
    this->_layout->addWidget(this->_permanentTitleBarText, 9, 1, 1, 1);
    this->_layout->addWidget(this->_titleBarText,         10, 0, 1, 2);
    this->_layout->addWidget(this->_titleBarColors,       11, 0, 1, 2);
    this->_layout->addWidget(this->_titleBarColor,        12, 0, 1, 1);
    this->_layout->addWidget(this->_titleBarTextColor,    12, 1, 1, 1);
    this->setLayout(this->_layout);
}

ProviderEditWidget::~ProviderEditWidget()
{
    _id->disconnect();
    delete _id;
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

    provider_ptr = nullptr;

    qDebug() << "ProviderEditWidget destroyed!";
}

void ProviderEditWidget::setProvider(Provider *provider)
{
    if (!provider)
        return;

    // create copy of object for modifications
    this->provider = *provider;

    // pointer to real object (for saving later)
    this->provider_ptr = provider;

    this->_update();
    first_start = false;
}

void ProviderEditWidget::_update()
{
    is_updating = true;

    this->_id->setText(provider.id);
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

    is_updating = false;
}

void ProviderEditWidget::_save()
{
    // saving code works, but is disabled due to a bug in StreamingProviderWriter
    // removing scripts and url interceptors doesn't work and crashes the program
    if (true)
        return;

    qDebug() << "Saving" << this->provider_ptr->id << "...";
    if (provider_renamed)
        qDebug() << "Hint: Provider was renamed to" << this->provider.id;

    // check if all required fields are there
    if (this->provider.id.isEmpty() ||
        this->provider.name.isEmpty() ||
        this->provider.url.isEmpty())
    {
        qDebug() << "Error: 'id', 'name' and 'icon' are required fields and must not be empty! Canceling save...";
        return;
    }

    // routine for config file renaming on id change
    if (provider_renamed)
    {
        // check for id conflict
        if (StreamingProviderStore::instance()->contains(this->provider.id))
        {
            qDebug() << "Error: ID" << this->provider.id << "is already taken! Canceling save...";
            return;
        }

        // rename file on disk
        const auto old_filename = Config()->localProviderStoreDir() + '/' + this->provider_ptr->id + StreamingProviderParser::extension;
        const auto new_filename = Config()->localProviderStoreDir() + '/' + this->provider.id + StreamingProviderParser::extension;

        QFileInfo old_info(old_filename);
        if (old_info.exists())
        {
            if (QFile(old_filename).rename(new_filename))
            {
                qDebug() << "Renamed config file.";
            }
            else
            {
                qDebug() << "Couldn't rename config file! Check your permissions! Canceling save...";
                return;
            }
        }
        else
        {
            // assume system config override
        }

        // reset boolean
        provider_renamed = false;
    }

    // write back modified provider to its file
    StreamingProviderWriter::StatusCode status = StreamingProviderWriter::write(this->provider);
    switch (status)
    {
        case StreamingProviderWriter::SUCCESS:
            // modify provider in memory on success
            (*this->provider_ptr) = this->provider;
            //static_cast<ProviderListModel*>(static_cast<ConfigWindow*>(this->parentWidget())->m_tblView->model())->reload();
            qDebug() << "Successfully saved" << this->provider_ptr->id;
            break;
        case StreamingProviderWriter::PERM_ERROR:
            qDebug() << "A permission error occurred! Please check if you have write permissions.";
            break;
        case StreamingProviderWriter::FILE_ERROR:
            qDebug() << "An unknown I/O error occurred!";
            break;
    }
}

void ProviderEditWidget::string_option_changed(const QString &)
{
    if (!first_start && !is_updating)
    {
        const auto option = this->sender()->objectName();

        if (option == "id")
        {
            provider_renamed = provider.id != _id->text() ? true : false;
            provider.id = _id->text();
        }
        else if (option == "name")
        { provider.name = _name->text(); }
        else if (option == "icon")
        { StreamingProviderParser::parseIcon(_icon->text(), &provider.icon.value, &provider.icon.icon); }
        else if (option == "url")
        { provider.url = QUrl(_url->text()); }
        else if (option == "useragent")
        { provider.useragent = _useragent->text(); }
        else if (option == "permanent_title")
        { if (provider.titleBarHasPermanentTitle) provider.titleBarPermanentTitle = _titleBarText->text(); }
        else if (option == "titlebar_color")
        { if (provider.titleBarVisible) provider.titleBarColor = QColor(_titleBarColor->text()); }
        else if (option == "titlebar_text_color")
        { if (provider.titleBarVisible) provider.titleBarTextColor = QColor(_titleBarTextColor->text()); }
    }
}

void ProviderEditWidget::textedit_option_changed()
{
    if (!first_start && !is_updating)
    {
        const auto option = this->sender()->objectName();

        if (option == "scripts")
        {
            const auto lines = _scripts->toPlainText().split(QRegExp("[\n\r]"), QString::SkipEmptyParts);
            QList<Script> scripts;
            for (auto&& line : lines)
                scripts.append(Script::parse(line.simplified()));
            provider.scripts.clear();
            provider.scripts = scripts;
        }
    }
}

void ProviderEditWidget::boolean_option_changed(bool)
{
    if (!first_start && !is_updating)
    {
        const auto option = this->sender()->objectName();

        if (option == "url_interceptor")
        { provider.urlInterceptor = _urlInterceptor->isChecked(); }
        else if (option == "show_titlebar")
        { provider.titleBarVisible = _titleBar->isChecked(); }
        else if (option == "use_permanent_title")
        { provider.titleBarHasPermanentTitle = _permanentTitleBarText->isChecked(); }
    }
}

void ProviderEditWidget::table_option_changed(int row, int column)
{
    if (!first_start && !is_updating)
    {
        const auto option = this->sender()->objectName();

        if (option == "url_interceptor_links")
        {
            if (column == 0) // pattern
                provider.urlInterceptorLinks[row].pattern = QRegExp(_urlInterceptorLinks->item(row, 0)->text());
            else if (column == 1) // url target
                provider.urlInterceptorLinks[row].target = QUrl(_urlInterceptorLinks->item(row, 1)->text());
        }
    }
}
