#include "BaseWindow.hpp"

#include <QApplication>
#include <QStyle>
#include <QProxyStyle>

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

BaseWindow::BaseWindow(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowOverridesSystemGestures);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);

    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_Hover);

    //this->setMinimumSize(400, 260);
    //this->resize(450, 600);

    // Style Overrides
    this->setBackgroundRole(QPalette::Background);
    QPalette rootWinColorScheme;
    rootWinColorScheme.setColor(QPalette::All, QPalette::Background, QColor( 50,  50,  50, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::Text,       QColor(255, 255, 255, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::ButtonText, QColor(255, 255, 255, 255));
    rootWinColorScheme.setColor(QPalette::All, QPalette::WindowText, QColor(255, 255, 255, 255));

    this->setBackgroundRole(QPalette::Background);
    this->setAutoFillBackground(true);
    this->setPalette(rootWinColorScheme);
    this->setStyle(new StyleOverrides);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    titleBarContainer = new QVBoxLayout();
    titleBarContainer->setSpacing(0);
    titleBarContainer->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(titleBarContainer);

    tb = new TitleBar(this);
    titleBarContainer->addWidget(tb);

    this->installEventFilter(this);
    this->setWindowTitle(qApp->applicationDisplayName());

    containerLayout = new QVBoxLayout();
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(5, 5, 5, 5);
    layout->addLayout(containerLayout);

    _containerWidget = new QWidget();
    _containerWidget->setContentsMargins(0, 0, 0, 0);
    _containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    containerLayout->addWidget(_containerWidget);
}

BaseWindow::~BaseWindow()
{
    delete tb;
    delete _containerWidget;

    delete containerLayout;
    delete titleBarContainer;
    delete layout;
}

void BaseWindow::setFixedSize(const QSize &size)
{
    this->fixedSize = true;
    this->setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    QWidget::setFixedSize(size);
}

void BaseWindow::setFixedSize(int w, int h)
{
    this->setFixedSize(QSize(w, h));
}

void BaseWindow::setContainerLayoutContentsMargins(int left, int top, int right, int bottom)
{
    this->containerLayout->setContentsMargins(left, top, right, bottom);
}

bool BaseWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverMove)
    {
        if (!this->fixedSize)
        {
            QHoverEvent *mouseHoverEvent = static_cast<QHoverEvent *>(event);
            this->mouseMove(mouseHoverEvent->pos(), mouseHoverEvent->oldPos());
        }
    }

    else if (event->type() == QEvent::WindowTitleChange)
    {
        tb->setTitle(this->windowTitle());
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}

void BaseWindow::mousePressEvent(QMouseEvent *e)
{
    this->activateWindow();
    if (!this->fixedSize)
    {
        mMousePressed = e->button() == Qt::LeftButton;
        if (mMousePressed) {
            if (top) {
                mClickedPos.setY(e->pos().y());
            }
            if (left) {
                mClickedPos.setX(e->pos().x());
            }
            if (right) {
                mClickedPos.setX(width() - e->pos().x());
            }
            if (bottom) {
                mClickedPos.setY(height() - e->pos().y());
            }
        }
    }
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (!this->fixedSize)
    {
        if (e->button() == Qt::LeftButton) {
            mMousePressed = false;
        }
    }
}

void BaseWindow::mouseMove(QPoint newPos, QPoint oldPos)
{
    static const constexpr int WINDOW_MARGIN = 5;

    if (!this->fixedSize)
    {
        if (mMousePressed) {
            int dx = newPos.x() - oldPos.x();
            int dy = newPos.y() - oldPos.y();

            QRect g = geometry();
            QSize minSize = minimumSize();

            // We don't resize if the windows has the minimum size
            if (top) {
                if (dy > 0 && (newPos.y() > mClickedPos.y())) {
                    dy -= newPos.y() - mClickedPos.y();
                    if (dy < 0) {
                        dy = 0;
                    }
                }
                g.setTop(g.top() + dy);
            }
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
            top = qAbs(newPos.y() - r.top()) <= WINDOW_MARGIN;
            left = qAbs(newPos.x()- r.left()) <= WINDOW_MARGIN;
            right = qAbs(newPos.x() - r.right()) <= WINDOW_MARGIN;
            bottom = qAbs(newPos.y() - r.bottom()) <= WINDOW_MARGIN;
            bool hor = left | right;
            bool ver = top | bottom;

            if (hor && bottom) {
                if (left)
                    setCursor(Qt::SizeBDiagCursor);
                else
                    setCursor(Qt::SizeFDiagCursor);
            } else if (hor) {
                setCursor(Qt::SizeHorCursor);
            } else if (ver) {
                setCursor(Qt::SizeVerCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }
}

void BaseWindow::showEvent(QShowEvent *e)
{
    this->activateWindow();
    this->raise();
    e->accept();
}
