#include "MainWindow.hpp"

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include <QStyle>
#include <QProxyStyle>

#include <QDebug>

class StyleOverrides : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget) const
    {
        // do not draw focus rectangles
        if (element == QStyle::PE_FrameFocusRect)
            return;

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::BypassGraphicsProxyWidget |
                         Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    this->resize(600, 138);
    this->setMaximumHeight(this->height());
    this->setMinimumHeight(this->height());
    QRect desktopSize = QApplication::desktop()->screenGeometry();
    this->move(desktopSize.width() / 2 - this->size().width() / 2, desktopSize.height() / 2 - this->size().height() / 2);

    this->setMouseTracking(true);
    this->installEventFilter(this);

    // Style Overrides
    this->setBackgroundRole(QPalette::Background);
    QPalette rootWinColorScheme;
    rootWinColorScheme.setColor(QPalette::All, QPalette::Background, QColor( 50,  50,  50, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::Text,       QColor(255, 255, 255, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::ButtonText, QColor(255, 255, 255, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::WindowText, QColor(255, 255, 255, 255));
    this->setPalette(rootWinColorScheme);
    this->setStyle(new StyleOverrides);

    QPalette toolTipPalette = QToolTip::palette();
    toolTipPalette.setColor(QPalette::All, QPalette::ToolTipBase, QColor( 70,  70,  70, 255));
    toolTipPalette.setColor(QPalette::All, QPalette::ToolTipText, QColor(255, 255, 255, 255));
    QToolTip::setPalette(toolTipPalette);
    QToolTip::setFont(QFont("Sans Serif", 10, QFont::Medium, false));

    // Create Layout and Widgets
    this->_lV_main = new QVBoxLayout();
    this->_lV_main->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lV_main->setContentsMargins(5, 8, 5, 5);
    this->_lV_main->setDirection(QBoxLayout::TopToBottom);

    this->_lH_titleBar = new QHBoxLayout();
    this->_lH_titleBar->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lH_titleBar->setContentsMargins(0, 0, 0, 2);
    this->_lV_main->addLayout(this->_lH_titleBar);

    this->_titleBarText = new QLabel(qApp->applicationDisplayName());
    this->_titleBarText->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->_titleBarText->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->_titleBarText->setFont(QFont("Sans Serif", 10, QFont::Medium, false));
    this->_titleBarBtn = new QPushButton(QString::fromUtf8("×"));
    this->_titleBarBtn->setFlat(true);
    this->_titleBarBtn->setFixedSize(15, 15);
    this->_titleBarBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->_titleBarBtn->setFocusPolicy(Qt::NoFocus);
    this->_titleBarBtn->setStyleSheet(
                "QPushButton{outline: none; border: none; padding: 0px; color: #ffffff;}"
                "QPushButton:focus{outline: none; border: 1px solid #f3f3f3; padding: 0px;}"
                "QPushButton:hover{outline: none; border: 1px solid #ffffff; padding: 0px; background-color: #555555;}"
                "QPushButton:pressed{outline: none; border: 1px solid #ffffff; padding: 0px; background-color: #484848;}");
    QObject::connect(this->_titleBarBtn, &QPushButton::clicked, this, &MainWindow::close);
    this->_lH_titleBar->addWidget(this->_titleBarText, 1, Qt::AlignCenter);
    this->_lH_titleBar->addWidget(this->_titleBarBtn, 0, Qt::AlignRight);

    this->_lH_providerButtonList = new QHBoxLayout();
    this->_lH_providerButtonList->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lH_providerButtonList->setContentsMargins(0, 0, 0, 0);
    this->_lH_providerButtonList->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    this->_lV_main->addLayout(this->_lH_providerButtonList);

    this->_rootWidget = new QWidget();
    this->_rootWidget->setLayout(this->_lV_main);

    this->setCentralWidget(this->_rootWidget);
    this->setContentsMargins(0, 0, 0, 0);

    this->browser = new BrowserWindow();
    this->browser->reset();

    for (auto&& i : StreamingProviderStore::instance()->providers())
    {
        this->_providerBtns.append(new QPushButton(i.name));
        this->_providerBtns.last()->setObjectName(i.id);
        this->_providerBtns.last()->setGeometry(-1, -1, 80, 80);
        this->_providerBtns.last()->setMinimumWidth(80);
        this->_providerBtns.last()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        this->_providerBtns.last()->setFlat(true);
        this->_providerBtns.last()->setFocusPolicy(Qt::TabFocus);
        this->_providerBtns.last()->setStyleSheet(
                    "QPushButton{outline: none; border: none; padding: 5px; color: #ffffff;}"
                    "QPushButton:focus{outline: none; border: 1px solid #f3f3f3; padding: 5px;}"
                    "QPushButton:hover{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #555555;}"
                    "QPushButton:pressed{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #484848;}");

        if (!i.icon.isNull())
        {
            this->_providerBtns.last()->setIcon(i.icon);
            this->_providerBtns.last()->setIconSize(QSize(80, 80));
            this->_providerBtns.last()->setText(QString());
            this->_providerBtns.last()->setToolTip(i.name);
        }

        this->_lH_providerButtonList->addWidget(this->_providerBtns.last());

        QObject::connect(this->_providerBtns.last(), &QPushButton::clicked, this, &MainWindow::loadProfile);
    }

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    this->_providerBtns.clear();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverMove) {
        QHoverEvent *mouseHoverEvent = static_cast<QHoverEvent *>(event);
        this->mouseMove(mouseHoverEvent->pos(), mouseHoverEvent->oldPos());
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    this->m_clickPos = e->pos();

    mMousePressed = e->button() == Qt::LeftButton;
    if (mMousePressed)
    {
        if (left)
        {
            mClickedPos.setX(e->pos().x());
        }
        if (right)
        {
            mClickedPos.setX(width() - e->pos().x());
        }
        if (bottom)
        {
            mClickedPos.setY(height() - e->pos().y());
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mMousePressed = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - this->m_clickPos);
}

// taken from https://github.com/alexcastano/borderless-qt
// note: doesn't work as it should due to lazy integration in this app ¯\_(ツ)_/¯
// needs more testing how this works and probably merge with `mouseMoveEvent`
void MainWindow::mouseMove(QPoint newPos, QPoint oldPos)
{
    if (mMousePressed) {
        int dx = newPos.x() - oldPos.x();
        int dy = newPos.y() - oldPos.y();

        QRect g = geometry();
        QSize minSize = minimumSize();

        // We don't resize if the windows has the minimum size
        if (left) {
            // Fix a bug when you try to resize to less than minimum size and
            // then the mouse goes right again.
            if (dx < 0 && oldPos.x() > mClickedPos.x() ) {
                dx += oldPos.x() - mClickedPos.x();
                if (dx > 0) {
                    dx = 0;
                }
            } else if ( dx > 0 && g.width() - dx < minSize.width()) {
                dx = g.width() - minSize.width();
            }
            g.setLeft(g.left() + dx);
        }

        if (right) {
            // Fix a bug when you try to resize to less than minimum size and
            // then the mouse goes right again.
            if (dx > 0 && (width() - newPos.x() > mClickedPos.x())) {
                dx -= width() - newPos.x() - mClickedPos.x();
                if (dx < 0) {
                    dx = 0;
                }
            }
            g.setRight(g.right() + dx);
        }
        if (bottom) {
            // Fix a bug when you try to resize to less than minimum size and
            // then the mouse goes down again.
            if (dy > 0 && (height() - newPos.y() > mClickedPos.y())) {
                dy -= height() - newPos.y() - mClickedPos.y();
                if (dy < 0) {
                    dy = 0;
                }
            }
            g.setBottom(g.bottom() + dy);
        }
        setGeometry(g);

    } else {
        QRect r = rect();
        left = qAbs(newPos.x()- r.left()) <= 5;// &&
            //newPos.y() > mTitleBar->height();
        right = qAbs(newPos.x() - r.right()) <= 5;// &&
            //newPos.y() > mTitleBar->height();
        bottom = qAbs(newPos.y() - r.bottom()) <= 5;
        bool hor = left | right;

        if (hor && bottom) {
            if (left)
                setCursor(Qt::SizeBDiagCursor);
            else
                setCursor(Qt::SizeFDiagCursor);
        } else if (hor) {
            setCursor(Qt::SizeHorCursor);
        } else if (bottom) {
            setCursor(Qt::SizeVerCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void MainWindow::loadProfile()
{
    QPushButton *button = qobject_cast<QPushButton*>(QObject::sender());
    Provider pr = StreamingProviderStore::instance()->provider(button->objectName());

    this->browser->reset();

    pr.titleBarHasPermanentTitle ?
        this->browser->setBaseTitle(pr.titleBarPermanentTitle, true) :
        this->browser->setBaseTitle(pr.name);
    this->browser->setTitleBarVisibility(pr.titleBarVisible);
    this->browser->setTitleBarColor(pr.titleBarColor, pr.titleBarTextColor);
    this->browser->setWindowTitle("Loading...");
    this->browser->setWindowIcon(pr.icon);
    this->browser->setProfile(pr.id);
    this->browser->setScripts(pr.scripts);
    this->browser->setUrlInterceptorEnabled(pr.urlInterceptor);
    this->browser->setUrl(pr.url);

    Config()->fullScreenMode() ? this->browser->showFullScreen() : this->browser->showNormal();
}
