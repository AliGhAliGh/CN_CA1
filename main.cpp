#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include "socket.io-client-cpp/src/sio_client.h"
#include "webrtc.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    qmlRegisterType<WebRTC>("WebRtc", 1, 0, "WebRtc");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("CA1", "Main");

    sio::client client;
    client.connect("http://127.0.0.1:3030");
    client.socket()->on("connect", [](sio::event event) {
        qDebug() << "connect: " << event.get_message()->get_string();
    });
    client.socket()->on("disconnect", [](sio::event event) {
        qDebug() << "disconnect: " << event.get_message()->get_string();
    });
    client.socket()->on("connect_error", [](sio::event event) {
        qDebug() << "connect_error" << event.get_message()->get_string();
    });
    client.socket()->on("message", [](sio::event event) {
        qDebug() << "message" << event.get_message()->get_string();
    });
    client.socket()->on("uniquenessError", [](sio::event event) {
        qDebug() << "uniquenessError" << event.get_message()->get_string();
    });
    // QJsonObject res;
    // res["from"] = "a";
    // res["target"] = "all";
    // QJsonDocument doc(res);
    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["from"] = sio::string_message::create("ali");
    msg->get_map()["target"] = sio::string_message::create("all");
    msg->get_map()["payload"] = sio::object_message::create();
    msg->get_map()["payload"]->get_map()["action"] = sio::string_message::create("offer");
    msg->get_map()["payload"]->get_map()["sdp"] = sio::string_message::create("sdp");
    client.socket()->emit("ready", msg);

    return app.exec();
}
