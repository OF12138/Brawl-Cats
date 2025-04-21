#include <QApplication>
#include "gamewindow.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GameWindow game;
    game.show();
    return app.exec();
}
