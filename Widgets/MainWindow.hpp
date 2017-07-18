#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
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

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseMove(QPoint newPos, QPoint oldPos);

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

    QPoint m_clickPos;
    QPoint mClickedPos;
    bool mMousePressed = false;
    bool left;
    bool right;
    bool bottom;
};

#endif // MAINWINDOW_HPP
