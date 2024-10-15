#ifndef SOCKETIO_H
#define SOCKETIO_H

#include <QObject>
#include <QtWebSockets/QWebSocket>

class SocketIo : public QObject
{
    Q_OBJECT
public:
    explicit SocketIo(const QUrl &url, QObject *parent = nullptr);
    void connectToServer();
signals:

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();

private:
    QWebSocket webSocket;
    QUrl serverUrl;
};

#endif // SOCKETIO_H
