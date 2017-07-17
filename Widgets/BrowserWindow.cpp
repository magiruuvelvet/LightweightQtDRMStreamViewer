#include "BrowserWindow.hpp"

#include <QApplication>
#include <QDesktopWidget>

#include <Core/StreamingProviderStore.hpp>

BrowserWindow::BrowserWindow(bool titleBarVisible, QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::BypassGraphicsProxyWidget |
                         Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QRect desktopSize = QApplication::desktop()->screenGeometry();
    this->resize(desktopSize.width() / 1.2, desktopSize.height() / 1.2);
    this->move(desktopSize.width() / 2 - this->size().width() / 2, desktopSize.height() / 2 - this->size().height() / 2);

//    this->installEventFilter(this);
//    this->setMouseTracking(true);

    this->m_layout = new QVBoxLayout();
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setMargin(0);
    this->m_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->createTitleBar(titleBarVisible);

    this->webView = new QWebEngineView();
    this->webView->setContextMenuPolicy(Qt::NoContextMenu);
    this->m_layout->addWidget(this->webView);
    this->setLayout(this->m_layout);

    QObject::connect(this->webView, &QWebEngineView::titleChanged, this, &BrowserWindow::setWindowTitle);
    QObject::connect(this->webView, &QWebEngineView::loadFinished, this, &BrowserWindow::hideScrollBars);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this, SLOT(toggleFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    // temporary workaround until i figured out how to implement a player-like behavior
    // note: the Maxdome player is buggy and slow, it also has major cursor visibility toggle issues,
    //       that's why i want to implement such a feature client-side in this app
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_H), this, SLOT(toggleCursorVisibility()));

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

void BrowserWindow::createTitleBar(bool visible)
{
    //this->setWindowFlag(Qt::FramelessWindowHint, false);
    if (!this->m_titleBar)
    {
        this->m_titleBar = new TitleBar(this);
        this->m_titleBar->setFixedHeight(25);
        this->m_titleBar->setBackgroundRole(QPalette::Background);
        QPalette titleBarScheme;
        titleBarScheme.setColor(QPalette::All, QPalette::Background, QColor( 50,  50,  50, 255));
        titleBarScheme.setColor(QPalette::All, QPalette::Text,       QColor(255, 255, 255, 255));
        titleBarScheme.setColor(QPalette::All, QPalette::WindowText, QColor(255, 255, 255, 255));
        this->m_titleBar->setPalette(titleBarScheme);
        this->m_titleBar->setVisible(visible);
        this->m_layout->addWidget(this->m_titleBar);
    }
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
    if (this->m_titleBar) this->m_titleBar->setTitle(QWidget::windowTitle());
}

void BrowserWindow::setWindowIcon(const QIcon &icon)
{
    QWidget::setWindowIcon(icon);
    if (this->m_titleBar) this->m_titleBar->setIcon(icon.pixmap(23, 23, QIcon::Normal, QIcon::On));
}

void BrowserWindow::setTitleBarVisibility(bool b)
{
    //this->setWindowFlag(Qt::FramelessWindowHint, !b);
    if (this->m_titleBar)
    {
        this->m_titleBar->setVisible(b);
    }
}

void BrowserWindow::setTitleBarColor(const QColor &color, const QColor &textColor)
{
    if (this->m_titleBar)
    {
        QPalette titleBarScheme = this->m_titleBar->palette();
        titleBarScheme.setColor(QPalette::All, QPalette::Background, color);
        titleBarScheme.setColor(QPalette::All, QPalette::Text,       textColor);
        titleBarScheme.setColor(QPalette::All, QPalette::WindowText, textColor);
        this->m_titleBar->setPalette(titleBarScheme);
        this->setPalette(titleBarScheme);
    }
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

//bool BrowserWindow::eventFilter(QObject*, QEvent *e)
//{
//    if (e->type() == QEvent::MouseMove)
//    {
//         this->mouseMoveEvent(static_cast<QMouseEvent*>(e));
//    }

//    return false;
//}

//void BrowserWindow::enterEvent(QEvent*)
//{
//    qDebug() << "enterEvent";
//    QTimer::singleShot(2000, this, &BrowserWindow::hideCursor);
//}

void BrowserWindow::hideCursor()
{
    this->setCursor(QCursor(Qt::BlankCursor));
}

//void BrowserWindow::leaveEvent(QEvent*)
//{
//    qDebug() << "leaveEvent";
//    this->showCursor();
//}

void BrowserWindow::showCursor()
{
    this->setCursor(QCursor(Qt::ArrowCursor));
}

//void BrowserWindow::mouseMoveEvent(QMouseEvent*)
//{
//    qDebug() << "mouseMoveEvent";
//    this->showCursor();
//    QTimer::singleShot(2000, this, &BrowserWindow::hideCursor);
//}

void BrowserWindow::showNormal()
{
    if (this->m_titleBar)
        this->m_titleBar->setVisible(true);
    QWidget::showNormal();
}

void BrowserWindow::showFullScreen()
{
    if (this->m_titleBar)
        this->m_titleBar->setVisible(false);
    QWidget::showFullScreen();
}

void BrowserWindow::toggleFullScreen()
{
    this->isFullScreen() ? this->showNormal() : this->showFullScreen();
}

void BrowserWindow::toggleCursorVisibility()
{
    // buggy as fuck and doesn't work most of the time
    static bool hidden = false; // assume cursor is visible
    this->unsetCursor();
    hidden ? this->showCursor() : this->hideCursor();
    hidden = !hidden;
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
