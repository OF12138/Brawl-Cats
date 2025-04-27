#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>

class StartPage : public QWidget
{
    Q_OBJECT
public:
    explicit StartPage(QWidget *parent = nullptr);

signals:
    void startGame();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *startButton;
    QPixmap backgroundImage;
};

#endif // STARTPAGE_H
