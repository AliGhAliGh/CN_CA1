#include "signalmanager.h"
#include "webrtc.h"

SignalManager::SignalManager(WebRTC *webRtc, QObject *parent)
    : QObject{parent}
{
    connect(webRtc, &WebRTC::offerIsReady, this, [this](const QString &peerID, const QString &sdp) {
        qDebug() << "Local description generated for peer:" << peerID;
        qDebug() << "SDP:" << sdp;
        sendOffer(sdp.toStdString(), peerID.toStdString());
    });
    connect(this,
            &SignalManager::onRemoteDescription,
            webRtc,
            &WebRTC::setRemoteDescription,
            Qt::AutoConnection);
}

void SignalManager::connectToSignalingServer(std::string peerId)
{
    socket_client.set_logs_quiet();
    socket_client.connect("http://localhost:3000");

    socket_client.socket()->on("offer", [&](sio::event &ev) {
        std::string sdpOffer = ev.get_message()->get_map()["sdpOffer"]->get_string();
        std::string from = ev.get_message()->get_map()["from"]->get_string();
        qDebug() << sdpOffer;
        qDebug() << from;
        Q_EMIT onRemoteDescription(QString::fromStdString(from), QString::fromStdString(sdpOffer));
    });

    socket_client.socket()->on("answer", [&](sio::event &ev) {

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

void SignalManager::sendAnswer(const std::string &sdp_answer)
{
    socket_client.socket()->emit("answer", sdp_answer);
}

void SignalManager::sendIceCandidate(const std::string &ice_candidate)
{
    socket_client.socket()->emit("ice-candidate", ice_candidate);
}
