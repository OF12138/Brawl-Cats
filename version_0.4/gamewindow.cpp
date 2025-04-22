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
    isGrass.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改草丛位置
    isGrass[12][12] = true;
    isGrass[12][13] = true;
    isGrass[12][14]=true;
    isGrass[12][15]=true;

    // 初始化block标记
    isBlock.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改block位置
    isBlock[2][3] = true;
    isBlock[2][2] = true;
    isBlock[2][4]=true;
    isBlock[2][5]=true;

    // 加载图片
    gridImage.load(":/images/resources/images/grid.png");
    grassImage.load(":/images/resources/images/grass.png");
    grassTransparentImage.load(":/images/resources/images/grass_trans.png");
    blockImage.load(":/images/resources/images/block.png"); // 新增：加载block图片

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
    for (int i = 0; i < mapWidthInGrids; ++i) {
        for (int j = 0; j < mapHeightInGrids; ++j) {
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
            } else if (isBlock[i][j]) {
                // 绘制block图片
                painter.drawPixmap(x, y, blockImage);
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

    // 绘制小地图
    drawMiniMap(painter);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
    targetPosition = event->pos() + QPointF(offsetX, offsetY);
    lastTargetPosition = targetPosition;
}

bool GameWindow::isColliding(const QPointF& newPosition)
{
    int gridX = static_cast<int>(newPosition.x() / gridSize);
    int gridY = static_cast<int>(newPosition.y() / gridSize);
    if (gridX >= 0 && gridX < mapWidthInGrids && gridY >= 0 && gridY < mapHeightInGrids) {
        return isBlock[gridX][gridY];
    }
    return false;
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
            QPointF newPosition = playerPosition + QPointF(directionX * playerSpeed, directionY * playerSpeed);

            if (!isColliding(newPosition)) {
                playerPosition = newPosition;
                lastTargetPosition = targetPosition;
                stepCount++;
            }
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
    int mapWidth = mapWidthInGrids * gridSize;
    int mapHeight = mapHeightInGrids * gridSize;

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

void GameWindow::drawMiniMap(QPainter& painter)
{
    const int maxMiniMapSize = 150; // 小地图最大尺寸
    const int borderWidth = 2;      // 小地图边框宽度

    // 计算大地图的实际宽度和高度
    int mapWidth = mapWidthInGrids * gridSize;
    int mapHeight = mapHeightInGrids * gridSize;

    // 计算小地图的缩放比例，确保小地图尺寸和大地图比例一致
    double scale = std::min(static_cast<double>(maxMiniMapSize) / mapWidth, static_cast<double>(maxMiniMapSize) / mapHeight);
    int miniMapWidth = static_cast<int>(mapWidth * scale);
    int miniMapHeight = static_cast<int>(mapHeight * scale);

    // 绘制小地图边框
    painter.setPen(QPen(Qt::black, borderWidth));
    painter.drawRect(0, 0, miniMapWidth, miniMapHeight);

    // 绘制小地图内容，按格子逻辑绘制
    for (int i = 0; i < mapWidthInGrids; ++i) {
        for (int j = 0; j < mapHeightInGrids; ++j) {
            int x = static_cast<int>(i * scale * gridSize + borderWidth);
            int y = static_cast<int>(j * scale * gridSize + borderWidth);
            int w = static_cast<int>((i + 1) * scale * gridSize + borderWidth) - x;
            int h = static_cast<int>((j + 1) * scale * gridSize + borderWidth) - y;

            if (isBlock[i][j]) {
                painter.fillRect(x, y, w, h, Qt::black);
            } else if (isGrass[i][j]) {
                painter.fillRect(x, y, w, h, Qt::green);
            } else {
                painter.fillRect(x, y, w, h, Qt::lightGray);
            }
        }
    }

    // 绘制玩家在小地图上的位置
    int playerX = static_cast<int>(playerPosition.x() * scale + borderWidth);
    int playerY = static_cast<int>(playerPosition.y() * scale + borderWidth);
    painter.fillRect(playerX - 2, playerY - 2, 4, 4, Qt::red);
}
