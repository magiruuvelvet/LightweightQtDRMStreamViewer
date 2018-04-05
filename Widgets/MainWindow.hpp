#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <Gui/BaseWindow.hpp>
#include <Gui/FlowLayout.hpp>

#include <QList>

class MainWindow : public BaseWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void launchBrowserWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QVBoxLayout *_lV_main;

    FlowLayout *_lF_providerButtonList;
    QList<QPushButton*> _providerBtns;
};

#endif // MAINWINDOW_HPP
