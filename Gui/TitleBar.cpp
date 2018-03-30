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

    this->m_closeBtn = new QPushButton(QString::fromUtf8("×"));
    this->m_closeBtn->setFlat(true);
    this->m_closeBtn->setFixedSize(15, 15);
    this->m_closeBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->m_closeBtn->setFocusPolicy(Qt::NoFocus);
    this->m_closeBtn->setStyleSheet(
                "QPushButton{outline: none; border: none; padding: 0px; color: #ffffff;}"
                "QPushButton:focus{outline: none; border: 1px solid #f3f3f3; padding: 0px;}"
                "QPushButton:hover{outline: none; border: 1px solid #ffffff; padding: 0px; background-color: #555555;}"
                "QPushButton:pressed{outline: none; border: 1px solid #ffffff; padding: 0px; background-color: #484848;}");
    QObject::connect(this->m_closeBtn, &QPushButton::clicked, this, [&]{
        if (this->parentWidget())
            this->parentWidget()->close();
    });

    this->m_layout->addWidget(this->m_icon);
    this->m_layout->addWidget(this->m_title);
    this->m_layout->addWidget(this->m_closeBtn);
    this->setLayout(this->m_layout);
}

TitleBar::~TitleBar()
{
    delete m_title;
    delete m_icon;

    delete m_closeBtn;
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    static_cast<QWidget*>(this->parent())->move(event->globalX() - m_nMouseClick_X_Coordinate,
                                                event->globalY() - m_nMouseClick_Y_Coordinate);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *)
{
    if (this->parentWidget()->isFullScreen())
        return;

    else if (this->parentWidget()->isMaximized())
        this->parentWidget()->showNormal();
    else
        this->parentWidget()->showMaximized();
}

void TitleBar::setTitle(const QString &title)
{
    this->m_title->setText(title);
}

void TitleBar::setIcon(const QPixmap &icon)
{
    this->m_icon->setPixmap(icon.scaled(23, 23, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
