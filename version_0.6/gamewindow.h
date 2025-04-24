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
    void mousePressEvent(QMouseEvent *event) override; // 处理鼠标点击事件

private:
    // image
    QPixmap gridImage;
    QPixmap grassImage;
    QPixmap grassTransparentImage;
    QPixmap blockImage;
    QPixmap poolImage;
    QPixmap bulletImage; 
    QPixmap enemyCryImage; 
    QPixmap enemyDeadImage; 

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
    struct Enemy
    {
        QPointF position;
        QPointF targetPosition;
        int stepCount;
        int health = 2; // 新增：敌人血量，初始值为2
        bool isHurt = false; // 新增：敌人是否受伤
        int hurtStartTime = 0; // 新增：敌人受伤开始时间
    };
    std::vector<Enemy> enemies;
    int enemyTimerId;
    const double enemySpeed = 1.0;
    const double enemyStopDistance = 50.0;
    const int hurtDuration = 800; // 新增：受伤持续时间（毫秒）
    void spawnEnemy();
    void moveEnemies();
    bool isInGrass(const QPointF& position); // 新增：判断是否在草丛内

    // 子弹相关
    struct Bullet
    {
        QPointF position;
        QPointF direction;
        int startTime; // 新增：记录子弹发射时间
    };
    std::vector<Bullet> bullets; // 新增：子弹列表
    const double bulletSpeed = 4.0; // 新增：子弹速度，便于修改
    const int bulletLifetime = 2000; // 新增：子弹生命周期（毫秒）

    // 检测子弹与敌人的碰撞
    bool isBulletHittingEnemy(const Bullet& bullet, const Enemy& enemy);
};

#endif // GAMEWINDOW_H
