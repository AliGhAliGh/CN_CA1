#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
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

    return app.exec();
}
