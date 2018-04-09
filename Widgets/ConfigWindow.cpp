#include "ConfigWindow.hpp"

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderStore.hpp>

#include <Gui/ProviderButton.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>
#include <QHeaderView>

#include <QDebug>

ConfigWindow::ConfigWindow(QWidget *parent)
    : BaseWindow(parent)
{
    this->setWindowTitle("Configuration");

    this->setWindowFlag(Qt::WindowCloseButtonHint, true);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint, false);

    this->resize(430, 730);

    // Center window on screen
    QRect desktopSize = QApplication::desktop()->screenGeometry();
    this->move(desktopSize.width() / 2 - this->size().width() / 2,
               desktopSize.height() / 2 - this->size().height() / 2);

    // Load saved geometry if greater than 0 (default on first startup)
    const auto geom = Config()->configWindowGeometry();
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

    // provider list
    this->m_tblView = new QTableView();
    this->m_tblView->setModel(new ProviderListModel());
    this->m_tblView->horizontalHeader()->hide();
    this->m_tblView->verticalHeader()->hide();
    this->_lV_main->addWidget(this->m_tblView);

    this->m_tblView->setStyleSheet("* {color: white; background-color: #434343;}");
    this->m_tblView->setEditTriggers(QTableView::NoEditTriggers);
    this->m_tblView->setSelectionBehavior(QTableView::SelectRows);
    this->m_tblView->setSelectionMode(QTableView::SingleSelection);

    this->m_tblView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    this->m_tblView->setColumnWidth(0, 25);

    QObject::connect(this->m_tblView, &QTableView::clicked, this, [&](const QModelIndex &idx) {
        this->m_editWidget->setProvider(StreamingProviderStore::instance()->providerAt(idx.row()));
    });
    QObject::connect(this->m_tblView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                     this, [&](const QModelIndex &current, const QModelIndex &) {
        this->m_editWidget->setProvider(StreamingProviderStore::instance()->providerAt(current.row()));
    });

    this->m_editWidget = new ProviderEditWidget();
    this->_lV_main->addWidget(this->m_editWidget);

    this->_lV_main->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->containerWidget()->setLayout(this->_lV_main);

    this->setContentsMargins(0, 0, 0, 0);
}

ConfigWindow::~ConfigWindow()
{
    m_tblView->disconnect();
    delete m_tblView;
    m_editWidget->disconnect();
    delete m_editWidget;

    qDebug() << "ConfigWindow destroyed!";
}

void ConfigWindow::closeEvent(QCloseEvent *event)
{
    Config()->setConfigWindowGeometry(this->geometry());
    event->accept();
    emit closed();
}
