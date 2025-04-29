// gamewindow.h
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <vector>
#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QAudioOutput>
#include <QLabel>
#include <QColor>

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    void resetGame(); // 新增：重置游戏状态
    void startGameMusic();
    void stopGameMusic();
    void stopLoseMusic();
    void stopTimers(); // 新增：停止所有定时器
    void startTimers();

signals:
    void gameEnded(); // 新增：游戏结束信号

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
    QPixmap endWinImage; // 新增：胜利图片

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
    int mapWidthInGrids = 15;  // 大地图横向格子数量
    int mapHeightInGrids = 15; // 大地图纵向格子数量
    void drawMiniMap(QPainter& painter); // 绘制小地图的函数
    bool isColliding(const QPointF& newPosition, bool checkPool,bool isbullet=false);

    // 敌人相关
    QPixmap enemyImagesRight[8];
    QPixmap enemyImagesLeft[8];
    struct Enemy
    {
        QPointF position;
        QPointF targetPosition;
        int stepCount;
        int health = 2;
        bool isHurt = false;
        int hurtStartTime = 0;
        bool isFacingRight = true; // 新增：存储敌人的朝向信息
    };
    std::vector<Enemy> enemies;
    int enemyTimerId;
    const double enemySpeed = 1.0;
    const double enemyStopDistance = 50.0;
    const int hurtDuration = 800; // 新增：受伤持续时间（毫秒）
    void spawnEnemy(int count); // 修改：支持生成指定数量的敌人
    void moveEnemies();
    bool isInGrass(const QPointF& position); // 新增：判断是否在草丛内
    std::vector<int> enemyLastDirectionChangeTime; // 记录每个敌人上一次改变方向的时间

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

    // 积分相关
    int score = 0; // 新增：积分
    bool gameWon = false; // 新增：游戏是否胜利
    int winStartTime = 0; // 新增：胜利开始时间
    QLabel* scoreLabel=nullptr; //延迟初始化

    // 声音播放
    QMediaPlayer* bulletHitSoundPlayer;
    QAudioOutput* audioOutput;
    QMediaPlayer* winSoundPlayer;
    QAudioOutput* winAudioOutput;
    QMediaPlayer* gameMusicPlayer;
    QAudioOutput* gameAudioOutput;
    int lastLoseSoundPlayTime = 0; // 记录上一次播放 lose.mp3 的时间

    // 新增：游戏状态枚举
    enum GameState { StartPage, GamePage };
    GameState gameState = StartPage; // 初始状态为开始页面

    // 技能相关
    QPixmap playerSkillImagesRight[2]; // 技能向右的图片
    QPixmap playerSkillImagesLeft[2];  // 技能向左的图片
    bool isSkillActive = false;        // 技能是否激活
    int skillStartTime = 0;            // 技能开始时间
    const int skillDuration = 3000;    // 技能持续时间（毫秒）
    QMediaPlayer* skillSoundPlayer;    // 技能声音播放器
    QAudioOutput* skillAudioOutput;    // 技能音频输出
    void activateSkill();              // 激活技能
    void deactivateSkill();            // 停用技能
    QList<QPair<QPoint, int>> modifiedGrids; // 存储被修改的格子坐标和恢复时间
    QHash<QPoint, bool> originalGrassState;   // 记录原始草丛状态
    bool isRecordingPath = false;            // 是否正在记录路径
    QPoint lastPlayerGrid;  // 记录玩家上一个所在格子坐标
    QHash<QPoint, int> activePoolGrids; // 当前激活的水池格子及恢复时间
    QHash<QPoint, bool> originalGridStates; // 原始格子状态（草丛/普通）
    // 技能冷却时间相关
    int lastSkillUseTime = 0;
    const int skillCooldown = 10000; // 10秒，单位：毫秒

    // 普攻冷却时间相关
    int lastAttackTime = 0;
    const int attackCooldown = 300; // 0.3秒，单位：毫秒

    // 蓝条相关
    int manaBarWidth = 60; // 蓝条初始宽度
    int currentMana = manaBarWidth; // 当前蓝条宽度

    //玩家血量相关成员
    int playerHealth = 10; // 玩家初始血量
    const int playerMaxHealth = 10; // 玩家最大血量
    bool playerIsHurt = false; // 玩家是否受击标志
    int playerHurtStartTime = 0; // 玩家受击开始时间
    const int playerHurtDuration = 400; // 玩家受击持续时间（毫秒）
    QPixmap playerOhImagesRight[1]; // 玩家受击向右的图片
    QPixmap playerOhImagesLeft[1];  // 玩家受击向左的图片
    QMediaPlayer* playerHurtSoundPlayer; // 玩家受击声音播放器
    QAudioOutput* playerHurtAudioOutput; // 玩家受击音频输出
    // 新增：敌人攻击间隔相关成员
    std::vector<int> enemyLastAttackTime; // 记录每个敌人上一次攻击的时间
    const int enemyAttackCooldown = 1000; // 敌人攻击冷却时间（毫秒）

    // 失败相关
    QPixmap endLoseImage; // 新增：失败图片
    QMediaPlayer* loseSoundPlayer; // 新增：失败声音播放器
    QAudioOutput* loseAudioOutput; // 新增：失败音频输出
    bool gameLost = false; // 新增：游戏是否失败
    int loseStartTime = 0; // 新增：失败开始时间

};

#endif // GAMEWINDOW_H
