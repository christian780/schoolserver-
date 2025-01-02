//#include "myserver.h"

#include "myserver.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MyServer::MyServer(QObject *parent)
    : QTcpServer(parent) {
    // Initialize and open the database connection
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/proj/smp/admin2.db");

    if (!db.open()) {
        qCritical() << "Failed to connect to the database:" << db.lastError().text();
    }else{

        qCritical()<< "Databse connected";
    }
}

void MyServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        clients[socket] = "";
        connect(socket, &QTcpSocket::readyRead, this, &MyServer::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &MyServer::onDisconnected);

        QString clientInfo = socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
        onNewClientConnected(clientInfo);
    } else {
        delete socket;
    }
}

QByteArray MyServer::onReadyRead(){
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());

    QByteArray requestData = client->readAll();
    QByteArray response;
    qDebug()<< "request received " << requestData;
    QStringList parts = QString(requestData).split(":");
    if (parts.size() < 3) {
        qDebug()<< " format wrong:";
        return "Error: Invalid request format.";
    }

    QString command = parts[0];
    QString username = parts[1];
    QString password = parts[2];

    if (command == "AUTHENTICATE") {
        QSqlQuery query(db);
        query.prepare("SELECT user, password, salt from admin WHERE user= :user AND password = :password");
        query.bindValue(":user", username);
        query.bindValue(":password", password);
        query.bindValue(":salt","salt");

        if (!query.exec()) {
            qCritical() << "Database query error:" << query.lastError().text();
            return "Error: Database query failed.";
        }

        if (query.next() && query.value(0).toString() == username) {
            qDebug()<<"database query successful:";
            client->write("Authentication successful");
            client->flush();
            return "Authentication successful";
        } else {
            return "Authentication failed";
        }
    } else if (command == "REGISTER") {
        // Handle registration logic
        response = handleRegisterStudent(requestData);
        if(response == "Student registered."){
            client->write(response);
            client->flush();
            return "Student registered.";
        }
            qDebug()<<"Register logic:";

    }else{
        return "Not Registered.";
    }

    return "Error: Unknown command.";

}

void MyServer::onDisconnected() {
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    clients.remove(client);
    client->deleteLater();
}

void MyServer::processCommand(QTcpSocket *client, const QString &command) {
    // Process client commands after authentication
    if (command == "PING") {
        client->write("PONG\n");
    } else {
        client->write("Unknown command\n");
    }
}

bool MyServer::authenticate(const QString &username, const QString &password) {
    // Query the database to authenticate the user
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    qDebug() << "Authentication failed:" << query.lastError().text();
    return false;
}

void MyServer::onNewClientConnected(const QString &clientInfo) {
    qDebug() << "New client connected from" << clientInfo;
}

/*
    if (client->canReadLine()) {
        QString line = client->readLine().trimmed();
        if (clients[client].isEmpty()) {
            QStringList parts = line.split(" ");
            if (parts.size() == 2) {
                QString username = parts[0];
                QString password = parts[1];
                // Authenticate against the database
                if (authenticate(username, password)) {
                    clients[client] = username;
                    client->write("Authentication successful\n");
                } else {
                    client->write("Authentication failed\n");
                    client->disconnectFromHost();
                }
            } else {
                client->write("Invalid command\n");
                client->disconnectFromHost();
            }
        } else {
            processCommand(client, line);
        }
    }
*/

QByteArray MyServer::handleRegisterStudent(const QByteArray &request)
{



        // Parse the requestData
    QStringList fields = QString(request).split(":");

    if (fields.size() < 8) {
        qCritical() << "Invalid REGISTER request format.";
        return "Error: Invalid REGISTER request format.";
    }

    QString firstname = fields[1];
    QString lastname = fields[2];
    QString address = fields[3];
    QString email = fields[4];
    QString mobile = fields[5];
    QString middlename = fields[6];
    QString gender = fields[7];
    QString classvalue = fields[8];



   // Default to "Other" if not provided
//    QByteArray imageBase64 = requestMap.value("image").toUtf8();



    // Validate data
    if (firstname.isEmpty() || lastname.isEmpty() ||
        email.isEmpty() || mobile.isEmpty()
        || gender.isEmpty() || classvalue.isEmpty()) {
        qDebug()<< "Invalid student data received.";
        //       clientSocket->write("ERROR: Missing required fields.");
        //       clientSocket->disconnectFromHost();
        return "ERROR: Missing required fields.";
    }
    qDebug()<< "Registering student: " + firstname + " " + lastname;

//    QByteArray imageData = QByteArray::fromBase64(imageBase64);
/*
    QImage image;
    if (!image.loadFromData(imageData)) {
        qDebug()<< "Failed to decode image data." ;
        return "ERROR: Invalid image format.";
    }
*/

    // Insert student data into the database
    QSqlQuery query;
    query.prepare("INSERT INTO students (fisrtname, middlename, lastname, address, email, phone, password, gender, image) "
                  "VALUES (:fisrtname, :middlename, :lastname, :address, :email, :phone, :password, :gender, :image)");
    query.bindValue(":fisrtname", firstname);
    query.bindValue(":middlename", middlename);
    query.bindValue(":lastname", lastname);
    query.bindValue(":address", address);
    query.bindValue(":email", email);
    query.bindValue(":phone", mobile);
    query.bindValue(":password", "password");
    query.bindValue(":gender", gender);
    query.bindValue(":image", "imageData");

    if (!query.exec()) {
        qDebug()<< "Database error: " + query.lastError().text();
        return "Not Registered.";
        //        clientSocket->write("ERROR: Failed to save student data.");
    } else {
        qDebug()<<"Student data saved successfully.";
        return "Student registered.";
        //        clientSocket->write("SUCCESS: Student registered.");
    }

    //    clientSocket->disconnectFromHost();
}

/*
QMap<QString, QString> MyServer::parsePlainTextRequest(const QByteArray &requestData)
{
    QMap<QString, QString> requestMap;

    QString requestStr = QString(requestData);

    // Split the input into fields using ';' as a delimiter
    QStringList fields = QString(requestData).split(';', Qt::SkipEmptyParts);

    requestStr = requestStr.trimmed();

    for (const QString &field : fields) {
        // Split each field into key-value pairs using '='
        QStringList keyValue = field.split('=', Qt::SkipEmptyParts);
        if (keyValue.size() == 2) {
            requestMap[keyValue[0].trimmed()] = keyValue[1].trimmed();
        }else {
            qDebug() << "Invalid key-value pair:" << field;
        }
    }

    return requestMap;
}
*/
