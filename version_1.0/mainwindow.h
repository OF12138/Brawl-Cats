#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "startpage.h"
#include "gamewindow.h"
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onStartGame();
    void onGameEnd();

private:
    QStackedWidget* stackedWidget;
    StartPage *startPage;
    GameWindow *gameWindow;
};

#endif // MAINWINDOW_H
