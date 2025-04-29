#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QtMultimedia/QMediaPlayer>
#include <QAudioOutput>

class StartPage : public QWidget
{
    Q_OBJECT
public:
    explicit StartPage(QWidget *parent = nullptr);
    void startMusic();
    void stopMusic();

signals:
    void startGame();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *startButton;
    QPixmap backgroundImage;
    QMediaPlayer* startMusicPlayer;
    QAudioOutput* startAudioOutput;

};

#endif // STARTPAGE_H
