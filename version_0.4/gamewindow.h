#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <vector>

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    QPixmap gridImage;
    QPixmap grassImage;
    QPixmap grassTransparentImage;
    QPixmap blockImage;
    std::vector<std::vector<bool>> isGrass;
    std::vector<std::vector<bool>> isBlock;
    QPixmap playerImagesRight[8];
    QPixmap playerImagesLeft[8];
    QPointF playerPosition;
    QPointF targetPosition;
    QPointF lastTargetPosition;
    double playerSpeed;
    int timerId;
    const double minMoveThreshold = 3.0;
    int stepCount;
    void ensurePlayerInBounds();
    int gridSize;
    int mapWidthInGrids = 40;  // 大地图横向格子数量
    int mapHeightInGrids = 30; // 大地图纵向格子数量
    void drawMiniMap(QPainter& painter); // 绘制小地图的函数
    bool isColliding(const QPointF& newPosition);
};

#endif // GAMEWINDOW_H
