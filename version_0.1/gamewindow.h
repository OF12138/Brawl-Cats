#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    // 定时器事件，用于定时更新游戏状态
    void timerEvent(QTimerEvent *event) override;

private:
    QPixmap backgroundImage;
    QPixmap playerImage;
    QPointF playerPosition;  //player location
    QPointF targetPosition;  //mouse location
    QPointF lastTargetPosition;
    double playerSpeed;
    int timerId;   // 定时器的 ID，用于管理定时器事件
    const double minMoveThreshold = 3.0;  //最小移动阈值
    void ensurePlayerInBounds();
};

#endif // GAMEWINDOW_H    