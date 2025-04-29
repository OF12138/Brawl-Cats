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
    lastPlayerGrid = QPoint(-1, -1); // 初始化为无效坐标

    // 计算地图中央位置
    int centerX = mapWidthInGrids * gridSize / 2;
    int centerY = mapHeightInGrids * gridSize / 2;
    playerPosition = QPointF(centerX, centerY);
    targetPosition = playerPosition;
    lastTargetPosition = targetPosition;

    // 初始化草丛标记
    isGrass.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改草丛位置
    {
    isGrass[5][2] = true;
    isGrass[5][3] = true;
    isGrass[5][4] = true;
    isGrass[5][5] = true;
    isGrass[6][5] = true;
    isGrass[7][5] = true;
    //isGrass[6][6] = true;
    //isGrass[7][6] = true;
    isGrass[11][6] = true;
    isGrass[12][6] = true;
    isGrass[11][7] = true;
    isGrass[12][7] = true;
    isGrass[13][7] = true;
    isGrass[13][9] = true;
    isGrass[14][9] = true;
    isGrass[6][9] = true;
    isGrass[7][9] = true;
    isGrass[8][9] = true;
    isGrass[8][10] = true;
    isGrass[8][11] = true;
    isGrass[8][12] = true;
    isGrass[0][11] = true;
    isGrass[1][11] = true;
    isGrass[2][11] = true;
    isGrass[2][12] = true;
    isGrass[3][12] = true;
    isGrass[13][12] = true;
    isGrass[13][13] = true;
    isGrass[13][14] = true;
    isGrass[12][14] = true;
    isGrass[13][14] = true;
    isGrass[14][14] = true;
    isGrass[1][0] = true;
    isGrass[1][1] = true;
    isGrass[1][2] = true;
    isGrass[2][2] = true;
    }

    // 初始化block标记
    isBlock.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改block位置
    {
    isBlock[2][1] = true;
    isBlock[3][1] = true;
    isBlock[3][2] = true;
    isBlock[4][2] = true;
    isBlock[4][3] = true;
    isBlock[7][1] = true;
    isBlock[8][1] = true;
    isBlock[8][2] = true;
    isBlock[9][4] = true;
    isBlock[10][4] = true;
    isBlock[11][4] = true;
    isBlock[11][5] = true;
    isBlock[13][3] = true;
    isBlock[13][4] = true;
    isBlock[14][4] = true;
    isBlock[14][3] = true;
    isBlock[14][5] = true;
    isBlock[1][7] = true;
    isBlock[2][7] = true;
    isBlock[2][8] = true;
    isBlock[1][10] = true;
    isBlock[2][10] = true;
    isBlock[3][10] = true;
    isBlock[4][10] = true;
    isBlock[0][12] = true;
    isBlock[1][12] = true;
    isBlock[5][12] = true;
    isBlock[5][13] = true;
    isBlock[6][13] = true;
    isBlock[10][11] = true;
    isBlock[10][12] = true;
    isBlock[11][11] = true;
    isBlock[14][12] = true;
    }

    // 初始化水池标记
    isPool.resize(mapWidthInGrids, std::vector<bool>(mapHeightInGrids, false));
    // 可修改水池位置
    {
    isPool[6][2] = true;
    isPool[6][3] = true;
    isPool[6][4] = true;
    isPool[7][2] = true;
    isPool[7][3] = true;
    isPool[7][4] = true;
    isPool[0][4] = true;
    isPool[0][5] = true;
    isPool[1][4] = true;
    isPool[1][5] = true;
    isPool[2][4] = true;
    isPool[2][5] = true;
    isPool[12][1] = true;
    isPool[12][2] = true;
    isPool[13][1] = true;
    isPool[13][2] = true;
    isPool[14][1] = true;
    isPool[14][2] = true;
    isPool[9][5] = true;
    isPool[9][6] = true;
    isPool[9][7] = true;
    isPool[10][5] = true;
    isPool[10][6] = true;
    isPool[10][7] = true;
    isPool[3][7] = true;
    isPool[3][8] = true;
    isPool[3][9] = true;
    isPool[4][7] = true;
    isPool[4][8] = true;
    isPool[4][9] = true;
    isPool[6][10] = true;
    isPool[6][11] = true;
    isPool[6][12] = true;
    isPool[7][10] = true;
    isPool[7][11] = true;
    isPool[7][12] = true;
    isPool[12][10] = true;
    isPool[12][11] = true;
    isPool[13][10] = true;
    isPool[13][11] = true;
    isPool[14][10] = true;
    isPool[14][11] = true;
    isPool[0][13] = true;
    isPool[0][14] = true;
    isPool[1][13] = true;
    isPool[1][14] = true;
    isPool[2][13] = true;
    isPool[2][14] = true;
    }

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
    playerOhImagesRight[0].load(":/images/resources/images/player_oh_right.png");
    playerOhImagesLeft[0].load(":/images/resources/images/player_oh_left.png");
    endLoseImage.load(":/images/resources/images/end_lose.png");


    // 加载玩家图片
    for (int i = 0; i < 8; ++i)
    {
        QString rightPath = QString(":/images/resources/images/player_right_%1.png").arg(i + 1);
        playerImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/player_left_%1.png").arg(i + 1);
        playerImagesLeft[i].load(leftPath);
    }

    // 加载敌人图片
    for (int i = 0; i < 8; ++i)
    {
        QString rightPath = QString(":/images/resources/images/enemy_right_%1.png").arg(i + 1);
        enemyImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/enemy_left_%1.png").arg(i + 1);
        enemyImagesLeft[i].load(leftPath);
    }

    // 加载技能图片
    for (int i = 0; i < 2; ++i)
    {
        QString rightPath = QString(":/images/resources/images/player_cry_right_%1.png").arg(i + 1);
        playerSkillImagesRight[i].load(rightPath);
        QString leftPath = QString(":/images/resources/images/player_cry_left_%1.png").arg(i + 1);
        playerSkillImagesLeft[i].load(leftPath);
    }

    // 游戏开始时生成5个敌人
    spawnEnemy(5);

    //声音初始化
    {
    // 初始化子弹声音播放器
    audioOutput = new QAudioOutput(this);
    bulletHitSoundPlayer = new QMediaPlayer(this);
    bulletHitSoundPlayer->setAudioOutput(audioOutput);
    bulletHitSoundPlayer->setSource(QUrl("qrc:/images/resources/images/enemy_wow.mp3"));
    audioOutput->setVolume(50);

    // 初始化胜利声音播放器
    winAudioOutput = new QAudioOutput(this);
    winSoundPlayer = new QMediaPlayer(this);
    winSoundPlayer->setAudioOutput(winAudioOutput);
    winSoundPlayer->setSource(QUrl("qrc:/images/resources/images/player_laugh.mp3"));
    winAudioOutput->setVolume(50);

    // 初始化技能声音播放器
    skillAudioOutput = new QAudioOutput(this);
    skillSoundPlayer = new QMediaPlayer(this);
    skillSoundPlayer->setAudioOutput(skillAudioOutput);
    skillSoundPlayer->setSource(QUrl("qrc:/images/resources/images/player_cry.mp3"));
    skillAudioOutput->setVolume(100); // 调整音量（可选）

    // 初始化游戏音乐播放器
    gameAudioOutput = new QAudioOutput(this);
    gameMusicPlayer = new QMediaPlayer(this);
    gameMusicPlayer->setAudioOutput(gameAudioOutput);
    gameMusicPlayer->setSource(QUrl("qrc:/images/resources/images/game.mp3"));
    gameAudioOutput->setVolume(20);

    // 初始化玩家受击声音播放器
    playerHurtAudioOutput = new QAudioOutput(this);
    playerHurtSoundPlayer = new QMediaPlayer(this);
    playerHurtSoundPlayer->setAudioOutput(playerHurtAudioOutput);
    playerHurtSoundPlayer->setSource(QUrl("qrc:/images/resources/images/player_oh.mp3"));
    playerHurtAudioOutput->setVolume(50);

    // 初始化失败声音播放器
    loseAudioOutput = new QAudioOutput(this);
    loseSoundPlayer = new QMediaPlayer(this);
    loseSoundPlayer->setAudioOutput(loseAudioOutput);
    loseSoundPlayer->setSource(QUrl("qrc:/images/resources/images/lose.mp3"));
    loseAudioOutput->setVolume(50);
    }

    // 初始化敌人攻击时间记录
    enemyLastAttackTime.resize(0);

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
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();

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
    if (gameLost)
    {
        painter.drawPixmap(0, 0, width(), height(), endLoseImage);
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
    if (playerIsHurt && (currentTime - playerHurtStartTime) < playerHurtDuration)
    {
        if (dx > 0) currentPlayerImage = playerOhImagesRight[0];
        else if (dx < 0)  currentPlayerImage = playerOhImagesLeft[0];
        else currentPlayerImage = playerOhImagesRight[0];
    }
    else if (isSkillActive)
    {
        int frameIndex = (currentTime - skillStartTime) / 1000 % 2; // 每 1 秒切换一次图片
        if (dx > 0) currentPlayerImage = playerSkillImagesRight[frameIndex];
        else if (dx < 0)  currentPlayerImage = playerSkillImagesLeft[frameIndex];
        else currentPlayerImage = playerSkillImagesRight[0];
    }
    else
    {
        if (dx > 0) currentPlayerImage = playerImagesRight[stepCount % 8];
        else if (dx < 0)  currentPlayerImage = playerImagesLeft[stepCount % 8];
        else currentPlayerImage = playerImagesRight[0];
    }
    painter.drawPixmap(width() / 2 - currentPlayerImage.width() / 2, height() / 2 - currentPlayerImage.height() / 2, currentPlayerImage);

    // 绘制血条
    int healthBarWidth = 60;
    int healthBarHeight = 5;
    int healthBarX = width() / 2 - healthBarWidth / 2;
    int healthBarY = height() / 2 - currentPlayerImage.height() / 2 - 20 - healthBarHeight - 5;
    painter.fillRect(healthBarX, healthBarY, healthBarWidth * (playerHealth / static_cast<double>(playerMaxHealth)), healthBarHeight, Qt::red);
    painter.setPen(Qt::black);
    painter.drawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight);

    // 绘制蓝条
    int manaBarX = width() / 2 - manaBarWidth / 2;
    int manaBarY = height() / 2 - currentPlayerImage.height() / 2 - 20;
    painter.fillRect(manaBarX, manaBarY, currentMana, 5, Qt::blue);
    painter.setPen(Qt::black);
    painter.drawRect(manaBarX, manaBarY, manaBarWidth, 5);

    // 绘制敌人
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

    int currentTime = QTime::currentTime().msecsSinceStartOfDay();

    // 左键普攻
    if (event->button() == Qt::LeftButton)
    {
        if (currentTime - lastAttackTime >= attackCooldown)
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
                bullet.startTime = currentTime;
                bullets.push_back(bullet);

                lastAttackTime = currentTime;
            }
        }
    }

    // 右键技能
    else if(event->button()==Qt::RightButton)
    {
        if (currentTime - lastSkillUseTime >= skillCooldown)
        {
            activateSkill();
            lastSkillUseTime = currentTime;
            currentMana = 0; // 使用技能时蓝条清零
        }
    }
}

bool GameWindow::isColliding(const QPointF& newPosition, bool checkPool, bool isbullet)
{
    int gridX = static_cast<int>(newPosition.x() / gridSize);
    int gridY = static_cast<int>(newPosition.y() / gridSize);

    if (gridX >= 0 && gridX < mapWidthInGrids && gridY >= 0 && gridY < mapHeightInGrids)
    {
        if (isbullet)
        {
            // 子弹只检测 block
            return isBlock[gridX][gridY];
        }
        else
        {
            // 玩家和敌人检测 block 和 pool
            return isBlock[gridX][gridY] || isPool[gridX][gridY];
        }
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
    // 若敌人血量为0，不进行碰撞检测
    if (enemy.health <= 0)  return false;

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
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();

    if (gameState == GameState::StartPage)  return;

    if (gameWon)
    {
        stopGameMusic();
        int currentTime = QTime::currentTime().msecsSinceStartOfDay();
        if (currentTime - winStartTime >= 4000)
        {
            emit gameEnded(); // 发出游戏结束信号
        }
        if (currentTime - winStartTime >= 0) winSoundPlayer->play();
        return;
    }
    if (gameLost)
    {
        if (gameState == GameState::StartPage)  return; // 当处于 StartPage 时，直接返回
        stopGameMusic();
        if (currentTime - loseStartTime >= 4000)
        {
            emit gameEnded(); // 发出游戏结束信号
        }
        if (currentTime - loseStartTime >= 0 &&currentTime - lastLoseSoundPlayTime >= 7000) loseSoundPlayer->play();
        lastLoseSoundPlayTime = currentTime;
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

        // 移动前记录当前格子
        QPoint currentGrid(
            static_cast<int>(playerPosition.x() / gridSize),
            static_cast<int>(playerPosition.y() / gridSize)
            );

        if (distance > minMoveThreshold || targetDistance > 1.0)
        {
            double directionX = dx / distance;
            double directionY = dy / distance;
            QPointF newPosition = playerPosition + QPointF(directionX * playerSpeed, directionY * playerSpeed);

            if (!isColliding(newPosition,true))
            {
                playerPosition = newPosition;
                lastTargetPosition = targetPosition;
                stepCount++;

                // 技能激活时处理格子变化
                if (isSkillActive)
                {
                    // 当进入新格子时处理上一个格子
                    if (currentGrid != lastPlayerGrid && lastPlayerGrid != QPoint(-1,-1))
                    {
                        // 保存原始状态
                        if (!originalGridStates.contains(lastPlayerGrid)) {
                            originalGridStates[lastPlayerGrid] = isGrass[lastPlayerGrid.x()][lastPlayerGrid.y()];
                        }

                        // 标记为水池
                        isPool[lastPlayerGrid.x()][lastPlayerGrid.y()] = true;
                        isGrass[lastPlayerGrid.x()][lastPlayerGrid.y()] = false; // 清除草丛标记
                        activePoolGrids[lastPlayerGrid] = currentTime + 5000;

                        // 记录修改的格子和恢复时间
                        modifiedGrids.append(qMakePair(lastPlayerGrid, currentTime + 5000));
                    }
                    lastPlayerGrid = currentGrid; // 更新最后记录格子
                }
            }
        }
        else playerPosition = targetPosition;

        ensurePlayerInBounds();

        //about enemy
        moveEnemies();
        for (size_t i = 0; i < enemies.size(); ++i)
        {
            auto& enemy = enemies[i];
            double dx = playerPosition.x() - enemy.position.x();
            double dy = playerPosition.y() - enemy.position.y();
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= 50)
            { // 假设碰撞半径为50
                if (currentTime - enemyLastAttackTime[i] >= enemyAttackCooldown)
                {
                    // 敌人攻击玩家
                    playerHealth--;
                    playerIsHurt = true;
                    playerHurtStartTime = currentTime;
                    if (playerHurtSoundPlayer->playbackState() == QMediaPlayer::PlayingState)
                    {
                        playerHurtSoundPlayer->stop();
                    }
                    playerHurtSoundPlayer->setPosition(0); // 确保音频从开头播放
                    playerHurtSoundPlayer->play();
                    enemyLastAttackTime[i] = currentTime;

                    if (playerHealth <= 0)
                    {
                        // 玩家死亡，处理游戏结束逻辑
                        gameWon = false;
                        winStartTime = currentTime;
                        update(); // 强制更新窗口
                    }
                }
            }
        }


        // 检查技能状态
        if (isSkillActive)
        {
            int currentTime = QTime::currentTime().msecsSinceStartOfDay();
            if (currentTime - skillStartTime >= skillDuration) {
                deactivateSkill();
            }
        }

        // 检查玩家受击状态
        if (playerIsHurt && (currentTime - playerHurtStartTime) >= playerHurtDuration)
        {
            playerIsHurt = false;
        }

        // 检查玩家血量是否为0
        if (playerHealth <= 0)
        {
            gameLost = true;
            loseStartTime = currentTime;
            update(); // 强制更新窗口以显示失败图片
            return;
        }

        // 蓝条恢复逻辑
        if (currentMana < manaBarWidth)
        {
            int elapsedTime = currentTime - lastSkillUseTime;
            currentMana = std::min(manaBarWidth, elapsedTime * manaBarWidth / skillCooldown);
        }

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
            if (!isColliding(newPosition,true,true) && (currentTime - bullet.startTime) < bulletLifetime && !bulletHit)
            {
                Bullet newBullet = bullet;
                newBullet.position = newPosition;
                newBullets.push_back(newBullet);
            }
        }
        bullets = newBullets;

        // 恢复过期格子
        for(int i = modifiedGrids.size() - 1; i >= 0; --i)
        {
            if(currentTime > modifiedGrids[i].second)
            {
                QPoint pos = modifiedGrids[i].first;
                isPool[pos.x()][pos.y()] = false; // 恢复池状态
                isGrass[pos.x()][pos.y()] = originalGridStates.value(pos, false); // 使用正确的记录恢复草丛状态
                modifiedGrids.removeAt(i);
                originalGridStates.remove(pos); // 移除记录的原始状态
            }
        }

        update();
    }
    else if (event->timerId() == enemyTimerId) spawnEnemy(1); // 每2秒生成1个敌人
}

void GameWindow::ensurePlayerInBounds()
{
    int halfWidth = playerImagesRight[0].width() / 2;
    int halfHeight = playerImagesRight[0].height() / 2;
    int mapWidth = mapWidthInGrids * gridSize;
    int mapHeight = mapHeightInGrids * gridSize;

    if (playerPosition.x() - halfWidth < 0) playerPosition.setX(halfWidth);
    else if (playerPosition.x() + halfWidth > mapWidth)  playerPosition.setX(mapWidth - halfWidth);

    if (playerPosition.y() - halfHeight < 0) playerPosition.setY(halfHeight);
    else if (playerPosition.y() + halfHeight > mapHeight)  playerPosition.setY(mapHeight - halfHeight);

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
            if (!isColliding(enemy.position,true) && !isInGrass(enemy.position))
            {
                validPosition = true;
            }
        }
        enemy.targetPosition = playerPosition;
        enemy.stepCount = 0;
        enemies.push_back(enemy);
        enemyLastDirectionChangeTime.push_back(QTime::currentTime().msecsSinceStartOfDay()); // 记录敌人创建时的时间
        enemyLastAttackTime.push_back(0); // 初始化敌人上一次攻击时间
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

                if (!isColliding(newPosition,true))
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

                if (!isColliding(newPosition,true))
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

    //color
    QColor blockcolor(253,176,94,255);
    QColor poolcolor(24,163,244,255);
    QColor grasscolor(50,175,52,255);
    QColor gridcolor(238,227,208,200);

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

            if (isBlock[i][j]) painter.fillRect(x, y, w, h, blockcolor);
            else if (isGrass[i][j])  painter.fillRect(x, y, w, h, grasscolor);
            else if (isPool[i][j]) painter.fillRect(x, y, w, h, poolcolor);
            else painter.fillRect(x, y, w, h, gridcolor);
        }
    }

    // 绘制玩家在小地图上的位置
    int playerMiniX = static_cast<int>(playerPosition.x() * scale + borderWidth);
    int playerMiniY = static_cast<int>(playerPosition.y() * scale + borderWidth);
    painter.setBrush(Qt::darkYellow);
    painter.setPen(Qt::NoPen); // 设置无描边
    // 绘制黄色方格代表玩家
    painter.drawRect(playerMiniX - 2, playerMiniY - 2, 4, 4);

    // 绘制等效大地图 300 像素的圆圈
    int circleRadius = static_cast<int>(300 * scale); // 计算等效的小地图半径
    painter.setPen(QPen(Qt::gray, 1)); // 设置画笔
    painter.setBrush(Qt::NoBrush); // 不填充
    painter.drawEllipse(playerMiniX - circleRadius, playerMiniY - circleRadius, 2 * circleRadius, 2 * circleRadius);

    // 绘制不在草丛里且距离玩家 300 像素以内的敌人
    const double enemyVisibilityRange = 300;
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
    //gameState = GameState::GamePage; // 新增：强制设置状态
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

    // 重置技能相关状态
    modifiedGrids.clear();
    originalGrassState.clear();
    isRecordingPath = false;

    // 重置玩家血量和受击状态
    playerHealth = playerMaxHealth;
    playerIsHurt = false;
    playerHurtStartTime = 0;

    // 重置游戏失败状态
    gameLost = false;
    loseStartTime = 0;
    loseSoundPlayer->stop();

    // 重置格子状态
    activePoolGrids.clear();
    originalGridStates.clear();
    lastPlayerGrid = QPoint(-1, -1);

    //timerId = startTimer(16);
    //enemyTimerId = startTimer(2000);
}

void GameWindow::activateSkill()
{
    if (!isSkillActive)
    {
        isSkillActive = true;
        isRecordingPath = true;  // 开始记录路径
        skillStartTime = QTime::currentTime().msecsSinceStartOfDay();
        skillSoundPlayer->play(); // 播放技能声音

        // 不清空之前的记录
        //modifiedGrids.clear();
        //originalGrassState.clear();

        update(); // 强制更新窗口以显示技能图片
    }
}

void GameWindow::deactivateSkill()
{
    isSkillActive = false;
    isRecordingPath = false; // 停止记录路径
    skillSoundPlayer->stop(); // 停止技能声音
    lastPlayerGrid = QPoint(-1, -1); // 重置 lastPlayerGrid
    update(); // 强制更新窗口以恢复正常图片
}

void GameWindow::startGameMusic()
{
    gameMusicPlayer->play();
}
void GameWindow::stopGameMusic()
{
    gameMusicPlayer->stop();
}
void GameWindow::stopLoseMusic()
{
    loseSoundPlayer->stop();
}
void GameWindow::stopTimers()
{
    killTimer(timerId);
    killTimer(enemyTimerId);
}

void GameWindow::startTimers()
{
    timerId = startTimer(16);
    enemyTimerId = startTimer(2000);
    gameState = GameState::GamePage; // 设置游戏状态为进行中
}

