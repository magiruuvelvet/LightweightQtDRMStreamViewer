#include "ProviderEditWidget.hpp"

#include <QHeaderView>

#include <QFile>
#include <QFileInfo>

#include <Gui/ProviderButton.hpp>
#include <Core/StreamingProviderParser.hpp>
#include <Core/StreamingProviderWriter.hpp>
#include <Util/RandomString.hpp>

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

    const auto create_lineedit = [&](const QString &placeholder, const FieldId &id) {
        QLineEdit *le = new QLineEdit();
        le->setProperty("0", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new FieldIdUserData(id))));
        le->setPlaceholderText(placeholder);
        le->setToolTip(placeholder);
        le->setStyleSheet(stylesheet);

        QObject::connect(le, &QLineEdit::textChanged, this, &ProviderEditWidget::string_option_changed);
        return le;
    };

    const auto create_checkbbox = [&](const QString &text, const FieldId &id) {
        QCheckBox *cb = new QCheckBox(text);
        cb->setProperty("0", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new FieldIdUserData(id))));
        cb->setStyleSheet("* {color: white;} /*QCheckBox::indicator {background:#444444;}*/");

        QObject::connect(cb, &QCheckBox::toggled, this, &ProviderEditWidget::boolean_option_changed);
        return cb;
    };

    const auto create_tablewidget = [&](int rows, int columns,
                                        const QStringList &rowNames,
                                        const QStringList &columNames,
                                        //const FieldId &id,
                                        const QString &objectName,
                                        bool hideRows = false, bool hideColums = false) {
        QTableWidget *tw = new QTableWidget(rows, columns);
        //tw->setUserData(0, new FieldIdUserData(id));
        tw->setObjectName(objectName);
        tw->setStyleSheet(stylesheet);
        if (hideRows)
            tw->verticalHeader()->hide();
        if (hideColums)
            tw->horizontalHeader()->hide();
        tw->setProperty("0", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new TableWidgetUserData(rowNames))));
        tw->setProperty("1", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new TableWidgetUserData(columNames))));
        tw->setVerticalHeaderLabels(reinterpret_cast<TableWidgetUserData*>(tw->property("0").value<std::uintptr_t>())->headerData);
        tw->setHorizontalHeaderLabels(reinterpret_cast<TableWidgetUserData*>(tw->property("1").value<std::uintptr_t>())->headerData);
        for (auto i = 0; i < columns; i++)
            tw->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

        QObject::connect(tw, &QTableWidget::cellChanged, this, &ProviderEditWidget::table_option_changed);
        return tw;
    };

    const auto create_textedit = [&](const FieldId &id) {
        QTextEdit *te = new QTextEdit();
        te->setProperty("0", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new FieldIdUserData(id))));
        te->setStyleSheet(stylesheet);

        QObject::connect(te, &QTextEdit::textChanged, this, &ProviderEditWidget::textedit_option_changed);
        return te;
    };

    this->_id = create_lineedit("ID (Filename)", ID);
    this->_name = create_lineedit("Name", NAME);
    this->_icon = create_lineedit("Icon", ICON);
    this->_url = create_lineedit("URL", URL);
    this->_urlInterceptor = create_checkbbox("URL Interceptor", URL_INTERCEPTOR);
    this->_urlInterceptorLinks = create_tablewidget(0, 2, {}, {"Pattern", "Target URL"}, "URL_INTERCEPTOR_LINKS", true, false);
    this->_scriptsLabel = create_label("Scripts");
    this->_scripts = create_textedit(SCRIPTS);
    this->_useragent = create_lineedit("User Agent", USERAGENT);
    this->_titleBar = create_checkbbox("Show title bar", SHOW_TITLEBAR);
    this->_permanentTitleBarText = create_checkbbox("Use permanent title bar text", USE_PERMANENT_TITLE);
    this->_titleBarText = create_lineedit("Permanent title bar text", PERMANENT_TITLE);
    this->_titleBarColors = create_label("Title bar colors");
    this->_titleBarColor = create_lineedit("Title bar color", TITLEBAR_COLOR);
    this->_titleBarTextColor = create_lineedit("Title bar text color", TITLEBAR_TEXT_COLOR);

    const auto create_button = [&](const QString &text, const QString &toolTip, const QSize &size, const ButtonId &id) {
        QPushButton *btn = new QPushButton(text);
        btn->setToolTip(toolTip);
        btn->setProperty("0", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(new ButtonIdUserData(id))));
        btn->setFlat(true);
        btn->setStyleSheet("* {color: white; background-color: #444444;}");

        if (!size.isNull())
            btn->setFixedSize(size);

        QObject::connect(btn, &QPushButton::clicked, this, &ProviderEditWidget::button_clicked);
        return btn;
    };

    this->m_btnAddProvider = create_button("+", "Add Provider", QSize(20, 20), ADD_PROVIDER);
    this->m_btnRemProvider = create_button("─", "Remove Provider", QSize(20, 20), REM_PROVIDER);
    this->m_btnAddUrlInterceptor = create_button("+", "Add URL Interceptor", QSize(20, 20), ADD_URL_INTERCEPTOR);
    this->m_btnRemUrlInterceptor = create_button("─", "Remove URL Interceptor", QSize(20, 20), REM_URL_INTERCEPTOR);

    const auto create_button_wrapper_box = [&](QList<QPushButton*> buttons) {
        QWidget *wrapper = new QWidget();
        wrapper->setFixedSize(45, 20);
        wrapper->setContentsMargins(0,0,0,0);
        for (auto&& btn : buttons)
            btn->setParent(wrapper);
        buttons[0]->setGeometry(25, 0, buttons.at(0)->width(), buttons.at(0)->height());
        buttons[1]->setGeometry(0, 0, buttons.at(1)->width(), buttons.at(1)->height());
        return wrapper;
    };

    this->btnWrapperProviders = create_button_wrapper_box({this->m_btnAddProvider, this->m_btnRemProvider});
    this->btnWrapperUrlInterceptors = create_button_wrapper_box({this->m_btnAddUrlInterceptor, this->m_btnRemUrlInterceptor});

    this->_layout->addWidget(this->btnWrapperProviders,       0, 0, 1, 6, Qt::AlignRight);
    this->_layout->addWidget(this->_id,                       1, 0, 1, 6);
    this->_layout->addWidget(this->_name,                     2, 0, 1, 6);
    this->_layout->addWidget(this->_icon,                     3, 0, 1, 6);
    this->_layout->addWidget(this->_url,                      4, 0, 1, 6);
    this->_layout->addWidget(this->_urlInterceptor,           5, 0, 1, 5);
    this->_layout->addWidget(this->btnWrapperUrlInterceptors, 5, 5, 1, 1, Qt::AlignRight);
    this->_layout->addWidget(this->_urlInterceptorLinks,      6, 0, 1, 6);
    this->_layout->addWidget(this->_scriptsLabel,             7, 0, 1, 6);
    this->_layout->addWidget(this->_scripts,                  8, 0, 1, 6);
    this->_layout->addWidget(this->_useragent,                9, 0, 1, 6);
    this->_layout->addWidget(this->_titleBar,                10, 0, 1, 3);
    this->_layout->addWidget(this->_permanentTitleBarText,   10, 3, 1, 3);
    this->_layout->addWidget(this->_titleBarText,            11, 0, 1, 6);
    this->_layout->addWidget(this->_titleBarColors,          12, 0, 1, 6);
    this->_layout->addWidget(this->_titleBarColor,           13, 0, 1, 3);
    this->_layout->addWidget(this->_titleBarTextColor,       13, 3, 1, 3);
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

    m_btnAddProvider->disconnect();
    delete m_btnAddProvider;
    m_btnRemProvider->disconnect();
    delete m_btnRemProvider;
    m_btnAddUrlInterceptor->disconnect();
    delete m_btnAddUrlInterceptor;
    m_btnRemUrlInterceptor->disconnect();
    delete m_btnRemUrlInterceptor;

    delete btnWrapperProviders;
    delete btnWrapperUrlInterceptors;

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
        reinterpret_cast<TableWidgetUserData*>(this->_urlInterceptorLinks->property("1").value<std::uintptr_t>())->headerData);
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
        if (StreamingProviderStore::instance()->contains(this->provider.id) &&
            !this->provider.isSystem)
        {
            qDebug() << "Error: ID" << this->provider.id << "is already taken! Canceling save...";
            return;
        }
        if (this->provider.isSystem)
        {
            qDebug() << "Hint: overriding shared (system-installed) provider file.";
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
            emit providersUpdated();
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
        const auto option = reinterpret_cast<FieldIdUserData*>(this->sender()->property("0").value<std::uintptr_t>())->id;

        if (option == ID)
        {
            provider_renamed = provider.id != _id->text() ? true : false;
            provider.id = _id->text();
        }
        else if (option == NAME)
        { provider.name = _name->text(); }
        else if (option == ICON)
        { StreamingProviderParser::parseIcon(_icon->text(), &provider.icon.value, &provider.icon.icon); }
        else if (option == URL)
        { provider.url = QUrl(_url->text()); }
        else if (option == USERAGENT)
        { provider.useragent = _useragent->text(); }
        else if (option == PERMANENT_TITLE)
        { if (provider.titleBarHasPermanentTitle) provider.titleBarPermanentTitle = _titleBarText->text(); }
        else if (option == TITLEBAR_COLOR)
        { if (provider.titleBarVisible) provider.titleBarColor = QColor(_titleBarColor->text()); }
        else if (option == TITLEBAR_TEXT_COLOR)
        { if (provider.titleBarVisible) provider.titleBarTextColor = QColor(_titleBarTextColor->text()); }
    }
}

void ProviderEditWidget::textedit_option_changed()
{
    if (!first_start && !is_updating)
    {
        const auto option = reinterpret_cast<FieldIdUserData*>(this->sender()->property("0").value<std::uintptr_t>())->id;

        if (option == SCRIPTS)
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
        const auto option = reinterpret_cast<FieldIdUserData*>(this->sender()->property("0").value<std::uintptr_t>())->id;

        if (option == URL_INTERCEPTOR)
        { provider.urlInterceptor = _urlInterceptor->isChecked(); }
        else if (option == SHOW_TITLEBAR)
        { provider.titleBarVisible = _titleBar->isChecked(); }
        else if (option == USE_PERMANENT_TITLE)
        { provider.titleBarHasPermanentTitle = _permanentTitleBarText->isChecked(); }
    }
}

void ProviderEditWidget::table_option_changed(int row, int column)
{
    if (!first_start && !is_updating)
    {
        // wrong enum value, in fact there is no enum value at all
        // table widget is a bit more complex about user data as it seems
        //const auto option = static_cast<FieldIdUserData*>(this->sender()->userData(0))->id;
        const auto option = this->sender()->objectName();

        if (option == "URL_INTERCEPTOR_LINKS")
        {
            if (column == 0) // pattern
                provider.urlInterceptorLinks[row].pattern = QRegExp(_urlInterceptorLinks->item(row, 0)->text());
            else if (column == 1) // url target
                provider.urlInterceptorLinks[row].target = QUrl(_urlInterceptorLinks->item(row, 1)->text());
        }
    }
}

void ProviderEditWidget::button_clicked()
{
    if (!first_start && !is_updating)
    {
        const auto option = reinterpret_cast<ButtonIdUserData*>(this->sender()->property("0").value<std::uintptr_t>())->id;

        if (option == REM_PROVIDER)
        {
            // no provider selected
            if (!this->provider_ptr)
                return;

            const auto filename = Config()->localProviderStoreDir() + '/' + this->provider_ptr->id + StreamingProviderParser::extension;
            QFileInfo info(filename);
            if (info.exists())
            {
                if (QFile(filename).remove())
                {
                    qDebug() << "Removed provider!";
                    StreamingProviderStore::instance()->removeProvider(this->provider_ptr->id);
                    emit providersUpdated();
                }
                else
                {
                    qDebug() << "Deletion of provider failed!";
                }
            }
            else
            {
                qDebug() << "Hint: Provider seems to be a system-installed one.";
                qDebug() << "Hiding of system-installed providers is not supported!";
            }
        }
        else if (option == ADD_URL_INTERCEPTOR)
        {
            _urlInterceptorLinks->insertRow(_urlInterceptorLinks->rowCount());
            provider.urlInterceptorLinks.append(UrlInterceptorLink{});
        }
        else if (option == REM_URL_INTERCEPTOR)
        {
            const auto selectedItems = _urlInterceptorLinks->selectedItems();
            QList<int> toRemove;

            qDebug() << selectedItems;

            // avoid segfault
            if (selectedItems.size() != 0)
            {
                for (auto&& i : selectedItems)
                    if (!toRemove.contains(i->row()))
                        toRemove.append(i->row());

                for (auto&& i : toRemove)
                {
                    _urlInterceptorLinks->removeRow(i);
                    provider.urlInterceptorLinks.removeAt(i);
                }
            }
        }
    }

    // allow adding providers on dialog first start
    else
    {
        const auto option = reinterpret_cast<ButtonIdUserData*>(this->sender()->property("0").value<std::uintptr_t>())->id;

        if (option == ADD_PROVIDER)
        {
            // init new Provider with required options
            Provider p;
            p.id = "new-" + RandomString::Hex(4);
            p.name = "New Provider";
            StreamingProviderStore::instance()->addProvider(p);
            emit providersUpdated();
        }
    }
}
