#include "BrowserWindow.hpp"

#include <QApplication>
#include <QDesktopWidget>

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>

#include <Util/UserAgent.hpp>

BrowserWindow::BrowserWindow(QWidget *parent)
    : BaseWindow(parent)
{
    this->setWindowFlags(this->windowFlags() |
                         Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QRect desktopSize = QApplication::desktop()->screenGeometry(this);
    this->resize(desktopSize.width() / 1.2, (desktopSize.height() / 1.2) + 31);
    this->move(desktopSize.width() / 2 - this->size().width() / 2, desktopSize.height() / 2 - this->size().height() / 2);

    this->installEventFilter(this);

    this->m_layout = new QVBoxLayout();
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setMargin(0);
    this->m_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->setContainerLayoutContentsMargins(0,0,0,0);

    this->emergencyAddressBar = new QLineEdit();
    this->emergencyAddressBar->hide();
    QObject::connect(this->emergencyAddressBar, &QLineEdit::editingFinished, this, [&]{
        this->setUrl(QUrl(this->emergencyAddressBar->text()));
    });

    QObject::connect(this, &BrowserWindow::urlChanged, this, [&](const QUrl &url){
        this->emergencyAddressBar->setText(url.toString());
    });

    this->m_layout->addWidget(this->emergencyAddressBar);
    new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(toggleAddressBarVisibility()));

    this->webView = new QWebEngineView();
    this->webView->setContextMenuPolicy(Qt::NoContextMenu);
    this->m_layout->addWidget(this->webView);
    this->containerWidget()->setLayout(this->m_layout);

    QObject::connect(this->webView, &QWebEngineView::titleChanged, this, &BrowserWindow::setWindowTitle);
    QObject::connect(this->webView, &QWebEngineView::loadProgress, this, &BrowserWindow::onLoadProgress);
    QObject::connect(this->webView, &QWebEngineView::loadFinished, this, &BrowserWindow::onLoadFinished);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this, SLOT(toggleFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    new QShortcut(QKeySequence(Qt::Key_F5), this->webView, SLOT(reload()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F5), this, SLOT(forceReload()));

    // Backup current user-agent
    this->m_originalUserAgent = this->webView->page()->profile()->httpUserAgent();

    // Inject app name and version into the default Qt Web Engine user agent
    this->restoreUserAgent();

    // allow PepperFlash to toggle fullscreen (Netflix, Amazon Video, etc.)
    this->webView->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    QObject::connect(this->webView->page(), &QWebEnginePage::fullScreenRequested, this, &BrowserWindow::acceptFullScreen);

    // other WebEngine settings
    this->webView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    this->webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    this->webView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);

    // load scripts for injection
    this->loadEmbeddedScript(this->mJs_hideScrollBars, "hide-scrollbars", true);

    // a much better way to inject scripts
    // make use of this when implementing a proper customizable js injector
    // the scrollbar remover script is the only one for now
    this->scripts = this->webView->page()->profile()->scripts();
    QWebEngineScript js_hideScrollBars;
    js_hideScrollBars.setName("hide-scrollbars");
    js_hideScrollBars.setInjectionPoint(QWebEngineScript::DocumentReady);
    js_hideScrollBars.setSourceCode(this->mJs_hideScrollBars);
    this->scripts->insert(js_hideScrollBars);
}

QWebEngineScript BrowserWindow::loadScript(const QString &filename, Script::InjectionPoint injection_pt)
{
    // check if script name is a relative or absolute path
    // on relative, its relative to the provider store directory
    QFile file;
    if (QFileInfo(filename).isAbsolute())
        file.setFileName(filename);
    else
        file.setFileName(this->providerPath + '/' + filename);

    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString target = file.readAll();
        file.close();

        QWebEngineScript script;
        script.setName(filename);
        switch (injection_pt)
        {
            case Script::Deferred:          script.setInjectionPoint(QWebEngineScript::Deferred); break;
            case Script::DocumentReady:     script.setInjectionPoint(QWebEngineScript::DocumentReady); break;
            case Script::DocumentCreation:  script.setInjectionPoint(QWebEngineScript::DocumentCreation); break;
            case Script::Automatic:         break;
        }
        script.setSourceCode(target);

        qDebug() << "Loaded script" << file.fileName();
        return script;
    }
    else
    {
        qDebug() << "Error loading script" << file.fileName();
        return QWebEngineScript(); // return null script
    }
}

void BrowserWindow::loadEmbeddedScript(QString &target, const QString &filename, bool compressed)
{
    if (compressed)
    {
        QFile file(":/" + filename + ".js.qgz");
        if (file.open(QFile::ReadOnly))
        {
            target = qUncompress(file.readAll());
            file.close();
            qDebug() << "Loaded compressed embedded script" << file.fileName();
        }
        else
        {
            qDebug() << "Error loading compressed embedded script" << file.fileName();
        }
    }
    else
    {
        QFile file(":/" + filename + ".js");
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            target = file.readAll();
            file.close();
            qDebug() << "Loaded embedded script" << file.fileName();
        }
        else
        {
            qDebug() << "Error loading embedded script" << file.fileName();
        }
    }
}

BrowserWindow *BrowserWindow::getInstance()
{
    static BrowserWindow *instance = ([]{
        BrowserWindow *i = new BrowserWindow();
        i->setWindowModality(Qt::ApplicationModal);
        StreamingProviderStore::resetProfile(i);
        return i;
    })();

    return instance;
}

BrowserWindow::~BrowserWindow()
{
    // delete browser window properties
    this->m_baseTitle.clear();
    this->m_cookieStoreId.clear();
    this->m_engineProfilePath.clear();
    this->providerPath.clear();

    // delete injected scripts
    this->mJs_hideScrollBars.clear();
    this->m_scripts.clear();
    this->scripts->clear();
    //delete scripts;

    // delete url interceptor
    delete m_interceptor;

    // delete address bar
    delete emergencyAddressBar;

    // delete browser cookie store
    this->m_cookies.clear();
    //delete m_cookieStore;

    // destory the web view
    delete webView;

    // delete layout
    delete m_layout;

    qDebug() << "BrowserWindow destroyed";
}

void BrowserWindow::setProfile(const Provider &profile)
{
    StreamingProviderStore::loadProfile(this, profile);
}

void BrowserWindow::resetProfile()
{
    this->webView->stop();
    StreamingProviderStore::resetProfile(this);
}

void BrowserWindow::show()
{
    QWidget::show();
    this->titleBar()->setVisible(this->m_titleBarVisibility);
}

void BrowserWindow::showNormal()
{
    QWidget::showNormal();

    if (this->m_titleBarVisibility)
    {
        this->m_titleBarVisibilityToggle = true;
        this->titleBar()->setVisible(this->m_titleBarVisibilityToggle);
    }
}

void BrowserWindow::showFullScreen()
{
    QWidget::showFullScreen();

    if (this->m_titleBarVisibility)
    {
        this->m_titleBarVisibilityToggle = false;
        this->titleBar()->setVisible(this->m_titleBarVisibilityToggle);
    }
}

void BrowserWindow::setWindowTitle(const QString &title)
{
    if (!this->m_permanentTitle)
    {
        QWidget::setWindowTitle(title + QString::fromUtf8(" ─ ") + this->m_baseTitle);
        this->titleBar()->setTitle(QWidget::windowTitle());
    }
}

void BrowserWindow::setWindowIcon(const QIcon &icon)
{
    if (icon.isNull())
    {
        QWidget::setWindowIcon(qApp->windowIcon());
        this->titleBar()->setIcon(qApp->windowIcon().pixmap(23, 23, QIcon::Normal, QIcon::On));
    }
    else
    {
        QWidget::setWindowIcon(icon);
        this->titleBar()->setIcon(icon.pixmap(23, 23, QIcon::Normal, QIcon::On));
    }
}

void BrowserWindow::setTitleBarVisibility(bool visible)
{
    const QRect desktopSize = QApplication::desktop()->screenGeometry(this);
    this->m_titleBarVisibility = visible;

    if (this->m_titleBarVisibility)
    {
        this->resize(desktopSize.width() / 1.2, (desktopSize.height() / 1.2) + 31);
        this->titleBar()->setVisible(true);
    }
    else
    {
        this->resize(desktopSize.width() / 1.2, desktopSize.height() / 1.2);
        this->titleBar()->setVisible(false);
    }
}

void BrowserWindow::setTitleBarColor(const QColor &color, const QColor &textColor)
{
    QPalette titleBarScheme = this->titleBar()->palette();
    titleBarScheme.setColor(QPalette::All, QPalette::Background, color);
    titleBarScheme.setColor(QPalette::All, QPalette::Text,       textColor);
    titleBarScheme.setColor(QPalette::All, QPalette::WindowText, textColor);
    this->titleBar()->setPalette(titleBarScheme);
    this->setPalette(titleBarScheme);
}

void BrowserWindow::setBaseTitle(const QString &title, bool permanent)
{
    this->m_permanentTitle = permanent;

    if (permanent)
    {
        this->m_baseTitle.clear();
        QWidget::setWindowTitle(title);
        this->titleBar()->setTitle(QWidget::windowTitle());
    }
    else
    {
        this->m_baseTitle = title;
    }
}

void BrowserWindow::setUrl(const QUrl &url)
{
    this->webView->setUrl(url);
    emit urlChanged(url);
}

void BrowserWindow::setUrlInterceptorEnabled(bool b, const QList<UrlInterceptorLink> &urlInterceptorLinks)
{
    if (this->m_interceptor)
        delete this->m_interceptor;

    this->m_interceptorEnabled = b;
    if (b)
    {
        qDebug() << "URL Interceptor enabled!";
        this->m_interceptor = new UrlRequestInterceptor(urlInterceptorLinks);
        this->webView->page()->profile()->setRequestInterceptor(this->m_interceptor);

        this->webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        this->webView->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    }
    else
    {
        qDebug() << "URL Interceptor disabled!";
        this->m_interceptor = nullptr;
        this->webView->page()->profile()->setRequestInterceptor(nullptr);

        this->webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
        this->webView->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    }
}

void BrowserWindow::setProfile(const QString &id)
{
    this->m_engineProfilePath = Config()->webEngineProfiles() + '/' + id;

    this->m_cookieStoreId = id;
    this->webView->page()->profile()->setCachePath(this->m_engineProfilePath);
    this->webView->page()->profile()->setPersistentStoragePath(this->m_engineProfilePath + '/' + "Storage");
    this->webView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    this->m_cookieStore = this->webView->page()->profile()->cookieStore();
    this->m_cookieStore->loadAllCookies();
}

void BrowserWindow::setScripts(const QList<Script> &scripts)
{
    // remove existing scripts first
    this->removeScripts();

    qDebug() << "Available scripts for this profile:" << scripts;
    for (auto&& script : scripts)
    {
        qDebug() << "Loading script:" << script.filename;
        QWebEngineScript scr = this->loadScript(script.filename, script.injectionPoint);
        if (!scr.isNull())
        {
            this->m_scripts.append(scr);
            this->scripts->insert(scr);
        }
    }
}

void BrowserWindow::removeScripts()
{
    for (auto&& script : this->m_scripts)
        this->scripts->remove(script);
    this->m_scripts.clear();
}

void BrowserWindow::setUserAgent(const QString &ua)
{
    this->webView->page()->profile()->setHttpUserAgent(ua);
}

void BrowserWindow::restoreUserAgent()
{
    this->webView->page()->profile()->setHttpUserAgent(
        UserAgent::GetUserAgent(this->m_originalUserAgent));
}

void BrowserWindow::showEvent(QShowEvent *event)
{
    emit opened();
    event->accept();
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    this->resetProfile();
    event->accept();
    emit closed();
}

void BrowserWindow::toggleFullScreen()
{
    this->isFullScreen() ? this->showNormal() : this->showFullScreen();
}

void BrowserWindow::forceReload()
{
    this->webView->triggerPageAction(QWebEnginePage::ReloadAndBypassCache);
}

void BrowserWindow::onLoadProgress(int progress)
{
    // soon™
}

void BrowserWindow::onLoadFinished(bool ok)
{
    // soon™
}

void BrowserWindow::setUrlAboutBlank()
{
    this->setUrl(QUrl("about:blank"));
}

void BrowserWindow::acceptFullScreen(QWebEngineFullScreenRequest req)
{
    // accept fullscreen request, does nothing to the window or widgets
    req.accept();

    // actually toggle fullscreen mode
    this->toggleFullScreen();
}

void BrowserWindow::toggleAddressBarVisibility()
{
    this->emergencyAddressBar->isVisible() ? this->emergencyAddressBar->hide() : this->emergencyAddressBar->show();
}
