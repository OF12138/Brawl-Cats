#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    startPage = new StartPage();
    gameWindow = new GameWindow();

    stackedWidget->addWidget(startPage);
    stackedWidget->addWidget(gameWindow);
    setCentralWidget(stackedWidget); // 设置堆栈部件为主窗口中心

    connect(startPage, &StartPage::startGame, this, &MainWindow::onStartGame);
    connect(gameWindow, &GameWindow::gameEnded, this, &MainWindow::onGameEnd);

    stackedWidget->setCurrentWidget(startPage); // 初始显示开始页面
    startPage->startMusic();//播放音乐
}

void MainWindow::onStartGame()
{
    gameWindow->stopLoseMusic();
    startPage->stopMusic();
    stackedWidget->setCurrentWidget(gameWindow); // 切换到游戏窗口
    gameWindow->resetGame();

    // 新增：手动启动游戏相关定时器
    gameWindow->startTimers();
    gameWindow->startGameMusic();
}

void MainWindow::onGameEnd()
{
    gameWindow->stopGameMusic();
    gameWindow->stopLoseMusic();
    gameWindow->stopTimers(); // 新增：停止所有定时器
    stackedWidget->setCurrentWidget(startPage); // 切换回开始页面
    gameWindow->resetGame(); // 重置游戏状态
    startPage->startMusic();
}
