#include <QApplication>

#include "MsgHandler.h"
#include "WindowMain.h"

int main(int argc, char *argv[]) {

    qInstallMessageHandler(MsgHandler);

    QApplication app(argc, argv);
    WindowMain w_Main;
    w_Main.show();
    return app.exec();

}
