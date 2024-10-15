#include "socketio.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

SocketIo::SocketIo(const QUrl &url, QObject *parent)
    : QObject(parent)
    , serverUrl(url)
{
    connect(&webSocket, &QWebSocket::connected, this, &SocketIo::onConnected);
    connect(&webSocket, &QWebSocket::disconnected, this, &SocketIo::onDisconnected);
    connect(&webSocket, &QWebSocket::textMessageReceived, this, &SocketIo::onTextMessageReceived);
}

void SocketIo::connectToServer()
{
    qDebug() << "Connecting to" << serverUrl;
    webSocket.open(serverUrl);
}

void SocketIo::onConnected()
{
    qDebug() << "Connected to server";
    // Optionally emit a Socket.IO handshake message here
}

void SocketIo::onTextMessageReceived(const QString &message)
{
    qDebug() << "Message received:" << message;
    // Parse the message as JSON if necessary
}

void SocketIo::onDisconnected()
{
    qDebug() << "Disconnected from server";
}
