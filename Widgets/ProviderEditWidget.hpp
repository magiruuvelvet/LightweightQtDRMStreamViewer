#ifndef PROVIDEREDITWIDGET_HPP
#define PROVIDEREDITWIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <Core/StreamingProviderStore.hpp>

class ProviderEditWidget : public QWidget
{
    Q_OBJECT

public:
    ProviderEditWidget(QWidget *parent = nullptr);
    ~ProviderEditWidget();

public slots:
    void setProvider(const Provider &provider);

private:
    Provider provider;

    void _update();
};

#endif // PROVIDEREDITWIDGET_HPP
