#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <QObject>
#include "socket.io-client-cpp/src/sio_client.h"

class WebRTC;

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(WebRTC *webRtc, QObject *parent = nullptr);

    void connectToSignalingServer(std::string peerId);

    void sendOffer(const std::string &sdp_offer, const std::string &peerId);

    void sendAnswer(const std::string &sdp_answer);

    void sendIceCandidate(const std::string &ice_candidate);

private:
    sio::client socket_client;

Q_SIGNALS:
    void onRemoteDescription(const QString &peerID, const QString &sdp);
};

#endif // SIGNALMANAGER_H
