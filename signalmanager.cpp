#include "signalmanager.h"
#include "webrtc.h"

SignalManager::SignalManager(WebRTC *webRtc, QObject *parent)
    : QObject{parent}
{
    connect(webRtc, &WebRTC::offerIsReady, this, [this](const QString &peerID, const QString &sdp) {
        qDebug() << "Local description generated for peer:" << peerID;
        sendOffer(sdp.toStdString(), peerID.toStdString());
    });
    connect(webRtc, &WebRTC::answerIsReady, this, [this](const QString &peerID, const QString &sdp) {
        qDebug() << "Local description generated for peer:" << peerID;
        sendAnswer(sdp.toStdString(), peerID.toStdString());
    });
    connect(webRtc, &WebRTC::callRejected, this, [this](const QString &peerID) {
        qDebug() << "Local description generated for peer:" << peerID;
        sendReject(peerID.toStdString());
    });
    connect(this,
            &SignalManager::onOfferReceived,
            webRtc,
            &WebRTC::offerReceived,
            Qt::AutoConnection);
    connect(this,
            &SignalManager::onAnswerReceived,
            webRtc,
            &WebRTC::answerReceived,
            Qt::AutoConnection);
    connect(this,
            &SignalManager::onRejectReceived,
            webRtc,
            &WebRTC::rejectReceived,
            Qt::AutoConnection);
}

void SignalManager::connectToSignalingServer(std::string peerId)
{
    socket_client.set_logs_quiet();
    socket_client.connect("http://localhost:3000");
    socket_client.socket()->on("offer", [&](sio::event &ev) {
        std::string sdpOffer = ev.get_message()->get_map()["sdpOffer"]->get_string();
        std::string from = ev.get_message()->get_map()["from"]->get_string();
        Q_EMIT onOfferReceived(QString::fromStdString(from), QString::fromStdString(sdpOffer));
    });

    socket_client.socket()->on("answer", [&](sio::event &ev) {
        std::string sdpAnswer = ev.get_message()->get_map()["sdpAnswer"]->get_string();
        std::string from = ev.get_message()->get_map()["from"]->get_string();
        Q_EMIT onAnswerReceived(QString::fromStdString(from), QString::fromStdString(sdpAnswer));
    });

    socket_client.socket()->on("reject", [&](sio::event &ev) {
        std::string from = ev.get_message()->get_map()["from"]->get_string();
        Q_EMIT onRejectReceived(QString::fromStdString(from));
    });

    socket_client.socket()->on("ice-candidate", [&](sio::event &ev) {
        // Add the ICE candidate to the peer connection
    });

    socket_client.socket()->emit("register", peerId);
}

void SignalManager::sendOffer(const std::string &sdp_offer, const std::string &targetPeerId)
{
    sio::message::list msg;
    msg.push(sio::string_message::create(targetPeerId));
    msg.push(sio::string_message::create(sdp_offer));
    socket_client.socket()->emit("offer", msg);
}

void SignalManager::sendAnswer(const std::string &sdp_answer, const std::string &targetPeerId)
{
    sio::message::list msg;
    msg.push(sio::string_message::create(targetPeerId));
    msg.push(sio::string_message::create(sdp_answer));
    socket_client.socket()->emit("answer", msg);
}

void SignalManager::sendReject(const std::string &peerId)
{
    socket_client.socket()->emit("reject", peerId);
}

void SignalManager::sendIceCandidate(const std::string &ice_candidate)
{
    socket_client.socket()->emit("ice-candidate", ice_candidate);
}
