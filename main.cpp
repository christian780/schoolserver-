#include <QCoreApplication>

#include "myserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    MyServer server;
    if (!server.listen(QHostAddress::LocalHost, 1234)) {
        qCritical() << "Could not start server";
        return 1;
    }

    qDebug() << "Server started on port" << server.serverPort();
    return a.exec();
}

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    return a.exec();
}
*/
