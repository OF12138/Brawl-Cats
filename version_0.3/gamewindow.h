#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <vector>

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

private:
    QPixmap gridImage;
    QPixmap grassImage;
    QPixmap grassTransparentImage;
    std::vector<std::vector<bool>> isGrass;
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
};

#endif // GAMEWINDOW_H
