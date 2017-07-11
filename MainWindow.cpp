#include "MainWindow.hpp"

#include <StreamingProviderStore.hpp>

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

    this->_lV_main = new QVBoxLayout();
    this->_lV_main->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lV_main->setContentsMargins(5, 8, 5, 5);
    this->_lV_main->setDirection(QBoxLayout::TopToBottom);

    this->_lH_titleBar = new QHBoxLayout();
    this->_lH_titleBar->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lH_titleBar->setContentsMargins(0, 0, 0, 2);
    //this->_lH_titleBar->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
        this->_providerBtns.last()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        this->_providerBtns.last()->setFlat(true);
        this->_providerBtns.last()->setFocusPolicy(Qt::TabFocus);
        this->_providerBtns.last()->setStyleSheet(
                    "QPushButton{outline: none; border: none; padding: 5px; color: #ffffff;}"
                    "QPushButton:focus{outline: none; border: 1px solid #f3f3f3; padding: 5px;}"
                    "QPushButton:hover{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #555555;}"
                    "QPushButton:pressed{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #484848;}");

        if (!i.icon.isEmpty())
        {
            this->_providerBtns.last()->setIcon(QIcon(StreamingProviderStore::instance()->providerStorePath() + '/' + i.icon));
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

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    this->m_clickPos = e->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - this->m_clickPos);
}

void MainWindow::loadProfile()
{
    QPushButton *button = qobject_cast<QPushButton*>(QObject::sender());
    StreamingProviderStore::Provider pr = StreamingProviderStore::instance()->provider(button->objectName());

    this->browser->reset();

    this->browser->setBaseTitle(pr.name);
    this->browser->setWindowTitle("Loading...");
    this->browser->setWindowIcon(QIcon(StreamingProviderStore::instance()->providerStorePath() + '/' + pr.icon));
    this->browser->setCookieStoreId(pr.id);
    this->browser->setUrl(QUrl(pr.url));

    this->browser->show();
}
