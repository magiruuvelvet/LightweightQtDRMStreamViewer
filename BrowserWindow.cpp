#include "BrowserWindow.hpp"

#include <QApplication>
#include <QDesktopWidget>

#include <StreamingProviderStore.hpp>

BrowserWindow::BrowserWindow(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::BypassGraphicsProxyWidget |
                         Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QRect desktopSize = QApplication::desktop()->screenGeometry();
    this->resize(desktopSize.width() / 1.2, desktopSize.height() / 1.2);
    this->move(desktopSize.width() / 2 - this->size().width() / 2, desktopSize.height() / 2 - this->size().height() / 2);

    this->m_layout = new QVBoxLayout();
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setMargin(0);
    this->m_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->webView = new QWebEngineView();
    this->webView->setContextMenuPolicy(Qt::NoContextMenu);
    this->m_layout->addWidget(this->webView);
    this->setLayout(this->m_layout);

    QObject::connect(this->webView, &QWebEngineView::titleChanged, this, &BrowserWindow::setWindowTitle);
    QObject::connect(this->webView, &QWebEngineView::loadFinished, this, &BrowserWindow::hideScrollBars);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this, SLOT(toggleFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

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
}

BrowserWindow::~BrowserWindow()
{
    this->m_baseTitle.clear();
    this->m_cookieStoreId.clear();
    this->m_cookies.clear();
}

void BrowserWindow::setWindowTitle(const QString &title)
{
    QWidget::setWindowTitle(title + QString::fromUtf8(" ─ ") + this->m_baseTitle);
}

void BrowserWindow::setBaseTitle(const QString &title)
{
    this->m_baseTitle = title;
}

void BrowserWindow::setUrl(const QUrl &url)
{
    this->webView->setUrl(url);
}

void BrowserWindow::setCookieStoreId(const QString &id)
{
    this->m_engineProfilePath = QDir::cleanPath(
            StreamingProviderStore::instance()->providerStorePath()
            + "/.." + '/' + "WebEngineProfiles" + '/' + id);

    this->m_cookieStoreId = id;
    this->webView->page()->profile()->setCachePath(this->m_engineProfilePath);
    this->webView->page()->profile()->setPersistentStoragePath(this->m_engineProfilePath + '/' + "Storage");
    this->webView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    this->m_cookieStore = this->webView->page()->profile()->cookieStore();
    this->m_cookieStore->loadAllCookies();
}

void BrowserWindow::reset()
{
    this->m_engineProfilePath = QDir::cleanPath(
            StreamingProviderStore::instance()->providerStorePath()
            + "/.." + '/' + "WebEngineProfiles" + '/' + "Default");

    this->m_baseTitle.clear();
    QWidget::setWindowTitle(QLatin1String("BrowserWindow"));
    this->webView->setUrl(QUrl());
    this->m_cookieStoreId.clear();
    this->webView->stop();

    this->webView->page()->profile()->setCachePath(this->m_engineProfilePath);
    this->webView->page()->profile()->setPersistentStoragePath(this->m_engineProfilePath + '/' + "Storage");
    this->webView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    this->m_cookieStore = this->webView->page()->profile()->cookieStore();

    this->close();
}

void BrowserWindow::toggleFullScreen()
{
    this->isFullScreen() ? this->showNormal() : this->showFullScreen();
}

void BrowserWindow::hideScrollBars()
{
    this->webView->page()->runJavaScript("document.styleSheets[0].addRule('::-webkit-scrollbar', 'width: 0px; height: 0px', 0);");
}

void BrowserWindow::acceptFullScreen(QWebEngineFullScreenRequest req)
{
    // accept fullscreen request, does nothing to the window or widgets
    req.accept();

    // actually toggle fullscreen mode
    this->toggleFullScreen();
}
