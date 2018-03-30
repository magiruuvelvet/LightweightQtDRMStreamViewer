#ifndef BASEWINDOW_HPP
#define BASEWINDOW_HPP

#include <QWidget>

#include <QEvent>
#include <QMouseEvent>

#include "TitleBar.hpp"

class BaseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow();

    void setFixedSize(const QSize &size);
    void setFixedSize(int w, int h);

    TitleBar *titleBar()
    { return this->tb; }

    QWidget *containerWidget()
    { return this->_containerWidget; }

    void setContainerLayoutContentsMargins(int left, int top, int right, int bottom);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMove(QPoint newPos, QPoint oldPos);

    void showEvent(QShowEvent *e);

private:
    QVBoxLayout *layout;
    QVBoxLayout *titleBarContainer;
    QVBoxLayout *containerLayout;
    QWidget *_containerWidget;

    TitleBar *tb;

    QPoint mClickedPos;
    bool fixedSize = false;
    bool mMousePressed = false;
    bool top;
    bool left;
    bool right;
    bool bottom;
};

#endif // BASEWINDOW_HPP
