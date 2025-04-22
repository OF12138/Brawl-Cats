#include "gamewindow.h"
#include <cmath>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    gridSize = 100;
    playerSpeed = 2.0;
    setFixedSize(800, 600);
    setMouseTracking(true);
    timerId = startTimer(16);
    stepCount = 0;

    playerPosition = QPointF(width() / 2, height() / 2);
    targetPosition = playerPosition;
    lastTargetPosition = targetPosition;

    // 初始化草丛标记
    isGrass.resize(30, std::vector<bool>(30, false));
    isGrass[12][12] = true;
    isGrass[12][13] = true;

    // 加载图片
    gridImage.load(":/images/resources/images/grid.png");
    grassImage.load(":/images/resources/images/grass.png");
    grassTransparentImage.load(":/images/resources/images/grass_trans.png");

    // 加载玩家图片
    for (int i = 0; i < 8; ++i) {
        QString rightPath = QString(":/images/resources/images/player_right_%1.png").arg(i + 1);
        playerImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/player_left_%1.png").arg(i + 1);
        playerImagesLeft[i].load(leftPath);
    }
}

GameWindow::~GameWindow()
{
    killTimer(timerId);
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;

    // 绘制方格背景
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            int x = i * gridSize - offsetX;
            int y = j * gridSize - offsetY;

            if (isGrass[i][j]) {
                int playerGridX = static_cast<int>(playerPosition.x() / gridSize);
                int playerGridY = static_cast<int>(playerPosition.y() / gridSize);
                if (i == playerGridX && j == playerGridY) {
                    // 玩家在草丛内，显示半透明草丛图片
                    painter.drawPixmap(x, y, grassTransparentImage);
                } else {
                    // 玩家不在草丛内，显示普通（不透明）草丛图片
                    painter.drawPixmap(x, y, grassImage);
                }
            } else {
                // 不是草丛，显示方格图片
                painter.drawPixmap(x, y, gridImage);
            }
        }
    }

    // 绘制玩家
    QPixmap currentPlayerImage;
    double dx = targetPosition.x() - playerPosition.x();
    if (dx > 0) {
        currentPlayerImage = playerImagesRight[stepCount % 8];
    } else if (dx < 0) {
        currentPlayerImage = playerImagesLeft[stepCount % 8];
    } else {
        currentPlayerImage = playerImagesRight[0];
    }
    painter.drawPixmap(width() / 2 - currentPlayerImage.width() / 2, height() / 2 - currentPlayerImage.height() / 2, currentPlayerImage);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
    targetPosition = event->pos() + QPointF(offsetX, offsetY);
    lastTargetPosition = targetPosition;
}

void GameWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId) {
        double dx = targetPosition.x() - playerPosition.x();
        double dy = targetPosition.y() - playerPosition.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        double targetDx = targetPosition.x() - lastTargetPosition.x();
        double targetDy = targetPosition.y() - lastTargetPosition.y();
        double targetDistance = std::sqrt(targetDx * targetDx + targetDy * targetDy);

        if (distance > minMoveThreshold || targetDistance > 1.0) {
            double directionX = dx / distance;
            double directionY = dy / distance;
            playerPosition += QPointF(directionX * playerSpeed, directionY * playerSpeed);
            lastTargetPosition = targetPosition;
            stepCount++;
        } else {
            playerPosition = targetPosition;
        }

        ensurePlayerInBounds();
        update();
    }
}

void GameWindow::ensurePlayerInBounds()
{
    int halfWidth = playerImagesRight[0].width() / 2;
    int halfHeight = playerImagesRight[0].height() / 2;
    int mapWidth = 30 * gridSize;
    int mapHeight = 30 * gridSize;

    if (playerPosition.x() - halfWidth < 0) {
        playerPosition.setX(halfWidth);
    } else if (playerPosition.x() + halfWidth > mapWidth) {
        playerPosition.setX(mapWidth - halfWidth);
    }

    if (playerPosition.y() - halfHeight < 0) {
        playerPosition.setY(halfHeight);
    } else if (playerPosition.y() + halfHeight > mapHeight) {
        playerPosition.setY(mapHeight - halfHeight);
    }
}
