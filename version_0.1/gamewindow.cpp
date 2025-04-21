#include "gamewindow.h"
#include <cmath>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    backgroundImage.load(":/images/resources/images/background.jpeg");
    playerImage.load(":/images/resources/images/player.png");
    // 初始化玩家
    playerPosition = QPointF(width() / 2, height() / 2);
    targetPosition = playerPosition;
    lastTargetPosition = targetPosition;

    playerSpeed = 2.0;                             // 玩家移动速度
    setFixedSize(800, 600);                     // 窗口大小

    setMouseTracking(true);     
    timerId = startTimer(16);
}
GameWindow::~GameWindow() {killTimer(timerId);}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // 镜头偏移量
    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
    //draw map & player
    painter.drawPixmap(-offsetX, -offsetY, backgroundImage);
    painter.drawPixmap(width() / 2 - playerImage.width() / 2, height() / 2 - playerImage.height() / 2, playerImage);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
    targetPosition = event->pos() + QPointF(offsetX, offsetY);
    lastTargetPosition = targetPosition;
}

// 定时器事件
void GameWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId) 
    {
        
        double dx = targetPosition.x() - playerPosition.x();
        double dy = targetPosition.y() - playerPosition.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        
        double targetDx = targetPosition.x() - lastTargetPosition.x();
        double targetDy = targetPosition.y() - lastTargetPosition.y();
        double targetDistance = std::sqrt(targetDx * targetDx + targetDy * targetDy);

        //大于阈值
        if (distance > minMoveThreshold || targetDistance > 1.0) 
        {
            double directionX = dx / distance;
            double directionY = dy / distance;
            playerPosition += QPointF(directionX * playerSpeed, directionY * playerSpeed);
            // 更新上一次的目标位置为当前目标位置
            lastTargetPosition = targetPosition;
        } 
        else playerPosition = targetPosition;// 如果距离小于阈值，将玩家位置设置为目标位置

        ensurePlayerInBounds();//边界检查
        update(); //触发重绘
    }
}

void GameWindow::ensurePlayerInBounds()
{
    // 计算玩家图片宽度的一半
    int halfWidth = playerImage.width() / 2;
    // 计算玩家图片高度的一半
    int halfHeight = playerImage.height() / 2;
    // 获取背景图片的宽度，即地图的宽度
    int mapWidth = backgroundImage.width();
    // 获取背景图片的高度，即地图的高度
    int mapHeight = backgroundImage.height();

    if (playerPosition.x() - halfWidth < 0)  playerPosition.setX(halfWidth);
    else if (playerPosition.x() + halfWidth > mapWidth)  playerPosition.setX(mapWidth - halfWidth);
    if (playerPosition.y() - halfHeight < 0)  playerPosition.setY(halfHeight);
    else if (playerPosition.y() + halfHeight > mapHeight) playerPosition.setY(mapHeight - halfHeight);
}    