#include "startpage.h"

StartPage::StartPage(QWidget *parent) : QWidget(parent)
{
    // 加载背景图片
    backgroundImage.load(":/images/resources/images/start.png");

    // 创建开始按钮
    startButton = new QPushButton("开始游戏", this);
    startButton->setGeometry(300, 400, 200, 60);
    connect(startButton, &QPushButton::clicked, this, &StartPage::startGame);
}

void StartPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), backgroundImage);
}
