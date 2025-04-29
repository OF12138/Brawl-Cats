#include "startpage.h"

StartPage::StartPage(QWidget *parent) : QWidget(parent)
{
    // 加载背景图片
    backgroundImage.load(":/images/resources/images/start.png");

    // 创建开始按钮
    startButton = new QPushButton("开始游戏", this);
    // 设置按钮的大小
    int buttonSize = 100; // 按钮的直径
    startButton->setGeometry(350, 407, buttonSize, buttonSize);

    // 设置按钮样式表
    startButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 200); /* 橙色背景 */"
        "    border-radius: 50px; /* 圆角半径为直径的一半，实现圆形 */"
        "    color: black; /* 白色文字 */"
        "    font-family: 黑体; /* 黑体字体 */"
        "    font-size: 16px; /* 字体大小 */"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(245, 245, 245, 128); /* 鼠标悬停时的背景颜色 */"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(255,245,238, 128); /* 鼠标按下时的背景颜色 */"
        "}"
        );

    connect(startButton, &QPushButton::clicked, this, &StartPage::startGame);

    // 初始化音乐播放器
    startAudioOutput = new QAudioOutput(this);
    startMusicPlayer = new QMediaPlayer(this);
    startMusicPlayer->setAudioOutput(startAudioOutput);
    startMusicPlayer->setSource(QUrl("qrc:/images/resources/images/start.mp3"));
    startAudioOutput->setVolume(50);
}

void StartPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), backgroundImage);

    // 新增：绘制教程提示框
    int margin = 20; // 框与窗口右上角的距离
    int boxWidth = 200;
    int boxHeight = 250;
    int borderRadius = 10; // 圆角半径
    int boxX = width() - boxWidth - margin;
    int boxY = margin;

    // 设置透明度
    QColor boxColor(255, 255, 255, 204); // 透明度 80% (255 * 0.8 = 204)
    painter.setBrush(boxColor);
    painter.setPen(Qt::black);

    // 绘制圆角矩形
    painter.drawRoundedRect(boxX, boxY, boxWidth, boxHeight, borderRadius, borderRadius);

    // 设置字体为黑体
    QFont font("黑体", 14);
    painter.setFont(font);

    // 居中显示教程文本
    QString tutorialText = "新手教程\n\n1.香蕉猫会追着鼠标跑\n\n2.苹果猫会追杀香蕉猫\n\n3.按鼠标左键发射子弹\n\n4.按鼠标右键释放技能\n\n5.进草丛可以躲苹果猫";
    QRect tutorialRect(boxX, boxY, boxWidth, boxHeight);
    painter.drawText(tutorialRect, Qt::AlignCenter, tutorialText);
}

void StartPage::startMusic()
{
    startMusicPlayer->play();
}

void StartPage::stopMusic()
{
    startMusicPlayer->stop();
}
