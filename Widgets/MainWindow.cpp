#include "MainWindow.hpp"

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>
#include <Core/BrowserWindowProcess.hpp>

#include <Gui/ProviderButton.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>

#include "BrowserWindow.hpp"
#include "ConfigWindow.hpp"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : BaseWindow(parent)
{
    this->setWindowFlag(Qt::WindowCloseButtonHint, true);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint, true);

    this->resize(600, 138);

    // Center window on screen
    QRect desktopSize = QApplication::desktop()->screenGeometry(this);
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
    this->titleBar()->addButton("⚙", [&]{
        ConfigWindow *w = new ConfigWindow();
        w->setWindowModality(Qt::ApplicationModal);
        QObject::connect(w, &ConfigWindow::providersUpdated, this, &MainWindow::updateProviderList);
        QObject::connect(w, &ConfigWindow::closed, w, &ConfigWindow::deleteLater);
        QObject::connect(w, &ConfigWindow::closed, this, [&]{
            this->setWindowOpacity(1.0);
        });
        this->setWindowOpacity(0.75);
        w->show();
    });

    this->updateProviderList();

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

    ///
    /// segfaulty method
    ///
//    BrowserWindow *w = BrowserWindow::getInstance();
//    w->resetProfile();
//    w->setProfile(pr);

//    Config()->fullScreenMode() ? w->showFullScreen() : w->showNormal();

    ///
    /// workaround random segfaults by spawning a new instance of the app
    ///
    BrowserWindowProcess *instance = new BrowserWindowProcess();
    instance->start(pr);
}

void MainWindow::updateProviderList()
{
    // remove all button pointers
    this->_providerBtns.clear();

    // remove all buttons from layout
    QLayoutItem *item;
    while ( (item = this->_lF_providerButtonList->layout()->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
    //delete this->_lF_providerButtonList->layout();

    // create provider list
    for (auto&& i : StreamingProviderStore::instance()->providers())
    {
        this->_providerBtns.append(ProviderButton::create(i));
        this->_lF_providerButtonList->addWidget(this->_providerBtns.last());
        QObject::connect(this->_providerBtns.last(), &QPushButton::clicked, this, &MainWindow::launchBrowserWindow);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Config()->setMainWindowGeometry(this->geometry());
    event->accept();
}
