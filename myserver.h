#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QSqlDatabase>

class MyServer : public QTcpServer {
    Q_OBJECT

public:
    MyServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void newClientConnected(const QString &clientInfo);

private slots:
    QByteArray onReadyRead();
    void onDisconnected();
    void onNewClientConnected(const QString &clientInfo);
    QByteArray handleRegisterStudent(const QByteArray &request);
//    QMap<QString, QString>  parsePlainTextRequest(const QByteArray &requestData);


private:
    QSqlDatabase db;
    QHash<QTcpSocket*, QString> clients; // Stores authenticated clients
    void processCommand(QTcpSocket* client, const QString& command);
    bool authenticate(const QString& username, const QString& password);
};

#endif // MYSERVER_H


/*
#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>

class myserver
{
public:
    myserver();
};

#endif // MYSERVER_H
*/
