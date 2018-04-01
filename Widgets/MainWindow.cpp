#include "MainWindow.hpp"

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>

#include "BrowserWindow.hpp"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : BaseWindow(parent)
{
    this->setWindowFlag(Qt::WindowCloseButtonHint, true);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint, true);

    this->resize(600, 138);

    // Center window on screen
    QRect desktopSize = QApplication::desktop()->screenGeometry();
    this->move(desktopSize.width() / 2 - this->size().width() / 2,
               desktopSize.height() / 2 - this->size().height() / 2);

    // Load saved geometry if greater than 0 (default on first startup)
    const auto geom = Config()->mainWindowGeometry();
    if (geom.x() >= 0 && geom.y() >= 0 &&
        geom.width() > 0 && geom.height() > 0)
    {
        this->setGeometry(geom);
    }

    this->installEventFilter(this);

    QPalette toolTipPalette = QToolTip::palette();
    toolTipPalette.setColor(QPalette::All, QPalette::ToolTipBase, QColor( 70,  70,  70, 255));
    toolTipPalette.setColor(QPalette::All, QPalette::ToolTipText, QColor(255, 255, 255, 255));
    QToolTip::setPalette(toolTipPalette);
    QToolTip::setFont(QFont("Sans Serif", 10, QFont::Medium, false));

    // Create Layout
    this->_lV_main = new QVBoxLayout();
    this->_lV_main->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lV_main->setContentsMargins(5, 8, 5, 5);
    this->_lV_main->setDirection(QBoxLayout::TopToBottom);

    this->_lF_providerButtonList = new FlowLayout();
    this->_lF_providerButtonList->setSizeConstraint(QLayout::SetMaximumSize);
    this->_lF_providerButtonList->setContentsMargins(0, 0, 0, 0);

    this->_lV_main->addLayout(this->_lF_providerButtonList);
    this->_lV_main->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->containerWidget()->setLayout(this->_lV_main);

    this->setContentsMargins(0, 0, 0, 0);

    // configuration button
    this->titleBar()->addButton("⚙", []{
        qDebug() << "Configuration Button pressed";
    });

    for (auto&& i : StreamingProviderStore::instance()->providers())
    {
        this->_providerBtns.append(new QPushButton(i.name));
        this->_providerBtns.last()->setObjectName(i.id);
        this->_providerBtns.last()->setGeometry(-1, -1, 80, 80);
        this->_providerBtns.last()->setFixedWidth(95);
        this->_providerBtns.last()->setFixedHeight(95);
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

        this->_lF_providerButtonList->addWidget(this->_providerBtns.last());

        QObject::connect(this->_providerBtns.last(), &QPushButton::clicked, this, &MainWindow::launchBrowserWindow);
    }

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    this->_providerBtns.clear();

    delete _lF_providerButtonList;
    delete _lV_main;
}

void MainWindow::launchBrowserWindow()
{
    QPushButton *button = qobject_cast<QPushButton*>(QObject::sender());
    const Provider pr = StreamingProviderStore::instance()->provider(button->objectName());

    BrowserWindow *w = BrowserWindow::createBrowserWindow(pr);
    QObject::connect(w, &BrowserWindow::closed, this, [&]{
        // FIXME: delete engine instance here
        // memory leak if you keep the app open and open/close profiles regularly
    });

    Config()->fullScreenMode() ? w->showFullScreen() : w->showNormal();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Config()->setMainWindowGeometry(this->geometry());
    event->accept();
}
