#ifndef TITLEBAR_HPP
#define TITLEBAR_HPP

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QMouseEvent>

#include <QLabel>
#include <QPixmap>

#include <functional>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

    void addButton(const QString &text, const std::function<void()> &receiver);

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

    QHBoxLayout *m_layout;
    QLabel *m_title;
    QLabel *m_icon;

    QPushButton *m_closeBtn;
    QList<QPushButton*> buttons;
};

#endif // TITLEBAR_HPP
