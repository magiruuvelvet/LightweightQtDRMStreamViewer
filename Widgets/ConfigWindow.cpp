#include "ConfigWindow.hpp"

#include <QDebug>

ConfigWindow::ConfigWindow(QWidget *parent)
    : BaseWindow(parent)
{
}

ConfigWindow::~ConfigWindow()
{
}

void ConfigWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit closed();
}
