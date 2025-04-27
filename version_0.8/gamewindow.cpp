#include "gamewindow.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QTime>
#include <QUrl>
#include <QAudioOutput>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    gridSize = 100; // background像素
    playerSpeed = 2.0;
    setFixedSize(800, 600); // 游戏窗口大小
    setMouseTracking(true);
    timerId = startTimer(16);
    enemyTimerId = startTimer(2000); // 每2秒生成敌人
    stepCount = 0;
    enemyLastDirectionChangeTime.resize(0);//about enemy move

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

    // 初始化水池标记
    isPool.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改水池位置
    isPool[5][5] = true;
    isPool[5][6] = true;
    isPool[6][5] = true;
    isPool[6][6] = true;

    // 加载图片
    gridImage.load(":/images/resources/images/grid.png");
    grassImage.load(":/images/resources/images/grass.png");
    grassTransparentImage.load(":/images/resources/images/grass_trans.png");
    blockImage.load(":/images/resources/images/block.png");
    poolImage.load(":/images/resources/images/pool.png");
    bulletImage.load(":/images/resources/images/bullet.png");
    enemyCryImage.load(":/images/resources/images/enemy_cry.png");
    enemyDeadImage.load(":/images/resources/images/enemy_dead.png");
    endWinImage.load(":/images/resources/images/end_win.png");

    // 加载玩家图片
    for (int i = 0; i < 8; ++i) {
        QString rightPath = QString(":/images/resources/images/player_right_%1.png").arg(i + 1);
        playerImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/player_left_%1.png").arg(i + 1);
        playerImagesLeft[i].load(leftPath);
    }

    // 加载敌人图片
    for (int i = 0; i < 8; ++i) {
        QString rightPath = QString(":/images/resources/images/enemy_right_%1.png").arg(i + 1);
        enemyImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/enemy_left_%1.png").arg(i + 1);
        enemyImagesLeft[i].load(leftPath);
    }

    // 游戏开始时生成5个敌人
    spawnEnemy(5);

    // 初始化声音播放器
    audioOutput = new QAudioOutput(this);
    bulletHitSoundPlayer = new QMediaPlayer(this);
    bulletHitSoundPlayer->setAudioOutput(audioOutput);
    bulletHitSoundPlayer->setSource(QUrl("qrc:/images/resources/images/enemy_wow.mp3"));

    // 初始化胜利声音播放器
    winAudioOutput = new QAudioOutput(this);
    winSoundPlayer = new QMediaPlayer(this);
    winSoundPlayer->setAudioOutput(winAudioOutput);
    winSoundPlayer->setSource(QUrl("qrc:/images/resources/images/player_laugh.mp3"));

    // 调整音量（可选）
    winAudioOutput->setVolume(50);
    audioOutput->setVolume(50);



}

GameWindow::~GameWindow()
{
    killTimer(timerId);
    killTimer(enemyTimerId);
    delete bulletHitSoundPlayer;
}


void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (gameState == GameState::StartPage)
    {
        QPixmap startImage;
        startImage.load(":/images/resources/images/start.png");
        painter.drawPixmap(0, 0, width(), height(), startImage);
        return;
    }

    if (gameWon)
    {
        painter.drawPixmap(0, 0, width(), height(), endWinImage);
        return;
    }

    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;

    // 绘制方格背景
    for (int i = 0; i < mapWidthInGrids; ++i)
    {
        for (int j = 0; j < mapHeightInGrids; ++j)
        {
            int x = i * gridSize - offsetX;
            int y = j * gridSize - offsetY;

            if (isGrass[i][j])
            {
                int playerGridX = static_cast<int>(playerPosition.x() / gridSize);
                int playerGridY = static_cast<int>(playerPosition.y() / gridSize);
                // 玩家在草丛内，显示半透明草丛图片
                if (i == playerGridX && j == playerGridY)  painter.drawPixmap(x, y, grassTransparentImage);
                // 玩家不在草丛内，显示普通（不透明）草丛图片
                else  painter.drawPixmap(x, y, grassImage);
            }
            // 绘制block图片
            else if (isBlock[i][j])  painter.drawPixmap(x, y, blockImage);
            // 绘制水池图片
            else if (isPool[i][j])  painter.drawPixmap(x, y, poolImage);
            // 普通方格图片
            else painter.drawPixmap(x, y, gridImage);
        }
    }

    // 绘制玩家
    QPixmap currentPlayerImage;
    double dx = targetPosition.x() - playerPosition.x();
    if (dx > 0) currentPlayerImage = playerImagesRight[stepCount % 8];
    else if (dx < 0)  currentPlayerImage = playerImagesLeft[stepCount % 8];
    else currentPlayerImage = playerImagesRight[0];
    painter.drawPixmap(width() / 2 - currentPlayerImage.width() / 2, height() / 2 - currentPlayerImage.height() / 2, currentPlayerImage);

    // 绘制敌人
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        if (!isInGrass(it->position))
        { // 只绘制不在草丛内的敌人
            QPixmap currentEnemyImage;
            if (it->isHurt && (currentTime - it->hurtStartTime) < hurtDuration)
            {
                if (it->health > 0)  currentEnemyImage = enemyCryImage;
                else  currentEnemyImage = enemyDeadImage;
            }
            else
            {
                if (it->isFacingRight) {
                    currentEnemyImage = enemyImagesRight[it->stepCount % 8];
                } else {
                    currentEnemyImage = enemyImagesLeft[it->stepCount % 8];
                }
            }
            int enemyX = static_cast<int>(it->position.x()) - offsetX - currentEnemyImage.width() / 2;
            int enemyY = static_cast<int>(it->position.y()) - offsetY - currentEnemyImage.height() / 2;
            painter.drawPixmap(enemyX, enemyY, currentEnemyImage);

            // 绘制血条
            int healthBarWidth = 40;
            int healthBarHeight = 3;
            int healthBarX = enemyX + currentEnemyImage.width() / 2 - healthBarWidth / 2;
            int healthBarY = enemyY - healthBarHeight - 2;
            painter.fillRect(healthBarX, healthBarY, healthBarWidth * (it->health / 2.0), healthBarHeight, Qt::red);
            painter.setPen(Qt::black);
            painter.drawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight);
        }
        if (it->isHurt && (currentTime - it->hurtStartTime) >= hurtDuration && it->health <= 0)
        {
            score++; // 敌人消失，积分加一
            scoreLabel->setText(QString("当前积分: %1 /10").arg(score)); // 更新积分显示
            if (score >= 10)
            {
                gameWon = true;
                winStartTime = currentTime;
                update(); // 强制更新窗口以显示胜利图片
            }
            it = enemies.erase(it);
        }
        else  ++it;
    }

    // 绘制子弹
    for (const auto& bullet : bullets)
    {
        int bulletX = static_cast<int>(bullet.position.x()) - offsetX - bulletImage.width() / 2;
        int bulletY = static_cast<int>(bullet.position.y()) - offsetY - bulletImage.height() / 2;
        painter.drawPixmap(bulletX, bulletY, bulletImage);
    }

    // 绘制小地图
    drawMiniMap(painter);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (gameWon) return;
    int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
    int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
    targetPosition = event->pos() + QPointF(offsetX, offsetY);
    lastTargetPosition = targetPosition;
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
    if (gameState == GameState::StartPage)
    {
        gameState = GameState::GamePage;
        resetGame();
        update();
        return;
    }

    if (gameWon) return;
    if (event->button() == Qt::LeftButton)
    {
        int offsetX = static_cast<int>(playerPosition.x()) - width() / 2;
        int offsetY = static_cast<int>(playerPosition.y()) - height() / 2;
        QPointF clickPosition = event->pos() + QPointF(offsetX, offsetY);

        double dx = clickPosition.x() - playerPosition.x();
        double dy = clickPosition.y() - playerPosition.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 0)
        {
            double directionX = dx / distance;
            double directionY = dy / distance;

            Bullet bullet;
            bullet.position = playerPosition;
            bullet.direction = QPointF(directionX, directionY);
            bullet.startTime = QTime::currentTime().msecsSinceStartOfDay();
            bullets.push_back(bullet);
        }
    }
}

bool GameWindow::isColliding(const QPointF& newPosition)
{
    int gridX = static_cast<int>(newPosition.x() / gridSize);
    int gridY = static_cast<int>(newPosition.y() / gridSize);
    if (gridX >= 0 && gridX < mapWidthInGrids && gridY >= 0 && gridY < mapHeightInGrids)
    {
        return isBlock[gridX][gridY] || isPool[gridX][gridY]; // 新增：考虑水池碰撞
    }
    return false;
}

bool GameWindow::isInGrass(const QPointF& position)
{
    int gridX = static_cast<int>(position.x() / gridSize);
    int gridY = static_cast<int>(position.y() / gridSize);
    if (gridX >= 0 && gridX < mapWidthInGrids && gridY >= 0 && gridY < mapHeightInGrids)
    {
        return isGrass[gridX][gridY];
    }
    return false;
}

bool GameWindow::isBulletHittingEnemy(const Bullet& bullet, const Enemy& enemy)
{
    // 碰撞检测逻辑
    double dx = bullet.position.x() - enemy.position.x();
    double dy = bullet.position.y() - enemy.position.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    if (distance <= 50) // 碰撞半径为50
    {
        // 检测到碰撞，播放声音
        if (bulletHitSoundPlayer->playbackState() == QMediaPlayer::PlayingState)
        {
            bulletHitSoundPlayer->stop();
        }
        bulletHitSoundPlayer->setPosition(0); // 确保音频从开头播放
        bulletHitSoundPlayer->play();
        return true;
    }
    return false;
}

void GameWindow::timerEvent(QTimerEvent *event)
{
    if (gameState == GameState::StartPage)  return;

    if (gameWon)
    {
        int currentTime = QTime::currentTime().msecsSinceStartOfDay();
        if (currentTime - winStartTime >= 4000)
        {
            emit gameEnded(); // 发出游戏结束信号
        }
        if (currentTime - winStartTime >= 0) winSoundPlayer->play();
        return;
    }

    if (event->timerId() == timerId)
    {
        double dx = targetPosition.x() - playerPosition.x();
        double dy = targetPosition.y() - playerPosition.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        double targetDx = targetPosition.x() - lastTargetPosition.x();
        double targetDy = targetPosition.y() - lastTargetPosition.y();
        double targetDistance = std::sqrt(targetDx * targetDx + targetDy * targetDy);

        if (distance > minMoveThreshold || targetDistance > 1.0)
        {
            double directionX = dx / distance;
            double directionY = dy / distance;
            QPointF newPosition = playerPosition + QPointF(directionX * playerSpeed, directionY * playerSpeed);

            if (!isColliding(newPosition))
            {
                playerPosition = newPosition;
                lastTargetPosition = targetPosition;
                stepCount++;
            }
        }
        else playerPosition = targetPosition;

        ensurePlayerInBounds();
        moveEnemies();

        // 更新子弹状态
        int currentTime = QTime::currentTime().msecsSinceStartOfDay();
        std::vector<Bullet> newBullets;
        for (const auto& bullet : bullets)
        {
            QPointF newPosition = bullet.position + bullet.direction * bulletSpeed;
            bool bulletHit = false;
            for (auto& enemy : enemies)
            {
                if (isBulletHittingEnemy(bullet, enemy))
                {
                    enemy.health--;
                    enemy.isHurt = true;
                    enemy.hurtStartTime = currentTime;
                    bulletHit = true;
                    break;
                }
            }
            if (!isColliding(newPosition) && (currentTime - bullet.startTime) < bulletLifetime && !bulletHit)
            {
                Bullet newBullet = bullet;
                newBullet.position = newPosition;
                newBullets.push_back(newBullet);
            }
        }
        bullets = newBullets;

        update();
    }
    else if (event->timerId() == enemyTimerId)
    {
        spawnEnemy(1); // 每2秒生成3个敌人
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

void GameWindow::spawnEnemy(int count)
{
    srand(time(nullptr));
    for (int i = 0; i < count; ++i)
    {
        Enemy enemy;
        bool validPosition = false;
        while (!validPosition)
        {
            int x = rand() % (mapWidthInGrids * gridSize);
            int y = rand() % (mapHeightInGrids * gridSize);
            enemy.position = QPointF(x, y);
            if (!isColliding(enemy.position) && !isInGrass(enemy.position))
            {
                validPosition = true;
            }
        }
        enemy.targetPosition = playerPosition;
        enemy.stepCount = 0;
        enemies.push_back(enemy);
        enemyLastDirectionChangeTime.push_back(QTime::currentTime().msecsSinceStartOfDay()); // 记录敌人创建时的时间
    }
}

void GameWindow::moveEnemies()
{
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();
    for (size_t i = 0; i < enemies.size(); ++i)
    {
        auto& enemy = enemies[i];
        double dx = playerPosition.x() - enemy.position.x();
        double dy = playerPosition.y() - enemy.position.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        // 判断玩家是否在草丛内
        int playerGridX = static_cast<int>(playerPosition.x() / gridSize);
        int playerGridY = static_cast<int>(playerPosition.y() / gridSize);
        bool playerInGrass = playerGridX >= 0 && playerGridX < mapWidthInGrids && playerGridY >= 0 && playerGridY < mapHeightInGrids && isGrass[playerGridX][playerGridY];

        // 每个敌人的随机时间偏移量，范围在 0 到 1000 毫秒之间
        static std::vector<int> enemyTimeOffsets;
        if (enemyTimeOffsets.size() != enemies.size())
        {
            enemyTimeOffsets.resize(enemies.size());
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            // 0 到 1000 毫秒的随机偏移
            for (size_t j = 0; j < enemies.size(); ++j) enemyTimeOffsets[j] = std::rand() % 1000;
        }

        // 若敌人处于受伤持续时间内，不移动
        if (enemy.isHurt && (currentTime - enemy.hurtStartTime) < hurtDuration) continue;

        if (playerInGrass || distance > 600)
        {
            // 玩家在草丛内或者玩家距离敌人 600 像素以上，敌人自由运动，每 2 秒改变一次移动方向
            if (currentTime - enemyLastDirectionChangeTime[i] >= 2000 + enemyTimeOffsets[i])
            {
                // 为每个敌人设置独立的随机数种子
                std::srand(static_cast<unsigned int>(i + currentTime));
                int randomX = std::rand() % 3 - 1; // -1, 0, 1
                int randomY = std::rand() % 3 - 1; // -1, 0, 1
                enemy.targetPosition = enemy.position + QPointF(randomX * enemySpeed * 2000, randomY * enemySpeed * 2000); // 2 秒的移动距离
                enemyLastDirectionChangeTime[i] = currentTime;
            }

            dx = enemy.targetPosition.x() - enemy.position.x();
            dy = enemy.targetPosition.y() - enemy.position.y();
            distance = std::sqrt(dx * dx + dy * dy);

            if (distance > enemyStopDistance)
            {
                double directionX = dx / distance;
                double directionY = dy / distance;
                QPointF newPosition = enemy.position + QPointF(directionX * enemySpeed, directionY * enemySpeed);

                if (!isColliding(newPosition))
                {
                    enemy.position = newPosition;
                    enemy.stepCount++;
                }
            }
        }
        else
        {
            // 玩家不在草丛内并且在敌人 600 像素以内，敌人朝着玩家运动
            if (distance > enemyStopDistance)
            {
                double directionX = dx / distance;
                double directionY = dy / distance;
                QPointF newPosition = enemy.position + QPointF(directionX * enemySpeed, directionY * enemySpeed);

                if (!isColliding(newPosition))
                {
                    enemy.position = newPosition;
                    enemy.stepCount++;
                }
            }
        }

        // 更新敌人图片的左右朝向
        if (dx > 0)  enemy.isFacingRight = true;
        else if (dx < 0)  enemy.isFacingRight = false;
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
    for (int i = 0; i < mapWidthInGrids; ++i)
    {
        for (int j = 0; j < mapHeightInGrids; ++j)
        {
            int x = static_cast<int>(i * scale * gridSize + borderWidth);
            int y = static_cast<int>(j * scale * gridSize + borderWidth);
            int w = static_cast<int>((i + 1) * scale * gridSize + borderWidth) - x;
            int h = static_cast<int>((j + 1) * scale * gridSize + borderWidth) - y;

            if (isBlock[i][j]) painter.fillRect(x, y, w, h, Qt::black);
            else if (isGrass[i][j])  painter.fillRect(x, y, w, h, Qt::green);
            else if (isPool[i][j]) painter.fillRect(x, y, w, h, Qt::blue);
            else painter.fillRect(x, y, w, h, Qt::white);
        }
    }

    // 绘制玩家在小地图上的位置
    int playerMiniX = static_cast<int>(playerPosition.x() * scale + borderWidth);
    int playerMiniY = static_cast<int>(playerPosition.y() * scale + borderWidth);
    painter.setBrush(Qt::darkYellow);
    painter.setPen(Qt::NoPen); // 设置无描边
    // 绘制黄色方格代表玩家
    painter.drawRect(playerMiniX - 2, playerMiniY - 2, 4, 4);

    // 绘制等效大地图 600 像素的圆圈
    int circleRadius = static_cast<int>(600 * scale); // 计算等效的小地图半径
    painter.setPen(QPen(Qt::gray, 1)); // 设置画笔
    painter.setBrush(Qt::NoBrush); // 不填充
    painter.drawEllipse(playerMiniX - circleRadius, playerMiniY - circleRadius, 2 * circleRadius, 2 * circleRadius);

    // 绘制不在草丛里且距离玩家 600 像素以内的敌人
    const double enemyVisibilityRange = 600;
    for (const auto& enemy : enemies) {
        if (!isInGrass(enemy.position)) {
            double dx = enemy.position.x() - playerPosition.x();
            double dy = enemy.position.y() - playerPosition.y();
            double distance = std::sqrt(dx * dx + dy * dy);
            if (distance <= enemyVisibilityRange)
            {
                int enemyMiniX = static_cast<int>(enemy.position.x() * scale + borderWidth);
                int enemyMiniY = static_cast<int>(enemy.position.y() * scale + borderWidth);
                painter.setBrush(Qt::red);
                painter.setPen(Qt::NoPen);
                // 绘制红色方格代表敌人
                painter.drawRect(enemyMiniX - 2, enemyMiniY - 2, 4, 4);
            }
        }
    }
}

void GameWindow::resetGame()
{
    // 创建积分显示标签
    if (!scoreLabel) {
        scoreLabel = new QLabel(this);
        scoreLabel->setGeometry(300, 20, 200, 60);
        scoreLabel->setStyleSheet("background-color: white; color: black; font-size: 20px;");
    }
    scoreLabel->setText(QString("当前积分: %1 /10").arg(score));
    scoreLabel->show();

    // 重置游戏状态为进行中
    gameState = GameState::GamePage; // 新增：强制设置状态
    score = 0;
    gameWon = false;
    winStartTime = 0;
    playerPosition = QPointF(width() / 2, height() / 2);
    targetPosition = playerPosition;
    lastTargetPosition = targetPosition;
    stepCount = 0;
    bullets.clear();
    enemies.clear();
    enemyLastDirectionChangeTime.clear();
    spawnEnemy(5);
}
