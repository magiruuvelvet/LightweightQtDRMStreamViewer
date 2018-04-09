#ifndef CONFIGWINDOW_HPP
#define CONFIGWINDOW_HPP

#include <Gui/BaseWindow.hpp>
#include <Widgets/ProviderEditWidget.hpp>

#include <QTableView>

class ConfigWindow : public BaseWindow
{
    Q_OBJECT

    friend class ProviderEditWidget;

public:
    explicit ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QVBoxLayout *_lV_main;

    QTableView *m_tblView;
    ProviderEditWidget *m_editWidget;
    QPushButton *m_saveBtn;
};

#endif // CONFIGWINDOW_HPP
