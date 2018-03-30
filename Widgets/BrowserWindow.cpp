#include "BrowserWindow.hpp"

#include <QApplication>
#include <QDesktopWidget>

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>

BrowserWindow::BrowserWindow(QWidget *parent)
    : BaseWindow(parent)
{
    this->setWindowFlags(this->windowFlags() |
                         Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QRect desktopSize = QApplication::desktop()->screenGeometry();
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

    // Inject app name and version into the default Qt Web Engine user agent
    // the default looks like this:
    //     Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) QtWebEngine/5.9.1 Chrome/56.0.2924.122 Safari/537.36
    QString UserAgent = this->webView->page()->profile()->httpUserAgent();
    UserAgent.replace("QtWebEngine", qApp->applicationName() + '/' + qApp->applicationVersion() + " QtWebEngine");
    this->webView->page()->profile()->setHttpUserAgent(UserAgent);

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

QWebEngineScript *BrowserWindow::loadScript(const QString &filename)
{
    QFile file(Config()->providerStoreDir() + '/' + filename);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString target = file.readAll();
        file.close();

        QWebEngineScript *script = new QWebEngineScript();
        script->setName(filename);
        script->setInjectionPoint(QWebEngineScript::DocumentReady);
        script->setSourceCode(target);

        qDebug() << "Loaded script" << file.fileName();
        return script;
    }
    else
    {
        qDebug() << "Error loading script" << file.fileName();
        return nullptr;
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

BrowserWindow::~BrowserWindow()
{
    this->m_baseTitle.clear();
    this->m_cookieStoreId.clear();
    this->m_engineProfilePath.clear();
    this->m_cookies.clear();

    this->mJs_hideScrollBars.clear();

//    delete scripts;
//    delete m_interceptor;
//    delete emergencyAddressBar;

//    delete m_cookieStore;
//    delete webView;
//    delete m_layout;
}

BrowserWindow *BrowserWindow::createBrowserWindow(const Provider &profile)
{
    BrowserWindow *w = new BrowserWindow();
    StreamingProviderStore::loadProfile(w, profile);
    return w;
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
    const QRect desktopSize = QApplication::desktop()->screenGeometry();
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
    if (permanent)
    {
        this->m_permanentTitle = true;
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

void BrowserWindow::setScripts(const QList<QString> &scripts)
{
    this->m_scripts = scripts;

    qDebug() << "Available scripts for this profile:" << this->m_scripts;
    for (auto&& script : this->m_scripts)
    {
        qDebug() << "Loading script:" << script;
        QWebEngineScript *scr = this->loadScript(script);
        scr ? this->scripts->insert(*scr) : void();
    }
}

void BrowserWindow::showEvent(QShowEvent *event)
{
    emit opened();
    event->accept();
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
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
