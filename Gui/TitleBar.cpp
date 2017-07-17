#include "TitleBar.hpp"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent)
{
    this->m_layout = new QHBoxLayout();
    this->m_layout->setContentsMargins(5, 1, 5, 1);
    this->m_layout->setSpacing(2);
    this->m_layout->setAlignment(Qt::AlignVCenter);
    this->m_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->m_icon = new QLabel();
    this->m_icon->setFixedSize(23, 23);
    this->m_icon->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    this->m_title = new QLabel();
    this->m_title->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->m_title->setFont(QFont("Sans Serif", 11, QFont::Medium, false));

    this->m_layout->addWidget(this->m_icon);
    this->m_layout->addWidget(this->m_title);
    this->setLayout(this->m_layout);
}

TitleBar::~TitleBar()
{
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    static_cast<QWidget*>(this->parent())->move(event->globalX() - m_nMouseClick_X_Coordinate, event->globalY() - m_nMouseClick_Y_Coordinate);
}

void TitleBar::setTitle(const QString &title)
{
    this->m_title->setText(title);
}

void TitleBar::setIcon(const QPixmap &icon)
{
    this->m_icon->setPixmap(icon.scaled(23, 23, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
