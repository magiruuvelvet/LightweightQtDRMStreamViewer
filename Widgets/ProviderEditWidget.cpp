#include "ProviderEditWidget.hpp"

ProviderEditWidget::ProviderEditWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setStyleSheet("*{background-color:white;}");
}

ProviderEditWidget::~ProviderEditWidget()
{
}

void ProviderEditWidget::setProvider(const Provider &provider)
{
    this->provider = provider;
    this->_update();
}

void ProviderEditWidget::_update()
{

}
