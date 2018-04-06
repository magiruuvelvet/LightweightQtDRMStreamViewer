#include "ProviderButton.hpp"

QPushButton *ProviderButton::create(const Provider &provider)
{
    QPushButton *btn = new QPushButton(provider.name);
    btn->setObjectName(provider.id);
    btn->setGeometry(-1, -1, 80, 80);
    btn->setFixedWidth(95);
    btn->setFixedHeight(95);
    btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    btn->setFlat(true);
    btn->setFocusPolicy(Qt::TabFocus);
    btn->setStyleSheet(
        "QPushButton{outline: none; border: none; padding: 5px; color: #ffffff;}"
        "QPushButton:focus{outline: none; border: 1px solid #f3f3f3; padding: 5px;}"
        "QPushButton:hover{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #555555;}"
        "QPushButton:pressed{outline: none; border: 1px solid #ffffff; padding: 5px; background-color: #484848;}");

    if (!provider.icon.isNull())
    {
        btn->setIcon(provider.icon);
        btn->setIconSize(QSize(80, 80));
        btn->setText(QString());
        btn->setToolTip(provider.name);
    }

    return btn;
}
