#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLabel>

#include <QList>

#include "BrowserWindow.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadProfile();

private:
    QVBoxLayout *_lV_main;
    QHBoxLayout *_lH_providerButtonList, *_lH_titleBar;
    QWidget *_rootWidget;

    QLabel *_titleBarText;
    QPushButton *_titleBarBtn;
    QList<QPushButton*> _providerBtns;

    BrowserWindow *browser;
};

#endif // MAINWINDOW_HPP
