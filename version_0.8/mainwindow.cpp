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
}

void MainWindow::onStartGame()
{
    stackedWidget->setCurrentWidget(gameWindow); // 切换到游戏窗口
    gameWindow->resetGame();
}

void MainWindow::onGameEnd()
{
    stackedWidget->setCurrentWidget(startPage); // 切换回开始页面
    gameWindow->resetGame(); // 重置游戏状态
}
