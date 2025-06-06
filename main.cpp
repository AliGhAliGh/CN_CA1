#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "webrtc.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterType<WebRTC>("WebRtc", 1, 0, "WebRtc");
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load("../../qml/Main.qml");
    return app.exec();
}
