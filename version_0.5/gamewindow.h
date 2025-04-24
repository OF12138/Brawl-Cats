#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <vector>
#include <QTimer>

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
    // image
    QPixmap gridImage;
    QPixmap grassImage;
    QPixmap grassTransparentImage;
    QPixmap blockImage;
    QPixmap poolImage;

    // 标记
    std::vector<std::vector<bool>> isGrass;
    std::vector<std::vector<bool>> isBlock;
    std::vector<std::vector<bool>> isPool;

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

    // 敌人相关
    QPixmap enemyImagesRight[8];
    QPixmap enemyImagesLeft[8];
    struct Enemy {
        QPointF position;
        QPointF targetPosition;
        int stepCount;
    };
    std::vector<Enemy> enemies;
    int enemyTimerId;
    const double enemySpeed = 1.0;
    const double enemyStopDistance = 50.0;
    void spawnEnemy();
    void moveEnemies();
    bool isInGrass(const QPointF& position); // 新增：判断是否在草丛内
};

#endif // GAMEWINDOW_H
